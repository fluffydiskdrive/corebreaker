#include "game.h"
#include "ball.h"
#include "assets.h"
#include "graphics.h"
#include "level.h"
#include "paddle.h"

#include "raylib.h"

#include <iostream>

//-----------------------------------------A



void update(float delta)
{
    b2World_Step(world_id, delta, 4);

    if (IsKeyPressed(KEY_ESCAPE) && game_state == in_game_state) {
        game_state = paused_state;
    }
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
        move_paddle(PADDLE_SPEED);
    }
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
        move_paddle(-PADDLE_SPEED);
    }

    if (IsKeyDown(KEY_BACKSLASH)) {
        Vector2 o = paddle._graph_position;
        auto i = pp_cart;
    }

    // switch (game_state) {
    // case menu_state: while (!IsKeyDown(KEY_ENTER)){draw_menu();} game_state = in_game_state; break;
    // case paused_state: while (!IsKeyDown(KEY_ESCAPE)){draw_menu();} game_state = in_game_state; break;
    //     default:;
    // }
    move_ball();
    contact_ball();
    destroy_boxes();



    if (!is_ball_inside_level()) {
        game_state = dir_choice_state;
        load_level();
        PlaySound(lose_sound);
    } else if (level_passed) {
        game_state = dir_choice_state;
        load_level(1);
        PlaySound(win_sound);
        level_passed = false;

    }
}

void draw()
{
    // switch (game_state) {
    // case in_game_state:{
        DrawCircleV({0,0}, paddle_pos.dist + 0.05, GRAY);
        DrawCircleV({0,0}, paddle_pos.dist - 0.05, BLACK);
        draw_level();
        draw_paddle();
        draw_ball();
        draw_ui();
    // case paused_state: draw_pause_menu();
    // }

}

void init_game()
{
    camera = Camera2D();
    camera.target = {0, 0};
    camera.offset = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
    camera.zoom = 10.0f;

    level_passed = false;

    load_fonts();
    load_textures();
    load_level();
    load_sounds();

    game_state = dir_choice_state;

}

void handle_states(float delta)
{
    switch (game_state) {
    case in_game_state: update(delta); break;
    case dir_choice_state: choose_dir(); break;
    // case paused_state: {
    //     PollInputEvents();
    //     if (IsKeyDown(KEY_ESCAPE)) game_state = in_game_state;
    // }
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


        BeginDrawing();
        BeginMode2D(camera);

        //DrawRectangleV({0,0}, {10, 10}, GREEN);

        draw();
        handle_states(delta);
        //update(delta);
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
