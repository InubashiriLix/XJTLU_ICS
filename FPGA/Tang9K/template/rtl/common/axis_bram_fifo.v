module axis_bram_fifo #(
    parameter integer CLK_HZ = 27_000_000,
    parameter integer DATA_WIDTH = 8,
    parameter integer DEPTH = 1024
) (
    input wire clk,
    input wire rst_n,

    // Slave/input side: upstream sends data into this FIFO.
    input wire s_axis_tvalid_i,
    output wire s_axis_tready_o,
    input wire [DATA_WIDTH-1:0] s_axis_tdata_i,

    // Master/output side: this FIFO sends data to downstream.
    output wire m_axis_tvalid_o,
    input wire m_axis_tready_i,
    output wire [DATA_WIDTH-1:0] m_axis_tdata_o
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

    // AXI output holding register.
    // m_axis_tvalid_o means this register already contains stable data.
    // It does NOT mean "the BRAM is non-empty right now".
    reg [DATA_WIDTH-1:0] m_axis_tdata_r;
    reg m_axis_tvalid_r;

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

    // The BRAM FIFO keeps one slot unused. Therefore usable capacity is DEPTH - 1.
    wire mem_full = (next_ptr(wr_ptr) == rd_ptr);
    wire mem_empty = (wr_ptr == rd_ptr);

    // AXI transfer on the output side. Only this event lets downstream consume data.
    wire m_axis_fire = m_axis_tvalid_r && m_axis_tready_i;

    // Internal BRAM prefetch:
    // - read when the output register is empty, or
    // - read when downstream consumes the current output word in this cycle.
    //
    // This is why the first output appears one clock after the first write:
    // synchronous BRAM read updates m_axis_tdata_r at the clock edge.
    wire read_fire = (!m_axis_tvalid_r || m_axis_fire) && !mem_empty;

    // AXI transfer on the input side.
    wire write_fire = s_axis_tvalid_i && s_axis_tready_o;

    // Accept input when the BRAM queue has space. If it is full but we are
    // also prefetching/consuming one word this cycle, accept the new word too.
    assign s_axis_tready_o = !mem_full || read_fire;

    assign m_axis_tvalid_o = m_axis_tvalid_r;
    assign m_axis_tdata_o  = m_axis_tdata_r;

    // Keep RAM access in a reset-free clocked block so Yosys can infer BRAM.
    always @(posedge clk) begin
        if (rst_n) begin
            if (write_fire) begin
                mem[wr_ptr] <= s_axis_tdata_i;
            end

            if (read_fire) begin
                // Synchronous BRAM read: data becomes visible after this clock edge.
                m_axis_tdata_r <= mem[rd_ptr];
            end
        end
    end

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            wr_ptr <= {AddrWidth{1'b0}};
            rd_ptr <= {AddrWidth{1'b0}};
            m_axis_tvalid_r <= 1'b0;
        end else begin
            if (write_fire) begin
                wr_ptr <= next_ptr(wr_ptr);
            end

            if (read_fire) begin
                rd_ptr <= next_ptr(rd_ptr);
            end

            if (read_fire) begin
                // A BRAM word has been prefetched into m_axis_tdata_r.
                m_axis_tvalid_r <= 1'b1;
            end else if (m_axis_fire) begin
                // Downstream consumed the last output word and no new word was fetched.
                m_axis_tvalid_r <= 1'b0;
            end
        end
    end
endmodule
