module user_pulser (
    input  logic            clk_i,
    input  logic            rst_ni,

    input  logic            start,
    input  logic            stop,
    input  logic [7:0]      f1_count,
    input  logic [7:0]      f2_count,
    input  logic [7:0]      stop_count,
    input  logic [15:0]     f1_end,
    input  logic [15:0]     f1_switch,
    input  logic [15:0]     f2_end,
    input  logic [15:0]     f2_switch,

    output logic            pulse_out,
    output logic [2:0]      state_out
);

    typedef enum logic [2:0] {
        IDLE,
        RUN_F1,
        RUN_F2,
        RUN_STOP,
        DONE
    } state_t;

    state_t state, next_state;
    assign state_out = state;

    // Pulse counters
    logic [7:0] pulse_counter;
    logic       pulse_counter_done;

    logic [7:0] current_count_target;

    // Clock cycle control
    logic [15:0] current_end;
    logic [15:0] current_switch;
    logic [15:0] clk_count;
    logic        pulse_done;
    logic        counter_enable;
    logic        rst_counter;

    counter #(
        .WIDTH($bits(clk_count)),
        .STICKY_OVERFLOW(1'b0)
    ) i_counter_common_cells (
        .clk_i      (clk_i),
        .rst_ni     (rst_ni),
        .clear_i    (1'b0),
        .en_i       (counter_enable),
        .load_i     (rst_counter),
        .down_i     (1'b0),
        .d_i        ({$bits(clk_count){1'b0}}),
        .q_o        (clk_count),
        .overflow_o ()
    );

    assign rst_counter = pulse_done | stop | !rst_ni | !counter_enable;

    assign pulse_done = clk_count == (current_end-1);

    // State machine
    always_ff @(posedge clk_i or negedge rst_ni) begin
        if (!rst_ni) begin
            state <= IDLE;
        end else begin
            state <= next_state;
        end
    end

    always_comb begin
        next_state = state;
        
        if (stop) begin
            next_state = IDLE;
        end else begin
            
            case (state)
                IDLE: begin
                    if (start) begin
                        if (f1_count > 0 && f1_end > 0) begin
                            next_state = RUN_F1;
                        end else if (f2_count > 0 && f2_end > 0) begin
                            next_state = RUN_F2;
                        end else if (stop_count > 0) begin
                            next_state = RUN_STOP;
                        end else begin
                            next_state = DONE;
                        end
                    end
                end

                RUN_F1: begin
                    if (pulse_counter_done) begin
                        if (f2_count > 0 && f2_end > 0) begin
                            next_state = RUN_F2;
                        end else if (stop_count > 0) begin
                            next_state = RUN_STOP;
                        end else begin
                            next_state = DONE;
                        end
                    end
                end

                RUN_F2: begin
                    if (pulse_counter_done) begin
                        if (stop_count > 0) begin
                            next_state = RUN_STOP;
                        end else begin
                            next_state = DONE;
                        end
                    end
                end

                RUN_STOP: begin
                    if (pulse_counter_done) begin
                        next_state = DONE;
                    end
                end

                DONE: begin
                    next_state = IDLE;
                end
            endcase
        end
    end


    // Pulse counter logic
    always_ff @(posedge clk_i or negedge rst_ni) begin
        if (!rst_ni) begin
            pulse_counter <= 8'd0;
        end else if (state != next_state) begin
            pulse_counter <= 8'd0;
        end else if (pulse_done) begin
            pulse_counter <= pulse_counter + 1;
        end
    end

    assign pulse_counter_done = (pulse_counter == current_count_target);

    // Clock counter enable during pulse states
    assign counter_enable = (state == RUN_F1 || state == RUN_F2 || state == RUN_STOP);

    // Dynamic assignment of current_end and switch
    always_comb begin
        current_end             = 16'd0;
        current_switch          = 16'd0;
        current_count_target    =  8'd0;
        case (state)
            RUN_F1: begin
                current_end             = f1_end;
                current_switch          = f1_switch;
                current_count_target    = f1_count;
            end
            RUN_F2: begin
                current_end             = f2_end;
                current_switch          = f2_switch;
                current_count_target    = f2_count;
            end
            RUN_STOP: begin
                current_end             = (f2_count > 0) ? f2_end       : f1_end;
                current_switch          = (f2_count > 0) ? f2_switch    : f1_switch;
                current_count_target    = stop_count;
            end
            default: begin
                current_end             = 16'd0;
                current_switch          = 16'd0;
                current_count_target    = 8'd0;
            end
        endcase
    end

    // Pulse output logic
    always_comb begin
        pulse_out = 1'd0;
        case (state)
            RUN_F1: begin
                if (next_state != RUN_STOP && next_state != DONE) begin
                    pulse_out = clk_count < current_switch;
                end
            end
            RUN_F2: begin
                if (next_state != RUN_STOP && next_state != DONE) begin
                    pulse_out = clk_count < current_switch;
                end
            end
            RUN_STOP: begin
                pulse_out = ~(clk_count < current_switch);
            end
        endcase
    end

endmodule
