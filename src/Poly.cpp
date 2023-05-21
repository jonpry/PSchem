#include "main.h"
#include <vector>
#include <stdio.h>
#include <iostream>

#include "include/core/SkPath.h"

using namespace std;

namespace pschem {

Poly::Poly(int _layer, int npoints, anydict_t props) : Drawable(props), layer(_layer) {
    pts.resize(npoints);
    if(m_props.find("fill") != m_props.end())
        m_fill = std::any_cast<bool>(m_props["fill"]);
}

void Poly::draw(SkPaint &paint, DrawContext &ctx){
    SkPath path;
    path.moveTo(pts[0].m_x,pts[0].m_y);
    for(int i=1; i < pts.size(); i++){
        path.lineTo(pts[i].m_x,pts[i].m_y);
    }
    
    paint.setColor(isSelected(ctx)?ctx.colorMap[COLOR_SEL]:ctx.colorMap[layer]);
    paint.setStyle(m_fill?SkPaint::Style::kFill_Style:SkPaint::Style::kStroke_Style);

    ctx.canvas->drawPath(path,paint);

    paint.setColor(mortonColor(ctx.objId));
    ctx.window->SetId(ctx.objId++,ctx.parent?ctx.parent:this);

    ctx.hitCanvas->drawPath(path,paint);
}

}; //Namespace pschem
