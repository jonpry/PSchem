#pragma once

#include <vector>
#include <string>
#include <map>
#include <any>
#include "include/core/SkCanvas.h"
#include "include/core/SkPaint.h"
#include "include/core/SkFont.h"

#define EPS 1e-6

enum { COLOR_BG, COLOR_NET, COLOR_SEL, COLOR_TEXT, COLOR_SYM, COLOR_PIN};

typedef std::map<std::string,std::any> anydict_t;

class DrawContext {
 public:
    SkFont font;
    SkMatrix inverse_view_mat, view_mat; 
    anydict_t props;
};


class Drawable {
 public:
    Drawable(std::map<std::string,std::any> &_props) : m_props(_props) {}
    virtual void draw(SkCanvas *canvas, SkPaint &paint, DrawContext &ctx) = 0;

    anydict_t m_props;
};

class Text : public Drawable{
 public:
    Text(std::string text, float x, float y, int rot, int mirrot, float size, anydict_t props);
    void draw(SkCanvas *canvas, SkPaint &paint, DrawContext& ctx);
    
    std::string text;
    float x, y, size;
    int rot, mirror;
    int layer;
 private:
};

class Component : public Drawable {
 public:
    Component(std::string symbol, float x, float y, int rot, int mirror, anydict_t props);
    void draw(SkCanvas *canvas, SkPaint &paint, DrawContext& ctx){}
    
    std::string symbol;
    float x, y;
    int rot, mirror;
 private:
};

class Line : public Drawable {
 public:
    Line(int layer, float x1, float y1, float x2, float y2, anydict_t props);
    void draw(SkCanvas *canvas, SkPaint &paint, DrawContext& ctx);
    
    int layer;
    float x1, y1, x2, y2;
 private:
};

class Net : public Drawable{
 public:
    Net(float x1, float y1, float x2, float y2, anydict_t props);
    void draw(SkCanvas *canvas, SkPaint &paint, DrawContext& ctx);
    
    float x1, y1, x2, y2;
 private:
};

class Box : public Drawable{
 public:
    Box(int layer, float x1, float y1, float x2, float y2, anydict_t props);
    void draw(SkCanvas *canvas, SkPaint &paint, DrawContext& ctx);
    
    int layer;
    float x1, y1, x2, y2;
 private:
};

class Arc : public Drawable{
 public:
    Arc(int layer, float cx, float cy, float rad, float sa, float ea, anydict_t props);
    void draw(SkCanvas *canvas, SkPaint &paint, DrawContext& ctx);
    
    int layer;
    float cx, cy, rad, sa, ea;
 private:
};

class Poly : public Drawable{
 public:
    Poly(int layer, int npoints, anydict_t props);
    void draw(SkCanvas *canvas, SkPaint &paint, DrawContext& ctx);
    
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


