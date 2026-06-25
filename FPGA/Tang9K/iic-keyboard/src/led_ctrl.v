module led_ctrl #(
    parameter integer CLK_FREQ_HZ = 27_000_000,
    parameter integer BLINK_DURATION_MS = 500

) (
    input  wire clk,
    input  wire reg_written,
    output wire led,
    input wire rst_n
);


localparam integer BLINK_CNT_MAX = (CLK_FREQ_HZ / 1000) * BLINK_DURATION_MS;
reg [25:0] blink_cnt;

localparam [2:0] ST_IDLE = 2'd0;
localparam [2:0] ST_BLINK_ONCE = 2'd1;
reg [2:0] state;
reg [2:0] state_next;

always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        state <= ST_IDLE;
        state_next <= ST_IDLE;
    end

    state <= state_next;

    case (state)
        ST_IDLE: begin
            if (reg_written) begin
                state_next <= ST_BLINK_ONCE;
            end else begin
                state_next <= ST_IDLE;
            end
        end

        ST_BLINK_ONCE: begin
            if (blink_cnt == BLINK_CNT_MAX-1) begin
                state_next <= ST_IDLE;
            end else begin
                state_next <= ST_BLINK_ONCE;
            end
        end
    endcase
end


always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        blink_cnt <= 26'd0;
    end else if (blink_cnt == BLINK_CNT_MAX-1) begin
        blink_cnt <= 26'd0;
    end else begin
        blink_cnt <= blink_cnt + 26'd1;
    end
end

endmodule
