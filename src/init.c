#include "init.h"


int loadAssests(SDL_Renderer* renderer, GameState* game) {
    SDL_Surface* tankSurface[PLAYERS];
    SDL_Surface* statusBarSurface;
    SDL_Surface* winScreenSurface[PLAYERS];

    // Load image files into SDL surfaces
    for (int p = 0; p < PLAYERS; p++) {
        switch(p) {
            case 0:
                tankSurface[p] = IMG_Load("tankblue.png");
                winScreenSurface[p] = IMG_Load("p1wins.png");
                break;
            case 1:
                tankSurface[p] = IMG_Load("tankred.png");
                winScreenSurface[p] = IMG_Load("p2wins.png");
                break;
            default:
                tankSurface[p] = IMG_Load("tankgreen.png");
                break;
        }
        if (tankSurface[p] == NULL) {
            printf("Error, failed to load asset.\n\n");
            SDL_Quit();
            return 1;
        }
    }

    statusBarSurface = IMG_Load("statusbar.png");
    if (statusBarSurface == NULL) {
        printf("Error, failed to load asset.\n\n");
        SDL_Quit();
        return 1;
    } 

    // Create textures from surfaces
    for (int p = 0; p < PLAYERS; p++) {
        game->tankTexture[p] = SDL_CreateTextureFromSurface(renderer, tankSurface[p]);
        SDL_FreeSurface(tankSurface[p]); 
        game->winScreen[p] = SDL_CreateTextureFromSurface(renderer, winScreenSurface[p]);
        SDL_FreeSurface(winScreenSurface[p]);
    }

    game->statusBar = SDL_CreateTextureFromSurface(renderer, statusBarSurface);
    SDL_FreeSurface(statusBarSurface); 

    return 0;
}


int initGame(SDL_Renderer* renderer, GameState* game) {
    // Load game textures
    if (loadAssests(renderer, game))
        return 1;

    // Create quadtree
    game->root = malloc(sizeof(Node));
    game->root->hasChildren = 0;
    game->root->parent = NULL;
    game->root->contents = 0;
    game->root->depth = 0;
    for (int q = 0; q <= 3; q++) {
        game->root->child[q] = NULL;
    }

    srand(time(NULL));

    // Generate random terrain shape
    int r[10];
    for (int i = 0; i < sizeof(r)/sizeof(int); i++) {
        r[i] = (rand()%31);
    }
    generateTerrain(game->root, 0, 0, r);
    for (int i = 0; i <= MAX_DEPTH; i++) {
        optimiseTree(game->root);
    }

    // Initialise game variables and parameters
    game->grid = 0;
    game->turn = 0;
    game->endScreen = 0;

    for (int p = 0; p < PLAYERS; p++) {
        game->tank[p].x = 100 + (p*(600/PLAYERS));
        game->tank[p].y = 100;
        game->tank[p].dx = 0;
        game->tank[p].dy = 0;
        game->tank[p].width = 24;
        game->tank[p].height = 12;
        game->tank[p].colliding = 0;
        game->tank[p].justCollided = 0;
        game->tank[p].touchingWall = 0;
        game->tank[p].rot = 0;

        game->stats[p].hp = 100;
        game->stats[p].power = 10;
        game->stats[p].fuel = 100;
    }

    game->shell.width = 3;
    game->shell.height = 3;
    game->shell.colliding = 0;
    game->shell.moving = 0;

    return 0;
}


void closeGame(SDL_Renderer* renderer, SDL_Window* window, GameState* game) {
    // Free quadtree and textures
    destroyTree(game->root);

    SDL_DestroyTexture(game->statusBar);
    for (int p = 0; p < PLAYERS; p++) {
        SDL_DestroyTexture(game->tankTexture[p]);
        SDL_DestroyTexture(game->winScreen[p]);
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    SDL_Quit();
    return;
}