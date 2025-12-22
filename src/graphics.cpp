#include "graphics.h"

#include "level.h"
#include "paddle.h"
#include "assets.h"
#include "ball.h"

#include "raylib.h"

#include <cmath>
#include <iostream>
#include <string>
#define CYAN (CLITERAL(Color) { 0, 255, 255, 255 })

struct Text {
    std::string str;
    Vector2 position = { 0.50f, 0.50f };
    float size = 32.0f;
    Color color = WHITE;
    float spacing = 4.0f;
    Font* font = nullptr;
};

constexpr float cell_scale = 0.6f;
constexpr float screen_scale_divisor = 700.0f;

constexpr size_t victory_ball_launch_degree_offset = 3;
constexpr size_t victory_balls_count = 360 / victory_ball_launch_degree_offset;
constexpr float victory_balls_speed = 1.0f;
constexpr float victory_balls_size = .2f;

Vector2 screen_size;
float screen_scale;
float cell_size;
Vector2 shift_to_center;

Vector2 victory_balls_pos[victory_balls_count];
Vector2 victory_balls_vel[victory_balls_count];

size_t game_frame = 0;

void draw_image(const Texture2D& image, const float x, const float y, const float width, const float height, const float rotation)
{
    const Rectangle source = { 0.0f, 0.0f, static_cast<float>(image.width), static_cast<float>(image.height) };
    const Rectangle destination = { x, y, width, height };
    DrawTexturePro(image, source, destination, { 0.0f, 0.0f }, rotation, WHITE);
}

void draw_image(const Texture2D& image, const float x, const float y, const float size, const float rotation)
{
    draw_image(image, x, y, size, size, 0);
}

void draw_sprite(sprite& sprite, const float x, const float y, const float width, const float height, const float rotation)
{
    draw_image(sprite.frames[sprite.frame_index], x, y, width, height, rotation);

    if (sprite.prev_game_frame == game_frame) {
        return;
    }
    if (sprite.frames_skipped < sprite.frames_to_skip) {
        ++sprite.frames_skipped;
    } else {
        sprite.frames_skipped = 0;

        ++sprite.frame_index;
        if (sprite.frame_index >= sprite.frame_count) {
            sprite.frame_index = sprite.loop ? 0 : sprite.frame_count - 1;
        }
    }
    sprite.prev_game_frame = game_frame;
}

void draw_sprite(sprite& sprite, const float x, const float y, const float size, const float rotation)
{
    draw_sprite(sprite, x, y, size, size, rotation);
}

void draw_text(const Text& text)
{
    const auto [x, y] = MeasureTextEx(*text.font, text.str.c_str(), text.size, text.spacing);
    const Vector2 pos = {
        text.position.x - 0.5f * x,
        text.position.y - 0.5f * y
    };
    DrawTextEx(*text.font, text.str.c_str(), pos, y, text.spacing, text.color);
}

void derive_graphics_metrics()
{
    screen_size.x = static_cast<float>(GetScreenWidth());
    screen_size.y = static_cast<float>(GetScreenHeight());

    cell_size = std::min(screen_size.x / static_cast<float>(current_level->columns), screen_size.y / static_cast<float>(current_level->rows));
    screen_scale = std::min(screen_size.x, screen_size.y) / screen_scale_divisor;

    const float level_width = static_cast<float>(current_level->columns) * cell_size;
    const float level_height = static_cast<float>(current_level->rows) * cell_size;
    shift_to_center = {
        (screen_size.x - level_width) * 0.5f,
        (screen_size.y - level_height)
    };
}

void draw_menu()
{
    ClearBackground(BLACK);

    const Text game_title = {
        "COREBREAKER",
        { 0.0f, 0.0f },
        10.0f,
        WHITE,
        .18f,
        &title_font
    };
    draw_text(game_title);

    const Text game_subtitle_1 = {
        "COREBREAKER",
        { 0.0f, 5.0f },
        1.5f,
        GRAY,
        5.0f,
        &menu_font
    };
    draw_text(game_subtitle_1);

    const Text game_subtitle = {
        "PRESS ENTER",
        { 0.0f, 10.0f },
        1.5f,
        LIGHTGRAY,
        .01f,
        &menu_font
    };
    draw_text(game_subtitle);

    DrawTextureEx(ball_texture, {-29.45, -3.22}, 0.0f, 0.39, WHITE);
}

void draw_ui()
{
    DrawTextureEx(heart_texture, {viewport_size.x / 2 - 16.0f, 10.0f -(heart_texture.height / 2 * GRAPH_SCALING * 6.0f) - 0.1f}, 0.0f, GRAPH_SCALING * 6.0f, WHITE);
    const Text lives_counter = {
        std::to_string(lives),
        {(viewport_size.x / 2.0f - 15.8f + (heart_texture.width / 2 * GRAPH_SCALING * 6.0f)), 10.0f},
        4.0f,
        BLACK,
        0.1f,
        &menu_font
    };
    draw_text(lives_counter);

    const Text level_counter = {
        "LEVEL " + std::to_string(current_level_index + 1),
        { -viewport_size.x / 2.0f + 12.0f, -viewport_size.y / 2 + 5.0f},
        2.0f,
        DARKGREEN,
        0.15f,
        &menu_font
    };
    draw_text(level_counter);

    const Text boxes_remaining = {
        "BLOCKS " + std::to_string(blocks_remaining),
        { -viewport_size.x / 2.0f + 14.15f, -viewport_size.y / 2 + 7.0f},
        2.0f,
        LIGHTGRAY,
        0.15f,
        &menu_font
    };
    draw_text(boxes_remaining);

    if (invincibility._active) {
        DrawTextureEx(invincibility_bonus_texture, {-viewport_size.x / 2.0f + 7.0f, -5}, 0.0, GRAPH_SCALING * 2, WHITE);
        const Text invincibility_timer_text = {
            std::to_string(static_cast<int>(roundf(invincibility.time_remaining_s))),
            { -viewport_size.x / 2.0f + 12.0f, -5 + invincibility_bonus_texture.height / 2.0f * GRAPH_SCALING * 2},
            2.0f,
            LIGHTGRAY,
            0.15f,
            &menu_font
        };
        draw_text(invincibility_timer_text);
    }
    if (paddle_x4._active) {
        DrawTextureEx(paddle_x4_bonus_texture, {-viewport_size.x / 2.0f + 7.0f, 0}, 0.0, GRAPH_SCALING * 2, WHITE);
        const Text paddle_x4_timer_text = {
            std::to_string(static_cast<int>(roundf(paddle_x4.time_remaining_s))),
            { -viewport_size.x / 2.0f + 12.0f, 0 + invincibility_bonus_texture.height / 2.0f * GRAPH_SCALING * 2},
            2.0f,
            LIGHTGRAY,
            0.15f,
            &menu_font
        };
        draw_text(paddle_x4_timer_text);
    }

    if (core_max_hp > 1) {
        DrawTextureEx(heart_black_texture, {viewport_size.x / 2 - 16.0f, -10.0f -(heart_texture.height / 2 * GRAPH_SCALING * 6.0f) - 0.1f}, 0.0f, GRAPH_SCALING * 6.0f, WHITE);
        const Text core_lives_counter = {
            std::to_string(core_hp),
            {(viewport_size.x / 2.0f - 15.8f + (heart_texture.width / 2 * GRAPH_SCALING * 6.0f)), -10.0f},
            4.0f,
            WHITE,
            0.1f,
            &menu_font
        };
        draw_text(core_lives_counter);
    }
}

void draw_level()
{
    ClearBackground(BLACK);

    // for (size_t row = 0; row < current_level->rows; ++row) {
    //     for (size_t column = 0; column < current_level->columns; ++column) {
    //         const char data = current_level->data[row * current_level->columns + column];
    //         const float texture_x_pos = shift_to_center.x + static_cast<float>(column) * cell_size;
    //         const float texture_y_pos = shift_to_center.y + static_cast<float>(row) * cell_size;
    //
    //         switch (data) {
    //         case WALL:
    //             draw_image(wall_texture, texture_x_pos, texture_y_pos, cell_size, 0);
    //             break;
    //         case BLOCK:
    //             draw_image(block_texture, texture_x_pos, texture_y_pos, cell_size, 0);
    //             break;
    //         default:;
    //         }
    //     }
    // }
    level_draw();
}

// void draw_paddle()
// {
//     const float texture_x_pos = shift_to_center.x + paddle_pos.to_cartesian().x * cell_size;
//     const float texture_y_pos = shift_to_center.y + paddle_pos.to_cartesian().y * cell_size;
//     draw_image(paddle_texture, texture_x_pos, texture_y_pos, paddle_size.x * cell_size, paddle_size.y * cell_size, 30);
// }

void draw_paddle()
{
        for (auto i : paddles) {
            i.draw();
        }
}


// void draw_ball()
// {
//     const float texture_x_pos = shift_to_center.x + ball_pos.x * cell_size;
//     const float texture_y_pos = shift_to_center.y + ball_pos.y * cell_size;
//     draw_sprite(ball_sprite, texture_x_pos, texture_y_pos, cell_size);
// }

void draw_ball()
{
    ball.draw();
}

void draw_boss_attack()
{
    attack_draw();
}

void draw_pause_menu()
{
    ClearBackground(BLACK);

    const Text paused_title = {
        "PAUSED",
        { 0.0f, -2.0f },
        2.5f,
        WHITE,
        .06f,
        &menu_font
    };
    draw_text(paused_title);
    const Text paused_subtitle = {
        "PRESS ESCAPE TO RESUME",
        { 0.0f, .7f },
        0.9f,
        LIGHTGRAY,
        .06f,
        &menu_font
    };
    draw_text(paused_subtitle);
}

float core_tex_scale;
float paddle_tex_pos;
float paddle_tex_rot;
unsigned char paddle_tex_tint;

void init_state_menu()
{
    // if (game_state == victory_state || game_state == defeat_state){
    //     for (size_t i = 0; i < victory_balls_count; ++i) {
    //         victory_balls_pos[i] = {viewport_origin.x + victory_balls_size, viewport_origin.y + victory_balls_size};
    //         victory_balls_vel[i] = {
    //             std::cosf(DEG2RAD * static_cast<float>(i * victory_ball_launch_degree_offset)) * victory_balls_speed,
    //             std::sinf(DEG2RAD * static_cast<float>(i * victory_ball_launch_degree_offset)) * victory_balls_speed
    //         };
    //     }
    // }
    anim_timer = timer{2.0f};
    anim_timer.restart_timer();
    switch (game_state) {
    case victory_state:{
        core_tex_scale = 0;
        break;
    }
    case defeat_state: {
        paddle_tex_pos = -50.0f;
        paddle_tex_rot = -30.0f;
        paddle_tex_tint = 80;
    }
    }
    //inited_victory_state = true;
}

void animate_state_menu()
{
    // if (game_state == victory_state || game_state == defeat_state) {
    //     for (size_t i = 0; i < victory_balls_count; ++i) {
    //         if (victory_balls_pos[i].x + victory_balls_vel[i].x > (viewport_origin.x + viewport_size.x) || victory_balls_pos[i].x + victory_balls_vel[i].x < viewport_origin.x) {
    //             victory_balls_vel[i].x *= -1.0f;
    //         }
    //         if (victory_balls_pos[i].y + victory_balls_vel[i].y > (viewport_origin.y + viewport_size.y)|| victory_balls_pos[i].y + victory_balls_vel[i].y < viewport_origin.y) {
    //             victory_balls_vel[i].y *= -1.0f;
    //         }
    //         victory_balls_pos[i] = {
    //             victory_balls_pos[i].x + victory_balls_vel[i].x,
    //             victory_balls_pos[i].y + victory_balls_vel[i].y
    //         };
    //     }
    // }
    switch (game_state) {
    case victory_state: {
        if (core_tex_scale < 15) core_tex_scale += 0.08;
        break;
    }
    case defeat_state: {
        if (paddle_tex_pos < 2.0f) paddle_tex_pos += 0.4;
        else {
            if (paddle_tex_rot < 0){ paddle_tex_rot += 0.8; paddle_tex_pos += 0.008;}
            else {
                if (paddle_tex_tint > 0) paddle_tex_tint -= 1;
            }
        }
    }
    }
}

void draw_state_menu()
{
    animate_state_menu();

    DrawRectangleV(viewport_origin, viewport_size, { 0, 0, 0, 50 });

    // for (const auto& [x, y] : victory_balls_pos) {
    //     DrawCircleV({ x, y }, victory_balls_size, WHITE);
    // }

    if (game_state == victory_state){
        DrawTextureEx(core_tex_scale < 15 ? core_texture : core_victory_texture, {-core_texture.width / 2.0f * core_tex_scale * GRAPH_SCALING, -core_texture.height / 2.0f * core_tex_scale * GRAPH_SCALING}, 0.0f, GRAPH_SCALING * core_tex_scale, DARKGRAY);
        const Text victory_title = {
            "VICTORY!",
            { 0.0f, 0.0f },
            6.0f,
            CYAN,
            .06f,
            &menu_font
        };
        draw_text(victory_title);
    } else if (game_state == defeat_state) {
        DrawTextureEx(paddle_texture, {-paddle_texture.width / 2.0f * 5 * GRAPH_SCALING, paddle_tex_pos}, paddle_tex_rot, GRAPH_SCALING * 5, Color{paddle_tex_tint, paddle_tex_tint, paddle_tex_tint, 255});
        const Text defeat_title = {
            "DEFEAT",
            { 0.0f, 0.0f },
            6.0f,
            RED,
            .06f,
            &menu_font
        };
        draw_text(defeat_title);
    }
    const Text subtitle = {
            "PRESS ENTER TO RESTART",
            { 0.0f, 5.5f },
            1.0f,
            GRAY,
            .01f,
            &menu_font
        };
    draw_text(subtitle);
}
