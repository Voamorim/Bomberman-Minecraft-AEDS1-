#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>

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
    int speed;
    int special;
    int draw_bomb;
    int put_bomb;
    int num_bombs;
    Bomb bombs[100];
} Hero;

typedef struct Map {
    Rectangle barriers[100];
    Rectangle borders[100];
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
    Map maps[10];
    int num_maps;
    int curr_map;
    int curr_player;
    Hero hero, hero2;
    int screenWidth;
    int screenHeight;
    int gameover;
    int aux;
    int start;
} Game;

/**
* Funções Modulares
**/
void InitGame(Game *g);         // Initialize game
void UpdateGame(Game *g);       // Update game (one frame)
void DrawGame(Game *g);         // Draw game (one frame)
void UpdateDrawFrame(Game *g);  // Update and Draw (one frame)

//------------------------------------------------------------------------------------
// Auxiliar Functions Declaration
//------------------------------------------------------------------------------------
void draw_borders(Game *g);
void draw_map(Game *g);
void draw_bomb(Game *g);
void update_hero_pos(Game *g);
void update_bomb(Game *g);

int barrier_collision(Map *m, Rectangle t);
void map0_setup(Game *g);

int checkP1bombsColl(Game *g);
int checkBombsColl(Game *g, Rectangle target, int playerID);
int checkP2bombsColl(Game *g);

/**
* Main do programa
**/
int main(void)
{
    srand(time(NULL));
    Game game;
    game.screenWidth = 800;
    game.screenHeight = 600;

    InitWindow(game.screenWidth, game.screenHeight, "Aedsinho's quest");
    SetTargetFPS(60);
    
    InitGame(&game);
    InitAudioDevice();

    Texture2D backgroundImage = LoadTexture("sprites/tela_de_inicio.png");
    Texture2D mapa1 = LoadTexture("sprites/mapa1.png");
    Texture2D mapa2 = LoadTexture("sprites/mapa2.png");
    Sound click = LoadSound("sprites/click.mp3");
    Music initial_music = LoadMusicStream("sprites/initial_music.mp3");
    
    PlayMusicStream(initial_music);

    int aux = 0;
    for(int i = 0; i < 20; i++){
        game.hero.nick[i] = '\0';
        game.hero2.nick[i] = '\0';
    }
    while(!game.start){
        BeginDrawing();
        ClearBackground(RAYWHITE);
        UpdateMusicStream(initial_music);
        Rectangle start_button = {335, 459, 171, 62};
        Rectangle map1_button = {170, 185, 150, 58};
        Rectangle map2_button = {532, 185, 150, 58};        
        
        /* Começa o jogo caso o player aperte o botão start*/
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            if(CheckCollisionPointRec(GetMousePosition(), start_button)){
                PlaySound(click);
                game.start = 1;
            }
            if(CheckCollisionPointRec(GetMousePosition(), map1_button)){
                PlaySound(click);
                game.curr_map = 0;
            }
            if(CheckCollisionPointRec(GetMousePosition(), map2_button)){
                PlaySound(click);
                game.curr_map = 1;
            }
        }

        DrawTexture(backgroundImage, 0, 0, RAYWHITE);

        if(game.curr_map == 0){
            DrawTexture(mapa1, 170, 185, RAYWHITE);
            DrawTexture(mapa2, 532, 185, GRAY);
        } else if(game.curr_map == 1){
            DrawTexture(mapa1, 170, 185, GRAY);
            DrawTexture(mapa2, 532, 185, RAYWHITE);
        }
        
        if(aux!=2){
            if(IsKeyPressed(KEY_ENTER)){
                game.curr_player = (game.curr_player == 0) ? 1 : 0;
                aux++;
            } else if (IsKeyPressed(KEY_BACKSPACE)){
                if(game.curr_player == 0 && strlen(game.hero.nick) > 0){
                    game.hero.nick[strlen(game.hero.nick) - 1] = '\0';
                } else if (game.curr_player == 1 && strlen(game.hero2.nick) > 0){
                    game.hero2.nick[strlen(game.hero2.nick - 1)] = '\0';
                }
            } else {
                int key = GetKeyPressed();
                if((key >= 32) && (key <= 125)){
                    if(game.curr_player == 0 && strlen(game.hero.nick) < 20){
                        game.hero.nick[strlen(game.hero.nick)] = (char) key;
                    } else if(game.curr_player == 1 && strlen(game.hero2.nick) < 20){
                        game.hero2.nick[strlen(game.hero2.nick)] = (char)key;
                    }
                }
            }
        } else{
            DrawText(game.hero.nick, 130, 364, 30, WHITE);
            DrawText(game.hero2.nick, 597, 364, 30, WHITE);
        }

        if(aux!=2){
            if(game.curr_player == 0)
                DrawText(TextFormat("%s_", game.hero.nick), 130, 364, 30, BLUE);
            else 
                DrawText(TextFormat("%s_", game.hero2.nick), 597, 364, 30, RED);
        }
        EndDrawing();
    }
    


    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        UpdateDrawFrame(&game);
        if(game.gameover) break;
    }
    while(!IsKeyDown(KEY_ENTER)){
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("GAME OVER", GetScreenWidth()/2 - MeasureText("GAME OVER", 20)/2, GetScreenHeight()/2 - 50, 20, BLACK);
        EndDrawing();
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
    g->hero.pos = (Rectangle) { 120, 120, 20, 20};// alteracao do da posicao inicial do jogador 1 para o canto da tela
    g->hero.color = BLACK;
    g->hero.speed = 3;
    g->hero.special = 0;
    g->gameover = 0;
    g->hero.num_bombs = 1;
    g->hero.put_bomb = 0; // assim que inicia o jogo ele nao coloca uma bomba
    g->hero.draw_bomb = 0;
    g->aux = 0;
    
    g->hero2.pos = (Rectangle) { 300, 300, 20, 20};// alteracao do da posicao inicial do jogador 1 para o canto da tela
    g->hero2.color = PINK;
    g->hero2.speed = 3;
    g->hero2.special = 0;
    g->hero2.num_bombs = 1;
    g->hero2.put_bomb = 0; // assim que inicia o jogo ele nao coloca uma bomba
    g->hero2.draw_bomb = 0;
    map0_setup(g);
}


// Update game (one frame)
void UpdateGame(Game *g)
{
    update_hero_pos(g);
    update_bomb(g);

    Map *map = &g->maps[g->curr_map];
}

// Draw game (one frame)
void DrawGame(Game *g)
{
    BeginDrawing();

    ClearBackground(RAYWHITE);
    DrawRectangle(0, 0, g->screenWidth, g->screenHeight, GRAY);
    draw_borders(g);
    draw_map(g);
    draw_bomb(g);
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

void draw_borders(Game *g)
{
    DrawRectangle(0, 0, SCREEN_BORDER, g->screenHeight, BLACK);
    DrawRectangle(0, 0, g->screenWidth, SCREEN_BORDER, BLACK);
    DrawRectangle(g->screenWidth-SCREEN_BORDER-110, 0, g->screenWidth, g->screenHeight, BLACK);
    DrawRectangle(0, g->screenHeight-SCREEN_BORDER+30, g->screenWidth, g->screenHeight, BLACK);
}

/**
 * Desenha as barreiras, objetos destrutíveis e itens de superpoderes do mapa
*/
void draw_map(Game *g){
    Map *map = &g->maps[g->curr_map];
    
    /* Colocar os sprites por cima desses itens */
    for(int i = 0; i < map->num_barriers; i++){
        DrawRectangleRec(map->barriers[i], BLACK);
    }
    for(int i = 0; i < map->num_destrutivas; i++){
        DrawRectangleRec(map->destrutivas[i], GREEN);
    }
    for(int i = 0; i < map->num_poder; i++){
        DrawRectangleRec(map->velocidade[i], BLUE); 
        DrawRectangleRec(map->explosao[i], ORANGE);
        DrawRectangleRec(map->num_bombas[i], PURPLE);
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
         for ( int i = 0; i < g->maps[0].num_poder; i++)  {
            
      if( (CheckCollisionRecs(g->hero.pos,g->maps[0].velocidade[i]))){
        g->hero.speed++;
        g->maps[0].velocidade[i] = (Rectangle) {0, 0, 0,0};
      } else if ( (CheckCollisionRecs(g->hero.pos, g->maps[0].explosao[i]))){
        for(int j = 0; j < g->hero.num_bombs; j++){
        

         g->hero.bombs[j].distance +=0.5;
        
    }
        g->maps[0].explosao[i] = (Rectangle) {0, 0, 0,0};
      }else if ( (CheckCollisionRecs(g->hero.pos, g->maps[0].num_bombas[i]))){
         g->hero.num_bombs++;
          g->hero.bombs[g->hero.num_bombs-1].distance = g->hero.bombs[g->hero.num_bombs-2].distance;
         g->maps[0].num_bombas[i] = (Rectangle) {0, 0, 0,0};
      }
        }
}
    
        
        
     else if(IsKeyDown(KEY_D)) {
        if(h->pos.x + h->pos.width < g->screenWidth - SCREEN_BORDER)
            h->pos.x += h->speed;
        if(barrier_collision(m, h->pos) || CheckCollisionRecs(h->pos, h2->pos)) 
            h->pos.x -= h->speed;
         for ( int i = 0; i < g->maps[0].num_poder; i++)  {
            
      if( (CheckCollisionRecs(g->hero.pos,g->maps[0].velocidade[i]))){
        g->hero.speed++;
        g->maps[0].velocidade[i] = (Rectangle) {0, 0, 0,0};
      } else if ( (CheckCollisionRecs(g->hero.pos, g->maps[0].explosao[i]))){
        for(int j = 0; j < g->hero.num_bombs; j++){
        

         g->hero.bombs[j].distance +=0.5;
        
    }
        g->maps[0].explosao[i] = (Rectangle) {0, 0, 0,0};
      }else if ( (CheckCollisionRecs(g->hero.pos, g->maps[0].num_bombas[i]))){
         g->hero.num_bombs++;
          g->hero.bombs[g->hero.num_bombs-1].distance = g->hero.bombs[g->hero.num_bombs-2].distance;
         g->maps[0].num_bombas[i] = (Rectangle) {0, 0, 0,0};
      }
        }
        
    } else if(IsKeyDown(KEY_W)) {
        if(h->pos.y > SCREEN_BORDER)
            h->pos.y -= h->speed;
        if(barrier_collision(m, h->pos) || CheckCollisionRecs(h->pos, h2->pos)) 
            h->pos.y += h->speed;
         for ( int i = 0; i < g->maps[0].num_poder; i++)  {
            
      if( (CheckCollisionRecs(g->hero.pos,g->maps[0].velocidade[i]))){
        g->hero.speed++;
        g->maps[0].velocidade[i] = (Rectangle) {0, 0, 0,0};
      } else if ( (CheckCollisionRecs(g->hero.pos, g->maps[0].explosao[i]))){
        for(int j = 0; j < g->hero.num_bombs; j++){
        

         g->hero.bombs[j].distance +=0.5;
        
    }
        g->maps[0].explosao[i] = (Rectangle) {0, 0, 0,0};
      }else if ( (CheckCollisionRecs(g->hero.pos, g->maps[0].num_bombas[i]))){
         g->hero.num_bombs++;
          g->hero.bombs[g->hero.num_bombs-1].distance = g->hero.bombs[g->hero.num_bombs-2].distance;
         g->maps[0].num_bombas[i] = (Rectangle) {0, 0, 0,0};
      }
         }
      
        
    }  else if(IsKeyDown(KEY_S)) {
        if(h->pos.y + h->pos.height < g->screenHeight+30 - SCREEN_BORDER)
            h->pos.y += h->speed;
        if(barrier_collision(m, h->pos) || CheckCollisionRecs(h->pos, h2->pos)) 
            h->pos.y -= h->speed;
         for ( int i = 0; i < g->maps[0].num_poder; i++)  {
            
      if( (CheckCollisionRecs(g->hero.pos,g->maps[0].velocidade[i]))){
        g->hero.speed++;
        g->maps[0].velocidade[i] = (Rectangle) {0, 0, 0,0};
      } else if ( (CheckCollisionRecs(g->hero.pos, g->maps[0].explosao[i]))){
        for(int j = 0; j < g->hero.num_bombs; j++){
        

         g->hero.bombs[j].distance +=0.5;
        
    }
        g->maps[0].explosao[i] = (Rectangle) {0, 0, 0,0};
      }else if ( (CheckCollisionRecs(g->hero.pos, g->maps[0].num_bombas[i]))){
         g->hero.num_bombs++;
         g->hero.bombs[g->hero.num_bombs-1].distance = g->hero.bombs[g->hero.num_bombs-2].distance;
         g->maps[0].num_bombas[i] = (Rectangle) {0, 0, 0,0};
      }
        }
  
    
    }
    

    if(IsKeyDown(KEY_LEFT)) {
        if(h2->pos.x > SCREEN_BORDER)
            h2->pos.x -= h2->speed;
        if(barrier_collision(m2, h2->pos) || CheckCollisionRecs(h->pos, h2->pos)) 
            h2->pos.x += h2->speed;
         for ( int i = 0; i < g->maps[0].num_poder; i++)  {
            
      if( (CheckCollisionRecs(g->hero2.pos,g->maps[0].velocidade[i]))){
        g->hero.speed++;
        g->maps[0].velocidade[i] = (Rectangle) {0, 0, 0,0};
      } else if ( (CheckCollisionRecs(g->hero2.pos, g->maps[0].explosao[i]))){
        for(int j = 0; j < g->hero2.num_bombs; j++){
        

         g->hero2.bombs[j].distance +=0.5;
        
    }
        g->maps[0].explosao[i] = (Rectangle) {0, 0, 0,0};
      }else if ( (CheckCollisionRecs(g->hero2.pos, g->maps[0].num_bombas[i]))){
         g->hero2.num_bombs++;
          g->hero2.bombs[g->hero2.num_bombs-1].distance = g->hero2.bombs[g->hero2.num_bombs-2].distance;
         g->maps[0].num_bombas[i] = (Rectangle) {0, 0, 0,0};
      }
        }

    } else if(IsKeyDown(KEY_RIGHT)) {
        if(h2->pos.x + h2->pos.width < g->screenWidth - SCREEN_BORDER)
            h2->pos.x += h2->speed;
        if(barrier_collision(m2, h2->pos) || CheckCollisionRecs(h->pos, h2->pos)) 
            h2->pos.x -= h2->speed;
          for ( int i = 0; i < g->maps[0].num_poder; i++)  {
            
      if( (CheckCollisionRecs(g->hero2.pos,g->maps[0].velocidade[i]))){
        g->hero.speed++;
        g->maps[0].velocidade[i] = (Rectangle) {0, 0, 0,0};
      } else if ( (CheckCollisionRecs(g->hero2.pos, g->maps[0].explosao[i]))){
        for(int j = 0; j < g->hero2.num_bombs; j++){
        

         g->hero2.bombs[j].distance +=0.5;
        
    }
        g->maps[0].explosao[i] = (Rectangle) {0, 0, 0,0};
      }else if ( (CheckCollisionRecs(g->hero2.pos, g->maps[0].num_bombas[i]))){
         g->hero2.num_bombs++;
          g->hero2.bombs[g->hero2.num_bombs-1].distance = g->hero2.bombs[g->hero2.num_bombs-2].distance;
         g->maps[0].num_bombas[i] = (Rectangle) {0, 0, 0,0};
      }
        }

    } else if(IsKeyDown(KEY_UP)) {
        if(h2->pos.y > SCREEN_BORDER)
            h2->pos.y -= h2->speed;
        if(barrier_collision(m2, h2->pos) ||CheckCollisionRecs(h->pos, h2->pos)) 
            h2->pos.y += h2->speed;
          for ( int i = 0; i < g->maps[0].num_poder; i++)  {
            
      if( (CheckCollisionRecs(g->hero2.pos,g->maps[0].velocidade[i]))){
        g->hero.speed++;
        g->maps[0].velocidade[i] = (Rectangle) {0, 0, 0,0};
      } else if ( (CheckCollisionRecs(g->hero2.pos, g->maps[0].explosao[i]))){
        for(int j = 0; j < g->hero2.num_bombs; j++){
        

         g->hero2.bombs[j].distance +=0.5;
        
    }
        g->maps[0].explosao[i] = (Rectangle) {0, 0, 0,0};
      }else if ( (CheckCollisionRecs(g->hero2.pos, g->maps[0].num_bombas[i]))){
         g->hero2.num_bombs++;
          g->hero2.bombs[g->hero2.num_bombs-1].distance = g->hero2.bombs[g->hero2.num_bombs-2].distance;
         g->maps[0].num_bombas[i] = (Rectangle) {0, 0, 0,0};
      }
        }

    } else if(IsKeyDown(KEY_DOWN)) {
        if(h2->pos.y + h2->pos.height < g->screenHeight - SCREEN_BORDER+30)
            h2->pos.y += h2->speed;
        if(barrier_collision(m2, h2->pos) || CheckCollisionRecs(h->pos, h2->pos)) 
            h2->pos.y -= h2->speed;
          for ( int i = 0; i < g->maps[0].num_poder; i++)  {
            
      if( (CheckCollisionRecs(g->hero2.pos,g->maps[0].velocidade[i]))){
        g->hero.speed++;
        g->maps[0].velocidade[i] = (Rectangle) {0, 0, 0,0};
      } else if ( (CheckCollisionRecs(g->hero2.pos, g->maps[0].explosao[i]))){
        for(int j = 0; j < g->hero2.num_bombs; j++){
        

         g->hero2.bombs[j].distance +=0.5;
        
    }
        g->maps[0].explosao[i] = (Rectangle) {0, 0, 0,0};
      }else if ( (CheckCollisionRecs(g->hero2.pos, g->maps[0].num_bombas[i]))){
         g->hero2.num_bombs++;
          g->hero2.bombs[g->hero2.num_bombs-1].distance = g->hero2.bombs[g->hero2.num_bombs-2].distance;
         g->maps[0].num_bombas[i] = (Rectangle) {0, 0, 0,0};
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
                    for(int i = 0; i < g->maps[0].num_destrutivas; i++){
                        
        if(CheckCollisionRecs(g->maps[0].destrutivas[i], verify_bomb)){
              int aux = rand() % 4 + 1;
               if(aux == 1){
                 g->maps[0].velocidade[i] = (Rectangle) {g->maps[0].destrutivas[i].x, g->maps[0].destrutivas[i].y, 20, 20};
               } else if(aux == 2){
                 g->maps[0].explosao[i] = (Rectangle) {g->maps[0].destrutivas[i].x, g->maps[0].destrutivas[i].y, 20, 20};
               } else if(aux == 3){
                 g->maps[0].num_bombas[i] = (Rectangle) {g->maps[0].destrutivas[i].x, g->maps[0].destrutivas[i].y, 20, 20};
               }
            g->maps[0].destrutivas[i] = (Rectangle) {0,0,0,0};     
           
             

          
            //sortear numero de 1 a 4 na lista de poderes possiveis 
            //colocar o item sorteado 
            // poder atual = p+1
                         }
                    }
                    if(!barrier_collision(actual_map, verify_bomb)){
                        g->hero.bombs[i].explosion_right.width += grow_tax;
                    } if(CheckCollisionRecs(g->hero.pos,rectangle_bomb )){
                        g->gameover = 1;
                        break;
                    } if(CheckCollisionRecs(g->hero2.pos,rectangle_bomb )){
                        g->gameover = 1;
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
                   for(int i = 0; i < g->maps[0].num_destrutivas; i++){
       if(CheckCollisionRecs(g->maps[0].destrutivas[i], verify_bomb)){
              int aux = rand() % 4 + 1;
               if(aux == 1){
                 g->maps[0].velocidade[i] = (Rectangle) {g->maps[0].destrutivas[i].x, g->maps[0].destrutivas[i].y, 20, 20};
               } else if(aux == 2){
                 g->maps[0].explosao[i] = (Rectangle) {g->maps[0].destrutivas[i].x, g->maps[0].destrutivas[i].y, 20, 20};
               } else if(aux == 3){
                 g->maps[0].num_bombas[i] = (Rectangle) {g->maps[0].destrutivas[i].x, g->maps[0].destrutivas[i].y, 20, 20};
               }
            g->maps[0].destrutivas[i] = (Rectangle) {0,0,0,0};     
           
             

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
                    } if(CheckCollisionRecs(g->hero.pos,rectangle_bomb )){
                        g->gameover = 1;
                        break;
                    }if(CheckCollisionRecs(g->hero2.pos,rectangle_bomb )){
                        g->gameover = 1;
                        break;
                    }
                   
                }

                if(g->hero.bombs[i].explosion_up.height < g->hero.bombs[i].distance * STD_SIZE_Y){
                    rectangle_bomb = g->hero.bombs[i].explosion_up;
                    Rectangle verify_bomb = (Rectangle){ rectangle_bomb.x, rectangle_bomb.y, rectangle_bomb.width, rectangle_bomb.height + grow_tax};
                    for(int i = 0; i < g->maps[0].num_destrutivas; i++){
        if(CheckCollisionRecs(g->maps[0].destrutivas[i], verify_bomb)){
              int aux = rand() % 4 + 1;
               if(aux == 1){
                 g->maps[0].velocidade[i] = (Rectangle) {g->maps[0].destrutivas[i].x, g->maps[0].destrutivas[i].y, 20, 20};
               } else if(aux == 2){
                 g->maps[0].explosao[i] = (Rectangle) {g->maps[0].destrutivas[i].x, g->maps[0].destrutivas[i].y, 20, 20};
               } else if(aux == 3){
                 g->maps[0].num_bombas[i] = (Rectangle) {g->maps[0].destrutivas[i].x, g->maps[0].destrutivas[i].y, 20, 20};
               }
            g->maps[0].destrutivas[i] = (Rectangle) {0,0,0,0};     
           
        }
        }
                    if(!barrier_collision(actual_map, verify_bomb)){
                        g->hero.bombs[i].explosion_up.height += grow_tax;
                    } if(CheckCollisionRecs(g->hero.pos,rectangle_bomb )){
                        g->gameover = 1;
                        break;
                    } if(CheckCollisionRecs(g->hero2.pos,rectangle_bomb )){
                        g->gameover = 1;
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
                   for(int i = 0; i < g->maps[0].num_destrutivas; i++){
        if(CheckCollisionRecs(g->maps[0].destrutivas[i], verify_bomb)){
              int aux = rand() % 4 + 1;
               if(aux == 1){
                 g->maps[0].velocidade[i] = (Rectangle) {g->maps[0].destrutivas[i].x, g->maps[0].destrutivas[i].y, 20, 20};
               } else if(aux == 2){
                 g->maps[0].explosao[i] = (Rectangle) {g->maps[0].destrutivas[i].x, g->maps[0].destrutivas[i].y, 20, 20};
               } else if(aux == 3){
                 g->maps[0].num_bombas[i] = (Rectangle) {g->maps[0].destrutivas[i].x, g->maps[0].destrutivas[i].y, 20, 20};
               }
            g->maps[0].destrutivas[i] = (Rectangle) {0,0,0,0};     
           
        }
        }
                    if(!barrier_collision(actual_map, verify_bomb)){
                        g->hero.bombs[i].explosion_down.height += grow_tax;
                        g->hero.bombs[i].explosion_down.y -= grow_tax;
                    } if(CheckCollisionRecs(g->hero.pos,rectangle_bomb )){
                        g->gameover = 1;
                        break;
                    } if(CheckCollisionRecs(g->hero2.pos,rectangle_bomb )){
                        g->gameover = 1;
                        break;
                    }
                    for(int j = 0 ; j< g->hero.num_bombs;j++){
                        if(CheckCollisionRecs(g->hero.bombs[j].pos,rectangle_bomb) && i!= j ){
                           
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
            }else if(fabs(g->hero.bombs[i].time - GetTime()) > 3){
                g->hero.bombs[i].isActive = 0;
            }
        }
    }

    g->hero.put_bomb = 0;

    if(IsKeyPressed(KEY_L)){
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
                    for(int i = 0; i < g->maps[0].num_destrutivas; i++){
       if(CheckCollisionRecs(g->maps[0].destrutivas[i], verify_bomb)){
              int aux = rand() % 4 + 1;
               if(aux == 1){
                 g->maps[0].velocidade[i] = (Rectangle) {g->maps[0].destrutivas[i].x, g->maps[0].destrutivas[i].y, 20, 20};
               } else if(aux == 2){
                 g->maps[0].explosao[i] = (Rectangle) {g->maps[0].destrutivas[i].x, g->maps[0].destrutivas[i].y, 20, 20};
               } else if(aux == 3){
                 g->maps[0].num_bombas[i] = (Rectangle) {g->maps[0].destrutivas[i].x, g->maps[0].destrutivas[i].y, 20, 20};
               }
            g->maps[0].destrutivas[i] = (Rectangle) {0,0,0,0};     
           
       }
                    }
                    if(!barrier_collision(actual_map, verify_bomb)){
                        g->hero2.bombs[i].explosion_right.width += grow_tax;
                    } if(CheckCollisionRecs(g->hero2.pos,rectangle_bomb )){
                        g->gameover = 1;
                        break;
                    } if(CheckCollisionRecs(g->hero.pos,rectangle_bomb)){
                        g->gameover = 1;
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
                    for(int i = 0; i < g->maps[0].num_destrutivas; i++){
      if(CheckCollisionRecs(g->maps[0].destrutivas[i], verify_bomb)){
              int aux = rand() % 4 + 1;
               if(aux == 1){
                 g->maps[0].velocidade[i] = (Rectangle) {g->maps[0].destrutivas[i].x, g->maps[0].destrutivas[i].y, 20, 20};
               } else if(aux == 2){
                 g->maps[0].explosao[i] = (Rectangle) {g->maps[0].destrutivas[i].x, g->maps[0].destrutivas[i].y, 20, 20};
               } else if(aux == 3){
                 g->maps[0].num_bombas[i] = (Rectangle) {g->maps[0].destrutivas[i].x, g->maps[0].destrutivas[i].y, 20, 20};
               }
            g->maps[0].destrutivas[i] = (Rectangle) {0,0,0,0};     
           
             
                         }
                    }
                    if(!barrier_collision(actual_map, verify_bomb)){
                        g->hero2.bombs[i].explosion_left.width += grow_tax;
                        g->hero2.bombs[i].explosion_left.x -= grow_tax;
                    } if(CheckCollisionRecs(g->hero2.pos,rectangle_bomb )){
                        g->gameover = 1;
                        break;
                    } if(CheckCollisionRecs(g->hero.pos,rectangle_bomb)){
                        g->gameover = 1;
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
                    for(int i = 0; i < g->maps[0].num_destrutivas; i++){
       if(CheckCollisionRecs(g->maps[0].destrutivas[i], verify_bomb)){
              int aux = rand() % 4 + 1;
               if(aux == 1){
                 g->maps[0].velocidade[i] = (Rectangle) {g->maps[0].destrutivas[i].x, g->maps[0].destrutivas[i].y, 20, 20};
               } else if(aux == 2){
                 g->maps[0].explosao[i] = (Rectangle) {g->maps[0].destrutivas[i].x, g->maps[0].destrutivas[i].y, 20, 20};
               } else if(aux == 3){
                 g->maps[0].num_bombas[i] = (Rectangle) {g->maps[0].destrutivas[i].x, g->maps[0].destrutivas[i].y, 20, 20};
               }
            g->maps[0].destrutivas[i] = (Rectangle) {0,0,0,0};     
           
             
                         }
                    }
                    if(!barrier_collision(actual_map, verify_bomb)){
                        g->hero2.bombs[i].explosion_up.height += grow_tax;
                    } if(CheckCollisionRecs(g->hero2.pos,rectangle_bomb )){
                        g->gameover = 1;
                        break;
                    } if(CheckCollisionRecs(g->hero.pos,rectangle_bomb)){
                        g->gameover = 1;
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
                    for(int i = 0; i < g->maps[0].num_destrutivas; i++){
       if(CheckCollisionRecs(g->maps[0].destrutivas[i], verify_bomb)){
              int aux = rand() % 4 + 1;
               if(aux == 1){
                 g->maps[0].velocidade[i] = (Rectangle) {g->maps[0].destrutivas[i].x, g->maps[0].destrutivas[i].y, 20, 20};
               } else if(aux == 2){
                 g->maps[0].explosao[i] = (Rectangle) {g->maps[0].destrutivas[i].x, g->maps[0].destrutivas[i].y, 20, 20};
               } else if(aux == 3){
                 g->maps[0].num_bombas[i] = (Rectangle) {g->maps[0].destrutivas[i].x, g->maps[0].destrutivas[i].y, 20, 20};
               }
            g->maps[0].destrutivas[i] = (Rectangle) {0,0,0,0};     
           
             
                         }
                    }
                    if(!barrier_collision(actual_map, verify_bomb)){
                        g->hero2.bombs[i].explosion_down.height += grow_tax;
                        g->hero2.bombs[i].explosion_down.y -= grow_tax;
                    } if(CheckCollisionRecs(g->hero2.pos,rectangle_bomb )){
                        g->gameover = 1;
                        break;
                    } if(CheckCollisionRecs(g->hero.pos,rectangle_bomb)){
                        g->gameover = 1;
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
    g->maps[0].num_borders = 10;
    g->maps[0].borders[0] = (Rectangle) {0, 0, SCREEN_BORDER+60, g->screenHeight};
    g->maps[0].borders[1] = (Rectangle) {0, 0, g->screenWidth, SCREEN_BORDER};
    g->maps[0].borders[2] = (Rectangle) {0, g->screenHeight+30-SCREEN_BORDER+30, g->screenWidth, g->screenHeight};
    g->maps[0].borders[3] = (Rectangle) {g->screenWidth-SCREEN_BORDER-110, 0, g->screenWidth, g->screenHeight};
    g->maps[0].num_barriers = 100;
    g->maps[0].num_destrutivas = 100;
    g->maps[0].num_poder = 100;
   int posy = 150;
for (int i = 0; i < 6; i++) {
    int posx = 210;

    for (int j = 0; j < 6; j++) {
        g->maps[0].barriers[i * 6 + j] = (Rectangle) { posx, posy, STD_SIZE_X, STD_SIZE_Y };

        if (j == 5) {
            // Bloco destrutivo no final da linha
            g->maps[0].destrutivas[i * 6 + j] = (Rectangle) { 0, 0, 0, 0 };  // Substitua os valores pelos desejados
        } else {
            // Bloco indestrutivo
            g->maps[0].destrutivas[i * 6 + j] = (Rectangle) { posx + 30, posy, STD_SIZE_X, STD_SIZE_Y };
        }

        posx += STD_SIZE_X + 30;  // Ajuste de espaçamento
    }

    posy += STD_SIZE_Y + 30;  // Ajuste de espaçamento
}
    g->maps[0].color = GRAY;
    g->maps[0].prev_map = -1;
    g->maps[0].next_map = 1;
    for(int i = 0; i < g->hero.num_bombs; i++){
        g->hero.bombs[i].isActive = 0;
        g->hero.bombs[i].distance = 1;
    }
    for(int i = 0; i < g->hero2.num_bombs; i++){
        g->hero2.bombs[i].isActive = 0;
        g->hero2.bombs[i].distance = 1;
    }
}
