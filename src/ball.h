#ifndef BALL_H
#define BALL_H

#include "body.h"
#include "raylib.h"

// inline constexpr float ball_launch_vel_mag = 0.15f;
// inline constexpr float ball_launch_angle_degrees = 49.6f;

inline float BALL_SPEED = 15.0f;
// inline constexpr Vector2 ball_size = { 1.0f, 1.0f };
inline Body ball;

inline float ball_radius = 0.8f;
inline Vector2 ball_pos = {0, 0};
inline b2Vec2 ball_init_vel = {0, 0};

void spawn_ball();
void move_ball();
void choose_dir();
bool is_ball_inside_level();

#endif // BALL_H
