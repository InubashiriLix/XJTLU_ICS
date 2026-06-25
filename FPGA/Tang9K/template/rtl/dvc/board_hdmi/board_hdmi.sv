module board_hdmi #(
    parameter integer CLK_HZ = 27_000_000,
    parameter integer DISP_HZ = 60,
    // horizontal, this part should depend by user themself
    parameter integer H_ACTIVATE = 720,
    parameter integer H_FRONT_PORCH = 16,
    parameter integer H_SYNC_PULSE = 62,
    parameter integer H_BACK_PORCH = 60,
    // vertical
    parameter integer V_ACTIVATE = 480,
    parameter integer V_FRONT_PORCH = 9,
    parameter integer V_SYNC_PULSE = 6,
    parameter integer V_BACK_PORCH = 30
) (
    input wire clk,
    input wire rst_n,

    output wire tmds_clk_p,
    output wire tmds_clk_n,
    // NOTE: data[0] -> blue, data[1] -> green, data[2] -> red
    output wire [2:0] tmds_data_p,
    output wire [2:0] tmds_data_n
);

    localparam integer HorizontalTotal = H_ACTIVATE + H_FRONT_PORCH + H_SYNC_PULSE + H_BACK_PORCH;
    localparam integer VerticalTotal = V_ACTIVATE + V_FRONT_PORCH + V_SYNC_PULSE + V_BACK_PORCH;
    localparam integer PixelClkHz = DISP_HZ * HorizontalTotal * VerticalTotal;
    localparam integer PixelClkHz5x = PixelClkHz * 5;
    localparam integer PllIdivSel = 0, PllFbdivSel = 0, PllOdivSel = 16;
    localparam integer PllIdivSel5x = 0, PllFbdivSel5x = 4, PllOdivSel5x = 4;

    // instanlize the PLL
    wire clk_pix;
    wire pll_lock;

    rPLL #(
        .FCLKIN("27"),
        .IDIV_SEL(PllIdivSel),
        .FBDIV_SEL(PllFbdivSel),
        .ODIV_SEL(PllOdivSel),
        .DYN_IDIV_SEL("false"),
        .DYN_FBDIV_SEL("false"),
        .DYN_ODIV_SEL("false"),
        .PSDA_SEL("0000"),
        .DYN_DA_EN("false"),
        .DUTYDA_SEL("1000"),
        .CLKOUT_FT_DIR(1'b1),
        .CLKOUTP_FT_DIR(1'b1),
        .CLKOUT_DLY_STEP(0),
        .CLKOUTP_DLY_STEP(0),
        .CLKFB_SEL("internal"),
        .CLKOUT_BYPASS("false"),
        .CLKOUTP_BYPASS("false"),
        .CLKOUTD_BYPASS("false"),
        .DYN_SDIV_SEL(2),
        .CLKOUTD_SRC("CLKOUT"),
        .CLKOUTD3_SRC("CLKOUT"),
        .DEVICE("GW1N-9C")
    ) u_pll_pix (
        .CLKIN   (clk),
        .CLKOUT  (clk_pix),
        .LOCK    (pll_lock),
        .CLKOUTP (),
        .CLKOUTD (),
        .CLKOUTD3(),
        .RESET   (1'b0),
        .RESET_P (1'b0),
        .CLKFB   (1'b0),
        .FBDSEL  (6'd0),
        .IDSEL   (6'd0),
        .ODSEL   (6'd0),
        .PSDA    (4'd0),
        .DUTYDA  (4'd0),
        .FDLY    (4'd0)
    );


    wire clk_pix_5x;
    wire pll_lock_5x;

    rPLL #(
        .FCLKIN("27"),
        .IDIV_SEL(PllIdivSel5x),
        .FBDIV_SEL(PllFbdivSel5x),
        .ODIV_SEL(PllOdivSel5x),
        .DYN_IDIV_SEL("false"),
        .DYN_FBDIV_SEL("false"),
        .DYN_ODIV_SEL("false"),
        .PSDA_SEL("0000"),
        .DYN_DA_EN("false"),
        .DUTYDA_SEL("1000"),
        .CLKOUT_FT_DIR(1'b1),
        .CLKOUTP_FT_DIR(1'b1),
        .CLKOUT_DLY_STEP(0),
        .CLKOUTP_DLY_STEP(0),
        .CLKFB_SEL("internal"),
        .CLKOUT_BYPASS("false"),
        .CLKOUTP_BYPASS("false"),
        .CLKOUTD_BYPASS("false"),
        .DYN_SDIV_SEL(2),
        .CLKOUTD_SRC("CLKOUT"),
        .CLKOUTD3_SRC("CLKOUT"),
        .DEVICE("GW1N-9C")
    ) u_pll_pix_5x (
        .CLKIN   (clk),
        .CLKOUT  (clk_pix_5x),
        .LOCK    (pll_lock_5x),
        .CLKOUTP (),
        .CLKOUTD (),
        .CLKOUTD3(),
        .RESET   (1'b0),
        .RESET_P (1'b0),
        .CLKFB   (1'b0),
        .FBDSEL  (6'd0),
        .IDSEL   (6'd0),
        .ODSEL   (6'd0),
        .PSDA    (4'd0),
        .DUTYDA  (4'd0),
        .FDLY    (4'd0)
    );

    wire rst = !rst_n || !pll_lock || !pll_lock_5x;

    localparam integer HWidth = $clog2(HorizontalTotal);
    localparam integer VWidth = $clog2(VerticalTotal);
    wire [HWidth-1:0] x;
    wire [VWidth-1:0] y;
    wire hsync;
    wire vsync;
    wire active;

    video_timing #(
        .H_ACTIVE(H_ACTIVATE),
        .H_FRONT_PORCH(H_FRONT_PORCH),
        .H_SYNC_PULSE(H_SYNC_PULSE),
        .H_BACK_PORCH(H_BACK_PORCH),

        .V_ACTIVE(V_ACTIVATE),
        .V_FRONT_PORCH(V_FRONT_PORCH),
        .V_SYNC_PULSE(V_SYNC_PULSE),
        .V_BACK_PORCH(V_BACK_PORCH)
    ) u_video_timing (
        .clk(clk_pix),
        .rst(rst),
        .x(x),
        .y(y),
        .hsync(hsync),
        .vsync(vsync),
        .active(active)
    );

    wire [7:0] rgb_r;
    wire [7:0] rgb_g;
    wire [7:0] rgb_b;

    pattern_gen #(
        .X_ACTIVE(H_ACTIVATE),
        .Y_ACTIVE(V_ACTIVATE),
        .X_WIDTH(HWidth),
        .Y_WIDTH(VWidth),
        .BOX_START_X(100),
        .BOX_START_Y(100),
        .BOX_END_X(300),
        .BOX_END_Y(300)
    ) pattern_gen (
        .x(x),
        .y(y),
        .active(active),
        .rgb_r_o(rgb_r),
        .rgb_g_o(rgb_g),
        .rgb_b_o(rgb_b)
    );


    wire [9:0] tmds_r;
    wire [9:0] tmds_g;
    wire [9:0] tmds_b;

    tmds_encoder u_tmds_red (
        .pixel_clk(clk_pix),
        .rst(rst),
        .din(rgb_r),
        .ctrl(2'b00),
        .de(active),
        .dout(tmds_r)
    );

    tmds_encoder u_tmds_green (
        .pixel_clk(clk_pix),
        .rst(rst),
        .din(rgb_g),
        .ctrl(2'b00),
        .de(active),
        .dout(tmds_g)
    );

    tmds_encoder u_tmds_blue (
        .pixel_clk(clk_pix),
        .rst(rst),
        .din(rgb_b),
        .ctrl({vsync, hsync}),
        .de(active),
        .dout(tmds_b)
    );

    tmds_serializer u_ser_blue (
        .pixel_clk(clk_pix),
        .pixel_clk_5x(clk_pix_5x),
        .rst(rst),
        .tmds_word(tmds_b),
        .tmds_p(tmds_data_p[0]),
        .tmds_n(tmds_data_n[0])
    );

    tmds_serializer u_ser_green (
        .pixel_clk(clk_pix),
        .pixel_clk_5x(clk_pix_5x),
        .rst(rst),
        .tmds_word(tmds_g),
        .tmds_p(tmds_data_p[1]),
        .tmds_n(tmds_data_n[1])
    );

    tmds_serializer u_ser_red (
        .pixel_clk(clk_pix),
        .pixel_clk_5x(clk_pix_5x),
        .rst(rst),
        .tmds_word(tmds_r),
        .tmds_p(tmds_data_p[2]),
        .tmds_n(tmds_data_n[2])
    );

    localparam unsigned [9:0] TmdsClkWord = 10'b0000011111;
    // NOTE: the tmds clock lnae will send 10'b0000011111
    tmds_serializer u_ser_clk (
        .pixel_clk(clk_pix),
        .pixel_clk_5x(clk_pix_5x),
        .rst(rst),
        .tmds_word(TmdsClkWord),
        .tmds_p(tmds_clk_p),
        .tmds_n(tmds_clk_n)
    );

endmodule
