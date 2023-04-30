#include "main.h"
#include <vector>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include "include/core/SkFontMetrics.h"

using namespace std;

std::vector<std::string> split(const std::string& target, char c)
{
	std::string temp;
	std::stringstream stringstream { target };
	std::vector<std::string> result;

	while (std::getline(stringstream, temp, c)) {
		result.push_back(temp);
	}

	return result;
}


Text::Text(string _text, float _x, float _y, int _rot, int _mirror, float _size) : text(_text), x(_x), y(_y), rot(_rot), mirror(_mirror), size(_size) {}

void Text::draw(SkCanvas* canvas, SkPaint &paint, DrawContext &ctx){
    canvas->save();
    paint.setStrokeWidth(0.25);
    

    ctx.font.setSize(size*50);
    SkFontMetrics metrics;
    ctx.font.getMetrics(&metrics);


    auto texts = split(text,'\n');

    float max_width=0;
    for(int i=0; i < texts.size(); i++){
        max_width = std::max(max_width,ctx.font.measureText(texts[i].c_str(), texts[i].size(), SkTextEncoding::kUTF8));
    }
    float twidth = max_width;
    float theight = ctx.font.getSpacing() * texts.size();


    canvas->translate(x,y);
    canvas->rotate(rot*90);
    if(mirror)
        canvas->scale(-1,1);

    SkMatrix cmat = canvas->getLocalToDeviceAs3x3();
    SkMatrix to_world =  ctx.inverse_view_mat * cmat;
   // to_world.dump();   
    SkPoint text_points[4] = {{0,0},{twidth,0},{0,theight},{twidth,theight}};

    if(false){
        SkColor colorSave = paint.getColor();
        SkRect rect = SkRect::MakeLTRB(text_points[0].x(), text_points[0].y(), text_points[3].x(),text_points[3].y());
        paint.setColor(SkColorSetARGB(40,255,255,0));
        canvas->drawRect(rect,paint);
    }
    to_world.mapPoints(text_points,text_points,4);

    canvas->save();
    canvas->setMatrix(ctx.view_mat);
	canvas->drawLine(text_points[0].x()-2, text_points[0].y(), text_points[0].x()+2, text_points[0].y(), paint);
	canvas->drawLine(text_points[0].x(), text_points[0].y()-2, text_points[0].x(), text_points[0].y()+2, paint);

    SkColor colorSave = paint.getColor();
    SkRect rect = SkRect::MakeLTRB(text_points[0].x(), text_points[0].y(), text_points[3].x(),text_points[3].y());
    paint.setColor(SkColorSetARGB(40,0,255,0));
    canvas->drawRect(rect,paint);

    paint.setColor(colorSave);
    bool isVert = abs(text_points[1].y() - text_points[0].y())>EPS;

    if(isVert){
        for(int i=0; i < texts.size(); i++){
            canvas->save();
            float tpos = (i+1)*ctx.font.getSpacing() - metrics.fDescent + text_points[0].x();
            float theight = text_points[2].x() - text_points[0].x();
            if(theight < 0)
                tpos += theight;
            canvas->translate(tpos, text_points[0].y());
            canvas->rotate(-90);
            float twidth = text_points[1].y()-text_points[0].y();
            canvas->drawSimpleText(texts[i].c_str(), texts[i].size(), SkTextEncoding::kUTF8, twidth>0?-twidth:0, 0, ctx.font, paint);

            canvas->restore();

        }                
    }else{
        for(int i=0; i < texts.size(); i++){
            canvas->save();
            float tpos = (i+1)*ctx.font.getSpacing() - metrics.fDescent + text_points[0].y();
            float theight = text_points[2].y() - text_points[0].y();
            if(theight < 0)
                tpos += theight;
            canvas->translate(text_points[0].x(), tpos);
            float twidth = text_points[1].x()-text_points[0].x();
            canvas->drawSimpleText(texts[i].c_str(), texts[i].size(), SkTextEncoding::kUTF8, twidth<0?twidth:0, 0, ctx.font, paint);
            canvas->restore();

        }        
    }
    canvas->restore();

//        printf("%f %f %f %f %f %f\n", font.getSize(), font.getSpacing(), metrics.fDescent, metrics.fAscent, metrics.fLeading, (font.getSpacing() - (metrics.fDescent - metrics.fAscent)) / 2);
    canvas->restore();
}
