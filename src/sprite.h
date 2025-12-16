#ifndef SPRITE_H
#define SPRITE_H

#include "raylib.h"

#include <string>

struct sprite {
    size_t frame_count = 0;
    size_t frames_to_skip = 3;
    size_t frames_skipped = 0;
    size_t frame_index = 0;
    bool loop = true;
    size_t prev_game_frame = 0;
    Texture2D* frames = nullptr;
};

sprite load_sprite(const std::string& file_name_prefix, const std::string& file_name_suffix, size_t frame_count = 1, bool loop = true, size_t frames_to_skip = 3);
void unload_sprite(sprite& sprite);

#endif // SPRITE_H
