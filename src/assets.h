#ifndef ASSETS_H
#define ASSETS_H

// #define ASSETS_PATH "./data/"

#include "raylib.h"

#include "sprite.h"

inline Font menu_font;
inline Font title_font;

inline Texture2D wall_texture;
inline Texture2D void_texture;
inline Texture2D paddle_texture;
inline Texture2D block_texture;
inline Texture2D ball_texture;
inline Texture2D core_texture;
inline Texture2D arrow_texture;
inline Texture2D paddle_x4_bonus_texture;
inline Texture2D invincibility_bonus_texture;
inline Texture2D heart_texture;
inline Texture2D laser_body_texture;
inline Texture2D laser_end_texture;
inline Texture2D heart_black_texture;
inline Texture2D core_victory_texture;

inline sprite ball_sprite;

inline Sound win_sound;
inline Sound lose_sound;
inline Sound death_sound;
inline Sound powerup_sound;
inline Sound laser_sound;

inline Music main_theme;
inline Music boss_theme;

void load_fonts();
void unload_fonts();

void load_textures();
void unload_textures();

void load_sounds();
void unload_sounds();

#endif // ASSETS_H
