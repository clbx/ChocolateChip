
#include <string>
#include <iostream>
#include <SDL2/SDL.h>

#include "Chocolate.hpp"

// GUI Stuff Soon, just reall increases on compile time
//#include "imgui.h"
//#include "imgui_sdl.h"

using namespace std;


const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 320;


int main(int argc, char* argv[]){
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *win = SDL_CreateWindow("Chip 8 Emulator",100,100,SCREEN_WIDTH,SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *ren = SDL_CreateRenderer(win,-1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    SDL_RenderSetLogicalSize(ren,SCREEN_WIDTH,SCREEN_HEIGHT);    

    SDL_Texture *tex = SDL_CreateTexture(ren,SDL_PIXELFORMAT_ABGR8888,SDL_TEXTUREACCESS_STATIC,64,32);

    //temp gfx
    uint8_t gfx[2048];
    memset(gfx,0,2048*sizeof(uint8_t));

    //sdl pixels
    uint32_t pixels[2048];
    memset(pixels,0,2048*sizeof(uint32_t));

    bool loop = true;
    SDL_Event event;
    Chocolate chip = Chocolate();

    chip.load("maze");

    while(loop){
        //ImGuiIO& io = ImGui::GetIO();
        getchar();
        chip.tick();

        while(SDL_PollEvent(&event)){

            switch(event.type){
                case SDL_QUIT:
                    loop = false;
                    break;

            }

        }

        for(int i = 0; i < 2048; i++){
            uint8_t pixel = gfx[i];
            pixels[i] = (0x00FFFFFF * pixel) | 0xFF000000;
        }


        SDL_UpdateTexture(tex,NULL,pixels,64*sizeof(uint32_t));      
        SDL_RenderClear(ren);
        SDL_RenderCopy(ren,tex,NULL,NULL);
        SDL_RenderPresent(ren);

    }

    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
}
