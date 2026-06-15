module top (
    input  wire        clk,
    inout  wire        scl,
    inout  wire        sda,
    output wire        int_n,
    output wire [5:0]  led,
    input  wire [2:0]  row_in,
    output wire [2:0]  col_out
);

    wire rst_n;
    wire scl_in;
    wire sda_in;
    wire sda_out;
    wire sda_oe;
    wire [7:0]  reg_data;
    wire [15:0] key_data;
    wire        key_valid;
    wire        reg_written;
    assign scl_in = scl;

    reg [17:0] por_cnt = 18'd0;
    assign rst_n = &por_cnt;

    always @(posedge clk) begin
        if (!rst_n) por_cnt <= por_cnt + 1'b1;
    end

    IOBUF sda_buf (
        .O  (sda_in),
        .IO (sda),
        .I  (sda_out),
        .OEN(~sda_oe)
    );

    iic_slave #(
        .SLAVE_ADDR(7'h5A)
    ) slave (
        .clk        (clk),
        .rst_n      (rst_n),
        .scl_in     (scl_in),
        .sda_in     (sda_in),
        .sda_out    (sda_out),
        .sda_oe     (sda_oe),
        .key_data   (key_data[7:0]),
        .key_valid  (key_valid),
        .reg_data   (reg_data),
        .reg_written(reg_written),
        .int_n      (int_n)
    );

    key_scanner #(
        .CLK_FREQ_HZ(27_000_000),
        .SCAN_FREQ_HZ(2000),
        .DEBOUNCE_TIME_MS(20)
    ) u_scanner (
        .clk       (clk),
        .rst_n     (rst_n),
        .col_out   (col_out),
        .row_in    (row_in),
        .key_data  (key_data),
        .key_valid (key_valid)
    );

endmodule
