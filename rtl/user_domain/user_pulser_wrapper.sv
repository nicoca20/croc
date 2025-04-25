// gives us the `FF(...) macro making it easy to have properly defined flip-flops
`include "common_cells/registers.svh"

module user_pulser_wrapper #(
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

  // Internal registers for handling OBI requests
  logic req_d, req_q;
  logic we_d, we_q;
  logic rvalid_d, rvalid_q;
  logic [ObiCfg.AddrWidth-1:0] addr_d, addr_q;
  logic [ObiCfg.IdWidth-1:0] id_d, id_q;
  logic [ObiCfg.DataWidth-1:0] wdata_d, wdata_q;

  // Signals used to create the response
  logic [ObiCfg.DataWidth-1:0] resp_data;
  logic rsp_err; // Error field of the OBI response

  // Internal signal registers for pulser control
  logic [15:0]  f1_end_reg, f1_high_reg;
  logic [15:0]  f2_end_reg, f2_high_reg;
  logic [7:0]   f1_count_reg, f2_count_reg, stop_count_reg;

  // State from pulser
  logic [2:0]  pulser_state;
  logic        pulse_out;
  logic        ready_status;

  // Control FSM pulse generation for "start"
  logic prev_start;
  logic start_pulse, stop_pulse;

  // Internal signals
  logic [7:0] addr_obi;

  // Note to avoid writing trivial always_ff statements we can use this macro defined in registers.svh
  `FF(req_q, req_d, '0);
  `FF(id_q , id_d , '0);
  `FF(we_q , we_d , '0);
  `FF(wdata_q , wdata_d , '0);
  `FF(addr_q , addr_d , '0);
  
  // Wire request
  assign req_d = obi_req_i.req;
  assign id_d = obi_req_i.a.aid;
  assign we_d = obi_req_i.a.we;
  assign addr_d = obi_req_i.a.addr;
  assign wdata_d = obi_req_i.a.wdata;
  
  // Wire  response
  // A channel
  assign obi_rsp_o.gnt = obi_req_i.req;
  // R channel:
  assign obi_rsp_o.rvalid = req_q;
  assign obi_rsp_o.r.rdata = resp_data;
  assign obi_rsp_o.r.rid = id_q;
  assign obi_rsp_o.r.err = rsp_err;
  assign obi_rsp_o.r.r_optional = '0;

  assign addr_obi = addr_q[7:0];

  // Start and stop should be triggered once and not stored in a reg
  always_comb begin
    start_pulse = 0;
    stop_pulse = 0;
    if (req_q && we_q && addr_obi ==8'h00) begin
      start_pulse = wdata_q[0];
      stop_pulse = wdata_q[1];
    end
  end

  // OBI write interface
  always_ff @(posedge clk_i or negedge rst_ni) begin
    if (!rst_ni) begin
      f1_end_reg      <= 16'd0;
      f1_high_reg     <= 16'd0;
      f2_end_reg      <= 16'd0;
      f2_high_reg     <= 16'd0;
      f1_count_reg    <= 8'd0;
      f2_count_reg    <= 8'd0;
      stop_count_reg  <= 8'd0;
    end else if (req_q && we_q) begin
      case (addr_obi)
        8'h00: begin
          // start_pulse     <= wdata_q[0];
          // stop_pulse      <= wdata_q[1];
          // Not saved in register but only a pulse. See always_comb above.
        end
        8'h04: begin
          f1_high_reg     <= wdata_q[15:0];
          f1_end_reg      <= wdata_q[31:16];
        end
        8'h08: begin
          f2_high_reg     <= wdata_q[15:0];
          f2_end_reg      <= wdata_q[31:16];
        end
        8'h0C: begin
          f1_count_reg    <= wdata_q[7:0];
          f2_count_reg    <= wdata_q[15:8];
          stop_count_reg  <= wdata_q[23:16];
        end
        default: begin
          // No valid register selected — optional: add error flag or log
          // For now, do nothing to avoid unintended writes
        end
      endcase
    end
  end

  // Instantiate pulser with local signals
  user_pulser i_pulser (
    .clk_i      (clk_i),
    .rst_ni     (rst_ni),
    .start      (start_pulse),
    .stop       (stop_pulse),
    .f1_count   (f1_count_reg),
    .f2_count   (f2_count_reg),
    .stop_count (stop_count_reg),
    .f1_end     (f1_end_reg),
    .f1_high    (f1_high_reg),
    .f2_end     (f2_end_reg),
    .f2_high    (f2_high_reg),
    .pulse_out  (pulse_out),
    .state_out  (pulser_state)
  );

  // Define done state (must match enum in pulser)
  localparam IDLE_STATE = 3'd0;
  localparam DONE_STATE = 3'd4;
  assign ready_status = (pulser_state == IDLE_STATE) || (pulser_state == DONE_STATE);

  // OBI read interface
  always_comb begin
    resp_data  = 32'd0;
    if (req_q && !we_q) begin
      case (addr_obi)
        8'h00: resp_data = 32'd0;
        8'h04: resp_data = {f1_end_reg, f1_high_reg};
        8'h08: resp_data = {f2_end_reg, f2_high_reg};
        8'h0C: resp_data = {8'd0, stop_count_reg, f2_count_reg, f1_count_reg};
        8'h10: resp_data = {28'd0, pulser_state, ready_status};
        default: resp_data = 32'hDEADBEEF;
      endcase
    end
  end

endmodule
