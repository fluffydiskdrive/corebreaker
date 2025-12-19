#include "level.h"

#include "assets.h"
// #include "ball.h"
#include "game.h"
#include "graphics.h"
// #include "paddle.h"

#include "ball.h"
#include "paddle.h"
#include "raylib.h"

#include <vector>



Body init_box(Vector2 position, CustomBodyData* box_data)
{

    b2BodyDef box_def = b2DefaultBodyDef();

    box_def.type = b2_staticBody;
    box_def.position = V2_to_b2V2(position);
    box_def.rotation = b2MakeRot(0.0f);
    box_def.userData = box_data;

    b2ShapeDef box_shape_def = b2DefaultShapeDef();
    box_shape_def.enableContactEvents = true;
    const b2Polygon box_polygon = b2MakeBox(0.8f, 0.8f);

    const auto box_id = b2CreateBody(world_id, &box_def);
    b2CreatePolygonShape(box_id, &box_shape_def, &box_polygon);
    auto box = Body{box_id, box_data->_collision_type == COLLISION_BOX ? &block_texture : &core_texture};
    box._graph_position = b2V2_to_V2(b2Body_GetPosition(box_id));
    return box;
}

void load_level(const int offset)
{
    if (current_level) {
        unload_level();
    }
    world_def = b2DefaultWorldDef();
    world_def.gravity = {0, 0};
    world_id = b2CreateWorld(&world_def);

    current_level_index += offset;
    if (current_level_index >= level_count) {
        game_state = victory_state;
        ClearBackground(BLACK);
        init_victory_menu();
        current_level_index = 0;

        return;
    }




    const size_t rows = levels[current_level_index].rows;
    const size_t columns = levels[current_level_index].columns;

    paddle_pos.dist = static_cast<float>(3.0f * sqrt((rows / 2) * (rows / 2) + (columns / 2) * (columns / 2)));


    current_level_blocks = 0;
    char* current_level_data = new char[rows * columns];
    for (int row = 0; row < rows; ++row) {
        for (int column = 0; column < columns; ++column) {
            current_level_data[row * columns + column] = levels[current_level_index].data[row * columns + column];
            if (current_level_data[row * columns + column] == BLOCK || current_level_data[row * columns + column] == CORE) {

                auto* box_data = new CustomBodyData { current_level_data[row * columns + column] == BLOCK ? COLLISION_BOX : COLLISION_CORE};
                auto box = init_box({((static_cast<float>(columns) / 2) - column) * 2 - 1.0f, ((static_cast<float>(rows) / -2) + row) * 2 + 1.0f}, box_data);
                boxes.push_back(box);
                //bodies.push_back(box);
                ++current_level_blocks;
            }
        }
    }
    current_level = new level { rows, columns, current_level_data };

    spawn_paddle();
    spawn_ball();

    derive_graphics_metrics();
}

void unload_level()
{
    b2DestroyWorld(world_id);
    delete[] current_level->data;
    delete current_level;
    bodies.clear();
    paddle_pos = {0, 0};
    ball_pos = {0, 0};
    boxes.clear();
    current_level = nullptr;
    //world_id = b2CreateWorld(&world_def);

}

bool is_inside_level(const int row, const int column)
{
    return row >= 0 && row < current_level->rows && column >= 0 && column < current_level->columns;
}

char& get_level_cell(const size_t row, const size_t column)
{
    return current_level->data[row * current_level->columns + column];
}

void set_level_cell(const size_t row, const size_t column, const char cell)
{
    get_level_cell(row, column) = cell;
}

void level_draw()
{
    for (const Body& i : boxes) {
        i.draw();
    }
}

void destroy_boxes()
{
    for (auto i = boxes.begin(); i != boxes.end(); ) {
        if (i->to_delete) {
            b2DestroyBody(i->_body_id);
            --current_level_blocks;
            i = boxes.erase(i);
        } else ++i;
    }
}

void contact_ball()
{
    b2ContactEvents ball_contact_events = b2World_GetContactEvents(world_id);
    for (int i = 0; i < ball_contact_events.endCount; ++i) {
        b2ContactEndTouchEvent* end_touch_event = ball_contact_events.endEvents + i;
        if (static_cast<CustomBodyData*>(b2Body_GetUserData(b2Shape_GetBody(end_touch_event->shapeIdA)))->_collision_type == COLLISION_BOX){
            auto curr_body = b2Shape_GetBody(end_touch_event->shapeIdA);
            for (auto &j : boxes) {
                if (j._body_id.index1 == curr_body.index1 && j._body_id.generation == curr_body.generation && j._body_id.world0 == curr_body.world0) {
                    j.to_delete = true;
                }
            }
            BALL_SPEED += 0.25f;
            PADDLE_SPEED += 0.055f;
        } else if (static_cast<CustomBodyData*>(b2Body_GetUserData(b2Shape_GetBody(end_touch_event->shapeIdA)))->_collision_type == COLLISION_CORE) {
            level_passed = true;
            // game_state = victory_state;
            // ClearBackground(BLACK);
            // init_victory_menu();
            // camera.offset = {0, 0};
            // camera.zoom = 1.0f;
            // draw_victory_menu();
        }
    }

}

// bool is_colliding_with_level_cell(const Vector2 pos, const Vector2 size, const char cell)
// {
//     const Rectangle hitbox = { pos.x, pos.y, size.x, size.y };
//
//     for (int row = static_cast<int>(pos.y); row <= static_cast<int>(pos.y + size.y); ++row) {
//         for (int column = static_cast<int>(pos.x); column <= static_cast<int>(pos.x + size.x); ++column) {
//             if (!is_inside_level(row, column)) {
//                 continue;
//             }
//
//             if (get_level_cell(row, column) == cell) {
//                 if (const Rectangle block_hitbox = { static_cast<float>(column), static_cast<float>(row), 1.0f, 1.0f }; CheckCollisionRecs(hitbox, block_hitbox)) {
//                     return true;
//                 }
//             }
//         }
//     }
//
//     return false;
// }
//
// char& get_colliding_level_cell(const Vector2 pos, const Vector2 size, const char look_for)
// {
//     const Rectangle hitbox = { pos.x, pos.y, size.x, size.y };
//
//     for (int row = static_cast<int>(pos.y); row <= static_cast<int>(pos.y + size.y); ++row) {
//         for (int column = static_cast<int>(pos.x); column <= static_cast<int>(pos.x + size.x); ++column) {
//             if (!is_inside_level(row, column)) {
//                 continue;
//             }
//
//             if (get_level_cell(row, column) == look_for) {
//                 if (const Rectangle block_hitbox = { static_cast<float>(column), static_cast<float>(row), 1.0f, 1.0f }; CheckCollisionRecs(hitbox, block_hitbox)) {
//                     return get_level_cell(row, column);
//                 }
//             }
//         }
//     }
//
//     return get_level_cell(static_cast<size_t>(pos.x), static_cast<size_t>(pos.y));
// }