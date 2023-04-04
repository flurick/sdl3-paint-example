#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL2/SDL_image.h>

/*
    References:
https://wiki.libsdl.org/SDL3/FrontPage
https://thenumb.at/cpp-course/index.html
save_texture() based on Czipperz answer https://stackoverflow.com/questions/34255820/save-sdl-texture-to-file
*/

void save_texture(const char* file_name, SDL_Renderer* renderer, SDL_Texture* texture) {   
    SDL_Texture* target = SDL_GetRenderTarget(renderer);
    SDL_SetRenderTarget(renderer, texture);
    int width, height;
    SDL_QueryTexture(texture, NULL, NULL, &width, &height);
    SDL_Surface* surface = SDL_CreateSurfaceFrom(0, width, height, 32, SDL_PIXELFORMAT_RGBA4444);

    printf("reading pixels..");
    SDL_RenderReadPixels(renderer, NULL, 0, surface->pixels, surface->pitch);
    printf("YAY!!! We finally made the pixel read happen!");
    IMG_SavePNG(surface, file_name);
    SDL_DestroySurface(surface);
    SDL_SetRenderTarget(renderer, target);
}

int main(){

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("sdl3 paint example", 1920, 1080, 0);
    renderer = SDL_CreateRenderer(window, NULL, 0);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA4444, SDL_TEXTUREACCESS_TARGET, 1920, 1080);

    //load image
    SDL_Texture* image = IMG_LoadTexture(renderer, "image.png");
    if (image){
        SDL_SetRenderTarget(renderer, texture);
        SDL_RenderTexture(renderer, image, NULL,NULL);
    }else{
        SDL_SetRenderDrawColor(renderer, 255,255,255, 100);
        SDL_RenderClear(renderer);
    }

    int is_running = 1;
    int is_changed = 1;
    Uint64 tick_count = 0;
    unsigned char* pixels;
    int pitch;
    int last_x;
    int last_y;
    while(is_running == 1) {

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_EVENT_QUIT:
                    is_running = 0;
                    break;
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    last_x = e.button.x;
                    last_y = e.button.y;
                    break;
                case SDL_EVENT_MOUSE_MOTION:
                    if (e.motion.state & SDL_BUTTON_LMASK){
                        SDL_SetRenderTarget(renderer, texture);
                        SDL_SetRenderDrawColor(renderer, 255,255,0, 255);
                        SDL_RenderLine(renderer, e.motion.x, e.motion.y, last_x, last_y);
                        last_x = e.motion.x;
                        last_y = e.motion.y;
                        is_changed = 1;
                    }
                    break;
            }
        }
        if (is_changed == 1){

            is_changed = 0;
            SDL_SetRenderTarget(renderer, NULL);
            SDL_SetRenderDrawColor(renderer, 127,127,127, 255);
            SDL_RenderClear(renderer);
            SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
            SDL_RenderTexture(renderer, texture, NULL, NULL); //is this right?
            SDL_RenderPresent(renderer);
            SDL_UpdateWindowSurface(window);
        }
    }
    save_texture("image.png", renderer, texture);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
