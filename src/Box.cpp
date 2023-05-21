#include "main.h"
#include <vector>
#include <stdio.h>
#include <iostream>
#include "include/core/SkRRect.h"

using namespace std;

namespace pschem {

Box::Box(int _layer, float _x1, float _y1, float _x2, float _y2, anydict_t props) : Drawable(props), layer(_layer), p1(_x1,_y1), p2(_x2,_y2) {}

void Box::draw(SkPaint &paint, DrawContext &ctx){
    SkRect rect = SkRect::MakeLTRB(p1.m_x,p1.m_y,p2.m_x,p2.m_y);
    SkRRect rrect = SkRRect::MakeRectXY(rect,0.5,0.5);
    paint.setColor(isSelected(ctx)?ctx.colorMap[COLOR_SEL]:ctx.colorMap[layer]);
    ctx.canvas->drawRRect(rrect,paint);
    paint.setColor(mortonColor(ctx.objId));
    ctx.window->SetId(ctx.objId++,ctx.parent?ctx.parent:this);
    ctx.hitCanvas->drawRRect(rrect,paint);

}

}; //Namespace pschem
