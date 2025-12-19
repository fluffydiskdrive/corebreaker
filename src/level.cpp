#include "level.h"

#include "assets.h"
// #include "ball.h"
#include "game.h"
#include "graphics.h"
// #include "paddle.h"

#include "ball.h"
#include "paddle.h"
#include "powerup.h"
#include "raylib.h"

#include <vector>


Body init_box(Vector2 position, CustomBodyData* box_data, Texture2D* tex)
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
    auto box = Body{box_id, tex};
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

            if (in(current_level_data[row * columns + column], type_box)) {
                CustomBodyData* box_data;
                Texture2D* tex;

                switch (current_level_data[row * columns + column]){
                case BLOCK: {
                    box_data = new CustomBodyData {COLLISION_BOX};
                    tex = &block_texture;
                    break;
                }
                case CORE: {
                    box_data = new CustomBodyData {COLLISION_CORE};
                    tex = &core_texture;
                    break;
                }
                case BONUS_INVINCIBILITY: {
                    box_data = new CustomBodyData {COLLISION_BONUS_INVINCIBILITY};
                    tex = &invincibility_bonus_texture;
                    break;
                }
                case BONUS_PADDLE_X2: {
                    box_data = new CustomBodyData {COLLISION_BONUS_PADDLE_X2};
                    tex = &paddle_x2_bonus_texture;
                    break;
                }
                case WALL: {
                    box_data = new CustomBodyData {COLLISION_WALL};
                    tex = &wall_texture;
                    break;
                }
                }

                auto box = init_box({((static_cast<float>(columns) / 2) - column) * 2 - 1.0f, ((static_cast<float>(rows) / -2) + row) * 2 + 1.0f}, box_data, tex);
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
    paddles.clear();
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
        if (i->_to_delete) {
            b2DestroyBody(i->_body_id);
            --current_level_blocks;
            i = boxes.erase(i);
        } else ++i;
    }
}

void for_box(const b2ContactEndTouchEvent* end_touch_event)
{
    auto curr_body = b2Shape_GetBody(end_touch_event->shapeIdA);
    for (auto &j : boxes) {
        if (j._body_id.index1 == curr_body.index1 && j._body_id.generation == curr_body.generation && j._body_id.world0 == curr_body.world0) {
            j._to_delete = true;
        }
    }
    BALL_SPEED += 0.45f;
    PADDLE_SPEED += 0.055f;
}

void contact_ball()
{

    b2ContactEvents ball_contact_events = b2World_GetContactEvents(world_id);
    for (int i = 0; i < ball_contact_events.endCount; ++i) {
        b2ContactEndTouchEvent* end_touch_event = ball_contact_events.endEvents + i;



        switch (static_cast<CustomBodyData*>(b2Body_GetUserData(b2Shape_GetBody(end_touch_event->shapeIdA)))->_collision_type) {
        case COLLISION_BONUS_PADDLE_X2: paddle_x2._active = true; for_box(end_touch_event); break;
        case COLLISION_BOX: {
            for_box(end_touch_event);
            //paddle_x2.active = true;
            break;
        }
        case COLLISION_CORE: {
            level_passed = true;
            break;
        }
        case COLLISION_WALL: break;
        case COLLISION_BONUS_INVINCIBILITY: invincibility._active = true; for_box(end_touch_event); break;
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