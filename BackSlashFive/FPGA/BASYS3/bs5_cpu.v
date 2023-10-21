`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 01.10.2023 14:50:59
// Design Name: 
// Module Name: bs5_cpu
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

// based on BackSlashFive ISA at https://github.com/redcrab2016/BackSlash-CPU
module bs5_cpu( input reset,
                input bus_clock,
                output bus_we,
                output bus_re, 
                output [15:0] bus_addr, 
                output [15:0] bus_data_write, 
                input [15:0] bus_data_read, 
                input bus_ack,
                output cpu_halt, 
                input cpu_interrupt );
                    
 parameter
    CPU_STATE_FETCH = 0,
    CPU_STATE_DECODE_EXECUTE1 = 1,
    CPU_STATE_EXECUTE2 = 2,
    CPU_STATE_AFTER = 3;   
 
 parameter
    CPU_REGISTER_R0 =0,
    CPU_REGISTER_FLAG = 14,
    CPU_REGISTER_PC = 15,
    FLAG_C = 0,
    FLAG_Z = 1,
    FLAG_X = 2,
    FLAG_H = 13,
    FLAG_I = 14,
    FLAG_K = 15;
 
 reg [15:0] cpu_register [15:0];
 reg [15:0] cpu_register_normal [15:0];
 reg [15:0] cpu_register_interrupt [15:0];
 reg [1:0]  cpu_state = CPU_STATE_FETCH;
 reg r_cpu_interrupt_pending;
 reg [15:0] r_bus_addr = 0;
 reg [15:0] r_bus_data_write = 0;
 reg [15:0] r_instruction;
 reg r_bus_we =0;
 reg r_bus_re = 1;
 reg r_cpu_halt = 0;
 
 reg [3:0] latency_wait = 0;
 
 wire cpu_cond_exec [7:0];
 
 wire [2:0] instr_cond;
 wire [3:0] instr_instr4M;
 wire [3:0] instr_instr4L;
 wire [3:0] instr_regY;
 wire [3:0] instr_regX;
 wire [7:0] instr_imm8;
 wire [7:0] instr_simm8;
 wire [3:0] instr_imm4a;
 wire [3:0] instr_imm4b;
 wire       instr_modflag;
 wire [15:0] instr;

// ALU wires
wire C;
wire [15:0] Rx;
wire [15:0] Ry;
wire [15:0] PC;

//	mov Rx, [Ry] 
wire [15:0] mov_Rx_atRy_result;
wire        mov_Rx_atRy_C;

//	add Rx, [Ry]
wire [16:0] add_Rx_atRy_result;
wire        add_Rx_atRy_C;

//	sub Rx, [Ry]
wire [16:0] sub_Rx_atRy_result;
wire        sub_Rx_atRy_C;

// mov Rx, Ry
wire [15:0] mov_Rx_Ry_result;
wire        mov_Rx_Ry_C;

//	add Rx, Ry
wire [16:0] add_Rx_Ry_result;
wire        add_Rx_Ry_C;

//	sub Rx, [Ry]
wire [16:0] sub_Rx_Ry_result;
wire        sub_Rx_Ry_C;

//  mov Rx:imm4, C
wire [15:0] mov_Rx_imm4_C_result;
wire        mov_Rx_imm4_C_C;

//  mov Rx:imm4, 0
wire [15:0] mov_Rx_imm4_0_result;
wire        mov_Rx_imm4_0_C;

//  not Rx:imm4
wire [15:0] not_Rx_imm4_result;
wire        not_Rx_imm4_C;

//	add Rx, 1
wire [16:0] add_Rx_1_result;
wire        add_Rx_1_C;

//	sub Rx, 1
wire [16:0] sub_Rx_1_result;
wire        sub_Rx_1_C;

//	mov high Rx, 0
wire [15:0] mov_high_Rx_0_result;
wire        mov_high_Rx_0_C;

//	mov low Rx, 0
wire [15:0] mov_low_Rx_0_result;
wire        mov_low_Rx_0_C;

//	mov Rx, 0
wire [15:0] mov_Rx_0_result;
wire        mov_Rx_0_C;

//  not Rx
wire [15:0] not_Rx_result;
wire        not_Rx_C;


// ALU wire assignements
assign Rx = cpu_register[instr_regX];
assign Ry = cpu_register[instr_regY];
assign PC = cpu_register[CPU_REGISTER_PC];
assign C  = cpu_register[CPU_REGISTER_FLAG][FLAG_C];

//ccc f 0001 yyyy xxxx
//	mov Rx, [Ry] 
assign mov_Rx_atRy_result = bus_data_read;
assign mov_Rx_atRy_C      = C;

//ccc f 0010 yyyy xxxx
//	add Rx, [Ry]
assign add_Rx_atRy_result = Rx + bus_data_read;
assign add_Rx_atRy_C      = add_Rx_atRy_result[16];

//ccc f 0011 yyyy xxxx
//	sub Rx, [Ry]
assign sub_Rx_atRy_result = Rx - bus_data_read; 
assign sub_Rx_atRy_C      = sub_Rx_atRy_result[16];

//ccc f 0100 yyyy xxxx
//	mov Rx, Ry
assign mov_Rx_Ry_result = Ry;  
assign mov_Rx_Ry_C      = C;

//ccc f 0101 yyyy xxxx
//	add Rx, Ry
assign add_Rx_Ry_result = Rx + Ry;
assign add_Rx_Ry_C      = add_Rx_Ry_result[16];

//ccc f 0110 yyyy xxxx
//	sub Rx, Ry
assign sub_Rx_Ry_result = Rx - Ry;
assign sub_Rx_Ry_C      = sub_Rx_Ry_result[16];

//ccc f 1011 iiii xxxx
//	mov Rx:imm4, C
assign mov_Rx_imm4_C_result = (C << instr_imm4a) | Rx;
assign mov_Rx_imm4_C_C      = C;

//ccc f 1100 iiii xxxx
//	mov Rx:imm4, 0
assign mov_Rx_imm4_0_result = (~(16'b0 | (1 << instr_imm4a))) & Rx;
assign mov_Rx_imm4_0_C      = C;

//ccc f 1101 iiii xxxx
//	mov Rx:imm4, 1
assign mov_Rx_imm4_1_result =  ({15'b0,1'b1} << instr_imm4a) | Rx;
assign mov_Rx_imm4_1_C      = C;

//ccc f 1110 iiii xxxx
//	not Rx:imm4
assign not_Rx_imm4_result   = ((~(16'b0 | (1 << instr_imm4a))) & Rx) | Rx[instr_imm4a];
assign not_Rx_imm4_C        = C;

//1 register
//ccc f 1111 0100 xxxx
//	add Rx, 1
assign add_Rx_1_result = Rx + 1;
assign add_Rx_1_C = add_Rx_1_result[16];

//ccc f 1111 0101 xxxx
//	sub Rx, 1
assign sub_Rx_1_result = Rx - 1;
assign sub_Rx_1_C = sub_Rx_1_result[16];

//ccc f 1111 1010 xxxx
//	mov low Rx, 0
assign mov_low_Rx_0_result = {Rx[15:7],8'b0};
assign mov_low_Rx_0_C = C;

//ccc f 1111 1011 xxxx
//	mov high Rx, 0
assign mov_high_Rx_0_result = {8'b0, Rx[7:0]};
assign mov_high_Rx_0_C = C;

//ccc f 1111 1100 xxxx
//	mov Rx, 0
assign mov_Rx_0_result = 16'b0;
assign mov_Rx_0_C      = C;

//ccc f 1111 1101 xxxx
//	not Rx
assign not_Rx_result = ~Rx;
assign not_Rx_C      = C;

// conditional execution : BS5 ccc
assign cpu_cond_exec[0] = 1'b1; // always
assign cpu_cond_exec[1] = cpu_register[CPU_REGISTER_FLAG][FLAG_Z];  // Z==1
assign cpu_cond_exec[2] = ~cpu_register[CPU_REGISTER_FLAG][FLAG_Z]; // Z==0
assign cpu_cond_exec[3] = cpu_register[CPU_REGISTER_FLAG][FLAG_C];  // C==1
assign cpu_cond_exec[4] = ~cpu_register[CPU_REGISTER_FLAG][FLAG_C]; // C==0
assign cpu_cond_exec[5] = cpu_register[CPU_REGISTER_FLAG][FLAG_X];  // X==1
assign cpu_cond_exec[6] = ~cpu_register[CPU_REGISTER_FLAG][FLAG_X]; // X==0
assign cpu_cond_exec[7] = 1'b0; // never (nop)

// instruction structure assignement
assign instr = (cpu_state == CPU_STATE_DECODE_EXECUTE1)?bus_data_read:r_instruction;
assign instr_cond    = instr[15:13];
assign instr_modflag = instr[12];
assign instr_instr4M = instr[11:8];
assign instr_instr4L = instr[7:4];
assign instr_regY    = instr[7:4];
assign instr_regX    = instr[3:0];
assign instr_imm8    = instr[7:0];
assign instr_simm8   = instr[7:0];
assign instr_imm4a   = instr[7:4];
assign instr_imm4b   = instr[3:0];


// bus assignment
assign bus_addr =  r_bus_addr;
assign bus_data_write = r_bus_data_write;
assign bus_we = r_bus_we;
assign bus_re = r_bus_re;

// cpu assignment
assign cpu_halt = r_cpu_halt;
integer i;

initial begin
        cpu_state = CPU_STATE_FETCH;
        r_bus_addr = 0;
        r_bus_data_write = 0;
        r_bus_we =0;
        r_cpu_halt = 0;
        r_cpu_interrupt_pending = 0;
        latency_wait = 0;
        // init working CPU registers
        for (i=0; i <=15 ; i=i+1) cpu_register[i] = 0;
        // init normal context CPU registers backup
        for (i=0; i <=15 ; i=i+1) cpu_register_normal[i] = 0;
        // init interrupt context CPU registers backup 
        for (i=0; i <=15 ; i=i+1) cpu_register_interrupt[i] = 0;
end 

always @(posedge cpu_interrupt) begin
    r_cpu_interrupt_pending <= 1;//cpu_interrupt;
end


always @(posedge bus_clock) begin
    if (reset == 0) begin
//        cpu_state <= CPU_STATE_FETCH;
//        r_bus_addr <= 0;
//        r_bus_data_write <= 0;
//        r_bus_we <=0;
//        r_cpu_halt <= 0;
//        cpu_register[0] <= 0;
//        cpu_register[1] <= 0;
//        cpu_register[2] <= 0;
//        cpu_register[3] <= 0;
//        cpu_register[4] <= 0;
//        cpu_register[5] <= 0;
//        cpu_register[6] <= 0;
//        cpu_register[7] <= 0;
//        cpu_register[8] <= 0;
//        cpu_register[9] <= 0;
//        cpu_register[10] <= 0;
//        cpu_register[11] <= 0;
//        cpu_register[12] <= 0;
//        cpu_register[13] <= 0;
//        cpu_register[14] <= 0;
//        cpu_register[15] <= 0;
    end 
    else begin
       if (latency_wait != 0) begin
            latency_wait <= latency_wait -1;
            r_bus_re <=0;
       end

`define fetch(pc_address) \
        r_bus_we <=  0;\
        r_bus_re <=  1;\
        r_bus_addr <= (pc_address);\
        cpu_register[CPU_REGISTER_PC] <= (pc_address) + 1;


       if (latency_wait == 0 && (bus_ack || r_bus_re)) begin        
           latency_wait <= 1;    
           case (cpu_state)
              CPU_STATE_FETCH  : 
                  begin
                    `fetch((cpu_register[CPU_REGISTER_PC]))
                    cpu_state <= CPU_STATE_DECODE_EXECUTE1;
                  end
              CPU_STATE_DECODE_EXECUTE1  : begin // if no execute then fetch next instruction
                    // check interrupt signal, then if K=0 (normal context) and I=1 (interrupt enabled) then switch cpu register context, then state the cpu for fetch.
                    if (r_cpu_interrupt_pending == 1 && 
                        cpu_register[CPU_REGISTER_FLAG][FLAG_I] == 1 && 
                        cpu_register[CPU_REGISTER_FLAG][FLAG_K] == 0) begin
                        //r_cpu_interrupt_pending <= 0;
                        for (i = 0 ; i <= 15 ; i = i +1) begin 
                            cpu_register_normal[i] <= cpu_register[i];
                        end
                        for (i = 0 ; i <= 13 ; i = i +1) begin
                            cpu_register[i] <= cpu_register_interrupt[i];
                        end
                        cpu_register[14] <= {1'b1,1'b1, cpu_register_interrupt[14][13:0]};
                        cpu_register[15] <= cpu_register_interrupt[15];
                        cpu_state <= CPU_STATE_FETCH;
                    end // end if signal interrupt switch
                    else if (cpu_cond_exec[instr_cond] == 0) begin // if condition of execution indicate (not execute) then fetch next instruction
                        `fetch((cpu_register[CPU_REGISTER_PC]))
                    end     
                    else begin // instruction to execute 
                         // no bus R/W involved instructions
                         case (instr_instr4M)
                            // 2 registers parameters 
                            
                            // ccc f 0000 yyyy xxxx
                            // 	mov [Rx], Ry
                            4'b0000 : 
                                begin
                                    r_bus_addr <= cpu_register[instr_regX];
                                    r_bus_data_write <= cpu_register[instr_regY];
                                    r_bus_we <= 1'b1;
                                    r_bus_re <= 1'b0;
                                    cpu_state <= CPU_STATE_FETCH;
                                    if (instr_modflag) begin
                                        cpu_register[CPU_REGISTER_FLAG][FLAG_Z] <= (cpu_register[instr_regY] != 16'b0 ); // modify Z flag
                                    end
                                end
                            
                            //ccc f 0001 yyyy xxxx
                            //	mov Rx, [Ry] 
                            //ccc f 0010 yyyy xxxx
                            //	add Rx, [Ry]
                            //ccc f 0011 yyyy xxxx
                            //	sub Rx, [Ry]
                            4'b0001, 4'b0010, 4'b0011 :
                                begin
                                    r_instruction <= bus_data_read;
                                    r_bus_we <=  1'b0; // No bus write 
                                    r_bus_re <=  1'b1; // Bus read
                                    r_bus_addr <= cpu_register[instr_regY];
                                    cpu_state <= CPU_STATE_EXECUTE2;
                                end
                                
                            //ccc f 0100 yyyy xxxx
                            //	mov Rx, Ry
                            4'b0100 : 
                                begin
                                    if (instr_regX == CPU_REGISTER_PC) begin // if "mov PC, Ry" therefore it is a jump to Ry
                                        `fetch((cpu_register[instr_regY]))
                                    end
                                    else begin
                                        cpu_register[instr_regX] <= cpu_register[instr_regY];
                                        `fetch((cpu_register[CPU_REGISTER_PC]))                                            
                                    end 
                                    if (instr_modflag) begin
                                        cpu_register[CPU_REGISTER_FLAG][FLAG_Z] <= (cpu_register[instr_regY] != 16'b0 ); // modify Z flag
                                    end
                                end
                            
                            //ccc f 0101 yyyy xxxx
                            //	add Rx, Ry
                            4'b0101 :
                                begin
                                    if (instr_regX == CPU_REGISTER_PC) begin // if "mov PC, Ry" therefore it is a jump to Ry
                                        `fetch((cpu_register[CPU_REGISTER_PC] + cpu_register[instr_regY]))
                                    end
                                    else begin
                                        cpu_register[instr_regX] <= cpu_register[instr_regX] + cpu_register[instr_regY];
                                        `fetch((cpu_register[CPU_REGISTER_PC]))
                                    end 
                                     if (instr_modflag) begin
                                        cpu_register[CPU_REGISTER_FLAG][FLAG_Z] <= ((cpu_register[instr_regX] + cpu_register[instr_regY]) != 16'b0 ); // modify Z flag
                                        cpu_register[CPU_REGISTER_FLAG][FLAG_C] <= ((cpu_register[instr_regX] + cpu_register[instr_regY]) < cpu_register[instr_regX] ); // modify C flag
                                    end
                               end
                            
                            //ccc f 0110 yyyy xxxx
                            //	sub Rx, Ry
                            4'b0110 :
                                 begin
                                    if (instr_regX == CPU_REGISTER_PC) begin // if "mov PC, Ry" therefore it is a jump to Ry
                                        `fetch((cpu_register[CPU_REGISTER_PC] - cpu_register[instr_regY]))
                                    end
                                    else begin
                                        cpu_register[instr_regX] <= cpu_register[instr_regX] - cpu_register[instr_regY];
                                        `fetch((cpu_register[CPU_REGISTER_PC]))
                                    end 
                                     if (instr_modflag) begin
                                        cpu_register[CPU_REGISTER_FLAG][FLAG_Z] <= ((cpu_register[instr_regX] - cpu_register[instr_regY]) != 16'b0 ); // modify Z flag
                                        cpu_register[CPU_REGISTER_FLAG][FLAG_C] <= ((cpu_register[instr_regX] - cpu_register[instr_regY]) > cpu_register[instr_regX] ); // modify C flag
                                    end
                               end
                           
                            //1 unsigned/signed byte parameter 
                            //ccc f 0111 iiii iiii
                            //	mov low R0, imm8
                            4'b0111 :
                                begin
                                    cpu_register[CPU_REGISTER_R0][7:0] <= instr_imm8;
                                    `fetch((cpu_register[CPU_REGISTER_PC]))
                                    if (instr_modflag) begin
                                        cpu_register[CPU_REGISTER_FLAG][FLAG_Z] <= ({cpu_register[CPU_REGISTER_R0][15:8], instr_imm8} != 16'b0 ); // modify Z flag
                                    end
                                end
    
                            //ccc f 1000 iiii iiii
                            //	mov high R0, imm8
                            4'b1000 : 
                                begin
                                    cpu_register[CPU_REGISTER_R0][15:8] <= instr_imm8;
                                    `fetch((cpu_register[CPU_REGISTER_PC]))
                                    if (instr_modflag) begin
                                        cpu_register[CPU_REGISTER_FLAG][FLAG_Z] <= ({ instr_imm8, cpu_register[CPU_REGISTER_R0][7:0]} != 16'b0 ); // modify Z flag
                                    end
                                end
    
                            //ccc f 1001 siii iiii
                            //	add R15, simm8
                            4'b1001 :
                                begin
                                   `fetch(( cpu_register[CPU_REGISTER_PC] + 
                                            {{instr_simm8[7]},{instr_simm8[7]},{instr_simm8[7]},{instr_simm8[7]},
                                             {instr_simm8[7]},{instr_simm8[7]},{instr_simm8[7]},{instr_simm8[7]},
                                             instr_simm8}))
                                end
                            
                            //1 register and 1 quartet parameter
                            //ccc f 1010 iiii xxxx
                            //	mov C, Rx:imm4
                            4'b1010 :
                                begin
                                    cpu_register[CPU_REGISTER_FLAG][FLAG_C] <= cpu_register[instr_regX][instr_imm4a];
                                    `fetch((cpu_register[CPU_REGISTER_PC]))
                                end
                            
                            //ccc f 1011 iiii xxxx
                            //	mov Rx:imm4, C
                            4'b1011 : 
                                begin
                                    if ( instr_regX == CPU_REGISTER_PC)  begin
                                        `fetch( ((cpu_register[CPU_REGISTER_PC] & ~({15'b0,1'b1} << instr_imm4a)) | (cpu_register[CPU_REGISTER_FLAG][FLAG_C] << instr_imm4a)   ))
                                    end
                                    else begin
                                        cpu_register[instr_regX][instr_imm4a] <= cpu_register[CPU_REGISTER_FLAG][FLAG_C];
                                        `fetch((cpu_register[CPU_REGISTER_PC]))
                                    end
                                    if (instr_modflag && instr_regX != CPU_REGISTER_FLAG ) begin
                                        cpu_register[CPU_REGISTER_FLAG][FLAG_Z] <= (cpu_register[instr_regX] != 16'b0 ); // modify Z flag
                                    end
                               end
                            
                            //ccc f 1100 iiii xxxx
                            //	mov Rx:imm4, 0
                            4'b1100 :
                                begin
                                    if ( instr_regX == CPU_REGISTER_PC)  begin
                                        `fetch( (cpu_register[CPU_REGISTER_PC] & ~({15'b0,1'b1} << instr_imm4a)  ))
                                    end
                                    else begin                              
                                        cpu_register[instr_regX][instr_imm4a] <= 1'b0;
                                        `fetch((cpu_register[CPU_REGISTER_PC]))
                                    end
                                    if (instr_modflag && instr_regX != CPU_REGISTER_FLAG )  begin
                                        cpu_register[CPU_REGISTER_FLAG][FLAG_Z] <= (cpu_register[instr_regX] != 16'b0 ); // modify Z flag
                                    end
                               end
                            
                            //ccc f 1101 iiii xxxx
                            //	mov Rx:imm4, 1
                            4'b1101 : 
                                begin
                                    
                                    if ( instr_regX == CPU_REGISTER_PC)  begin
                                        `fetch( (cpu_register[CPU_REGISTER_PC] | ({15'b0, 1'b1} << instr_imm4a)) )
                                    end
                                    else begin   
                                        cpu_register[instr_regX][instr_imm4a] <= 1'b1;                           
                                        `fetch((cpu_register[CPU_REGISTER_PC]))
                                    end
                                    if (instr_modflag && instr_regX != CPU_REGISTER_FLAG )  begin
                                        cpu_register[CPU_REGISTER_FLAG][FLAG_Z] <= (cpu_register[instr_regX] != 16'b0 ); // modify Z flag
                                    end
                               end
                            
                            //ccc f 1110 iiii xxxx
                            //	not Rx:imm4
                            4'b1110 : 
                                  begin
                                    
                                    if ( instr_regX == CPU_REGISTER_PC)  begin
                                        `fetch( (cpu_register[CPU_REGISTER_PC] ^ ({15'b0,1'b1} << instr_imm4a)) )
                                    end
                                    else begin  
                                        `fetch((cpu_register[CPU_REGISTER_PC]))
                                    end
                                    if (instr_modflag && instr_regX != CPU_REGISTER_FLAG )  begin
                                        cpu_register[CPU_REGISTER_FLAG][FLAG_Z] <= (cpu_register[instr_regX] != 16'b0 ); // modify Z flag
                                    end
                               end
                          
                            //ccc f 1111 .... ....
                            //   set of instruction with instr_instr4L
                            
                            4'b1111 : 
                                case (instr_instr4L)
                                //1 quartet
                                //ccc f 1111 0000 iiii
                                //	add R0, imm4
                                4'b0000 :
                                    begin
                                        cpu_register[CPU_REGISTER_R0] <= cpu_register[CPU_REGISTER_R0] + {12'b0,instr_imm4b};
                                        `fetch((cpu_register[CPU_REGISTER_PC]))
                                        if (instr_modflag && instr_regX != CPU_REGISTER_FLAG )  begin
                                            cpu_register[CPU_REGISTER_FLAG][FLAG_Z] <= (cpu_register[CPU_REGISTER_R0] != 16'b0 ); // modify Z flag
                                            cpu_register[CPU_REGISTER_FLAG][FLAG_C] <= ((cpu_register[CPU_REGISTER_R0] + {12'b0,instr_imm4b}) < cpu_register[CPU_REGISTER_R0] ); // modify C flag
                                        end
                                    end
                        
                                
                                //ccc f 1111 0001 iiii
                                //	sub R0, imm4
                                4'b0001 :
                                    begin
                                        cpu_register[CPU_REGISTER_R0] <= cpu_register[CPU_REGISTER_R0] - {12'b0,instr_imm4b};
                                        `fetch((cpu_register[CPU_REGISTER_PC]))
                                        if (instr_modflag && instr_regX != CPU_REGISTER_FLAG )  begin
                                            cpu_register[CPU_REGISTER_FLAG][FLAG_Z] <= (cpu_register[CPU_REGISTER_R0] != 16'b0 ); // modify Z flag
                                            cpu_register[CPU_REGISTER_FLAG][FLAG_C] <= ((cpu_register[CPU_REGISTER_R0] - {12'b0,instr_imm4b}) > cpu_register[CPU_REGISTER_R0] ); // modify C flag
                                        end
                                    end
                                
                                //ccc f 1111 0010 iiii
                                //	shl R0, imm4
                                4'b0010 :
                                     begin
                                        cpu_register[CPU_REGISTER_R0] <= cpu_register[CPU_REGISTER_R0] << instr_imm4b;
                                        `fetch((cpu_register[CPU_REGISTER_PC]))
                                        if (instr_modflag && instr_regX != CPU_REGISTER_FLAG )  begin
                                            cpu_register[CPU_REGISTER_FLAG][FLAG_Z] <= ((cpu_register[CPU_REGISTER_R0] << instr_imm4b) != 16'b0 ); // modify Z flag
                                        end
                                    end
                                
                                //ccc f 1111 0011 iiii
                                //	shr R0, imm4
                                4'b0011 :
                                     begin
                                        cpu_register[CPU_REGISTER_R0] <= cpu_register[CPU_REGISTER_R0] >> instr_imm4b;
                                        `fetch((cpu_register[CPU_REGISTER_PC]))
                                        if (instr_modflag && instr_regX != CPU_REGISTER_FLAG )  begin
                                            cpu_register[CPU_REGISTER_FLAG][FLAG_Z] <= ((cpu_register[CPU_REGISTER_R0] >> instr_imm4b) != 16'b0 ); // modify Z flag
                                        end
                                    end
                                
                                //1 register
                                //ccc f 1111 0100 xxxx
                                //	add Rx, 1
                                4'b0100 :
                                      begin
                                        if ( instr_regX == CPU_REGISTER_PC ) begin
                                            `fetch( (cpu_register[CPU_REGISTER_PC] + 1) )
                                        end
                                        else begin
                                            cpu_register[instr_regX] <= cpu_register[instr_regX] + 1;
                                            `fetch((cpu_register[CPU_REGISTER_PC]))
                                        end
                                        if (instr_modflag && instr_regX != CPU_REGISTER_FLAG )  begin
                                            cpu_register[CPU_REGISTER_FLAG][FLAG_Z] <= ((cpu_register[instr_regX] + 1) != 16'b0 ); // modify Z flag
                                            cpu_register[CPU_REGISTER_FLAG][FLAG_C] <= (cpu_register[instr_regX] == 16'hffff ); // modify C flag
                                        end
                                    end                           
    
                                //ccc f 1111 0101 xxxx
                                //	sub Rx, 1
                                4'b0101 :
                                      begin
                                        if ( instr_regX == CPU_REGISTER_PC ) begin
                                            `fetch( (cpu_register[CPU_REGISTER_PC] - 1) )
                                        end
                                        else begin
                                            cpu_register[instr_regX] <= cpu_register[instr_regX] - 1;
                                            `fetch((cpu_register[CPU_REGISTER_PC]))
                                        end
                                        if (instr_modflag && instr_regX != CPU_REGISTER_FLAG )  begin
                                            cpu_register[CPU_REGISTER_FLAG][FLAG_Z] <= ((cpu_register[instr_regX] - 1) != 16'b0 ); // modify Z flag
                                            cpu_register[CPU_REGISTER_FLAG][FLAG_C] <= (cpu_register[instr_regX] == 16'hffff ); // modify C flag
                                        end
                                    end                           
                                
                                //ccc f 1111 0110 xxxx
                                //	mov low R0, low Rx
                                4'b0110 : 
                                    begin
                                        cpu_register[CPU_REGISTER_R0][7:0] <= cpu_register[instr_regX][7:0];
                                        `fetch((cpu_register[CPU_REGISTER_PC]))
                                        if (instr_modflag )  begin
                                            cpu_register[CPU_REGISTER_FLAG][FLAG_Z] <= ({cpu_register[CPU_REGISTER_R0][15:8],cpu_register[instr_regX][7:0]} != 16'b0 ); // modify Z flag
                                        end
                                    end         
    
                                //ccc f 1111 0111 xxxx
                                //	mov low R0, high Rx
                                4'b0111 :
                                    begin
                                        cpu_register[CPU_REGISTER_R0][7:0] <= cpu_register[instr_regX][15:8];
                                        `fetch((cpu_register[CPU_REGISTER_PC]))
                                        if (instr_modflag )  begin
                                            cpu_register[CPU_REGISTER_FLAG][FLAG_Z] <= ({cpu_register[CPU_REGISTER_R0][15:8],cpu_register[instr_regX][15:8]} != 16'b0 ); // modify Z flag
                                        end
                                    end         
                                
                                
                                //ccc f 1111 1000 xxxx
                                //	mov high R0, low Rx
                                4'b1000 : 
                                    begin
                                        cpu_register[CPU_REGISTER_R0][15:8] <= cpu_register[instr_regX][7:0];
                                        `fetch((cpu_register[CPU_REGISTER_PC]))
                                        if (instr_modflag )  begin
                                            cpu_register[CPU_REGISTER_FLAG][FLAG_Z] <= ({cpu_register[instr_regX][7:0],cpu_register[CPU_REGISTER_R0][7:0]} != 16'b0 ); // modify Z flag
                                        end
                                    end         
                                                           
                                //ccc f 1111 1001 xxxx
                                //	mov high R0, high Rx
                                4'b1001 : 
                                    begin
                                        cpu_register[CPU_REGISTER_R0][15:8] <= cpu_register[instr_regX][15:8];
                                        `fetch((cpu_register[CPU_REGISTER_PC]))
                                        if (instr_modflag )  begin
                                            cpu_register[CPU_REGISTER_FLAG][FLAG_Z] <= ({cpu_register[instr_regX][15:8],cpu_register[CPU_REGISTER_R0][7:0]} != 16'b0 ); // modify Z flag
                                        end
                                    end         
                                
                                //ccc f 1111 1010 xxxx
                                //	mov low Rx, 0
                                4'b1010 : 
                                    begin
                                        
                                        if (instr_regX == CPU_REGISTER_PC) begin
                                            `fetch(({cpu_register[CPU_REGISTER_PC][15:8],8'b0}))
                                        end
                                        else begin
                                            cpu_register[instr_regX][7:0] <= 8'b0;
                                            `fetch((cpu_register[CPU_REGISTER_PC]))
                                        end
                                        if (instr_modflag && instr_regX != CPU_REGISTER_FLAG )  begin
                                            cpu_register[CPU_REGISTER_FLAG][FLAG_Z] <= ({cpu_register[instr_regX][15:8],8'b0} != 16'b0 ); // modify Z flag
                                        end
                                    end         
                                
                                //ccc f 1111 1011 xxxx
                                //	mov high Rx, 0
                                4'b1011 :
                                    begin
                                        if (instr_regX == CPU_REGISTER_PC) begin
                                            `fetch( ({8'b0, cpu_register[CPU_REGISTER_PC][7:0]}) )
                                        end
                                        else begin
                                            cpu_register[instr_regX][15:8] <= 8'b0;
                                            `fetch((cpu_register[CPU_REGISTER_PC]))
                                        end
                                        if (instr_modflag && instr_regX != CPU_REGISTER_FLAG )  begin
                                            cpu_register[CPU_REGISTER_FLAG][FLAG_Z] <= ({8'b0,cpu_register[instr_regX][7:0]} != 16'b0 ); // modify Z flag
                                        end
                                    end                                   
                                
                                //ccc f 1111 1100 xxxx
                                //	mov Rx, 0
                                4'b1100 : 
                                    begin   
                                        if (instr_regX == CPU_REGISTER_PC) begin
                                            `fetch( (16'b0) )
                                        end
                                        else begin
                                            cpu_register[instr_regX] <= 16'b0;
                                            `fetch((cpu_register[CPU_REGISTER_PC]))
                                        end                                    
                                        if (instr_modflag && instr_regX != CPU_REGISTER_FLAG )  begin
                                            cpu_register[CPU_REGISTER_FLAG][FLAG_Z] <=  1'b1; // modify Z flag
                                        end
                                    end         
                                
                                //ccc f 1111 1101 xxxx
                                //	not Rx
                                4'b1101 : 
                                    begin
                                        
                                        if (instr_regX == CPU_REGISTER_PC) begin
                                            `fetch((~cpu_register[CPU_REGISTER_PC]))
                                        end
                                        else begin
                                            cpu_register[instr_regX] <= ~cpu_register[instr_regX];
                                            `fetch((cpu_register[CPU_REGISTER_PC]))
                                        end
                                        if (instr_modflag && instr_regX != CPU_REGISTER_FLAG )  begin
                                            cpu_register[CPU_REGISTER_FLAG][FLAG_Z] <= (~cpu_register[instr_regX] != 16'b0); // modify Z flag
                                        end
                                    end         
                                
                                //ccc f 1111 1110 xxxx
                                //	and R0, Rx
                                4'b1110 :
                                    begin
                                        cpu_register[CPU_REGISTER_R0] <= cpu_register[CPU_REGISTER_R0] & cpu_register[instr_regX];
                                        `fetch((cpu_register[CPU_REGISTER_PC]))
                                        if (instr_modflag )  begin
                                            cpu_register[CPU_REGISTER_FLAG][FLAG_Z] <=  ((cpu_register[CPU_REGISTER_R0] & cpu_register[instr_regX]) != 16'b0); // modify Z flag
                                        end
                                    end         
                                
                                //ccc f 1111 1111 xxxx
                                //	or  R0, Rx
                                4'b1111 :                                
                                     begin
                                        cpu_register[CPU_REGISTER_R0] <= cpu_register[CPU_REGISTER_R0] | cpu_register[instr_regX];
                                        `fetch((cpu_register[CPU_REGISTER_PC]))
                                        if (instr_modflag && instr_regX != CPU_REGISTER_FLAG )  begin
                                            cpu_register[CPU_REGISTER_FLAG][FLAG_Z] <=  ((cpu_register[CPU_REGISTER_R0] | cpu_register[instr_regX]) != 16'b0); // modify Z flag
                                        end
                                    end         
                                endcase                    
                         endcase
                    end
                end
              CPU_STATE_EXECUTE2 : begin
                    case (instr_instr4M)
                           //ccc f 0001 yyyy xxxx
                            //	mov Rx, [Ry] 
                            4'b0001 :
                                begin
                                    if (instr_regX == CPU_REGISTER_PC) begin
                                        `fetch((bus_data_read))
                                    end
                                    else begin
                                        cpu_register[instr_regX] <= bus_data_read;
                                        `fetch((cpu_register[CPU_REGISTER_PC]))
                                    end
                                    if (instr_modflag && instr_regX != CPU_REGISTER_FLAG )  begin
                                        cpu_register[CPU_REGISTER_FLAG][FLAG_Z] <=  (bus_data_read != 16'b0); // modify Z flag
                                    end
                                    cpu_state <= CPU_STATE_DECODE_EXECUTE1;
                                   
                                end
                                                    
                            //ccc f 0010 yyyy xxxx
                            //	add Rx, [Ry]
                            4'b0010 : 
                                begin
                                    if (instr_regX == CPU_REGISTER_PC) begin
                                        `fetch((bus_data_read  + cpu_register[CPU_REGISTER_PC]))
                                    end
                                    else begin
                                        cpu_register[instr_regX] <= cpu_register[instr_regX] + bus_data_read;
                                        `fetch((cpu_register[CPU_REGISTER_PC]))
                                    end
                                    if (instr_modflag && instr_regX != CPU_REGISTER_FLAG )  begin
                                        cpu_register[CPU_REGISTER_FLAG][FLAG_Z] <=  ((bus_data_read + cpu_register[instr_regX]) != 16'b0); // modify Z flag
                                        cpu_register[CPU_REGISTER_FLAG][FLAG_C] <=  (bus_data_read + cpu_register[instr_regX]) < cpu_register[instr_regX];
                                    end                                   
                                    cpu_state <= CPU_STATE_DECODE_EXECUTE1;
                                end 
    
                            
                            //ccc f 0011 yyyy xxxx
                            //	sub Rx, [Ry]
                            4'b0011 :
                                begin
                                   if (instr_regX == CPU_REGISTER_PC) begin
                                        `fetch((bus_data_read  - cpu_register[CPU_REGISTER_PC]))
                                   end
                                   else begin
                                        cpu_register[instr_regX] <= cpu_register[instr_regX] - bus_data_read;
                                        `fetch((cpu_register[CPU_REGISTER_PC]))
                                   end
                                   if (instr_modflag && instr_regX != CPU_REGISTER_FLAG )  begin
                                        cpu_register[CPU_REGISTER_FLAG][FLAG_Z] <=  ((cpu_register[instr_regX] - bus_data_read) != 16'b0); // modify Z flag
                                        cpu_register[CPU_REGISTER_FLAG][FLAG_C] <=  (cpu_register[instr_regX] - bus_data_read ) > cpu_register[instr_regX];
                                   end
                                   cpu_state <= CPU_STATE_DECODE_EXECUTE1;
                                end
                            default:
                                begin
                                    `fetch((cpu_register[CPU_REGISTER_PC]))
                                    cpu_state <= CPU_STATE_DECODE_EXECUTE1;
                                end
                    endcase
                end
              CPU_STATE_AFTER  : begin
                          cpu_state <= cpu_state;
                end
           endcase
      end // if latency_wait == 0
    end 
end 

endmodule
