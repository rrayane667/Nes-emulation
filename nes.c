#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

//sets the address of the required location
uint16_t *address_bus;

//read or write selected data
uint8_t *data_bus;

//contains info on whethert to read or write
uint8_t *control_bus;

//program counter
uint16_t pc;

//stack pointer
uint8_t sp;

//number of cycles needed to execute the instruction
uint8_t cycles = 0;

//registers
uint8_t A;
uint8_t X;
uint8_t Y;
union {
    struct b{
        char C : 1;
        char Z : 1;
        char I : 1;
        char D : 1;
        char B : 1;
        char WALOO : 1;
        char V : 1;
        char N : 1;
    } bits;
    uint8_t status;
} processorStatus;

enum FLAG6502{
    C = (1<<0),
    Z = (1<<1),
    I = (1<<2),
    D = (1<<3),
    B = (1<<4),
    WALOO = (1<<5),
    V = (1<<6),
    N = (1<<7),
};

void writeMem(uint16_t addr, uint8_t value);
uint8_t readMem(uint16_t addr);

void pushStack(uint8_t v){
    writeMem(0x0100 + sp, v);
    sp--;
}
uint8_t pullStack(){
    uint8_t v = readMem(0x0100 + sp);
    sp++;
    return v;
}




void decodeOpcode(uint8_t opCode){
    switch (opCode) {
        case 0x00 : BRK(); break;
        case 0x10 : BPL(); break;
        case 0x20 : JSR(); break;
        case 0x30 : BMI(); break;
        case 0x40 : RTI(); break;
        case 0x50 : BVC(); break;
        case 0x60 : RTS(); break;
        case 0x70 : BVS(); break;
        case 0x90 : BCC(); break;
        case 0xA0 : LDY(); break;
        case 0xB0 : BCS(); break;
        case 0xC0 : CPY(); break;
        case 0xD0 : BNE(); break;
        case 0xE0 : CPX(); break;
        case 0xF0 : BEQ(); break;

        case 0x01 : ORA_Xind(); break;
        case 0x11 : ORA_Yind(); break;
        case 0x21 : AND_Xind(); break;
        case 0x31 : AND_Yind(); break;
        case 0x41 : EOR_Xind(); break;
        case 0x51 : EOR_Yind(); break;
        case 0x61 : ADC_Xind(); break;
        case 0x71 : ADC_Yind(); break;
        case 0x81 : STA_Xind(); break;
        case 0x91 : STA_Yind(); break;
        case 0xA1 : LDA_Xind(); break;
        case 0xB1 : LDA_Yind(); break;
        case 0xC1 : CMP_Xind(); break;
        case 0xD1 : CMP_Yind(); break;
        case 0xE1 : SBC_Xind(); break;
        case 0xF1 : SBC_Yind(); break;

        case 0xA2 : LDX(); break;

        case 0x24 : BIT_ZPG(); break;
        case 0x84 : STY_ZPG(); break;
        case 0x94 : STY_Xzpg(); break;
        case 0xA4 : LDY_zpg(); break;
        case 0xB4 : LDY_Xzpg(); break;
        case 0xC4 : CPY_ZPG(); break;
        case 0xE4 : CPX_ZPG(); break;

        case 0x05 : ORA_ZPG(); break;
        case 0x15 : ORA_Xzpg(); break;
        case 0x25 : AND_ZPG(); break;
        case 0x35 : AND_Xzpg(); break;
        case 0x45 : EOR_ZPG(); break;
        case 0x55 : EOR_Xzpg(); break;
        case 0x65 : ADC_ZPG(); break;
        case 0x75 : ADC_Xzpg(); break;
        case 0x85 : STA_ZPG(); break;
        case 0x95 : STA_Xzpg(); break;
        case 0xA5 : LDA_ZPG(); break;
        case 0xB5 : LDA_Xzpg(); break;
        case 0xC5 : CMP_ZPG(); break;
        case 0xD5 : CMP_Xzpg(); break;
        case 0xE5 : SBC_ZPG(); break;
        case 0xF5 : SBC_Xzpg(); break;

        case 0x06 : ASL_ZPG(); break;
        case 0x16 : ASL_Xzpg(); break;
        case 0x26 : ROL_ZPG(); break;
        case 0x36 : ROL_Xzpg(); break;
        case 0x46 : LSR_ZPG(); break;
        case 0x56 : LSR_Xzpg(); break;
        case 0x66 : ROR_ZPG(); break;
        case 0x76 : ROR_Xzpg(); break;
        case 0x86 : STX_ZPG(); break;
        case 0x96 : STX_Yzpg(); break;
        case 0xA6 : LDX_ZPG(); break;
        case 0xB6 : LDX_Yzpg(); break;
        case 0xC6 : DEC_zpg(); break;
        case 0xD6 : DEC_Xzpg(); break;
        case 0xE6 : INC_ZPG(); break;
        case 0xF6 : INC_Xzpg(); break;

        case 0x08 : PHP(); break;
        case 0x18 : CLC(); break;
        case 0x28 : PLP(); break;
        case 0x38 : SEC(); break;
        case 0x48 : PHA(); break;
        case 0x58 : CLI(); break;
        case 0x68 : PLA(); break;
        case 0x78 : SEI(); break;
        case 0x88 : DEY(); break;
        case 0x98 : TYA(); break;
        case 0xA8 : TAY(); break;
        case 0xB8 : CLV(); break;
        case 0xC8 : INY(); break;
        case 0xD8 : CLD(); break;
        case 0xE8 : INX(); break;
        case 0xF8 : SED(); break;

        case 0x09 : ORA(); break; 
        case 0x19 : ORA_Yabs(); break;
        case 0x29 : AND(); break;
        case 0x39 : AND_Yabs(); break;
        case 0x49 : EOR(); break;
        case 0x59 : EOR_Yabs(); break;
        case 0x69 : ADC(); break;
        case 0x79 : ADC_Yabs(); break;
        case 0x99 : STA_Yabs(); break;
        case 0xA9 : LDA(); break;
        case 0xB9 : LDA_Yabs(); break;
        case 0xC9 : CMP(); break;
        case 0xD9 : CMP_Yabs(); break;
        case 0xE9 : SBC(); break;
        case 0xF9 : SBC_Yabs(); break;

        case 0x0A : ASL(); break;
        case 0x2A : ROL(); break;
        case 0x4A : LSR(); break;
        case 0x6A : ROR(); break;
        case 0x8A : TXA(); break;
        case 0x9A : TXS(); break;
        case 0xAA : TAX(); break;
        case 0xBA : TSX(); break;
        case 0xCA : DEX(); break;
        case 0xEA : NOP(); break;

        case 0x2C : BIT_ABS(); break;
        case 0x4C : JMP_ABS(); break;
        case 0x6C : JMP_IND(); break;
        case 0x8C : STY_ABS(); break;
        case 0xAC : LDY_ABS(); break;
        case 0xBC : LDY_Xabs(); break;
        case 0xCC : CPY_ABS(); break;
        case 0xEC : CPX_ABS(); break;

        case 0x0D : ORA_ABS(); break;
        case 0x1D : ORA_Xabs(); break;
        case 0x2D : AND_ABS(); break;
        case 0x3D : AND_Xabs(); break;
        case 0x4D : EOR_ABS(); break;
        case 0x5D : EOR_Xabs(); break;
        case 0x6D : ADC_ABS(); break;
        case 0x7D : ADC_Xabs(); break;
        case 0x8D : STA_ABS(); break;
        case 0x9D : STA_Xabs(); break;
        case 0xAD : LDA_ABS(); break;
        case 0xBD : LDA_Xabs(); break;
        case 0xCD : CMP_ABS(); break; 
        case 0xDD : CMP_Xabs(); break;
        case 0xED : SBC_ABS(); break;
        case 0xFD : SBC_Xabs(); break;

        case 0x0E : ASL_ABS(); break;
        case 0x1E : ASL_Xabs(); break;
        case 0x2E : ROL_ABS(); break;
        case 0x3E : ROL_Xabs(); break;
        case 0x4E : LSR_ABS(); break;
        case 0x5E : LSR_Xabs(); break;
        case 0x6E : ROR_ABS(); break;
        case 0x7E : ROR_Xabs(); break;
        case 0x8E : STX_ABS(); break;
        case 0xAE : LDX_ABS(); break;
        case 0xBE : LDX_Yabs(); break;
        case 0xCE : DEC_ABS(); break;
        case 0xDE : DEC_Xabs(); break;
        case 0xEE : INC_ABS(); break;
        case 0xFE : INC_Xabs(); break;
        default: printf("Unknown opcode: \n", opCode); break;
    }
}
void BRK(){
    if (!processorStatus.bits.I){
        writeMem(0x0100 + sp, (pc >> 8) & 0x00FF);
        sp--;
        writeMem(0x0100 + sp, pc & 0x00FF);
        processorStatus.bits.B = 0;
        processorStatus.bits.WALOO = 1;
        processorStatus.bits.I = 1;
        writeMem(0x0100 + sp, processorStatus.status);
        sp--;

        uint16_t lo = readMem(0xFFFE);
        uint16_t hi = readMem(0xFFFE + 1);

        pc = (hi << 8) | lo;

        cycles = 7;
        
    }
}

void NMI(){
    writeMem(0x0100 + sp, (pc >> 8) & 0x00FF);
    sp--;
    writeMem(0x0100 + sp, pc & 0x00FF);
    processorStatus.bits.B = 0;
    processorStatus.bits.WALOO = 1;
    processorStatus.bits.I = 1;
    writeMem(0x0100 + sp, processorStatus.status);
    sp--;

    uint16_t lo = readMem(0xFFFA);
    uint16_t hi = readMem(0xFFFA + 1);

    pc = (hi << 8) | lo;

    cycles = 8;
}

void RTI(){
    sp++;
    processorStatus.status = pullStack();
    processorStatus.status &= ~B;
    processorStatus.status &= ~WALOO;

    sp++;
    pc = (uint16_t)pullStack();
    sp++;
    pc |= (uint16_t)pullStack();
    
    cycles = 6;

}

void BPL(){
    if (!processorStatus.bits.N ){
        pc ++;
        int8_t bb = readMem(pc);
        pc += bb;

    }else{
        pc++;
    }
    
    cycles = 7;
}

void JSR(){
    pc++;
    uint8_t lo = readMem(pc);
    pc++;
    uint8_t hi = readMem(pc);
    uint16_t addr = (hi<<8) + lo;
    uint16_t returnAddress = pc + 1;
    pushStack((returnAddress >> 8) & 0xFF); // Push high byte
    pushStack(returnAddress & 0xFF);  // push low byte
    pc = addr;
    cycles = 6;
}
void BMI(){
    if (processorStatus.bits.N ){
        pc ++;
        int8_t bb = readMem(pc);

        uint16_t old_pc = pc+1;
        pc += bb;

        if ((old_pc & 0xFF00) != (pc&0xFF00)){
            cycles = 4;
        }else{
            cycles = 3;
        }
    }else{
        pc++;
    }
}

void BVC(){
    if (!processorStatus.bits.V){
        pc ++;
        int8_t bb = readMem(pc);

        uint16_t old_pc = pc+1;
        pc += bb;

        if ((old_pc & 0xFF00) != (pc&0xFF00)){
            cycles = 4;
        }else{
            cycles = 3;
        }
    }else{
        pc++;
    }
}

void RTS(){

    uint8_t lo = pullStack();

    uint8_t hi = pullStack();
    uint16_t addr = (hi<<8) + lo;
    pc = 1+ addr;
    cycles = 6;
}

void BVS(){
    if (processorStatus.bits.V){
        pc ++;
        int8_t bb = readMem(pc);

        uint16_t old_pc = pc+1;
        pc += bb;

        if ((old_pc & 0xFF00) != (pc&0xFF00)){
            cycles = 4;
        }else{
            cycles = 3;
        }
    }else{
        pc++;
    }
}

void clock(){
    if (cycles == 0){
        decodeOpcode(readMem(pc));
        pc++;

    }
    cycles--;
}