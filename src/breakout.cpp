#include "game.h"
#include "ball.h"
#include "assets.h"
#include "graphics.h"
#include "level.h"
#include "paddle.h"

#include "raylib.h"

#include <iostream>

//-----------------------------------------A

Camera2D camera = Camera2D();

void update(float delta)
{
    b2World_Step(world_id, delta, 4);

    if (IsKeyPressed(KEY_ESCAPE)) {
        game_state = paused_state;
    }
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
        move_paddle(paddle_speed);
    }
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
        move_paddle(-paddle_speed);
    }

    if (IsKeyDown(KEY_BACKSLASH)) {
        Vector2 o = paddle._graph_position;
        auto i = pp_cart;


    }
    move_ball();
    contact_ball();
    destroy_boxes();



    if (!is_ball_inside_level()) {
        load_level();
        PlaySound(lose_sound);
    } else if (current_level_blocks == 0) {
        load_level(1);
        PlaySound(win_sound);
    }
}

void draw()
{

    draw_level();
    draw_paddle();
    draw_ball();
    draw_ui();
}

void init_game()
{



    camera.target = {0, 0};
    camera.offset = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
    camera.zoom = 10.0f;

}

int main()
{
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(1280, 720, "Breakout");
    SetTargetFPS(60);

    init_game();

    load_fonts();
    load_textures();
    load_level();
    load_sounds();

    SetExitKey(KEY_NULL);

    while (!WindowShouldClose()) {
        float delta = GetFrameTime();
        update(delta);

        BeginDrawing();
        BeginMode2D(camera);
        //DrawRectangleV({0,0}, {10, 10}, GREEN);
        DrawCircleV({0,0}, paddle_pos.dist + 0.05, GRAY);
        DrawCircleV({0,0}, paddle_pos.dist - 0.05, BLACK);
        draw();
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
