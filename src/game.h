#ifndef GAME_H
#define GAME_H

#include "body.h"

#include <cmath>
#include <cstddef>
#include <vector>

constexpr char VOID = ' ';
constexpr char WALL = '#';
constexpr char BLOCK = '@';
constexpr char PADDLE = 'P';
constexpr char CORE = '!';
constexpr char BALL = '*';

constexpr int BLOCK_SIZE = 16;

inline b2WorldDef world_def;
inline b2WorldId world_id;

inline std::vector<Body> bodies;


struct RectangleR {
    Vector2 position_;
    float rotation_;

    RectangleR(Vector2 position, float rotation) : position_(position), rotation_(rotation){}
};

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
};

inline Vector2p to_polar(const Vector2 v2)
{
    return Vector2p{sqrtf(v2.x * v2.x + v2.y * v2.y), atan2f(v2.y, v2.x)};
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
    char* data = nullptr;
};

enum game_state {
    menu_state,
    in_game_state,
    paused_state,
    victory_state
};

enum collision_type : int {
    COLLISION_BOX = 0,
    COLLISION_BALL = 1,
    COLLISION_PADDLE = 2,
    COLLISION_WALL = 3,
    COLLISION_BONUS = 4,
};

inline char level_1_data[] = {
    '@',' ',' ',' ','@',' ',' ',' ','@',
    ' ',' ',' ',' ',' ',' ',' ',' ',' ',
    ' ',' ',' ','@','@','@',' ',' ',' ',
    ' ',' ','@','@','@','@','@',' ',' ',
    '@',' ','@','@','!','@','@',' ','@',
    ' ',' ','@','@','@','@','@',' ',' ',
    ' ',' ',' ','@','@','@',' ',' ',' ',
    ' ',' ',' ',' ',' ',' ',' ',' ',' ',
    '@',' ',' ',' ','@',' ',' ',' ','@'
};

inline level level_1 = {
    9, 9,
    level_1_data
};

inline char level_2_data[] = {
    '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#',
    '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#',
    '#', ' ', '@', ' ', ' ', ' ', '@', ' ', ' ', ' ', '@', ' ', '#',
    '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#',
    '#', ' ', ' ', ' ', '#', ' ', ' ', ' ', '#', ' ', ' ', ' ', '#',
    '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#',
    '#', ' ', '@', ' ', ' ', ' ', '@', ' ', ' ', ' ', '@', ' ', '#',
    '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#',
    '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#',
    '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#',
    '#', ' ', '@', ' ', ' ', ' ', '*', ' ', ' ', ' ', '@', ' ', '#',
    '#', ' ', ' ', ' ', ' ', 'P', ' ', ' ', ' ', ' ', ' ', ' ', '#',
    '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#'
};
inline level level_2 = {
    13, 13,
    level_2_data
};

inline constexpr size_t level_count = 2;
inline level levels[level_count] = {
    level_1, level_2
};

inline game_state game_state = menu_state;

#endif // GAME_H
