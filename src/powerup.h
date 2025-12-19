#ifndef POWERUP_H
#define POWERUP_H
#include "ball.h"


#include <functional>
struct powerup {
    float time_remaining_s;
    bool _active;
    std::function<void(bool active)> _effect;

    powerup();
    powerup(const std::function<void(bool active)> effect, float duration);

    void update_powerup(float delta);
    //static void init_powerups();
};

#endif // POWERUP_H
