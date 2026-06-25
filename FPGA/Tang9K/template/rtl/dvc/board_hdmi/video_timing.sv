module video_timing #(
    // horizontal
    parameter integer H_ACTIVE = 640,
    parameter integer H_FRONT_PORCH = 16,
    parameter integer H_SYNC_PULSE = 62,
    parameter integer H_BACK_PORCH = 60,

    //  vertical
    parameter integer V_ACTIVE = 480,
    parameter integer V_FRONT_PORCH = 9,
    parameter integer V_SYNC_PULSE = 6,
    parameter integer V_BACK_PORCH = 30,

    parameter bit H_SYNC_POLARITY = 1'b0,
    parameter bit V_SYNC_POLARITY = 1'b0
) (
    input wire clk,
    input wire rst,

    output logic [$clog2(H_ACTIVE + H_FRONT_PORCH + H_SYNC_PULSE +
      H_BACK_PORCH)-1:0] x,
    output logic [$clog2(V_ACTIVE + V_FRONT_PORCH + V_SYNC_PULSE +
      V_BACK_PORCH)-1:0] y,
    output logic hsync,
    output logic vsync,
    output logic active
);

    localparam integer H_TOTAL = H_ACTIVE + H_FRONT_PORCH + H_SYNC_PULSE + H_BACK_PORCH;
    localparam integer V_TOTAL = V_ACTIVE + V_FRONT_PORCH + V_SYNC_PULSE + V_BACK_PORCH;

    logic [$clog2(H_TOTAL)-1:0] h_count;
    logic [$clog2(V_TOTAL)-1:0] v_count;

    wire h_sync_area = h_count >= (H_ACTIVE + H_FRONT_PORCH)
      && h_count < (H_ACTIVE + H_FRONT_PORCH + H_SYNC_PULSE);
    wire v_sync_area = v_count >= (V_ACTIVE + V_FRONT_PORCH)
      && v_count < (V_ACTIVE + V_FRONT_PORCH + V_SYNC_PULSE);

    always_ff @(posedge clk) begin
        if (rst) begin
            h_count <= 0;
            v_count <= 0;
        end else begin
            if (h_count == H_TOTAL - 1) begin
                h_count <= 0;

                if (v_count == V_TOTAL - 1) begin
                    v_count <= 0;
                end else begin
                    v_count <= v_count + 1'b1;
                end
            end else begin
                h_count <= h_count + 1'b1;
            end
        end
    end

    always_comb begin
        active = h_count < H_ACTIVE && v_count < V_ACTIVE;
        x = h_count;
        y = v_count;

        hsync = h_sync_area ? H_SYNC_POLARITY : ~H_SYNC_POLARITY;
        vsync = v_sync_area ? V_SYNC_POLARITY : ~V_SYNC_POLARITY;
    end


endmodule
