`include "common_cells/registers.svh"

module user_pulser_wrapper #(
  parameter obi_pkg::obi_cfg_t ObiCfg    = obi_pkg::ObiDefaultConfig,
  parameter type               obi_req_t = logic,
  parameter type               obi_rsp_t = logic,
  parameter N_PULSER_INST                = 4,
  parameter PULSER_SEL_ADDR_WIDTH        = 2
) (
  input  logic clk_i,
  input  logic rst_ni,

  input  obi_req_t obi_req_i,
  output obi_rsp_t obi_rsp_o,

  output logic [N_PULSER_INST-1:0] pulse_o
);

  // Internal OBI handshake registers
  logic req_d, req_q;
  logic we_d, we_q;
  logic [ObiCfg.AddrWidth-1:0]  addr_d, addr_q;
  logic [ObiCfg.IdWidth-1:0]    id_d, id_q;
  logic [ObiCfg.DataWidth-1:0]  wdata_d, wdata_q;

  logic [ObiCfg.DataWidth-1:0]  resp_data;
  logic rsp_err;

  // Address decode
  logic [7:0]                       addr_obi;
  logic [PULSER_SEL_ADDR_WIDTH-1:0] pulser_sel;
  logic [4:0]                       reg_addr;

  // Signals per pulser (arrays of 4)
  logic [N_PULSER_INST-1:0][15:0] f1_end, f1_switch, f2_end, f2_switch;
  logic [N_PULSER_INST-1:0][7:0]  f1_count, f2_count, stop_count;
  logic [N_PULSER_INST-1:0][2:0]  state;
  logic [N_PULSER_INST-1:0]       ready;

  // Pulse control (single wires)
  logic [N_PULSER_INST-1:0]       start_pulse, stop_pulse;

  // Register updates
  `FF(req_q, req_d, '0);
  `FF(id_q , id_d , '0);
  `FF(we_q , we_d , '0);
  `FF(wdata_q , wdata_d , '0);
  `FF(addr_q , addr_d , '0);

  assign req_d    = obi_req_i.req;
  assign id_d     = obi_req_i.a.aid;
  assign we_d     = obi_req_i.a.we;
  assign addr_d   = obi_req_i.a.addr;
  assign wdata_d  = obi_req_i.a.wdata;

  assign obi_rsp_o.gnt          = obi_req_i.req;
  assign obi_rsp_o.rvalid       = req_q;
  assign obi_rsp_o.r.rdata      = resp_data;
  assign obi_rsp_o.r.rid        = id_q;
  assign obi_rsp_o.r.err        = rsp_err;
  assign obi_rsp_o.r.r_optional = '0;

  assign addr_obi    = addr_q[7:0];
  assign pulser_sel  = addr_obi[5+PULSER_SEL_ADDR_WIDTH-1:5];  // which pulser
  assign reg_addr    = addr_obi[4:0];  // offset within pulser

  // start/stop pulse generation — no register, just edge trigger
  // Important: all pulser can be started and stopped with one write at 0x00
  always_comb begin
    start_pulse = 0;
    stop_pulse  = 0;
    if (req_q && we_q && addr_obi == 5'h00) begin
      start_pulse = wdata_q[N_PULSER_INST-1:0];
      stop_pulse  = wdata_q[2*N_PULSER_INST-1:N_PULSER_INST];
    end
  end

  // Register file writes
  always_ff @(posedge clk_i or negedge rst_ni) begin
    if (!rst_ni) begin
      f1_end      <= 16'd0;
      f1_switch   <= 16'd0;
      f2_end      <= 16'd0;
      f2_switch   <= 16'd0;
      f1_count    <= 8'd0;
      f2_count    <= 8'd0;
      stop_count  <= 8'd0;
    end else if (req_q && we_q) begin
      case (reg_addr)
        5'h00: begin
          // Not saved in register but as a pulse. See always_comb above.
        end
        5'h04: begin
          f1_switch[pulser_sel] <= wdata_q[15:0];
          f1_end[pulser_sel]    <= wdata_q[31:16];
        end
        5'h08: begin
          f2_switch[pulser_sel] <= wdata_q[15:0];
          f2_end[pulser_sel]    <= wdata_q[31:16];
        end
        5'h0C: begin
          f1_count[pulser_sel]   <= wdata_q[7:0];
          f2_count[pulser_sel]   <= wdata_q[15:8];
          stop_count[pulser_sel] <= wdata_q[23:16];
        end
        default: ;
      endcase
    end
  end

  // Instantiate 4 pulsers
  generate
    for (genvar i = 0; i < N_PULSER_INST; i++) begin : gen_pulsers
      user_pulser i_pulser (
        .clk_i        (clk_i),
        .rst_ni       (rst_ni),
        .start_i      (start_pulse[i]),
        .stop_i       (stop_pulse[i]),
        .f1_cnt_i     (f1_count[i]),
        .f2_cnt_i     (f2_count[i]),
        .stop_cnt_i   (stop_count[i]),
        .f1_end_i     (f1_end[i]),
        .f1_switch_i  (f1_switch[i]),
        .f2_end_i     (f2_end[i]),
        .f2_switch_i  (f2_switch[i]),
        .pulse_o      (pulse_o[i]),
        .state_o      (state[i])
      );
    end
  endgenerate

  // ready = IDLE or DONE
  localparam IDLE_STATE = 3'd0;
  localparam DONE_STATE = 3'd4;
  always_comb begin
    for (int i = 0; i < 4; i++) begin
      ready[i] = (state[i] == IDLE_STATE) || (state[i] == DONE_STATE);
    end
  end

  // Read interface
  always_comb begin
    resp_data = 32'd0;
    if (req_q && !we_q) begin
      case (reg_addr)
        5'h00: resp_data    = 32'd0;
        5'h04: resp_data    = {f1_end[pulser_sel], f1_switch[pulser_sel]};
        5'h08: resp_data    = {f2_end[pulser_sel], f2_switch[pulser_sel]};
        5'h0C: resp_data    = {8'd0, stop_count[pulser_sel], f2_count[pulser_sel], f1_count[pulser_sel]};
        5'h10: resp_data    = {28'd0, state[pulser_sel], ready[pulser_sel]};
        default: resp_data  = 32'hDEADBEEF;
      endcase
    end
  end

endmodule
