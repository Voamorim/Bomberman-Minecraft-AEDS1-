#include "raylib.h"
#include <math.h>
#include <stdlib.h>

#include "UpdateGame.h"
#include "InitGame.h"

#define STD_SIZE_X 30 // largura do jogador
#define STD_SIZE_Y 30 // altura do jogador
#define STD_SIZE_BOMB_X 20 // largura da bomba
#define STD_SIZE_BOMB_X2 30
#define STD_SIZE_BOMB_Y 20 // altura da bomba
#define STD_SIZE_BOMB_Y2 30
#define SCREEN_BORDER 120 // tamanho da borda
#define COUNTDOWN_DURATION 120 //duração máxima do jogo

int barrier_collision(Map *map, Rectangle target){
    for(int i = 0; i < map->num_barriers; i++){
        if(CheckCollisionRecs(target, map->barriers[i])){
            return 1;
        }
    }
    for(int i = 0; i < map->num_borders; i++){
        if(CheckCollisionRecs(target, map->borders[i])){
            return 1;
        }
    } 
    for(int i = 0; i < map->num_destrutivas; i++){
        if(CheckCollisionRecs(target, map->destrutivas[i])){
            return 1;
        }
    } 
    return 0;
}

void update_hero_pos(Game *g){

    Hero *h = &g->hero;
    Map *m = &g->maps[g->curr_map];
    Hero *h2 = &g->hero2;
    Map *m2 = &g->maps[g->curr_map];
    
    
    if(IsKeyDown(KEY_A)) {
        /* Verifica se o comando do jogador se movimentar pode ser executado */
        if(h->pos.x > SCREEN_BORDER && g->countdown_value > 0)
            h->pos.x -= h->speed;

        /* Faz a colisão dos jogadores com as bombas */
        for(int i = 0; i < g->hero.num_bombs; i++){
            if(g->hero.bombs[i].isActive == 1){
                if(!CheckCollisionRecs(g->hero.pos, g->hero.bombs[i].pos)){
                    g->hero.bombs[i].dono_saiu = 1;
                }
                if(fabs(g->hero.bombs[i].time - GetTime()) < 3 && g->hero.bombs[i].dono_saiu){
                    if(CheckCollisionRecs(g->hero.pos, g->hero.bombs[i].pos)){
                        g->hero.pos.x += h->speed;
                    }
                }
            }
        }  
        for(int i = 0; i < g->hero2.num_bombs; i++){
            if(g->hero2.bombs[i].isActive == 1){
                if(!CheckCollisionRecs(g->hero.pos, g->hero2.bombs[i].pos)){
                    g->hero2.bombs[i].adversario_saiu = 1;
                }
                if(fabs(g->hero2.bombs[i].time - GetTime()) < 3 && g->hero2.bombs[i].adversario_saiu){
                    if(CheckCollisionRecs(g->hero.pos, g->hero2.bombs[i].pos)){
                        g->hero.pos.x += h->speed;
                    }
                }
            }
        }
        /* Verifica se o jogador pegou algum poder */
        if(barrier_collision(m, h->pos))
            h->pos.x += h->speed;
        for(int i = 0; i < g->maps[g->curr_map].num_poder; i++){   
            if((CheckCollisionRecs(g->hero.pos,g->maps[g->curr_map].velocidade[i]))){
                g->hero.speed++;
                g->maps[g->curr_map].velocidade[i] = (Rectangle) {0, 0, 0,0};
            } else if ((CheckCollisionRecs(g->hero.pos, g->maps[g->curr_map].explosao[i]))){
                for(int j = 0; j < g->hero.num_bombs; j++){
                    g->hero.bombs[j].distance +=0.5;
                }
                g->maps[g->curr_map].explosao[i] = (Rectangle) {0, 0, 0,0};
            } else if ((CheckCollisionRecs(g->hero.pos, g->maps[g->curr_map].num_bombas[i]))){
                g->hero.num_bombs++;
                g->hero.bombs[g->hero.num_bombs-1].distance = g->hero.bombs[g->hero.num_bombs-2].distance;
                g->maps[g->curr_map].num_bombas[i] = (Rectangle) {0, 0, 0,0};
            }
        }

        /* O mesmo padrão se repete para todas as letras do teclado e seu respectivo movimento na tela */

    } else if(IsKeyDown(KEY_D)) {
        if(h->pos.x + h->pos.width < g->screenWidth - SCREEN_BORDER && g->countdown_value > 0)
            h->pos.x += h->speed;

        for(int i = 0; i < g->hero.num_bombs; i++){
            if(g->hero.bombs[i].isActive == 1){
                if(!CheckCollisionRecs(g->hero.pos, g->hero.bombs[i].pos)){
                    g->hero.bombs[i].dono_saiu = 1;
                }
                if(fabs(g->hero.bombs[i].time - GetTime()) < 3 && g->hero.bombs[i].dono_saiu){
                    if(CheckCollisionRecs(g->hero.pos, g->hero.bombs[i].pos)){
                        g->hero.pos.x -= h->speed;
                    }
                }
            }
        }  
        
        for(int i = 0; i < g->hero2.num_bombs; i++){
            if(g->hero2.bombs[i].isActive == 1){
                if(!CheckCollisionRecs(g->hero.pos, g->hero2.bombs[i].pos)){
                    g->hero2.bombs[i].adversario_saiu = 1;
                }
                if(fabs(g->hero2.bombs[i].time - GetTime()) < 3 && g->hero2.bombs[i].adversario_saiu){
                    if(CheckCollisionRecs(g->hero.pos, g->hero2.bombs[i].pos)){
                        g->hero.pos.x -= h->speed;
                    }
                }
            }
        }

        if(barrier_collision(m, h->pos)) 
            h->pos.x -= h->speed;
        for(int i = 0; i < g->maps[g->curr_map].num_poder; i++){ 
            if((CheckCollisionRecs(g->hero.pos,g->maps[g->curr_map].velocidade[i]))){
                g->hero.speed++;
                g->maps[g->curr_map].velocidade[i] = (Rectangle) {0, 0, 0,0};
            } else if ((CheckCollisionRecs(g->hero.pos, g->maps[g->curr_map].explosao[i]))){
                for(int j = 0; j < g->hero.num_bombs; j++){
                    g->hero.bombs[j].distance +=0.5;
                }
                g->maps[g->curr_map].explosao[i] = (Rectangle) {0, 0, 0,0};
            } else if ((CheckCollisionRecs(g->hero.pos, g->maps[g->curr_map].num_bombas[i]))){
                g->hero.num_bombs++;
                g->hero.bombs[g->hero.num_bombs-1].distance = g->hero.bombs[g->hero.num_bombs-2].distance;
                g->maps[g->curr_map].num_bombas[i] = (Rectangle) {0, 0, 0,0};
            }
        }
    } else if(IsKeyDown(KEY_W)) {
        if(h->pos.y > SCREEN_BORDER && g->countdown_value > 0)
            h->pos.y -= h->speed;
        
        for(int i = 0; i < g->hero.num_bombs; i++){
            if(g->hero.bombs[i].isActive == 1){
                if(!CheckCollisionRecs(g->hero.pos, g->hero.bombs[i].pos)){
                    g->hero.bombs[i].dono_saiu = 1;
                }
                if(fabs(g->hero.bombs[i].time - GetTime()) < 3 && g->hero.bombs[i].dono_saiu){
                    if(CheckCollisionRecs(g->hero.pos, g->hero.bombs[i].pos)){
                        g->hero.pos.y += h->speed;
                    }
                }
            }
        }  
        
        for(int i = 0; i < g->hero2.num_bombs; i++){
            if(g->hero2.bombs[i].isActive == 1){
                if(!CheckCollisionRecs(g->hero.pos, g->hero2.bombs[i].pos)){
                    g->hero2.bombs[i].adversario_saiu = 1;
                }
                if(fabs(g->hero2.bombs[i].time - GetTime()) < 3 && g->hero2.bombs[i].adversario_saiu){
                    if(CheckCollisionRecs(g->hero.pos, g->hero2.bombs[i].pos)){
                        g->hero.pos.y += h->speed;
                    }
                }
            }
        }

        if(barrier_collision(m, h->pos)) 
            h->pos.y += h->speed;
        for(int i = 0; i < g->maps[g->curr_map].num_poder; i++){ 
            if((CheckCollisionRecs(g->hero.pos,g->maps[g->curr_map].velocidade[i]))){
                g->hero.speed++;
                g->maps[g->curr_map].velocidade[i] = (Rectangle) {0, 0, 0,0};
            } else if ((CheckCollisionRecs(g->hero.pos, g->maps[g->curr_map].explosao[i]))){
                for(int j = 0; j < g->hero.num_bombs; j++){
                    g->hero.bombs[j].distance +=0.5;
                }
                g->maps[g->curr_map].explosao[i] = (Rectangle) {0, 0, 0,0};
            } else if ((CheckCollisionRecs(g->hero.pos, g->maps[g->curr_map].num_bombas[i]))){
                g->hero.num_bombs++;
                g->hero.bombs[g->hero.num_bombs-1].distance = g->hero.bombs[g->hero.num_bombs-2].distance;
                g->maps[g->curr_map].num_bombas[i] = (Rectangle) {0, 0, 0,0};
            }
        }
    } else if (IsKeyDown(KEY_S)) {
        if(h->pos.y + h->pos.height < g->screenHeight+30 - SCREEN_BORDER && g->countdown_value > 0)
            h->pos.y += h->speed;

        for(int i = 0; i < g->hero.num_bombs; i++){
            if(g->hero.bombs[i].isActive == 1){
                if(!CheckCollisionRecs(g->hero.pos, g->hero.bombs[i].pos)){
                    g->hero.bombs[i].dono_saiu = 1;
                }
                if(fabs(g->hero.bombs[i].time - GetTime()) < 3 && g->hero.bombs[i].dono_saiu){
                    if(CheckCollisionRecs(g->hero.pos, g->hero.bombs[i].pos)){
                        g->hero.pos.y -= h->speed;
                    }
                }
            }
        }  
        
        for(int i = 0; i < g->hero2.num_bombs; i++){
            if(g->hero2.bombs[i].isActive == 1){
                if(!CheckCollisionRecs(g->hero.pos, g->hero2.bombs[i].pos)){
                    g->hero2.bombs[i].adversario_saiu = 1;
                }
                if(fabs(g->hero2.bombs[i].time - GetTime()) < 3 && g->hero2.bombs[i].adversario_saiu){
                    if(CheckCollisionRecs(g->hero.pos, g->hero2.bombs[i].pos)){
                        g->hero.pos.y -= h->speed;
                    }
                }
            }
        }

        if(barrier_collision(m, h->pos)) 
            h->pos.y -= h->speed;
        for(int i = 0; i < g->maps[g->curr_map].num_poder; i++){      
            if((CheckCollisionRecs(g->hero.pos,g->maps[g->curr_map].velocidade[i]))){
                g->hero.speed++;
                g->maps[g->curr_map].velocidade[i] = (Rectangle) {0, 0, 0,0};
            } else if ( (CheckCollisionRecs(g->hero.pos, g->maps[g->curr_map].explosao[i]))){
                for(int j = 0; j < g->hero.num_bombs; j++){
                    g->hero.bombs[j].distance +=0.5;
                }
                g->maps[g->curr_map].explosao[i] = (Rectangle) {0, 0, 0,0};
            } else if ((CheckCollisionRecs(g->hero.pos, g->maps[g->curr_map].num_bombas[i]))){
                g->hero.num_bombs++;
                g->hero.bombs[g->hero.num_bombs-1].distance = g->hero.bombs[g->hero.num_bombs-2].distance;
                g->maps[g->curr_map].num_bombas[i] = (Rectangle) {0, 0, 0,0};
            }
        }
    }
    if(IsKeyDown(KEY_LEFT)) {
        if(h2->pos.x > SCREEN_BORDER && g->countdown_value > 0)
            h2->pos.x -= h2->speed;

        for(int i = 0; i < g->hero2.num_bombs; i++){
            if(g->hero2.bombs[i].isActive == 1){
                if(!CheckCollisionRecs(g->hero2.pos, g->hero2.bombs[i].pos)){
                    g->hero2.bombs[i].dono_saiu = 1;
                }
                if(fabs(g->hero2.bombs[i].time - GetTime()) < 3 && g->hero2.bombs[i].dono_saiu){
                    if(CheckCollisionRecs(g->hero2.pos, g->hero2.bombs[i].pos)){
                        g->hero2.pos.x += h2->speed;
                    }
                }
            }
        }  
        
        for(int i = 0; i < g->hero.num_bombs; i++){
            if(g->hero.bombs[i].isActive == 1){
                if(!CheckCollisionRecs(g->hero2.pos, g->hero.bombs[i].pos)){
                    g->hero.bombs[i].adversario_saiu = 1;
                }
                if(fabs(g->hero.bombs[i].time - GetTime()) < 3 && g->hero.bombs[i].adversario_saiu){
                    if(CheckCollisionRecs(g->hero2.pos, g->hero.bombs[i].pos)){
                        g->hero2.pos.x += h2->speed;
                    }
                }
            }
        }

        if(barrier_collision(m2, h2->pos)) 
            h2->pos.x += h2->speed;
        for(int i = 0; i < g->maps[g->curr_map].num_poder; i++){
            if((CheckCollisionRecs(g->hero2.pos,g->maps[g->curr_map].velocidade[i]))){
                g->hero2.speed++;
                g->maps[g->curr_map].velocidade[i] = (Rectangle) {0, 0, 0,0};
            } else if ((CheckCollisionRecs(g->hero2.pos, g->maps[g->curr_map].explosao[i]))){
                for(int j = 0; j < g->hero2.num_bombs; j++){
                    g->hero2.bombs[j].distance +=0.5;
                }
                g->maps[g->curr_map].explosao[i] = (Rectangle) {0, 0, 0,0};
            } else if ((CheckCollisionRecs(g->hero2.pos, g->maps[g->curr_map].num_bombas[i]))){
                g->hero2.num_bombs++;
                g->hero2.bombs[g->hero2.num_bombs-1].distance = g->hero2.bombs[g->hero2.num_bombs-2].distance;
                g->maps[g->curr_map].num_bombas[i] = (Rectangle) {0, 0, 0,0};
            }
        }
    } else if(IsKeyDown(KEY_RIGHT)) {
        if(h2->pos.x + h2->pos.width < g->screenWidth - SCREEN_BORDER && g->countdown_value > 0)
            h2->pos.x += h2->speed;

        for(int i = 0; i < g->hero2.num_bombs; i++){
            if(g->hero2.bombs[i].isActive == 1){
                if(!CheckCollisionRecs(g->hero2.pos, g->hero2.bombs[i].pos)){
                    g->hero2.bombs[i].dono_saiu = 1;
                }
                if(fabs(g->hero2.bombs[i].time - GetTime()) < 3 && g->hero2.bombs[i].dono_saiu){
                    if(CheckCollisionRecs(g->hero2.pos, g->hero2.bombs[i].pos)){
                        g->hero2.pos.x -= h2->speed;
                    }
                }
            }
        }  
        
        for(int i = 0; i < g->hero.num_bombs; i++){
            if(g->hero.bombs[i].isActive == 1){
                if(!CheckCollisionRecs(g->hero2.pos, g->hero.bombs[i].pos)){
                    g->hero.bombs[i].adversario_saiu = 1;
                }
                if(fabs(g->hero.bombs[i].time - GetTime()) < 3 && g->hero.bombs[i].adversario_saiu){
                    if(CheckCollisionRecs(g->hero2.pos, g->hero.bombs[i].pos)){
                        g->hero2.pos.x -= h2->speed;
                    }
                }
            }
        }

        if(barrier_collision(m2, h2->pos)) 
            h2->pos.x -= h2->speed;
        for(int i = 0; i < g->maps[g->curr_map].num_poder; i++){   
            if((CheckCollisionRecs(g->hero2.pos,g->maps[g->curr_map].velocidade[i]))){
                g->hero2.speed++;
            g->maps[g->curr_map].velocidade[i] = (Rectangle) {0, 0, 0,0};
            } else if ((CheckCollisionRecs(g->hero2.pos, g->maps[g->curr_map].explosao[i]))){
                for(int j = 0; j < g->hero2.num_bombs; j++){
                     g->hero2.bombs[j].distance +=0.5;
                }
                g->maps[g->curr_map].explosao[i] = (Rectangle) {0, 0, 0,0};
            } else if ((CheckCollisionRecs(g->hero2.pos, g->maps[g->curr_map].num_bombas[i]))){
                g->hero2.num_bombs++;
                g->hero2.bombs[g->hero2.num_bombs-1].distance = g->hero2.bombs[g->hero2.num_bombs-2].distance;
                g->maps[g->curr_map].num_bombas[i] = (Rectangle) {0, 0, 0,0};
            }
        }
    } else if(IsKeyDown(KEY_UP)) {
        if(h2->pos.y > SCREEN_BORDER && g->countdown_value > 0)
            h2->pos.y -= h2->speed;

        for(int i = 0; i < g->hero2.num_bombs; i++){
            if(g->hero2.bombs[i].isActive == 1){
                if(!CheckCollisionRecs(g->hero2.pos, g->hero2.bombs[i].pos)){
                    g->hero2.bombs[i].dono_saiu = 1;
                }
                if(fabs(g->hero2.bombs[i].time - GetTime()) < 3 && g->hero2.bombs[i].dono_saiu){
                    if(CheckCollisionRecs(g->hero2.pos, g->hero2.bombs[i].pos)){
                        g->hero2.pos.y += h2->speed;
                    }
                }
            }
        }  
        
        for(int i = 0; i < g->hero.num_bombs; i++){
            if(g->hero.bombs[i].isActive == 1){
                if(!CheckCollisionRecs(g->hero2.pos, g->hero.bombs[i].pos)){
                    g->hero.bombs[i].adversario_saiu = 1;
                }
                if(fabs(g->hero.bombs[i].time - GetTime()) < 3 && g->hero.bombs[i].adversario_saiu){
                    if(CheckCollisionRecs(g->hero2.pos, g->hero.bombs[i].pos)){
                        g->hero2.pos.y += h2->speed;
                    }
                }
            }
        }

        if(barrier_collision(m2, h2->pos)) 
            h2->pos.y += h2->speed;
        for(int i = 0; i < g->maps[g->curr_map].num_poder; i++){ 
            if((CheckCollisionRecs(g->hero2.pos,g->maps[g->curr_map].velocidade[i]))){
                g->hero2.speed++;
                g->maps[g->curr_map].velocidade[i] = (Rectangle) {0, 0, 0,0};
            } else if ((CheckCollisionRecs(g->hero2.pos, g->maps[g->curr_map].explosao[i]))){
                for(int j = 0; j < g->hero2.num_bombs; j++){
                    g->hero2.bombs[j].distance +=0.5;
                }
                g->maps[g->curr_map].explosao[i] = (Rectangle) {0, 0, 0,0};
            }else if((CheckCollisionRecs(g->hero2.pos, g->maps[g->curr_map].num_bombas[i]))){
                g->hero2.num_bombs++;
                g->hero2.bombs[g->hero2.num_bombs-1].distance = g->hero2.bombs[g->hero2.num_bombs-2].distance;
                g->maps[g->curr_map].num_bombas[i] = (Rectangle) {0, 0, 0,0};
            }
        }
    } else if(IsKeyDown(KEY_DOWN)) {
        if(h2->pos.y + h2->pos.height < g->screenHeight - SCREEN_BORDER+30 && g->countdown_value > 0)
            h2->pos.y += h2->speed;

        for(int i = 0; i < g->hero2.num_bombs; i++){
            if(g->hero2.bombs[i].isActive == 1){
                if(!CheckCollisionRecs(g->hero2.pos, g->hero2.bombs[i].pos)){
                    g->hero2.bombs[i].dono_saiu = 1;
                }
                if(fabs(g->hero2.bombs[i].time - GetTime()) < 3 && g->hero2.bombs[i].dono_saiu){
                    if(CheckCollisionRecs(g->hero2.pos, g->hero2.bombs[i].pos)){
                        g->hero2.pos.y -= h2->speed;
                    }
                }
            }
        }  
        
        for(int i = 0; i < g->hero.num_bombs; i++){
            if(g->hero.bombs[i].isActive == 1){
                if(!CheckCollisionRecs(g->hero2.pos, g->hero.bombs[i].pos)){
                    g->hero.bombs[i].adversario_saiu = 1;
                }
                if(fabs(g->hero.bombs[i].time - GetTime()) < 3 && g->hero.bombs[i].adversario_saiu){
                    if(CheckCollisionRecs(g->hero2.pos, g->hero.bombs[i].pos)){
                        g->hero2.pos.y -= h2->speed;
                    }
                }
            }
        }

        if(barrier_collision(m2, h2->pos))
            h2->pos.y -= h2->speed;
        for(int i = 0; i < g->maps[g->curr_map].num_poder; i++){
            if( (CheckCollisionRecs(g->hero2.pos,g->maps[g->curr_map].velocidade[i]))){
                g->hero2.speed++;
                g->maps[g->curr_map].velocidade[i] = (Rectangle) {0, 0, 0,0};
            } else if ((CheckCollisionRecs(g->hero2.pos, g->maps[g->curr_map].explosao[i]))){
                for(int j = 0; j < g->hero2.num_bombs; j++){
                    g->hero2.bombs[j].distance +=0.5;
                }
                g->maps[g->curr_map].explosao[i] = (Rectangle) {0, 0, 0,0};
            }else if ((CheckCollisionRecs(g->hero2.pos, g->maps[g->curr_map].num_bombas[i]))){
                g->hero2.num_bombs++;
                g->hero2.bombs[g->hero2.num_bombs-1].distance = g->hero2.bombs[g->hero2.num_bombs-2].distance;
                g->maps[g->curr_map].num_bombas[i] = (Rectangle) {0, 0, 0,0};
            }
        }
        
    }
}

void update_bomb(Game *g, Sound tnt_sound){
    Map *actual_map = &g->maps[g->curr_map];
    if(IsKeyPressed(KEY_SPACE)){
        int aux = 0;
        for(int i = 0; i<g->hero.num_bombs; i++){
            if(CheckCollisionRecs(g->hero.bombs[i].pos, g->hero.pos)){
                aux++;
            }
        }
        if(aux == 0){
            g->hero.put_bomb = 1;  
        }
    }
    if(g->hero.put_bomb == 1){
        for(int i = 0; i < g->hero.num_bombs; i++){
            if(g->hero.bombs[i].isActive == 0){
                PlaySound(tnt_sound);
                g->hero.bombs[i].isActive = 1;
                g->hero.bombs[i].pos = (Rectangle) {g->hero.pos.x, g->hero.pos.y, STD_SIZE_BOMB_X, STD_SIZE_BOMB_Y};
                g->hero.bombs[i].explosion_right = (Rectangle) {g->hero.pos.x, g->hero.pos.y, STD_SIZE_BOMB_X, STD_SIZE_BOMB_Y};
                g->hero.bombs[i].explosion_left = (Rectangle) {g->hero.pos.x, g->hero.pos.y, STD_SIZE_BOMB_X, STD_SIZE_BOMB_Y};
                g->hero.bombs[i].explosion_down = (Rectangle) {g->hero.pos.x, g->hero.pos.y, STD_SIZE_BOMB_X, STD_SIZE_BOMB_Y};
                g->hero.bombs[i].explosion_up = (Rectangle) {g->hero.pos.x, g->hero.pos.y, STD_SIZE_BOMB_X, STD_SIZE_BOMB_Y};

                g->hero.bombs[i].time = GetTime();
                break;
            }
        }
    }

    for(int i = 0; i < g->hero.num_bombs; i++){
        if(g->hero.bombs[i].isActive == 1){
            if(fabs(g->hero.bombs[i].time - GetTime()) > 3 && fabs(g->hero.bombs[i].time - GetTime()) < 5){
                Rectangle verify_bomb;
                Rectangle rectangle_bomb;
                double grow_tax = g->hero.bombs[i].distance;
                if(g->hero.bombs[i].explosion_right.width < g->hero.bombs[i].distance * STD_SIZE_X){
                    rectangle_bomb = g->hero.bombs[i].explosion_right;
                    Rectangle verify_bomb = (Rectangle){ rectangle_bomb.x, rectangle_bomb.y, rectangle_bomb.width + grow_tax,  rectangle_bomb.height};
                    for(int i = 0; i < g->maps[g->curr_map].num_destrutivas; i++){           
                        if(CheckCollisionRecs(g->maps[g->curr_map].destrutivas[i], verify_bomb)){
                            int aux = rand() % 4 + 1;
                            if(aux == 1){
                                g->maps[g->curr_map].velocidade[i] = (Rectangle) {g->maps[g->curr_map].destrutivas[i].x, g->maps[g->curr_map].destrutivas[i].y, 20, 20};
                            } else if(aux == 2){
                                g->maps[g->curr_map].explosao[i] = (Rectangle) {g->maps[g->curr_map].destrutivas[i].x, g->maps[g->curr_map].destrutivas[i].y, 20, 20};
                            } else if(aux == 3){
                                g->maps[g->curr_map].num_bombas[i] = (Rectangle) {g->maps[g->curr_map].destrutivas[i].x, g->maps[g->curr_map].destrutivas[i].y, 20, 20};
                            }
                            g->maps[g->curr_map].destrutivas[i] = (Rectangle) {0,0,0,0};     
                        }
                    }
                    if(!barrier_collision(actual_map, verify_bomb)){
                        g->hero.bombs[i].explosion_right.width += grow_tax;
                    } 

                    if(CheckCollisionRecs(g->hero.pos,rectangle_bomb) && CheckCollisionRecs(g->hero2.pos, rectangle_bomb)){
                        g->hero.isAlive = 0;
                        g->hero2.isAlive = 0;
                        break;
                    }

                    if(CheckCollisionRecs(g->hero.pos,rectangle_bomb)){
                        g->hero2.vitorias += 1;
                        g->hero.isAlive = 0;
                        break;
                    } 
                    if(CheckCollisionRecs(g->hero2.pos,rectangle_bomb )){
                        g->hero.vitorias += 1;
                        g->hero2.isAlive = 0;
                        break;
                    } 
                    for(int j = 0 ; j< g->hero.num_bombs;j++){
                        if(CheckCollisionRecs(g->hero.bombs[j].pos,rectangle_bomb)  && i!= j ){
                            if(fabs(g->hero.bombs[j].time - GetTime()<=3)){
                                g->hero.bombs[j].time = GetTime() -3; 
                            } 
                        }
                    }
                    for(int j = 0; j<g->hero.num_bombs;j++){
                        if(CheckCollisionRecs(g->hero2.bombs[j].pos,rectangle_bomb) ){
                            if(fabs(g->hero2.bombs[j].time - GetTime()<=3)){
                                g->hero2.bombs[j].time = GetTime() -3;                          
                            } 
                        }        
                    }
                }
                if(g->hero.bombs[i].explosion_left.width < g->hero.bombs[i].distance * STD_SIZE_X){
                    rectangle_bomb = g->hero.bombs[i].explosion_left;
                    Rectangle verify_bomb = (Rectangle){ rectangle_bomb.x - grow_tax, rectangle_bomb.y, rectangle_bomb.width + grow_tax,  rectangle_bomb.height};
                    for(int i = 0; i < g->maps[g->curr_map].num_destrutivas; i++){
                        if(CheckCollisionRecs(g->maps[g->curr_map].destrutivas[i], verify_bomb)){
                            int aux = rand() % 4 + 1;
                            if(aux == 1){
                                g->maps[g->curr_map].velocidade[i] = (Rectangle) {g->maps[g->curr_map].destrutivas[i].x, g->maps[g->curr_map].destrutivas[i].y, 20, 20};
                            } else if(aux == 2){
                                g->maps[g->curr_map].explosao[i] = (Rectangle) {g->maps[g->curr_map].destrutivas[i].x, g->maps[g->curr_map].destrutivas[i].y, 20, 20};
                            } else if(aux == 3){
                                g->maps[g->curr_map].num_bombas[i] = (Rectangle) {g->maps[g->curr_map].destrutivas[i].x, g->maps[g->curr_map].destrutivas[i].y, 20, 20};
                            }
                            g->maps[g->curr_map].destrutivas[i] = (Rectangle) {0,0,0,0};     
                        }      
                    }
                    for(int j = 0 ; j< g->hero.num_bombs;j++){
                        if(CheckCollisionRecs(g->hero.bombs[j].pos,rectangle_bomb)  && i!= j && g->hero.bombs[j].isActive == 1){
                            if(fabs(g->hero.bombs[j].time - GetTime()<=3)){
                                g->hero.bombs[j].time = GetTime() -3;  
                            }
                        }
                    }
                    for(int j = 0; j<g->hero.num_bombs;j++){
                        if(CheckCollisionRecs(g->hero2.bombs[j].pos,rectangle_bomb) ){
                            if(fabs(g->hero2.bombs[j].time - GetTime()<=3)){
                                g->hero2.bombs[j].time = GetTime() -3;
                            } 
                        }
                    }
                    if(!barrier_collision(actual_map, verify_bomb)){
                        g->hero.bombs[i].explosion_left.width += grow_tax;
                        g->hero.bombs[i].explosion_left.x -= grow_tax;
                    } 
                    if(CheckCollisionRecs(g->hero.pos,rectangle_bomb) && CheckCollisionRecs(g->hero2.pos, rectangle_bomb)){
                        g->hero.isAlive = 0;
                        g->hero2.isAlive = 0;
                        break;
                    }

                    if(CheckCollisionRecs(g->hero.pos,rectangle_bomb)){
                        g->hero2.vitorias += 1;
                        g->hero.isAlive = 0;
                        break;
                    } 
                    if(CheckCollisionRecs(g->hero2.pos,rectangle_bomb )){
                        g->hero.vitorias += 1;
                        g->hero2.isAlive = 0;
                        break;
                    } 
                }
                if(g->hero.bombs[i].explosion_up.height < g->hero.bombs[i].distance * STD_SIZE_Y){
                    rectangle_bomb = g->hero.bombs[i].explosion_up;
                    Rectangle verify_bomb = (Rectangle){ rectangle_bomb.x, rectangle_bomb.y, rectangle_bomb.width, rectangle_bomb.height + grow_tax};
                    for(int i = 0; i < g->maps[g->curr_map].num_destrutivas; i++){
                        if(CheckCollisionRecs(g->maps[g->curr_map].destrutivas[i], verify_bomb)){
                            int aux = rand() % 4 + 1;
                            if(aux == 1){
                                g->maps[g->curr_map].velocidade[i] = (Rectangle) {g->maps[g->curr_map].destrutivas[i].x, g->maps[g->curr_map].destrutivas[i].y, 20, 20};
                            } else if(aux == 2){
                                g->maps[g->curr_map].explosao[i] = (Rectangle) {g->maps[g->curr_map].destrutivas[i].x, g->maps[g->curr_map].destrutivas[i].y, 20, 20};
                            } else if(aux == 3){
                                g->maps[g->curr_map].num_bombas[i] = (Rectangle) {g->maps[g->curr_map].destrutivas[i].x, g->maps[g->curr_map].destrutivas[i].y, 20, 20};
                            }
                            g->maps[g->curr_map].destrutivas[i] = (Rectangle) {0,0,0,0};     
                        }
                    }
                    if(!barrier_collision(actual_map, verify_bomb)){
                        g->hero.bombs[i].explosion_up.height += grow_tax;
                    } 
                    if(CheckCollisionRecs(g->hero.pos,rectangle_bomb) && CheckCollisionRecs(g->hero2.pos, rectangle_bomb)){
                        g->hero.isAlive = 0;
                        g->hero2.isAlive = 0;
                        break;
                    }

                    if(CheckCollisionRecs(g->hero.pos,rectangle_bomb)){
                        g->hero2.vitorias += 1;
                        g->hero.isAlive = 0;
                        break;
                    } 
                    if(CheckCollisionRecs(g->hero2.pos,rectangle_bomb )){
                        g->hero.vitorias += 1;
                        g->hero2.isAlive = 0;
                        break;
                    } 
                    for(int j = 0 ; j< g->hero.num_bombs;j++){
                        if(CheckCollisionRecs(g->hero.bombs[j].pos,rectangle_bomb) && i!= j && g->hero.bombs[j].isActive == 1){
                            if(fabs(g->hero.bombs[j].time - GetTime()<=3)){
                                g->hero.bombs[j].time = GetTime() -3;                          
                            }
                        }
                    }
                    for(int j = 0; j<g->hero.num_bombs;j++){
                        if(CheckCollisionRecs(g->hero2.bombs[j].pos,rectangle_bomb) ){
                            if(fabs(g->hero2.bombs[j].time - GetTime()<=3)){
                               g->hero2.bombs[j].time = GetTime() -3;          
                            } 
                        }
                            
                    }
                }
                if(g->hero.bombs[i].explosion_down.height < g->hero.bombs[i].distance * STD_SIZE_Y){
                    rectangle_bomb = g->hero.bombs[i].explosion_down;
                    Rectangle verify_bomb = (Rectangle){ rectangle_bomb.x, rectangle_bomb.y - grow_tax, rectangle_bomb.width, rectangle_bomb.height + grow_tax};
                    for(int i = 0; i < g->maps[g->curr_map].num_destrutivas; i++){
                        if(CheckCollisionRecs(g->maps[g->curr_map].destrutivas[i], verify_bomb)){
                            int aux = rand() % 4 + 1;
                            if(aux == 1){
                                g->maps[g->curr_map].velocidade[i] = (Rectangle) {g->maps[g->curr_map].destrutivas[i].x, g->maps[g->curr_map].destrutivas[i].y, 20, 20};
                            } else if(aux == 2){
                                g->maps[g->curr_map].explosao[i] = (Rectangle) {g->maps[g->curr_map].destrutivas[i].x, g->maps[g->curr_map].destrutivas[i].y, 20, 20};
                            } else if(aux == 3){
                                g->maps[g->curr_map].num_bombas[i] = (Rectangle) {g->maps[g->curr_map].destrutivas[i].x, g->maps[g->curr_map].destrutivas[i].y, 20, 20};
                            }
                            g->maps[g->curr_map].destrutivas[i] = (Rectangle) {0,0,0,0};     
                        }
                    }
                    if(!barrier_collision(actual_map, verify_bomb)){
                        g->hero.bombs[i].explosion_down.height += grow_tax;
                        g->hero.bombs[i].explosion_down.y -= grow_tax;
                    } 
                    if(CheckCollisionRecs(g->hero.pos,rectangle_bomb) && CheckCollisionRecs(g->hero2.pos, rectangle_bomb)){
                        g->hero.isAlive = 0;
                        g->hero2.isAlive = 0;
                        break;
                    }

                    if(CheckCollisionRecs(g->hero.pos,rectangle_bomb)){
                        g->hero2.vitorias += 1;
                        g->hero.isAlive = 0;
                        break;
                    } 
                    if(CheckCollisionRecs(g->hero2.pos,rectangle_bomb )){
                        g->hero.vitorias += 1;
                        g->hero2.isAlive = 0;
                        break;
                    } 
                    for(int j = 0 ; j< g->hero.num_bombs;j++){
                        if(CheckCollisionRecs(g->hero.bombs[j].pos,rectangle_bomb) && i!= j){       
                            if(fabs(g->hero.bombs[j].time - GetTime()<=3)){
                                g->hero.bombs[j].time = GetTime() -3;   
                            } 
                        }   
                    }
                    for(int j = 0; j<g->hero.num_bombs;j++){
                        if(CheckCollisionRecs(g->hero2.bombs[j].pos,rectangle_bomb)){    
                            if(fabs(g->hero2.bombs[j].time - GetTime()<=3)){
                               g->hero2.bombs[j].time = GetTime() -3;          
                            } 
                        }    
                    }
                }
            }else if(fabs(g->hero.bombs[i].time - GetTime()) > 3){
                /* Reseta as informações da bomba */
                g->hero.bombs[i].isActive = 0;
                g->hero.bombs[i].dono_saiu = 0;
                g->hero.bombs[i].adversario_saiu = 0;
            }
        }
    }
    g->hero.put_bomb = 0;
    if(IsKeyPressed(KEY_KP_ENTER)){
        int aux = 0;
        for(int i = 0; i<g->hero2.num_bombs; i++){
            if(CheckCollisionRecs(g->hero2.bombs[i].pos, g->hero2.pos)){
                aux++;
            }
        }
        if(aux == 0){
            g->hero2.put_bomb = 1;
        }
    }
    if(g->hero2.put_bomb == 1){
        for(int i = 0; i < g->hero2.num_bombs; i++){
            if(g->hero2.bombs[i].isActive == 0){
                PlaySound(tnt_sound);
                g->hero2.bombs[i].isActive = 1;
                g->hero2.bombs[i].pos = (Rectangle) {g->hero2.pos.x, g->hero2.pos.y, STD_SIZE_BOMB_X, STD_SIZE_BOMB_Y};
                g->hero2.bombs[i].explosion_right = (Rectangle) {g->hero2.pos.x, g->hero2.pos.y, STD_SIZE_BOMB_X, STD_SIZE_BOMB_Y};
                g->hero2.bombs[i].explosion_left = (Rectangle) {g->hero2.pos.x, g->hero2.pos.y, STD_SIZE_BOMB_X, STD_SIZE_BOMB_Y};
                g->hero2.bombs[i].explosion_down = (Rectangle) {g->hero2.pos.x, g->hero2.pos.y, STD_SIZE_BOMB_X, STD_SIZE_BOMB_Y};
                g->hero2.bombs[i].explosion_up = (Rectangle) {g->hero2.pos.x, g->hero2.pos.y, STD_SIZE_BOMB_X, STD_SIZE_BOMB_Y};

                g->hero2.bombs[i].time = GetTime();
                break;
            }
        }
    }
    for(int i = 0; i < g->hero2.num_bombs; i++){
        if(g->hero2.bombs[i].isActive == 1){
            if(fabs(g->hero2.bombs[i].time - GetTime()) > 3 && fabs(g->hero2.bombs[i].time - GetTime()) < 5){
                Rectangle verify_bomb;
                Rectangle rectangle_bomb, rectangle_bomb2;
                int grow_tax = g->hero2.bombs[i].distance;
                if(g->hero2.bombs[i].explosion_right.width < g->hero2.bombs[i].distance * STD_SIZE_X){
                    rectangle_bomb = g->hero2.bombs[i].explosion_right;
                    Rectangle verify_bomb = (Rectangle){ rectangle_bomb.x, rectangle_bomb.y, rectangle_bomb.width + grow_tax,  rectangle_bomb.height};
                    for(int i = 0; i < g->maps[g->curr_map].num_destrutivas; i++){
                        if(CheckCollisionRecs(g->maps[g->curr_map].destrutivas[i], verify_bomb)){
                            int aux = rand() % 4 + 1;
                            if(aux == 1){
                                g->maps[g->curr_map].velocidade[i] = (Rectangle) {g->maps[g->curr_map].destrutivas[i].x, g->maps[g->curr_map].destrutivas[i].y, 20, 20};
                            } else if(aux == 2){
                                g->maps[g->curr_map].explosao[i] = (Rectangle) {g->maps[g->curr_map].destrutivas[i].x, g->maps[g->curr_map].destrutivas[i].y, 20, 20};
                            } else if(aux == 3){
                                g->maps[g->curr_map].num_bombas[i] = (Rectangle) {g->maps[g->curr_map].destrutivas[i].x, g->maps[g->curr_map].destrutivas[i].y, 20, 20};
                            }
                            g->maps[g->curr_map].destrutivas[i] = (Rectangle) {0,0,0,0};         
                        }
                    }
                    if(!barrier_collision(actual_map, verify_bomb)){
                        g->hero2.bombs[i].explosion_right.width += grow_tax;
                    } 
                    if(CheckCollisionRecs(g->hero.pos,rectangle_bomb) && CheckCollisionRecs(g->hero2.pos, rectangle_bomb)){
                        g->hero.isAlive = 0;
                        g->hero2.isAlive = 0;
                        break;
                    }

                    if(CheckCollisionRecs(g->hero.pos,rectangle_bomb)){
                        g->hero2.vitorias += 1;
                        g->hero.isAlive = 0;
                        break;
                    } 
                    if(CheckCollisionRecs(g->hero2.pos,rectangle_bomb )){
                        g->hero.vitorias += 1;
                        g->hero2.isAlive = 0;
                        break;
                    } 
                    for(int j = 0 ; j< g->hero2.num_bombs;j++){
                        if(CheckCollisionRecs(g->hero2.bombs[j].pos,rectangle_bomb) && i!= j ){
                            if(fabs(g->hero2.bombs[j].time - GetTime()<=3)){
                                g->hero2.bombs[j].time = GetTime() -3;    
                            } 
                        }
                    }
                    for(int j = 0; j<g->hero2.num_bombs;j++){
                        if(CheckCollisionRecs(g->hero.bombs[j].pos,rectangle_bomb) ){    
                            if(fabs(g->hero.bombs[j].time - GetTime()<=3)){
                                g->hero.bombs[j].time = GetTime() -3;
                                  
                            } 
                        }
                    }
                }
                if(g->hero2.bombs[i].explosion_left.width < g->hero2.bombs[i].distance * STD_SIZE_X){
                    rectangle_bomb = g->hero2.bombs[i].explosion_left;
                    Rectangle verify_bomb = (Rectangle){ rectangle_bomb.x - grow_tax, rectangle_bomb.y, rectangle_bomb.width + grow_tax,  rectangle_bomb.height};
                    for(int i = 0; i < g->maps[g->curr_map].num_destrutivas; i++){
                        if(CheckCollisionRecs(g->maps[g->curr_map].destrutivas[i], verify_bomb)){
                            int aux = rand() % 4 + 1;
                            if(aux == 1){
                                g->maps[g->curr_map].velocidade[i] = (Rectangle) {g->maps[g->curr_map].destrutivas[i].x, g->maps[g->curr_map].destrutivas[i].y, 20, 20};
                            } else if(aux == 2){
                                g->maps[g->curr_map].explosao[i] = (Rectangle) {g->maps[g->curr_map].destrutivas[i].x, g->maps[g->curr_map].destrutivas[i].y, 20, 20};
                            } else if(aux == 3){
                                g->maps[g->curr_map].num_bombas[i] = (Rectangle) {g->maps[g->curr_map].destrutivas[i].x, g->maps[g->curr_map].destrutivas[i].y, 20, 20};
                            }
                            g->maps[g->curr_map].destrutivas[i] = (Rectangle) {0,0,0,0};     
                        }
                    }
                    if(!barrier_collision(actual_map, verify_bomb)){
                        g->hero2.bombs[i].explosion_left.width += grow_tax;
                        g->hero2.bombs[i].explosion_left.x -= grow_tax;
                    } 
                    if(CheckCollisionRecs(g->hero.pos,rectangle_bomb) && CheckCollisionRecs(g->hero2.pos, rectangle_bomb)){
                        g->hero.isAlive = 0;
                        g->hero2.isAlive = 0;
                        break;
                    }

                    if(CheckCollisionRecs(g->hero.pos,rectangle_bomb)){
                        g->hero2.vitorias += 1;
                        g->hero.isAlive = 0;
                        break;
                    } 
                    if(CheckCollisionRecs(g->hero2.pos,rectangle_bomb )){
                        g->hero.vitorias += 1;
                        g->hero2.isAlive = 0;
                        break;
                    } 
                    for(int j = 0 ; j< g->hero2.num_bombs;j++){
                        if(CheckCollisionRecs(g->hero2.bombs[j].pos,rectangle_bomb) && i!= j ){       
                            if(fabs(g->hero2.bombs[j].time - GetTime()<=3)){
                                g->hero2.bombs[j].time = GetTime() -3;
                            } 
                        } 
                    }
                    for(int j = 0; j<g->hero2.num_bombs;j++){
                         if(CheckCollisionRecs(g->hero.bombs[j].pos,rectangle_bomb) ){
                            if(fabs(g->hero.bombs[j].time - GetTime()<=3)){
                                g->hero.bombs[j].time = GetTime() -3;  
                            } 
                        }
                    }
                }
                if(g->hero2.bombs[i].explosion_up.height < g->hero2.bombs[i].distance * STD_SIZE_Y){
                    rectangle_bomb = g->hero2.bombs[i].explosion_up;
                    Rectangle verify_bomb = (Rectangle){ rectangle_bomb.x, rectangle_bomb.y, rectangle_bomb.width, rectangle_bomb.height + grow_tax};
                    for(int i = 0; i < g->maps[g->curr_map].num_destrutivas; i++){
                        if(CheckCollisionRecs(g->maps[g->curr_map].destrutivas[i], verify_bomb)){
                            int aux = rand() % 4 + 1;
                            if(aux == 1){
                                g->maps[g->curr_map].velocidade[i] = (Rectangle) {g->maps[g->curr_map].destrutivas[i].x, g->maps[g->curr_map].destrutivas[i].y, 20, 20};
                            } else if(aux == 2){
                                g->maps[g->curr_map].explosao[i] = (Rectangle) {g->maps[g->curr_map].destrutivas[i].x, g->maps[g->curr_map].destrutivas[i].y, 20, 20};
                            } else if(aux == 3){
                                g->maps[g->curr_map].num_bombas[i] = (Rectangle) {g->maps[g->curr_map].destrutivas[i].x, g->maps[g->curr_map].destrutivas[i].y, 20, 20};
                            }
                            g->maps[g->curr_map].destrutivas[i] = (Rectangle) {0,0,0,0};     
                        }
                    }
                    if(!barrier_collision(actual_map, verify_bomb)){
                        g->hero2.bombs[i].explosion_up.height += grow_tax;
                    } 
                    if(CheckCollisionRecs(g->hero.pos,rectangle_bomb) && CheckCollisionRecs(g->hero2.pos, rectangle_bomb)){
                        g->hero.isAlive = 0;
                        g->hero2.isAlive = 0;
                        break;
                    }

                    if(CheckCollisionRecs(g->hero.pos,rectangle_bomb)){
                        g->hero2.vitorias += 1;
                        g->hero.isAlive = 0;
                        break;
                    } 
                    if(CheckCollisionRecs(g->hero2.pos,rectangle_bomb )){
                        g->hero.vitorias += 1;
                        g->hero2.isAlive = 0;
                        break;
                    } 
                    for(int j = 0 ; j< g->hero2.num_bombs;j++){
                        if(CheckCollisionRecs(g->hero2.bombs[j].pos,rectangle_bomb) && i!= j ){       
                            if(fabs(g->hero2.bombs[j].time - GetTime()<=3)){
                                g->hero2.bombs[j].time = GetTime() -3;
                            } 
                        }
                    }
                    for(int j = 0; j<g->hero2.num_bombs;j++){
                        if(CheckCollisionRecs(g->hero.bombs[j].pos,rectangle_bomb) ){
                            if(fabs(g->hero.bombs[j].time - GetTime()<=3)){
                                g->hero.bombs[j].time = GetTime() -3;
                            } 
                        }
                    }
                }
                if(g->hero2.bombs[i].explosion_down.height < g->hero2.bombs[i].distance * STD_SIZE_Y){
                    rectangle_bomb = g->hero2.bombs[i].explosion_down;
                    Rectangle verify_bomb = (Rectangle){ rectangle_bomb.x, rectangle_bomb.y - grow_tax, rectangle_bomb.width, rectangle_bomb.height + grow_tax};
                    for(int i = 0; i < g->maps[g->curr_map].num_destrutivas; i++){
                        if(CheckCollisionRecs(g->maps[g->curr_map].destrutivas[i], verify_bomb)){
                            int aux = rand() % 4 + 1;
                            if(aux == 1){
                                g->maps[g->curr_map].velocidade[i] = (Rectangle) {g->maps[g->curr_map].destrutivas[i].x, g->maps[g->curr_map].destrutivas[i].y, 20, 20};
                            } else if(aux == 2){
                                g->maps[g->curr_map].explosao[i] = (Rectangle) {g->maps[g->curr_map].destrutivas[i].x, g->maps[g->curr_map].destrutivas[i].y, 20, 20};
                            } else if(aux == 3){
                                g->maps[g->curr_map].num_bombas[i] = (Rectangle) {g->maps[g->curr_map].destrutivas[i].x, g->maps[g->curr_map].destrutivas[i].y, 20, 20};
                            }
                            g->maps[g->curr_map].destrutivas[i] = (Rectangle) {0,0,0,0};     
                        }
                    }
                    if(!barrier_collision(actual_map, verify_bomb)){
                        g->hero2.bombs[i].explosion_down.height += grow_tax;
                        g->hero2.bombs[i].explosion_down.y -= grow_tax;
                    } 
                    if(CheckCollisionRecs(g->hero.pos,rectangle_bomb) && CheckCollisionRecs(g->hero2.pos, rectangle_bomb)){
                        g->hero.isAlive = 0;
                        g->hero2.isAlive = 0;
                        break;
                    }

                    if(CheckCollisionRecs(g->hero.pos,rectangle_bomb)){
                        g->hero2.vitorias += 1;
                        g->hero.isAlive = 0;
                        break;
                    } 
                    if(CheckCollisionRecs(g->hero2.pos,rectangle_bomb )){
                        g->hero.vitorias += 1;
                        g->hero2.isAlive = 0;
                        break;
                    } 
                    for(int j = 0 ; j< g->hero2.num_bombs;j++){
                        if(CheckCollisionRecs(g->hero2.bombs[j].pos,rectangle_bomb) && i!= j ){       
                            if(fabs(g->hero2.bombs[j].time - GetTime()<=3)){
                                g->hero2.bombs[j].time = GetTime() -3;
                            } 
                        }
                    }
                    for(int j = 0; j<g->hero2.num_bombs;j++){
                        if(CheckCollisionRecs(g->hero.bombs[j].pos,rectangle_bomb) ){    
                            if(fabs(g->hero.bombs[j].time - GetTime()<=3)){
                                g->hero.bombs[j].time = GetTime() -3;
                            } 
                        }
                    }
                }
            }else if(fabs(g->hero2.bombs[i].time - GetTime()) > 3){
                /* Reseta as informações da bomba */
                g->hero2.bombs[i].isActive = 0;
                g->hero2.bombs[i].dono_saiu = 0;
                g->hero2.bombs[i].adversario_saiu = 0;
            }
        }
    }
    g->hero2.put_bomb = 0;
}

void checkMap2Specials(Game *g){
    /* Para o Hero 1 */
    if(CheckCollisionRecs(g->hero.pos, g->maps[1].slow_speed[0]) || CheckCollisionRecs(g->hero.pos, g->maps[1].slow_speed[1])){
        g->hero.speed = 1;  
    } else {    
        g->hero.speed = g->hero.aux_speed;
    }
    if(CheckCollisionRecs(g->hero.pos, g->maps[1].insta_death[0]) || CheckCollisionRecs(g->hero.pos, g->maps[1].insta_death[1])){
        Sound morte = LoadSound("sprites/audio/minecraft-death-sound.mp3");
        PlaySound(morte);
        g->hero2.vitorias += 1;
        g->hero.isAlive = 0;
    }
    /* Para o Hero 2 */
    if(CheckCollisionRecs(g->hero2.pos, g->maps[1].slow_speed[0]) || CheckCollisionRecs(g->hero2.pos, g->maps[1].slow_speed[1])){
        g->hero2.speed = 1;
    } else{
        g->hero2.speed = g->hero2.aux_speed;
    }
    if(CheckCollisionRecs(g->hero2.pos, g->maps[1].insta_death[0]) || CheckCollisionRecs(g->hero2.pos, g->maps[1].insta_death[1])){
        Sound morte = LoadSound("sprites/audio/minecraft-death-sound.mp3");
        PlaySound(morte);
        g->hero.vitorias += 1;
        g->hero2.isAlive = 0;
    }
}

void checkMap1Specials(Game *g){
    int teleporte_aleatoriox = rand() % 12;
    int teleporte_som = rand() % 2;
    /* Para o Hero 1 */
    if(CheckCollisionRecs(g->hero.pos, g->maps[0].teleport[0])){
        Sound teleporte;
        if(teleporte_som == 0){
            teleporte = LoadSound("sprites/audio/teleport1.mp3");
        } else {
            teleporte = LoadSound("sprites/audio/teleport2.mp3");
        }
        PlaySound(teleporte);
        int teleporte_aleatorioy = rand() % 6;
        g->hero.pos.x = teleporte_aleatoriox * 30 + 180;
        g->hero.pos.y = teleporte_aleatorioy * 60 + 120;
    }
    if(CheckCollisionRecs(g->hero.pos, g->maps[0].teleport[1])){
        Sound teleporte;
        if(teleporte_som == 0){
            teleporte = LoadSound("sprites/audio/teleport1.mp3");
        } else {
            teleporte = LoadSound("sprites/audio/teleport2.mp3");
        }
        PlaySound(teleporte);
        int teleporte_aleatorioy = rand() % 6;
        g->hero.pos.x = teleporte_aleatoriox * 30 + 180;
        g->hero.pos.y = teleporte_aleatorioy * 60 + 120;
    }
    /* Para o Hero 2 */
    if(CheckCollisionRecs(g->hero2.pos, g->maps[0].teleport[0])){
        Sound teleporte;
        if(teleporte_som == 0){
            teleporte = LoadSound("sprites/audio/teleport1.mp3");
        } else {
            teleporte = LoadSound("sprites/audio/teleport2.mp3");
        }
        PlaySound(teleporte);
        int teleporte_aleatorioy = rand() % 6;
        g->hero2.pos.x = teleporte_aleatoriox * 30 + 180;
        g->hero2.pos.y = teleporte_aleatorioy * 60 + 120;
    }
    if(CheckCollisionRecs(g->hero2.pos, g->maps[0].teleport[1])){
        Sound teleporte;
        if(teleporte_som == 0){
            teleporte = LoadSound("sprites/audio/teleport1.mp3");
        } else {
            teleporte = LoadSound("sprites/audio/teleport2.mp3");
        }
        PlaySound(teleporte);
        int teleporte_aleatorioy = rand() % 6;
        g->hero2.pos.x = teleporte_aleatoriox * 30 + 180;
        g->hero2.pos.y = teleporte_aleatorioy * 60 + 120;
    }
}

void UpdateGame(Game *g, Sound tnt_sound)
{
    update_hero_pos(g);
    update_bomb(g, tnt_sound);
    if(g->curr_map == 1){
        if(g->hero.speed != 1)
            g->hero.aux_speed = g->hero.speed;
        if(g->hero2.speed != 1)
            g->hero2.aux_speed = g->hero2.speed;
        checkMap2Specials(g);
    }else{
        checkMap1Specials(g);
    }
    

    Map *map = &g->maps[g->curr_map];
}
