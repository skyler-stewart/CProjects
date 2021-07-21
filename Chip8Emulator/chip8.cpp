#include "chip8.h"
#include <iostream>
#include <fstream>
#include <limits.h> 
#include <stdlib.h>
#include <time.h>
#include <filesystem>
#include <SDL2/SDL.h>

// CORE FUNCTIONS ///////////////////////////////////////////////////////////////////

void chip8::initGraphics(){
	SDL_Window* window = NULL;
	SDL_Surface* screenSurface = NULL;

	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		std::cout << "SDL could not initialize. SDL_Error: %s\n" + std::string(SDL_GetError()) << std::endl;
	}
	else{
		window = SDL_CreateWindow("Chip8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, myChip8.screenWidth, myChip8.screenHeight, SDL_WINDOW_SHOWN);
		if(window == NULL){
			std::cout << "SDL window could not be created. SDL_Error: %s\n" + std::string(SDL_GetError()) << std::endl;
		}
		else{
			//Get window surface
			screenSurface = SDL_GetWindowSurface( window );
			//Fill the surface white
			SDL_FillRect( screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0xFF, 0xFF, 0xFF ) );
			//Update the surface
			SDL_UpdateWindowSurface( window );
			//Wait ten seconds
			SDL_Delay( 10000 );
		}
	}

    SDL_DestroyWindow(window);
    SDL_Quit();
}

void chip8::initEmulator(){
    pc       = 0x200;  // pc starts at 0x200
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

void chip8::loadGame(const std::string &fileName){
    // Open file 
    FILE *gameFile;
    gameFile = fopen(fileName, "rb");

    // Error checking 
    if (gameFile == NULL){
        fputs("Game file not found!", stderr); 
        return false;
    }

    int gameFileSize = std::filesystem::file_size(gameFile); 
    int memSize = 4096 - 512; 
    if (gameFileSize > memSize){
        fputs("Game file is too large!", stderr); 
        return false; 
    }

    // Copy game in to memory 
    fread(&memory[0x200], 0xfff, 1, in);
    fclose(in);

    // Close file 
    fclose(gameFile); 
}

void chip8::emulateCycle(){
    // Fetch Opcode (one opcode is two bytes, so shift one and combine with the next)
    opcode = memory[pc] << 8 | memory[pc + 1];
    pc += 2; 

    // Decode and Execute Opcode
    DecodeOpcode(opcode); 

    // Update timers
    if(delay_timer > 0){
        delay_timer--;
    }
	if(sound_timer > 0){
		if(sound_timer == 1)
			printf("BEEP\n");
		sound_timer--;
	}	
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

// Calls machine code routine at address NNN
void chip8::Opcode0NNN(unsigned char opcode){
    int addrNNN = opcode & 0x0FFF;
    pc = addrNNN;  
}

// Clears screen 
void chip8::Opcode00E0(unsigned char opcode){
   for (int i = 0; i < screenRows; i++){
       for (int j = 0; j < screenCols; j++){
           screen[i][j] = 0; 
       }
   }
}

// Returns from a subroutine 
void chip8::Opcode00EE(unsigned char opcode){
    int returnDest = stack.back(); 
    stack.pop_back(); 
    pc = returnDest; 
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
    unsigned short lsb = gpRegisters[regX] >> 15;
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
    unsigned short msb = gpRegisters[regX] << 15;
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
    int addrNN = opcode & 0x00FF; 

    std::srand((unsigned)time(NULL));
    unsigned short randNum = rand() % USHRT_MAX; 
    gpRegisters[regX] = randNum & addrNN; 
}

// Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N+1 pixels
// Each row of 8 pixels is read as bit-coded starting from memory location I
// I value does not change after the execution of this instruction
// VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 otherwise 
void chip8::OpcodeDXYN(unsigned char opcode){

    // Get registers
    int regX = opcode & 0x0F00;
    regX = regX >> 8;
    int regY = opcode & 0x00F0;
    regY = regY >> 4;

    // Get sprite coordinate and set V0 
    int height = opcode & 0x000F;
    int coordX = gpRegisters[regX];
    int coordY = gpRegisters[regY];
    gpRegisters[0xf] = 0;

    // Draw sprite
    for (int yLines = 0; yLines < height; yLines++){
        unsigned char data = memory[addressI + yLines];
        int xPixel = 0;
        int xPixelInverse = 7;
        for (xPixel = 0; xPixel <= 7; xPixel++, xPixelInverse--){
            int mask = 1 << xPixelInverse;
            if (data & mask){
                int x = coordX + xPixel;
                int y = coordY + yLines;
                if (screen[x][y] == 1) 
                    gpRegisters[0xF] = 1; // Collision occured 
                screen[x][y] ^= 1;
            }
        }
    }
}

// Skips the next instruction if the key stored in VX is pressed
void chip8::OpcodeEX9E(unsigned char opcode){
    int regX = opcode & 0x0F00;
    regX = regX >> 8;
    int xVal = gpRegisters[regX];
    if (keyState[xVal] == true){
        pc += 2; 
    }
}

// Skips the next instruction if the key stored in VX is not pressed
void chip8::OpcodeEXA1(unsigned char opcode){
    int regX = opcode & 0x0F00;
    regX = regX >> 8;
    int xVal = gpRegisters[regX];
    if (keyState[xVal] == false){
        pc += 2; 
    }
}

// Sets VX to the value of the delay timer
void chip8::OpcodeFX07(unsigned char opcode){
    int regX = opcode & 0x0F00;
    regX = regX >> 8;
    gpRegisters[regX] = delayTimer; 
}

// A key press is awaited, and then stored in VX. 
// Blocking Operation. All instruction halted until next key event.  
void chip8::OpcodeFX0A(unsigned char opcode){
    int regX = opcode & 0x0F00;
    regX = regX >> 8;
    int k = 0; 
    while(k < 16){
        if (keyState[k]){
            gpRegisters[regX] = k; 
            break; 
        } 
        if (k >= 15){
            k = 0; 
        }
    }
}

// Sets the delay timer to VX
void chip8::OpcodeFX15(unsigned char opcode){
    int regX = opcode & 0x0F00;
    regX = regX >> 8;
    delayTimer = gpRegisters[regX]; 
}

// Sets the sound timer to VX
void chip8::OpcodeFX18(unsigned char opcode){
    int regX = opcode & 0x0F00;
    regX = regX >> 8;
    soundTimer = gpRegisters[regX]; 
}

// Adds VX to I. VF is not affected.
void chip8::OpcodeFX1E(unsigned char opcode){
    int regX = opcode & 0x0F00;
    regX = regX >> 8;
    addressI += gpRegisters[regX]; 
}

// Sets I to the location of the sprite for the character in VX. 
// Characters 0-F (in hexadecimal) are represented by a 4x5 font. 
void chip8::OpcodeFX29(unsigned char opcode){
    int regX = opcode & 0x0F00;
    regX = regX >> 8;
    addressI = gpRegisters[regX] * 5; 
}

// Takes the decimal representation of VX, place the hundreds digit in memory at location in I, 
// the tens digit at location I+1, and the ones digit at location I+2.
void chip8::OpcodeFX33(unsigned char opcode){
    int regX = opcode & 0x0F00;
    regX = regX >> 8;
    int xVal = gpRegisters[regX]; 
    memory[addressI + 2] = xVal % 10;
    xVal /= 10; 
    memory[addressI + 1] = xVal % 10;
    xVal /= 10; 
    memory[addressI] = xVal % 10;
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

// Fills V0 to VX (including VX) with values from memory starting at address I. 
// The offset from I is increased by 1 for each value written, but I itself is left unmodified.
void chip8::OpcodeFX65(unsigned char opcode){
    int regX = opcode & 0x0F00;
    regX >>= 8;

    int copyI = addressI;
    for(int i = 0; i <= regX; i++){
        gpRegisters[i] = memory[copyI]; 
        copyI++; 
    }
    
} 