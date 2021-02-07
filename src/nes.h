#ifndef NES_H
#define NES_H

#define MEMORY_SIZE 65536 // 64 KiB

#include <stdint.h>
#include <string.h>
#include <fstream>
#include <iostream>

typedef struct iNES_header {
    // Indicates a valid iNES-header
    bool is_valid_header;

    // The number of 16 KB PRG-ROM banks, where program code is stored
    uint8_t nr_prg_rom_banks;

    // The number of 8 KB PRG-ROM banks, where graphics information is stored
    uint8_t nr_chr_rom_banks;

    // False: horizontal mirroring; True: vertical mirroring
    bool mirror_type;

    // Indicates presence of battery-backed RAM
    bool has_battery_backed_ram;

    // Indicates the presence of a 512-byte trainer after the header
    bool has_trainer;

    // Indicates that four-screen mirroring should be used
    bool four_screen_mirroring;

    // Compose the mapper number from the lowest 4 bits of each control byte
    uint8_t mapper_number;
} iNES_header;

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

        // Parse the iNES-header
        iNES_header parse_header(std::ifstream& input);

    public:
        NES();

        void initialize(); // Set all registers and entire memory to 0
        bool load_rom(const char* rom_path); // Loads the ROM into memory
};

#endif