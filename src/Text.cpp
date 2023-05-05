#include "main.h"
#include <vector>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <any>
#include "include/core/SkFontMetrics.h"

using namespace std;

static std::string toString(std::any any){
    if(any.type() == typeid(std::string))
        return std::any_cast<string>(any);
    if(any.type() == typeid(float))
        return std::to_string(std::any_cast<float>(any));
    if(any.type() == typeid(int))
        return std::to_string(std::any_cast<int>(any));
    return "other";
}

static std::string replace(std::string &input, anydict_t &props){
    vector<char> ret, key;
    for(int i=0; i < input.size(); i++){
        if(input[i] != '@'){
            ret.push_back(input[i]);
            continue;
        }
        for(i=i+1; i <= input.size(); i++){
            if(i == input.size() || input[i] == ' '){
                string skey = string(key.begin(),key.end());
                if(props.find(skey) != props.end()){
                    string v = toString(props[skey]);
                    ret.insert(ret.end(),v.begin(),v.end());
                }else{
                    ret.push_back('@');
                    ret.insert(ret.end(),key.begin(),key.end());
                }
                key.clear();
                if(i != input.size() - 1)
                    ret.push_back(' ');
                break;
            }
            key.push_back(input[i]);
        }
    }
    return string(ret.begin(),ret.end());
}

static std::vector<std::string> split(const std::string& target, char c, anydict_t &props)
{
	std::string temp;
	std::stringstream stringstream { target };
	std::vector<std::string> result;

	while (std::getline(stringstream, temp, c)) {
	    temp = replace(temp, props);
		result.push_back(temp);
	}

	return result;
}


Text::Text(string _text, float _x, float _y, int _rot, int _mirror, float _size, anydict_t props) : Drawable(props), text(_text), x(_x), y(_y), rot(_rot), mirror(_mirror), size(_size) {
    layer = COLOR_TEXT;
    if(props.find("layer") != props.end())
        layer = (int)std::any_cast<int>(props["layer"]);
}

void Text::draw(SkCanvas* canvas, SkPaint &paint, DrawContext &ctx){
    canvas->save();
    paint.setStrokeWidth(0.25);
    

    ctx.font.setSize(size*50);
    SkFontMetrics metrics;
    ctx.font.getMetrics(&metrics);


    auto texts = split(text,'\n',ctx.props);

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
