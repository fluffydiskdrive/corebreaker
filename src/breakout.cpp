#include "game.h"
#include "ball.h"
#include "assets.h"
#include "graphics.h"
#include "level.h"
#include "paddle.h"

#include "raylib.h"

#include <iostream>

//-----------------------------------------A

bool escape_was_down;
bool inited_victory_state;

void update(float delta)
{
    b2World_Step(world_id, delta, 4);

    bool escape_is_down = IsKeyDown(KEY_ESCAPE);
    if (!escape_was_down && escape_is_down && game_state == in_game_state) {
        game_state = paused_state;

    }
    escape_was_down = escape_is_down;

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
    switch (game_state) {
    case dir_choice_state:
    case in_game_state:{
        DrawCircleV({0,0}, paddle_pos.dist + 0.05, GRAY);
        DrawCircleV({0,0}, paddle_pos.dist - 0.05, BLACK);
        draw_level();
        draw_paddle();
        draw_ball();
        draw_ui();
        break;
        case paused_state: draw_pause_menu(); break;
        case menu_state: draw_menu(); break;
        case victory_state: draw_victory_menu(); break;
    }
    }

}

void init_game(bool re = false)
{
    if (!re){
        camera = Camera2D();
        camera.target = {0, 0};
        camera.offset = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
        camera.zoom = 10.0f;
    }

    viewport_size = Vector2{GetScreenWidth() / camera.zoom, GetScreenHeight() / camera.zoom};
    viewport_origin = Vector2{-viewport_size.x / 2.0f, -viewport_size.y / 2.0f};

   // render_texture = LoadRenderTexture(1280, 720);

    level_passed = false;
    escape_was_down = false;
    inited_victory_state = false;

    if (!re){
        load_fonts();
        load_textures();
        load_sounds();
    }
    load_level();

    game_state = re ? dir_choice_state : menu_state;

}

void handle_states(float delta)
{
    switch (game_state) {
    case in_game_state: update(delta); break;
    case dir_choice_state: choose_dir(); break;
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
    case victory_state: {
        if (!inited_victory_state) {init_victory_menu(); inited_victory_state = true;}
        if (IsKeyDown(KEY_ENTER)) {load_level(-(level_count - 1)); current_level_index = 0; game_state = dir_choice_state;}
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

        // BeginTextureMode(render_texture);
        // EndTextureMode();


        BeginDrawing();
        BeginMode2D(camera);
        //DrawRectangleV({0,0}, {10, 10}, GREEN);

        draw();
        handle_states(delta);
        //update(delta);
        // DrawTexturePro(
        //     render_texture.texture,
        //     Rectangle(0, 0, render_texture.texture.width, render_texture.texture.height),
        //     Rectangle(-(render_texture.texture.width / 2), -(render_texture.texture.height / 2), render_texture.texture.width, render_texture.texture.height),
        //     {0, 0},
        //     0, WHITE);
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
