#include "main.h"
#include <vector>
#include <stdio.h>
#include <iostream>

using namespace std;

namespace pschem {

Net::Net(float _x1, float _y1, float _x2, float _y2, anydict_t props) : Drawable(props), x1(_x1), y1(_y1), x2(_x2), y2(_y2) {}

void Net::draw(SkPaint &paint, DrawContext &ctx){
    paint.setColor(isSelected(ctx)?ctx.colorMap[COLOR_SEL]:ctx.colorMap[COLOR_NET]);
	ctx.canvas->drawLine(x1, y1, x2, y2, paint);
    paint.setColor(mortonColor(ctx.objId));
    ctx.window->SetId(ctx.objId++,ctx.parent?ctx.parent:this);

	ctx.hitCanvas->drawLine(x1, y1, x2, y2, paint);
}

}; //Namespace pschem
