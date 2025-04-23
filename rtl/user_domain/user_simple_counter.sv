module user_simple_counter (
    input  logic         clk_i,
    input  logic         rst_ni,
    input  logic         enable,
    input  logic [15:0]  end_val,
    output logic [15:0]  count,
    output logic         done
);

    always_ff @(posedge clk_i or negedge rst_ni) begin
        if (!rst_ni) begin
            count <= 16'd0;
        end else if (!enable) begin
            count <= 16'd0;
        end else begin
            if (count == end_val - 1) begin
                count <= 16'd0;
            end else begin
                count <= count + 1;
            end
        end
    end

    assign done = (count == end_val - 1);

endmodule
