#ifndef LEVEL_H
#define LEVEL_H

#include "game.h"

#include "box2d/box2d.h"
#include "raylib.h"

#include "box2d/types.h"

#include <cstddef>

inline level* current_level;
inline size_t current_level_blocks;
inline size_t current_level_index = 0;
inline std::vector<Body> boxes;

void load_level(int offset = 0);
void unload_level();

bool is_inside_level(int row, int column);

char& get_level_cell(size_t row, size_t column);
void set_level_cell(size_t row, size_t column, char cell);
void level_draw();
void attack_draw();
void update_level(float delta);
// bool is_colliding_with_level_cell(Vector2 pos, Vector2 size, char cell = '#');
// char& get_colliding_level_cell(Vector2 pos, Vector2 size, char look_for);

#endif // LEVEL_H
