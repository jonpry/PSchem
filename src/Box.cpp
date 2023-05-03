#include "main.h"
#include <vector>
#include <stdio.h>
#include <iostream>
#include "include/core/SkRRect.h"

using namespace std;


Box::Box(int _layer, float _x1, float _y1, float _x2, float _y2) : layer(_layer), x1(_x1), y1(_y1), x2(_x2), y2(_y2) {}

void Box::draw(SkCanvas* canvas, SkPaint &paint, DrawContext &ctx){
    SkRect rect = SkRect::MakeLTRB(x1,y1,x2,y2);
    canvas->drawRRect(SkRRect::MakeRectXY(rect,0.5,0.5),paint);
}

