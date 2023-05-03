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

class DrawContext {
 public:
    SkFont font;
    SkMatrix inverse_view_mat, view_mat; 
};


class IDrawable {
 public:
    virtual void draw(SkCanvas *canvas, SkPaint &paint, DrawContext &ctx) = 0;
};

class Text : IDrawable{
 public:
    Text(std::string text, float x, float y, int rot, int mirrot, float size, std::map<std::string,std::any> props);
    void draw(SkCanvas *canvas, SkPaint &paint, DrawContext& ctx);
    
    std::map<std::string,std::any> props;
    std::string text;
    float x, y, size;
    int rot, mirror;
    int layer;
 private:
};

class Component {
 public:
    Component(std::string symbol, float x, float y, int rot, int mirror);
    
    std::string symbol;
    float x, y;
    int rot, mirror;
 private:
};

class Line : IDrawable {
 public:
    Line(int layer, float x1, float y1, float x2, float y2);
    void draw(SkCanvas *canvas, SkPaint &paint, DrawContext& ctx);
    
    int layer;
    float x1, y1, x2, y2;
 private:
};

class Net : IDrawable{
 public:
    Net(float x1, float y1, float x2, float y2);
    void draw(SkCanvas *canvas, SkPaint &paint, DrawContext& ctx);
    
    float x1, y1, x2, y2;
 private:
};

class Box : IDrawable{
 public:
    Box(int layer, float x1, float y1, float x2, float y2);
    void draw(SkCanvas *canvas, SkPaint &paint, DrawContext& ctx);
    
    int layer;
    float x1, y1, x2, y2;
 private:
};

class Arc : IDrawable{
 public:
    Arc(int layer, float cx, float cy, float rad, float sa, float ea);
    void draw(SkCanvas *canvas, SkPaint &paint, DrawContext& ctx);
    
    int layer;
    float cx, cy, rad, sa, ea;
 private:
};

class Poly : IDrawable{
 public:
    Poly(int layer, int npoints);
    void draw(SkCanvas *canvas, SkPaint &paint, DrawContext& ctx);
    
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


