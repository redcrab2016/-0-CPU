`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 29.09.2023 23:06:24
// Design Name: 
// Module Name: sub_bus_rw_vga
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module sub_bus_rw_vga (input            reset,
                       //board_clock, // base board clock
                       input            bus_clock,
                       input            i_bus_we, 
                       input    [0:0]   i_bus_addr, 
                       input    [15:0]  i_bus_data_write, 
                       output   [15:0]  o_bus_data_read, // for CPU communication by bus
                       input            vga_clock,
                       output   [15:0]  o_vga_addr, 
                       input    [3:0]   i_vga_4bitvalue, // for direct read to VGA vram
                       output           o_vga_vsync, 
                       output           o_vga_hsync, 
                       output   [3:0]   o_vga_red, 
                       output   [3:0]   o_vga_green, 
                       output   [3:0]   o_vga_blue); // VGA output signals

  parameter 
    // for VGA 800 x 600 60Hz, Clock at 40Mhz, http://tinyvga.com/vga-timing/800x600@60Hz 
  	VGA_H_VISIBLE_AREA     = 800,// 800 horizontal visible pixels
  	VGA_H_FRONT_PORCH      = 46, //40,
  	VGA_H_SYNC_PULSE       = 128,
    VGA_H_BACK_PORCH       = 82, //88,
    VGA_WHOLE_LINE         = 1056,
  	VGA_V_VISIBLE_AREA     = 600,      // 600 vertical visible pixels
  	VGA_V_FRONT_PORCH      = 1,
  	VGA_V_SYNC_PULSE       = 4,
    VGA_V_BACK_PORCH       = 23,
    VGA_WHOLE_FRAME        = 628,  
    
    VGA_H_VIEW_MODE_DEFAULT = 2'd0, // 
    VGA_V_VIEW_MODE_DEFAULT = 3'd0, //
    
  	VGA_H_PLOT_PER_PIXEL_0 = 10, // 800 video pixels / 80 vram pixels = 10 video pixels per vram pixel horizontaly
  	VGA_H_PLOT_PER_PIXEL_1 = 8,  // 800 video pixels / 100 vram pixels = 8 video pixels per vram pixel horizontaly
  	VGA_H_PLOT_PER_PIXEL_2 = 5,  // 800 video pixels / 160 vram pixels = 5 video pixels per vram pixel horizontaly
  	VGA_H_PLOT_PER_PIXEL_3 = 4,  // 800 video pixels / 200 vram pixels = 4 video pixels per vram pixel horizontaly
  	
    VGA_V_PLOT_PER_PIXEL_0 = 15, // 600 video pixels / 40 vram pixels = 15 video pixel per vram pixel verticaly  
    VGA_V_PLOT_PER_PIXEL_1 = 12, // 600 video pixels / 50 vram pixels = 12 video pixel per vram pixel verticaly  
    VGA_V_PLOT_PER_PIXEL_2 = 10, // 600 video pixels / 60 vram pixels = 10 video pixel per vram pixel verticaly  
    VGA_V_PLOT_PER_PIXEL_3 = 8,  // 600 video pixels / 75 vram pixels = 8 video pixel per vram pixel verticaly  
    VGA_V_PLOT_PER_PIXEL_4 = 6,  // 600 video pixels / 100 vram pixels = 6 video pixel per vram pixel verticaly  
    VGA_V_PLOT_PER_PIXEL_5 = 5,  // 600 video pixels / 120 vram pixels = 5 video pixel per vram pixel verticaly  
    VGA_V_PLOT_PER_PIXEL_6 = 4,  // 600 video pixels / 150 vram pixels = 4 video pixel per vram pixel verticaly  
    VGA_V_PLOT_PER_PIXEL_7 = 2;  // 600 video pixels / 200 vram pixels = 2 video pixel per vram pixel verticaly

  
  parameter 
  	ADDRESS_MASK = 16'hFFFE,
    ADDRESS_BASE = 16'hBF00,
    ADDRESS_SIZE = 2; // 2 Words : must be between 1 and  (~ADDRESS_MASK + 1)

  
  
  reg [10:0] vga_hcounter = 0;
  reg [10:0] vga_vcounter = 0;
  reg [10:0] vga_hcounter2 = 0;
  reg [10:0] vga_vcounter2 = 0;
  

  //(* ram_style = "block" *)
  reg [15:0] vga_registers [21:0]; 
  // VGA registers:
  //      0 - 15 : R/W color palette 12 bits  :  16'h0RGB; quad bits per channel (4096 colors) basys 3 or VGA Digilent pmod
  //      16     : R/W VGA vram offset (usefull for multi page : e.g 0 for page 0, 0x7800 for page 1
  //      17     : R/W Graphic mode X X X X X X X X X X X H H V V V  ( H H : for horizontal mode, V V for vertical mode , see comment "view mode" below)
  //      18     : RO VGA output status 16 bits  HB VB VS HS R R R R G G G G B B B B
  //      19     : RO VGA horizontal counter (10 bits) zero based
  //      20     : RO VGA vertical counter (10 bits) zero based
  //      21     : RO VGA current vram pixel value (4 bits)
  
  reg [4:0] vga_register_addr = 0;
  reg [15:0] vga_regdata = 0;
  reg [1:0] vga_h_view_mode = VGA_H_VIEW_MODE_DEFAULT; 
  reg [2:0] vga_v_view_mode = VGA_V_VIEW_MODE_DEFAULT; 
  // view mode
  //  horizontal 2 bits (4 modes)
  //       00 (80 pixels , divide by 10) : 0
  //       01 (100 pixels , divide by 8) : 1
  //       10 (160 pixels , divide by 5) : 2
  //       11 (200 pixels , divide by 4) : 3
  //  vertical  3 bits (8 modes)
  //      000 (40 pixels, divide by 15) : 0
  //      001 (50 pixels, divide by 12) : 1
  //      010 (60 pixels, divide by 10) : 2
  //      011 (75 pixels, divide by 8)  : 3
  //      100 (100 pixels, divide by 6) : 4
  //      101 (120 pixels, divide by 5) : 5
  //      110 (150 pixels, divide by 3) : 6
  //      111 (200 pixels, divide by 3) : 7
  
  // internal bus wires
  //wire chipselect;
  //wire [15:0] addr;
  
  // internal vga wires
  wire [7:0] vram_x;
  wire [7:0] vram_y;
//  wire [7:0] vram_x [3:0];
//  wire [7:0] vram_y [7:0];
  wire blank; // if 1 then RGB vga output must be zero (black)
  wire blankV; // if 1 then blank due to Vertical over visible
  wire blankH; // if 1 then blank due to Horizontal over visible
  
  // bus assignement
  assign o_bus_data_read = vga_regdata;
  
  // vga assignement
  assign blankV = (vga_vcounter >= VGA_V_VISIBLE_AREA);
  assign blankH = (vga_hcounter >= VGA_H_VISIBLE_AREA);
  assign blank =  blankV | blankH;
  assign vram_x = (blank)?0:(vga_h_view_mode == 0?(vga_hcounter / VGA_H_PLOT_PER_PIXEL_0):         // 80 pixels horizontal
                            (vga_h_view_mode == 1?(vga_hcounter / VGA_H_PLOT_PER_PIXEL_1):         // 100 pixxels horieontal
                            (vga_h_view_mode == 2?(vga_hcounter / VGA_H_PLOT_PER_PIXEL_2):         // 160 pixels horizontal
                                                  (vga_hcounter / VGA_H_PLOT_PER_PIXEL_3) )));     // 200 pixels horizontal
  assign vram_y = (blank)?0:(vga_v_view_mode == 0?(vga_vcounter / VGA_V_PLOT_PER_PIXEL_0):         // 40 pixels vertical
                            (vga_v_view_mode == 1?(vga_vcounter / VGA_V_PLOT_PER_PIXEL_1):         // 50 pixels vertical
                            (vga_v_view_mode == 2?(vga_vcounter / VGA_V_PLOT_PER_PIXEL_2):         // 60 pixels vertical
                            (vga_v_view_mode == 3?(vga_vcounter / VGA_V_PLOT_PER_PIXEL_3):         // 75 pixels vertical
                            (vga_v_view_mode == 4?(vga_vcounter / VGA_V_PLOT_PER_PIXEL_4):         // 100 pixels vertical
                            (vga_v_view_mode == 5?(vga_vcounter / VGA_V_PLOT_PER_PIXEL_5):         // 120 pixels vertical
                            (vga_v_view_mode == 6?(vga_vcounter / VGA_V_PLOT_PER_PIXEL_6):         // 150 pixels vertical
                                                  (vga_vcounter / VGA_V_PLOT_PER_PIXEL_7) ))))))); // 200 pixels vertical
                                                  
//  assign vram_x[0] = vga_hcounter / VGA_H_PLOT_PER_PIXEL_0;
//  assign vram_x[1] = vga_hcounter / VGA_H_PLOT_PER_PIXEL_1;
//  assign vram_x[2] = vga_hcounter / VGA_H_PLOT_PER_PIXEL_2;
//  assign vram_x[3] = vga_hcounter / VGA_H_PLOT_PER_PIXEL_3;
  
//  assign vram_y[0] = vga_vcounter / VGA_V_PLOT_PER_PIXEL_0;
//  assign vram_y[1] = vga_vcounter / VGA_V_PLOT_PER_PIXEL_1;
//  assign vram_y[2] = vga_vcounter / VGA_V_PLOT_PER_PIXEL_2;
//  assign vram_y[3] = vga_vcounter / VGA_V_PLOT_PER_PIXEL_3;
//  assign vram_y[4] = vga_vcounter / VGA_V_PLOT_PER_PIXEL_4;
//  assign vram_y[5] = vga_vcounter / VGA_V_PLOT_PER_PIXEL_5;
//  assign vram_y[6] = vga_vcounter / VGA_V_PLOT_PER_PIXEL_6;
//  assign vram_y[7] = vga_vcounter / VGA_V_PLOT_PER_PIXEL_7;
  
                                                  
  assign o_vga_addr  = {vram_y, vram_x} + vga_registers[16]; // address in vram with offset
//  assign o_vga_addr  = {vram_y[vga_v_view_mode], vram_x[vga_h_view_mode]} + vga_registers[16]; // address in vram with offset
  assign o_vga_red   = blank?4'b0000:vga_registers[{1'b0,i_vga_4bitvalue}][11:8];
  assign o_vga_green = blank?4'b0000:vga_registers[{1'b0,i_vga_4bitvalue}][7:4];
  assign o_vga_blue  = blank?4'b0000:vga_registers[{1'b0,i_vga_4bitvalue}][3:0];
  assign o_vga_hsync = (vga_hcounter >= (VGA_H_VISIBLE_AREA + VGA_H_FRONT_PORCH) ) &&
                         (vga_hcounter < (VGA_H_VISIBLE_AREA + VGA_H_FRONT_PORCH + VGA_H_SYNC_PULSE));
  assign o_vga_vsync = (vga_vcounter >= (VGA_V_VISIBLE_AREA + VGA_V_FRONT_PORCH) ) && 
                         (vga_vcounter < (VGA_V_VISIBLE_AREA + VGA_V_FRONT_PORCH + VGA_V_SYNC_PULSE));
  
  initial begin
      // default palette CGA
      vga_registers[0]  = 16'h0000; // black
      vga_registers[1]  = 16'h000A; // blue
      vga_registers[2]  = 16'h00A0; // green
      vga_registers[3]  = 16'h00AA; // cyan
      vga_registers[4]  = 16'h0A00; // red
      vga_registers[5]  = 16'h0A0A; // magenta
      vga_registers[6]  = 16'h0A50; // special CGA brown
      vga_registers[7]  = 16'h0AAA; // light grey
      vga_registers[8]  = 16'h0555; // dark grey
      vga_registers[9]  = 16'h055F; // light blue
      vga_registers[10] = 16'h05F5; // light green
      vga_registers[11] = 16'h05FF; // light cyan
      vga_registers[12] = 16'h0F55; // light red
      vga_registers[13] = 16'h0F5F; // light magenta
      vga_registers[14] = 16'h0FF5; // yellow
      vga_registers[15] = 16'h0FFF; // white
      // memory screen offset
      vga_registers[16] = 16'h0000; // no offset
      // view mode 
      vga_registers[17] = {11'b0, VGA_H_VIEW_MODE_DEFAULT, VGA_V_VIEW_MODE_DEFAULT}; // 160 x 120
      /*vga_registers[18] = 0;
      vga_registers[19] = 0;
      vga_registers[20] = 0;
      vga_registers[21] = 0;*/
  end 
  
  
  // manage bus 
  always @(posedge bus_clock) begin
    if (!reset) begin
      // default palette CGA 
      vga_register_addr <= 5'b0;
      vga_registers[0]  <= 16'h0000; // black
      vga_registers[1]  <= 16'h000A; // blue
      vga_registers[2]  <= 16'h00A0; // green
      vga_registers[3]  <= 16'h00AA; // cyan
      vga_registers[4]  <= 16'h0A00; // red
      vga_registers[5]  <= 16'h0A0A; // magenta
      vga_registers[6]  <= 16'h0A50; // special CGA brown
      vga_registers[7]  <= 16'h0AAA; // light grey
      vga_registers[8]  <= 16'h0555; // dark grey
      vga_registers[9]  <= 16'h055F; // light blue
      vga_registers[10] <= 16'h05F5; // light green
      vga_registers[11] <= 16'h05FF; // light cyan
      vga_registers[12] <= 16'h0F55; // light red
      vga_registers[13] <= 16'h0F5F; // light magenta
      vga_registers[14] <= 16'h0FF5; // yellow
      vga_registers[15] <= 16'h0FFF; // white
      // memory screen offset
      vga_registers[16] <= 16'h0000; // no offset
      // view mode 
      vga_registers[17] <= {11'b0, VGA_H_VIEW_MODE_DEFAULT, VGA_V_VIEW_MODE_DEFAULT}; // 160 x 120
    end 
    else
        if (i_bus_we)
          if ( !i_bus_addr ) vga_register_addr <= i_bus_data_write[4:0];
          else
            case (vga_register_addr)
                0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16:
                    vga_registers[vga_register_addr] <= i_bus_data_write;
                17:
                    begin
                        vga_registers[17] <= i_bus_data_write;
                        vga_h_view_mode   <= i_bus_data_write[4:3];
                        vga_v_view_mode   <= i_bus_data_write[2:0];
                    end
            endcase
        else vga_regdata <= i_bus_addr?{11'b0, vga_register_addr}:vga_registers[vga_register_addr];
    
  end
 
  // manage VGA init/ Readonly VGA registers value / vga output scan
  always @(posedge vga_clock) begin
    vga_hcounter <= vga_hcounter2;
    vga_vcounter <= vga_vcounter2;
  /*  if (!reset) begin
      // initialize horizontal and vertical vga output pixel counter.
      vga_hcounter2 <= 0;
      vga_vcounter2 <= 0;
      vga_registers[18] <= 0;
      vga_registers[19] <= 0;
      vga_registers[20] <= 0;
      vga_registers[21] <= 0;
    end
    else */begin
      vga_registers[18] <= {blankH, blankV, o_vga_vsync, o_vga_hsync, o_vga_red, o_vga_green, o_vga_blue};
      vga_registers[19] <= {5'b000000, vga_hcounter2};
      vga_registers[20] <= {5'b000000, vga_vcounter2};
      vga_registers[21] <= {12'b000000000000, i_vga_4bitvalue};
      // scan the frame
      if (vga_hcounter2  == (VGA_WHOLE_LINE - 1)) begin  
        vga_hcounter2 <= 11'b0;
        if (vga_vcounter2  == (VGA_WHOLE_FRAME - 1))  vga_vcounter2 <= 11'b0;
        else                                         vga_vcounter2 <= vga_vcounter2 +1;
      end
      else 
        vga_hcounter2 <= vga_hcounter2 + 1;
    end
  end
  
  
endmodule
