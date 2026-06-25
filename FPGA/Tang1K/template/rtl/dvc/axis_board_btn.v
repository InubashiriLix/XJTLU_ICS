`define BTN_EVENT_PRESSED 0
`define BTN_EVENT_RELEASED 1

module axis_board_btn #(
    parameter integer CLOCK_HZ = 27_000_000,
    parameter integer COUNT_MAX = CLOCK_HZ / 100,  // 10ms to debounce the button
    parameter integer EVENT_TYPE = `BTN_EVENT_PRESSED
) (
    input wire clk,    // 27MHz
    input wire rst_n,  // Active low reset
    input wire btn,    // Button input

    // whether the downstream module is ready to receive data
    input  wire s_axis_tready_i,
    // whether the button module has valid data to send
    output wire s_axis_tvalid_o,
    // the data part
    output wire selected_event_o,
    output wire btn_level_o
);

    localparam integer BtnEventPressed = 0;
    localparam integer BtnEventReleased = 1;
    localparam integer CountWidth = $clog2(COUNT_MAX + 1);

    // Counts how long the synchronized button input has stayed different from
    // the accepted stable level. When it reaches COUNT_MAX, the new level is
    // accepted as a real button state change.
    reg [CountWidth-1:0] cnt = 0;

    // Two-flop synchronizer for the physical button input. The button is
    // asynchronous to clk, so do not use btn directly in debounce logic.
    reg btn_meta = 0;
    reg btn_sync = 0;

    // Debounced, stable button level.
    reg btn_level_r = 0;

    // Previous-cycle debounced level. Used to generate one-cycle edge pulses:
    // pressed = 0 -> 1, released = 1 -> 0.
    reg btn_level_d = 0;

    // AXI-style valid holding register. Once the selected button event occurs,
    // this stays high until downstream asserts ready and accepts the event.
    reg s_axis_tvalid_r = 0;

    wire btn_pressed_pulse = btn_level_r & ~btn_level_d;
    wire btn_released_pulse = ~btn_level_r & btn_level_d;
    wire selected_event_pulse = (EVENT_TYPE == BtnEventReleased) ?
        btn_released_pulse : btn_pressed_pulse;
    wire axis_fire = s_axis_tvalid_o && s_axis_tready_i;

    assign btn_level_o = btn_level_r;
    assign s_axis_tvalid_o = s_axis_tvalid_r;

    // The stream data is a one-bit event token. Keep it high while valid is
    // waiting for ready, because AXI data must remain stable under backpressure.
    assign selected_event_o = s_axis_tvalid_r;

    /// sync the async button input into clk domain
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            btn_meta <= 1'b0;
            btn_sync <= 1'b0;
        end else begin
            btn_meta <= btn;
            btn_sync <= btn_meta;
        end
    end

    /// debounce, detect selected edge, and hold AXI valid until ready
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            cnt <= {CountWidth{1'b0}};
            btn_level_r <= 1'b0;
            btn_level_d <= 1'b0;
            s_axis_tvalid_r <= 1'b0;
        end else begin
            btn_level_d <= btn_level_r;

            if (btn_sync == btn_level_r) begin
                // if thte button level is the same with previous stable level, then reset the counter
                // this is because the button level is thinked to be shaking.
                // for the previous stable level, we define that it is not gone
                cnt <= {CountWidth{1'b0}};
            end else begin
                if (cnt == COUNT_MAX - 1) begin
                    // if the counter reaches the max, then accept the new
                    // level (it should be ~btn_level_r) as a new stable level
                    btn_level_r <= btn_sync;
                end else begin
                    cnt <= cnt + 1'b1;
                end
            end

            if (selected_event_pulse) begin
                s_axis_tvalid_r <= 1'b1;
            end else if (axis_fire) begin
                s_axis_tvalid_r <= 1'b0;
            end


            // btn_level_d <= btn_level_r;
            //
            // if (btn_sync == btn_level_r) begin
            //     cnt <= {CountWidth{1'b0}};
            // end else begin
            //     if (cnt == COUNT_MAX - 1) begin
            //         btn_level_r <= btn_sync;
            //         cnt <= {CountWidth{1'b0}};
            //     end else begin
            //         cnt <= cnt + 1'b1;
            //     end
            // end
            //
            // if (selected_event_pulse) begin
            //     s_axis_tvalid_r <= 1'b1;
            // end else if (axis_fire) begin
            //     s_axis_tvalid_r <= 1'b0;
            // end
        end
    end

endmodule
