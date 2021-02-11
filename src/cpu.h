#ifndef CPU_H
#define CPU_H

#define MEMORY_SIZE 0x10000 // 64 KiB
#define LOWER_PRG_ROM_START 0x8000 // Start of lower PRG-ROM bank
#define UPPER_PRG_ROM_START 0xC000 // Start of upper PRG-ROM bank
#define IO_REGISTERS_START 0x2000 // Address of the first IO-register

#include <stdint.h>
#include <assert.h>
#include <vector>

enum StatusBit { Negative = 0, Overflow, NotUsed, Break, DecimalMode, InterruptDisable, Zero, Carry };
enum Instruction {
    ADC, AND, CMP, EOR,
    LDA, ORA, SBC, STA,
    ASL, LDX, LDY, LSR,
    ROL, ROR, DEC, INC,
    STX, STY, CPX, CPY,
    BIT, JMP, BCC, BCS,
    BEQ, BMI, BNE, BPL,
    BRK, BVC, BVS, CLC,
    CLD, CLI, CLV, DEX,
    DEY, INX, INY, NOP,
    PHA, PHP, PLA, PLP,
    RTI, RTS, SEC, SED,
    SEI, TAX, TAY, TSX,
    TXA, TXS, TYA, JSR
};

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

    const std::vector<Instruction> group_1A {ADC, AND, CMP, EOR, LDA, ORA, SBC, STA};
    const std::vector<Instruction> group_1B {ASL, LDX, LDY, LSR, ROL, ROR};
    const std::vector<Instruction> group_2A {DEC, INC, STX, STY};
    const std::vector<Instruction> group_2B {CPX, CPY};
    const std::vector<Instruction> group_3A {BIT};
    const std::vector<Instruction> group_3B {JMP};

public:
    CPU();

    void set_status_bit(StatusBit bit, bool flag); // Sets a statusbit with the value of flag

    void handle_status_ADC(uint8_t arg); // Handle status register changes caused by ADC
    void handle_status_AND(uint8_t arg); // Handle status register changes caused by AND
    void handle_status_CMP(uint8_t arg); // Handle status register changes caused by CMP

    void initialize(); // Set all registers and entire memory to 0
    void write_to_memory(char* data, uint16_t start, uint16_t size); // Write array of bytes to memory
    void execute();

    uint8_t next_prg_byte(); // Read the next byte from the program code
};

#endif