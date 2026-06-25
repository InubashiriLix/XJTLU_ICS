module tmds_serializer #(
    parameter integer DATA_WIDTH = 10
) (
    input wire pixel_clk,
    input wire pixel_clk_5x,
    input wire rst,
    input wire [DATA_WIDTH-1:0] tmds_word,
    output wire tmds_p,
    output wire tmds_n
);

    wire tmds_serial;

    OSER10 u_oser_p (
        .Q(tmds_serial),
        .D0(tmds_word[0]),
        .D1(tmds_word[1]),
        .D2(tmds_word[2]),
        .D3(tmds_word[3]),
        .D4(tmds_word[4]),
        .D5(tmds_word[5]),
        .D6(tmds_word[6]),
        .D7(tmds_word[7]),
        .D8(tmds_word[8]),
        .D9(tmds_word[9]),
        .FCLK(pixel_clk_5x),
        .PCLK(pixel_clk),
        .RESET(rst)
    );

    ELVDS_OBUF u_tmds_obuf (
        .I (tmds_serial),
        .O (tmds_p),
        .OB(tmds_n)
    );

endmodule
