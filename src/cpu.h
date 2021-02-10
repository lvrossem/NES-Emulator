#ifndef CPU_H
#define CPU_H

#define MEMORY_SIZE 0x10000 // 64 KiB
#define LOWER_PRG_ROM_START 0x8000 // Start of lower PRG-ROM bank
#define UPPER_PRG_ROM_START 0xC000 // Start of upper PRG-ROM bank
#define IO_REGISTERS_START 0x2000 // Address of the first IO-register

#include <stdint.h>
#include <assert.h>

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

    void flip_carry(); // Flip the carry flag in the status register
    void flip_zero(); // Flip the zero flag in the status register
    void flip_interrupt_disable(); // Flip the interrupt disable flag in the status register
    void flip_break(); // Flip the break flag in the status register
    void flip_overflow(); // Flip the overflow flag in the status register
    void flip_negative(); // Flip the negative flag in the status register

    void handle_status_ADC(uint8_t arg); // Handle status register changes caused by ADC

    void initialize(); // Set all registers and entire memory to 0
    void write_to_memory(char* data, uint16_t start, uint16_t size); // Write array of bytes to memory
    void execute();

    uint8_t next_prg_byte(); // Read the next byte from the program code
};

#endif