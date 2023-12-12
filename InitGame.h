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
    /* Identificam se os jogadores estão fora da posição em que a bomba foi implementada para ativar a colisão entre os jogadores e a bomba */
    int dono_saiu;
    int adversario_saiu;

    double distance;
    int time;
}Bomb;

/**
 * Cria o tipo Hero para os players do jogo 
*/
typedef struct Hero {
    Rectangle pos;
    Color color;
    char nick[20]; //Nome do jogador
    int isAlive; //Identifica se o jogador está vivo ou não
    int vitorias; //Contabiliza o número de vitórias do jogador 
    int speed;
    int aux_speed;
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
    Rectangle teleport[2];
    
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
    int curr_map; //Identifica em qual mapa o jogo está sendo jogado no momento 
    int curr_player;
    Hero hero, hero2; //Cria os dois players
    int screenWidth;
    int screenHeight;
    int gameover; //Verifica se o jogo deve ser encerrado 
    int aux;
    int start; //Detecta se o jogo deve começar 
    int countdown_value; //Representa o valor da contagem regressiva ao longo do tempo 

    /* Texturas utilizadas no jogo */
    Texture2D backgroundImage; //Tela inicial do jogo
    Texture2D mapa1; //Botão para selecionar o mapa 1
    Texture2D mapa2; //Botão para selecionar o mapa 2
    Texture2D backgroundRevanche; //Tela de revanche
    Texture2D telaJogoMapa1; //Tela do mapa 2
    Texture2D telaJogoMapa0; //Tela do mapa 1 
    Texture2D obsidian; //Barreira indestrutível do mapa 2
    Texture2D bau; //Itens destrutíveis
    Texture2D bedrock; //Barreira indestrutível do mapa 1
    Texture2D boost_bomba; //Textura dos superpoderes relacionados a bombas
    Texture2D boots; //Superpoder de patins
    Texture2D player1_texture; //Textura do jogador 1
    Texture2D player2_texture; //Textura do jogador 2
    Texture2D tnt; //Textura da bomba 
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
