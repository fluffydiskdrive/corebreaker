#include "graphics.h"

#include "level.h"
#include "paddle.h"
#include "assets.h"
#include "ball.h"

#include "raylib.h"

#include <cmath>
#include <iostream>
#include <string>

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

void draw_sprite(sprite& sprite, const float x, const float y, const float width, const float height)
{
    draw_image(sprite.frames[sprite.frame_index], x, y, width, height, 0);

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

void draw_sprite(sprite& sprite, const float x, const float y, const float size)
{
    draw_sprite(sprite, x, y, size, size);
}

void draw_text(const Text& text)
{
    const auto [x, y] = MeasureTextEx(*text.font, text.str.c_str(), text.size * screen_scale, text.spacing);
    const Vector2 pos = {
        screen_size.x * text.position.x - 0.5f * x,
        screen_size.y * text.position.y - 0.5f * y
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
        "Breakout!",
        { 0.0f, 0.0f },
        10.0f,
        LIGHTGRAY,
        .18f,
        &title_font
    };
    draw_text(game_title);

    const Text game_subtitle = {
        "Press Enter to Start",
        { 0.0f, 0.01f },
        2.0f,
        LIGHTGRAY,
        .01f,
        &menu_font
    };
    draw_text(game_subtitle);
}

void draw_ui()
{
    const Text level_counter = {
        "LEVEL " + std::to_string(current_level_index + 1) + " OUT OF " + std::to_string(level_count),
        { 0.0f, -.045f },
        4.0f,
        WHITE,
        0.4f,
        &menu_font
    };
    draw_text(level_counter);

    const Text boxes_remaining = {
        "BLOCKS " + std::to_string(current_level_blocks),
        { 0.0f, 0.045f },
        4.0f,
        WHITE,
        0.4f,
        &menu_font
    };
    draw_text(boxes_remaining);
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

void draw_pause_menu()
{
    ClearBackground(BLACK);

    const Text paused_title = {
        "Press Escape to Resume",
        { 0.0f, 0.0f },
        4.0f,
        LIGHTGRAY,
        .06f,
        &menu_font
    };
    draw_text(paused_title);
}

void init_victory_menu()
{
    for (size_t i = 0; i < victory_balls_count; ++i) {
        victory_balls_pos[i] = {viewport_origin.x + victory_balls_size, viewport_origin.y + victory_balls_size};
        victory_balls_vel[i] = {
            std::cosf(DEG2RAD * static_cast<float>(i * victory_ball_launch_degree_offset)) * victory_balls_speed,
            std::sinf(DEG2RAD * static_cast<float>(i * victory_ball_launch_degree_offset)) * victory_balls_speed
        };
    }
    //inited_victory_state = true;
}

void animate_victory_menu()
{
    for (size_t i = 0; i < victory_balls_count; ++i) {
        if (victory_balls_pos[i].x + victory_balls_vel[i].x > (viewport_origin.x + viewport_size.x) || victory_balls_pos[i].x + victory_balls_vel[i].x < viewport_origin.x) {
            victory_balls_vel[i].x *= -1.0f;
        }
        if (victory_balls_pos[i].y + victory_balls_vel[i].y > (viewport_origin.y + viewport_size.y)|| victory_balls_pos[i].y + victory_balls_vel[i].y < viewport_origin.y) {
            victory_balls_vel[i].y *= -1.0f;
        }
        victory_balls_pos[i] = {
            victory_balls_pos[i].x + victory_balls_vel[i].x,
            victory_balls_pos[i].y + victory_balls_vel[i].y
        };
    }
}

void draw_victory_menu()
{
    animate_victory_menu();

    DrawRectangleV(viewport_origin, viewport_size, { 0, 0, 0, 50 });

    for (const auto& [x, y] : victory_balls_pos) {
        DrawCircleV({ x, y }, victory_balls_size, WHITE);
    }

    const Text victory_title = {
        "Victory!",
        { 0.0f, 0.0f },
        10.0f,
        RED,
        .06f,
        &menu_font
    };
    draw_text(victory_title);

    const Text victory_subtitle = {
        "Press Enter to Restart",
        { 0.0f, 0.01f },
        2.0f,
        WHITE,
        .02f,
        &menu_font
    };
    draw_text(victory_subtitle);
}
