#include "cartridge.h"

Cartridge::Cartridge(std::ifstream& input) {
    if (!input) {
        std::cout << "Failed to open ROM" << std::endl;
        exit(1);
    }

    // Read the 16-byte header into the buffer
    unsigned char header[16];
    
    input.read((char*) header, 16);

    /*
    for (int i = 0; i < 16; i++) {
        std::cout << (int) header[i] << std::endl;
    }
    */

    // Check if first 4 bytes are correct
    if (strncmp((char*) header, "NES", 3) != 0 || header[3] != 0x1A) {
        std::cout << "Invalid file header" << std::endl;
        exit(1);
    }

    nr_prg_rom_banks = (uint8_t) header[4];
    //std::cout << ((uint8_t) header[4] == 1) << " PRG BANKS" << std::endl;
    nr_chr_rom_banks = (uint8_t) header[5];

    uint8_t control_byte_1 = header[6];
    uint8_t control_byte_2 = header[7];

    mirror_type = control_byte_1 & 0x01;

    battery_backed_ram = (control_byte_1 & 0x02) >> 1;

    trainer = (control_byte_1 & 0x40) >> 2;

    four_screen_mirroring = (control_byte_1 & 0x08) >> 3;

    if (control_byte_2 & 0x0F != 0) {
        std::cout << "Invalid file header: Invalid control byte 2" << std::endl;
        exit(1);
    }

    // Compose the mapper number from the lowest 4 bits of each control byte
    mapper_number = (control_byte_2 & 0xF0) | ((control_byte_1 & 0xF0) >> 4);

    // If the header says 0, it means 1
    if (header[8] == 0) {
        nr_ram_banks = 1;
    } else {
        nr_ram_banks = header[8];
    }

    // Bytes 9 to 15 should be 0
    for (int i = 9; i < 16; i++) {
        if (header[i] != 0) {
            std::cout << "Invalid file header: bytes 9 to 15 not zero" << std::endl;
            exit(1);
        }
    }

    valid_header = true;
    print();
}

Cartridge::~Cartridge() {}

void Cartridge::print() {
    std::cout << "--- ROM INFO ---" << std::endl;
    std::cout << "Header valid: " << valid_header << std::endl;
    std::cout << "PRG ROM banks: " << (int) nr_prg_rom_banks << std::endl;
    std::cout << "CHR ROM banks: " << (int) nr_chr_rom_banks << std::endl;
    std::cout << "RAM banks: " << (int) nr_ram_banks << std::endl;
    std::cout << "Mirror type: " << mirror_type << std::endl;
    std::cout << "Battery backed RAM: " << battery_backed_ram << std::endl;
    std::cout << "Four screen mirroring: " << four_screen_mirroring << std::endl;
    std::cout << "Mapper number: " << (int) mapper_number << std::endl;
}
