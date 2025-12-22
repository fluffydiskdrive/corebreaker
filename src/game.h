#ifndef GAME_H
#define GAME_H

#include "body.h"
#include "powerup.h"

#include <cmath>
#include <cstddef>
#include <random>
#include <vector>

constexpr char VOID = ' ';
constexpr char WALL = '#';
constexpr char BLOCK = '@';
constexpr char PADDLE = 'P';
constexpr char CORE = '!';
constexpr char BALL = '*';
constexpr char BONUS_PADDLE_X4 = '4';
constexpr char BONUS_INVINCIBILITY = '&';

constexpr char type_box[5] = {WALL, BLOCK, CORE, BONUS_INVINCIBILITY, BONUS_PADDLE_X4};

constexpr int BLOCK_SIZE = 16;

inline auto BORDER_COLOR = GRAY;

inline b2WorldDef world_def;
inline b2WorldId world_id;
inline RenderTexture2D render_texture;
inline Vector2 viewport_size;
inline Vector2 viewport_origin;
inline Camera2D camera;

inline std::vector<Body> bodies;
inline std::vector<Body> paddles;

inline bool level_passed;
inline bool boss_attacking;

inline powerup invincibility;
inline powerup paddle_x4;

inline Body core;

inline int blocks_remaining;

constexpr int MAX_LIVES = 5;


struct timer {
    float duration;
    float time;
    bool paused;

    bool time_out;

    timer(){paused = true;}
    timer(const float dur) : duration(dur){paused = true; time = duration; time_out = true;}
    timer(const float dur, const bool paus, const bool out) : duration(dur){paused = paus; time = duration; time_out = out;}

    void update_timer(const float delta)
    {
        if (!paused && !time_out) time += delta;
        if (time >= duration) time_out = true;
    }

    void restart_timer()
    {
        time = 0;
        time_out = false;
        paused = false;
    }
};

inline timer paddle_invincible_timer;
inline timer anim_timer;

inline float norm_ang(float angle_d)
{
    return std::fmod(angle_d, 360);
}


struct RectangleR {
    Vector2 position_;
    float rotation_;

    RectangleR(Vector2 position, float rotation) : position_(position), rotation_(rotation){}
};

inline bool in(const auto& val, const auto& arr)
{
    return std::find(std::begin(arr), std::end(arr), val) != std::end(arr);
}

inline int randint(int min, int max)
{
    static std::mt19937 rng(std::random_device {}());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

struct Vector2p {
    float dist;
    float ang_d;
    float ang_r;

    Vector2p(float distance, float angle_deg) : dist(distance), ang_d(angle_deg), ang_r(angle_deg * DEG2RAD){}

    Vector2 to_cartesian() const
    {
        return Vector2{std::cos(ang_r) * dist, std::sin(ang_r) * dist};
    }

    b2Vec2 to_cartesian_b2() const
    {
        return b2Vec2{std::cos(ang_r) * dist, std::sin(ang_r) * dist};
    }

    Vector2p operator+ (Vector2p other) const
    {
        return Vector2p{dist + other.dist, ang_d + other.ang_d};
    }

    Vector2p operator- (Vector2p other) const
    {
        return Vector2p{dist - other.dist, ang_d - other.ang_d};
    }
};

inline Vector2p to_polar(const Vector2 v2)
{
    return Vector2p{sqrtf(v2.x * v2.x + v2.y * v2.y), RAD2DEG * atan2f(v2.y, v2.x)};
}

inline Vector2p to_polar(const b2Vec2 b2v2)
{
    return Vector2p{sqrtf(b2v2.x * b2v2.x + b2v2.y * b2v2.y), atan2f(b2v2.y, b2v2.x)};
}

inline b2Vec2 V2_to_b2V2(const Vector2 v2)
{
    return b2Vec2{v2.x, v2.y};
}

inline Vector2 b2V2_to_V2(const b2Vec2 b2v2)
{
    return Vector2{b2v2.x, b2v2.y};
}

struct CustomBodyData {
    int _collision_type;
    explicit CustomBodyData(const int type) : _collision_type(type){};
    bool operator==(const CustomBodyData& other) const
    {
        return _collision_type == other._collision_type;
    }
};

//-----------------------------------------V

constexpr float GRAPH_SCALING = 0.1f;




struct level {
    size_t rows = 0, columns = 0;
    std::vector<char>* data = nullptr;
    bool boss_level = false;
};

enum game_state {
    menu_state,
    in_game_state,
    dir_choice_state,
    paused_state,
    victory_state,
    defeat_state
};

enum collision_type : int {
    COLLISION_BOX,
    COLLISION_BALL,
    COLLISION_PADDLE,
    COLLISION_WALL,
    COLLISION_BONUS_PADDLE_X4,
    COLLISION_BONUS_INVINCIBILITY,
    COLLISION_CORE
};

inline std::vector level_1_data = {
    '@',' ',' ',' ','@',' ',' ',' ','@',
    ' ',' ',' ',' ',' ',' ',' ',' ',' ',
    ' ',' ',' ','@','@','@',' ',' ',' ',
    ' ',' ','@','@','@','@','@',' ',' ',
    '#',' ','@','@','!','@','@',' ','@',
    ' ',' ','@','@','@','@','@',' ',' ',
    ' ',' ',' ','@','@','#',' ',' ',' ',
    ' ',' ',' ',' ',' ',' ',' ',' ',' ',
    '4',' ',' ',' ',' ',' ',' ',' ','&'
};

inline level level_1 = {
    9, 9,
    &level_1_data
};

inline std::vector level_2_data = {
    '@',' ',' ',' ','@',' ',' ',' ','@',
    '@',' ',' ',' ',' ',' ',' ',' ',' ',
    ' ',' ',' ','@','@','@',' ',' ',' ',
    ' ',' ','@','@','@','@','@',' ',' ',
    '@',' ','@','@','!','@','@',' ','@',
    ' ',' ','@','@','@','@','@',' ',' ',
    ' ',' ',' ','@','@','@',' ',' ',' ',
    ' ',' ',' ',' ',' ',' ',' ',' ','@',
    '@',' ',' ',' ','@',' ',' ',' ','@'
};
inline level level_2 = {
    9, 9,
    &level_2_data
};

inline std::vector level_3_data = {
    '@',' ',' ',' ','@',' ',' ',' ','@',
    '@',' ',' ',' ',' ',' ',' ',' ',' ',
    '@',' ',' ','@','@','@',' ',' ',' ',
    ' ',' ','@','@','@','@','@',' ',' ',
    '@',' ','@','@','!','@','@',' ','@',
    ' ',' ','@','@','@','@','@',' ',' ',
    ' ',' ',' ','@','@','@',' ',' ','@',
    ' ',' ',' ',' ',' ',' ',' ',' ','@',
    '@',' ',' ',' ','@',' ',' ',' ','@'
};
inline level level_3 = {
    9, 9,
    &level_3_data
};

inline std::vector level_4_data = {
    '@',' ',' ',' ','@',' ',' ',' ','@',
    '@',' ',' ',' ',' ',' ',' ',' ',' ',
    '@',' ',' ','@','@','@',' ',' ',' ',
    '@',' ','@','@','@','@','@',' ',' ',
    '@',' ','@','@','!','@','@',' ','@',
    ' ',' ','@','@','@','@','@',' ','@',
    ' ',' ',' ','@','@','@',' ',' ','@',
    ' ',' ',' ',' ',' ',' ',' ',' ','@',
    '@',' ',' ',' ','@',' ',' ',' ','@'
};
inline level level_4 = {
    9, 9,
    &level_4_data
};

inline std::vector level_5_data = {
    ' ',' ',' ',' ',' ',' ',' ',' ',' ',
    ' ',' ',' ',' ',' ',' ',' ',' ',' ',
    ' ',' ','&',' ','@',' ','4',' ',' ',
    ' ',' ',' ','@',' ','#',' ',' ',' ',
    ' ',' ','@',' ','!',' ','@',' ',' ',
    ' ',' ',' ','#',' ','@',' ',' ',' ',
    ' ',' ','4',' ','@',' ','&',' ',' ',
    ' ',' ',' ',' ',' ',' ',' ',' ',' ',
    ' ',' ',' ',' ',' ',' ',' ',' ',' ',
};
inline level level_5 = {
    9, 9,
    &level_5_data,
    true
};

inline constexpr size_t level_count = 1;
inline level levels[level_count] = {
    level_1,
    // level_2, level_3, level_4,
    //level_5
};

inline game_state game_state = menu_state;

#endif // GAME_H
