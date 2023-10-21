`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// VGA video ram 
//    65536 pixels defined by 4 bits (256 width x 256 height pixels)
//    each pixel can have 16 different colors
//    One word address contains 4 pixels MSq to LSq, left to right on screen
//    16384 word addresses for 65536 pixels
//    160x120 pixels view port
module sub_bus_rw_vram(input            reset,
                       input            bus_clock,
                       input            i_bus_we,
                       input    [15:0]  i_bus_addr, 
                       input    [15:0]  i_bus_data_write, 
                       output   [15:0]  o_bus_data_read,
                       input            vga_clock,
                       input    [15:0]  i_vga_addr, 
                       output   [3:0]   o_vga_4bitvalue ); 

  parameter  
    ADDRESS_SIZE = 16384; // 16KiW : must be between 1 and  (~ADDRESS_MASK + 1)
  
  (* ram_style = "block" *)
  reg [15:0]    vram [ADDRESS_SIZE -1 : 0];
  reg [15:0]    vramdata16=0;
  reg [3:0]     vramdata_gfx=0;
  reg [3:0]     vramdata_vga=0;
  
  reg [1:0]    vga_addr_sub=0;
  reg [13:0]   vga_addr_main=0;
  
//  assign vga_addr_main = i_vga_addr[15:2];
//  assign vga_addr_sub  = i_vga_addr[1:0];
  assign o_bus_data_read = vramdata16; 
  assign o_vga_4bitvalue = vramdata_vga;
  
  initial begin : setup
     integer i;
     for(i=0 ; i <ADDRESS_SIZE ; i=i+3) begin
        vram[i]   = 16'h4321;
        vram[i+1] = 16'h8765;
        vram[i+2] = 16'hCBA9;
     end
  end
  
  always @(i_vga_addr)
    begin
        vga_addr_main <= i_vga_addr[15:2];
        vga_addr_sub  <= i_vga_addr[1:0];
    end
  
  // bus access
  always @(posedge bus_clock)
    if (reset)
        if (i_bus_we ) begin
          vram[i_bus_addr] <= i_bus_data_write;
        end
        else vramdata16 <= vram[i_bus_addr];
 
  // Retrieve pixel data for VGA output
  
  
  always @(posedge vga_clock) 
    case (vga_addr_sub)
        2'b00: 
            vramdata_vga <= vram[vga_addr_main][3:0];
        2'b01:
            vramdata_vga <= vram[vga_addr_main][7:4];
        2'b10:
            vramdata_vga <= vram[vga_addr_main][11:8];
        2'b11:
            vramdata_vga <= vram[vga_addr_main][15:12];
    endcase 
endmodule