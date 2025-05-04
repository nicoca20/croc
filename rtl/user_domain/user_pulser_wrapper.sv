//-----------------------------------------------------------------------------------------------
// Copyright 2025 ETH Zurich and University of Bologna.
// Solderpad Hardware License, Version 0.51, see LICENSE for details.
// SPDX-License-Identifier: SHL-0.51
//
// Authors:
// - Nico Canzani <ncanzani@student.ethz.ch>
//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
// Include common register macros (e.g., `FF for flip-flops)
//-----------------------------------------------------------------------------------------------
`include "common_cells/registers.svh"

//-----------------------------------------------------------------------------------------------
// Pulser Register Map Definitions (relative to BASEADDR)
//-----------------------------------------------------------------------------------------------
`define REG_PULSER_CMD           8'h00  // Start/Stop command register
`define REG_PULSER_F1_CFG        8'h04  // F1 Switch and End configuration
`define REG_PULSER_F2_CFG        8'h08  // F2 Switch and End configuration
`define REG_PULSER_COUNT_CFG     8'h0C  // F1/F2/Stop count configuration
`define REG_PULSER_STATUS        8'h10  // State + Ready status

//-----------------------------------------------------------------------------------------------
// Top-level wrapper module for multiple user_pulser instances
//-----------------------------------------------------------------------------------------------
module user_pulser_wrapper #(
  parameter obi_pkg::obi_cfg_t ObiCfg               = obi_pkg::ObiDefaultConfig,
  parameter type               obi_req_t            = logic,
  parameter type               obi_rsp_t            = logic,
  parameter int                N_PULSER_INST        = 4,
  parameter int                PULSER_SEL_ADDR_WIDTH = 2
) (
  input  logic clk_i,
  input  logic rst_ni,

  input  obi_req_t obi_req_i,
  output obi_rsp_t obi_rsp_o,

  output logic [N_PULSER_INST-1:0] pulse_o
);

  //-----------------------------------------------------------------------------------------------
  // Internal OBI handshake registers for tracking request context
  //-----------------------------------------------------------------------------------------------
  logic req_d, req_q;
  logic we_d, we_q;
  logic [ObiCfg.AddrWidth-1:0] addr_d, addr_q;
  logic [ObiCfg.IdWidth-1:0]   id_d, id_q;
  logic [ObiCfg.DataWidth-1:0] wdata_d, wdata_q;

  logic [ObiCfg.DataWidth-1:0] resp_data;
  logic rsp_err;

  //-----------------------------------------------------------------------------------------------
  // Address decode signals (for register addressing and instance selection)
  //-----------------------------------------------------------------------------------------------
  logic [7:0] addr_obi;
  logic [PULSER_SEL_ADDR_WIDTH-1:0] pulser_sel;
  logic [4:0] reg_addr;

  //-----------------------------------------------------------------------------------------------
  // Per-pulser configuration registers (D/Q pairs for FFs)
  //-----------------------------------------------------------------------------------------------
  logic [N_PULSER_INST-1:0][15:0] f1_end_q, f1_end_d;
  logic [N_PULSER_INST-1:0][15:0] f1_switch_q, f1_switch_d;
  logic [N_PULSER_INST-1:0][15:0] f2_end_q, f2_end_d;
  logic [N_PULSER_INST-1:0][15:0] f2_switch_q, f2_switch_d;
  logic [N_PULSER_INST-1:0][7:0]  f1_count_q, f1_count_d;
  logic [N_PULSER_INST-1:0][7:0]  f2_count_q, f2_count_d;
  logic [N_PULSER_INST-1:0][7:0]  stop_count_q, stop_count_d;

  logic [N_PULSER_INST-1:0][2:0]  state;
  logic [N_PULSER_INST-1:0]       ready;

  logic [N_PULSER_INST-1:0]       start_pulse, stop_pulse;

  //-----------------------------------------------------------------------------------------------
  // Register updates using `FF macro for flip-flops
  //-----------------------------------------------------------------------------------------------
  `FF(req_q, req_d, '0);
  `FF(id_q , id_d , '0);
  `FF(we_q , we_d , '0);
  `FF(wdata_q , wdata_d , '0);
  `FF(addr_q , addr_d , '0);

  `FF(f1_end_q     , f1_end_d     , '0)
  `FF(f1_switch_q  , f1_switch_d  , '0)
  `FF(f2_end_q     , f2_end_d     , '0)
  `FF(f2_switch_q  , f2_switch_d  , '0)
  `FF(f1_count_q   , f1_count_d   , '0)
  `FF(f2_count_q   , f2_count_d   , '0)
  `FF(stop_count_q , stop_count_d , '0)

  //-----------------------------------------------------------------------------------------------
  // Capture OBI request on input
  //-----------------------------------------------------------------------------------------------
  assign req_d    = obi_req_i.req;
  assign id_d     = obi_req_i.a.aid;
  assign we_d     = obi_req_i.a.we;
  assign addr_d   = obi_req_i.a.addr;
  assign wdata_d  = obi_req_i.a.wdata;

  //-----------------------------------------------------------------------------------------------
  // OBI response logic
  //-----------------------------------------------------------------------------------------------
  assign obi_rsp_o.gnt          = obi_req_i.req;
  assign obi_rsp_o.rvalid       = req_q;
  assign obi_rsp_o.r.rdata      = resp_data;
  assign obi_rsp_o.r.rid        = id_q;
  assign obi_rsp_o.r.err        = rsp_err;
  assign obi_rsp_o.r.r_optional = '0;

  //-----------------------------------------------------------------------------------------------
  // Address decoding (supports multiple pulsers via upper bits)
  //-----------------------------------------------------------------------------------------------
  assign addr_obi   = addr_q[7:0];
  assign pulser_sel = addr_obi[5 + PULSER_SEL_ADDR_WIDTH - 1 : 5];
  assign reg_addr   = addr_obi[4:0];

  //-----------------------------------------------------------------------------------------------
  // Command register decoding: generate start/stop pulses for instances
  //-----------------------------------------------------------------------------------------------
  always_comb begin
    start_pulse = '0;
    stop_pulse  = '0;

    if (req_q && we_q && addr_obi == 5'h00) begin
      start_pulse = wdata_q[N_PULSER_INST-1:0];
      stop_pulse  = wdata_q[2*N_PULSER_INST-1 : N_PULSER_INST];
    end
  end

  //-----------------------------------------------------------------------------------------------
  // Write logic for configuration registers
  //-----------------------------------------------------------------------------------------------
  always_comb begin
    f1_end_d      = f1_end_q;
    f1_switch_d   = f1_switch_q;
    f2_end_d      = f2_end_q;
    f2_switch_d   = f2_switch_q;
    f1_count_d    = f1_count_q;
    f2_count_d    = f2_count_q;
    stop_count_d  = stop_count_q;

    if (req_q && we_q) begin
      case (reg_addr)
        `REG_PULSER_F1_CFG: begin
          f1_switch_d[pulser_sel] = wdata_q[15:0];
          f1_end_d[pulser_sel]    = wdata_q[31:16];
        end
        `REG_PULSER_F2_CFG: begin
          f2_switch_d[pulser_sel] = wdata_q[15:0];
          f2_end_d[pulser_sel]    = wdata_q[31:16];
        end
        `REG_PULSER_COUNT_CFG: begin
          f1_count_d[pulser_sel]   = wdata_q[7:0];
          f2_count_d[pulser_sel]   = wdata_q[15:8];
          stop_count_d[pulser_sel] = wdata_q[23:16];
        end
        default: ;
      endcase
    end
  end

  //-----------------------------------------------------------------------------------------------
  // Pulser instantiations
  //-----------------------------------------------------------------------------------------------
  for (genvar ii = 0; ii < N_PULSER_INST; ii++) begin : gen_pulsers
    user_pulser i_pulser (
      .clk_i        (clk_i),
      .rst_ni       (rst_ni),
      .start_i      (start_pulse[ii]),
      .stop_i       (stop_pulse[ii]),
      .f1_cnt_i     (f1_count_q[ii]),
      .f2_cnt_i     (f2_count_q[ii]),
      .stop_cnt_i   (stop_count_q[ii]),
      .f1_end_i     (f1_end_q[ii]),
      .f1_switch_i  (f1_switch_q[ii]),
      .f2_end_i     (f2_end_q[ii]),
      .f2_switch_i  (f2_switch_q[ii]),
      .pulse_o      (pulse_o[ii]),
      .state_o      (state[ii])
    );
  end

  //-----------------------------------------------------------------------------------------------
  // READY signal: high when pulser is in IDLE or DONE state
  //-----------------------------------------------------------------------------------------------
  localparam IDLE_STATE = 3'd0;
  localparam DONE_STATE = 3'd4;

  always_comb begin
    for (int i = 0; i < N_PULSER_INST; i++) begin
      ready[i] = (state[i] == IDLE_STATE) || (state[i] == DONE_STATE);
    end
  end

  //-----------------------------------------------------------------------------------------------
  // Read logic for returning register values
  //-----------------------------------------------------------------------------------------------
  always_comb begin
    resp_data = 32'd0;
    if (req_q && !we_q) begin
      case (reg_addr)
        `REG_PULSER_CMD:        resp_data = 32'd0;
        `REG_PULSER_F1_CFG:     resp_data = {f1_end_q[pulser_sel], f1_switch_q[pulser_sel]};
        `REG_PULSER_F2_CFG:     resp_data = {f2_end_q[pulser_sel], f2_switch_q[pulser_sel]};
        `REG_PULSER_COUNT_CFG:  resp_data = {
          8'd0,
          stop_count_q[pulser_sel],
          f2_count_q[pulser_sel],
          f1_count_q[pulser_sel]
        };
        `REG_PULSER_STATUS:     resp_data = {
          28'd0,
          state[pulser_sel],
          ready[pulser_sel]
        };
        default:                resp_data = 32'hDEADBEEF;
      endcase
    end
  end

endmodule
