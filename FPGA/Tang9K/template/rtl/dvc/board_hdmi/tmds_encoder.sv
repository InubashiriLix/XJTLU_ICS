module tmds_encoder #(
    parameter integer INPUT_WIDTH  = 8,
    parameter integer OUTPUT_WIDTH = 10
) (
    input pixel_clk,
    input rst,
    input [INPUT_WIDTH-1:0] din,
    input [1:0] ctrl,
    input de,
    output reg [OUTPUT_WIDTH-1:0] dout
);

    localparam logic [9:0] Ctrl00 = 10'b1101010100;
    localparam logic [9:0] Ctrl01 = 10'b0010101011;
    localparam logic [9:0] Ctrl10 = 10'b0101010100;
    localparam logic [9:0] Ctrl11 = 10'b1010101011;


    // encode the 8-bit input data into 10-bit TMDS data
    // ====== 1 ========
    function automatic [8:0] tmds_encode_1;
        input [7:0] din;
        reg [3:0] ones;
        reg xnor_mode;
        integer i;
        begin
            ones = $countones(din);
            xnor_mode = (ones > 4) || (ones == 4 && din[0] == 1'b0);
            tmds_encode_1[0] = din[0];
            tmds_encode_1[8] = ~xnor_mode;
            for (i = 1; i < 8; i = i + 1) begin
                tmds_encode_1[i] = xnor_mode ?
                tmds_encode_1[i-1] ~^ din[i] :
                tmds_encode_1[i-1] ^ din[i];
            end
        end
    endfunction

    // stage 2: dc balance
    reg signed [4:0] cnt;  // running disparity counter
    wire       [8:0] q_m;
    reg [3:0] q_m_ones, q_m_zeros;

    assign q_m = tmds_encode_1(din);

    always @(posedge pixel_clk or posedge rst) begin
        if (rst) begin
            cnt  <= 0;
            dout <= 10'b1101010100;  // ctrl=2'b00 default
        end else if (~de) begin
            // control period: no dc balancing, just pass ctrl code
            case (ctrl)
                2'b00:   dout <= Ctrl00;
                2'b01:   dout <= Ctrl01;
                2'b10:   dout <= Ctrl10;
                2'b11:   dout <= Ctrl11;
                // it never happens, but lint will warn if not handled
                default: dout <= Ctrl00;
            endcase
            cnt <= 0;
        end else begin
            // data period: apply dc balancing
            q_m_ones  = q_m[0] + q_m[1] + q_m[2] + q_m[3] + q_m[4] + q_m[5] + q_m[6] + q_m[7];
            q_m_zeros = 8 - q_m_ones;

            // cnt == 0 or q_m has equal ones and zeros: force inversion based on q_m[8]
            if ((cnt == 0) || (q_m_ones == 4)) begin
                dout[9]   <= ~q_m[8];
                dout[8]   <= q_m[8];
                dout[7:0] <= q_m[8] ? q_m[7:0] : ~q_m[7:0];

                if (q_m[8] == 1'b0) cnt <= cnt + (q_m_zeros - q_m_ones);
                else cnt <= cnt + (q_m_ones - q_m_zeros);
            end  // disparity direction matches data skew: invert
            else if ((cnt > 0 && q_m_ones > q_m_zeros) || (cnt < 0 && q_m_ones < q_m_zeros)) begin
                dout[9] <= 1'b1;
                dout[8] <= q_m[8];
                dout[7:0] <= ~q_m[7:0];
                cnt <= cnt + 2 * q_m[8] + (q_m_zeros - q_m_ones);
            end  // otherwise: don't invert
            else begin
                dout[9] <= 1'b0;
                dout[8] <= q_m[8];
                dout[7:0] <= q_m[7:0];
                cnt <= cnt - 2 * (~q_m[8]) + (q_m_ones - q_m_zeros);
            end
        end
    end
endmodule
