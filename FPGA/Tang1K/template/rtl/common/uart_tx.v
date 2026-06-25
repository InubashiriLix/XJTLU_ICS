module uart_tx #(
    parameter integer CLOCK_HZ = 27_000_000,
    parameter integer BAUD_RATE = 115_200,
    parameter integer BAUD_CNT_MAX = CLOCK_HZ / BAUD_RATE,
    parameter integer BAUD_CNT_WIDTH = 16
) (
    input wire clk,
    input wire rst_n,

    input wire [7:0] tx_data_i,
    input wire tx_valid_i,

    output wire tx_ready_o,
    output reg  tx_o
);

    localparam integer StIdle = 3'd0;
    localparam integer StStart = 3'd1;
    localparam integer StData = 3'd2;
    localparam integer StStop = 3'd3;

    reg [2:0] state;

    reg [BAUD_CNT_WIDTH-1:0] baud_cnt;
    reg [2:0] bit_idx;
    reg [7:0] tx_data_buf;


    assign tx_ready_o = (state == StIdle);

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            state <= StIdle;
            baud_cnt <= {BAUD_CNT_WIDTH{1'b0}};
            bit_idx <= 3'b0;
            tx_data_buf <= 8'b0;
            tx_o <= 1'b1;
        end else begin
            case (state)
                StIdle: begin
                    baud_cnt <= {BAUD_CNT_WIDTH{1'b0}};
                    tx_o <= 1'b1;
                    bit_idx <= 3'b0;

                    if (tx_valid_i) begin
                        state <= StStart;
                        tx_o <= 1'b0;
                        tx_data_buf <= tx_data_i;
                    end
                end

                StStart: begin
                    if (baud_cnt == BAUD_CNT_MAX - 1) begin
                        // update the state
                        state <= StData;
                        // reset the data index
                        bit_idx <= 3'b0;
                        // reset teh baud counter
                        baud_cnt <= {BAUD_CNT_WIDTH{1'b0}};
                        // start to send the first data bit
                        tx_o <= tx_data_buf[bit_idx];
                    end else begin
                        // set output as 0 until the ccondidtion evokes
                        baud_cnt <= baud_cnt + 1'b1;
                    end
                end

                StData: begin
                    if (baud_cnt == BAUD_CNT_MAX - 1) begin
                        // if baud count reaches the max, we need to update the counter
                        baud_cnt <= {BAUD_CNT_WIDTH{1'b0}};

                        if (bit_idx == 3'd7) begin
                            // if bit idx reaches the end
                            // send the end bit
                            tx_o  <= 1'b1;
                            state <= StStop;
                        end else begin
                            bit_idx <= bit_idx + 1'b1;
                            tx_o <= tx_data_buf[bit_idx+1'b1];
                        end
                    end else begin
                        baud_cnt <= baud_cnt + 1'b1;
                    end
                end

                StStop: begin
                    if (baud_cnt == BAUD_CNT_MAX - 1) begin
                        // if baud cnt reaches the max, then we nned to reset all
                        baud_cnt <= {BAUD_CNT_WIDTH{1'b0}};
                        tx_o <= 1'b1;
                        state <= StIdle;
                    end else begin
                        baud_cnt <= baud_cnt + 1'b1;
                    end
                end

                default: begin
                    state <= StIdle;
                end
            endcase
        end
    end

endmodule
