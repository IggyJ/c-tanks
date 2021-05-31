#ifndef INIT_H
#define INIT_H

#include "game.h"


int initGame(SDL_Renderer* renderer, GameState* game);
void closeGame(SDL_Renderer* renderer, SDL_Window* window, GameState* game);


#endif