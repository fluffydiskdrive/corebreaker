#include "assets.h"

#include "raylib.h"

void load_fonts()
{

    title_font = LoadFontEx(ASSETS_PATH "fonts/Forma.ttf", 256, nullptr, 0);
    menu_font = LoadFontEx(ASSETS_PATH "fonts/prstartk.ttf", 256, nullptr, 0);
}

void unload_fonts()
{
    UnloadFont(menu_font);
    UnloadFont(title_font);
}

void load_textures()
{
    wall_texture = LoadTexture(ASSETS_PATH "images/wall.png");
    void_texture = LoadTexture(ASSETS_PATH "images/void.png");
    block_texture = LoadTexture(ASSETS_PATH "images/block.png");
    paddle_texture = LoadTexture(ASSETS_PATH "images/paddle.png");
    ball_sprite = load_sprite(ASSETS_PATH "images/ball/ball", ".png", 8, true, 10);
    ball_texture = LoadTexture(ASSETS_PATH "images/ball/ball0.png");
    core_texture = LoadTexture(ASSETS_PATH "images/core.png");
    arrow_texture = LoadTexture(ASSETS_PATH "images/arrow.png");
    paddle_x4_bonus_texture = LoadTexture(ASSETS_PATH "images/paddle_x4_bonus.png");
    invincibility_bonus_texture = LoadTexture(ASSETS_PATH "images/invincibility_bonus.png");
    heart_texture = LoadTexture(ASSETS_PATH "images/heart.png");
    laser_body_texture = LoadTexture(ASSETS_PATH "images/laser_body.png");
    laser_end_texture = LoadTexture(ASSETS_PATH "images/laser_end.png");
    heart_black_texture = LoadTexture(ASSETS_PATH "images/heart_black.png");
    core_victory_texture = LoadTexture(ASSETS_PATH "images/core_victory.png");

    SetTextureFilter(wall_texture, TEXTURE_FILTER_POINT);
    SetTextureFilter(void_texture, TEXTURE_FILTER_POINT);
    SetTextureFilter(paddle_texture, TEXTURE_FILTER_POINT);
    SetTextureFilter(block_texture, TEXTURE_FILTER_POINT);
    SetTextureFilter(ball_texture, TEXTURE_FILTER_POINT);
    SetTextureFilter(core_texture, TEXTURE_FILTER_POINT);
    SetTextureFilter(arrow_texture, TEXTURE_FILTER_POINT);
    SetTextureFilter(paddle_x4_bonus_texture, TEXTURE_FILTER_POINT);
    SetTextureFilter(invincibility_bonus_texture, TEXTURE_FILTER_POINT);
    SetTextureFilter(heart_texture, TEXTURE_FILTER_POINT);
    SetTextureFilter(laser_body_texture, TEXTURE_FILTER_POINT);
    SetTextureFilter(laser_end_texture, TEXTURE_FILTER_POINT);
}

void unload_textures()
{
    UnloadTexture(wall_texture);
    UnloadTexture(void_texture);
    UnloadTexture(block_texture);
    UnloadTexture(paddle_texture);
    UnloadTexture(ball_texture);
    UnloadTexture(core_texture);
    UnloadTexture(arrow_texture);
    UnloadTexture(paddle_x4_bonus_texture);
    UnloadTexture(invincibility_bonus_texture);
    UnloadTexture(heart_texture);
    UnloadTexture(laser_body_texture);
    UnloadTexture(laser_end_texture);
    UnloadTexture(heart_black_texture);
    UnloadTexture(core_victory_texture);

    unload_sprite(ball_sprite);
}

void load_sounds()
{
    InitAudioDevice();
    win_sound = LoadSound(ASSETS_PATH "sounds/win.wav");
    lose_sound = LoadSound(ASSETS_PATH "sounds/lose.mp3");
    death_sound = LoadSound(ASSETS_PATH "sounds/death.mp3");
    powerup_sound = LoadSound(ASSETS_PATH "sounds/powerup.mp3");
    laser_sound = LoadSound(ASSETS_PATH "sounds/laser.mp3");

    main_theme = LoadMusicStream(ASSETS_PATH "sounds/main_theme.mp3");
    boss_theme = LoadMusicStream(ASSETS_PATH "sounds/boss_theme.mp3");
}

void unload_sounds()
{
    UnloadSound(win_sound);
    UnloadSound(lose_sound);
    UnloadSound(death_sound);
    UnloadSound(powerup_sound);
    UnloadSound(laser_sound);

    CloseAudioDevice();
}
