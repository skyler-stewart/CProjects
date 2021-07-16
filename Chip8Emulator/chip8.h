#include <vector> 

class chip8{
    public: 
        // 35 opcodes, 2 bytes each 
        unsigned short opcode; 

        // Memory (0x200 to 0xFFF for working memory)
        unsigned char memory[4096];
        unsigned char gpRegisters[16]; 

        unsigned short I; 
        unsigned short pc; 
        unsigned short sp;
        std::vector<unsigned short> stack;

        unsigned char keyState[16];
        unsigned char screen[64][32];
        unsigned char delayTimer; 
        unsigned char soundTimer;
        bool drawFlag;  

        // Fontset
        unsigned char chip8_fontset[80] =
        { 
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
        };

    // Core Functions 
    void initialize();
    void emulateCycle(); 
    void loadGame(); 
    void DecodeOpcode(unsigned char opcode); 

    // Execute Opcodes 
    void Opcode0NNN(unsigned char opcode);
    void Opcode00E0(unsigned char opcode);
    void Opcode00EE(unsigned char opcode);

    void Opcode1NNN(unsigned char opcode);
    void Opcode2NNN(unsigned char opcode);
    void Opcode3XNN(unsigned char opcode);
    void Opcode4XNN(unsigned char opcode);
    void Opcode5XY0(unsigned char opcode);
    void Opcode6XNN(unsigned char opcode);
    void Opcode7XNN(unsigned char opcode);

    void Opcode8XY0(unsigned char opcode);
    void Opcode8XY1(unsigned char opcode);
    void Opcode8XY2(unsigned char opcode);
    void Opcode8XY3(unsigned char opcode);
    void Opcode8XY4(unsigned char opcode);
    void Opcode8XY5(unsigned char opcode);
    void Opcode8XY6(unsigned char opcode);
    void Opcode8XY7(unsigned char opcode);
    void Opcode8XYE(unsigned char opcode);

    void Opcode9XY0(unsigned char opcode);
    void OpcodeANNN(unsigned char opcode);
    void OpcodeBNNN(unsigned char opcode);
    void OpcodeCXNN(unsigned char opcode);
    void OpcodeDXYN(unsigned char opcode);

    void OpcodeEX9E(unsigned char opcode);
    void OpcodeEXA1(unsigned char opcode);

    void OpcodeFX07(unsigned char opcode);
    void OpcodeFX0A(unsigned char opcode);
    void OpcodeFX15(unsigned char opcode);
    void OpcodeFX18(unsigned char opcode);
    void OpcodeFX1E(unsigned char opcode);
    void OpcodeFX29(unsigned char opcode);
    void OpcodeFX33(unsigned char opcode);
    void OpcodeFX55(unsigned char opcode);
    void OpcodeFX65(unsigned char opcode);
};
