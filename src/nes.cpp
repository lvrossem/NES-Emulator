#include "nes.h"

NES::NES() {
    initialize();
}

void NES::initialize() {
    PC = 0;
    SP = 0;
    A = 0;
    X = 0;
    Y = 0;
    P = 0;

    for (int i = 0; i < MEMORY_SIZE; i++) {
        memory[i] = 0;
    }
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

    // False: horizontal mirroring; True: vertical mirroring
    result.mirror_type = (control_byte_1 & 0x80) >> 7;

    // Indicates presence of battery-backed RAM
    result.has_battery_backed_ram = (control_byte_1 & 0x40) >> 6;

    // Indicates the presence of a 512-byte trainer after the header
    result.has_trainer = (control_byte_1 & 0x20) >> 5;

    // Indicates that four-screen mirroring should be used
    result.four_screen_mirroring = (control_byte_1 & 0x10) >> 4;

    if (control_byte_2 & 0xF0 == 0) {
        std::cout << "Invalid file header" << std::endl;
        return result;
    }

    // Compose the mapper number from the lowest 4 bits of each control byte
    result.mapper_number = ((control_byte_2 & 0x0F) << 4) | (control_byte_1 & 0x0F);

    result.is_valid_header = true;
    return result;
}

bool NES::load_rom(const char* rom_path) {
    std::ifstream input(rom_path);
    iNES_header rom_header = parse_header(input);
    return rom_header.is_valid_header;
}