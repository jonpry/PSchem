#pragma once

#include <vector>
#include <string>
#include <map>
#include <any>
#include "include/core/SkCanvas.h"
#include "include/core/SkPaint.h"
#include "include/core/SkFont.h"

#define EPS 1e-6

namespace pschem {

SkColor mortonColor(uint32_t i);
uint32_t mortonId(uint32_t argb);

enum { COLOR_BG, COLOR_NET, COLOR_SEL, COLOR_TEXT, COLOR_SYM, COLOR_PIN};

typedef std::map<std::string,std::any> anydict_t;

class Drawable;
class MainWindow;

class IIdReceiver {
 public:
    virtual void SetId(int id, Drawable* tgt)=0;
};

class DrawContext {
 public:
    SkFont font, hitFont;
    SkMatrix inverse_view_mat, view_mat; 
    anydict_t props;
    SkCanvas *canvas, *hitCanvas;
    int objId;
    std::array<SkColor,22> colorMap;    
    Drawable *parent, *selected;
    IIdReceiver *window;
    
};


class Drawable {
 public:
    Drawable(std::map<std::string,std::any> &_props) : m_props(_props) {}
    virtual void draw(SkPaint &paint, DrawContext &ctx) = 0;

    bool isSelected(DrawContext &ctx){ return ctx.selected == this || (ctx.parent && ctx.selected == ctx.parent);}
    virtual void rotate() {}
    virtual void flip() {}
    virtual void move(float dx, float dy) {}
    anydict_t m_props;
};

class Text : public Drawable{
 public:
    Text(std::string text, float x, float y, int rot, int mirrot, float size, anydict_t props);
    void draw(SkPaint &paint, DrawContext& ctx);
    
    void rotate();
    void flip();
    
    std::string text;
    float x, y, size;
    int rot, mirror;
    int layer;
 private:
};

class Component : public Drawable {
 public:
    Component(std::string symbol, float x, float y, int rot, int mirror, anydict_t props);
    void draw(SkPaint &paint, DrawContext& ctx){}

    void rotate();
    void flip();
    void move(float dx, float dy);
    
    std::string symbol;
    float x, y;
    int rot, mirror;
 private:
};

class Line : public Drawable {
 public:
    Line(int layer, float x1, float y1, float x2, float y2, anydict_t props);
    void draw(SkPaint &paint, DrawContext& ctx);
    
    int layer;
    float x1, y1, x2, y2;
 private:
};

class Net : public Drawable{
 public:
    Net(float x1, float y1, float x2, float y2, anydict_t props);
    void draw(SkPaint &paint, DrawContext& ctx);
    
    float x1, y1, x2, y2;
    void move(float dx, float dy);

 private:
};

class Box : public Drawable{
 public:
    Box(int layer, float x1, float y1, float x2, float y2, anydict_t props);
    void draw(SkPaint &paint, DrawContext& ctx);
    
    int layer;
    float x1, y1, x2, y2;
 private:
};

class Arc : public Drawable{
 public:
    Arc(int layer, float cx, float cy, float rad, float sa, float ea, anydict_t props);
    void draw(SkPaint &paint, DrawContext& ctx);
    
    int layer;
    float cx, cy, rad, sa, ea;
 private:
};

class Poly : public Drawable{
 public:
    Poly(int layer, int npoints, anydict_t props);
    void draw(SkPaint &paint, DrawContext& ctx);
    
    bool m_fill {false};
    int layer;
    std::vector<float> xs, ys;
 private:
};

class Drawing {
 public:
    explicit Drawing(std::string filename);

    std::vector<Line> lines;
    std::vector<Arc> arcs;
    std::vector<Box> boxes;
    std::vector<Text> texts;
    std::vector<Net> nets;
    std::vector<Poly> polys;
    std::vector<Component> components;
};

}; //Namespace pschem
