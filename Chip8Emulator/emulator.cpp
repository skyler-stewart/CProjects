#include "chip8.hpp"


// Main loop
int main(int argc, char **argv)
{
	chip8 myChip8;

	// Initialize the Chip8 system and load the game into the memory
	myChip8.initGraphics(); 
	std::cout << "initGraphics was successful!" << std::endl; 
	myChip8.initEmulator();
	std::cout << "initEmulator was successful!" << std::endl; 
	myChip8.loadGame("./Games/pong.c8");
	std::cout << "loadGame was successful!" << std::endl; 

	// TO DO 
	// renderFrame
	// handleInput 
	// modify emulateCycle cycle rate 

	// Emulation loop
  	// for(;;)
	// {
	// 	// Emulate one cycle
	// 	myChip8.emulateCycle();

	// 	// // If the draw flag is set, update the screen
	// 	// if(myChip8.drawFlag)
	// 	// 	drawGraphics();

	// 	// // Store key state
	// 	// myChip8.setKeys();
	// }

	// For Testing purposes 
	// SDL_Event e;
	// bool quit = false;
	// while (!quit){
	// 	while (SDL_PollEvent(&e)){
	// 		if (e.type == SDL_QUIT){
	// 			quit = true;
	// 		}
	// 		if (e.type == SDL_KEYDOWN){
	// 			quit = true;
	// 		}
	// 	}
	// }

	//Destroy window
    // SDL_DestroyWindow(window);
    // SDL_Quit();

	return 0;
}