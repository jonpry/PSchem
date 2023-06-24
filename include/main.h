#pragma once

#include <vector>
#include <string>
#include <map>
#include <any>
#include "include/core/SkImage.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkPaint.h"
#include "include/core/SkFont.h"

#include <Eigen/Core>

#define EPS 1e-6

namespace pschem {

SkColor mortonColor(uint32_t i);
uint32_t mortonId(uint32_t argb);

enum { COLOR_BG, COLOR_NET, COLOR_SEL, COLOR_TEXT, COLOR_SYM, COLOR_PIN};

typedef std::map<std::string,std::any> anydict_t;

class Drawable;
class MainWindow;

class PriorityDict {
 public:
   PriorityDict(){};
   
   void Push(anydict_t* dict) { m_dicts.push_back(dict); }
   void Pop() { m_dicts.pop_back(); }
   void Clear() { m_dicts.clear(); }
   
   std::any* operator[](std::string &);
   
   std::vector<anydict_t*> m_dicts;
};

template<typename T>
class GuiProp {
 public:
    GuiProp(std::string name, T* field, T increment, T min = std::numeric_limits<T>::min(), T max = std::numeric_limits<T>::max()) : m_name(name), m_field(field), m_increment(increment), m_min(min), m_max(max) {}

    GuiProp(std::string name, T* field, bool editable=true) : m_name(name), m_field(field), m_editable(editable) {}
    
    std::string m_name;
    T* m_field; 
    T  m_increment;
    T  m_min, m_max;
    bool m_editable;
};

class Point {
 public:
    Point() {}
    Point(float x, float y) { m_x=x; m_y = y; }
    
    float m_x, m_y;
    
    Eigen::Vector2f Vec() { return Eigen::Vector2f(m_x,m_y); }
};

class IIdReceiver {
 public:
    virtual void SetId(int id, Drawable* tgt)=0;
};

class DrawContext {
 public:
    SkFont font, hitFont;
    SkMatrix inverse_view_mat, view_mat; 
    PriorityDict props;
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
    virtual const char *ClassName() { return 0; }
    virtual std::vector<std::any> getPropPairs() {return {};}
    anydict_t m_props;
};

class Text : public Drawable{
 public:
    Text(std::string text, float x, float y, int rot, int mirrot, float size, anydict_t props);
    void draw(SkPaint &paint, DrawContext& ctx);
    
    void rotate();
    void flip();
    
    const char *ClassName() { return "Text"; }
    std::vector<std::any> getPropPairs() {return {GuiProp("x",&pt.m_x,1.0f),GuiProp("y",&pt.m_y,1.0f),GuiProp("text",&text)};}
    
    std::string text;
    float size;
    Point pt;
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
    const char *ClassName() { return "Component"; }
    std::vector<std::any> getPropPairs();
    std::vector<std::any> getExplicitPropPairs();
    
    std::string symbol;
    Point pt;
    int rot, mirror;
 private:
};

class Line : public Drawable {
 public:
    Line(int layer, float x1, float y1, float x2, float y2, anydict_t props);
    void draw(SkPaint &paint, DrawContext& ctx);
    const char *ClassName() { return "Line"; }
    std::vector<std::any> getPropPairs() {return {GuiProp("x1",&p1.m_x,1.0f),GuiProp("y1",&p1.m_y,1.0f),GuiProp("x2",&p2.m_x,1.0f),GuiProp("y2",&p2.m_y,1.0f)};}
    
    int layer;
    Point p1, p2;
 private:
};

class Net : public Drawable{
 public:
    Net(float x1, float y1, float x2, float y2, anydict_t props);
    void draw(SkPaint &paint, DrawContext& ctx);
    const char *ClassName() { return "Net"; }
    std::vector<std::any> getPropPairs() {return {GuiProp("x1",&p1.m_x,1.0f),GuiProp("y1",&p1.m_y,1.0f),GuiProp("x2",&p2.m_x,1.0f),GuiProp("y2",&p2.m_y,1.0f)};}

    Point p1, p2;
    void move(float dx, float dy);

 private:
};

class Box : public Drawable{
 public:
    Box(int layer, float x1, float y1, float x2, float y2, anydict_t props);
    void draw(SkPaint &paint, DrawContext& ctx);
    const char *ClassName() { return "Box"; }
    std::vector<std::any> getPropPairs() {return {GuiProp("x1",&p1.m_x,1.0f),GuiProp("y1",&p1.m_y,1.0f),GuiProp("x2",&p2.m_x,1.0f),GuiProp("y2",&p2.m_y,1.0f),GuiProp("alpha",&alpha,.01f,0.0f,1.0f),GuiProp("image",&image)};}
    
    int layer;
    float alpha;
    std::string image;
    Point p1,p2;
    sk_sp<SkImage> sk_image;
 private:
};

class Arc : public Drawable{
 public:
    Arc(int layer, float cx, float cy, float rad, float sa, float ea, anydict_t props);
    void draw(SkPaint &paint, DrawContext& ctx);
    const char *ClassName() { return "Arc"; }
    std::vector<std::any> getPropPairs();
    
    int layer;
    Point c;
    float rad, sa, ea;
 private:
};

class Poly : public Drawable{
 public:
    Poly(int layer, int npoints, anydict_t props);
    void draw(SkPaint &paint, DrawContext& ctx);
    const char *ClassName() { return "Poly"; }
    
    bool m_fill {false};
    int layer;
    std::vector<Point> pts;
 private:
};

class Drawing : public Drawable{
 public:
    explicit Drawing(std::string filename);

    void draw(SkPaint &paint, DrawContext& ctx) {}
    const char *ClassName() { return "Drawing"; }
 
    std::vector<Line> lines;
    std::vector<Arc> arcs;
    std::vector<Box> boxes;
    std::vector<Text> texts;
    std::vector<Net> nets;
    std::vector<Poly> polys;
    std::vector<Component> components;
    
    void DeriveConnectivity(SkPaint &paint, DrawContext &ctx);
};

class Sweep {
 public:
    class Query: public Point {
     public:
        Query(Point pt, Drawable *src) : Point(pt), m_src(src) {}
     
        Drawable *owner;

        std::vector<Net*> m_hits;
        Drawable *m_src;
    };

    class SortedLine {
     public:
        SortedLine(Point p1, Point p2, Net *src);
        
        float distanceTo(Point pi);
        Net *m_src;
        
        Point m_p1, m_p2;
    };
    
    Sweep();
   
    void SetLines(std::vector<Net> &lines);
    void QueryPt(Point p);
    void QueryMany(std::vector<Query> &p);
   
    std::vector<SortedLine> m_lines;
    std::vector<SortedLine*> m_starts,m_ends;
};

}; //Namespace pschem
