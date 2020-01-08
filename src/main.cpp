#include <iostream>
#include <thread>

#include <SDL.h>

#include "../includes/cpu.h"

#define WINDOW_HEIGHT 600
#define WINDOW_WIDTH 800

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "Pass in a path to a ROM file!" << std::endl;
        return -1;
    }
    
    CPU cpu;
    
    std::string file = argv[1];
    cpu.ReadRomFile(file);
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Couldn't initialize SDL:" << SDL_GetError() << std::endl;
        return -1;
    }
    
    SDL_Window *window = SDL_CreateWindow("CHIP-8 Emulator",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT,
                                          SDL_WINDOW_SHOWN);

    if (!window) {
        std::cerr << "Cannot create window: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, 
                                                -1, 
                                                SDL_RENDERER_ACCELERATED);

    if (!renderer) {
        std::cerr << "Cannot create renderer: " << SDL_GetError() << std::endl;
        return -1;
    }
    
    SDL_Texture *texture = SDL_CreateTexture(renderer,
                                             SDL_PIXELFORMAT_ARGB8888,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             COLS, ROWS);
    
    uint32_t pixels[DISPLAY_SIZE];

    bool running = true;
    while (running) {
        cpu.Execute();
        
        SDL_Event e;
        SDL_PollEvent(&e);

        if (e.type == SDL_QUIT) {
            running = false;
        }
        
        for (int i = 0; i < DISPLAY_SIZE; ++i) {
            uint8_t pixel = cpu.GetDisplayBuffer()[i];
            // Convert a 1 to white, 0 to black 
            pixels[i] = 0xFFFFFFFF * pixel;
        }
        
        SDL_UpdateTexture(texture, nullptr, pixels, COLS * sizeof(Uint32));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
        
        // The event loop should run at approximately 50Hz
        std::this_thread::sleep_for(std::chrono::microseconds(2000));
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    
    SDL_Quit();
    return 0;
}
