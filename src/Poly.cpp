#include "main.h"
#include <vector>
#include <stdio.h>
#include <iostream>

#include "include/core/SkPath.h"

using namespace std;


Poly::Poly(int _layer, int npoints) : layer(_layer) {
    xs.resize(npoints);
    ys.resize(npoints);
}

void Poly::draw(SkCanvas* canvas, SkPaint &paint, DrawContext &ctx){
    SkPath path;
    path.moveTo(xs[0],ys[0]);
    for(int i=1; i < xs.size(); i++){
        path.lineTo(xs[i],ys[i]);
    }
    canvas->drawPath(path,paint);
}
