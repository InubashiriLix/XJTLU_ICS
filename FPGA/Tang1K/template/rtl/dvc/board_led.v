module board_led (
    input wire clk,
    input wire rst_n,
    output wire [2:0] led_ctrl,
    input wire toggle_led_color
);

    reg [2:0] led_state = 3'b110;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            led_state <= 3'b110;
        end else if (toggle_led_color) begin
            case (led_state)
                3'b110:  led_state <= 3'b101;
                3'b101:  led_state <= 3'b011;
                default: led_state <= 3'b110;
            endcase
        end
    end

    assign led_ctrl = led_state;

endmodule
