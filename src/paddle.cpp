#include "paddle.h"

#include "assets.h"
#include "game.h"
#include "level.h"

#include "raylib.h"

#include <cmath>

void update_paddle(const float move_angle_deg)
{
    paddle_pos.ang_d += move_angle_deg;
    paddle_pos.ang_r = DEG2RAD * paddle_pos.ang_d;

    b2Body_SetTransform(paddles[0]._body_id, paddle_pos.to_cartesian_b2(), b2MakeRot(DEG2RAD * (paddle_pos.ang_d + 90.0f)));
    paddles[0]._graph_position = (paddle_pos + Vector2p { -0.4, 0 }).to_cartesian();

    for (int i = 1; i < 4; ++i) {
        auto new_position = Vector2p { paddle_pos.dist, paddle_pos.ang_d + 90.0f * i };
        b2Body_SetTransform(paddles[i]._body_id, new_position.to_cartesian_b2(), b2MakeRot(DEG2RAD * (new_position.ang_d + 90.0f)));
        paddles[i]._graph_position = Vector2p { new_position.dist - 0.4f, new_position.ang_d }.to_cartesian();
        // paddles[i]._graph_rotation_d = b2Body_GetRotation(paddles[i]._body_id).
    }
}

void spawn_paddle()
{
    paddle_pos = Vector2p { 20.0f, 90.0f };
    for (int i = 0; i < 4; ++i) {
        b2BodyDef paddle_body_def = b2DefaultBodyDef();

        paddle_body_def.type = b2_staticBody;
        paddle_body_def.position = paddle_pos.to_cartesian_b2();
        paddle_body_def.rotation = b2MakeRot(45.0f * DEG2RAD);

        auto* paddle_data = new CustomBodyData { COLLISION_PADDLE };
        paddle_body_def.userData = paddle_data;

        b2ShapeDef paddle_shape_def = b2DefaultShapeDef();
        paddle_shape_def.enableContactEvents = true;
        paddle_shape_def.material.restitution = 1.0f;
        b2Polygon paddle_polygon = b2MakeBox(2.4f, 0.4f);

        b2BodyId paddle_id = b2CreateBody(world_id, &paddle_body_def);
        b2CreatePolygonShape(paddle_id, &paddle_shape_def, &paddle_polygon);

        paddles.emplace_back(paddle_id, &paddle_texture);
        b2Body_Disable(paddles.back()._body_id);
        paddles.back()._to_draw = false;
    }
    // paddle = paddles[0];
    b2Body_Enable(paddles[0]._body_id);
    paddles[0]._to_draw = true;

    update_paddle(0);
    // outer_loop_end:;
}
