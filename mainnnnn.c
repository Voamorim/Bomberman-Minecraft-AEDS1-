#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>

/**
 * Defines do jogo
*/
#define STD_SIZE_X 30 // largura do jogador
#define STD_SIZE_Y 30 // altura do jogador
#define STD_SIZE_BOMB_X 20 // largura da bomba
#define STD_SIZE_BOMB_X2 30
#define STD_SIZE_BOMB_Y 20 // altura da bomba
#define STD_SIZE_BOMB_Y2 30
#define SCREEN_BORDER 120 // tamanho da borda

typedef struct Power{
    int velocidade;
    int aumentar_bomba;
    int explosao_bomba;
}Power;

typedef struct Bomb{
    Rectangle pos; // posicionamento da bomba
    Rectangle explosion_right; // aumento da bomba para direita
    Rectangle explosion_left; //aumento da bomba para esquerda
    Rectangle explosion_up; // aumento da bomba para cima
    Rectangle explosion_down; // aumento da bomba para baixo
    int isActive;
    double distance;
    int time;
}Bomb;

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

typedef struct Map {
    Rectangle barriers[50];
    Rectangle borders[50];
    Rectangle destrutivas[100];
    Rectangle poder[100];
    Rectangle velocidade[100];
    Rectangle explosao[100];
    Rectangle num_bombas[100];
    int num_poder;
    int num_destrutivas; 
    int num_borders;
    int num_barriers;
    Color color;
    int next_map;
    int prev_map;
} Map;

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

    Texture2D backgroundImage;
    Texture2D mapa1;
    Texture2D mapa2;
    Texture2D backgroundRevanche;
    Texture2D telaJogoMapa1;
    Texture2D obsidian;
    Texture2D boost_bomba;
    Texture2D boots;
} Game;

/**
 * Funções modlares
*/
void InitGame(Game *g);         // Initialize game
void UpdateGame(Game *g);       // Update game (one frame)
void DrawGame(Game *g);         // Draw game (one frame)
void UpdateDrawFrame(Game *g);  // Update and Draw (one frame)

/**
 * Funções axiliares
*/
void draw_borders(Game *g);
void draw_map(Game *g);
void draw_bomb(Game *g);
void update_hero_pos(Game *g);
void update_bomb(Game *g);
void initial_screen(Game *g, int *aux, Sound click);

int barrier_collision(Map *m, Rectangle t);
void map0_setup(Game *g);

int checkP1bombsColl(Game *g);
int checkBombsColl(Game *g, Rectangle target, int playerID);
int checkP2bombsColl(Game *g);

void initRevanche(Game *g);

/**
 * Main do programa
*/
int main(void)
{
    srand(time(NULL));
    Game game;
    game.screenWidth = 800;
    game.screenHeight = 600;

    InitWindow(game.screenWidth, game.screenHeight, "Bomberman Minecraft");
    SetTargetFPS(60);
    
    InitGame(&game);
    InitAudioDevice();

    Music game_music = LoadMusicStream("sounds/minecraft-bell.mp3");
    Sound click = LoadSound("sprites/click.mp3");
    Music initial_music = LoadMusicStream("sprites/initial_music.mp3");
    Sound win_sound = LoadSound("sprites/challenge_complete.mp3");
    
    PlayMusicStream(initial_music);
    int aux = 0;
    while(!game.start){
        UpdateMusicStream(initial_music);
        initial_screen(&game, &aux, click);
    }
    PauseMusicStream(initial_music);
    UnloadMusicStream(initial_music);

    map0_setup(&game);
    

    char vencedor2[40] = "VENCEU ";
    strcat(vencedor2, game.hero2.nick);
    strcat(vencedor2, "!");
    char vencedor1[40] = "VENCEU ";
    strcat(vencedor1, game.hero.nick);
    strcat(vencedor1, "!");

    char placar[10];
    char vitorias1c = (char) game.hero.vitorias;
    char vitorias2c = (char) game.hero2.vitorias;
    char vitorias1[2];
    char vitorias2[2];
    vitorias1[0] = vitorias1c;
    vitorias1[1] = '\0';
    vitorias2[0] = vitorias2c;
    vitorias2[1] = '\0';

    while(!game.gameover){
        while (game.start)    // Detect window close button or ESC key
        {
            PlayMusicStream(game_music);
            UpdateMusicStream(game_music);
            DrawTexture(game.telaJogoMapa1, 0, 0, RAYWHITE);
            UpdateDrawFrame(&game);
            if(!game.hero.isAlive || !game.hero2.isAlive){
                game.start = 0;
                PlaySound(win_sound);
                break;
            }
        }
        PauseMusicStream(game_music);




        char vitorias1c = '0' + game.hero.vitorias;
        char vitorias2c =  '0' + game.hero2.vitorias;
        vitorias1[0] = vitorias1c;
        vitorias1[1] = '\0';
        vitorias2[0] = vitorias2c;
        vitorias2[1] = '\0';
        for(int i = 0; i < 10; i++){
            placar[i] = '\0';
        }
        strcat(placar, vitorias1);
        strcat(placar, " x ");
        strcat(placar, vitorias2);





        while(!IsKeyDown(KEY_ENTER)){
            BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawTexture(game.backgroundRevanche, 0, 0, RAYWHITE);
            Rectangle gameover_button = {32, 430, 218, 52};
            Rectangle revenge_button = {558, 430, 218, 52};
            DrawText(placar, (GetScreenWidth()/2 - MeasureText("EMPATE", 80)/2), 217, 80, WHITE);

            if(game.hero.isAlive == 0 && game.hero2.isAlive == 0){
                DrawText("EMPATE!", (GetScreenWidth()/2 - MeasureText("EMPATE", 80)/2), 80, 60, WHITE);
            } else if(game.hero.isAlive == 0){
                DrawText(vencedor2, (GetScreenWidth()/2 - MeasureText(vencedor2, 80)/2), 80, 60, WHITE);
            } else{
                DrawText(vencedor1, (GetScreenWidth()/2 - MeasureText(vencedor1, 80)/2), 80, 60, WHITE);
            }
            if(IsKeyPressed(KEY_ESCAPE)){
                game.gameover = 1;
                break;
            }
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                if(CheckCollisionPointRec(GetMousePosition(), gameover_button)){
                    PlaySound(click);
                    game.gameover = 1;
                    break;
                }
                if(CheckCollisionPointRec(GetMousePosition(), revenge_button)){
                    PlaySound(click);
                    game.start = 1;
                    initRevanche(&game);
                    break;
                }
            }
            EndDrawing();
        }
    }
    return 0;
}
//------------------------------------------------------------------------------------
// Module Functions Definitions (local)
//------------------------------------------------------------------------------------

// Initialize game variables
void InitGame(Game *g){
    g->curr_map = 0;
    g->num_maps = 10;
    g->hero.pos = (Rectangle) { 180, 120, 20, 20};// alteracao do da posicao inicial do jogador 1 para o canto da tela
    g->hero.color = BLACK;
    g->hero.speed = 3;
    g->hero.special = 0;
    g->gameover = 0;
    g->hero.num_bombs = 1;
    g->hero.put_bomb = 0; // assim que inicia o jogo ele nao coloca uma bomba
    g->hero.draw_bomb = 0;
    g->aux = 0;

    g->hero.isAlive = 1;
    g->hero2.isAlive = 1;
    g->hero.vitorias = 0;
    g->hero2.vitorias = 0;
    
    g->hero2.pos = (Rectangle) { 300, 300, 20, 20};// alteracao do da posicao inicial do jogador 1 para o canto da tela
    g->hero2.color = PINK;
    g->hero2.speed = 3;
    g->hero2.special = 0;
    g->hero2.num_bombs = 1;
    g->hero2.put_bomb = 0; // assim que inicia o jogo ele nao coloca uma bomba
    g->hero2.draw_bomb = 0;

    for(int i = 0; i < 20; i++){
        g->hero.nick[i] = '\0';
        g->hero2.nick[i] = '\0';
    }

    g->backgroundImage = LoadTexture("sprites/tela_de_inicio.png");
    g->mapa1 = LoadTexture("sprites/mapa1.png");
    g->mapa2 = LoadTexture("sprites/mapa2.png");
    g->backgroundRevanche = LoadTexture("sprites/tela_revanche.png");
    g->telaJogoMapa1 = LoadTexture("sprites/tela_jogo_mapa1.png");
    g->obsidian = LoadTexture("sprites/obsidian.png");
    g->boost_bomba = LoadTexture("sprites/tntboost.png");
    g->boots = LoadTexture("sprites/boots.png");
    
}

void initRevanche(Game *g){
    g->hero.pos = (Rectangle) { 180, 120, 20, 20};
    g->hero2.pos = (Rectangle) { 300, 300, 20, 20};
    g->hero.speed = 3;
    g->hero2.speed = 3;
    g->hero.special = 0;
    g->hero2.special = 0;
    g->hero.num_bombs = 1;
    g->hero2.num_bombs = 1;
    g->hero.put_bomb = 0;
    g->hero2.put_bomb = 0;
    g->hero.draw_bomb = 0;
    g->hero2.draw_bomb = 0;
    g->hero.isAlive = 1;
    g->hero2.isAlive = 1;

    if(g->curr_map == 1){
        g->curr_map = 0;
    } else {
        g->curr_map = 1;
    }
}


// Update game (one frame)
void UpdateGame(Game *g)
{
    update_hero_pos(g);
    update_bomb(g);

    Map *map = &g->maps[g->curr_map];
}

/**
 * Desenha o jogo
*/
void DrawGame(Game *g)
{
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawRectangle(0, 0, g->screenWidth, g->screenHeight, GRAY);
    draw_borders(g);
    draw_map(g);
    draw_bomb(g);

    /* Aproveitar dessas funções para adicionar os sprites dos players */
    DrawRectangleRec(g->hero.pos, g->hero.color);
    DrawRectangleRec(g->hero2.pos, g->hero2.color);

    EndDrawing();
}

// Update and Draw (one frame)
void UpdateDrawFrame(Game *g)
{
    UpdateGame(g);
    DrawGame(g);
}


/**
 * Desenha as bordas do jogo
*/
void draw_borders(Game *g)
{
    //DrawRectangle(0, 0, SCREEN_BORDER, g->screenHeight, BLACK);
    //DrawRectangle(0, 0, g->screenWidth, SCREEN_BORDER, BLACK);
    //DrawRectangle(g->screenWidth-SCREEN_BORDER-110, 0, g->screenWidth, g->screenHeight, BLACK);
    //DrawRectangle(0, g->screenHeight-SCREEN_BORDER+30, g->screenWidth, g->screenHeight, BLACK);
}

void initial_screen(Game *g, int *aux, Sound click){
    BeginDrawing();
    ClearBackground(RAYWHITE);
    Rectangle start_button = {335, 459, 171, 62};
    Rectangle map1_button = {170, 185, 150, 58};
    Rectangle map2_button = {532, 185, 150, 58};        
        
    /* Começa o jogo caso o player aperte o botão start*/
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        if(CheckCollisionPointRec(GetMousePosition(), start_button)){
            PlaySound(click);
            g->start = 1;
        }
        if(CheckCollisionPointRec(GetMousePosition(), map1_button)){
            PlaySound(click);
            g->curr_map = 0;
        }
        if(CheckCollisionPointRec(GetMousePosition(), map2_button)){
            PlaySound(click);
            g->curr_map = 1;
        }
    }
    DrawTexture(g->backgroundImage, 0, 0, RAYWHITE);
    if(g->curr_map == 0){
        DrawTexture(g->mapa1, 170, 185, RAYWHITE);
        DrawTexture(g->mapa2, 532, 185, GRAY);
    } else if(g->curr_map == 1){
        DrawTexture(g->mapa1, 170, 185, GRAY);
        DrawTexture(g->mapa2, 532, 185, RAYWHITE);
    } 
    if(*aux != 2){
        if(IsKeyPressed(KEY_ENTER)){
            g->curr_player = (g->curr_player == 0) ? 1 : 0;
            *aux += 1;
        } else if (IsKeyPressed(KEY_BACKSPACE)){
            if(g->curr_player == 0 && strlen(g->hero.nick) > 0){
                g->hero.nick[strlen(g->hero.nick) - 1] = '\0';
            } else if (g->curr_player == 1 && strlen(g->hero2.nick) > 0){
                g->hero2.nick[strlen(g->hero2.nick) - 1] = '\0';
            }
        } else {
            int key = GetKeyPressed();
            if((key >= 32) && (key <= 125)){
                if(g->curr_player == 0 && strlen(g->hero.nick) < 20){
                    g->hero.nick[strlen(g->hero.nick)] = (char) key;
                } else if(g->curr_player == 1 && strlen(g->hero2.nick) < 20){
                    g->hero2.nick[strlen(g->hero2.nick)] = (char)key;
                }
            }
        }
    } else{
        DrawText(g->hero.nick, 130, 364, 30, WHITE);
        DrawText(g->hero2.nick, 597, 364, 30, WHITE);
    }
    if(*aux != 2){
        if(g->curr_player == 0)
            DrawText(TextFormat("%s_", g->hero.nick), 130, 364, 30, BLUE);
        else 
            DrawText(TextFormat("%s_", g->hero2.nick), 597, 364, 30, RED);
    }
    EndDrawing();
}

/**
 * Desenha as barreiras, objetos destrutíveis e itens de superpoderes do mapa
*/
void draw_map(Game *g){
    Map *map = &g->maps[g->curr_map];
    if(g->curr_map == 0){
        DrawTexture(g->telaJogoMapa1, 0, 0, RAYWHITE);
    }
    /* Colocar os sprites por cima desses itens */
    for(int i = 0; i < map->num_barriers; i++){
        DrawRectangleRec(map->barriers[i], BLACK);
        int posx = map->barriers[i].x;
        int posy = map->barriers[i].y;
        DrawTexture(g->obsidian, posx, posy, RAYWHITE);
    }
    
    for(int i = 0; i < map->num_destrutivas; i++){
        DrawRectangleRec(map->destrutivas[i], GREEN);
    }
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
        DrawTexture(g->boost_bomba, posx_nbombas, posy_nbombas, RAYWHITE);
    }
}

/**
 * Desenha a bomba e a sua área de explosão
*/
void draw_bomb(Game *g){
    for(int i = 0; i < g->hero.num_bombs; i++){
        if(g->hero.bombs[i].isActive == 1){
            /* Aproveitar dessa primeira função para colocar o srprite da bomba */
            DrawRectangleRec(g->hero.bombs[i].pos, RED);

            DrawRectangleRec(g->hero.bombs[i].explosion_right, RED);
            DrawRectangleRec(g->hero.bombs[i].explosion_left, RED);
            DrawRectangleRec(g->hero.bombs[i].explosion_up, RED);
            DrawRectangleRec(g->hero.bombs[i].explosion_down, RED);
        }
    }
    for(int i = 0; i < g->hero2.num_bombs; i++){
        if(g->hero2.bombs[i].isActive == 1){
            /* Aproveitar dessa primeira função para colocar o srprite da bomba */
            DrawRectangleRec(g->hero2.bombs[i].pos, RED);

            DrawRectangleRec(g->hero2.bombs[i].explosion_right, RED);
            DrawRectangleRec(g->hero2.bombs[i].explosion_left, RED);
            DrawRectangleRec(g->hero2.bombs[i].explosion_up, RED);
            DrawRectangleRec(g->hero2.bombs[i].explosion_down, RED);
        }
    }
}

void update_hero_pos(Game *g){

    Hero *h = &g->hero;
    Map *m = &g->maps[g->curr_map];
    Hero *h2 = &g->hero2;
    Map *m2 = &g->maps[g->curr_map];
    
    
    if(IsKeyDown(KEY_A)) {
        if(h->pos.x > SCREEN_BORDER)
            h->pos.x -= h->speed;
        if(barrier_collision(m, h->pos) || CheckCollisionRecs(h->pos, h2->pos))
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
    } else if(IsKeyDown(KEY_D)) {
        if(h->pos.x + h->pos.width < g->screenWidth - SCREEN_BORDER)
            h->pos.x += h->speed;
        if(barrier_collision(m, h->pos) || CheckCollisionRecs(h->pos, h2->pos)) 
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
        if(h->pos.y > SCREEN_BORDER)
            h->pos.y -= h->speed;
        if(barrier_collision(m, h->pos) || CheckCollisionRecs(h->pos, h2->pos)) 
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
        if(h->pos.y + h->pos.height < g->screenHeight+30 - SCREEN_BORDER)
            h->pos.y += h->speed;
        if(barrier_collision(m, h->pos) || CheckCollisionRecs(h->pos, h2->pos)) 
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
        if(h2->pos.x > SCREEN_BORDER)
            h2->pos.x -= h2->speed;
        if(barrier_collision(m2, h2->pos) || CheckCollisionRecs(h->pos, h2->pos)) 
            h2->pos.x += h2->speed;
        for(int i = 0; i < g->maps[g->curr_map].num_poder; i++){
            if((CheckCollisionRecs(g->hero2.pos,g->maps[g->curr_map].velocidade[i]))){
                g->hero.speed++;
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
        if(h2->pos.x + h2->pos.width < g->screenWidth - SCREEN_BORDER)
            h2->pos.x += h2->speed;
        if(barrier_collision(m2, h2->pos) || CheckCollisionRecs(h->pos, h2->pos)) 
            h2->pos.x -= h2->speed;
        for(int i = 0; i < g->maps[g->curr_map].num_poder; i++){   
            if((CheckCollisionRecs(g->hero2.pos,g->maps[g->curr_map].velocidade[i]))){
                g->hero.speed++;
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
        if(h2->pos.y > SCREEN_BORDER)
            h2->pos.y -= h2->speed;
        if(barrier_collision(m2, h2->pos) ||CheckCollisionRecs(h->pos, h2->pos)) 
            h2->pos.y += h2->speed;
        for(int i = 0; i < g->maps[g->curr_map].num_poder; i++){ 
            if((CheckCollisionRecs(g->hero2.pos,g->maps[g->curr_map].velocidade[i]))){
                g->hero.speed++;
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
        if(h2->pos.y + h2->pos.height < g->screenHeight - SCREEN_BORDER+30)
            h2->pos.y += h2->speed;
        if(barrier_collision(m2, h2->pos) || CheckCollisionRecs(h->pos, h2->pos))
            h2->pos.y -= h2->speed;
        for(int i = 0; i < g->maps[g->curr_map].num_poder; i++){
            if( (CheckCollisionRecs(g->hero2.pos,g->maps[g->curr_map].velocidade[i]))){
                g->hero.speed++;
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



void update_bomb(Game *g){
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
                g->hero.bombs[i].isActive = 0;
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
                g->hero2.bombs[i].isActive = 0;
            }
        }
    }
    g->hero2.put_bomb = 0;
}



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

// Maps Setup
void map0_setup(Game *g){
    g->maps[g->curr_map].num_borders = 10;
    g->maps[g->curr_map].borders[0] = (Rectangle) {0, 0, SCREEN_BORDER+60, g->screenHeight};
    g->maps[g->curr_map].borders[1] = (Rectangle) {0, 0, g->screenWidth, SCREEN_BORDER};
    g->maps[g->curr_map].borders[2] = (Rectangle) {0, g->screenHeight+30-SCREEN_BORDER+30, g->screenWidth, g->screenHeight};
    g->maps[g->curr_map].borders[3] = (Rectangle) {g->screenWidth-SCREEN_BORDER-110, 0, g->screenWidth, g->screenHeight};
    g->maps[g->curr_map].num_barriers = 100;
    g->maps[g->curr_map].num_destrutivas = 100;
    g->maps[g->curr_map].num_poder = 100;
   int posy = 150;
for (int i = 0; i < 6; i++) {
    int posx = 210;

    for (int j = 0; j < 6; j++) {
        g->maps[g->curr_map].barriers[i * 6 + j] = (Rectangle) { posx, posy, STD_SIZE_X, STD_SIZE_Y };

        if (j == 5) {
            // Bloco destrutivo no final da linha
            g->maps[g->curr_map].destrutivas[i * 6 + j] = (Rectangle) { 0, 0, 0, 0 };  // Substitua os valores pelos desejados
        } else {
            // Bloco indestrutivo
            g->maps[g->curr_map].destrutivas[i * 6 + j] = (Rectangle) { posx + 30, posy, STD_SIZE_X, STD_SIZE_Y };
        }

        posx += STD_SIZE_X + 30;  // Ajuste de espaçamento
    }

    posy += STD_SIZE_Y + 30;  // Ajuste de espaçamento
}
    g->maps[g->curr_map].color = GRAY;
    g->maps[g->curr_map].prev_map = -1;
    g->maps[g->curr_map].next_map = 1;
    for(int i = 0; i < g->hero.num_bombs; i++){
        g->hero.bombs[i].isActive = 0;
        g->hero.bombs[i].distance = 1;
    }
    for(int i = 0; i < g->hero2.num_bombs; i++){
        g->hero2.bombs[i].isActive = 0;
        g->hero2.bombs[i].distance = 1;
    }
}
