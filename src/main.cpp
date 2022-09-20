#include <iostream>
#include <fstream>
#include <string>
#include <stdint.h>

#include "SDL2/SDL.h"
#include "nes.h"

// Controller keymap
unsigned int keymap[NR_OF_BUTTONS] = {
    SDLK_a,
    SDLK_b,
    SDLK_RSHIFT,
    SDLK_RETURN,
    SDLK_UP,
    SDLK_DOWN,
    SDLK_LEFT,
    SDLK_RIGHT,
};

int main(int argc, char **argv) {
    uint8_t a = 160;
    std::cout << ((uint16_t) a << 4) << std::endl;
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

    
    window = SDL_CreateWindow("Nemulator", 
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
    
    NES *nes = new NES();
    uint32_t pixels[2048];
    
    std::cout << "Loading ROM..." << std::endl;

    load:
    // Attempt to load ROM
    if (!nes->load_rom(argv[1])) {
        return 2;
    }
    std::cout << "HERE" << std::endl;
    while (true) {
        nes->execute_next_instruction();

        // Process SDL events
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                delete nes;
                exit(0);
            }

            // Process keydown events
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) {
                    delete nes;
                    exit(0);
                }

                if (e.key.keysym.sym == SDLK_F1) {
                    // Reset ROM
                    goto load;     
                }

                for (int i = 0; i < NR_OF_BUTTONS; ++i) {
                    if (e.key.keysym.sym == keymap[i]) {
                        std::cout << "DOWNPRESS DETECTED" << std::endl;
                        nes->change_button(i, true);

                        std::cout << e.key.keysym.sym << std::endl;
                    }
                }
            }
            // Process keyup events
            if (e.type == SDL_KEYUP) {
                for (int i = 0; i < NR_OF_BUTTONS; ++i) {
                    if (e.key.keysym.sym == keymap[i]) {
                        nes->change_button(i, false);
                    }
                }
            }
        }
    }
}