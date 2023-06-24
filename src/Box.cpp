#include "main.h"
#include <vector>
#include <stdio.h>
#include <iostream>
#include "include/core/SkRRect.h"
#include "include/core/SkData.h"
#include "include/core/SkImage.h"
#include "include/utils/SkBase64.h"

using namespace std;

namespace pschem {

Box::Box(int _layer, float _x1, float _y1, float _x2, float _y2, anydict_t props) : Drawable(props), layer(_layer), p1(_x1,_y1), p2(_x2,_y2) {
    alpha = 1.0f;
    if(props.find("alpha") != props.end())
        alpha = (float)std::any_cast<float>(props["alpha"]);
    if(props.find("image") != props.end())
        image = (string)std::any_cast<string>(props["image"]);
    if(props.find("image_data") != props.end()){
        string b64 = (string)std::any_cast<string>(props["image_data"]);  
        size_t sz=0;
        SkBase64::Decode(b64.c_str(),b64.size(),0,&sz);
        void *png_data = malloc(sz);
        SkBase64::Decode(b64.c_str(),b64.size(),png_data,&sz);

        auto skdata = SkData::MakeFromMalloc(png_data,sz);
        printf("Loading image\n");
        sk_image = SkImages::DeferredFromEncodedData(skdata);
    }
}

void Box::draw(SkPaint &paint, DrawContext &ctx){
    SkRect rect = SkRect::MakeLTRB(p1.m_x,p1.m_y,p2.m_x,p2.m_y);
    SkRRect rrect = SkRRect::MakeRectXY(rect,0.5,0.5);
    paint.setColor(isSelected(ctx)?ctx.colorMap[COLOR_SEL]:ctx.colorMap[layer]);
    if(sk_image){
        SkSamplingOptions so;
        paint.setAlphaf(alpha);
        ctx.canvas->drawImageRect(sk_image,rect,so,&paint);
        paint.setAlphaf(1.0f);
    } else {
        ctx.canvas->drawRRect(rrect,paint);
    }
    paint.setColor(mortonColor(ctx.objId));
    ctx.window->SetId(ctx.objId++,ctx.parent?ctx.parent:this);
    ctx.hitCanvas->drawRRect(rrect,paint);

}

}; //Namespace pschem
