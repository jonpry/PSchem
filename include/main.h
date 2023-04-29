#pragma once

#include <vector>
#include <string>

class Text {
 public:
    Text(std::string text, float x, float y, int rot, int mirrot, float size);
    
    std::string text;
    float x, y, size;
    int rot, mirror;
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

class Line {
 public:
    Line(int layer, float x1, float y1, float x2, float y2);
    
    int layer;
    float x1, y1, x2, y2;
 private:
};

class Net {
 public:
    Net(float x1, float y1, float x2, float y2);
    
    float x1, y1, x2, y2;
 private:
};

class Box {
 public:
    Box(int layer, float x1, float y1, float x2, float y2);
    
    int layer;
    float x1, y1, x2, y2;
 private:
};

class Arc {
 public:
    Arc(int layer, float cx, float cy, float rad, float sa, float ea);
    
    int layer;
    float cx, cy, rad, sa, ea;
 private:
};

class Poly {
 public:
    Poly(int layer, int npoints);
    
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


