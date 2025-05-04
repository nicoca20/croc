// Copyright 2025 ETH Zurich and University of Bologna.
// Solderpad Hardware License, Version 0.51, see LICENSE for details.
// SPDX-License-Identifier: SHL-0.51
//
// Authors:
// - Nico Canzani <ncanzani@student.ethz.ch>

// gives us the `FF(...) macro making it easy to have properly defined flip-flops

`include "common_cells/registers.svh"

module user_pulser (
  input  logic            clk_i,
  input  logic            rst_ni,

  input  logic            start_i,
  input  logic            stop_i,
  input  logic [7:0]      f1_cnt_i,
  input  logic [7:0]      f2_cnt_i,
  input  logic [7:0]      stop_cnt_i,
  input  logic [15:0]     f1_end_i,
  input  logic [15:0]     f1_switch_i,
  input  logic [15:0]     f2_end_i,
  input  logic [15:0]     f2_switch_i,

  output logic            pulse_o,
  output logic [2:0]      state_o
);

  typedef enum logic [2:0] {
  IDLE,
  RUN_F1,
  RUN_F2,
  RUN_STOP,
  DONE
  } state_t;

  state_t state, next_state;
  assign state_o = state;

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

  assign rst_counter = pulse_done | stop_i | !rst_ni | !counter_enable;

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
  
  if (stop_i) begin
    next_state = IDLE;
  end else begin
    
    case (state)
    IDLE: begin
      if (start_i) begin
      if (f1_cnt_i > 0 && f1_end_i > 0) begin
        next_state = RUN_F1;
      end else if (f2_cnt_i > 0 && f2_end_i > 0) begin
        next_state = RUN_F2;
      end else if (stop_cnt_i > 0) begin
        next_state = RUN_STOP;
      end else begin
        next_state = DONE;
      end
      end
    end

    RUN_F1: begin
      if (pulse_counter_done) begin
      if (f2_cnt_i > 0 && f2_end_i > 0) begin
        next_state = RUN_F2;
      end else if (stop_cnt_i > 0) begin
        next_state = RUN_STOP;
      end else begin
        next_state = DONE;
      end
      end
    end

    RUN_F2: begin
      if (pulse_counter_done) begin
      if (stop_cnt_i > 0) begin
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
    current_end             = f1_end_i;
    current_switch          = f1_switch_i;
    current_count_target    = f1_cnt_i;
    end
    RUN_F2: begin
    current_end             = f2_end_i;
    current_switch          = f2_switch_i;
    current_count_target    = f2_cnt_i;
    end
    RUN_STOP: begin
    current_end             = (f2_cnt_i > 0) ? f2_end_i     : f1_end_i;
    current_switch          = (f2_cnt_i > 0) ? f2_switch_i  : f1_switch_i;
    current_count_target    = stop_cnt_i;
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
  pulse_o = 1'd0;
  case (state)
    RUN_F1: begin
    if (next_state != RUN_STOP && next_state != DONE) begin
      pulse_o = clk_count < current_switch;
    end
    end
    RUN_F2: begin
    if (next_state != RUN_STOP && next_state != DONE) begin
      pulse_o = clk_count < current_switch;
    end
    end
    RUN_STOP: begin
    pulse_o = ~(clk_count < current_switch);
    end
  endcase
  end

endmodule
