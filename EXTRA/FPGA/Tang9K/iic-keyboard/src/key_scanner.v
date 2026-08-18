module key_scanner #(
    parameter CLK_FREQ_HZ      = 27_000_000,
    parameter SCAN_FREQ_HZ     = 2000,
    parameter DEBOUNCE_TIME_MS = 20
) (
    input  wire        clk,
    input  wire        rst_n,
    output wire [2:0]  col_out,
    input  wire [2:0]  row_in,
    output wire [15:0] key_data,
    output wire        key_valid
);

    localparam COL_NUM       = 3;
    localparam ROW_NUM       = 3;
    localparam SCAN_CNT_MAX  = CLK_FREQ_HZ / SCAN_FREQ_HZ / COL_NUM;

    // 消抖：需要连续多少轮扫描值不变才认
    // 每轮矩阵扫描 = 1 / SCAN_FREQ 秒 = 0.5ms
    // 20ms 消抖 = 40 轮
    localparam DEBOUNCE_SCANS = (DEBOUNCE_TIME_MS * SCAN_FREQ_HZ) / 1000;
    localparam DEBOUNCE_W     = $clog2(DEBOUNCE_SCANS + 1);

    // ── 列扫描 ──────────────────────────────────────────────────────────
    reg  [ 1:0] col_cnt;
    reg  [25:0] scan_cnt;
    wire        col_tick = (scan_cnt == SCAN_CNT_MAX - 1);

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            col_cnt  <= 2'd0;
            scan_cnt <= 26'd0;
        end else if (col_tick) begin
            col_cnt  <= (col_cnt == COL_NUM - 1) ? 2'd0 : col_cnt + 2'd1;
            scan_cnt <= 26'd0;
        end else begin
            scan_cnt <= scan_cnt + 26'd1;
        end
    end

    // col_out: 低有效，只有当前列拉低
    assign col_out = ~(3'b001 << col_cnt);

    // ── 消抖计数器（数扫描轮数，不是时钟数） ───────────────────────────
    reg [DEBOUNCE_W-1:0] debounce_cnt;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            debounce_cnt <= {DEBOUNCE_W{1'b0}};
        end else if (col_tick && col_cnt == COL_NUM - 1) begin
            // 每轮扫描结束（col 扫完一圈），检查变化
            if (raw_keys != stable_keys)
                debounce_cnt <= {DEBOUNCE_W{1'b0}};
            else if (debounce_cnt < DEBOUNCE_SCANS)
                debounce_cnt <= debounce_cnt + 1'b1;
        end
    end

    // ── 行采样 + 原始键值 ──────────────────────────────────────────────
    // 在每列扫描快到结束时采样 row_in（给按键信号充足稳定时间）
    reg [2:0] row_sample;
    reg [8:0] raw_keys;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            row_sample <= 3'b111;
            raw_keys   <= 9'h1FF;
        end else begin
            // 在计数器过半时采样
            if (scan_cnt == (SCAN_CNT_MAX / 2))
                row_sample <= row_in;

            // 每列扫完，写入对应位置的 raw_keys
            if (col_tick) begin
                raw_keys[col_cnt + 0] <= row_sample[0];  // row 0
                raw_keys[col_cnt + 3] <= row_sample[1];  // row 1
                raw_keys[col_cnt + 6] <= row_sample[2];  // row 2
            end
        end
    end

    // ── 消抖后的稳定键值 ───────────────────────────────────────────────
    reg [8:0] stable_keys;
    reg       key_valid_reg;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            stable_keys   <= 9'h1FF;
            key_valid_reg <= 1'b0;
        end else begin
            key_valid_reg <= 1'b0;  // 默认脉冲为 0

            if (debounce_cnt == DEBOUNCE_SCANS && raw_keys != stable_keys) begin
                stable_keys   <= raw_keys;
                key_valid_reg <= 1'b1;
            end
        end
    end

    assign key_data  = {7'd0, stable_keys};
    assign key_valid = key_valid_reg;

endmodule
