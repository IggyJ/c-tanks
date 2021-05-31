#include "render.h"


void renderTree(SDL_Renderer* renderer, Node * root, int x, int y, GameState* game) {
    Node * node = root;
    int size = TREE_SIZE/(pow(2, node->depth));

    if (node->hasChildren == 0 && node->contents == 1) {
        SDL_Rect rect = {x, y, size, size};
        SDL_RenderFillRect(renderer, &rect);
        for (int p = 0; p < PLAYERS; p++) {
            if ( game->tank[p].x < x + size && game->tank[p].x + game->tank[p].width >= x && game->tank[p].y < y + size && game->tank[p].y + game->tank[p].height >= y ) {
                game->tank[p].colliding = 1;
            }
            if ( game->shell.x < x + size && game->shell.x + game->shell.width >= x && game->shell.y < y + size && game->shell.y + game->shell.height >= y ) {
                game->shell.colliding = 1;
            }
        }
    }

    if (node->hasChildren == 1) {
        for (int q = 0; q <= 3; q++) {
            if (node->child[q] != NULL) {
                renderTree(renderer, node->child[q], x + ( (q == NE || q == SE) ? (size/2):0), y + ( (q == SW || q == SE) ? (size/2):0), game);
            }
        }
    }
    return;
}

void renderTreeBoundaries(SDL_Renderer* renderer, Node* root, int x, int y) {
    Node * node = root;
    int size = TREE_SIZE/(pow(2, node->depth));

    SDL_RenderDrawLine(renderer, x, y, x + size, y);
    SDL_RenderDrawLine(renderer, x, y, x, y + size);

    if (node->hasChildren == 1) {
        for (int q = 0; q <= 3; q++) {
            if (node->child[q] != NULL) {
                renderTreeBoundaries(renderer, node->child[q], x + ( (q == NE || q == SE) ? (size/2):0), y + ( (q == SW || q == SE) ? (size/2):0));
            }
        }
    }
    return;
}

void doRender(SDL_Renderer* renderer, GameState* game) {
    // Fill screen
    SDL_SetRenderDrawColor(renderer, 176, 235, 255, 255);
    SDL_RenderClear(renderer);

    // Draw trajectory
    if (!game->shell.moving) {
        for (int i = 0; i < AIM; i++){
            SDL_SetRenderDrawColor(renderer, i*30, i*30, i*30, 255);
            SDL_Rect dot = {game->trajectory[i].x, game->trajectory[i].y, 2, 2};
            SDL_RenderFillRect(renderer, &dot);
        }
    }

    // Draw quadtree and update collisions
    for (int p = 0; p < PLAYERS; p++) {
        game->tank[p].colliding = 0;
    }
    SDL_SetRenderDrawColor(renderer, 253, 255, 122, 255);
    renderTree(renderer, game->root, 0, 0, game);

    // Draw quadtree boundaries
    if (game->grid == 1) {
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        renderTreeBoundaries(renderer, game->root, 0, 0);
    }


    // Draw shell
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    if (game->shell.moving) {
        SDL_Rect shell = {game->shell.x, game->shell.y, game->shell.width, game->shell.height};
        SDL_RenderFillRect(renderer, &shell);
    }
    
    // Draw players
    for (int p = 0; p < PLAYERS; p++) {
        SDL_RenderDrawLine(renderer, game->turret[p].x1, game->turret[p].y1, game->turret[p].x2, game->turret[p].y2);
        SDL_Rect body = {(game->tank[p].touchingWall && game->tank[p].colliding ? (game->tank[p].xPrev):(game->tank[p].x)),
                        (game->tank[p].colliding ? (game->tank[p].yPrev):(game->tank[p].y) ),
                            game->tank[p].width,
                            game->tank[p].height
                        };
        SDL_RenderCopy(renderer, game->tankTexture[p], NULL, &body);
    }

    // Draw status bar
    SDL_Rect statusBar = {0, 0, 512, 100};
    SDL_RenderCopy(renderer, game->statusBar, NULL, &statusBar);

    // P1 stats
    if (game->stats[0].hp > 0) {
        SDL_SetRenderDrawColor(renderer, 0, 207, 86, 255);          
        SDL_Rect p1HP = {55, 55, game->stats[0].hp, 10};
        SDL_RenderFillRect(renderer, &p1HP);
    }

    SDL_SetRenderDrawColor(renderer, 160, 9, 230, 255);
    SDL_Rect p1Fuel = {55, 67, game->stats[0].fuel, 10};
    SDL_RenderFillRect(renderer, &p1Fuel);
    
    SDL_SetRenderDrawColor(renderer, 252, 186, 3, 255);
    SDL_Rect p1Power = {55, 79, ((double)(game->stats[0].power - 6)/(MAXPOWER - 6))*100, 10};
    SDL_RenderFillRect(renderer, &p1Power);

    // P2 stats
    if (game->stats[1].hp > 0) {
        SDL_SetRenderDrawColor(renderer, 0, 207, 86, 255);
        SDL_Rect p2HP = {357, 55, game->stats[1].hp, 10};
        SDL_RenderFillRect(renderer, &p2HP);
    }

    SDL_SetRenderDrawColor(renderer, 160, 9, 230, 255);
    SDL_Rect p2Fuel = {357, 67, game->stats[1].fuel, 10};
    SDL_RenderFillRect(renderer, &p2Fuel);
    
    SDL_SetRenderDrawColor(renderer, 252, 186, 3, 255);
    SDL_Rect p2Power = {357, 79, ((double)(game->stats[1].power - 6)/(MAXPOWER - 6))*100, 10};
    SDL_RenderFillRect(renderer, &p2Power);

    // End screen
    if (game->endScreen) {
        SDL_Rect winScreen = {0, 0, 512, 512};
        if (game->stats[0].hp <= 0) {
            SDL_RenderCopy(renderer, game->winScreen[1], NULL, &winScreen);
        }
        if (game->stats[1].hp <= 0) {
            SDL_RenderCopy(renderer, game->winScreen[0], NULL, &winScreen);
        }
    }

    // Update screen
    SDL_RenderPresent(renderer);
}