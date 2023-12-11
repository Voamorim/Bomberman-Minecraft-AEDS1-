#ifndef DRAWGAME_H
#define DRAWGAME_H


#include "raylib.h"
#include "InitGame.h"

/**
 * Desenha os sprites dos dois jogadores
*/
void drawPlayers(Game *g);

/**
 * Desenha os sprites do mapa
*/
void draw_map(Game *g);

/**
 * Desenha os sprites e o raio de explosão da bomba
*/
void draw_bomb(Game *g);

/**
 * Desenha o jogo como um todo
*/
void DrawGame(Game *g);

#endif