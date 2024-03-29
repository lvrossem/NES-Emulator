#ifndef PPU_H
#define PPU_H

#include <cstring>
#include <cstdint>

#define FRAME_WIDTH 0x20
#define FRAME_HEIGHT 0x1E

#define ATTRIBUTE_TABLE_BOTTOM 0x23C0
#define NAME_TABLE_BOTTOM 0x2000
#define PATTERN_TABLE_BOTTOM 0x0000

#define IMAGE_PALETTE_BOTTOM 0x3F00
#define SPRITE_PALETTE_BOTTOM 0x3F10

#define CONTROL_REGISTER_1 0x2000
#define CONTROL_REGISTER_2 0x2001

#include "bus.h"

/*
uint32_t colour_palette[64] = {
    0x666666FF, 0x002A88FF, 0x1412A7FF, 0x3B00A4FF,
    0x5C007EFF, 0x6E0040FF, 0x6C0600FF, 0x561D00FF,
    0x333500FF, 0x0B4800FF, 0x005200FF, 0x004F08FF,
    0x00404DFF, 0x000000FF, 0x000000FF, 0x000000FF,
    0xADADADFF, 0x155fD9FF, 0x4240FFFF, 0x7527FEFF,
    0xA01ACCFF, 0xB71E7BFF, 0xB53120FF, 0x994E00FF,
    0x6B6D00FF, 0x388700FF, 0x0C9300FF, 0x008F32FF,
    0x007C8DFF, 0x000000FF, 0x000000FF, 0x000000FF,
    0xFFFEFFFF, 0x64B0FFFF, 0x9290FFFF, 0xC676FFFF,
    0xF36AFFFF, 0xFE6ECCFF, 0xFE8170FF, 0xEA9E22FF,
    0xBCBE00FF, 0x88D800FF, 0x5CE430FF, 0x45E082FF,
    0x48CDDEFF, 0x4F4F4FFF, 0x000000FF, 0x000000FF,
    0xFFFEFFFF, 0xC0DFFFFF, 0xD3D2FFFF, 0xE8C8FFFF,
    0xFBC2FFFF, 0xFEC4EAFF, 0xFECCC5FF, 0xF7D8A5FF,
    0xE4E594FF, 0xCFEF96FF, 0xBDf4ABFF, 0xB3F3CCFF,
    0xB5EBF2FF, 0xB8B8B8FF, 0x000000FF, 0x000000FF
};
*/

class Bus;
// Picture processing unit
class PPU {
private:
    Bus* bus;

    constexpr static uint32_t colour_palette[64] = {
        0x757575, 0x271B8F, 0x0000AB, 0x47009F,
        0x8F0077, 0xAB0013, 0xA70000, 0x7F0B00,
        0x432F00, 0x004700, 0x005100, 0x003F17,
        0x1B3F5F, 0x000000, 0x000000, 0x000000,
        0xBCBCBC, 0x0073EF, 0x233BEF, 0x8300F3,
        0xBF00BF, 0xE7005B, 0xDB2B00, 0xCB4F0F,
        0x8B7300, 0x009700, 0x00AB00, 0x00933B,
        0x0083AB, 0x000000, 0x000000, 0x000000,
        0xFFFFFF, 0x3FBFFF, 0x5F97FF, 0xA78BFD,
        0xF77BFF, 0xFF77B7, 0xFF7763, 0xFF9B3B,
        0xF3BF3F, 0x83D313, 0x4FDF4B, 0x58F898,
        0x00EBDB, 0x000000, 0x000000, 0x000000,
        0xFFFFFF, 0xABE7FF, 0xC7D7FF, 0xFFCBDB,
        0xFFC7FF, 0xFFC7DB, 0xFFBFB3, 0xFFDBAB,
        0xFFE7A3, 0xE3FFA3, 0xABF3BF, 0xB3FFCF,
        0x9FFFF3, 0x000000, 0x000000, 0x000000
    };

    // Display matrix
    uint32_t** display;

    // Fill a block tile with the proper color values
    void fill_tile(uint8_t tile_x, uint8_t tile_y, uint8_t palette_index);

    // Fill a block quadrant with the proper color values
    void fill_quadrant(uint8_t upper_left_tile_x, uint8_t upper_left_tile_y, uint8_t palette_index);

    // Fill a block with the proper color values
    void fill_block(uint8_t block_nr);
public:
    PPU();
    ~PPU();

    void set_bus(Bus* bus_ptr) { this->bus = bus_ptr; }
    void draw();
    uint32_t** get_display() { return display; } 
};

#endif