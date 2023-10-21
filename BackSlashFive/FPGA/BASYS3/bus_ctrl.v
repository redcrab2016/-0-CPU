`timescale 1ns / 1ps

module bus_ctrl(
        // bus clock
        input           bus_clock,
        
        // master (cpu)
        input   [15:0]  i_bus_addr,
        input   [15:0]  i_bus_data_write,
        output  [15:0]  o_bus_data_read,
        input           i_bus_we,
        input           i_bus_re,
        output          o_bus_ack,
        output          o_bus_ready,
        
        // subordinate ram
        output  [15:0]  o_ram_addr,
        output  [15:0]  o_ram_data_write,
        input   [15:0]  i_ram_data_read,
        output          o_ram_we,
        
        // subordinate vram
        output  [15:0]  o_vram_addr,
        output  [15:0]  o_vram_data_write,
        input   [15:0]  i_vram_data_read,
        output          o_vram_we,
        
        // subordinate vga
        output  [0:0]   o_vga_addr,
        output  [15:0]  o_vga_data_write,
        input   [15:0]  i_vga_data_read,
        output          o_vga_we
    );

parameter
    BUS_READY           = 9'b00000001,
    BUS_WAIT_RAM_WRITE  = 9'b00000010,
    BUS_WAIT_VRAM_WRITE = 9'b00000100,
    BUS_WAIT_VGA_WRITE  = 9'b00001000,
    BUS_WAIT_RAM_READ   = 9'b00010000,
    BUS_WAIT_VRAM_READ  = 9'b00100000,
    BUS_WAIT_VGA_READ   = 9'b01000000,
    BUS_BAD_ADDR        = 9'b10000000;
    
    
    // output registers
    //   master
    reg [15:0]  r_bus_data_read   = 0;
    reg         r_bus_ack         = 0;
    //   ram
    reg [15:0]  r_ram_addr        = 0;
    reg [15:0]  r_ram_data_write  = 0;
    reg         r_ram_we          = 0;
    //   vram
    reg [15:0]  r_vram_addr       = 0;
    reg [15:0]  r_vram_data_write = 0;
    reg         r_vram_we         = 0;
    //   vga
    reg [0:0]   r_vga_addr        = 0;
    reg [15:0]  r_vga_data_write  = 0;
    reg         r_vga_we          = 0;
    
    reg [8:0]   r_bus_state       = BUS_READY;
    reg [1:0]   r_bus_wait_count  = 0;
    reg         r_ready      = 0;    
    // output registers assignement
    //   master
    assign o_bus_data_read   = r_bus_data_read;
    assign o_bus_ack         = r_bus_ack;
    assign o_bus_ready       = r_ready;
    //   ram
    assign o_ram_addr        = r_ram_addr;
    assign o_ram_data_write  = r_ram_data_write;
    assign o_ram_we          = r_ram_we;
    //   vram
    assign o_vram_addr       = r_vram_addr;
    assign o_vram_data_write = r_vram_data_write;
    assign o_vram_we         = r_vram_we;
    //   vga
    assign o_vga_addr        = r_vga_addr;
    assign o_vga_data_write  = r_vga_data_write;
    assign o_vga_we          = r_vga_we;
    

    
    always @(posedge bus_clock) begin
        
        case (r_bus_state)
            BUS_READY           :
                begin
                    if ({i_bus_we, i_bus_re}) begin
                        r_bus_ack <= 0;
                        r_bus_wait_count <= 1;
                        r_ready <= 0;
                        if (i_bus_addr < 16'hbf00) begin                                    // ram access from 0000 to BEFF
                            r_bus_state <= i_bus_we?BUS_WAIT_RAM_WRITE:BUS_WAIT_RAM_READ;
                            r_ram_we <= i_bus_we;
                            r_ram_data_write <= i_bus_data_write;
                            r_ram_addr <= i_bus_addr;                        
                        end
                        else if (i_bus_addr < 16'hc000) begin                               // io access from BF00 to BFFF
                            if ({i_bus_addr[15:1],1'b0} == 16'hbf00) begin                      // access to BF00 and BF01
                                r_bus_state <= i_bus_we?BUS_WAIT_VGA_WRITE:BUS_WAIT_VGA_READ;
                                r_vga_we <= i_bus_we;
                                r_vga_data_write <= i_bus_data_write;
                                r_vga_addr <= i_bus_addr[0];
                            end 
                            else begin
                                r_bus_state <= BUS_BAD_ADDR;                                
                            end                        
                        end 
                        else begin                                                              // vram access from C000 to FFFF
                            r_bus_state <= i_bus_we?BUS_WAIT_VRAM_WRITE:BUS_WAIT_VRAM_READ;
                            r_vram_we <= i_bus_we;
                            r_vram_data_write <= i_bus_data_write;
                            r_vram_addr <= i_bus_addr[13:0];                        
                        end
                    end
                    else r_ready <= 1;
                end
            BUS_WAIT_RAM_WRITE  :
                if (r_bus_wait_count) r_bus_wait_count <= r_bus_wait_count-1;
                else begin
                    r_ram_we <= 0;
                    r_bus_ack <= 1;
                    r_bus_state <= BUS_READY;   
                end
            BUS_WAIT_VRAM_WRITE :
                if (r_bus_wait_count) r_bus_wait_count <= r_bus_wait_count-1;
                else begin 
                    r_vram_we <= 0;    
                    r_bus_ack <= 1;   
                    r_bus_state <= BUS_READY;   
                end
            BUS_WAIT_VGA_WRITE  :
                if (r_bus_wait_count) r_bus_wait_count <= r_bus_wait_count-1;
                else begin    
                    r_vga_we <= 0;    
                    r_bus_ack <= 1;   
                    r_bus_state <= BUS_READY;   
                end
            BUS_WAIT_RAM_READ   :
                if (r_bus_wait_count) r_bus_wait_count <= r_bus_wait_count-1;
                else begin
                    r_bus_data_read <= i_ram_data_read;    
                    r_bus_ack <= 1;   
                    r_bus_state <= BUS_READY;   
                end
            BUS_WAIT_VRAM_READ  :
                if (r_bus_wait_count) r_bus_wait_count <= r_bus_wait_count-1;
                else begin    
                    r_bus_data_read <= i_vram_data_read;    
                    r_bus_ack <= 1;   
                    r_bus_state <= BUS_READY;   
                end
            BUS_WAIT_VGA_READ   :
                if (r_bus_wait_count) r_bus_wait_count <= r_bus_wait_count-1;
                else begin    
                    r_bus_data_read <= i_vga_data_read;    
                    r_bus_ack <= 1;   
                    r_bus_state <= BUS_READY;   
                end
            BUS_BAD_ADDR        :
                if (r_bus_wait_count) r_bus_wait_count <= r_bus_wait_count-1;
                else begin    
                    r_bus_data_read <= 0;    
                    r_bus_ack <= 1;   
                    r_bus_state <= BUS_READY;   
                end
        endcase 
    end 
    
endmodule
