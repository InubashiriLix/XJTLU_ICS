module pattern_gen #(
    parameter integer X_ACTIVE,
    parameter integer Y_ACTIVE,
    parameter integer X_WIDTH,
    parameter integer Y_WIDTH,
    parameter integer BOX_START_X,
    parameter integer BOX_START_Y,
    parameter integer BOX_END_X,
    parameter integer BOX_END_Y,
    parameter integer BODER_WIDTH = 10
) (
    input wire [X_WIDTH-1:0] x,
    input wire [Y_WIDTH-1:0] y,
    input wire active,

    output logic [7:0] rgb_r_o,
    output logic [7:0] rgb_g_o,
    output logic [7:0] rgb_b_o
);

    wire border;
    wire grid;
    wire box;

    // well we have a fucking border line,
    assign border = (x < BODER_WIDTH) || (x >= X_ACTIVE - BODER_WIDTH)
    || (y < BODER_WIDTH) || (y >= Y_ACTIVE - BODER_WIDTH);
    // if the pidxel (x, y) in the box area then we draw a fucking box
    assign box = (x >= BOX_START_X) && (x < BOX_END_X) && (y >= BOX_START_Y) && (y < BOX_END_Y);

    always_comb begin
        if (!active) begin
            rgb_r_o = 8'h00;
            rgb_g_o = 8'h00;
            rgb_b_o = 8'h00;
        end else if (border) begin
            rgb_r_o = 8'hff;
            rgb_g_o = 8'hff;
            rgb_b_o = 8'hff;
        end else if (box) begin
            rgb_r_o = 8'hff;
            rgb_g_o = 8'h40;
            rgb_b_o = 8'h20;
        end else begin
            rgb_r_o = 8'h00;
            rgb_g_o = 8'h00;
            rgb_b_o = 8'h40;
        end
    end
endmodule
