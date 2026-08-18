module board_button #(
    parameter integer CLOCK_HZ = 27_000_000,
    parameter integer COUNT_WIDTH = 20,
    parameter integer COUNT_MAX = CLOCK_HZ / 100  // 10ms to debounce the button
) (
    input wire clk,    // 27MHz
    input wire rst_n,  // Active low reset
    input wire btn,    // Button input

    output reg btn_level_o,
    output reg btn_pressed_o,
    output reg btn_released_o
);

    reg btn_meta;
    reg btn_sync;

    reg [COUNT_WIDTH-1:0] cnt;
    reg btn_level_d;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            btn_meta <= 1'b0;
            btn_sync <= 1'b0;
        end else begin
            btn_meta <= btn;
            btn_sync <= btn_meta;
        end
    end

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            cnt <= {COUNT_WIDTH{1'b0}};
            btn_level_o <= 1'b0;
        end else begin
            if (btn_sync == btn_level_o) begin
                // if no change happend or still is unstable, then
                cnt <= {COUNT_WIDTH{1'b0}};
            end else begin
                // if change happend
                if (cnt == COUNT_MAX - 1) begin
                    // new state is stable now, admit the new state
                    btn_level_o <= btn_sync;
                    // reset the ccnt
                    cnt <= {COUNT_WIDTH{1'b0}};
                end else begin
                    // increase the counter to wait for the new state to be stable
                    cnt <= cnt + 1'b1;
                end
            end
        end
    end

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            btn_level_d <= 1'b0;
            // btn_level_o <= 1'd0;
            btn_pressed_o <= 1'b0;
            btn_released_o <= 1'b0;
        end else begin
            btn_level_d <= btn_level_o;
            btn_pressed_o <= btn_level_o & ~btn_level_d;
            btn_released_o <= ~btn_level_o & btn_level_d;
        end
    end
endmodule
