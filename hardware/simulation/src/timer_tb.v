`timescale 1ns / 1ps
`include "iob_lib.vh"
`include "iob_utils.vh"
`include "iob_timer_swreg_def.vh"

module timer_tb;

   localparam PER = 10;
   localparam DATA_W = 32;

   integer fd;

   `IOB_CLOCK(clk, PER)
   reg rst;

   `IOB_VAR(TIMER_ENABLE, 1)
   `IOB_VAR(TIMER_SAMPLE, 1)
   `IOB_WIRE(TIMER_VALUE, 2 * DATA_W)

   initial begin
`ifdef VCD
      $dumpfile("timer.vcd");
      $dumpvars();
`endif
      TIMER_ENABLE = 0;
      TIMER_SAMPLE = 0;

      rst          = 1;
      // deassert hard reset
      @(posedge clk) #1 rst = 0;
      @(posedge clk) #1 TIMER_ENABLE = 1;
      @(posedge clk) #1 TIMER_SAMPLE = 1;
      @(posedge clk) #1 TIMER_SAMPLE = 0;

      //uncomment to fail the test 
      //@(posedge clk) #1;

      $write("Current time: %d; Timer value %d\n", $time(), TIMER_VALUE);
      #(1000 * PER) @(posedge clk) #1 TIMER_SAMPLE = 1;
      @(posedge clk) #1 TIMER_SAMPLE = 0;
      $write("Current time: %d; Timer value %d\n", $time(), TIMER_VALUE);

      if (TIMER_VALUE == 1003) begin
         $display("Test passed");
         fd = $fopen("test.log", "w");
         $fdisplay(fd, "Test passed!");
         $fclose(fd);

      end else begin
         $display("Test failed: expecting timer value 1003 but got %d", TIMER_VALUE);
         fd = $fopen("test.log", "w");
         $fdisplay(fd, "Test failed: expecting timer value 1003 but got %d", TIMER_VALUE);
         $fclose(fd);
      end

      $finish();
   end

   //instantiate timer core
   timer_core timer0 (
      .TIMER_ENABLE(TIMER_ENABLE),
      .TIMER_SAMPLE(TIMER_SAMPLE),
      .TIMER_VALUE (TIMER_VALUE),
      .clk_i       (clk),
      .cke_i       (1'b1),
      .arst_i      (rst)
   );

endmodule
