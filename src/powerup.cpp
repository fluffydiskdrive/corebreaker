#include "powerup.h"

#include "ball.h"
#include "game.h"

#include <iostream>

powerup::powerup(): _active(false){};
powerup::powerup(const std::function<void(bool active)> effect, const float duration): _effect(effect), time_remaining_s(duration), _active(false)
{
    std::cout << "";
}

void powerup::update_powerup(const float delta)
{
    if (time_remaining_s <= 0.0f) {
        _active = false;
    }
    if (_active) {
        time_remaining_s -= delta;
    }
    _effect(_active);



}

// inline std::function<void()> invincibility_effect()
// {
//     return []() {
//         if (!is_ball_inside_level()) {
//             const auto curr_vel_p = to_polar(b2Body_GetLinearVelocity(ball._body_id));
//             b2Body_SetLinearVelocity(ball._body_id, Vector2p{curr_vel_p.dist, curr_vel_p.ang_d + 180}.to_cartesian_b2());
//         }
//     };
// }