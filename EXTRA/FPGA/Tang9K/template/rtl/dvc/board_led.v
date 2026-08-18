module board_led (
    input wire clk,
    input wire rst_n,
    output wire [5:0] led_ctrl,
    input wire toggle_led_color
);

    reg [2:0] led_index = 3'd0;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            led_index <= 3'd0;
        end else if (toggle_led_color) begin
            if (led_index == 3'd5) begin
                led_index <= 3'd0;
            end else begin
                led_index <= led_index + 3'd1;
            end
        end
    end

    assign led_ctrl = ~(6'b000001 << led_index);

endmodule
