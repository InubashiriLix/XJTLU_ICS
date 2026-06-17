module uart_rx #(
    parameter integer CLOCK_HZ = 27_000_000,
    parameter integer BAUD_RATE = 115_200,
    parameter integer BAUD_CNT_MAX = CLOCK_HZ / BAUD_RATE,
    parameter integer BAUD_CNT_WIDTH = 16
) (
    input wire clk,
    input wire rst_n,
    input wire rx_i,

    output reg [7:0] rx_data_o,
    output reg rx_valid_o,
    output reg frame_error_o
);

    localparam integer StIdle = 3'd0;
    localparam integer StStart = 3'd1;
    localparam integer StData = 3'd2;
    localparam integer StStop = 3'd3;
    localparam integer StartSampleCnt = BAUD_CNT_MAX / 2 - 1;
    localparam integer BitSampleCnt = BAUD_CNT_MAX - 1;

    reg [2:0] state;

    // the register for sync
    reg rx_meta;
    reg rx_sync;

    // counter for baud rate
    reg [BAUD_CNT_WIDTH-1:0] baud_cnt;
    reg [2:0] bit_idx;
    reg [7:0] rx_data;

    // sync the rx signal first
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            rx_meta <= 1'b1;
            rx_sync <= 1'b1;
        end else begin
            rx_meta <= rx_i;
            rx_sync <= rx_meta;
        end
    end

    // state machine
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            // handle the reset logic
            state <= StIdle;
            baud_cnt <= {BAUD_CNT_WIDTH{1'b0}};
            bit_idx <= 3'b0;
            rx_data <= 8'b0;
            rx_data_o <= 8'b0;
            rx_valid_o <= 1'b0;
            frame_error_o <= 1'b0;
        end else begin
            rx_valid_o <= 1'b0;
            frame_error_o <= 1'b0;

            case (state)
                StIdle: begin
                    baud_cnt <= {BAUD_CNT_WIDTH{1'b0}};
                    bit_idx <= 3'b0;

                    if (!rx_sync) begin
                        state <= StStart;
                    end
                end

                StStart: begin
                    if (baud_cnt == StartSampleCnt[BAUD_CNT_WIDTH-1:0]) begin
                        baud_cnt <= {BAUD_CNT_WIDTH{1'b0}};

                        if (!rx_sync) begin
                            state <= StData;
                        end else begin
                            state <= StIdle;
                        end
                    end else begin
                        baud_cnt <= baud_cnt + 1'b1;
                    end
                end

                StData: begin
                    if (baud_cnt == BitSampleCnt[BAUD_CNT_WIDTH-1:0]) begin
                        baud_cnt <= {BAUD_CNT_WIDTH{1'b0}};

                        rx_data[bit_idx] <= rx_sync;

                        if (bit_idx == 3'd7) begin
                            bit_idx <= 3'd0;
                            state <= StStop;
                        end else begin
                            bit_idx <= bit_idx + 1'b1;
                        end
                    end else begin
                        baud_cnt <= baud_cnt + 1'b1;
                    end
                end

                StStop: begin
                    if (baud_cnt == BitSampleCnt[BAUD_CNT_WIDTH-1:0]) begin
                        baud_cnt <= {BAUD_CNT_WIDTH{1'b0}};
                        state <= StIdle;

                        if (rx_sync) begin
                            rx_data_o <= rx_data;
                            rx_valid_o <= 1'b1;
                        end else begin
                            frame_error_o <= 1'b1;
                        end
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
