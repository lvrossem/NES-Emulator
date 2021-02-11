#ifndef CPU_H
#define CPU_H

#define MEMORY_SIZE 0x10000 // 64 KiB
#define LOWER_PRG_ROM_START 0x8000 // Start of lower PRG-ROM bank
#define UPPER_PRG_ROM_START 0xC000 // Start of upper PRG-ROM bank
#define IO_REGISTERS_START 0x2000 // Address of the first IO-register

#include <stdint.h>
#include <assert.h>

enum StatusBit { Negative = 0, Overflow, NotUsed, Break, DecimalMode, InterruptDisable, Zero, Carry };

class CPU {
private:
    uint8_t cpu_memory[MEMORY_SIZE]; // The available memory

    uint16_t PC; // The program counter
    uint16_t SP; // The stack pointer
    uint8_t A;   // The accumulator
    uint8_t X;   // The index register X
    uint8_t Y;   // The index register Y
    /* Processor status; 8 status bits
        Bit 0: Carry
        Bit 1: Zero
        Bit 2: Interrupt disable
        Bit 3: Decimal mode (will most likely be ignored)
        Bit 4: Break
        Bit 5: Not used
        Bit 6: Overflow
        Bit 7: Negative */
    uint8_t P;

public:
    CPU();

    void set_status_bit(StatusBit bit, bool flag); // Sets a statusbit with the value of flag

    void handle_status_ADC(uint8_t arg); // Handle status register changes caused by ADC

    void initialize(); // Set all registers and entire memory to 0
    void write_to_memory(char* data, uint16_t start, uint16_t size); // Write array of bytes to memory
    void execute();

    uint8_t next_prg_byte(); // Read the next byte from the program code
};

#endif