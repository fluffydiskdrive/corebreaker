#include "sprite.h"

#include <cassert>

sprite load_sprite(
    const std::string& file_name_prefix,
    const std::string& file_name_suffix,
    const size_t frame_count,
    const bool loop,
    const size_t frames_to_skip)
{
    assert(frame_count < 100);

    const sprite result = { frame_count, frames_to_skip, 0, 0, loop, 0, new Texture2D[frame_count] };
    for (size_t i = 0; i < frame_count; ++i) {
        std::string file_name;
        if (frame_count < 10) {
            file_name = file_name_prefix;
            file_name += std::to_string(i);
            file_name += file_name_suffix;
        } else {
            file_name = file_name_prefix;
            file_name += i < 10 ? "0" + std::to_string(i) : std::to_string(i);
            file_name += file_name_suffix;
        }
        result.frames[i] = LoadTexture(file_name.c_str());
    }

    return result;
}

void unload_sprite(sprite& sprite)
{
    assert(sprite.frames != nullptr);

    for (size_t i = 0; i < sprite.frame_count; ++i) {
        UnloadTexture(sprite.frames[i]);
    }
    delete[] sprite.frames;
    sprite.frames = nullptr;
}
