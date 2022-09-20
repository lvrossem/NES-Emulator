#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <stdint.h>
#include <string.h>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

class Cartridge {
    private:
        // Indicates a valid iNES-header
        bool valid_header;

        // The number of 16 KB PRG-ROM banks, where program code is stored
        uint8_t nr_prg_rom_banks;

        // The number of 8 KB CHR-ROM banks, where graphics information is stored
        uint8_t nr_chr_rom_banks;

        // The number of 8 KB RAM banks
        uint8_t nr_ram_banks;

        // False: horizontal mirroring; True: vertical mirroring
        bool mirror_type;

        // Indicates presence of battery-backed RAM
        bool battery_backed_ram;

        // Indicates the presence of a 512-byte trainer after the header
        bool trainer;

        // Indicates that four-screen mirroring should be used
        bool four_screen_mirroring;

        // Compose the mapper number from the lowest 4 bits of each control byte
        uint8_t mapper_number;

    public:
        Cartridge(std::ifstream& input);
        ~Cartridge();
        
        void print();

        bool is_valid_header() { return valid_header; }
        uint8_t get_nr_prg_rom_banks() { return nr_prg_rom_banks; }
        uint8_t get_nr_chr_rom_banks() { return nr_chr_rom_banks; }
        uint8_t get_nr_ram_banks() { return nr_ram_banks; }
        bool get_mirror_type() { return mirror_type; }
        bool has_battery_backed_ram() { return battery_backed_ram; }
        bool has_trainer() { return trainer; }
        bool has_four_screen_mirroring() { return four_screen_mirroring; }
        uint8_t get_mapper_number() { return mapper_number; }
};

#endif