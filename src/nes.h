#ifndef NES_H
#define NES_H

#define MEMORY_SIZE 65536 // 64 KiB

#include <stdint.h>

class NES {
    private:
        uint8_t memory[MEMORY_SIZE]; // The available memory

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
        void initialize(); // Set all registers and entire memory to 0

};

#endif