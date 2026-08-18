module iic_slave #(
    parameter [6:0] SLAVE_ADDR = 7'h5A
) (
    input  wire       clk,          // system clock
    input  wire       rst_n,
    input  wire       scl_in,
    input  wire       sda_in,
    output wire       sda_out,
    output wire       sda_oe,
    input  wire [7:0] key_data,
    input  wire       key_valid,
    output wire [7:0] reg_data,
    output wire       reg_written,
    output wire       int_n
);

    // ===================== registers and wires =====================
    reg  scl_unstable;  // scl1 is used to store the unstable vlaue of input
    reg  scl_stable;  // scl2 scl is used to store the previous value of scl_unstable
    reg  sda_s1;  // sda1 is used to store the current value of sda_in;
    reg  sda_s2;  // sda2 is used to store the previous value of sda_s1
    reg  scl_prev;  // the scl_prev reg is used to store the last stable vlaue of scl
    // =========================== wires =============================
    // if last vlaue of scl is 0 and current value of scl is 1, then it is a rising edge
    wire scl_rise = (scl_prev == 1'b0) && (scl_stable == 1'b1);
    // if last value of scl is 1 and current value of scl is 0, then it is a falling edge
    wire scl_fall = (scl_prev == 1'b1) && (scl_stable == 1'b0);
    // ======================== State Machine ========================
    reg [2:0] state, state_next;
    localparam [2:0] ST_IDLE = 3'd0;
    localparam [2:0] ST_ADDR = 3'd1;
    localparam [2:0] ST_ACK = 3'd2;
    localparam [2:0] ST_DATA_WR = 3'd3;
    localparam [2:0] ST_DATA_RD = 3'd4;
    localparam [2:0] ST_ACK_WAIT = 3'd5;

    wire start_det = (scl_stable == 1'b1) && (sda_s1 == 1'b1) && (sda_s2 == 1'b0);  // => start
    wire stop_det = (scl_stable == 1'b1) && (sda_s1 == 1'b0) && (sda_s2 == 1'b1);  // => stop

    reg [7:0] shift_reg;
    reg [2:0] bit_cnt;
    wire rw_bit = shift_reg[0];
    wire addr_match = (shift_reg[7:1] == SLAVE_ADDR);
    wire master_nack = sda_s2;

    // ======================= end =======================


    /// @brief Synchronize the input signals scl_in and sda_in to the local clock domain to avoid metastability issues:
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin  // if reset, then set all the registers to 1
            scl_unstable <= 1'b1;
            scl_stable <= 1'b1;
            sda_s1 <= 1'b1;
            sda_s2 <= 1'b1;
        end else begin
            // if not reset, then update the registers with the current values of scl_in and sda_in
            scl_unstable <= scl_in;
            scl_stable <= scl_unstable;  // update scl_stable with the stable value of scl_unstable
            sda_s1 <= sda_in;
            sda_s2 <= sda_s1;  // update sda_s2 with the previous value of sda_s1
        end
    end
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            // reset logic, same.
            scl_prev <= 1'b1;
        end else begin
            // store the previous value of scl_stable using s3 for furthre coparisons
            scl_prev <= scl_stable;
        end
    end

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) state <= ST_IDLE;
        else state <= state_next;
    end

    /// @brief Implement The I2C Slave state meachine logic:
    always @(*) begin
        state_next = state;
        case (state)
            ST_IDLE: begin
                if (start_det) begin
                    state_next = ST_ADDR;
                end
            end

            ST_ADDR: begin
                if (scl_rise && bit_cnt == 3'd7) begin
                    state_next = ST_ACK;
                end else if (stop_det) begin
                    state_next = ST_IDLE;
                end
            end

            ST_ACK: begin
                if (scl_fall && addr_match && !rw_bit) begin
                    // if the address matches and Master want to write
                    state_next = ST_DATA_WR;
                end else if (scl_fall && addr_match && rw_bit) begin
                    // if the address matches and Master want to read
                    state_next = ST_DATA_RD;
                end else if (scl_fall && !addr_match) begin
                    state_next = ST_IDLE;
                end
            end

            ST_DATA_WR: begin
                if (scl_rise && bit_cnt == 3'd7) begin
                    state_next = ST_ACK;
                end else if (stop_det) begin
                    state_next = ST_IDLE;
                end
            end

            ST_DATA_RD: begin
                if (scl_rise && bit_cnt == 3'd7) begin
                    state_next = ST_ACK_WAIT;
                end else if (stop_det) begin
                    state_next = ST_IDLE;
                end
            end

            ST_ACK_WAIT: begin
                if (scl_fall && master_nack) begin
                    state_next = ST_IDLE;
                end else if (scl_fall && !master_nack) begin
                    state_next = ST_DATA_RD;
                end else if (stop_det) begin
                    state_next = ST_IDLE;
                end
            end

            default: begin
                state_next = ST_IDLE;
            end
        endcase
    end
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            bit_cnt   <= 3'd0;
            shift_reg <= 8'd0;
        end else if (scl_rise) begin
            bit_cnt   <= bit_cnt + 3'd1;
            shift_reg <= {shift_reg[6:0], sda_s2};
        end else if (state == ST_IDLE) begin
            bit_cnt   <= 3'd0;
            shift_reg <= 8'd0;
        end
    end

endmodule
