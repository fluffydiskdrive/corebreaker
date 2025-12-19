#include "ball.h"
#include "level.h"
#include "paddle.h"
#include "assets.h"

#include "raylib.h"

#include <cmath>
#include <iostream>
#include <numbers>



void spawn_ball()
{
//     for (int column = 0; column < current_level->columns; column++) {
//         for (int row = 0; row < current_level->rows; row++) {
//             if (get_level_cell(row, column) == BALL) {
//                 set_level_cell(row, column, VOID);
//                 ball_pos = { static_cast<float>(column), static_cast<float>(row) };
//                 constexpr float ball_launch_angle_radians = ball_launch_angle_degrees * (std::numbers::pi_v<float> / 180.0f);
//                 ball_vel.y = -ball_launch_vel_mag * std::sin(ball_launch_angle_radians);
//                 ball_vel.x = (rand() % 2 == 0) ? ball_launch_vel_mag * std::cos(ball_launch_angle_radians) : -ball_launch_vel_mag * std::cos(ball_launch_angle_radians);
//                 goto outer_loop_end;
//             }
//         }
//     }
// outer_loop_end:;

    ball_pos = (Vector2p{paddle_pos.dist - 2.0f, paddle_pos.ang_d}).to_cartesian();
    //ball_pos = Vector2(0.83,-paddle_pos.dist - 20);
    //ball_pos = Vector2(-paddle_pos.dist - 20, 0);
    b2BodyDef ball_body_def = b2DefaultBodyDef();

    ball_body_def.type = b2_dynamicBody;
    ball_body_def.position = {ball_pos.x, ball_pos.y};
    ball_body_def.rotation = b2MakeRot(0.0f);
    ball_body_def.fixedRotation = true;

    auto* ball_data = new CustomBodyData{COLLISION_BALL};
    ball_body_def.userData = ball_data;

    b2ShapeDef ball_shape_def = b2DefaultShapeDef();
    ball_shape_def.enableContactEvents = true;
    ball_shape_def.material.restitution = 1.0f;
    b2Circle ball_circle = {b2Vec2{0, 0}, ball_radius};
    
    auto ball_id = b2CreateBody(world_id, &ball_body_def);

    b2CreateCircleShape(ball_id, &ball_shape_def, &ball_circle);
    b2Body_SetBullet(ball_id, true);
    //b2Body_SetLinearVelocity(ball_id, Vector2p{-1, 90.0f}.to_cartesian_b2());

    //ball_pos = b2V2_to_V2(b2Body_GetPosition(ball_id));

    bodies.emplace_back(ball_id, &ball_texture);
    ball = bodies.back();
    //ball_pos = b2V2_to_V2(b2Body_GetPosition(ball._body_id));
    ball._graph_position = ball_pos;
}

void move_ball()
{
    // Vector2 next_ball_pos = {
    //     ball_pos.x + ball_vel.x,
    //     ball_pos.y + ball_vel.y
    // };
    //
    //
    //
    // ball_pos = next_ball_pos;

    ball_pos = b2V2_to_V2(b2Body_GetPosition(ball._body_id));
    b2Vec2 ball_vel = b2Body_GetLinearVelocity(ball._body_id);
    float ball_vel_mag = b2Length(ball_vel);
    b2Body_SetLinearVelocity(ball._body_id, b2Vec2{ball_vel.x / ball_vel_mag * BALL_SPEED, ball_vel.y / ball_vel_mag * BALL_SPEED});

    ball._graph_position = ball_pos;
}

std::vector<Body> arrows;
Body arrow;
inline constexpr float arrow_speed = 2;

void choose_dir()
{
    //Vector2p arrow_pos = {1.5f, 270.0f};
    Vector2p arrow_pos = to_polar(ball_pos);
    if (arrows.empty()){

        // b2BodyDef arrow_body_def = b2DefaultBodyDef();
        //
        // // auto* arrow_data = new CustomBodyData{COLLISION_PADDLE};
        // // arrow_body_def.userData = arrow_data;
        //
        // arrow_body_def.type = b2_staticBody;
        // arrow_body_def.position = arrow_pos.to_cartesian_b2();
        // b2ShapeDef arrow_shape_def = b2DefaultShapeDef();
        // b2Polygon arrow_polygon = b2MakeBox(0.4f, 2.4f);
        //
        //b2BodyId arrow_id;
        // b2CreatePolygonShape(arrow_id, &arrow_shape_def, &arrow_polygon);
        arrows.emplace_back(&arrow_texture);
        arrow = arrows.back();

    }

    const Vector2 arrow_relative_pos = Vector2p{5, arrow._graph_rotation_d - 90}.to_cartesian();

    arrow._graph_position = Vector2{ball_pos.x + arrow_relative_pos.x, ball_pos.y + arrow_relative_pos.y};


    arrow.draw();

    //auto arrow_pos = to_polar(b2Body_GetPosition(arrow._body_id));
    if (IsKeyDown(KEY_A)) {
        // b2Body_SetTransform(arrow._body_id, (arrow_pos + Vector2p{0, -paddle_speed / 10}).to_cartesian_b2(), b2MakeRot(DEG2RAD * (arrow_pos.ang_d + paddle_speed)));
        arrow._graph_rotation_d -= arrow_speed;
        if (arrow._graph_rotation_d <= -45) arrow._graph_rotation_d = -45;
    }
    if (IsKeyDown(KEY_D)) {
        // b2Body_SetTransform(arrow._body_id, (arrow_pos + Vector2p{0, paddle_speed / 10}).to_cartesian_b2(), b2MakeRot(DEG2RAD * (arrow_pos.ang_d - paddle_speed)));
        arrow._graph_rotation_d += arrow_speed;
        if (arrow._graph_rotation_d >= 45) arrow._graph_rotation_d = 45;
    }
    if (IsKeyDown(KEY_SPACE)) {
        b2Body_SetLinearVelocity(ball._body_id, b2Vec2{-std::cosf(DEG2RAD * (arrow._graph_rotation_d + 90.0f)), -std::sinf(DEG2RAD * (arrow._graph_rotation_d + 90.0f))});
        //std::cout << arrow._graph_rotation_d;
        //b2Body_Disable(arrow._body_id);
        //b2Body_SetTransform(arrow._body_id, {100, 100}, b2MakeRot(0));
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


