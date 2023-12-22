`timescale 1ns / 1ps

  // bus address 0: R/W vga register number (0 to 21)
  // bus address 1: R/W vga register value (16 bits)
  // VGA registers:
  //      0 - 15 : R/W color palette 12 bits  :  16'h0RGB; quad bits per channel (4096 colors) basys 3 or VGA Digilent pmod : at reset palette is intialized with the 16 CGA colors
  //      16     : R/W VGA vram offset (usefull for multi page : e.g 0 for page 0, 0x7800 for page 1 in mode 21: MSB is Y coordinate in vram, LSB is X coordinate in vram (square of 256x256 4bits pixels).
  //      17     : R/W Graphic/view mode X X X X X X X X X X X H H V V V  ( H H : for horizontal mode, V V V for vertical mode , see comment "view mode" below)
  //      18     : RO VGA output status 16 bits  HB VB VS HS R R R R G G G G B B B B
  //      19     : RO VGA horizontal counter (10 bits) zero based
  //      20     : RO VGA vertical counter (10 bits) zero based
  //      21     : RO VGA current vram pixel value (4 bits)
  // view mode : vga register 17
  //  H H : horizontal 2 bits (4 modes)
  //       00 (80 pixels , divide by 10) : 0
  //       01 (100 pixels , divide by 8) : 1
  //       10 (160 pixels , divide by 5) : 2
  //       11 (200 pixels , divide by 4) : 3
  //  V V V : vertical  3 bits (8 modes)
  //      000 (40 pixels, divide by 15) : 0
  //      001 (50 pixels, divide by 12) : 1
  //      010 (60 pixels, divide by 10) : 2
  //      011 (75 pixels, divide by 8)  : 3
  //      100 (100 pixels, divide by 6) : 4
  //      101 (120 pixels, divide by 5) : 5
  //      110 (150 pixels, divide by 3) : 6
  //      111 (200 pixels, divide by 3) : 7
  // view mode summary (32 modes)
  //  0  :  80 x 40
  //  1  :  80 x 50
  //  2  :  80 x 60
  //  3  :  80 x 75
  //  4  :  80 x 100
  //  5  :  80 x 120
  //  6  :  80 x 150
  //  7  :  80 x 200
  //  8  :  100 x 40
  //  9  :  100 x 50
  //  10 :  100 x 60
  //  11 :  100 x 75
  //  12 :  100 x 100
  //  13 :  100 x 120
  //  14 :  100 x 150
  //  15 :  100 x 200
  //  16 :  160 x 40
  //  17 :  160 x 50
  //  18 :  160 x 60
  //  19 :  160 x 75
  //  20 :  160 x 100
  //  21 :  160 x 120
  //  22 :  160 x 150
  //  23 :  160 x 200
  //  24 :  200 x 40
  //  25 :  200 x 50
  //  26 :  200 x 60
  //  27 :  200 x 75
  //  28 :  200 x 100
  //  29 :  200 x 120
  //  30 :  200 x 150
  //  31 :  200 x 200

module sub_bus_rw_vga2(input            reset,
                       input            bus_clock,
                       input            i_bus_we, 
                       input    [0:0]   i_bus_addr, 
                       input    [15:0]  i_bus_data_write, 
                       output   [15:0]  o_bus_data_read, // for CPU communication by bus
                       input            vga_clock, // expect 40 Mhz for 800x600 60 hz VGA output
                       output   [15:0]  o_vga_addr, 
                       input    [3:0]   i_vga_4bitvalue, // for direct read to VGA vram
                       output           o_vga_vsync, 
                       output           o_vga_hsync, 
                       output   [3:0]   o_vga_red, 
                       output   [3:0]   o_vga_green, 
                       output   [3:0]   o_vga_blue); // VGA output signals

  parameter 
    VGA_STATE_RESET         = 3'b001,
    VGA_STATE_RESETING      = 3'b010,
    VGA_STATE_RUN           = 3'b100,
    
    // for VGA 800 x 600 60Hz, Clock at 40Mhz, http://tinyvga.com/vga-timing/800x600@60Hz 
  	VGA_H_VISIBLE_AREA     = 800,        // 800 horizontal visible pixels
  	VGA_H_FRONT_PORCH      = 46, //40,
  	VGA_H_SYNC_PULSE       = 128,
    VGA_H_BACK_PORCH       = 82, //88,
    VGA_WHOLE_LINE         = 1056,
  	VGA_V_VISIBLE_AREA     = 600,        // 600 vertical visible pixels
  	VGA_V_FRONT_PORCH      = 1,
  	VGA_V_SYNC_PULSE       = 4,
    VGA_V_BACK_PORCH       = 23,
    VGA_WHOLE_FRAME        = 628,  
    
    VGA_H_VIEW_MODE_DEFAULT = 2'd0,     // if this is change change also VGA_H_PLOT_PER_PIXEL_DEFAULT parameter
    VGA_V_VIEW_MODE_DEFAULT = 3'd0,     // if this is change change also VGA_V_PLOT_PER_PIXEL_DEFAULT parameter
    VGA_H_PLOT_PER_PIXEL_DEFAULT = 9,   // for default h view mode 0 (must be compliant with avove VGA_H_VIEW_MODE_DEFAULT)
    VGA_V_PLOT_PER_PIXEL_DEFAULT = 14,  // for default v view mode 0 (must be compliant with avove VGA_V_VIEW_MODE_DEFAULT)
    
    // zero based :to be set by the chosen view mode
  	VGA_H_PLOT_PER_PIXEL_0 = 9,      // 800 video pixels / 80 vram pixels = 10 video pixels per vram pixel horizontaly
  	VGA_H_PLOT_PER_PIXEL_1 = 7,      // 800 video pixels / 100 vram pixels = 8 video pixels per vram pixel horizontaly
  	VGA_H_PLOT_PER_PIXEL_2 = 4,      // 800 video pixels / 160 vram pixels = 5 video pixels per vram pixel horizontaly
  	VGA_H_PLOT_PER_PIXEL_3 = 3,      // 800 video pixels / 200 vram pixels = 4 video pixels per vram pixel horizontaly
  	
  	// zero based :to be set by the chosen view mode (example : 15 vga pixel clock is a count from 0 to 14
    VGA_V_PLOT_PER_PIXEL_0 = 14,    // 600 video pixels / 40 vram pixels = 15 video pixel per vram pixel verticaly  
    VGA_V_PLOT_PER_PIXEL_1 = 11,    // 600 video pixels / 50 vram pixels = 12 video pixel per vram pixel verticaly  
    VGA_V_PLOT_PER_PIXEL_2 = 9,     // 600 video pixels / 60 vram pixels = 10 video pixel per vram pixel verticaly  
    VGA_V_PLOT_PER_PIXEL_3 = 7,     // 600 video pixels / 75 vram pixels = 8 video pixel per vram pixel verticaly  
    VGA_V_PLOT_PER_PIXEL_4 = 5,     // 600 video pixels / 100 vram pixels = 6 video pixel per vram pixel verticaly  
    VGA_V_PLOT_PER_PIXEL_5 = 4,     // 600 video pixels / 120 vram pixels = 5 video pixel per vram pixel verticaly  
    VGA_V_PLOT_PER_PIXEL_6 = 3,     // 600 video pixels / 150 vram pixels = 4 video pixel per vram pixel verticaly  
    VGA_V_PLOT_PER_PIXEL_7 = 2;     // 600 video pixels / 200 vram pixels = 3 video pixel per vram pixel verticaly

  reg [2:0]  vga_state = VGA_STATE_RESET; // by default execute a reset 
  reg [4:0]  vga_substate = 0; 
  reg [10:0] vga_hcounter = 0;
  reg [10:0] vga_vcounter = 0;
  reg [15:0] vga_reg_18;
  reg [15:0] vga_reg_19;
  reg [15:0] vga_reg_20;
  reg [15:0] vga_reg_21;
  reg [15:0] vga_reg_16; // vram address offset
  reg [15:0] vga_reg_17; // view mode
  reg [15:0] vram_addr = 0;
  reg [15:0] vram_startline_addr = 0;
  
  reg [3:0] vga_pixel_per_vram_pixel = VGA_H_PLOT_PER_PIXEL_DEFAULT; 
  reg [3:0] vga_line_per_vram_line   = VGA_V_PLOT_PER_PIXEL_DEFAULT; 
  
  reg [3:0] vga_one_pixel_count =0;
  reg [3:0] vga_one_line_count  =0;
  
  
  (* ram_style = "block" *)
  reg [15:0] vga_registers [15:0]; 
  
  reg [4:0] vga_register_addr = 0;
  reg [15:0] vga_regdata = 0;
  
  
  // internal vga wires
  wire blank; // if 1 then RGB vga output must be zero (black)
  reg blankV; // if 1 then blank due to Vertical non visible area
  reg blankH; // if 1 then blank due to Horizontal non visible area
  reg vsync;  // if 1 then vertical sync pulsing
  reg hsync;  // if 1 then horizontal sync pulsing
  reg [3:0] vga_red   = 0;
  reg [3:0] vga_green = 0;
  reg [3:0] vga_blue  = 0;
  
  // bus assignement
  assign o_bus_data_read = vga_regdata;
  
  // vga assignement
  assign blank =  blankV | blankH;
  assign o_vga_addr  = vram_addr;               
  assign o_vga_red   = blank?4'b0000:vga_red;   
  assign o_vga_green = blank?4'b0000:vga_green; 
  assign o_vga_blue  = blank?4'b0000:vga_blue;  
  assign o_vga_hsync = hsync;
  assign o_vga_vsync = vsync;
  
  
  always @*
    begin
        vga_red   = vga_registers[{1'b0,i_vga_4bitvalue}][11:8];
        vga_green = vga_registers[{1'b0,i_vga_4bitvalue}][7:4];
        vga_blue  = vga_registers[{1'b0,i_vga_4bitvalue}][3:0];
    end
  
  
  // manage bus 
  always @(posedge bus_clock) 
    case (vga_state)
        VGA_STATE_RESET:
            begin
                vga_state    <= VGA_STATE_RESETING; 
                vga_substate <= 0;
            end
        VGA_STATE_RESETING:
            begin
                case(vga_substate)
                  // default palette CGA 
                  0: vga_registers[0]  <= 16'h0000; // black
                  1: vga_registers[1]  <= 16'h000A; // blue
                  2: vga_registers[2]  <= 16'h00A0; // green
                  3: vga_registers[3]  <= 16'h00AA; // cyan
                  4: vga_registers[4]  <= 16'h0A00; // red
                  5: vga_registers[5]  <= 16'h0A0A; // magenta
                  6: vga_registers[6]  <= 16'h0A50; // special CGA brown
                  7: vga_registers[7]  <= 16'h0AAA; // light grey
                  8: vga_registers[8]  <= 16'h0555; // dark grey
                  9: vga_registers[9]  <= 16'h055F; // light blue
                  10: vga_registers[10] <= 16'h05F5; // light green
                  11: vga_registers[11] <= 16'h05FF; // light cyan
                  12: vga_registers[12] <= 16'h0F55; // light red
                  13: vga_registers[13] <= 16'h0F5F; // light magenta
                  14: vga_registers[14] <= 16'h0FF5; // yellow
                  15: vga_registers[15] <= 16'h0FFF; // white
                  default:
                      begin
                          // memory screen offset
                          vga_reg_16                <= 16'h0000; // no offset
                          // view mode 
                          vga_reg_17                <= {11'b0, VGA_H_VIEW_MODE_DEFAULT, VGA_V_VIEW_MODE_DEFAULT}; 
                          vga_pixel_per_vram_pixel  <= VGA_H_PLOT_PER_PIXEL_DEFAULT; // zero based :to be set by the chosen view mode
                          vga_line_per_vram_line    <= VGA_V_PLOT_PER_PIXEL_DEFAULT;   // zero based :to be set by the chosen view mode
                          vga_register_addr         <= 5'b0;
                          vga_state <= VGA_STATE_RUN;
                      end
                endcase
                vga_substate <= vga_substate + 1;
            end
        VGA_STATE_RUN:    
            begin
                if (!reset) vga_state <= VGA_STATE_RESET; 
                if (i_bus_we)
                  if ( !i_bus_addr ) vga_register_addr <= i_bus_data_write[4:0];
                  else
                    case (vga_register_addr)
                        0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15:
                            vga_registers[vga_register_addr] <= i_bus_data_write;
                        16: vga_reg_16 <= i_bus_data_write;
                        17:
                            begin
                                vga_reg_17 <= i_bus_data_write;
                                case (i_bus_data_write[4:3])
                                    2'b00: vga_pixel_per_vram_pixel <= VGA_H_PLOT_PER_PIXEL_0;
                                    2'b01: vga_pixel_per_vram_pixel <= VGA_H_PLOT_PER_PIXEL_1;
                                    2'b10: vga_pixel_per_vram_pixel <= VGA_H_PLOT_PER_PIXEL_2;
                                    2'b11: vga_pixel_per_vram_pixel <= VGA_H_PLOT_PER_PIXEL_3;
                                endcase
                                case (i_bus_data_write[2:0])
                                    3'b000: vga_line_per_vram_line <= VGA_V_PLOT_PER_PIXEL_0;
                                    3'b001: vga_line_per_vram_line <= VGA_V_PLOT_PER_PIXEL_1;
                                    3'b010: vga_line_per_vram_line <= VGA_V_PLOT_PER_PIXEL_2;
                                    3'b011: vga_line_per_vram_line <= VGA_V_PLOT_PER_PIXEL_3;
                                    3'b100: vga_line_per_vram_line <= VGA_V_PLOT_PER_PIXEL_4;
                                    3'b101: vga_line_per_vram_line <= VGA_V_PLOT_PER_PIXEL_5;
                                    3'b110: vga_line_per_vram_line <= VGA_V_PLOT_PER_PIXEL_6;
                                    3'b111: vga_line_per_vram_line <= VGA_V_PLOT_PER_PIXEL_7;
                                endcase
                            end
                    endcase
                else 
                    if (!i_bus_addr) vga_regdata <= {11'b0, vga_register_addr};
                    else
                        case (vga_register_addr)
                            16: vga_regdata <= vga_reg_16;
                            17: vga_regdata <= vga_reg_17;
                            18: vga_regdata <= vga_reg_18;
                            19: vga_regdata <= vga_reg_19;
                            20: vga_regdata <= vga_reg_20;
                            21: vga_regdata <= vga_reg_21;
                            default:vga_regdata <= vga_registers[vga_register_addr[3:0]];
                        endcase
            end
    endcase
 
  
  // manage VGA frame scan - video out and get vram at correct address
  always @(posedge vga_clock) begin

    // set the information vga registers
    vga_reg_18 <= {blankH, blankV, o_vga_vsync, o_vga_hsync, o_vga_red, o_vga_green, o_vga_blue};
    vga_reg_19 <= {5'b000000, vga_hcounter};
    vga_reg_20 <= {5'b000000, vga_vcounter};
    vga_reg_21 <= {12'b000000000000, i_vga_4bitvalue};
    
    // scan the vram frame
    if (vga_hcounter == (VGA_WHOLE_LINE - 2 )) begin // if scan is at "just before"(to take into account vram access latency) last vga pixel clock line
        if (vga_vcounter == (VGA_WHOLE_FRAME - 1)) begin // if the scan is at last line
            vram_addr               <= vga_reg_16; // init vram adress to the vram offset.
            vram_startline_addr     <= vga_reg_16;
            vga_one_pixel_count     <= 0;
            vga_one_line_count      <= 0;
        end
        else   // if the scan is not at last line 
            if (vga_one_line_count == vga_line_per_vram_line) begin
                vga_one_line_count  <= 0;
                vga_one_pixel_count <= 0;
                vram_startline_addr <= vram_startline_addr + 16'h0100;
                vram_addr           <= vram_startline_addr + 16'h0100;
            end
            else begin
                vram_addr           <= vram_startline_addr;
                vga_one_line_count  <= vga_one_line_count + 1;
                vga_one_pixel_count <= 0;
            end
    end 
    else begin
        if (vga_one_pixel_count == vga_pixel_per_vram_pixel) begin
            vga_one_pixel_count     <= 0;
            vram_addr               <= vram_addr + 1;
        end
        else vga_one_pixel_count    <= vga_one_pixel_count + 1;
    end 
    
    // scan the vga  frame
    //      vertical and horizontal pulse sync
    if (vga_vcounter == (VGA_V_VISIBLE_AREA + VGA_V_FRONT_PORCH - 1) && vga_hcounter  == (VGA_WHOLE_LINE - 1))                      vsync   <= 1;
    if (vga_vcounter == (VGA_V_VISIBLE_AREA + VGA_V_FRONT_PORCH + VGA_V_SYNC_PULSE - 1) && vga_hcounter  == (VGA_WHOLE_LINE - 1))   vsync   <= 0;                                  
    if (vga_hcounter == (VGA_H_VISIBLE_AREA + VGA_H_FRONT_PORCH - 1))                                                               hsync   <= 1;
    if (vga_hcounter == (VGA_H_VISIBLE_AREA + VGA_H_FRONT_PORCH + VGA_H_SYNC_PULSE - 1))                                            hsync   <= 0;
    //      Horizonral and Vertical blank register
    if (vga_vcounter == (VGA_WHOLE_FRAME    - 1) && vga_hcounter  == (VGA_WHOLE_LINE - 1))                                          blankV  <= 0;
    if (vga_vcounter == (VGA_V_VISIBLE_AREA - 1) && vga_hcounter  == (VGA_WHOLE_LINE - 1))                                          blankV  <= 1;
    if (vga_hcounter == (VGA_WHOLE_LINE     - 1))                                                                                   blankH  <= 0;
    if (vga_hcounter == (VGA_H_VISIBLE_AREA - 1))                                                                                   blankH  <= 1; 
    //      vga scan counters (horizontal and vertical)
    case ({vga_hcounter  == (VGA_WHOLE_LINE - 1),vga_vcounter  == (VGA_WHOLE_FRAME - 1) })
        2'b00,
        2'b01:  vga_hcounter <= vga_hcounter + 1;                                   // increment horizontal scan while not end of line. 
        2'b10: begin vga_hcounter <= 11'b0; vga_vcounter <= vga_vcounter +1 ; end   // end of line but not end of frame, then increment line
        2'b11: begin vga_hcounter <= 11'b0; vga_vcounter <= 11'b0; end              // end of line and end of frame (reach bottom right of vga scan)
    endcase 
  end
endmodule

