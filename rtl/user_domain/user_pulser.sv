// Copyright 2025 ETH Zurich and University of Bologna.
// Solderpad Hardware License, Version 0.51, see LICENSE for details.
// SPDX-License-Identifier: SHL-0.51
//
// Authors:
// - Nico Canzani <ncanzani@student.ethz.ch>

// Gives us the `FF(...)` macro for properly defined flip-flops
`include "common_cells/registers.svh"

module user_pulser (
  input  logic        clk_i,
  input  logic        rst_ni,

  input  logic        start_i,
  input  logic        stop_i,
  input  logic [7:0]  f1_cnt_i,
  input  logic [7:0]  f2_cnt_i,
  input  logic [7:0]  stop_cnt_i,
  input  logic [15:0] f1_end_i,
  input  logic [15:0] f1_switch_i,
  input  logic [15:0] f2_end_i,
  input  logic [15:0] f2_switch_i,

  output logic        pulse_o,
  output logic [2:0]  state_o
);

  typedef enum logic [2:0] {
    IDLE,
    RUN_F1,
    RUN_F2,
    RUN_STOP,
    DONE
  } state_e;

  state_e state_d, state_q;

  // Pulse counter
  logic       pulse_counter_done;
  logic [7:0] pulse_cnt_d, pulse_cnt_q;

  logic [7:0] current_count_target;

  // Clock cycle control
  logic [15:0] current_end;
  logic [15:0] current_switch;
  logic [15:0] clk_count;
  logic        pulse_done;
  logic        counter_enable;
  logic        rst_counter;

  `FF(state_q , state_d , IDLE);
  `FF(pulse_cnt_q, pulse_cnt_d , '0);

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

  assign state_o = state_q;

  assign rst_counter = pulse_done | stop_i | !counter_enable;
  assign pulse_done  = clk_count == (current_end - 1);

  always_comb begin
    state_d = state_q;

    if (stop_i) begin
      state_d = IDLE;
    end else begin
      case (state_q)
        IDLE: begin
          if (start_i) begin
            if (f1_cnt_i > 0 && f1_end_i > 0) begin
              state_d = RUN_F1;
            end else if (f2_cnt_i > 0 && f2_end_i > 0) begin
              state_d = RUN_F2;
            end else if (stop_cnt_i > 0) begin
              state_d = RUN_STOP;
            end else begin
              state_d = DONE;
            end
          end
        end

        RUN_F1: begin
          if (pulse_counter_done) begin
            if (f2_cnt_i > 0 && f2_end_i > 0) begin
              state_d = RUN_F2;
            end else if (stop_cnt_i > 0) begin
              state_d = RUN_STOP;
            end else begin
              state_d = DONE;
            end
          end
        end

        RUN_F2: begin
          if (pulse_counter_done) begin
            if (stop_cnt_i > 0) begin
              state_d = RUN_STOP;
            end else begin
              state_d = DONE;
            end
          end
        end

        RUN_STOP: begin
          if (pulse_counter_done) begin
            state_d = DONE;
          end
        end

        DONE: begin
          state_d = IDLE;
        end

        default: begin
          state_d = IDLE;
        end
      endcase
    end
  end

  // Pulse counter logic
  always_comb begin
    pulse_cnt_d = pulse_cnt_q;

    if (state_q != state_d) begin
      pulse_cnt_d = 8'd0;
    end else if (pulse_done) begin
      pulse_cnt_d = pulse_cnt_q + 1;
    end
  end

  assign pulse_counter_done = (pulse_cnt_q == current_count_target);
  assign counter_enable     = (state_q == RUN_F1 || state_q == RUN_F2 || state_q == RUN_STOP);

  // Dynamic assignment of current_end and switch
  always_comb begin
    current_end          = 16'd0;
    current_switch       = 16'd0;
    current_count_target = 8'd0;

    case (state_q)
      RUN_F1: begin
        current_end          = f1_end_i;
        current_switch       = f1_switch_i;
        current_count_target = f1_cnt_i;
      end
      RUN_F2: begin
        current_end          = f2_end_i;
        current_switch       = f2_switch_i;
        current_count_target = f2_cnt_i;
      end
      RUN_STOP: begin
        current_end          = (f2_cnt_i > 0) ? f2_end_i    : f1_end_i;
        current_switch       = (f2_cnt_i > 0) ? f2_switch_i : f1_switch_i;
        current_count_target = stop_cnt_i;
      end
      default: begin
        current_end          = 16'd0;
        current_switch       = 16'd0;
        current_count_target = 8'd0;
      end
    endcase
  end

  // Pulse output logic
  always_comb begin
    pulse_o = 1'd0;

    case (state_q)
      RUN_F1, RUN_F2: begin
        if (state_d != RUN_STOP && state_d != DONE) begin
          pulse_o = clk_count < current_switch;
        end
      end
      RUN_STOP: begin
        pulse_o = ~(clk_count < current_switch);
      end
      default: begin
        pulse_o = '0;
      end
    endcase
  end

endmodule
