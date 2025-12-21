#ifndef PADDLE_H
#define PADDLE_H

#include "game.h"
#include "raylib.h"

inline constexpr Vector2 paddle_size = { 3.0f, 1.0f };
inline float PADDLE_SPEED = 1.0f;

inline Vector2 pp_cart;

inline Body paddle;

inline int lives;

inline Vector2p paddle_pos = {0, 0};

void spawn_paddle();
void update_paddle(float move_angle_deg);
// bool is_colliding_with_paddle(Vector2 pos, Vector2 size);

#endif // PADDLE_H
