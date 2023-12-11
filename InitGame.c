#include "InitGame.h"

#include "raylib.h"
#include <string.h>

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
#define COUNTDOWN_DURATION 120 //duração máxima do jogo

void InitGame(Game *g){
    g->curr_map = 0;
    g->num_maps = 2;
    g->hero.pos = (Rectangle) { 180, 120, 20, 20};
    g->hero.color = BLACK;
    g->hero.speed = 3;
    g->hero.special = 0;
    g->gameover = 0;
    g->hero.num_bombs = 1;
    g->hero.put_bomb = 0; 
    g->hero.draw_bomb = 0;
    g->aux = 0;

    if(g->curr_map == 1){
        g->maps[1].slow_speed[0] = (Rectangle) {330, 300, 90, 30};
        g->maps[1].slow_speed[1] = (Rectangle) {539, 150, 30, 90};
        g->maps[1].insta_death[0] = (Rectangle) {299, 210, 30, 30};
        g->maps[1].insta_death[1] = (Rectangle) {359, 390, 30, 30};
    }

    g->hero.isAlive = 1;
    g->hero2.isAlive = 1;
    g->hero.vitorias = 0;
    g->hero2.vitorias = 0;
    
    g->hero2.pos = (Rectangle) { 549, 490, 20, 20};
    g->hero2.color = PINK;
    g->hero2.speed = 3;
    g->hero2.special = 0;
    g->hero2.num_bombs = 1;
    g->hero2.put_bomb = 0;
    g->hero2.draw_bomb = 0;

    for(int i = 0; i < 20; i++){
        g->hero.nick[i] = '\0';
        g->hero2.nick[i] = '\0';
    }

    g->backgroundImage = LoadTexture("sprites/images/tela_de_inicio.png");
    g->mapa1 = LoadTexture("sprites/images/mapa1.png");
    g->mapa2 = LoadTexture("sprites/images/mapa2.png");
    g->backgroundRevanche = LoadTexture("sprites/images/tela_revanche.png");
    g->telaJogoMapa0 = LoadTexture("sprites/images/tela_jogo_mapa0.png");
    g->telaJogoMapa1 = LoadTexture("sprites/images/tela_jogo_mapa1.png");
    g->obsidian = LoadTexture("sprites/images/obsidian.png");
    g->bedrock = LoadTexture("sprites/images/bedrock.png");
    g->boost_bomba = LoadTexture("sprites/images/tntboost.png");
    g->boots = LoadTexture("sprites/images/boots.png");
    g->player1_texture = LoadTexture("sprites/images/mine.png");
    g->player2_texture = LoadTexture("sprites/images/creeper.png");
    g->bau = LoadTexture("sprites/images/chest.png");
    g->tnt = LoadTexture("sprites/images/tnt.png");
}

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

void initRevanche(Game *g){
    g->hero.pos = (Rectangle) { 180, 120, 20, 20};
    g->hero2.pos = (Rectangle) { 549, 490, 20, 20};
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
    if(g->curr_map == 1){
        g->maps[1].slow_speed[0] = (Rectangle) {330, 300, 90, 30};
        g->maps[1].slow_speed[1] = (Rectangle) {539, 150, 30, 90};
        g->maps[1].insta_death[0] = (Rectangle) {299, 210, 30, 30};
        g->maps[1].insta_death[1] = (Rectangle) {359, 390, 30, 30};
    }
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
