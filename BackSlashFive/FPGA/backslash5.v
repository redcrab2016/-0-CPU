`timescale 1ns / 1ps
module backslash5(clk, vgaRed, vgaGreen, vgaBlue, Hsync, Vsync, led);

    // FPGA board primary clock
    input clk; 
    
    // FPGA Basys 3 vga signal output
    output [3:0] vgaRed;
    output [3:0] vgaGreen;
    output [3:0] vgaBlue;
    output Hsync;
    output Vsync;
    
    // FPGA led
    output led;
    
    wire reset;
    wire bus_clock;
    wire vga_clock;
    
    // cpu: manager
    wire        cpu_we;
    wire        cpu_re;
    wire        cpu_ack;
    wire        cpu_ready;
    wire        cpu_interrupt;
    wire [15:0] cpu_addr;
    wire [15:0] cpu_data_write;
    wire [15:0] cpu_data_read;
    // ram: subordinate
    wire        ram_we;
    wire [15:0] ram_addr;
    wire [15:0] ram_data_write;
    wire [15:0] ram_data_read;
    // vram: subordinate
    wire        vram_we;
    wire [15:0] vram_addr;
    wire [15:0] vram_data_write;
    wire [15:0] vram_data_read;
    // vga: subordinate
    wire        vga_we;
    wire [0:0]  vga_addr;
    wire [15:0] vga_data_write;
    wire [15:0] vga_data_read;
    
    // vga bus (vram to vga raster)  
    wire [15:0] vga_vram_addr;
    wire [3:0]  vga_vram_4bitvalue;
    
    wire cpu_halt;
    
    reg r_cpu_halt=0;
    reg r_reset=0;

   bus_ctrl bus_ctrl_inst(.bus_clock(bus_clock),
                          // master (cpu)
                          .i_bus_addr(cpu_addr),
                          .i_bus_data_write(cpu_data_write),
                          .o_bus_data_read(cpu_data_read),
                          .i_bus_we(cpu_we),
                          .i_bus_re(cpu_re),
                          .o_bus_ack(cpu_ack),
                          .o_bus_ready(cpu_ready),
                          // subordinate ram
                          .o_ram_addr(ram_addr),
                          .o_ram_data_write(ram_data_write),
                          .i_ram_data_read(ram_data_read),
                          .o_ram_we(ram_we),
                          // subordinate vram
                          .o_vram_addr(vram_addr),
                          .o_vram_data_write(vram_data_write),
                          .i_vram_data_read(vram_data_read),
                          .o_vram_we(vram_we),
                          // subordinate vga
                          .o_vga_addr(vga_addr),
                          .o_vga_data_write(vga_data_write),
                          .i_vga_data_read(vga_data_read),
                          .o_vga_we(vga_we));
    
    // BackSlashFive CPU
    bs5_cpu2 bs5_cpu_inst( .reset(reset),
                          .bus_clock(bus_clock),
                          .bus_we(cpu_we),
                          .bus_re(cpu_re), 
                          .bus_addr(cpu_addr),
                          .bus_data_write(cpu_data_write), 
                          .bus_data_read(cpu_data_read),
                          .bus_ack(cpu_ack), 
                          .bus_ready(cpu_ready),
                          .cpu_interrupt(cpu_interrupt), 
                          .cpu_halt(cpu_halt));
                          
    // BackSlashFive main RAM  
    sub_bus_rw_ram ram (  .reset(reset),
                          .bus_clock(bus_clock),
                          .i_bus_we(ram_we),
                          .i_bus_addr(ram_addr),
                          .i_bus_data_write(ram_data_write), 
                          .o_bus_data_read(ram_data_read)); 
                          
    // Video RAM
    sub_bus_rw_vram vram( .reset(reset),
                          .bus_clock(bus_clock),
                          .i_bus_we(vram_we),
                          .i_bus_addr(vram_addr),
                          .i_bus_data_write(vram_data_write),
                          .o_bus_data_read(vram_data_read),
                          // wire with VGA output
                          .vga_clock(vga_clock), 
                          .i_vga_addr(vga_vram_addr),
                          .o_vga_4bitvalue(vga_vram_4bitvalue) );

    // VGA output
    sub_bus_rw_vga2 vgaout(.reset(reset),
                          .bus_clock(bus_clock),
                          .i_bus_we(vga_we), 
                          .i_bus_addr(vga_addr), 
                          .i_bus_data_write(vga_data_write), 
                          .o_bus_data_read(vga_data_read),
                          .vga_clock(vga_clock),
                          // wire to Video RAM
                          .o_vga_addr(vga_vram_addr), 
                          .i_vga_4bitvalue(vga_vram_4bitvalue),
                          // wire to board VGA output
                          .o_vga_vsync(Vsync), 
                          .o_vga_hsync(Hsync), 
                          .o_vga_red(vgaRed), 
                          .o_vga_green(vgaGreen), 
                          .o_vga_blue(vgaBlue)); 

  clk_wiz_1 bs5clocks
   (
    // Clock out ports
    .clk_out1(bus_clock),     // 100Mhz Digilent Basys 3
    .clk_out2(vga_clock),     // 40Mhz for VGA 800x600 60hz
   // Clock in ports
    .clk_in1(clk)      
); 

// Temporary design below                           
// TODO assign bus_clock and reset with constraints                                                                         
    assign reset = r_reset; 
 //   assign bus_clock = clk; // clk constraint for board clock 100Mhz for Digilent Basys 3 
    assign led = Vsync;
    assign cpu_interrupt = 0;
    always @(posedge bus_clock) begin
        r_reset <= 1'b1;
        r_cpu_halt <= (r_cpu_halt != cpu_halt)?cpu_halt:r_cpu_halt; // dummy
        
    end                         
endmodule
