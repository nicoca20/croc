`timescale 1ns / 1ps

module user_pulser_tb;

    logic clk = 0;
    logic rst_n = 0;
    logic start;
    logic [7:0] f1_count, f2_count, stop_count;
    logic [7:0] f1_end, f1_high;
    logic [7:0] f2_end, f2_high;
    logic enable_f2;
    logic pulse_out;

    // Clock generator: 10ns period (100MHz)
    always #5 clk = ~clk;

    user_pulser dut (
        .clk(clk),
        .rst_n(rst_n),
        .start(start),
        .f1_count(f1_count),
        .f2_count(f2_count),
        .stop_count(stop_count),
        .f1_end(f1_end), .f1_high(f1_high),
        .f2_end(f2_end), .f2_high(f2_high),
        .enable_f2(enable_f2),
        .pulse_out(pulse_out)
    );

    initial begin
        $dumpfile("pulser_tb.vcd");
        $dumpvars(0, pulser_tb);

        // Reset
        rst_n = 0;
        start = 0;
        #20;
        rst_n = 1;
        #20;

        // Test case 1: All 3 phases, F1 = 50 end, 15 high, 3 pulses
        f1_count = 3;
        f2_count = 3;
        stop_count = 3;
        f1_end = 8'd50; f1_high = 8'd15;
        f2_end = 8'd25; f2_high = 8'd5;
        enable_f2 = 1;
        start = 1;
        #10 start = 0;

        wait_for_done();

        // Test case 2: F2 is disabled, fallback to F1 for STOP
        $display("Starting test case 2...");
        f1_count = 4;
        f2_count = 0;
        stop_count = 2;
        f1_end = 8'd40; f1_high = 8'd10;
        f2_end = 8'd25; f2_high = 8'd5; // ignored
        enable_f2 = 0;
        start = 1;
        #10 start = 0;

        wait_for_done();

        // Test case 3: Skip F1, jump to F2 or STOP
        $display("Starting test case 3...");
        f1_count = 0;
        f2_count = 2;
        stop_count = 2;
        enable_f2 = 1;
        start = 1;
        #10 start = 0;

        wait_for_done();

        // Done
        $display("All test cases completed.");
        $finish;
    end

    task wait_for_done();
        int max_wait = 10000; // timeout
        int cycle = 0;
        while (uut.state !== uut.DONE && cycle < max_wait) begin
            #10;
            cycle++;
        end
        assert (uut.state === uut.DONE) else
            $fatal("Test failed: FSM did not reach DONE");
    endtask

endmodule
