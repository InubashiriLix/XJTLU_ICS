`timescale 1ns/1ps

module board_led_tb;
    reg clk = 1'b0;
    reg rst_n = 1'b0;
    reg toggle_led_color = 1'b0;

    wire [5:0] led_ctrl;

    always #5 clk = ~clk;

    board_led dut (
        .clk(clk),
        .rst_n(rst_n),
        .led_ctrl(led_ctrl),
        .toggle_led_color(toggle_led_color)
    );

    initial begin
        $dumpfile("sim/board_led.vcd");
        $dumpvars(0, board_led_tb);

        #20;
        rst_n = 1'b1;

        #30;
        toggle_led_color = 1'b1;
        #10;
        toggle_led_color = 1'b0;

        #40;
        toggle_led_color = 1'b1;
        #10;
        toggle_led_color = 1'b0;

        #100;
        $finish;
    end
endmodule
