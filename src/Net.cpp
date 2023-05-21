#include "main.h"
#include <vector>
#include <stdio.h>
#include <iostream>

using namespace std;

namespace pschem {

Net::Net(float _x1, float _y1, float _x2, float _y2, anydict_t props) : Drawable(props), p1(_x1,_y1), p2(_x2,_y2) {}

void Net::draw(SkPaint &paint, DrawContext &ctx){
    paint.setColor(isSelected(ctx)?ctx.colorMap[COLOR_SEL]:ctx.colorMap[COLOR_NET]);
	ctx.canvas->drawLine(p1.m_x, p1.m_y, p2.m_x, p2.m_y, paint);
    paint.setColor(mortonColor(ctx.objId));
    ctx.window->SetId(ctx.objId++,ctx.parent?ctx.parent:this);

	ctx.hitCanvas->drawLine(p1.m_x, p1.m_y, p2.m_x, p2.m_y, paint);
}

void Net::move(float dx, float dy){
    p1.m_x += dx; p2.m_x += dx;
    p1.m_y += dy; p2.m_y += dy;
}


}; //Namespace pschem
