#ifndef UPDATEGAME_H
#define UPDATEGAME_H

#include "raylib.h"
#include "InitGame.h"

/**
 * Detecta se o jogador colidiu com alguma barreira
*/
int barrier_collision(Map *map, Rectangle target);

/**
 * Atualiza a posição dos dois jogadores
*/
void update_hero_pos(Game *g);

/**
 * Atualiza a bomba
*/
void update_bomb(Game *g, Sound tnt_sound);

void UpdateGame(Game *g, Sound tnt_sound);

#endif