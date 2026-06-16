module pwm #(
    parameter integer CLOCK_HZ     = 27_000_000,
    parameter integer PWM_HZ       = 1_000,
    parameter integer PERIOD_COUNT = CLOCK_HZ / PWM_HZ,
    parameter integer COUNT_WIDTH  = $clog2(PERIOD_COUNT + 1)
) (
    input  wire                   clk,
    input  wire                   rst_n,

    input  wire [COUNT_WIDTH-1:0] duty_i,
    output wire                   pwm_o
);

    reg [COUNT_WIDTH-1:0] cnt;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            cnt <= {COUNT_WIDTH{1'b0}};
        end else begin
            if (cnt == PERIOD_COUNT - 1) begin
                cnt <= {COUNT_WIDTH{1'b0}};
            end else begin
                cnt <= cnt + 1'b1;
            end
        end
    end

    assign pwm_o = (cnt < duty_i);
endmodule
