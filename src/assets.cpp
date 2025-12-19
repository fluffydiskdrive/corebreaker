#include "assets.h"

#include "raylib.h"

void load_fonts()
{

    title_font = LoadFontEx(ASSETS_PATH"fonts/DaydreamDEMO.otf", 256, nullptr, 0);
    menu_font = LoadFontEx(ASSETS_PATH"fonts/prstartk.ttf", 256, nullptr, 0);
}

void unload_fonts()
{
    UnloadFont(menu_font);
}

void load_textures()
{
    wall_texture = LoadTexture(ASSETS_PATH"images/wall.png");
    void_texture = LoadTexture(ASSETS_PATH"images/void.png");
    block_texture = LoadTexture(ASSETS_PATH"images/block.png");
    paddle_texture = LoadTexture(ASSETS_PATH"images/paddle.png");
    ball_sprite = load_sprite(ASSETS_PATH"images/ball/ball", ".png", 8, true, 10);
    ball_texture = LoadTexture(ASSETS_PATH"images/ball/ball0.png");
    core_texture = LoadTexture(ASSETS_PATH"images/core.png");
    arrow_texture = LoadTexture(ASSETS_PATH"images/arrow.png");
    paddle_x2_bonus_texture = LoadTexture(ASSETS_PATH"images/paddle_x2_bonus.png");
    invincibility_bonus_texture = LoadTexture(ASSETS_PATH"images/invincibility_bonus.png");
}

void unload_textures()
{
    UnloadTexture(wall_texture);
    UnloadTexture(void_texture);
    UnloadTexture(block_texture);
    UnloadTexture(paddle_texture);
    unload_sprite(ball_sprite);
}

void load_sounds()
{
    InitAudioDevice();
    win_sound = LoadSound(ASSETS_PATH"sounds/win.wav");
    lose_sound = LoadSound(ASSETS_PATH"sounds/lose.wav");
}

void unload_sounds()
{
    UnloadSound(win_sound);
    UnloadSound(lose_sound);
    CloseAudioDevice();
}
