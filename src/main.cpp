#include <iostream>
#include <fstream>
#include <string>
#include <stdint.h>

#include "SDL2/SDL.h"
#include "nes.h"

int main(int argc, char **argv) {
    /*
    char nes[4] = "NES";
    char bits[4];
    bits[0] = 0x4E;
    bits[1] = 0x45;
    bits[2] = 0x53;
    bits[3] = 0x1A;
    bool test = 1;
    bool test2 = true;
    uint8_t z = 0xFA;
    std::cout << "Hello NES" << std::endl << ("a" > "A") << std::endl;
    */
    SDL_Window *window;

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cout << "Failed to initialize SDL" << std::endl;
        return 1;
    }

    window = SDL_CreateWindow("Chip-8", 
                               SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED,
                               FRAME_WIDTH * 8,
                               FRAME_HEIGHT * 8,
                               SDL_WINDOW_SHOWN);

    if (!window) {
        std::cout << "Failed to create window" << std::endl;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_RenderSetLogicalSize(renderer, FRAME_WIDTH * 8, FRAME_HEIGHT * 8);

    SDL_Texture* sdlTexture = SDL_CreateTexture(renderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            FRAME_HEIGHT * 8, FRAME_WIDTH * 8);

    NES nes = NES();
    uint32_t pixels[2048];

    load:
    // Attempt to load ROM
    if (nes.load_rom(argv[1])) {
        return 2;
    }
}