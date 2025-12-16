#include "paddle.h"

#include "assets.h"
#include "game.h"
#include "level.h"

#include "raylib.h"

#include <cmath>

void move_paddle(const float angle_deg)
{
    paddle_pos.ang_d += angle_deg;

    paddle_pos.ang_r = DEG2RAD * paddle_pos.ang_d;

    b2Body_SetTransform(paddle._body_id, paddle_pos.to_cartesian_b2(), b2MakeRot(DEG2RAD * (paddle_pos.ang_d + 90.0f)));
    paddle._graph_position = (paddle_pos + Vector2p{-0.4, 0}).to_cartesian() ;
}

void spawn_paddle()
{
    // for (int column = 0; column < current_level.columns; column++) {
    //     for (int row = 0; row < current_level.rows; row++) {
    //         if (get_level_cell(row, column) == PADDLE) {
    //             set_level_cell(row, column, VOID);
    //             paddle_pos = { static_cast<float>(column), static_cast<float>(row) };
    //             goto outer_loop_end;
    //         }
    //     }
    // }
    paddle_pos = paddle_pos + Vector2p{10.0f, 0.0f};
    b2BodyDef paddle_body_def = b2DefaultBodyDef();

    paddle_body_def.type = b2_staticBody;
    paddle_body_def.position = paddle_pos.to_cartesian_b2();
    paddle_body_def.rotation = b2MakeRot(45.0f * DEG2RAD);

    auto paddle_data = CustomBodyData{COLLISION_PADDLE};
    paddle_body_def.userData = &paddle_data;

    b2ShapeDef paddle_shape_def = b2DefaultShapeDef();
    paddle_shape_def.enableContactEvents = true;
    paddle_shape_def.material.restitution = 1.0f;
    b2Polygon paddle_polygon = b2MakeBox(2.4f, 0.4f);

    b2BodyId paddle_id = b2CreateBody(world_id, &paddle_body_def);
    b2CreatePolygonShape(paddle_id, &paddle_shape_def, &paddle_polygon);

    bodies.emplace_back(paddle_id, &paddle_texture);
    paddle = bodies.back();

    move_paddle(0);
// outer_loop_end:;
}



// bool is_colliding_with_paddle(const Vector2 pos, const Vector2 size)
// {
//     const Rectangle paddle_hitbox = { paddle_pos.x, paddle_pos.y, paddle_size.x, paddle_size.y };
//     const Rectangle hitbox = { pos.x, pos.y, size.x, size.y };
//     return CheckCollisionRecs(paddle_hitbox, hitbox);
// }
