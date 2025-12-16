#ifndef BODY_H
#define BODY_H

#include "box2d/box2d.h"
#include "box2d/types.h"
#include "raylib.h"

struct Body
{
    b2BodyId _body_id;

    bool to_delete;

    Vector2 _graph_position;
    Texture2D *_tex;

    Body();
    Body(b2BodyId body_id, Texture2D *tex);


    void draw() const;
};

#endif // BODY_H
