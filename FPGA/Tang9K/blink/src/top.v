module top (
    input wire clk,  // 24MHz
    output wire [5:0] led  // LED output
);

    // Tangnano 9k has 6 LEDS with clock freq 26MHz, and we want to blnmk the LED
    // at 5Hz, so we nened to count 26M/5 = 5.2M clock cycles to toggle the LED, 
    localparam CLK_HZ = 27_000_000;
    localparam BLINK_FREQ = 5;  // 5 toogle per second
    localparam WAIT_TICKS = CLK_HZ / BLINK_FREQ;  // 5.4M
    // 27M / 5 = 5.4M, 2*23 = 8388608 < 5.4M < 2*24 = 16777216, so we need 24 bits to count to 5.4M 
    localparam [23:0] WAIT_MAX = WAIT_TICKS - 1;

    // the wait counter 
    reg [23:0] wait_cnt = 24'd0;
    // led state
    reg [ 2:0] led_state = 3'b000;

    // while (check clk change)
    always @(posedge clk) begin
        if (wait_cnt == WAIT_MAX) begin
            wait_cnt <= 24'd0;

            // if led state is 5, reset to 0, otherwise increment by 1
            if (led_state == 3'd5) begin
                led_state <= 3'd0;
            end else begin
                led_state <= led_state + 3'd1;
            end

        end else begin
            wait_cnt <= wait_cnt + 24'd1;
        end
    end

    // Create a one-hot LED pattern:
    // led_pos = 0 -> 000001
    // led_pos = 1 -> 000010
    // led_pos = 2 -> 000100
    // 这个傻逼是低电平亮，所以要取反
    // ...
    wire [5:0] led_onehot = 6'b000001 << led_state;

    // The onboard LEDs are active-low
    // 0 means ON, 1 means OFF
    assign led = ~led_onehot;

endmodule

`default_nettype wire
