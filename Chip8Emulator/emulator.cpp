
#include "chip8.h"
// Include OpenGL graphics and input too

chip8 myChip8; 

// Main loop 
int main(int argc, char **argv) 
{
  // Set up graphics
  setupGraphics();
  setupInput();
 
  // Initialize the Chip8 system and load the game into the memory  
  myChip8.initialize();
  myChip8.loadGame("pong");
 
  // Emulation loop
  for(;;)
  {
    // Emulate one cycle
    myChip8.emulateCycle();
 
    // If the draw flag is set, update the screen
    if(myChip8.drawFlag)
      drawGraphics();
 
    // Store key state
    myChip8.setKeys();	
  }
 
  return 0;
}