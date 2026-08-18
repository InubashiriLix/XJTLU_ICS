module axis_spi #(
    parameter int unsigned CLK_FREQ = 270_000_000,
    parameter int unsigned SPI_FREQ = 100_000_000,
    parameter int unsigned SPI_MODE = 0,  // NOTE: not changable now, set 0 temporily
    parameter int unsigned SPI_DATA_WIDTH = 8,
    parameter int unsigned TX_FIFO_DEPTH = 32
) (
    input wire clk,
    input wire rst_n,

    //  ====== from master ======
    input wire [SPI_DATA_WIDTH-1:0] m_axis_tdata,
    input wire m_axis_tvalid,  // got valid data to send
    input wire m_axis_keep,  // this frame is valid
    input wire m_axis_tlast,  // last frame of data.
    // ========= to master ========
    output wire m_axis_tready,  // ready to accept data

    // ========== spi pins: inputs ==========
    input  wire sclk,
    input  wire cs_n,
    input  wire mosi,
    // ======= spi pins: baord inputs =======
    output wire miso,
    output wire int_n
);

    // check if mode parameter is valid
    initial begin
        if (SPI_MODE != 0) begin
            $error("SPI_MODE %0d is not supported, only mode 0 is supported now.", SPI_MODE);
        end
    end

    logic sclk_meta, sclk_sync, sclk_prev;
    logic miso_meta, miso_sync;
    logic cs_meta, cs_sync, cs_prev;
    wire axis_fire = m_axis_tvalid && m_axis_tready && m_axis_keep;

    always_ff @(posedge clk) begin
        if (!rst_n) begin
            sclk_meta <= 1'b0;
            sclk_sync <= 1'b0;
            sclk_prev <= 1'b0;

            miso_meta <= 1'b0;
            miso_sync <= 1'b0;

            cs_meta   <= 1'b1;
            cs_sync   <= 1'b1;
            cs_prev   <= 1'b1;
        end else begin
            sclk_meta <= sclk;
            sclk_sync <= sclk_meta;
            sclk_prev <= sclk_sync;

            miso_meta <= miso;
            miso_sync <= miso_meta;

            cs_meta   <= cs_n;
            cs_sync   <= cs_meta;
            cs_prev   <= cs_sync;
        end
    end

    wire cs_active = ~cs_sync;
    wire cs_fall = cs_prev & ~cs_sync;
    wire cs_rise = ~cs_prev & cs_sync;

    wire sclk_rise = sclk_prev & ~sclk_sync;
    wire sclk_fall = ~sclk_prev & sclk_sync;

    // bram fifo signals
    wire fifo_wr_en;
    wire [SPI_DATA_WIDTH-1:0] fifo_wr_data;
    wire fifo_wr_valid;
    wire full;

    wire fifo_rd_en;
    wire [SPI_DATA_WIDTH-1:0] fifo_rd_data;
    wire fifo_rd_valid;
    wire fifo_empty;

    bram_fifo #(
        .DATA_WIDTH(SPI_DATA_WIDTH),
        .DEPTH(TX_FIFO_DEPTH)
    ) tx_fifo (
        .clk  (clk),
        .rst_n(rst_n),

        .wr_en(fifo_wr_en),
        .wr_data(fifo_wr_data),
        .wr_valid_o(fifo_wr_valid),
        .full(full),

        .rd_en(fifo_rd_en),
        .rd_data(fifo_rd_data),
        .rd_valid_o(fifo_rd_valid),
        .empty(fifo_empty)
    );

    // master sample MISO on sclk rising edge
    // master samples MOSI on sclk rising edge.
    // slave change MISO on sclk falling edge.
    localparam int BitCntWidth = SPI_DATA_WIDTH <= 2? 1 : $clog2(SPI_DATA_WIDTH);
    reg [BitCntWidth-1:0] bit_cnt;

    logic tx_shift;
    logic rx_shift;

    logic [7:0] rx_bytes;
    logic rx_bytes_valid;

    logic miso_r;
    assign miso = cs_active? miso_r : 1'b0;

    wire [7:0] next_tx_byte = tx_fifo_empty? 8'h00 : tx_fifo_rdata;


endmodule
