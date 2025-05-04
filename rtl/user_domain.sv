// Copyright 2024 ETH Zurich and University of Bologna.
// Solderpad Hardware License, Version 0.51, see LICENSE for details.
// SPDX-License-Identifier: SHL-0.51
//
// Authors:
// - Philippe Sauter <phsauter@iis.ee.ethz.ch>
// - Nico Canzani <ncanzani@student.ethz.ch>

module user_domain import user_pkg::*; import croc_pkg::*; #(
  parameter int unsigned GpioCount = 16
) (
  input  logic      clk_i,
  input  logic      ref_clk_i,
  input  logic      rst_ni,
  input  logic      testmode_i,
  
  input  sbr_obi_req_t user_sbr_obi_req_i, // User Sbr (rsp_o), Croc Mgr (req_i)
  output sbr_obi_rsp_t user_sbr_obi_rsp_o,

  output mgr_obi_req_t user_mgr_obi_req_o, // User Mgr (req_o), Croc Sbr (rsp_i)
  input  mgr_obi_rsp_t user_mgr_obi_rsp_i,

  input  logic [      GpioCount-1:0] gpio_in_sync_i, // synchronized GPIO inputs
  output logic [NumExternalIrqs-1:0] interrupts_o // interrupts to core
);

  assign interrupts_o = '0;  


  //////////////////////
  // User Manager MUX //
  /////////////////////

  // No manager so we don't need a obi_mux module and just terminate the request properly
  assign user_mgr_obi_req_o = '0;


  ////////////////////////////
  // User Subordinate DEMUX //
  ////////////////////////////

  // ----------------------------------------------------------------------------------------------
  // User Subordinate Buses
  // ----------------------------------------------------------------------------------------------
  
  // collection of signals from the demultiplexer
  sbr_obi_req_t [NumDemuxSbr-1:0] all_user_sbr_obi_req;
  sbr_obi_rsp_t [NumDemuxSbr-1:0] all_user_sbr_obi_rsp;

  // Error Subordinate Bus
  // sbr_obi_req_t user_error_obi_req;
  // sbr_obi_rsp_t user_error_obi_rsp;

  // Advanced Timer Subordinate Bus
  sbr_obi_req_t adv_timer_obi_req;
  sbr_obi_rsp_t adv_timer_obi_rsp;

  // Pulser Subordinate Bus
  sbr_obi_req_t user_pulser_obi_req;
  sbr_obi_rsp_t user_pulser_obi_rsp;

  // Fanout into more readable signals
  // assign user_error_obi_req              = all_user_sbr_obi_req[UserError];
  // assign all_user_sbr_obi_rsp[UserError] = user_error_obi_rsp;
  assign adv_timer_obi_req           = all_user_sbr_obi_req[UserAdvTimer];
  assign all_user_sbr_obi_rsp[UserAdvTimer] = adv_timer_obi_rsp;
  assign user_pulser_obi_req           = all_user_sbr_obi_req[UserPulser];
  assign all_user_sbr_obi_rsp[UserPulser] = user_pulser_obi_rsp;


  //-----------------------------------------------------------------------------------------------
  // Demultiplex to User Subordinates according to address map
  //-----------------------------------------------------------------------------------------------

  logic [cf_math_pkg::idx_width(NumDemuxSbr)-1:0] user_idx;

  addr_decode #(
    .NoIndices ( NumDemuxSbr                    ),
    .NoRules   ( NumDemuxSbrRules               ),
    .addr_t    ( logic[SbrObiCfg.DataWidth-1:0] ),
    .rule_t    ( addr_map_rule_t                ),
    .Napot     ( 1'b0                           )
  ) i_addr_decode_periphs (
    .addr_i           ( user_sbr_obi_req_i.a.addr ),
    .addr_map_i       ( user_addr_map             ),
    .idx_o            ( user_idx                  ),
    .dec_valid_o      (),
    .dec_error_o      (),
    .en_default_idx_i ( 1'b1 ),
    .default_idx_i    ( '0   )
  );

  obi_demux #(
    .ObiCfg      ( SbrObiCfg     ),
    .obi_req_t   ( sbr_obi_req_t ),
    .obi_rsp_t   ( sbr_obi_rsp_t ),
    .NumMgrPorts ( NumDemuxSbr   ),
    .NumMaxTrans ( 2             )
  ) i_obi_demux (
    .clk_i,
    .rst_ni,

    .sbr_port_select_i ( user_idx             ),
    .sbr_port_req_i    ( user_sbr_obi_req_i   ),
    .sbr_port_rsp_o    ( user_sbr_obi_rsp_o   ),

    .mgr_ports_req_o   ( all_user_sbr_obi_req ),
    .mgr_ports_rsp_i   ( all_user_sbr_obi_rsp )
  );


//-------------------------------------------------------------------------------------------------
// User Subordinates
//-------------------------------------------------------------------------------------------------

  // Error Subordinate
  // obi_err_sbr #(
  //   .ObiCfg      ( SbrObiCfg     ),
  //   .obi_req_t   ( sbr_obi_req_t ),
  //   .obi_rsp_t   ( sbr_obi_rsp_t ),
  //   .NumMaxTrans ( 1             ),
  //   .RspData     ( 32'hBADCAB1E  )
  // ) i_user_err (
  //   .clk_i,
  //   .rst_ni,
  //   .testmode_i ( testmode_i      ),
  //   .obi_req_i  ( user_error_obi_req ),
  //   .obi_rsp_o  ( user_error_obi_rsp )
  // );

  // adv_timer Subordinate
  reg_req_t adv_timer_reg_req;
  reg_rsp_t adv_timer_reg_rsp;

  periph_to_reg #(
    .AW    ( SbrObiCfg.AddrWidth  ),
    .DW    ( SbrObiCfg.DataWidth  ),
    .BW    ( 8                    ),
    .IW    ( SbrObiCfg.IdWidth    ),
    .req_t ( reg_req_t            ),
    .rsp_t ( reg_rsp_t            )
  ) i_adv_timer_translate (
    .clk_i     ( clk_i                      ),
    .rst_ni    ( rst_ni                     ),

    .req_i     ( adv_timer_obi_req.req      ),
    .add_i     ( adv_timer_obi_req.a.addr   ),
    .wen_i     ( ~adv_timer_obi_req.a.we    ),
    .wdata_i   ( adv_timer_obi_req.a.wdata  ),
    .be_i      ( adv_timer_obi_req.a.be     ),
    .id_i      ( adv_timer_obi_req.a.aid    ),

    .gnt_o     ( adv_timer_obi_rsp.gnt      ),
    .r_rdata_o ( adv_timer_obi_rsp.r.rdata  ),
    .r_opc_o   ( adv_timer_obi_rsp.r.err    ),
    .r_id_o    ( adv_timer_obi_rsp.r.rid    ),
    .r_valid_o ( adv_timer_obi_rsp.rvalid   ),

    .reg_req_o ( adv_timer_reg_req          ),
    .reg_rsp_i ( adv_timer_reg_rsp          )
  );

  reg_adv_timer_wrap #(
    .AddrWidth        ( 32          ),
    .reg_req_t        ( reg_req_t   ),
    .reg_rsp_t        ( reg_rsp_t   )
  ) i_adv_timer_wrap (
    .clk_i            ( clk_i             ),
    .rst_ni           ( rst_ni            ),

    .reg_req_i        ( adv_timer_reg_req ),
    .reg_rsp_o        ( adv_timer_reg_rsp ),

    .dft_cg_enable_i  ( 1'b0              ),
    .low_speed_clk_i  ( ref_clk_i         ),
    .ext_sig_i        (                   ),
    .events_o         (                   ),
    .ch_0_o           (                   ),
    .ch_1_o           (                   ),
    .ch_2_o           (                   ),
    .ch_3_o           (                   )
  );

  // Pulser Subordinate
    user_pulser_wrapper #(
    .ObiCfg                 ( SbrObiCfg     ),
    .obi_req_t              ( sbr_obi_req_t ),
    .obi_rsp_t              ( sbr_obi_rsp_t ),
    .N_PULSER_INST          ( 4             ),
    .PULSER_SEL_ADDR_WIDTH  ( 2             )
    ) i_user_pulser (
    .clk_i        ( clk_i               ),
    .rst_ni       ( rst_ni              ),
    .obi_req_i    ( user_pulser_obi_req ),
    .obi_rsp_o    ( user_pulser_obi_rsp ),
    .pulse_o      (  )
  );

endmodule
