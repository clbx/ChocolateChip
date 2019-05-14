#include "main.hpp"
#include "Chocolate.hpp"

using namespace std;


const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 320;

SDL_Window* win;
SDL_Renderer* ren;
SDL_Texture* tex;

uint8_t keymap[16] = {
    SDLK_1,
    SDLK_2,
    SDLK_3,
    SDLK_4,
    SDLK_q,
    SDLK_w,
    SDLK_e,
    SDLK_r,
    SDLK_a,
    SDLK_s,
    SDLK_d,
    SDLK_f,
    SDLK_z,
    SDLK_x,
    SDLK_c,
    SDLK_v
};

/* Emulation Setings */
double emulationSpeed = 1;
char* filename = nullptr;
bool debug = false;
bool manual = false;



int main(int argc, char** argv){


    if(argc == 1){
        cout << "chocolate: try \"chocolate -h\" for more information" << endl;
        return(1);
    }

    filename = argv[2];

    bool cont = handleArgs(argc,argv);

    
    if(!cont){
        return(0);
    }

    SDL_Init(SDL_INIT_VIDEO);
    win = SDL_CreateWindow("Chip 8 Emulator",100,100,SCREEN_WIDTH,SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    ren = SDL_CreateRenderer(win,-1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    SDL_RenderSetLogicalSize(ren,SCREEN_WIDTH,SCREEN_HEIGHT);    

    tex = SDL_CreateTexture(ren,SDL_PIXELFORMAT_ABGR8888,SDL_TEXTUREACCESS_STATIC,64,32);

    uint32_t pixels[2048];
    memset(pixels,0,2048*sizeof(uint32_t));

    bool loop = true;
    SDL_Event event;
    Chocolate chip = Chocolate();

    chip.load(filename);

    while(loop){

        
        
        if(manual){
            getchar();
        }

        printf("\033c");

        chip.tick();

        while(SDL_PollEvent(&event)){

            switch(event.type){
                case SDL_QUIT:{
                    loop = false;
                } break;
                
                case SDL_KEYDOWN:{
                    for(int i = 0; i < 16; i++){
                        if(event.key.keysym.sym == keymap[i]){
                            chip.keymap[i] = 1;
                        }
                    }
                } break;

                case SDL_KEYUP:{
                    for(int i = 0; i < 16; i++){
                        if(event.key.keysym.sym == keymap[i]){
                            chip.keymap[i] = 0;
                        }
                    }                    
                } break;
                

            }

            

        }

        if(chip.draw){
            for(int i = 0; i < 2048; i++){
                uint8_t pixel = chip.graphics[i];
                pixels[i] = (0x00FFFFFF * pixel) | 0xFF000000;
            }

            SDL_UpdateTexture(tex,NULL,pixels,64*sizeof(uint32_t));      
            SDL_RenderClear(ren);
            SDL_RenderCopy(ren,tex,NULL,NULL);
            SDL_RenderPresent(ren);
        }

        usleep(1200 * emulationSpeed);

    }

    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
}


bool handleArgs(int argc, char** argv){
    //Get flags
    for(int i = 1; i < argc-1;){
        printf("%s\n",argv[i]);
        switch(argv[i][1]){
            case 'h':{help();return false;i++;}break;
            case 's':{emulationSpeed = atoi(argv[i+1]);i+=2;}break;
            case 'd':{debug = true;i++;}break;
            case 'm':{manual = true;i++;}break;
            default:{}break;
        }
    }
    filename = argv[argc-1];
    return true;
}


void help(){
    cout << "Usage: chocolate [options...] <rom>" << endl;
    cout << "Options" << endl;
    cout << "-h         Shows this message" << endl;
    cout << "-s SPEED   Emulation speed multiplier" << endl;
    cout << "-d         Shows debugging information" << endl;
    cout << "-m         Enables manual clocking" << endl;
}