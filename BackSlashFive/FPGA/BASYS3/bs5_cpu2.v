`timescale 1ns / 1ps

// based on BackSlashFive ISA at https://github.com/redcrab2016/BackSlash-CPU

module bs5_cpu2(input   wire            reset,
                input   wire            bus_clock,
                output  reg             bus_we,
                output  reg             bus_re, 
                output  reg     [15:0]  bus_addr, 
                output  reg     [15:0]  bus_data_write, 
                input   wire    [15:0]  bus_data_read, 
                input   wire            bus_ack,
                input   wire            bus_ready,
                output  reg             cpu_halt, 
                input   wire            cpu_interrupt );
parameter 
    CPU_STATE_FETCH_INSTR_BEGIN     = 14'b00000000000001,
    CPU_STATE_FETCH_INSTR_END       = 14'b00000000000010,
    CPU_STATE_DECODE                = 14'b00000000000100,
    CPU_STATE_FETCH_DATA_BEGIN      = 14'b00000000001000,
    CPU_STATE_FETCH_DATA_END        = 14'b00000000010000,
    CPU_STATE_EXECUTE               = 14'b00000000100000,
    CPU_STATE_WRITE_DATA            = 14'b00000001000000,
    CPU_STATE_SWITCH_INTERRUPT      = 14'b00000010000000,
    CPU_STATE_SWITCH_NORMAL         = 14'b00000100000000,
    CPU_STATE_HALT                  = 14'b00001000000000,
    CPU_STATE_RESET                 = 14'b00010000000000,
    CPU_STATE_RESETING              = 14'b00100000000000,
    CPU_STATE_SWITCHING_INTERRUPT   = 14'b01000000000000,
    CPU_STATE_SWITCHING_NORMAL      = 14'b10000000000000;

parameter // ISA of 31 instructions based on 8 mnemonics 
     // for ccc f xxxx .... .... instruction ( xxx code is enumerated below :  INSTR_MOV_atRX_RY etc... until INSTR_ABOVE
    INSTR_MOV_atRX_RY           = 4'b0000, // mov [Rx], Ry
    INSTR_MOV_RX_atRY           = 4'b0001, // mov Rx, [Ry]
    INSTR_ADD_RX_atRY           = 4'b0010, // add Rx, [Ry]
    INSTR_SUB_RX_atRY           = 4'b0011, // sub Rx, [Ry
    INSTR_MOV_RX_RY             = 4'b0100, // mov Rx, Ry
    INSTR_ADD_RX_RY             = 4'b0101, // add Rx, Ry
    INSTR_SUB_RX_RY             = 4'b0110, // sub Rx, Ry
    INSTR_MOV_LOW_R0_IMM8       = 4'b0111, // mov low R0, Imm8
    INSTR_MOV_HIGH_R0_IMM8      = 4'b1000, // mov high R0, Imm8
    INSTR_ADD_R15_SIMM8         = 4'b1001, // add R15, simm8
    INSTR_MOV_X_RXIMM4          = 4'b1010, // mov X, Rx:Imm4
    INSTR_MOV_RXIMM4_X          = 4'b1011, // mov Rx:Imm4, X
    INSTR_MOV_RXIMM4_0          = 4'b1100, // mov Rx:Imm4, 0
    INSTR_MOV_RXIMM4_1          = 4'b1101, // mov Rx:Imm4, 1
    INSTR_NOT_RXIMM4            = 4'b1110, // not Rx:Imm4

    INSTR_ABOVE                 = 4'b1111,
    // for ccc f 1111 xxxx .... instructions ( xxxx code is enumerated below: INSTR_ADD_R0_IMM4 etc..) 
    INSTR_ADD_R0_IMM4           = 4'b0000, // add R0, Imm4
    INSTR_SUB_R0_IMM4           = 4'b0001, // sub R0, Imm4
    INSTR_SHL_R0_IMM4           = 4'b0010, // shl R0, Imm4
    INSTR_SHR_R0_IMM4           = 4'b0011, // shr R0, Imm4
    INSTR_ADD_RX_1              = 4'b0100, // add Rx, 1
    INSTR_SUB_RX_1              = 4'b0101, // sub Rx, 1
    INSTR_MOV_LOW_RO_LOW_RX     = 4'b0110, // mov low R0, low Rx
    INSTR_MOV_LOW_R0_HIGH_RX    = 4'b0111, // mov low R0, high Rx
    INSTR_MOV_HIGH_R0_LOW_RX    = 4'b1000, // mov high R0, low Rx
    INSTR_MOV_HIGH_R0_HIGH_RX   = 4'b1001, // mov high R0, high Rx
    INSTR_MOV_LOW_RX_0          = 4'b1010, // mov low Rx, 0
    INSTR_MOV_HIGH_RX_0         = 4'b1011, // mov high Rx, 0
    INSTR_MOV_RX_0              = 4'b1100, // mov Rx, 0
    INSTR_NOT_RX                = 4'b1101, // not Rx
    INSTR_AND_R0_RX             = 4'b1110, // and R0, Rx
    INSTR_OR_R0_RX              = 4'b1111; // or R0, Rx

parameter // 8 ISA mnemonicS for 8 ALU
    ALU_OPE_MOV                 = 8'b00000001,
    ALU_OPE_ADD                 = 8'b00000010,
    ALU_OPE_SUB                 = 8'b00000100,
    ALU_OPE_AND                 = 8'b00001000,
    ALU_OPE_OR                  = 8'b00010000,
    ALU_OPE_NOT                 = 8'b00100000,
    ALU_OPE_SHL                 = 8'b01000000,
    ALU_OPE_SHR                 = 8'b10000000;

parameter [4:0]
    WRITE_TYPE_REGISTER_LOW     = 5'b00001, // type value is register number
    WRITE_TYPE_REGISTER_HIGH    = 5'b00010, // type value is register number
    WRITE_TYPE_REGISTER         = 5'b00100, // type value is register number
    WRITE_TYPE_REGISTER_BIT     = 5'b01000, // type value is register number, bit 15:11 is bit number
    WRITE_TYPE_MEMORY           = 5'b10000; // type value is write target address
    
parameter
    CPU_REGISTER_PC             = 4'd15,
    CPU_REGISTER_FLAG           = 4'd14;

parameter   // flags KIHR---- GLASVXZC
    FLAG_C                      = 4'd0, // carry flag
    FLAG_Z                      = 4'd1, // Zero  flag
    FLAG_X                      = 4'd2, // Custom flag (usable for conditional instruction execution)
    FLAG_V                      = 4'd3, // Overflow flag
    FLAG_S                      = 4'd4, // Sign flag
    FLAG_A                      = 4'd5, // Computed result of flags C == 1 && Z == 0
    FLAG_L                      = 4'd6, // Computed result of flags S != V
    FLAF_G                      = 4'd7, // Computed result of flags Z == 0 && S == V
    FLAG_R                      = 4'd12,// Reset flag 
    FLAG_H                      = 4'd13,// Halt flag
    FLAG_I                      = 4'd14,// Interrupt enabled flag
    FLAG_K                      = 4'd15;// Context execution flag (normal or interrupt)
    
wire            cpu_cond_exec [7:0];                    
reg     [15:0]  cpu_register[15:0];
reg     [15:0]  cpu_register_interrupt[15:0];
reg     [15:0]  cpu_register_normal[15:0];
reg     [4:0]   cpu_switch_state        = 0;

reg             bus_ack_latency         = 1;

reg             cpu_interrupt_pending   = 0;
reg             cpu_interrupt_pendingclr= 0;
reg             cpu_interrupt_bysignal  = 0;
reg             cpu_reset_pending       = 0;
reg             cpu_reset_pendingclr    = 0;
reg     [4:0]   cpu_reset_substate      = 0;
reg     [13:0]  cpu_state               = CPU_STATE_FETCH_INSTR_BEGIN;
reg             cpu_instr_modflag       = 0;
reg     [3:0]   cpu_instr_p1            = 0;
reg     [3:0]   cpu_instr_p2            = 0;
reg     [3:0]   cpu_instr_p3            = 0;
reg     [15:0]  cpu_instr_Rx            = 0;
reg     [15:0]  cpu_instr_Ry            = 0;
reg     [15:0]  cpu_instr_R0            = 0;
reg     [15:0]  cpu_instr_PC            = 0;
reg     [15:0]  cpu_instr_FLAG          = 0;

reg     [7:0]   alu_ope                 = 0;
reg     [15:0]  alu_result_value        = 0;
reg     [15:0]  alu_result_flag         = 0;
reg     [15:0]  alu_param1              = 0;
reg     [15:0]  alu_param2              = 0;

reg     [4:0]   write_type              = 0;
reg     [15:0]  write_type_value        = 0;
reg             write_to_flag           = 0;
reg             write_nothing           = 0;

integer i;
initial begin : initialize_cpu_registers
    for (i = 0; i <= 15; i = i + 1) begin
        cpu_register[i]             = 16'b0;
        cpu_register_interrupt[i]   = 16'b0;
        cpu_register_normal[i]      = 16'b0;
    end
    bus_data_write                  = 0;
    cpu_halt                        = 0;
    bus_addr                        = 0;
    bus_re                          = 0;
    bus_we                          = 0;
end 

always @(posedge bus_clock) //* //(cpu_interrupt_pendingclr,cpu_interrupt,cpu_interrupt_pending )
    case ({cpu_interrupt_pending, cpu_interrupt_pendingclr})
        2'b00,
        2'b01: cpu_interrupt_pending <= cpu_interrupt;
        2'b10: cpu_interrupt_pending <= cpu_interrupt_pending;
        2'b11: cpu_interrupt_pending <= 0;
    endcase

always @(posedge bus_clock) //* //(cpu_reset_pendingclr, reset,cpu_reset_pending)
    case ({cpu_reset_pending,cpu_reset_pendingclr})
        2'b00,
        2'b01: cpu_reset_pending <= ~reset;
        2'b10: cpu_reset_pending <= cpu_reset_pending;
        2'b11: cpu_reset_pending <= 0;
    endcase

// conditional execution : BS5 ccc
assign cpu_cond_exec[0] = 1'b1; // always
assign cpu_cond_exec[1] =  cpu_register[CPU_REGISTER_FLAG][FLAG_Z]; // Z==1
assign cpu_cond_exec[2] = ~(cpu_register[CPU_REGISTER_FLAG][FLAG_Z]); // Z==0
assign cpu_cond_exec[3] =  cpu_register[CPU_REGISTER_FLAG][FLAG_C]; // C==1
assign cpu_cond_exec[4] = ~(cpu_register[CPU_REGISTER_FLAG][FLAG_C]); // C==0
assign cpu_cond_exec[5] =  cpu_register[CPU_REGISTER_FLAG][FLAG_X]; // X==1
assign cpu_cond_exec[6] = ~(cpu_register[CPU_REGISTER_FLAG][FLAG_X]); // X==0
assign cpu_cond_exec[7] = 1'b1;  // execute but no write back result

always @ (posedge bus_clock) 
    case (cpu_state)
        CPU_STATE_FETCH_INSTR_BEGIN:
            begin
                bus_we          <= 0;
                bus_ack_latency <= 1;
                if (bus_ready & bus_ack_latency) begin
                    bus_re                               <= 1;
                    bus_addr                             <= cpu_register[CPU_REGISTER_PC];
                    cpu_register[CPU_REGISTER_PC]        <= cpu_register[CPU_REGISTER_PC] + 1;
                    cpu_instr_R0                         <= cpu_register[0];
                    cpu_instr_PC                         <= cpu_register[CPU_REGISTER_PC] + 1;
                    // Compute G,L,A flags
                    cpu_register[CPU_REGISTER_FLAG][7:5] <= { 
                                                                ~(cpu_register[CPU_REGISTER_FLAG][FLAG_Z]) & (  cpu_register[CPU_REGISTER_FLAG][FLAG_S] == cpu_register[CPU_REGISTER_FLAG][FLAG_V] ), // G flag algebra
                                                                cpu_register[CPU_REGISTER_FLAG][FLAG_S] != cpu_register[CPU_REGISTER_FLAG][FLAG_V],                                                   // L flag algebra
                                                                cpu_register[CPU_REGISTER_FLAG][FLAG_C] & ~(cpu_register[CPU_REGISTER_FLAG][FLAG_Z])                                                  // A flag algebra 
                                                            }; 
                    bus_ack_latency                      <= 0;
                    case ({cpu_reset_pending, cpu_interrupt_pending})
                        {1'b1,1'b0},                                    
                        {1'b1,1'b1}: 
                            begin 
                                cpu_state               <= CPU_STATE_RESET;  // priority to reset signal
                                cpu_interrupt_bysignal  <= 0;
                            end
                        {1'b0,1'b1}: 
                            begin
                                cpu_state               <= cpu_register[CPU_REGISTER_FLAG][FLAG_K]?CPU_STATE_FETCH_INSTR_END:(cpu_register[CPU_REGISTER_FLAG][FLAG_I]?CPU_STATE_SWITCH_INTERRUPT:CPU_STATE_FETCH_INSTR_END);// interrupt only if we in normal context
                                cpu_interrupt_bysignal  <= cpu_register[CPU_REGISTER_FLAG][FLAG_K]?0:1; 
                            end
                        {1'b0,1'b0}: 
                            begin
                                cpu_state      <= CPU_STATE_FETCH_INSTR_END;
                                cpu_interrupt_bysignal  <= 0;
                            end
                    endcase
                end 
            end
        CPU_STATE_FETCH_INSTR_END:
            begin
                bus_re          <= 0;
                bus_ack_latency <= 1;
                if (bus_ack && bus_ack_latency ) begin
                    cpu_instr_FLAG     <= cpu_register[CPU_REGISTER_FLAG];
                    cpu_instr_modflag  <= bus_data_read[12];
                    cpu_instr_p1       <= bus_data_read[11:8]; // instr
                    cpu_instr_p2       <= bus_data_read[7:4];  // instr above  or Ry register number or MSQ simm8 or MSQ imm8 or imm4
                    cpu_instr_p3       <= bus_data_read[3:0]; //  Rx register number or LSQ simm8 or LSQ imm8
                    cpu_instr_Rx       <= cpu_register[bus_data_read[3:0]];
                    cpu_instr_Ry       <= cpu_register[bus_data_read[7:4]];
                    cpu_state          <= cpu_cond_exec[ bus_data_read[15:13] ]? CPU_STATE_DECODE : CPU_STATE_FETCH_INSTR_BEGIN;
                    alu_param1         <= 0;
                    alu_param2         <= 0;
                    alu_result_value   <= 0;
                    write_type         <= 0;
                    write_type_value   <= 0;
                    write_to_flag      <= 0;
                    write_nothing      <= (bus_data_read[15:13] == 3'b111)?1:0;  // if 1 then no write result of the instruction
                end 
            end 
        CPU_STATE_DECODE:
            begin
                alu_result_flag <= cpu_instr_FLAG; //cpu_register[CPU_REGISTER_FLAG];
                case (cpu_instr_p1)
                    //ccc f 0000 yyyy xxxx
                    //	mov [Rx], Ry
                    INSTR_MOV_atRX_RY:
                        begin
                            alu_ope             <= ALU_OPE_MOV;
                            alu_result_value    <= cpu_instr_Ry; //cpu_register[cpu_instr_p2];
                            write_type          <= WRITE_TYPE_MEMORY;
                            write_type_value    <= cpu_instr_Rx; //cpu_register[cpu_instr_p3]; // address to store the value to
                            cpu_state           <= CPU_STATE_WRITE_DATA; 
                        end

                    //ccc f 0001 yyyy xxxx
                    //	mov Rx, [Ry] 
                    INSTR_MOV_RX_atRY:
                        begin
                            alu_ope             <= ALU_OPE_MOV;
                            alu_param2          <= cpu_instr_Ry; //cpu_register[cpu_instr_p2]; // temp value is the address where to fetch the value from bus
                            write_type          <= WRITE_TYPE_REGISTER;
                            write_type_value    <= {12'b0,cpu_instr_p3}; // the Rx register number
                            cpu_state           <= CPU_STATE_FETCH_DATA_BEGIN; // replace alu_param2 address by the content at this address
                            write_to_flag       <= (cpu_instr_p3 == CPU_REGISTER_FLAG) ? 1: 0;
                        end
                    
                    //ccc f 0010 yyyy xxxx
                    //	add Rx, [Ry]
                    INSTR_ADD_RX_atRY:
                        begin
                            alu_ope             <= ALU_OPE_ADD;
                            alu_param1          <= cpu_instr_Rx;
                            alu_param2          <= cpu_instr_Ry; //cpu_register[cpu_instr_p2]; // temp value is the address where to fetch the value from bus
                            write_type          <= WRITE_TYPE_REGISTER;
                            write_type_value    <= {12'b0,cpu_instr_p3}; // the Rx register number
                            cpu_state           <= CPU_STATE_FETCH_DATA_BEGIN; // replace alu_param2 address by the content at this address
                            write_to_flag       <= (cpu_instr_p3 == CPU_REGISTER_FLAG) ? 1: 0;
                        end
                    
                    //ccc f 0011 yyyy xxxx
                    //	sub Rx, [Ry]
                    INSTR_SUB_RX_atRY:
                        begin
                            alu_ope             <= ALU_OPE_SUB;
                            alu_param1          <= cpu_instr_Rx;
                            alu_param2          <= cpu_instr_Ry; //cpu_register[cpu_instr_p2]; // temp value is the address where to fetch the value from bus
                            write_type          <= WRITE_TYPE_REGISTER;
                            write_type_value    <= {12'b0,cpu_instr_p3}; // the register number
                            cpu_state           <= CPU_STATE_FETCH_DATA_BEGIN; // replace alu_param2 address by the content at this address
                            write_to_flag       <= (cpu_instr_p3 == CPU_REGISTER_FLAG) ? 1: 0;
                        end
                    
                    //ccc f 0100 yyyy xxxx
                    //	mov Rx, Ry
                    INSTR_MOV_RX_RY :
                        begin
                            alu_ope             <= ALU_OPE_MOV;
                            alu_result_value    <= cpu_instr_Ry; //cpu_register[cpu_instr_p2];
                            write_type          <= WRITE_TYPE_REGISTER;
                            write_type_value    <= {12'b0,cpu_instr_p3}; // the register number
                            cpu_state           <= CPU_STATE_WRITE_DATA;
                            write_to_flag       <= (cpu_instr_p3 == CPU_REGISTER_FLAG) ? 1: 0;
                        end
                    
                    //ccc f 0101 yyyy xxxx
                    //	add Rx, Ry
                    INSTR_ADD_RX_RY:
                        begin
                            alu_ope             <= ALU_OPE_ADD;
                            alu_param1          <= cpu_instr_Rx; //cpu_register[cpu_instr_p3]; // Rx
                            alu_param2          <= cpu_instr_Ry; //cpu_register[cpu_instr_p2]; // Ry
                            write_type          <= WRITE_TYPE_REGISTER;
                            write_type_value    <= {12'b0,cpu_instr_p3}; // the register number
                            cpu_state           <= CPU_STATE_EXECUTE;
                            write_to_flag       <= (cpu_instr_p3 == CPU_REGISTER_FLAG) ? 1: 0;
                        end
                    
                    //ccc f 0110 yyyy xxxx
                    //	sub Rx, Ry
                    INSTR_SUB_RX_RY:
                        begin
                            alu_ope             <= ALU_OPE_SUB;
                            alu_param1          <= cpu_instr_Rx; //cpu_register[cpu_instr_p3]; // Rx
                            alu_param2          <= cpu_instr_Ry; //cpu_register[cpu_instr_p2]; // Ry
                            write_type          <= WRITE_TYPE_REGISTER;
                            write_type_value    <= {12'b0,cpu_instr_p3}; // the register number
                            cpu_state           <= CPU_STATE_EXECUTE;
                            write_to_flag       <= (cpu_instr_p3 == CPU_REGISTER_FLAG) ? 1: 0;
                        end
                    
                    //ccc f 0111 iiii iiii
                    //	mov low R0, imm8
                    INSTR_MOV_LOW_R0_IMM8:
                        begin
                            alu_ope             <= ALU_OPE_MOV;
                            alu_result_value    <= {8'b0,cpu_instr_p2, cpu_instr_p3};
                            write_type          <= WRITE_TYPE_REGISTER_LOW;
                            write_type_value    <= {16'b0}; // register number 0
                            cpu_state           <= CPU_STATE_WRITE_DATA;
                        end
                    
                    //ccc f 1000 iiii iiii
                    //	mov high R0, imm8
                    INSTR_MOV_HIGH_R0_IMM8:
                        begin
                            alu_ope <= ALU_OPE_MOV;
                            alu_result_value    <= {8'b0,cpu_instr_p2, cpu_instr_p3};
                            write_type          <= WRITE_TYPE_REGISTER_HIGH;
                            write_type_value    <= {16'b0}; // register number 0
                            cpu_state           <= CPU_STATE_WRITE_DATA;
                        end
                    
                    //ccc f 1001 siii iiii
                    //	add R15, simm8
                    INSTR_ADD_R15_SIMM8:
                        begin
                            alu_ope             <= ALU_OPE_ADD;
                            alu_param1          <= cpu_instr_PC; //cpu_register[CPU_REGISTER_PC];
                            alu_param2          <= {cpu_instr_p2[3],cpu_instr_p2[3],cpu_instr_p2[3],cpu_instr_p2[3],cpu_instr_p2[3],cpu_instr_p2[3],cpu_instr_p2[3],cpu_instr_p2[3],cpu_instr_p2,cpu_instr_p3}; // extend the sign of 8 bit to 16 bits
                            write_type          <= WRITE_TYPE_REGISTER;
                            write_type_value    <= 16'd15; // R15 register number
                            cpu_state           <= CPU_STATE_EXECUTE;
                        end
                    
                    //ccc f 1010 iiii xxxx
                    //	mov X, Rx:imm4
                    INSTR_MOV_X_RXIMM4 :
                        begin
                            alu_ope             <= ALU_OPE_MOV;
                            alu_result_value    <= {15'b0, cpu_instr_Rx[cpu_instr_p2]}; //cpu_register[cpu_instr_p3][cpu_instr_p2]};
                            write_type          <= WRITE_TYPE_REGISTER_BIT;
                            write_type_value    <= {4'b0010,8'b0,CPU_REGISTER_FLAG}; // imm4=2 , 0, R14  (X flag is the 3rd bit in Flag register)
                            cpu_state           <= CPU_STATE_WRITE_DATA;
                            write_to_flag       <= (cpu_instr_p3 == CPU_REGISTER_FLAG) ? 1: 0;
                        end
                    
                    //ccc f 1011 iiii xxxx
                    //	mov Rx:imm4, X
                    INSTR_MOV_RXIMM4_X:
                        begin
                            alu_ope             <= ALU_OPE_MOV;
                            alu_result_value    <= {15'b0, cpu_instr_FLAG[FLAG_X]}; //cpu_register[CPU_REGISTER_FLAG][FLAG_X]};
                            write_type          <= WRITE_TYPE_REGISTER_BIT;
                            write_type_value    <= {cpu_instr_p2,8'b0,cpu_instr_p3};
                            cpu_state           <= CPU_STATE_WRITE_DATA;
                            write_to_flag       <= (cpu_instr_p3 == CPU_REGISTER_FLAG) ? 1: 0;
                        end
                    
                    //ccc f 1100 iiii xxxx
                    //	mov Rx:imm4, 0
                    INSTR_MOV_RXIMM4_0,
                    //ccc f 1101 iiii xxxx
                    //	mov Rx:imm4, 1
                    INSTR_MOV_RXIMM4_1:
                        begin
                            alu_ope             <= ALU_OPE_MOV;
                            alu_result_value    <= {15'b0, cpu_instr_p1[0]};
                            write_type          <= WRITE_TYPE_REGISTER_BIT;
                            write_type_value    <= {cpu_instr_p2,8'b0,cpu_instr_p3};
                            cpu_state           <= CPU_STATE_WRITE_DATA;
                            write_to_flag       <= (cpu_instr_p3 == CPU_REGISTER_FLAG) ? 1: 0;
                        end
                    
                    //ccc f 1110 iiii xxxx
                    //	not Rx:imm4
                    INSTR_NOT_RXIMM4:
                        begin
                            alu_ope             <= ALU_OPE_NOT;
                            alu_param1          <= {15'b0, cpu_instr_Rx[cpu_instr_p2]}; // cpu_register[cpu_instr_p3][cpu_instr_p2]};
                            write_type          <= WRITE_TYPE_REGISTER_BIT;
                            write_type_value    <= {cpu_instr_p2, 8'b0, cpu_instr_p3}; // bit number cpu_instr_p2 on register number cpu_instr_p3
                            cpu_state           <= CPU_STATE_EXECUTE;
                            write_to_flag       <= (cpu_instr_p3 == CPU_REGISTER_FLAG) ? 1: 0;
                        end
                    
                    INSTR_ABOVE:
                        case (cpu_instr_p2)
                            //ccc f 1111 0000 iiii
                            //	add R0, imm4
                            INSTR_ADD_R0_IMM4:
                                begin
                                    alu_ope             <= ALU_OPE_ADD;
                                    alu_param1          <= cpu_instr_R0; //cpu_register[0];
                                    alu_param2          <= {12'b0,cpu_instr_p3};
                                    write_type          <= WRITE_TYPE_REGISTER;
                                    write_type_value    <= 0 ; // R0 register number
                                    cpu_state           <= CPU_STATE_EXECUTE; 
                                end
                            
                            //ccc f 1111 0001 iiii
                            //	sub R0, imm4
                            INSTR_SUB_R0_IMM4:
                                begin
                                    alu_ope             <= ALU_OPE_SUB;
                                    alu_param1          <= cpu_instr_R0; //cpu_register[0];
                                    alu_param2          <= {12'b0,cpu_instr_p3};
                                    write_type          <= WRITE_TYPE_REGISTER;
                                    write_type_value    <= 0 ; // R0 register number
                                    cpu_state           <= CPU_STATE_EXECUTE; 
                                end
                            
                            //ccc f 1111 0010 iiii
                            //	shl R0, imm4
                            INSTR_SHL_R0_IMM4:
                                begin
                                    alu_ope             <= ALU_OPE_SHL;
                                    alu_param1          <= cpu_instr_R0; //cpu_register[0];
                                    alu_param2          <= {12'b0,cpu_instr_p3};
                                    write_type          <= WRITE_TYPE_REGISTER;
                                    write_type_value    <= 0 ; // R0 register number
                                    cpu_state           <= CPU_STATE_EXECUTE; 
                                end
                            
                            //ccc f 1111 0011 iiii
                            //	shr R0, imm4
                            INSTR_SHR_R0_IMM4:
                                begin
                                    alu_ope <= ALU_OPE_SHR;
                                    alu_param1          <= cpu_instr_R0; //cpu_register[0];
                                    alu_param2          <= {12'b0,cpu_instr_p3};
                                    write_type          <= WRITE_TYPE_REGISTER;
                                    write_type_value    <= 0 ; // R0 register number
                                    cpu_state           <= CPU_STATE_EXECUTE; 
                                end
                            
                            //1 register
                            //ccc f 1111 0100 xxxx
                            //	add Rx, 1
                            INSTR_ADD_RX_1 :
                                begin
                                    alu_ope             <= ALU_OPE_ADD;
                                    alu_param1          <= cpu_instr_Rx; //cpu_register[cpu_instr_p3];
                                    alu_param2          <= {15'b0, 1'b1};
                                    write_type          <= WRITE_TYPE_REGISTER;
                                    write_type_value    <= {12'b0, cpu_instr_p3}; // Rx register number
                                    write_to_flag       <= (cpu_instr_p3 == CPU_REGISTER_FLAG) ? 1: 0;
                                    cpu_state           <= CPU_STATE_EXECUTE;
                                end
                            
                            //ccc f 1111 0101 xxxx
                            //	sub Rx, 1
                            INSTR_SUB_RX_1:
                                begin
                                    alu_ope             <= ALU_OPE_SUB;
                                    alu_param1          <= cpu_instr_Rx; //cpu_register[cpu_instr_p3];
                                    alu_param2          <= {15'b0, 1'b1};
                                    write_type          <= WRITE_TYPE_REGISTER;
                                    write_type_value    <= {12'b0, cpu_instr_p3}; // Rx register number
                                    write_to_flag       <= (cpu_instr_p3 == CPU_REGISTER_FLAG) ? 1: 0;
                                    cpu_state           <= CPU_STATE_EXECUTE;
                                end
                            
                            //ccc f 1111 0110 xxxx
                            //	mov low R0, low Rx
                            INSTR_MOV_LOW_RO_LOW_RX:
                                begin
                                    alu_ope             <= ALU_OPE_MOV;
                                    alu_result_value    <= {8'b0, cpu_instr_Rx[7:0]};
                                    write_type          <= WRITE_TYPE_REGISTER_LOW;
                                    write_type_value    <= 0; // R0 register number
                                    cpu_state           <= CPU_STATE_WRITE_DATA;
                                end
                            
                            //ccc f 1111 0111 xxxx
                            //	mov low R0, high Rx
                            INSTR_MOV_LOW_R0_HIGH_RX:
                                begin
                                    alu_ope             <= ALU_OPE_MOV;
                                    alu_result_value    <= {8'b0, cpu_instr_Rx[15:8]};
                                    write_type          <= WRITE_TYPE_REGISTER_LOW;
                                    write_type_value    <= 0; // R0 register number
                                    cpu_state           <= CPU_STATE_WRITE_DATA;
                                end
                            
                            //ccc f 1111 1000 xxxx
                            //	mov high R0, low Rx
                            INSTR_MOV_HIGH_R0_LOW_RX:
                                begin
                                    alu_ope             <= ALU_OPE_MOV;
                                    alu_result_value    <= {8'b0, cpu_instr_Rx[7:0]};
                                    write_type          <= WRITE_TYPE_REGISTER_HIGH;
                                    write_type_value    <= 0; // R0 register number
                                    cpu_state           <= CPU_STATE_WRITE_DATA;
                                end
                            
                            //ccc f 1111 1001 xxxx
                            //	mov high R0, high Rx
                            INSTR_MOV_HIGH_R0_HIGH_RX:
                                begin
                                    alu_ope             <= ALU_OPE_MOV;
                                    alu_result_value    <= {8'b0, cpu_instr_Rx[15:8]};
                                    write_type          <= WRITE_TYPE_REGISTER_HIGH;
                                    write_type_value    <= 0; // R0 register number
                                    cpu_state           <= CPU_STATE_WRITE_DATA;
                                end
                            
                            //ccc f 1111 1010 xxxx
                            //	mov low Rx, 0
                            INSTR_MOV_LOW_RX_0:
                                begin
                                    alu_ope <= ALU_OPE_MOV;
                                    alu_result_value    <= 0; // value 0
                                    write_type          <= WRITE_TYPE_REGISTER_LOW;
                                    write_type_value    <= {12'b0, cpu_instr_p3}; // Rx register number
                                    write_to_flag       <= (cpu_instr_p3 == CPU_REGISTER_FLAG) ? 1: 0;
                                    cpu_state           <= CPU_STATE_WRITE_DATA;
                                end
                            
                            //ccc f 1111 1011 xxxx
                            //	mov high Rx, 0
                            INSTR_MOV_HIGH_RX_0:
                                begin
                                    alu_ope             <= ALU_OPE_MOV;
                                    alu_result_value    <= 0; // value 0
                                    write_type          <= WRITE_TYPE_REGISTER_HIGH;
                                    write_type_value    <= {12'b0, cpu_instr_p3}; // Rx register number
                                    write_to_flag       <= (cpu_instr_p3 == CPU_REGISTER_FLAG) ? 1: 0;
                                    cpu_state           <= CPU_STATE_WRITE_DATA;
                                end
                            
                            //ccc f 1111 1100 xxxx
                            //	mov Rx, 0
                            INSTR_MOV_RX_0 :
                                begin
                                    alu_ope             <= ALU_OPE_MOV;
                                    alu_result_value    <= 0; // value 0
                                    write_type          <= WRITE_TYPE_REGISTER;
                                    write_type_value    <= {12'b0, cpu_instr_p3}; // Rx register number
                                    write_to_flag       <= (cpu_instr_p3 == CPU_REGISTER_FLAG) ? 1: 0;
                                    cpu_state           <= CPU_STATE_WRITE_DATA;
                                end
                            
                            //ccc f 1111 1101 xxxx
                            //	not Rx
                            INSTR_NOT_RX :
                                begin
                                    alu_ope             <= ALU_OPE_NOT;
                                    alu_result_value    <= cpu_instr_Rx; // value of Rx
                                    write_type          <= WRITE_TYPE_REGISTER;
                                    write_type_value    <= {12'b0, cpu_instr_p3}; // Rx register number
                                    write_to_flag       <= (cpu_instr_p3 == CPU_REGISTER_FLAG) ? 1: 0;
                                    cpu_state           <= CPU_STATE_EXECUTE;
                                end
                            
                            //ccc f 1111 1110 xxxx
                            //	and R0, Rx
                            INSTR_AND_R0_RX:
                                begin
                                    alu_ope             <= ALU_OPE_AND;
                                    alu_param1          <= cpu_instr_R0; //cpu_register[0]; // value of R0
                                    alu_param2          <= cpu_instr_Rx; // value of Rx
                                    write_type          <= WRITE_TYPE_REGISTER;
                                    write_type_value    <= 0; // R0 register number
                                    cpu_state           <= CPU_STATE_EXECUTE;
                                end
                            
                            //ccc f 1111 1111 xxxx
                            //	or  R0, Rx
                            INSTR_OR_R0_RX:
                                begin
                                    alu_ope             <= ALU_OPE_OR;
                                    alu_param1          <= cpu_instr_R0; //cpu_register[0]; // value of R0
                                    alu_param2          <= cpu_instr_Rx; // value of Rx
                                    write_type          <= WRITE_TYPE_REGISTER;
                                    write_type_value    <= 0; // R0 register number
                                    cpu_state           <= CPU_STATE_EXECUTE;
                                end
                    endcase 
                endcase
            end
        CPU_STATE_FETCH_DATA_BEGIN:
            if (bus_ready) begin
                bus_re          <= 1;
                bus_addr        <= alu_param2;
                cpu_state       <= CPU_STATE_FETCH_DATA_END;
                bus_ack_latency <= 0;
            end 
        CPU_STATE_FETCH_DATA_END:
            begin
                bus_re          <= 0;
                bus_ack_latency <= 1;
                if (bus_ack & bus_ack_latency) begin
                    alu_param2          <= bus_data_read;
                    alu_result_value    <= bus_data_read; // for ALE_OPE_MOV, will be overwritten at CPU_STATE_EXECUTE for the other alu_ope
                    cpu_state           <= (alu_ope == ALU_OPE_MOV) ? CPU_STATE_WRITE_DATA : CPU_STATE_EXECUTE;
                end
            end 
        CPU_STATE_EXECUTE: // execute the ALU operation and take care of C flag (stored in alu_result_flag)
            begin
                case (alu_ope)
                    ALU_OPE_MOV: alu_result_value <= alu_result_value ;// should be useless here, because every mov decoding instruction do step to CPU_STATE_WRITE_DATA
                    ALU_OPE_ADD: {alu_result_flag[FLAG_C],alu_result_value}  <=  alu_param1 +  alu_param2;
                    ALU_OPE_SUB: begin
                                 {alu_result_flag[FLAG_C],alu_result_value}  <=  alu_param1 -  alu_param2; // TODO : check the C flag rule with a substraction
                                 alu_param2                                  <=  (~alu_param2)+1;
                                 end
                    ALU_OPE_AND: alu_result_value                            <=  alu_param1 &  alu_param2;
                    ALU_OPE_OR:  alu_result_value                            <=  alu_param1 &  alu_param2;
                    ALU_OPE_NOT: alu_result_value                            <= ~alu_param1;
                    ALU_OPE_SHL: {alu_result_flag[FLAG_C],alu_result_value}  <=  {1'b0,alu_param1} << alu_param2;
                    ALU_OPE_SHR: {alu_result_value,alu_result_flag[FLAG_C]}  <=  {alu_param1,1'b0} >> alu_param2;
                endcase
                cpu_state <= CPU_STATE_WRITE_DATA;
            end
        CPU_STATE_WRITE_DATA: // this step write the result and take care of the Z flag result, K flag from 0 to 1 and from 1 to 0 (go to interrupt, or go to normal)
            begin
                if (~write_nothing) begin // if the result of the operation has to be written
                    case (write_type)
                        WRITE_TYPE_REGISTER_LOW:  cpu_register[write_type_value[3:0]][7:0]                      <= alu_result_value[7:0];  // type value is register number
                        WRITE_TYPE_REGISTER_HIGH: cpu_register[write_type_value[3:0]][15:8]                     <= alu_result_value[7:0];  // type value is register number
                        WRITE_TYPE_REGISTER:      cpu_register[write_type_value[3:0]]                           <= alu_result_value;       // type value is register number
                        WRITE_TYPE_REGISTER_BIT:  cpu_register[write_type_value[3:0]][write_type_value[15:11]]  <= alu_result_value[0];    // type value is register number, bit 15:11 is bit number
                        WRITE_TYPE_MEMORY: // type value is write target address
                            if (bus_ready) begin
                                bus_we          <= 1;
                                bus_addr        <= write_type_value;
                                bus_data_write  <= alu_result_value;
                                bus_ack_latency <= 0;
                            end 
                    endcase
                    case ({write_to_flag,write_type})
                        {1'b0,WRITE_TYPE_MEMORY}       ,
                        {1'b1,WRITE_TYPE_MEMORY}       : cpu_state <= bus_ready?CPU_STATE_FETCH_INSTR_BEGIN:CPU_STATE_WRITE_DATA;
                        {1'b0,WRITE_TYPE_REGISTER_LOW} ,
                        {1'b0,WRITE_TYPE_REGISTER_HIGH},
                        {1'b0,WRITE_TYPE_REGISTER}     ,
                        {1'b0,WRITE_TYPE_REGISTER_BIT} ,
                        {1'b1,WRITE_TYPE_REGISTER_LOW} : cpu_state <= CPU_STATE_FETCH_INSTR_BEGIN;
                        {1'b1,WRITE_TYPE_REGISTER_HIGH}: cpu_state <= (alu_result_value[FLAG_K-8]!=alu_result_flag[FLAG_K])?(alu_result_value[FLAG_K-8]?CPU_STATE_SWITCH_INTERRUPT:CPU_STATE_SWITCH_NORMAL):CPU_STATE_FETCH_INSTR_BEGIN;
                        {1'b1,WRITE_TYPE_REGISTER}     : cpu_state <= (alu_result_value[FLAG_K  ]!=alu_result_flag[FLAG_K])?(alu_result_value[FLAG_K  ]?CPU_STATE_SWITCH_INTERRUPT:CPU_STATE_SWITCH_NORMAL):CPU_STATE_FETCH_INSTR_BEGIN;
                        {1'b1,WRITE_TYPE_REGISTER_BIT} :
                            begin
                                case (write_type_value[15:12])
                                    FLAG_K: cpu_state <= (alu_result_value[0]!=alu_result_flag[FLAG_K])?(alu_result_value[0]?CPU_STATE_SWITCH_INTERRUPT:CPU_STATE_SWITCH_NORMAL):CPU_STATE_FETCH_INSTR_BEGIN;
                                    FLAG_H: 
                                        begin
                                            cpu_halt <= alu_result_value[0];
                                            cpu_state <= (alu_result_value[0])?CPU_STATE_HALT:CPU_STATE_FETCH_INSTR_BEGIN;
                                        end
                                    FLAG_R: cpu_state <= (alu_result_value[0])?CPU_STATE_RESET:CPU_STATE_FETCH_INSTR_BEGIN;
                                endcase 
                            end                 
                    endcase 
                end
                else begin  // result not written to target 
                    cpu_state <= CPU_STATE_FETCH_INSTR_BEGIN;
                end
                if (cpu_instr_modflag && !write_to_flag) begin   // flags has to be modified 
                    // Z and C flags
                    cpu_register[CPU_REGISTER_FLAG][1:0] <= {(alu_result_value==0)/*Z*/, alu_result_flag[0] /*C*/};
                    // S and V flags
                    case (alu_ope)
                        ALU_OPE_MOV: // S flag only
                            case (write_type)
                                WRITE_TYPE_REGISTER_LOW:    cpu_register[CPU_REGISTER_FLAG][4]  <= cpu_register[write_type_value[3:0]][15];
                                WRITE_TYPE_REGISTER_HIGH:   cpu_register[CPU_REGISTER_FLAG][4]  <= alu_result_value[7];
                                WRITE_TYPE_REGISTER,
                                WRITE_TYPE_MEMORY:          cpu_register[CPU_REGISTER_FLAG][4]  <= alu_result_value[15];
                            endcase
                        ALU_OPE_ADD,
                        ALU_OPE_SUB: cpu_register[CPU_REGISTER_FLAG][4:3]        <= { alu_result_value[15], (alu_param1[15] & alu_param2[15] & ~alu_result_value[15]) | (~alu_param1[15] & ~alu_param2[15] & alu_result_value[15]) };
                        ALU_OPE_AND,
                        ALU_OPE_OR,
                        ALU_OPE_NOT,
                        ALU_OPE_SHL,
                        ALU_OPE_SHR: cpu_register[CPU_REGISTER_FLAG][4]          <=  alu_result_value[15];
                    endcase                    
                end 
            end
        CPU_STATE_SWITCH_INTERRUPT:
            begin
                cpu_switch_state            <= 0;
                cpu_state                   <= CPU_STATE_SWITCHING_INTERRUPT;
                cpu_halt                    <= 1; // show to the external world that cpu can't respond for the moment
                cpu_interrupt_pendingclr    <= 1;
            end
        CPU_STATE_SWITCH_NORMAL:
            begin
                cpu_switch_state    <= 0;
                cpu_state           <= CPU_STATE_SWITCHING_NORMAL;
                cpu_halt            <= 1; // show to the external world that cpu can't respond for the moment
            end
        CPU_STATE_SWITCHING_INTERRUPT:
            begin
                case (cpu_switch_state)
                    0,1,2,3,4,5,6,7,8,9,10,11,12,13,15:
                    begin
                        cpu_register_normal[cpu_switch_state]   <= cpu_register[cpu_switch_state] ;
                        cpu_register[cpu_switch_state]          <= cpu_register_interrupt[cpu_switch_state];
                    end
                    14:
                    begin
                        cpu_register_normal[14]                 <= {1'b0,cpu_register[14][14:0]};
                        cpu_register[14]                        <= {1'b1,cpu_interrupt_bysignal,cpu_register_interrupt[14][13:0]};
                    end 
                    default:
                    begin
                        cpu_state                               <= CPU_STATE_FETCH_INSTR_BEGIN;
                        //if (cpu_interrupt_pending)   cpu_interrupt_pending <= 0; //      <= cpu_interrupt_pending?0:1'bZ;
                        cpu_interrupt_pendingclr                <= 0;
                        cpu_halt                                <= 0; // show to the external world that cpu is available 
                    end
                endcase
                cpu_switch_state <= cpu_switch_state + 1;
            end
        CPU_STATE_SWITCHING_NORMAL:
            begin
                bus_re  <= 0;
                bus_we  <= 0;
                case (cpu_switch_state)
                    0,1,2,3,4,5,6,7,8,9,10,11,12,13,15:
                    begin
                        cpu_register_interrupt[cpu_switch_state]    <= cpu_register[cpu_switch_state] ;
                        cpu_register[cpu_switch_state]              <= cpu_register_normal[cpu_switch_state];
                    end
                    14:
                    begin
                        cpu_register_interrupt[14]                  <= {1'b1,cpu_register[14][14:0]};
                        cpu_register[14]                            <= {1'b0,cpu_register_normal[14][14:0]};
                    end 
                    default:
                    begin
                        cpu_state                                   <= CPU_STATE_FETCH_INSTR_BEGIN;
                        cpu_halt                                    <= 0; // show to the external world that cpu is available
                    end
                endcase
                cpu_switch_state <= cpu_switch_state + 1; 
            end
        CPU_STATE_HALT:
            begin
                cpu_state   <= (cpu_reset_pending)?CPU_STATE_RESET:CPU_STATE_HALT;
                bus_re      <= 0;
                bus_re      <= 0;
            end
        CPU_STATE_RESET:
            begin
                cpu_halt                    <= 1; // show to the external world that cpu can't respond for the moment
                cpu_state                   <= CPU_STATE_RESETING;
                cpu_reset_substate          <= 0;
                bus_re                      <= 0;
                cpu_reset_pendingclr        <= 1;
                cpu_interrupt_pendingclr    <= 1;
            end            
        CPU_STATE_RESETING:
            begin
                case (cpu_reset_substate)
                    0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15:
                        begin
                            cpu_register[cpu_reset_substate]             <= 16'b0;
                            cpu_register_interrupt[cpu_reset_substate]   <= 16'b0;
                            cpu_register_normal[cpu_reset_substate]      <= 16'b0;
                        end 
                    default:
                        begin
                            cpu_halt                                    <= 0;
                            cpu_state                                   <= CPU_STATE_FETCH_INSTR_BEGIN;
                            cpu_reset_pendingclr                        <= 0;
                            cpu_interrupt_pendingclr                    <= 0;

                        end
                endcase
                cpu_reset_substate <= cpu_reset_substate + 1;
            end            
    endcase
endmodule
