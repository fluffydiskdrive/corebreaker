#include "body.h"
#include "game.h"
#include <cmath>

Body::Body(){}
Body::Body(b2BodyId body_id, Texture2D *tex) : _body_id(body_id), _tex(tex), _to_delete(false)
{
    _graph_position = _tex ? Vector2 {b2Body_GetPosition(_body_id).x * GRAPH_SCALING, b2Body_GetPosition(_body_id).y * GRAPH_SCALING} : Vector2 {0, 0};
    _graph_rotation_d = 0;
    _to_draw = true;
    //_is_enabled = b2Body_IsEnabled(_body_id);
}

Body::Body(Texture2D *tex) : _tex(tex), _to_delete(false)
{
    _to_draw = true;
    _graph_position = {0, 0};
    _graph_rotation_d = 0;
}

void Body::draw() const
{
    //b2ShapeId shapes[5];
    // if (_body_id) {
    //b2Body_GetShapes(_body_id, shapes, 1);
    if (_to_draw){
        const auto dest = Rectangle{_graph_position.x, _graph_position.y, _tex->width * GRAPH_SCALING, _tex->height * GRAPH_SCALING};
        DrawTexturePro(*_tex,
            Rectangle{0, 0, static_cast<float>(_tex->width), static_cast<float>(_tex->height)},
            dest,
            {dest.width / 2.0f, dest.height / 2.0f},
            !b2Body_IsValid(_body_id) ? _graph_rotation_d : RAD2DEG * b2Rot_GetAngle(b2Body_GetRotation(_body_id)),
            WHITE);
    }
    //}
}