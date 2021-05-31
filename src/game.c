#include "game.h"


void newTurn(GameState* game) {
    game->turn++;
    if (game->turn >= PLAYERS) {
        game->turn = 0;
    }
    game->tank[game->turn].justCollided = 0;
    game->stats[game->turn].fuel += 25;
    if (game->stats[game->turn].fuel > 100) {
        game->stats[game->turn].fuel = 100;
    }
    return;
}

void calculateDamage(GameState* game, int player) {
    double shellDistance = sqrt(pow((double)game->shell.x - (game->tank[player].x + game->tank[player].width/2), 2) + pow((double)game->shell.y - (game->tank[player].y + game->tank[player].height/2), 2));
    if (shellDistance < 70) {
        game->stats[player].hp -= (int)(pow(70 - shellDistance, 2)/120);
    }
    if (game->stats[player].hp <= 0) {
        game->endScreen = 1;
    }
    return;
}

int processEvents(SDL_Window* window, GameState* game) {
    SDL_Event event;
    int done = 0;

    // Handle key presses
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_WINDOWEVENT_CLOSE: {
                if(window) {
                    SDL_DestroyWindow(window);
                    window = NULL;
                    done = 1;
                }
                break;
            }
            case SDL_KEYDOWN: {
                switch(event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        done = 1;
                        break;
                    case SDLK_SPACE:
                        if(!game->shell.moving) {
                            game->shell.dx = (game->stats[game->turn].power)*cos(game->tank[game->turn].rot);
                            game->shell.dy = (game->stats[game->turn].power)*sin(game->tank[game->turn].rot);
                            game->shell.moving = 1;
                        }
                        break;
                    case SDLK_g:
                        if (game->grid == 0) {
                            game->grid = 1;
                        }
                        else {
                            game->grid = 0;
                        }
                        break;
                    case SDLK_w:
                        if (game->stats[game->turn].power < MAXPOWER) {
                            game->stats[game->turn].power++;
                        }
                        break;
                    case SDLK_s:
                        if (game->stats[game->turn].power > 6) {
                            game->stats[game->turn].power--;
                        }
                        break;
                }
                break;
            }
            case SDL_QUIT:
                done = 1;
                break;
        }
    }

    for (int p = 0; p < PLAYERS; p++) {
        game->tank[p].dx = 0;
        game->tank[p].moving = 0;
    }
    
    const Uint8* state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_A]) {
        if (game->stats[game->turn].fuel > 0) {
            game->tank[game->turn].dx = -1;
            game->tank[game->turn].justCollided = 0;
            game->tank[game->turn].moving = 1;
            game->stats[game->turn].fuel--;
        }
    }
    if (state[SDL_SCANCODE_D]) {  
        if (game->stats[game->turn].fuel > 0) {
            game->tank[game->turn].dx = 1;
            game->tank[game->turn].justCollided = 0;
            game->tank[game->turn].moving = 1;
            game->stats[game->turn].fuel--;
        }
    }    

    // Mouse cursor
    SDL_GetMouseState(&(game->mouse.x), &(game->mouse.y));

    for (int p = 0; p < PLAYERS; p++) {
        // Tanks
        if (game->tank[p].colliding) {
            if (game->tank[p].dy > 0) {
                game->tank[p].dy = 0;
            }
            game->tank[p].dy -= 1;
            game->tank[p].justCollided = 1;
        }
        else {
            game->tank[p].dy += GRAVITY;
            if (game->tank[p].justCollided) {
                game->tank[p].dy = 0;
            }
            game->tank[p].xPrev = game->tank[p].x;
            game->tank[p].yPrev = game->tank[p].y;
        }

        game->tank[p].x += game->tank[p].dx;
        game->tank[p].y += game->tank[p].dy;

        // Avoid climbing walls
        game->tank[p].touchingWall = 0;
        if (game->tank[p].y < game->tank[p].yPrev - 5) {
            game->tank[p].touchingWall = 1;
        }
        if (game->tank[p].y < game->tank[p].yPrev - 10) {
            game->tank[p].x = game->tank[p].xPrev;
            game->tank[p].y = game->tank[p].yPrev;
            game->tank[p].touchingWall = 1;
        }

        // Avoid leaving boundaries
        if (game->tank[p].x < 0) {
            game->tank[p].x = 0;
        } 
        if (game->tank[p].x + game->tank[p].width > 512) {
            game->tank[p].x = 512 - game->tank[p].width;
        }
        if (game->tank[p].y + game->tank[p].height > 512) {
            game->tank[p].y = 512 - game->tank[p].height;
            game->tank[p].dy = 0;
        }

        //Turrets
        game->turret[p].x1 = (int)((game->tank[p].touchingWall ? (game->tank[p].xPrev):(game->tank[p].x)) + game->tank[p].width/2);
        game->turret[p].y1 = (int)(game->tank[p].yPrev + 1);
        double x = (double)game->mouse.y - (double)game->turret[p].y1;
        double y = (double)game->mouse.x - (double)game->turret[p].x1;
        double theta = game->tank[p].rot;
        if (p == game->turn) {
            if (y >= 0) {
                theta = atan(x/y);
            }
            else {
                theta = atan(x/y) + M_PI;
            }
        }
        game->tank[p].rot = theta;
        game->turret[p].x2 = game->turret[p].x1 + (int)(14*cos(theta));
        game->turret[p].y2 = game->turret[p].y1 + (int)(14*sin(theta)); 
    }

    // Shell
    if(!game->shell.moving) {
        game->shell.x = (double)game->turret[game->turn].x2;
        game->shell.y = (double)game->turret[game->turn].y2;
    }
    else {
        game->shell.x += game->shell.dx;
        game->shell.y += game->shell.dy;
        game->shell.dy += GRAVITY;
        int op = !game->turn;
        if (game->shell.colliding || ( game->tank[op].x < game->shell.x && game->tank[op].x + game->tank[op].width >= game->shell.x && game->tank[op].y < game->shell.y && game->tank[op].y + game->tank[op].height >= game->shell.y )) {
            createCircle(game->root, game->shell.x - 40, game->shell.y - 40, 80, 0);
            calculateDamage(game, op);
            game->shell.moving = 0;
            game->shell.colliding = 0;
            game->shell.x = game->tank[game->turn].x + game->tank[game->turn].width/2;
            game->shell.y = game->tank[game->turn].y;
            game->shell.dx = 0;
            game->shell.dy = 0;
            game->tank[game->turn].justCollided = 0;
            newTurn(game);
        }
        if(game->shell.x > 512 || game->shell.x < 0 || game->shell.y > 512) {
            calculateDamage(game, !game->turn);
            game->shell.moving = 0;
            game->shell.colliding = 0;
            game->shell.x = game->tank[game->turn].x + game->tank[game->turn].width/2;
            game->shell.y = game->tank[game->turn].y;
            game->shell.dx = 0;
            game->shell.dy = 0;
            newTurn(game);
        }
    }

    // Trajectory
    for (int i = 0; i < AIM; i++) {
        game->trajectory[i].x = game->turret[game->turn].x2;
        game->trajectory[i].y = game->turret[game->turn].y2;
        double dy = (game->stats[game->turn].power)*sin(game->tank[game->turn].rot);
        for (int j = 0; j <= i*2; j++) {
            game->trajectory[i].x += (int)((game->stats[game->turn].power)*cos(game->tank[game->turn].rot));
            game->trajectory[i].y += (int)dy;
            dy += GRAVITY;
        }
    }

    return done;
}