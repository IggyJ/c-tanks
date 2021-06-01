#include <SDL2/SDL.h>
#include "game.h"
#include "render.h"
#include "init.h"


int main(int argc, char* argv[]) {
    // Initialise game and SDL components
    GameState game;
    SDL_Window* window;
    SDL_Renderer* renderer;

    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("Tanks",                    // Window title
                               SDL_WINDOWPOS_UNDEFINED,   // initial x position
                               SDL_WINDOWPOS_UNDEFINED,   // initial y position
                               TREE_SIZE,                 // width 
                               TREE_SIZE,                 // height
                               0                          // flags
                             );

    renderer = SDL_CreateRenderer(window, -1,
                                  SDL_RENDERER_ACCELERATED |
                                  SDL_RENDERER_PRESENTVSYNC
                                 );

    if (initGame(renderer, &game))
        return 1;

    // Main loop
    int done = 0;
    while(!done) {
        done = processEvents(window, &game);
        doRender(renderer, &game);
    }

    // Free allocated memory
    closeGame(renderer, window, &game);

    return 0;
}