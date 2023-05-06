#include "main.h"
#include <vector>
#include <stdio.h>
#include <iostream>
#include "include/core/SkRRect.h"

using namespace std;

namespace pschem {

Box::Box(int _layer, float _x1, float _y1, float _x2, float _y2, anydict_t props) : Drawable(props), layer(_layer), x1(_x1), y1(_y1), x2(_x2), y2(_y2) {}

void Box::draw(SkPaint &paint, DrawContext &ctx){
    SkRect rect = SkRect::MakeLTRB(x1,y1,x2,y2);
    ctx.canvas->drawRRect(SkRRect::MakeRectXY(rect,0.5,0.5),paint);
}

}; //Namespace pschem
