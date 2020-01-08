#include <fstream>
#include <iostream>
#include <vector>

#include "../includes/cpu.h"

CPU::CPU()
{
    pc = PROGRAM_START;
    ireg = 0;
    instr = 0;
    
    // Clear memory
    for (int i = 0; i < MEMORY_SIZE; ++i) {
        memory[i] = 0;
    }

    // Clear display buffer
    for (int i = 0; i < DISPLAY_SIZE; ++i) {
        display[i] = 0;
    }
    
    // Clear V registers
    for (int i = 0; i < 16; ++i) {
        Vreg[i] = 0;
    }
}

void CPU::ReadRomFile(std::string &file)
{
    std::clog << "Reading in file: " << file << std::endl;
    
    std::ifstream isfile(file.c_str(), 
                         std::ios::in | std::ios::binary | std::ios::ate);

    if (!isfile.is_open()) {
        std::cerr << "Unable to open file!" << std::endl;
        exit(-1);
    }
    
    std::streamsize size = isfile.tellg();
    isfile.seekg(0, std::ios::beg);
    
    if (size > (MEMORY_SIZE - PROGRAM_START)) {
        std::cerr << "ROM is too large!" << std::endl;
        exit(-1);
    }
    
    std::vector<char> buffer(size);
    if (isfile.read(buffer.data(), size)) {
        std::clog << "Loaded rom..." << std::endl;
        isfile.close();
    }
    
    for (int i = 0; i < size; ++i) {
        this->memory[i + PROGRAM_START] = (uint8_t) buffer[i];
    }
}

void CPU::Execute() 
{
    // 2 bytes per instruction
    instr = memory[pc] << 8 | memory[pc + 1];
    
    // Top 4 bits represent the opcode
    switch(instr & 0xF000) {
    case 0x0000:
        switch (instr & 0x00FF) {
        case 0x00E0:
            for (int i = 0; i < DISPLAY_SIZE; ++i) {
                display[i] = 0;
            }
            
            pc += 2;

            break;
        }

        break;
        
    case 0xA000:
        // ANNN, Sets I to NNN
        ireg = instr & 0x0FFF;
        
        pc += 2;
        
        break;
    case 0x6000:
    {
        // 6XNN, Sets VX to NN
        const unsigned char X = (instr & 0x0F00) >> 8;
        Vreg[X] = instr & 0x00FF;
        
        pc += 2;

        break;
    }
    case 0xD000:
    {
        // DXYN
        // Draws a sprite at coordinate (V_X, V_Y) with width 8 and height N.
        // Each pixel is read as starting from the value in the I register.
        // Sprites are xored onto the screen.
        // If an overwrite operation is performed on the display, set Vf to 1,
        // and 0 otherwise.
        const uint8_t X_COORD = Vreg[(instr & 0x0F00) >> 8];
        const uint8_t Y_COORD = Vreg[(instr & 0x00F0) >> 4];
        const uint8_t height = instr & 0x000F;

        Vreg[0xF] = 0;

        for (int y = 0; y < height; ++y) {
            const uint8_t pixel = memory[ireg + y];

            for(int x = 0; x < 8; ++x) {
                // The display is 32x64 pixels stored in a 1D array row-wise.
                // If the requested pixel to be drawn is some non-zero value,
                // perform a draw operation
                if((pixel & (0x80 >> x)) != 0) {

                    // If the requested pixel to be drawn and the current pixel
                    // on the display collide, set the Vf flag
                    if(display[(X_COORD + x + ((Y_COORD + y) * COLS))] == 1) {
                        Vreg[0xF] = 1;
                    }

                    display[X_COORD + x + ((Y_COORD + y) * COLS)] ^= 1;
                }
            }
        }        
        pc += 2;
        break;
    }
    case 0x7000:
        // 7XNN, Adds NN TO V_X
        Vreg[(instr & 0x0F00) >> 8] += instr & 0x00FF;
        
        pc += 2;
        break;
        
    case 0x3000:
        // 3XNN, Skips next instruction if V_X equals NN
        if (Vreg[(instr & 0x0F00) >> 8] == (instr & 0x00FF)) {
            pc += 4;
        } else {
            pc += 2;
        }
        
        break;
        
    case 0x1000:
        // 1NNN, Jump to NNN
        pc = instr & 0x0FFF;
        
        break;
    default:
        printf("Unimplemented/unrecognized instruction: %.4X\n", instr);
        break;
    }

}

const uint8_t *CPU::GetDisplayBuffer() 
{
    return this->display;
}
