`timescale 1ns / 1ps
// Main RAM
module sub_bus_rw_ram(  input           reset,
                        input           bus_clock,
                        input           i_bus_we, 
                        input   [15:0]  i_bus_addr, 
                        input   [15:0]  i_bus_data_write, 
                        output  [15:0]  o_bus_data_read );

  parameter  
    ADDRESS_SIZE = 49152-256; // 48KiW -256 words : must be between 1 and  (~ADDRESS_MASK + 1)
  
  (* ram_style = "block" *)
  reg [15:0] ram [ADDRESS_SIZE-1:0];
  reg [15:0] ramdata=0;
  
 
    integer ram_index;
    initial begin
        ram[0] = 16'h17ff; // al fl mov low r0,255
        ram[1] = 16'h1401; // al fl mov r1, r0
        ram[2] = 16'h1f41; // al fl add r1, 1
        ram[3] = 16'h19fe; // al fl add r15, -2
        for (ram_index = 4; ram_index < ADDRESS_SIZE; ram_index = ram_index + 1)
            ram[ram_index] = {16{1'b0}};
    end
  
  assign o_bus_data_read = ramdata; 
  
  always @(posedge bus_clock) begin
    if (reset) begin
        if (i_bus_we) ram[i_bus_addr] <= i_bus_data_write;
        ramdata <= ram[i_bus_addr];
    end
  end
  
endmodule