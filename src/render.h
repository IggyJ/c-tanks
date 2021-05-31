#ifndef RENDER_H
#define RENDER_H


#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include "quadtree.h"
#include "game.h"


void renderTree(SDL_Renderer* renderer, Node* root, int x, int y, GameState* game);
void renderTreeBoundaries(SDL_Renderer* renderer, Node* root, int x, int y);
void doRender(SDL_Renderer* renderer, GameState* game);


#endif