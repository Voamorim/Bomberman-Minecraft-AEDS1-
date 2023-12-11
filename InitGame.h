#ifndef INITGAME_H
#define INITGAME_H

#include "raylib.h"

/**
 * Cria o tipo Power para os poderes adicionais do jogo
*/
typedef struct Power{
    int velocidade;
    int aumentar_bomba;
    int explosao_bomba;
}Power;

/**
 * Cria o tipo Bomb
*/
typedef struct Bomb{
    Rectangle pos; 
    Rectangle explosion_right;
    Rectangle explosion_left;
    Rectangle explosion_up; 
    Rectangle explosion_down; 
    int isActive;
    int dono_saiu;
    double distance;
    int time;
}Bomb;

/**
 * Cria o tipo Hero para os players do jogo 
*/
typedef struct Hero {
    Rectangle pos;
    Color color;
    char nick[20];
    int isAlive;
    int vitorias;
    int speed;
    int special;
    int draw_bomb;
    int put_bomb;
    int num_bombs;
    Bomb bombs[100];
} Hero;


/**
 * Cria o tipo Map para os mapas do jogo
*/
typedef struct Map {
    Rectangle barriers[50];
    Rectangle borders[50];
    Rectangle destrutivas[100];
    Rectangle poder[100];
    Rectangle velocidade[100];
    Rectangle explosao[100];
    Rectangle num_bombas[100];
    Rectangle slow_speed[2];
    Rectangle insta_death[2];
    int num_poder;
    int num_destrutivas; 
    int num_borders;
    int num_barriers;
    Color color;
    int next_map;
    int prev_map;
} Map;

/**
 * Cria o tipo Game para as variáveis do jogo
*/
typedef struct Game { 
    Power power;
    Map maps[2];
    int num_maps;
    int curr_map;
    int curr_player;
    Hero hero, hero2;
    int screenWidth;
    int screenHeight;
    int gameover;
    int aux;
    int start;
    int countdown_value;

    Texture2D backgroundImage;
    Texture2D mapa1;
    Texture2D mapa2;
    Texture2D backgroundRevanche;
    Texture2D telaJogoMapa1;
    Texture2D telaJogoMapa0;
    Texture2D obsidian;
    Texture2D bau;
    Texture2D bedrock;
    Texture2D boost_bomba;
    Texture2D boots;
    Texture2D player1_texture;
    Texture2D player2_texture;
    Texture2D tnt;
} Game;
/**
 * Inicializaa as variáveis do tipo Game
*/
void InitGame(Game *g);

/**
 * Define o layout dos mapas do jogo
*/
void map0_setup(Game *g);

/**
 * Inicializa as variáveis do tipo Game para uma revanche
*/
void initRevanche(Game *g);

/**
 * Inicializa a tela inicial do jogo
*/
void initial_screen(Game *g, int *aux, Sound click);

#endif
#endif
