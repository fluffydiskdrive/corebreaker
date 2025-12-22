#include "level.h"
#include "assets.h"
// #include "ball.h"
#include "game.h"
#include "graphics.h"
// #include "paddle.h"

#include "ball.h"
#include "paddle.h"
#include "powerup.h"
#include "raylib.h"

#include <vector>

timer boss_attack_recharge_timer;
timer boss_attack_timer;
timer core_flash_timer;

float base_angle_start;
float base_angle_end;
float base_angle;

std::vector<char> curr_level_data;

Body init_box(Vector2 position, CustomBodyData* box_data, Texture2D* tex)
{

    b2BodyDef box_def = b2DefaultBodyDef();

    box_def.type = b2_staticBody;
    box_def.position = V2_to_b2V2(position);
    box_def.rotation = b2MakeRot(0.0f);
    box_def.userData = box_data;

    b2ShapeDef box_shape_def = b2DefaultShapeDef();
    box_shape_def.enableContactEvents = true;
    const b2Polygon box_polygon = b2MakeBox(0.8f, 0.8f);

    const auto box_id = b2CreateBody(world_id, &box_def);
    b2CreatePolygonShape(box_id, &box_shape_def, &box_polygon);
    auto box = Body { box_id, tex };
    box._graph_position = b2V2_to_V2(b2Body_GetPosition(box_id));
    return box;
}

void load_level(const int offset, bool new_init)
{
    current_level_index += offset;
    if (current_level_index >= level_count) {
        game_state = victory_state;
        ClearBackground(BLACK);
        init_state_menu();
        current_level_index = 0;
        // unload_level();
        return;
    }
    if (new_init || offset) {
        if (levels[current_level_index].boss_level)
            core_max_hp = 9;
        else
            core_max_hp = 1;
        lives = MAX_LIVES;
        core_hp = core_max_hp;
        if (curr_level_data.size())
            curr_level_data.clear();
        // for (int i = 0; i < levels[current_level_index].rows * levels[current_level_index].columns; ++i) curr_level_data->push_back(levels[current_level_index].data->[i]);
        curr_level_data = *levels[current_level_index].data;
    }
    if (current_level) {
        unload_level();
    }

    // core_max_hp = 1;
    BALL_SPEED = 15.0f;
    PADDLE_SPEED = 1.25f;
    world_def = b2DefaultWorldDef();
    world_def.gravity = { 0, 0 };
    world_id = b2CreateWorld(&world_def);

    if (levels[current_level_index].boss_level) {
        boss_attack_recharge_timer = timer { 5.0f };
        boss_attack_recharge_timer.restart_timer();
        boss_attack_timer = timer { 3.0f };
        paddle_invincible_timer = timer { 3.0f };
        core_flash_timer = timer { 1.0f };
        base_angle_start = randint(0, 180);
        base_angle_end = base_angle_start + randint(10, 40);
        base_angle = base_angle_start;
        core_max_hp = 9;
    }

    const size_t rows = levels[current_level_index].rows;
    const size_t columns = levels[current_level_index].columns;

    paddle_pos.dist = static_cast<float>(3.0f * sqrt((rows / 2) * (rows / 2) + (columns / 2) * (columns / 2)));
    blocks_remaining = 0;

    current_level_blocks = 0;
    for (int row = 0; row < rows; ++row) {
        for (int column = 0; column < columns; ++column) {

            if (in((curr_level_data)[row * columns + column], type_box)) {
                CustomBodyData* box_data;
                Texture2D* tex;

                switch ((curr_level_data)[row * columns + column]) {
                case BLOCK: {
                    box_data = new CustomBodyData { COLLISION_BOX };
                    tex = &block_texture;
                    ++blocks_remaining;
                    break;
                }
                case CORE: {
                    box_data = new CustomBodyData { COLLISION_CORE };
                    tex = &core_texture;
                    break;
                }
                case BONUS_INVINCIBILITY: {
                    box_data = new CustomBodyData { COLLISION_BONUS_INVINCIBILITY };
                    tex = &invincibility_bonus_texture;
                    break;
                }
                case BONUS_PADDLE_X4: {
                    box_data = new CustomBodyData { COLLISION_BONUS_PADDLE_X4 };
                    tex = &paddle_x4_bonus_texture;
                    break;
                }
                case WALL: {
                    box_data = new CustomBodyData { COLLISION_WALL };
                    tex = &wall_texture;
                    break;
                }
                }

                auto box = init_box({ ((static_cast<float>(columns) / 2) - column) * 2 - 1.0f, ((static_cast<float>(rows) / -2) + row) * 2 + 1.0f }, box_data, tex);
                box.map_index = row * columns + column;
                boxes.push_back(box);
                // bodies.push_back(box);
                ++current_level_blocks;
            }
        }
    }
    current_level = new level { rows, columns, &curr_level_data, levels[current_level_index].boss_level };

    spawn_paddle();
    spawn_ball();

    derive_graphics_metrics();
}

void unload_level()
{
    b2DestroyWorld(world_id);
    // delete current_level->data;
    delete current_level;
    bodies.clear();
    paddles.clear();
    paddle_pos = { 0, 0 };
    ball_pos = { 0, 0 };
    boxes.clear();
    current_level = nullptr;
    // world_id = b2CreateWorld(&world_def);
}

bool is_inside_level(const int row, const int column)
{
    return row >= 0 && row < current_level->rows && column >= 0 && column < current_level->columns;
}

char& get_level_cell(const size_t row, const size_t column)
{
    return (*current_level->data)[row * current_level->columns + column];
}

void set_level_cell(const size_t row, const size_t column, const char cell)
{
    get_level_cell(row, column) = cell;
}

void level_draw()
{
    for (const Body& i : boxes) {
        i.draw();
    }
}

void destroy_boxes()
{
    for (auto i = boxes.begin(); i != boxes.end();) {
        if (i->_to_delete) {
            (curr_level_data)[i->map_index] = ' ';
            b2DestroyBody(i->_body_id);
            --current_level_blocks;
            i = boxes.erase(i);
        } else
            ++i;
    }
    if (blocks_remaining <= 0)
        level_passed = true;
}

void for_box(const b2ContactEndTouchEvent* end_touch_event)
{
    auto curr_body = b2Shape_GetBody(end_touch_event->shapeIdA);
    for (auto& j : boxes) {
        if (j._body_id.index1 == curr_body.index1 && j._body_id.generation == curr_body.generation && j._body_id.world0 == curr_body.world0) {
            j._to_delete = true;
        }
    }
    BALL_SPEED += 0.45f;
    PADDLE_SPEED += 0.055f;
    --blocks_remaining;
}

void contact_ball()
{

    b2ContactEvents ball_contact_events = b2World_GetContactEvents(world_id);
    for (int i = 0; i < ball_contact_events.endCount; ++i) {
        b2ContactEndTouchEvent* end_touch_event = ball_contact_events.endEvents + i;

        switch (static_cast<CustomBodyData*>(b2Body_GetUserData(b2Shape_GetBody(end_touch_event->shapeIdA)))->_collision_type) {
        case COLLISION_BONUS_PADDLE_X4:
            PlaySound(powerup_sound);
            paddle_x4._active = true;
            for_box(end_touch_event);
            break;
        case COLLISION_BOX: {
            for_box(end_touch_event);
            // paddle_x2.active = true;
            break;
        }
        case COLLISION_CORE: {
            --core_hp;
            if (core_hp <= 0) {
                level_passed = true;
                // load_level(1);
            }
            core_flash_timer.restart_timer();
            break;
        }
        case COLLISION_WALL:
            break;
        case COLLISION_BONUS_INVINCIBILITY:
            PlaySound(powerup_sound);
            invincibility._active = true;
            for_box(end_touch_event);
            break;
        }
    }
}

void boss_attack(float delta)
{

    if (base_angle == base_angle_start)
        boss_attack_timer.restart_timer();
    // base_angle = base_angle_start + (boss_attack_timer.time + 0.01) / boss_attack_timer.duration * (base_angle_end - base_angle_start);
    base_angle += (base_angle_end / boss_attack_timer.duration * delta);
    if (!IsSoundPlaying(laser_sound))
        PlaySound(laser_sound);

    attack_draw();
    if (((norm_ang(paddle_pos.ang_d) <= norm_ang(base_angle + 8.0f) && norm_ang(paddle_pos.ang_d) >= norm_ang(base_angle - 8.0f))
            || (norm_ang(paddle_pos.ang_d) <= norm_ang(base_angle + 188.0f) && norm_ang(paddle_pos.ang_d) >= norm_ang(base_angle + 172.0f)))
        && paddle_invincible_timer.time_out) {
        --lives;
        paddle_invincible_timer.restart_timer();
        PlaySound(death_sound);
    }

    if (boss_attack_timer.time_out) {
        StopSound(laser_sound);
        boss_attack_recharge_timer.restart_timer();
        base_angle_start = randint(0, 180);
        base_angle_end = base_angle_start + randint(10, 40);
        base_angle = base_angle_start;
    }
    base_angle = norm_ang(base_angle);
}

constexpr float paddle_flash_interval = 0.1f;
float paddle_flash_prev_time = 0.0f;
auto flash_timer = timer { paddle_flash_interval, false, false };

void update_timers(float delta)
{
    boss_attack_recharge_timer.update_timer(delta);
    paddle_invincible_timer.update_timer(delta);
    boss_attack_timer.update_timer(delta);
    flash_timer.update_timer(delta);
    core_flash_timer.update_timer(delta);
}

void update_level(float delta)
{
    contact_ball();
    destroy_boxes();
    if (current_level->boss_level) {
        if (IsMusicStreamPlaying(main_theme)) {
            PauseMusicStream(main_theme);
            ResumeMusicStream(boss_theme);
        }
        update_timers(delta);

        if (flash_timer.time_out) {
            if (!paddle_invincible_timer.time_out) {
                for (int i = 0; i < (paddle_x4._active ? paddles.size() : 1); ++i) {
                    paddles[i]._to_draw = !paddles[i]._to_draw;
                }
            } else {
                if (!paddles[0]._to_draw)
                    paddles[0]._to_draw = true;
            }
            if (!core_flash_timer.time_out) {
                for (auto& i : boxes) {
                    if (static_cast<CustomBodyData*>(b2Body_GetUserData(i._body_id))->_collision_type == COLLISION_CORE)
                        i._to_draw = !i._to_draw;
                }

            } else {
                for (auto& i : boxes) {
                    if (static_cast<CustomBodyData*>(b2Body_GetUserData(i._body_id))->_collision_type == COLLISION_CORE)
                        if (!i._to_draw)
                            i._to_draw = true;
                }
            }
            flash_timer.restart_timer();
        }

        if (boss_attack_recharge_timer.time_out) {
            boss_attack(delta);
        }
        // DrawTextPro(menu_font, std::to_string(boss_attack_recharge_timer.time).c_str(), {-30.0f, -20}, {0,0}, 0, 5.0f, 0.2f, WHITE);
        // DrawTextPro(menu_font, std::to_string(core_flash_timer.time).c_str(), { -30.0f, -10.0f }, { 0, 0 }, 0, 5.0f, 0.2f, WHITE);
        // DrawTextPro(menu_font, std::to_string(paddle_invincible_timer.time).c_str(), { -30.0f, 0.0f }, { 0, 0 }, 0, 5.0f, 0.2f, WHITE);
        // DrawTextPro(menu_font, std::to_string(flash_timer.time).c_str(), { -30.0f, 10.0f }, { 0, 0 }, 0, 5.0f, 0.2f, WHITE);
    } else {
        if (IsMusicStreamPlaying(boss_theme)) {
            PauseMusicStream(boss_theme);
            ResumeMusicStream(main_theme);
        }
    }
}

void attack_draw()
{
    const float distance = paddle_pos.dist;
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; distance - j * 1.6 > 3.2; ++j) {
            Vector2 target_pos = Vector2p(j * 16 * GRAPH_SCALING, base_angle + 180.0f * i).to_cartesian();
            target_pos = { target_pos.x + std::cosf((180.0f * i + 90.0f + base_angle) * DEG2RAD) * laser_body_texture.width / 2.0f * GRAPH_SCALING, target_pos.y + std::sinf((180.0f * i + 90.0f + base_angle) * DEG2RAD) * laser_body_texture.width / 2.0f * GRAPH_SCALING };
            DrawTexturePro(
                laser_body_texture,
                Rectangle(0, 0, laser_body_texture.width, laser_body_texture.height),
                Rectangle(target_pos.x, target_pos.y, laser_body_texture.width * GRAPH_SCALING, laser_body_texture.height * GRAPH_SCALING),
                { 0, 0 },
                base_angle + 90.0f + 180.0f * (i + 1),
                WHITE);
        }
        Vector2 target_pos = Vector2p(distance - 3.2f, base_angle + 180.0f * i).to_cartesian();
        target_pos = { target_pos.x + std::cosf((180.0f * i + 90 + base_angle) * DEG2RAD) * laser_end_texture.width / 2.0f * GRAPH_SCALING, target_pos.y + std::sinf((180.0f * i + 90.0f + base_angle) * DEG2RAD) * laser_end_texture.width / 2.0f * GRAPH_SCALING };
        DrawTexturePro(
            laser_end_texture,
            Rectangle(0, 0, laser_end_texture.width, laser_end_texture.height),
            Rectangle(target_pos.x, target_pos.y, laser_end_texture.width * GRAPH_SCALING, laser_end_texture.height * GRAPH_SCALING),
            { 0, 0 },
            base_angle + 90.0f + 180.0f * (i + 1),
            WHITE);
    }
}