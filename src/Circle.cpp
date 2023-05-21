#include "main.h"
#include <vector>
#include <stdio.h>
#include <iostream>

using namespace std;

namespace pschem {

Arc::Arc(int _layer, float _cx, float _cy, float _rad, float _sa, float _ea, anydict_t props) : Drawable(props), layer(_layer), c(_cx,_cy), rad(_rad), sa(_sa), ea(_ea) {}

void Arc::draw(SkPaint &paint, DrawContext &ctx){
    SkRect rect = SkRect::MakeXYWH(c.m_x - rad, c.m_y - rad, rad*2, rad*2);

    paint.setColor(isSelected(ctx)?ctx.colorMap[COLOR_SEL]:ctx.colorMap[layer]);
	ctx.canvas->drawArc(rect, sa, ea, false, paint);

    paint.setColor(mortonColor(ctx.objId));
    ctx.window->SetId(ctx.objId++,ctx.parent?ctx.parent:this);

	ctx.hitCanvas->drawArc(rect, sa, ea, false, paint);

}

}; //Namespace pschem
