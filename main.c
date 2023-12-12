/**
 * Bomberman Minecraft, 
 * 
 * por: Pedro Barreto e Vítor Amorim
 * 
 * Trabalho prático AEDS1 2023-2
*/

#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>

#include "InitGame.h"
#include "UpdateGame.h"
#include "DrawGame.h"
#include "UpdateDrawFrame.h"

#define STD_SIZE_X 30 // largura do jogador
#define STD_SIZE_Y 30 // altura do jogador
#define STD_SIZE_BOMB_X 20 // largura da bomba
#define STD_SIZE_BOMB_X2 30
#define STD_SIZE_BOMB_Y 20 // altura da bomba
#define STD_SIZE_BOMB_Y2 30
#define SCREEN_BORDER 120 // tamanho da borda
#define COUNTDOWN_DURATION 120 //duração máxima do jogo

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

    /* Inicializa as variáveis de som */
    Sound click = LoadSound("sprites/audio/click.mp3");
    Music initial_music = LoadMusicStream("sprites/audio/initial_music.mp3");
    Sound win_sound = LoadSound("sprites/audio/challenge_complete.mp3");
    Sound tnt_sound = LoadSound("sprites/audio/tnt-explosion.mp3");
    /* Parte responsável pela tela inicial do jogo*/
    PlayMusicStream(initial_music);
    int aux = 0;
    while(!game.start){
        UpdateMusicStream(initial_music);
        initial_screen(&game, &aux, click);
    }
    PauseMusicStream(initial_music);
    UnloadMusicStream(initial_music);

    map0_setup(&game);

    /* Parte responsável por criar as strings responsáveis pelos placares a serem exibidos */
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

    /* Atualiza o jogo enquanto os jogadores não decretam gameover pela tela de revanche */
    while(!game.gameover){
        Music game_music = LoadMusicStream("sprites/audio/in_game_music1.ogg");
        srand(time(NULL));
        /* Responsável por conseguir o tempo passado desde o início do programa, para depois possibilitar que sempre o jogo tenha um tempo limite de 120 segundos */
        float tempo_desde_inicio = GetTime();
        int aux_tempo = tempo_desde_inicio;

        while (game.start)
        {
            PlayMusicStream(game_music);
            UpdateMusicStream(game_music);
            DrawTexture(game.telaJogoMapa1, 0, 0, RAYWHITE);
            /* Atualiza a contagem regressiva de acordo com o tempo que se passou desde a entrada no jogo */
            float current_time = GetTime();
            game.countdown_value = COUNTDOWN_DURATION - (int) current_time + aux_tempo;

            UpdateDrawFrame(&game, tnt_sound);
            /* Termina o jogo caso peo menos um dos jogadores tenha morrido ou o tempo tenha se expirado */
            if(!game.hero.isAlive || !game.hero2.isAlive || (game.countdown_value <= -5)){
                game.start = 0;
                PlaySound(win_sound);
                break;
            }
        }
        PauseMusicStream(game_music);
        UnloadMusicStream(game_music);
        Music revenge_music = LoadMusicStream("sprites/audio/revenge_music.mp3");
        /* Parte responsável por atualizar o placar do jogo */
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
        /* Faz a tela de revanche */
        PlayMusicStream(revenge_music);
        while(!IsKeyDown(KEY_ENTER)){
            BeginDrawing();
            UpdateMusicStream(revenge_music);
            ClearBackground(RAYWHITE);
            DrawTexture(game.backgroundRevanche, 0, 0, RAYWHITE);
            /* Cria os botões de gameover e de revanche */
            Rectangle gameover_button = {32, 430, 218, 52};
            Rectangle revenge_button = {558, 430, 218, 52};
            /* Escreve o placar do jogo */
            DrawText(placar, (GetScreenWidth()/2 - MeasureText("EMPATE", 80)/2 + 80), 230, 80, WHITE);
            /* Escreve quem ganhou a partida */
            if((game.hero.isAlive == 0 && game.hero2.isAlive == 0) || (game.hero.isAlive == 1 && game.hero2.isAlive == 1)){
                DrawText("EMPATE!", (GetScreenWidth()/2 - MeasureText("EMPATE", 80)/2 + 40), 80, 60, WHITE);
            } else if(game.hero.isAlive == 0){
                DrawText(vencedor2, (GetScreenWidth()/2 - MeasureText(vencedor2, 80)/2 + 80), 80, 60, WHITE);
            } else{
                DrawText(vencedor1, (GetScreenWidth()/2 - MeasureText(vencedor1, 80)/2 + 80), 80, 60, WHITE);
            }
            /* Detecta se um dos jogadores apertou a tecla ESC para fechar a tela do jogo */
            if(IsKeyPressed(KEY_ESCAPE)){
                game.gameover = 1;
                break;
            }
            /* Detecta se o usuário apertou o botão esquerdo do mouse em cima de um dos botões da tela de revanche*/
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                if(CheckCollisionPointRec(GetMousePosition(), gameover_button)){
                    PlaySound(click);
                    /* Atualiza o arquivo placar.txt com o placar do jogo */
                    FILE *file = fopen("placar.txt", "a");
                    fputs(placar, file);
                    fputc('\n', file);
                    game.gameover = 1;
                    fclose(file);
                    break;
                }
                if(CheckCollisionPointRec(GetMousePosition(), revenge_button)){
                    PlaySound(click);
                    game.start = 1;
                    initRevanche(&game);
                    map0_setup(&game);
                    break;
                }
            }
            EndDrawing();
        }
        PauseMusicStream(revenge_music);
        UnloadMusicStream(revenge_music);
    }
    return 0;
}
