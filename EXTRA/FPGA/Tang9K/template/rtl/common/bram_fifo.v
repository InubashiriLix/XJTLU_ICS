module bram_fifo #(
    parameter integer CLK_HZ = 27_000_000,
    parameter integer DATA_WIDTH = 8,
    parameter integer DEPTH = 1024
) (
    input wire clk,
    input wire rst_n,

    input wire wr_en,
    input wire [DATA_WIDTH-1:0] wr_data,
    output reg wr_valid_o,
    output wire full,

    input wire rd_en,
    output reg [DATA_WIDTH-1:0] rd_data,
    output reg rd_valid_o,
    output wire empty
);

    localparam integer AddrWidth = $clog2(DEPTH);

    /// NOTE: to make the yosys infer mem as bram, we should
    /// - memory array 不要异步复位
    /// - 读最好是同步读：rd_data <= mem[addr]
    /// - 写是同步写：if (we) mem[addr] <= data
    /// - 指针、状态机、valid 可以单独复位
    /// - 不要试图 reset 整个 mem
    /// - 不要把 RAM 读写拆得太奇怪，否则综合器匹配不到模板
    reg [DATA_WIDTH-1:0] mem[DEPTH];
    reg [AddrWidth-1:0] wr_ptr = 0;
    reg [AddrWidth-1:0] rd_ptr = 0;

    // This FIFO keeps one RAM slot unused so full and empty can be detected
    // using only the read/write pointers. Usable capacity is DEPTH - 1.
    function automatic [AddrWidth-1:0] next_ptr;
        input [AddrWidth-1:0] ptr;
        begin
            if (ptr == DEPTH - 1) begin
                next_ptr = {AddrWidth{1'b0}};
            end else begin
                next_ptr = ptr + 1'b1;
            end
        end
    endfunction

    assign full  = (next_ptr(wr_ptr) == rd_ptr);
    assign empty = (wr_ptr == rd_ptr);

    wire rd_fire = rd_en && !empty;
    wire wr_fire = wr_en && (!full || rd_fire);

    // Keep RAM access in a reset-free clocked block so Yosys can infer BRAM.
    always @(posedge clk) begin
        if (rst_n) begin
            if (wr_fire) begin
                mem[wr_ptr] <= wr_data;
            end

            if (rd_fire) begin
                rd_data <= mem[rd_ptr];
            end
        end
    end

    /// pointer and valid logic
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            wr_ptr <= {AddrWidth{1'b0}};
            rd_ptr <= {AddrWidth{1'b0}};
            wr_valid_o <= 1'b0;
            rd_valid_o <= 1'b0;
        end else begin
            if (wr_fire) begin
                wr_ptr <= next_ptr(wr_ptr);
            end

            if (rd_fire) begin
                rd_ptr <= next_ptr(rd_ptr);
            end

            wr_valid_o <= wr_fire;
            rd_valid_o <= rd_fire;
        end
    end
endmodule
