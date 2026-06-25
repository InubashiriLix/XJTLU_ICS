module top (
    input wire clk,  // 27MHz
    input wire btn,  // Button input
    output wire [2:0] led  // RGB LED output: red, green, blue
);


    wire rst_n = 1'b1;  // no reset signal for default
    wire btn_level;
    wire btn_pressed;
    wire btn_released;

    localparam integer RstCntMax = 1_000_000;
    reg [19:0] rst_cnt = 19'b0;


    board_button #(
        .COUNT_WIDTH(20),
        .COUNT_MAX  (1_000_000)  // 1_000_000 * 37ns = 37ms debounce time
    ) u_board_button (
        .clk(clk),
        .rst_n(rst_n),
        .btn(btn),
        .btn_level_o(btn_level),
        .btn_pressed_o(btn_pressed),
        .btn_released_o(btn_released)
    );

    board_led u_board_led (
        .clk(clk),
        .rst_n(rst_n),
        .led_ctrl(led),
        .toggle_led_color(btn_pressed)
    );

    // axis_bram_fifo #(
    //     .CLK_HZ(27_000_000),
    //     .DATA_WIDTH(1),
    //     .DEPTH(128)
    // ) (
    //     .clk(clk),
    //     .s_axis_tvalid_i(btn_pressed),
    //     .s_axis_tready_o(),
    //
    // )

    // delay and reset after system power on.
    always @(posedge clk) begin
        rst_cnt <= rst_cnt + 1'b1;
    end
    assign rst_n = (rst_cnt == RstCntMax);

endmodule

`default_nettype wire
