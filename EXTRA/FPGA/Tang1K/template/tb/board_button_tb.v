`timescale 1ns/1ps

module board_button_tb;
    reg clk = 1'b0;
    reg rst_n = 1'b0;
    reg btn = 1'b0;

    wire btn_level_o;
    wire btn_pressed_o;
    wire btn_released_o;

    always #3 clk = ~clk;

    board_button #(
        .COUNT_WIDTH(4),
        .COUNT_MAX(4)
    ) dut (
        .clk(clk),
        .rst_n(rst_n),
        .btn(btn),
        .btn_level_o(btn_level_o),
        .btn_pressed_o(btn_pressed_o),
        .btn_released_o(btn_released_o)
    );

    initial begin
        $dumpfile("sim/board_button.vcd");
        $dumpvars(0, board_button_tb);

        #20;
        rst_n = 1'b1;

        #30;
        btn = 1'b1;

        #100;
        btn = 1'b0;

        #100;
        $finish;
    end
endmodule

