`timescale 1ns / 1ps

module axis_bram_fifo_tb;
    localparam integer DATA_WIDTH = 8;
    localparam integer DEPTH = 4;

    reg clk = 0;
    reg rst_n = 0;

    reg s_axis_tvalid_i = 0;
    wire s_axis_tready_o;
    reg [DATA_WIDTH-1:0] s_axis_tdata_i = 0;

    wire m_axis_tvalid_o;
    reg m_axis_tready_i = 0;
    wire [DATA_WIDTH-1:0] m_axis_tdata_o;

    integer errors = 0;

    always #1 clk = ~clk;

    axis_bram_fifo #(
        .DATA_WIDTH(DATA_WIDTH),
        .DEPTH(DEPTH)
    ) dut (
        .clk(clk),
        .rst_n(rst_n),
        .s_axis_tvalid_i(s_axis_tvalid_i),
        .s_axis_tready_o(s_axis_tready_o),
        .s_axis_tdata_i(s_axis_tdata_i),
        .m_axis_tvalid_o(m_axis_tvalid_o),
        .m_axis_tready_i(m_axis_tready_i),
        .m_axis_tdata_o(m_axis_tdata_o)
    );

    task tick;
        begin
            @(posedge clk);
            #0.1;
        end
    endtask

    task check;
        input condition;
        input [255:0] message;
        begin
            if (!condition) begin
                errors = errors + 1;
                $display("FAIL: %0s", message);
            end
        end
    endtask

    task push_stalled_output;
        input [DATA_WIDTH-1:0] data;
        begin
            s_axis_tvalid_i = 1'b1;
            s_axis_tdata_i  = data;
            m_axis_tready_i = 1'b0;
            #0.1;
            check(s_axis_tready_o, "s_axis_tready_o should accept while filling");
            tick();
        end
    endtask

    task consume_expect;
        input [DATA_WIDTH-1:0] expected;
        begin
            s_axis_tvalid_i = 1'b0;
            m_axis_tready_i = 1'b1;
            #0.1;
            check(m_axis_tvalid_o, "m_axis_tvalid_o should be high before consume");
            check(m_axis_tdata_o == expected, "m_axis_tdata_o mismatch before consume");
            tick();
        end
    endtask

    initial begin
        $dumpfile("sim/axis_bram_fifo.vcd");
        $dumpvars(0, axis_bram_fifo_tb);

        repeat (3) tick();
        rst_n = 1'b1;
        repeat (2) tick();
        check(!m_axis_tvalid_o, "m_axis_tvalid_o should be low after reset");

        push_stalled_output(8'h11);
        push_stalled_output(8'h22);
        push_stalled_output(8'h33);
        push_stalled_output(8'h44);

        s_axis_tvalid_i = 1'b1;
        s_axis_tdata_i  = 8'h55;
        m_axis_tready_i = 1'b0;
        #0.1;
        check(!s_axis_tready_o, "input should stall when FIFO is full");
        check(m_axis_tvalid_o, "output valid should stay high during backpressure");
        check(m_axis_tdata_o == 8'h11, "output data should hold during backpressure");
        tick();
        check(m_axis_tvalid_o, "output valid should still hold after one stalled cycle");
        check(m_axis_tdata_o == 8'h11, "output data should still hold after one stalled cycle");

        s_axis_tvalid_i = 1'b1;
        s_axis_tdata_i  = 8'h55;
        m_axis_tready_i = 1'b1;
        #0.1;
        check(s_axis_tready_o, "input should be accepted when full and output consumes");
        check(m_axis_tvalid_o, "output valid should be high on full plus consume");
        check(m_axis_tdata_o == 8'h11, "first output should be visible before consume");
        tick();
        s_axis_tvalid_i = 1'b0;

        consume_expect(8'h22);
        consume_expect(8'h33);
        consume_expect(8'h44);
        consume_expect(8'h55);
        check(!m_axis_tvalid_o, "m_axis_tvalid_o should drop after last word");

        repeat (4) tick();
        if (errors == 0) begin
            $display("axis_bram_fifo_tb PASS");
        end else begin
            $display("axis_bram_fifo_tb FAIL: errors=%0d", errors);
        end
        $finish;
    end
endmodule
