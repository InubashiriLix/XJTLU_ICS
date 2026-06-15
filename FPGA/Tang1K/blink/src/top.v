module top (
    input wire clk,  // 24MHz
    output wire [2:0] led  // RGB LED output: red, green, blue
);

    localparam CLK_HZ = 24_000_000;
    localparam BLINK_FREQ = 5;
    localparam WAIT_TICKS = CLK_HZ / BLINK_FREQ;
    localparam [22:0] WAIT_MAX = WAIT_TICKS - 1;

    reg [22:0] wait_cnt = 23'd0;
    reg [ 1:0] led_state = 2'd0;

    always @(posedge clk) begin
        if (wait_cnt == WAIT_MAX) begin
            wait_cnt <= 23'd0;

            if (led_state == 2'd2) begin
                led_state <= 2'd0;
            end else begin
                led_state <= led_state + 2'd1;
            end
        end else begin
            wait_cnt <= wait_cnt + 23'd1;
        end
    end

    wire [2:0] led_onehot = 3'b001 << led_state;

    assign led = led_onehot;

endmodule

`default_nettype wire
