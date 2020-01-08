#ifndef CPU_H
#define CPU_H

#include <string>

#define PROGRAM_START 0x200
#define MEMORY_SIZE 4096
#define COLS 64 
#define ROWS 32
#define DISPLAY_SIZE (ROWS*COLS)

class CPU {
    
public:
    CPU();
    void Execute();
    void ReadRomFile(std::string &file);
    
    const uint8_t *GetDisplayBuffer();

private:
    uint8_t memory[MEMORY_SIZE];
    uint16_t pc;
    uint16_t instr;
    uint16_t ireg;
    uint8_t Vreg[16]; // V registers
                      // V_0 through V_F

    uint8_t display[DISPLAY_SIZE];
};
#endif
