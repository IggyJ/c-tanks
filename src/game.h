#ifndef GAME_H
#define GAME_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "quadtree.h"


#define GRAVITY 1
#define AIM 7        // number of trajectory dots
#define PLAYERS 2    // number of players
#define MAXPOWER 21  // max power allowed


typedef struct Entity {
    double x, y;            // position
    double xPrev, yPrev;    // position before colliding
    double dx, dy;          // velocity
    int width, height;      // dimensions
    double rot;             // rotation
    int colliding;
    int justCollided;
    int touchingWall;
    int moving;
} Entity;


typedef struct Stats {
    int hp;
    int power;
    int fuel;
} Stats;


typedef struct Point {
    int x, y;
} Point;


typedef struct Line {
    int x1, y1;
    int x2, y2;
} Line;


typedef struct GameState {
    // Game elements
    struct Node* root;                     // Root of quadtree
    struct Entity tank[PLAYERS];           // Player tanks
    struct Line turret[PLAYERS];           // Tank turrets
    struct Entity shell;                   // Tank shell
    struct Stats stats[PLAYERS];           // Game stats
    int turn;                              // Player turn
    struct Point mouse;                    // Cursor
    struct Point trajectory[AIM];          // Trajectory

    // Textures
    SDL_Texture* tankTexture[PLAYERS];
    SDL_Texture* statusBar;
    SDL_Texture* winScreen[PLAYERS];
    
    // Grid state
    int grid;

    // End Screen
    int endScreen;
} GameState;


void newTurn(GameState* game);
void calculateDamage(GameState* game, int player);
int processEvents(SDL_Window* window, GameState* game);


#endif