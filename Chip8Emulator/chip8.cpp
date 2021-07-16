#include "chip8.h"
#include <iostream>
#include <fstream>

// CORE FUNCTIONS ///////////////////////////////////////////////////////////////////

void chip8::initialize(){
    pc     = 0x200;  // Program counter starts at 0x200
    opcode = 0;      // Reset current opcode	
    I      = 0;      // Reset index register
    sp     = 0;      // Reset stack pointer

    // Clear screen
    int numRows = sizeof(screen)/sizeof(screen[0]);
    int numCols = sizeof(screen[0])/sizeof(char);
    for (int i = 0; i < numRows; ++i){
        for (int j = 0; j < numCols; ++j){
            screen[i][j] = 0; 
        } 
    }
    drawFlag = true;

    // Clear stack and registers V0-VF
    for (int i = 0; i < 16; ++i){
        stack[i] = 0;
        gpRegisters[i] = 0; 
    }

    // Clear memory
    for (int i = 0; i < 4096; ++i){
        memory[i] = 0; 
    }
    
    // Load fontset
    for(int i = 0; i < 80; ++i){
        memory[i] = chip8_fontset[i];
    }	

    // Reset timers
	delayTimer = 0;
	soundTimer = 0;

}

void chip8::loadGame(){
   FILE *in;
   in = fopen( "c:/INVADERS", "rb" );
   fread( &memory[0x200], 0xfff, 1, in);
   fclose(in);
}

void chip8::emulateCycle(){
    // Fetch Opcode (one opcode is two bytes, so shift one and combine with the next)
    opcode = memory[pc] << 8 | memory[pc + 1];
    pc += 2; 

    // Decode Opcode

    // Update timers
}

// OPCODES ///////////////////////////////////////////////////////////////////

void chip8::DecodeOpcode(unsigned char opcode){

    unsigned short firstNumber = opcode & 0xF000; 
    unsigned short secondNumber = opcode & 0x0F00; 
    unsigned short thirdNumber = opcode & 0x00F0; 
    unsigned short fourthNumber = opcode & 0x000F;

    switch(firstNumber){
        // Starts with 0
        case 0x0000: 
            switch(fourthNumber){
                case 0x0000: Opcode00E0(opcode); break; // clear screen 
                case 0x000E: Opcode00EE(opcode); break; // return subroutine
            } 

        // Starts with 1-7 
        case 0x1000: Opcode1NNN(opcode); break;
        case 0x2000: Opcode2NNN(opcode); break;
        case 0x3000: Opcode3XNN(opcode); break;
        case 0x4000: Opcode4XNN(opcode); break;
        case 0x5000: Opcode5XY0(opcode); break;
        case 0x6000: Opcode6XNN(opcode); break;
        case 0x7000: Opcode7XNN(opcode); break;

        // Starts with 8 
        case 0x8000:
            switch(fourthNumber){
                case 0x0000: Opcode8XY0(opcode); break;
                case 0x0001: Opcode8XY1(opcode); break;
                case 0x0002: Opcode8XY2(opcode); break;
                case 0x0003: Opcode8XY3(opcode); break;
                case 0x0004: Opcode8XY4(opcode); break;
                case 0x0005: Opcode8XY5(opcode); break;
                case 0x0006: Opcode8XY6(opcode); break;
                case 0x0007: Opcode8XY7(opcode); break;
                case 0x000E: Opcode8XYE(opcode); break;
            }

        // Starts with 9-D
        case 0x9000: Opcode9XY0(opcode); break;
        case 0xA000: OpcodeANNN(opcode); break; 
        case 0xB000: OpcodeBNNN(opcode); break;
        case 0xC000: OpcodeCXNN(opcode); break;
        case 0xD000: OpcodeDXYN(opcode); break;

        // Starts with E
        case 0xE000:
            switch(fourthNumber){
                case 0x000E: OpcodeEX9E(opcode); break; 
                case 0x0001: OpcodeEXA1(opcode); break; 
            }
        
        // Starts with F
        case 0xF000: 
            switch(fourthNumber){
                case 0x0005: 
                    switch(thirdNumber){
                        case 0x0010: OpcodeFX15(opcode); break; 
                        case 0x0050: OpcodeFX55(opcode); break;
                        case 0x0060: OpcodeFX65(opcode); break;  
                    }
                case 0x0003: OpcodeFX33(opcode); break;
                case 0x0007: OpcodeFX07(opcode); break; 
                case 0x0008: OpcodeFX18(opcode); break;
                case 0x0009: OpcodeFX29(opcode); break; 
                case 0x000A: OpcodeFX0A(opcode); break;
                case 0x000E: OpcodeFX1E(opcode); break;  
            }

        default: break ;
    }

}


void chip8::Opcode2NNN(unsigned char opcode){
    stack.push_back(pc) ; // save pc
    pc = opcode & 0x0FFF ; // jump to address NNN
} 


/*

Unique start opcodes: 
First num 1-7, 9, A-D

*/