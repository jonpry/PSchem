#include "main.h"
#include <vector>
#include <stdio.h>
#include <iostream>

using namespace std;

namespace pschem {

Line::Line(int _layer, float _x1, float _y1, float _x2, float _y2, anydict_t props) : Drawable(props), layer(_layer), x1(_x1), y1(_y1), x2(_x2), y2(_y2) {}

void Line::draw(SkPaint &paint, DrawContext &ctx){
    paint.setColor(ctx.objId == ctx.selectedId?ctx.colorMap[COLOR_SEL]:ctx.colorMap[layer]);
	ctx.canvas->drawLine(x1, y1, x2, y2, paint);
    paint.setColor(mortonColor(ctx.objId++));
	ctx.hitCanvas->drawLine(x1, y1, x2, y2, paint);
}

}; //Namespace pschem
