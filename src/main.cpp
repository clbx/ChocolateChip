#include <string>
#include <iostream>
#include <SDL2/SDL.h>

using namespace std;


const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;


int main(){
    SDL_Init(SDL_INIT_VIDEO);
    

    SDL_Window *win = SDL_CreateWindow("Chip 8 Emulator",100,100,SCREEN_WIDTH,SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *ren = SDL_CreateRenderer(win,-1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    SDL_RenderSetLogicalSize(ren,640,480);

    string imagePath = "./dude.bmp";
    SDL_Surface *bmp = SDL_LoadBMP(imagePath.c_str());
    if(bmp == nullptr){
        printf("poop");
    }
    
    /*
    SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, bmp);
    SDL_FreeSurface(bmp);
    */

   SDL_Texture *tex = SDL_CreateTexture(ren,SDL_PIXELFORMAT_ABGR8888,SDL_TEXTUREACCESS_STATIC,SCREEN_WIDTH,SCREEN_HEIGHT);

    Uint32 *pixels = new Uint32[640*480];

    memset(pixels,255,640*480*sizeof(Uint32));

    bool loop = true;
    bool mouseDown = false;
    SDL_Event event;

    while(loop){
        SDL_UpdateTexture(tex,NULL,pixels,640*sizeof(Uint32));

        SDL_WaitEvent(&event);

        switch(event.type){
            case SDL_QUIT:
                loop = false;
                break;
            case SDL_MOUSEBUTTONUP:
                if(event.button.button == SDL_BUTTON_LEFT){
                    mouseDown = false;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                if(event.button.button == SDL_BUTTON_LEFT){
                    mouseDown = true;
                }
                break;
            case SDL_MOUSEMOTION:
                if(mouseDown){
                    int mouseX = event.motion.x;
                    int mouseY = event.motion.y;
                    pixels[mouseY * 640 + mouseX] = 0;
                }
                break;
        }

        SDL_RenderClear(ren);
        SDL_RenderCopy(ren,tex,NULL,NULL);
        SDL_RenderPresent(ren);

    }




    



    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
}


void logSDLError(std::ostream &os, const std::string &msg){
    os << msg << " ERROR: " << SDL_GetError() << std::endl;
}