#include "main.h"
#include <vector>
#include <stdio.h>
#include <iostream>

#include "include/core/SkPath.h"

using namespace std;

namespace pschem {

Poly::Poly(int _layer, int npoints, anydict_t props) : Drawable(props), layer(_layer) {
    xs.resize(npoints);
    ys.resize(npoints);
    if(m_props.find("fill") != m_props.end())
        m_fill = std::any_cast<bool>(m_props["fill"]);
}

void Poly::draw(SkCanvas* canvas, SkPaint &paint, DrawContext &ctx){
    SkPath path;
    path.moveTo(xs[0],ys[0]);
    for(int i=1; i < xs.size(); i++){
        path.lineTo(xs[i],ys[i]);
    }
    
    paint.setStyle(m_fill?SkPaint::Style::kFill_Style:SkPaint::Style::kStroke_Style);

    canvas->drawPath(path,paint);
}

}; //Namespace pschem
