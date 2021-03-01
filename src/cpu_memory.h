#ifndef CPU_MEMORY_H
#define CPU_MEMORY_H

#define MEMORY_SIZE 0x10000 // 64 KiB
#define LOWER_PRG_ROM_START 0x8000 // Start of lower PRG-ROM bank
#define UPPER_PRG_ROM_START 0xC000 // Start of upper PRG-ROM bank
#define IO_REGISTERS_START 0x2000 // Address of the first IO-register
#define STACK_SIZE 0x00FF; // Size of the stack

#include <assert.h>
#include <stdint.h>

class CPUMemory {
private:
    uint8_t memory[MEMORY_SIZE];
    const uint16_t TOP_OF_STACK = 0x01FF; // Address of the top of the stack

public:
    CPUMemory();
    uint16_t get_top_of_stack() { return TOP_OF_STACK; }
    uint8_t& operator[](uint16_t index);

    void write_byte(uint16_t address, uint8_t byte);
    uint8_t read_byte(uint16_t address);
};

#endif