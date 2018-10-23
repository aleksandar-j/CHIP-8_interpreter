
#define _CRT_SECURE_NO_WARNINGS 1

#include <SDL.h>
#include <Windows.h>
#include <thread>

#include "chip8.hpp"

bool running = true;
const int SS_MULTIPLIER = 20;
const int SCREEN_WIDTH = SS_MULTIPLIER*64;
const int SCREEN_HEIGHT = SS_MULTIPLIER*32;

void timers(uint8_t* DT, uint8_t* ST) 
{

    while (running) {
        
        if ((*DT) > 0) {
            (*DT)--;
        }
        if ((*ST) > 0) {
            (*ST)--;
        }

        Sleep(16);

    }

}

void sounds(uint8_t* ST) 
{
    
    while (running) {

        if ((*ST) > 1) {
            Beep(1000, (*ST) * (1000/60));
        }

        Sleep(1);
    
    }

}

int main(int argc, char** argv)
{

    // Initialize randomness
    srand(time(NULL));
    
    // Initialize window
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("CHIP-8 Interpreter",
                                SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                SCREEN_WIDTH, SCREEN_HEIGHT,
                                SDL_WINDOW_SHOWN);

    SDL_Surface* window_surface = SDL_GetWindowSurface(window);
    SDL_Surface* our_surface = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0);

    // Making everything black
    for (size_t i = 0; i < SCREEN_WIDTH*SCREEN_HEIGHT; i++) {
        ((uint32_t*)our_surface->pixels)[i] = 0x0;
    }
    SDL_BlitSurface(our_surface, NULL, window_surface, NULL);
    SDL_UpdateWindowSurface(window);

    // Load code
    FILE* software = fopen("CODE.chip8", "r");
    if (software == NULL) {
        SDL_ShowSimpleMessageBox(0, "Error", "Couldn't find CODE file", NULL);
        return EXIT_FAILURE;
    }

    uint16_t* instructions = new uint16_t[4096]{0};
    fread(instructions, sizeof(uint16_t), 4096, software);

    fclose(software);

    Chip8 chip8(instructions, (uint32_t*)our_surface->pixels, SCREEN_WIDTH, SCREEN_HEIGHT);

    std::thread timer(timers, &chip8.DT, &chip8.ST);
    std::thread sound(sounds, &chip8.ST);

    while (running) {

        // Main emulator function, emulate one cycle
        chip8.execute_cycle();

        if (chip8.redraw_screen) {
            SDL_BlitSurface(our_surface, NULL, window_surface, NULL);
            SDL_UpdateWindowSurface(window);

            chip8.redraw_screen = false;
        }

        SDL_Event eve;
        while (SDL_PollEvent(&eve)) {
            
            switch (eve.type) 
            {
                case SDL_QUIT:
                {
                    running = false;
                } break;

                case SDL_KEYDOWN:
                {
                    switch (eve.key.keysym.sym) 
                    {
                        case SDLK_1: 
                        {
                            chip8.pressed[7] = true;
                        } break;
                        case SDLK_2:
                        {
                            chip8.pressed[8] = true;
                        } break;
                        case SDLK_3:
                        {
                            chip8.pressed[9] = true;
                        } break;
                        case SDLK_4:
                        {
                            chip8.pressed[0xC] = true;
                        } break;
                        case SDLK_q:
                        {
                            chip8.pressed[4] = true;
                        } break;
                        case SDLK_w:
                        {
                            chip8.pressed[5] = true;
                        } break;
                        case SDLK_e:
                        {
                            chip8.pressed[6] = true;
                        } break;
                        case SDLK_r:
                        {
                            chip8.pressed[0xD] = true;
                        } break;
                        case SDLK_a:
                        {
                            chip8.pressed[1] = true;
                        } break;
                        case SDLK_s:
                        {
                            chip8.pressed[2] = true;
                        } break;
                        case SDLK_d:
                        {
                            chip8.pressed[3] = true;
                        } break;
                        case SDLK_f:
                        {
                            chip8.pressed[0xE] = true;
                        } break;
                        case SDLK_z:
                        {
                            chip8.pressed[0xA] = true;
                        } break;
                        case SDLK_x:
                        {
                            chip8.pressed[0] = true;
                        } break;
                        case SDLK_c:
                        {
                            chip8.pressed[0xB] = true;
                        } break;
                        case SDLK_v:
                        {
                            chip8.pressed[0xF] = true;
                        } break;
                    }
                } break;
                case SDL_KEYUP:
                {
                    switch (eve.key.keysym.sym) 
                    {
                        case SDLK_1:
                        {
                            chip8.pressed[7] = false;
                        } break;
                        case SDLK_2:
                        {
                            chip8.pressed[8] = false;
                        } break;
                        case SDLK_3:
                        {
                            chip8.pressed[9] = false;
                        } break;
                        case SDLK_4:
                        {
                            chip8.pressed[0xC] = false;
                        } break;
                        case SDLK_q:
                        {
                            chip8.pressed[4] = false;
                        } break;
                        case SDLK_w:
                        {
                            chip8.pressed[5] = false;
                        } break;
                        case SDLK_e:
                        {
                            chip8.pressed[6] = false;
                        } break;
                        case SDLK_r:
                        {
                            chip8.pressed[0xD] = false;
                        } break;
                        case SDLK_a:
                        {
                            chip8.pressed[1] = false;
                        } break;
                        case SDLK_s:
                        {
                            chip8.pressed[2] = false;
                        } break;
                        case SDLK_d:
                        {
                            chip8.pressed[3] = false;
                        } break;
                        case SDLK_f:
                        {
                            chip8.pressed[0xE] = false;
                        } break;
                        case SDLK_z:
                        {
                            chip8.pressed[0xA] = false;
                        } break;
                        case SDLK_x:
                        {
                            chip8.pressed[0] = false;
                        } break;
                        case SDLK_c:
                        {
                            chip8.pressed[0xB] = false;
                        } break;
                        case SDLK_v:
                        {
                            chip8.pressed[0xF] = false;
                        } break;
                    }
                } break;
            }

        }

        static int sleep_time = 0;
        if (sleep_time >= 1) {
            sleep_time--;
        } else {
            sleep_time++;
        }
        Sleep(sleep_time);

    }

    timer.join();
    sound.join();

    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}