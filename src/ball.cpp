#include "ball.h"
#include "level.h"
#include "paddle.h"
#include "assets.h"

#include "raylib.h"

#include <cmath>
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
    ball_pos = Vector2p{paddle_pos.dist - 5, paddle_pos.ang_d}.to_cartesian();
    //ball_pos = Vector2(0.83,-paddle_pos.dist - 20);
    //ball_pos = Vector2(-paddle_pos.dist - 20, 0);
    b2BodyDef ball_body_def = b2DefaultBodyDef();

    ball_body_def.type = b2_dynamicBody;
    ball_body_def.position = {ball_pos.x, ball_pos.y};
    ball_body_def.rotation = b2MakeRot(0.0f);

    auto ball_data = CustomBodyData{COLLISION_BALL};
    ball_body_def.userData = &ball_data;

    b2ShapeDef ball_shape_def = b2DefaultShapeDef();
    ball_shape_def.enableContactEvents = true;
    ball_shape_def.material.restitution = 1.0f;
    b2Circle ball_circle = {b2Vec2{0, 0}, ball_radius};
    
    auto ball_id = b2CreateBody(world_id, &ball_body_def);

    b2CreateCircleShape(ball_id, &ball_shape_def, &ball_circle);
    b2Body_SetBullet(ball_id, true);
    b2Body_SetLinearVelocity(ball_id, Vector2p{-1, 0}.to_cartesian_b2());

    bodies.emplace_back(ball_id, &ball_texture);
    ball = bodies.back();
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

bool is_ball_inside_level()
{
    // return is_inside_level(static_cast<int>(ball_pos.y), static_cast<int>(ball_pos.x));
    return to_polar(ball_pos).dist <= paddle_pos.dist;
}

void contact_ball()
{
    b2ContactEvents ball_contact_events = b2World_GetContactEvents(world_id);
    for (int i = 0; i < ball_contact_events.endCount; ++i) {
        b2ContactEndTouchEvent* end_touch_event = ball_contact_events.endEvents + i;
        if (static_cast<CustomBodyData*>(b2Body_GetUserData(b2Shape_GetBody(end_touch_event->shapeIdB)))->_collision_type == COLLISION_BOX)
        {
            auto curr_body = b2Shape_GetBody(end_touch_event->shapeIdB);
            for (auto j : bodies) {
                if (j._body_id.index1 == curr_body.index1 && j._body_id.generation == curr_body.generation && j._body_id.world0 == curr_body.world0) {
                    j.to_delete = true;
                }
            }
        }
    }

}
