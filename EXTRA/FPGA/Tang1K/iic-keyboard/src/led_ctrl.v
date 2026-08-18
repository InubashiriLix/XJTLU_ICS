module led_ctrl #(
    parameter integer CLK_FREQ_HZ = 27_000_000,
    parameter integer BLINK_DURATION_MS = 500
) (
    input  wire clk,
    input  wire reg_written,
    output wire led,
    input wire rst_n
);

localparam integer BLINK_DURATION_CYCLES = (CLK_FREQ_HZ / 1000) * BLINK_DURATION_MS;
localparam [1:0] R_COLOR = 2'b00;
localparam [1:0] G_COLOR = 2'b01;
localparam [1:0] B_COLOR = 2'b10;

reg [22:0] wait_cnt = 23'd0;

always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        wait_cnt <= 23'd0;
    end
end


endmodule
