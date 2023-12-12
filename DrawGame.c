#include "raylib.h"
#include <string.h>
#include <stdio.h>

#include "DrawGame.h"
#include "UpdateGame.h"
#include "InitGame.h"

void drawPlayers(Game *g){
    /* Desenha os dois jogadores */
    DrawTexture(g->player1_texture, g->hero.pos.x, g->hero.pos.y, RAYWHITE);
    DrawTexture(g->player2_texture, g->hero2.pos.x, g->hero2.pos.y, RAYWHITE);
}

void draw_map(Game *g){
    Map *map = &g->maps[g->curr_map];
    /* Desenha a tela de jogo do mapa */
    if(g->curr_map == 1){
        DrawTexture(g->telaJogoMapa1, 0, 0, RAYWHITE);
    } 
    if(g->curr_map == 0){
        DrawTexture(g->telaJogoMapa0, 0, 0, RAYWHITE);
    }

    /* Desenha os blocos destrutíveis relativos a cada mapa */
    for(int i = 0; i < map->num_barriers; i++){
        int posx = map->barriers[i].x;
        int posy = map->barriers[i].y;
        if(g->curr_map == 1){
            DrawTexture(g->obsidian, posx, posy, RAYWHITE);
        }else{ 
            DrawTexture(g->bedrock, posx, posy, RAYWHITE);
        }
    }
    /* Desenha os sprites dos blocos destrutíveis */
    for(int i = 0; i < map->num_destrutivas; i++){
        int posx = map->destrutivas[i].x;
        int posy = map->destrutivas[i].y;
        DrawTexture(g->bau, posx, posy, RAYWHITE);
    }
    /* Desenha os sprites dos poderes */
    for(int i = 0; i < map->num_poder; i++){
        int posx_boots = map->velocidade[i].x;
        int posy_boots = map->velocidade[i].y;
        DrawTexture(g->boots, posx_boots, posy_boots, RAYWHITE);
        
        DrawRectangleRec(map->explosao[i], ORANGE);
        int posx_explosao = map->explosao[i].x;
        int posy_explosao = map->explosao[i].y;
        DrawTexture(g->boost_bomba, posx_explosao, posy_explosao, BLUE);

        DrawRectangleRec(map->num_bombas[i], PURPLE);
        int posx_nbombas = map->num_bombas[i].x;
        int posy_nbombas = map->num_bombas[i].y;
        DrawTexture(g->boost_bomba, posx_nbombas, posy_nbombas, RED);
    }
}

void draw_bomb(Game *g){
    for(int i = 0; i < g->hero.num_bombs; i++){
        if(g->hero.bombs[i].isActive == 1){
            DrawRectangleRec(g->hero.bombs[i].explosion_right, RED);
            DrawRectangleRec(g->hero.bombs[i].explosion_left, RED);
            DrawRectangleRec(g->hero.bombs[i].explosion_up, RED);
            DrawRectangleRec(g->hero.bombs[i].explosion_down, RED);

            /* Desenha o sprite da bomba */
            DrawTexture(g->tnt, g->hero.bombs[i].pos.x, g->hero.bombs[i].pos.y, RAYWHITE);
        }
    }
    for(int i = 0; i < g->hero2.num_bombs; i++){
        if(g->hero2.bombs[i].isActive == 1){
            DrawRectangleRec(g->hero2.bombs[i].explosion_right, RED);
            DrawRectangleRec(g->hero2.bombs[i].explosion_left, RED);
            DrawRectangleRec(g->hero2.bombs[i].explosion_up, RED);
            DrawRectangleRec(g->hero2.bombs[i].explosion_down, RED);

            /* Desenha o sprite da bomba */
            DrawTexture(g->tnt, g->hero2.bombs[i].pos.x, g->hero2.bombs[i].pos.y, RAYWHITE);
        }
    }
}

void DrawGame(Game *g)
{
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawRectangle(0, 0, g->screenWidth, g->screenHeight, GRAY);
    draw_map(g);
    draw_bomb(g);
    drawPlayers(g); 

    /* Responsável por desenhar a contagem regressiva na tela */
    char contagem_regressiva[20];
    snprintf(contagem_regressiva, 20, "%02d s", g->countdown_value);
    DrawText(contagem_regressiva, g->screenWidth/2 - MeasureText(contagem_regressiva, 50)/2, 20, 50, WHITE);

    EndDrawing();
}
