#ifndef INES_HEADER_TEST
#define INES_HEADER_TEST
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE

#include <boost/test/unit_test.hpp>
#include <fstream>

#include "../src/nes.h"

BOOST_AUTO_TEST_CASE(simple_test) {
    NES nes = NES();
    std::ifstream input("../roms/LegendOfZelda.nes", std::ios::binary);
    iNES_header header = nes.parse_header(input);

    BOOST_CHECK_EQUAL(header.is_valid_header, true);
    BOOST_CHECK_EQUAL(header.nr_prg_rom_banks , 8);
    BOOST_CHECK_EQUAL(header.nr_chr_rom_banks, 0);
    BOOST_CHECK_EQUAL(header.four_screen_mirroring, true);
    BOOST_CHECK_EQUAL(header.has_battery_backed_ram, false);
    BOOST_CHECK_EQUAL(header.has_trainer, false);
    BOOST_CHECK_EQUAL(header.mapper_number, 2);
    BOOST_CHECK_EQUAL(header.nr_ram_banks, 1);

    input.close();
}

#endif