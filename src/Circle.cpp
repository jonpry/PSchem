#include "main.h"
#include <vector>
#include <stdio.h>
#include <iostream>

using namespace std;


Arc::Arc(int _layer, float _cx, float _cy, float _rad, float _sa, float _ea) : layer(_layer), cx(_cx), cy(_cy), rad(_rad), sa(_sa), ea(_ea) {}

void Arc::draw(SkCanvas* canvas, SkPaint &paint, DrawContext &ctx){
    SkRect rect = SkRect::MakeXYWH(cx - rad, cy - rad, rad*2, rad*2);
	canvas->drawArc(rect, sa, ea, false, paint);
}
