#include "chip8.h"
#include <iostream>
#include <fstream>
#include <limits.h> 

// CORE FUNCTIONS ///////////////////////////////////////////////////////////////////

void chip8::initialize(){
    pc       = 0x200;  // Program counter starts at 0x200
    sp       = 0;      // Reset stack pointer
    opcode   = 0;      // Reset current opcode	
    addressI = 0;      // Reset index register

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

    // Decode and Execute Opcode
    DecodeOpcode(opcode); 

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


// Jumps to NNN
void chip8::Opcode1NNN(unsigned char opcode){
   pc = opcode & 0x0FFF ; // Get NNN of opcode 1NNN
}

// Calls subroutine at NNN 
void chip8::Opcode2NNN(unsigned char opcode){
    stack.push_back(pc) ; // save pc
    pc = opcode & 0x0FFF ; // jump to address NNN
} 

// Skips the next instruction if VX equals NN
void chip8::Opcode3XNN(unsigned char opcode){
    int regX = opcode & 0x0F00; // mask off reg x
    regX = regX >> 8;
    int addressNN = opcode & 0x00FF; // get address NN
    if (gpRegisters[regX] == memory[addressNN])
        pc += 2; // skip next instruction 
}

// Skips the next instruction if VX does not equal NN.
void chip8::Opcode4XNN(unsigned char opcode){
    int regX = opcode & 0x0F00; // mask off reg x
    regX = regX >> 8;
    int addressNN = opcode & 0x00FF; // get address NN
    if (gpRegisters[regX] != memory[addressNN])
        pc += 2; // skip next instruction 
}

// Skips the next instruction if VX equals VY
void chip8::Opcode5XY0(unsigned char opcode){
     int regX = opcode & 0x0F00; // mask off reg x
     regX = regX >> 8; // shift x across
     int regY = opcode & 0x00F0; // mask off reg y
     regY = regY >> 4; // shift y across
     if (gpRegisters[regX] == gpRegisters[regY])
         pc += 2; // skip next instruction
} 

// Sets VX to NN
void chip8::Opcode6XNN(unsigned char opcode){
    int regX = opcode & 0x0F00; 
    regX = regX >> 8; 
    int addressNN = opcode & 0x00FF; 
    gpRegisters[regX] = memory[addressNN]; 
}

// Adds NN to VX (carry flag is not changed)
void chip8::Opcode7XNN(unsigned char opcode){
    int regX = opcode & 0x0F00; 
    regX = regX >> 8; 
    int addressNN = opcode & 0x00FF; 
    gpRegisters[regX] = gpRegisters[regX] + memory[addressNN]; 
}

// Sets VX to the value of VY
void chip8::Opcode8XY0(unsigned char opcode){
    int regX = opcode & 0x0F00;
    regX = regX >> 8; 
    int regY = opcode & 0x00F0;
    regY = regY >> 4; 
    gpRegisters[regX] = gpRegisters[regY];
}

// Sets VX to VX or VY
void chip8::Opcode8XY1(unsigned char opcode){
    int regX = opcode & 0x0F00; 
    regX = regX >> 8;
    int regY = opcode & 0x00F0;
    regY = regY >> 4;
    gpRegisters[regX] = gpRegisters[regX] | gpRegisters[regY];
}

// Sets VX to VX & VY
void chip8::Opcode8XY2(unsigned char opcode){
    int regX = opcode & 0x0F00; 
    regX = regX >> 8;
    int regY = opcode & 0x00F0;
    regY = regY >> 4;
    gpRegisters[regX] = gpRegisters[regX] & gpRegisters[regY];
}

// Sets VX to VX ^ VY
void chip8::Opcode8XY3(unsigned char opcode){
    int regX = opcode & 0x0F00; 
    regX = regX >> 8;
    int regY = opcode & 0x00F0;
    regY = regY >> 4;
    gpRegisters[regX] = gpRegisters[regX] ^ gpRegisters[regY];
}

// Sets VX to VX += VY
void chip8::Opcode8XY4(unsigned char opcode){
    gpRegisters[0xF] = 0; 
    int regX = opcode & 0x0F00; 
    regX = regX >> 8;
    int regY = opcode & 0x00F0;
    regY = regY >> 4;

    int xVal = gpRegisters[regX];
    int yVal = gpRegisters[regY];
    if (xVal + yVal > USHRT_MAX){
        gpRegisters[0xF] = 1;
    }
    gpRegisters[regX] = xVal + yVal;
}

// VX is set to VX - VY 
// VF is set to 0 when there's a borrow, and 1 when there is not 
void chip8::Opcode8XY5(unsigned char opcode){
     gpRegisters[0xF] = 1; // Set carry 
     int regX = opcode & 0x0F00; // mask off reg x
     regX = regX >> 8; // shift x across
     int regY = opcode & 0x00F0; // mask off reg y
     regY = regY >> 4; // shift y across

     int xVal = gpRegisters[regX];
     int yVal = gpRegisters[regY];
     if (yVal > xVal) // if this is true will result in a value < 0
          gpRegisters[0xF] = 0;
     gpRegisters[regX] = xVal-yVal;
} 

// Stores the least significant bit of VX in VF and then shifts VX to the right by 1
void chip8::Opcode8XY6(unsigned char opcode){
    int regX = opcode & 0x0F00; // mask off reg x
    regX = regX >> 8; // shift x across
    unsigned short lsb = gpRegisters[regX] >> 15; // TODO: double check this
    gpRegisters[0xF] = lsb;
    gpRegisters[regX] = gpRegisters[regX] >> 1;
} 

// Sets VX to VY - VX
// VF is set to 0 when there's a borrow, and 1 when there is not. 
void chip8::Opcode8XY7(unsigned char opcode){
     gpRegisters[0xF] = 1; // Set carry 
     int regX = opcode & 0x0F00; // mask off reg x
     regX = regX >> 8; // shift x across
     int regY = opcode & 0x00F0; // mask off reg y
     regY = regY >> 4; // shift y across

     int xVal = gpRegisters[regX];
     int yVal = gpRegisters[regY];
     if (xVal > yVal) // if this is true will result in a value < 0
          gpRegisters[0xF] = 0;
     gpRegisters[regX] = yVal-xVal;
} 

// Stores the least significant bit of VX in VF and then shifts VX to the right by 1
void chip8::Opcode8XYE(unsigned char opcode){
    int regX = opcode & 0x0F00; // mask off reg x
    regX = regX >> 8; // shift x across
    unsigned short msb = gpRegisters[regX] << 15; // TODO: check if right kind of shift
    gpRegisters[0xF] = msb;
    gpRegisters[regX] = gpRegisters[regX] << 1;
}

// Skips the next instruction if VX does not equal VY
void chip8::Opcode9XY0(unsigned char opcode){
    int regX = opcode & 0x0F00;
    regX = regX >> 8;
    int regY = opcode & 0x00F0;
    regY = regY >> 4;
    if (gpRegisters[regX] != gpRegisters[regY])
        pc += 2; 
}

// Sets I to the address NNN
void chip8::OpcodeANNN(unsigned char opcode){
    int addressNNN = opcode & 0x0FFF; 
    addressI = addressNNN; 
}

// Jumps to the address NNN plus V0
void chip8::OpcodeBNNN(unsigned char opcode){
    int addressNNN = opcode & 0x0FFF;
    pc = addressNNN + gpRegisters[0]; 
}

// Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN
void chip8::OpcodeCXNN(unsigned char opcode){
    int regX = opcode & 0x0F00;
    regX = regX >> 8;
    // TODO 
}

// Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N+1 pixels
// Each row of 8 pixels is read as bit-coded starting from memory location I
// I value does not change after the execution of this instruction
// VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that does not happen 
void chip8::OpcodeDXYN(unsigned char opcode){
    // TODO (in tutorial?)
}

// Skips the next instruction if the key stored in VX is pressed
void chip8::OpcodeEX9E(unsigned char opcode){
    int regX = opcode & 0x0F00;
    regX = regX >> 8;
    xVal = gpRegisters[regX];
    if (keyState[xVal] == True){
        pc += 2; 
    }
}

// Stores V0 to VX (including VX) in memory starting at address I.
void chip8::OpcodeFX55(unsigned char opcode){
     int regX = opcode & 0x0F00;
     regX >>= 8;
     for (int i = 0 ; i <= regX; i++){
          memory[addressI + i] = gpRegisters[i];
     }
     addressI = addressI + regX + 1;
} 