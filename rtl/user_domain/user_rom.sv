// Copyright 2023 ETH Zurich and University of Bologna.
// Solderpad Hardware License, Version 0.51, see LICENSE for details.
// SPDX-License-Identifier: SHL-0.51

// gives us the `FF(...) macro making it easy to have properly defined flip-flops
`include "common_cells/registers.svh"

// simple ROM
module user_rom #(
  /// The OBI configuration for all ports.
  parameter obi_pkg::obi_cfg_t           ObiCfg      = obi_pkg::ObiDefaultConfig,
  /// The request struct.
  parameter type                         obi_req_t   = logic,
  /// The response struct.
  parameter type                         obi_rsp_t   = logic
) (
  /// Clock
  input  logic clk_i,
  /// Active-low reset
  input  logic rst_ni,

  /// OBI request interface
  input  obi_req_t obi_req_i,
  /// OBI response interface
  output obi_rsp_t obi_rsp_o
);

  // Define some registers to hold the requests fields
  logic req_d, req1_d, req_q, req1_q; // Request valid
  logic we_d, we1_d, we_q, we1_q; // Write enable
  logic [ObiCfg.AddrWidth-1:0] addr_d, addr1_d, addr_q, addr1_q; // Internal address of the word to read
  logic [ObiCfg.IdWidth-1:0] id_d, id1_d, id_q, id1_q; // Id of the request, must be same for the response

  // Signals used to create the response
  logic [ObiCfg.DataWidth-1:0] rsp_data; // Data field of the obi response
  logic rsp_err; // Error field of the obi response

  // Wire the registers holding the request
  assign req_d = obi_req_i.req;
  assign id_d = obi_req_i.a.aid;
  assign we_d = obi_req_i.a.we;
  assign addr_d = obi_req_i.a.addr;

  always_ff @(posedge (clk_i) or negedge (rst_ni)) begin
    if (!rst_ni) begin
      req_q <= '0;
      id_q <= '0;
      we_q <= '0;
      addr_q <= '0;
    end else begin
      req_q <= req_d;
      id_q <= id_d;
      we_q <= we_d;
      addr_q <= addr_d;
    end
  end

  assign req1_d = req_q;
  assign id1_d = id_q;
  assign we1_d = we_q;
  assign addr1_d = addr_q; 

  always_ff @(posedge (clk_i) or negedge (rst_ni)) begin
    if (!rst_ni) begin
      req1_q <= '0;
      id1_q <= '0;
      we1_q <= '0;
      addr1_q <= '0;
    end else begin
      req1_q <= req1_d;
      id1_q <= id1_d;
      we1_q <= we1_d;
      addr1_q <= addr1_d;
    end
  end 

  // Assign the response data
  logic [2:0] word_addr;
  always_comb begin
    rsp_data = '0;
    rsp_err  = '0;
    word_addr = addr1_q[4:2];

    if(req1_q) begin
      if(~we1_q) begin
        case(word_addr)
          2'h0: rsp_data = 32'h4926_434e;   // Signature of chip   
          2'h1: rsp_data = 32'h2073_2748;   
          2'h2: rsp_data = 32'h4349_5341;
          default: rsp_data = 32'h0;
        endcase
      end else begin
        rsp_err = '1;
      end
    end
  end

  // Wire the response
  // A channel
  assign obi_rsp_o.gnt = obi_req_i.req;
  // R channel:
  assign obi_rsp_o.rvalid = req1_q;
  assign obi_rsp_o.r.rdata = rsp_data;
  assign obi_rsp_o.r.rid = id1_q;
  assign obi_rsp_o.r.err = rsp_err;
  assign obi_rsp_o.r.r_optional = '0;

endmodule
