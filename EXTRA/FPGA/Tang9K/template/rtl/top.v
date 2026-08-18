module top #(
    parameter integer CLK_HZ = 27_000_000,
    parameter integer BLINK_HZ = 5,
    parameter integer RESET_CYCLES = 1_000_000
) (
    input wire clk,  // 27MHz
    output wire [5:0] led,  // Tang Nano 9K onboard LEDs, active-low
    output wire tmds_clk_p,
    output wire tmds_clk_n,
    output wire [2:0] tmds_data_p,
    output wire [2:0] tmds_data_n
);

    localparam integer RstCntMax = RESET_CYCLES;
    localparam integer WaitTicks = CLK_HZ / BLINK_HZ;
    localparam integer WaitMax = WaitTicks - 1;

    reg [19:0] rst_cnt = 20'd0;
    reg [23:0] wait_cnt = 24'd0;
    reg led_step = 1'b0;

    wire rst_n = (rst_cnt == RstCntMax);

    board_led u_board_led (
        .clk(clk),
        .rst_n(rst_n),
        .led_ctrl(led),
        .toggle_led_color(led_step)
    );

    board_hdmi #(
        .CLK_HZ(CLK_HZ)
    ) u_board_hdmi (
        .clk(clk),
        .rst_n(rst_n),
        .tmds_clk_p(tmds_clk_p),
        .tmds_clk_n(tmds_clk_n),
        .tmds_data_p(tmds_data_p),
        .tmds_data_n(tmds_data_n)
    );

    always @(posedge clk) begin
        if (rst_cnt != RstCntMax) begin
            rst_cnt <= rst_cnt + 1'b1;
        end
    end

    always @(posedge clk) begin
        if (!rst_n) begin
            wait_cnt <= 24'd0;
            led_step <= 1'b0;
        end else if (wait_cnt == WaitMax) begin
            wait_cnt <= 24'd0;
            led_step <= 1'b1;
        end else begin
            wait_cnt <= wait_cnt + 24'd1;
            led_step <= 1'b0;
        end
    end

endmodule

`default_nettype wire
