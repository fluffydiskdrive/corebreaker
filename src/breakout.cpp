#include "assets.h"
#include "ball.h"
#include "game.h"
#include "graphics.h"
#include "level.h"
#include "paddle.h"

#include "raylib.h"

#include <iostream>

bool escape_was_down;
bool inited_state;

void update(float delta)
{
    b2World_Step(world_id, delta, 4);

    bool escape_is_down = IsKeyDown(KEY_ESCAPE);
    if (!escape_was_down && escape_is_down && game_state == in_game_state) {
        game_state = paused_state;
    }
    escape_was_down = escape_is_down;

    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
        update_paddle(PADDLE_SPEED);
    }
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
        update_paddle(-PADDLE_SPEED);
    }

    move_ball();
    update_level(delta);

    invincibility.update_powerup(delta);
    paddle_x4.update_powerup(delta);

    if (!is_ball_inside_level() && !invincibility._active) {
        game_state = dir_choice_state;
        load_level();
        PlaySound(death_sound);
        if (IsSoundPlaying(laser_sound))
            StopSound(laser_sound);
        paddle_invincible_timer.restart_timer();
        --lives;
    } else if (level_passed) {
        game_state = dir_choice_state;
        load_level(1);
        PlaySound(win_sound);
        level_passed = false;
    }
    if (lives <= 0) {
        game_state = defeat_state;
        StopMusicStream(main_theme);
        StopMusicStream(boss_theme);
        PlaySound(lose_sound);
    }
}

void draw()
{
    switch (game_state) {
    case dir_choice_state:
    case in_game_state: {
        DrawCircleV({ 0, 0 }, paddle_pos.dist + 0.05, BORDER_COLOR);
        DrawCircleV({ 0, 0 }, paddle_pos.dist - 0.05, BLACK);
        draw_level();
        draw_paddle();
        draw_ball();
        draw_ui();
        break;
    case paused_state:
        draw_pause_menu();
        break;
    case menu_state:
        lives = MAX_LIVES;
        draw_menu();
        break;
    case victory_state:
        draw_state_menu();
        break;
    case defeat_state:
        draw_state_menu();
        break;
    }
    }
}

void init_game()
{
    camera = Camera2D();
    camera.target = { 0, 0 };
    camera.offset = { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
    camera.zoom = 15.0f;

    viewport_size = Vector2 { GetScreenWidth() / camera.zoom, GetScreenHeight() / camera.zoom };
    viewport_origin = Vector2 { -viewport_size.x / 2.0f, -viewport_size.y / 2.0f };

    // render_texture = LoadRenderTexture(1280, 720);

    level_passed = false;
    escape_was_down = false;
    inited_state = false;

    load_fonts();
    load_textures();
    load_sounds();
    load_level(0, true);

    game_state = menu_state;
    core_hp = 1;
    // invincibility = powerup(invincibility_effect())
    PlayMusicStream(main_theme);
    PlayMusicStream(boss_theme);
    PauseMusicStream(boss_theme);
}

void handle_states(const float delta)
{
    anim_timer.update_timer(delta);
    switch (game_state) {
    case in_game_state:
        update(delta);
        break;
    case dir_choice_state:
        choose_dir();
        break;
    case paused_state: {
        bool escape_is_down = IsKeyDown(KEY_ESCAPE);
        if (!escape_was_down && escape_is_down) {
            game_state = in_game_state;
        }
        escape_was_down = escape_is_down;
        break;
    }
    case menu_state: {
        if (IsKeyDown(KEY_ENTER)) {
            game_state = dir_choice_state;
        }
        break;
    }
    case defeat_state:
    case victory_state: {
        if (IsSoundPlaying(laser_sound))
            StopSound(laser_sound);
        if (!inited_state) {
            init_state_menu();
            inited_state = true;
        }
        if (IsKeyDown(KEY_ENTER)) {
            lives = MAX_LIVES;
            StopMusicStream(main_theme);
            StopMusicStream(boss_theme);
            PlayMusicStream(main_theme);
            // PlayMusicStream(boss_theme);
            current_level_index = 0;
            load_level(0, true);
            game_state = dir_choice_state;
        }
        break;
    }
    }
}

int main()
{
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(1280, 720, "Breakout");
    SetTargetFPS(60);

    init_game();

    SetExitKey(KEY_NULL);

    while (!WindowShouldClose()) {
        float delta = GetFrameTime();

        UpdateMusicStream(main_theme);
        UpdateMusicStream(boss_theme);

        BeginDrawing();
        BeginMode2D(camera);

        draw();
        handle_states(delta);

        EndMode2D();
        EndDrawing();

        // std::cout << "paddle_pos_GRAPH: x" << paddle._graph_position.x << ", y" << paddle._graph_position.y << "\n";
        // std::cout << "paddle_pos_PHYS: x" << b2Body_GetPosition(paddle._body_id).x << ", y" << b2Body_GetPosition(paddle._body_id).y << "\n";
        // std::cout << "ball_pos_GRAPH: x" << ball._graph_position.x << ", y" << ball._graph_position.y << "\n";
        // std::cout << "ball_pos_PHYS: x" << b2Body_GetPosition(ball._body_id).x << ", y" << b2Body_GetPosition(ball._body_id).y << "\n";
    }
    CloseWindow();

    unload_sounds();
    unload_level();
    unload_textures();
    unload_fonts();

    return 0;
}
