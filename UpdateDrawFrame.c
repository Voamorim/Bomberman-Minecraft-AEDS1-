#include "raylib.h"

#include "UpdateDrawFrame.h"
#include "UpdateGame.h"
#include "InitGame.h"
#include "DrawGame.h"

void UpdateDrawFrame(Game *g, Sound tnt_sound)
{
    UpdateGame(g, tnt_sound);
    DrawGame(g);
}