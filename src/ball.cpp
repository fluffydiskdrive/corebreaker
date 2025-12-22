#include "ball.h"
#include "assets.h"
#include "level.h"
#include "paddle.h"

#include "raylib.h"

#include <cmath>
#include <iostream>
#include <numbers>

void spawn_ball()
{
    ball_pos = (Vector2p { paddle_pos.dist - 2.0f, paddle_pos.ang_d }).to_cartesian();
    // ball_pos = Vector2(0.83,-paddle_pos.dist - 20);
    // ball_pos = Vector2(-paddle_pos.dist - 20, 0);
    b2BodyDef ball_body_def = b2DefaultBodyDef();

    ball_body_def.type = b2_dynamicBody;
    ball_body_def.position = { ball_pos.x, ball_pos.y };
    ball_body_def.rotation = b2MakeRot(0.0f);
    ball_body_def.fixedRotation = true;

    auto* ball_data = new CustomBodyData { COLLISION_BALL };
    ball_body_def.userData = ball_data;

    b2ShapeDef ball_shape_def = b2DefaultShapeDef();
    ball_shape_def.enableContactEvents = true;
    ball_shape_def.material.restitution = 1.0f;
    b2Circle ball_circle = { b2Vec2 { 0, 0 }, ball_radius };

    auto ball_id = b2CreateBody(world_id, &ball_body_def);

    b2CreateCircleShape(ball_id, &ball_shape_def, &ball_circle);
    b2Body_SetBullet(ball_id, true);
    // b2Body_SetLinearVelocity(ball_id, Vector2p{-1, 90.0f}.to_cartesian_b2());

    // ball_pos = b2V2_to_V2(b2Body_GetPosition(ball_id));

    bodies.emplace_back(ball_id, &ball_texture);
    ball = bodies.back();
    // ball_pos = b2V2_to_V2(b2Body_GetPosition(ball._body_id));
    ball._graph_position = ball_pos;

    invincibility = powerup(
        [](bool active) {
            if (active) {
                BORDER_COLOR = { 68, 149, 255, 255 };
                if (!is_ball_inside_level()) {
                    const auto curr_vel = b2Body_GetLinearVelocity(ball._body_id);
                    const auto normal_vel = b2Normalize(b2Body_GetPosition(ball._body_id));
                    const auto reflected_vel = curr_vel - (2 * (curr_vel.x * normal_vel.x + curr_vel.y * normal_vel.y)) * normal_vel;
                    b2Body_SetLinearVelocity(ball._body_id, reflected_vel);
                }
            } else {
                BORDER_COLOR = GRAY;
            }
        },
        7.0f);
    paddle_x4 = powerup(
        [](bool active) {
            for (int i = 1; i < 4; ++i) {
                if (active) {
                    b2Body_Enable(paddles[i]._body_id);
                    paddles[i]._to_draw = true;
                } else {
                    b2Body_Disable(paddles[i]._body_id);
                    paddles[i]._to_draw = false;
                }
            }
        },
        7.0f);
}

void move_ball()
{

    ball_pos = b2V2_to_V2(b2Body_GetPosition(ball._body_id));
    b2Vec2 ball_vel = b2Body_GetLinearVelocity(ball._body_id);
    float ball_vel_mag = b2Length(ball_vel);
    b2Body_SetLinearVelocity(ball._body_id, b2Vec2 { ball_vel.x / ball_vel_mag * BALL_SPEED, ball_vel.y / ball_vel_mag * BALL_SPEED });

    ball._graph_position = ball_pos;
}

std::vector<Body> arrows;
Body arrow;
inline constexpr float arrow_speed = 2;

void choose_dir()
{
    // Vector2p arrow_pos = {1.5f, 270.0f};
    Vector2p arrow_pos = to_polar(ball_pos);
    if (arrows.empty()) {
        arrows.emplace_back(&arrow_texture);
        arrow = arrows.back();
    }

    const Vector2 arrow_relative_pos = Vector2p { 5, arrow._graph_rotation_d - 90 }.to_cartesian();

    arrow._graph_position = Vector2 { ball_pos.x + arrow_relative_pos.x, ball_pos.y + arrow_relative_pos.y };

    arrow.draw();

    // auto arrow_pos = to_polar(b2Body_GetPosition(arrow._body_id));
    if (IsKeyDown(KEY_A)) {
        // b2Body_SetTransform(arrow._body_id, (arrow_pos + Vector2p{0, -paddle_speed / 10}).to_cartesian_b2(), b2MakeRot(DEG2RAD * (arrow_pos.ang_d + paddle_speed)));
        arrow._graph_rotation_d -= arrow_speed;
        if (arrow._graph_rotation_d <= -45)
            arrow._graph_rotation_d = -45;
    }
    if (IsKeyDown(KEY_D)) {
        // b2Body_SetTransform(arrow._body_id, (arrow_pos + Vector2p{0, paddle_speed / 10}).to_cartesian_b2(), b2MakeRot(DEG2RAD * (arrow_pos.ang_d - paddle_speed)));
        arrow._graph_rotation_d += arrow_speed;
        if (arrow._graph_rotation_d >= 45)
            arrow._graph_rotation_d = 45;
    }
    if (IsKeyDown(KEY_SPACE)) {
        b2Body_SetLinearVelocity(ball._body_id, b2Vec2 { -std::cosf(DEG2RAD * (arrow._graph_rotation_d + 90.0f)), -std::sinf(DEG2RAD * (arrow._graph_rotation_d + 90.0f)) });
        arrows.clear();
        game_state = in_game_state;
    }
    PollInputEvents();
}

bool is_ball_inside_level()
{
    // return is_inside_level(static_cast<int>(ball_pos.y), static_cast<int>(ball_pos.x));
    return to_polar(ball_pos).dist <= paddle_pos.dist;
}
