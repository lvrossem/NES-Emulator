#ifndef NES_H
#define NES_H

#include <stdint.h>
#include <string.h>
#include <fstream>
#include <iostream>

#include "bus.h"

typedef struct Cartridge {
    // Indicates a valid iNES-header
    bool is_valid_header;

    // The number of 16 KB PRG-ROM banks, where program code is stored
    uint8_t nr_prg_rom_banks;

    // The number of 8 KB CHR-ROM banks, where graphics information is stored
    uint8_t nr_chr_rom_banks;

    // The number of 8 KB RAM banks
    uint8_t nr_ram_banks;

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
} Cartridge;


class NES {
private:
    Bus* bus;
    Controller* controller;
    Cartridge* cartridge;
public:
    NES();
    ~NES();

    bool load_rom(const char* rom_path); // Loads the ROM into memory
    void parse_header(std::ifstream& input); // Parse the iNES-header
    void execute_next_instruction();
    void change_button(uint8_t button_index, bool pressed);
};

#endif