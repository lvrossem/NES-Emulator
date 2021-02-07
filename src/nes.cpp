#include "nes.h"

NES::NES() {
    cpu = new CPU();
}

NES::~NES() {
    delete cpu;
}


iNES_header NES::parse_header(std::ifstream& input) {
    iNES_header result;
    result.is_valid_header = false;

    if (!input) {
        std::cout << "Failed to open ROM" << std::endl;
        return result;
    }

    // Read the 16-byte header into the buffer
    char header[16];
    input.read(header, 16);

    // Check if first 4 bytes are correct
    if (strncmp(header, "NES", 3) != 0 || header[3] != 0x1A) {
        std::cout << "Invalid file header" << std::endl;
        return result;
    }

    result.nr_prg_rom_banks = header[4];
    result.nr_chr_rom_banks = header[5];

    uint8_t control_byte_1 = header[6];
    uint8_t control_byte_2 = header[7];

    result.mirror_type = (control_byte_1 & 0x80) >> 7;

    result.has_battery_backed_ram = (control_byte_1 & 0x40) >> 6;

    result.has_trainer = (control_byte_1 & 0x20) >> 5;

    result.four_screen_mirroring = (control_byte_1 & 0x10) >> 4;

    if (control_byte_2 & 0xF0 == 0) {
        std::cout << "Invalid file header" << std::endl;
        return result;
    }

    // Compose the mapper number from the lowest 4 bits of each control byte
    result.mapper_number = ((control_byte_2 & 0x0F) << 4) | (control_byte_1 & 0x0F);

    result.nr_ram_banks = header[8];

    // If the header says 0, it means 1
    if (result.nr_ram_banks == 0) {
        result.nr_ram_banks = 1;
    }

    // Bytes 9 to 15 should be 0
    for (int i = 9; i < 16; i++) {
        if (header[i] != 0) {
            std::cout << "Invalid file header" << std::endl;
            return result;
        }
    }

    result.is_valid_header = true;
    return result;
}

bool NES::load_rom(const char* rom_path) {
    std::ifstream input(rom_path, std::ios::binary);
    iNES_header rom_header = parse_header(input);
    return rom_header.is_valid_header;
}