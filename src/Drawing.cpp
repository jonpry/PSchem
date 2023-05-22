#include "main.h"
#include <vector>
#include <stdio.h>
#include <iostream>
#include <set>
#include "include/core/SkRRect.h"

using namespace std;

namespace pschem {
void test_peg(string filename, vector<Line> &lines, vector<Arc> &arcs, vector<Box> &boxes, vector<Text> &texts, vector<Net> &nets, vector<Component> &components, vector<Poly> &polys);

Drawing::Drawing(string filename) {
    test_peg(filename, lines, arcs, boxes, texts, nets, components, polys);
}

void Drawing::DeriveConnectivity(SkPaint &paint, DrawContext &ctx){
    std::vector<Sweep::Query> queries;
    for(auto &l : nets){
        queries.push_back({l.p1,&l});
        queries.push_back({l.p2,&l});
    }
    
    Sweep sweep;
    sweep.SetLines(nets);
    
    sweep.QueryMany(queries);
    
//    printf("Qs\n");
    
    std::set<pair<float,float>> intersections;
    for(auto q : queries){
        //TODO: this is not the proper condition
        if(q.m_hits.size() > 2){
            intersections.insert(make_pair(q.m_x,q.m_y));
//            printf("%ld %f %f\n", q.m_hits.size(), q.m_x, q.m_y);
        }
    }
    
    for(auto p : intersections) {
        paint.setColor(ctx.colorMap[COLOR_NET]);
        ctx.canvas->drawCircle(p.first,p.second,5,paint);
    }
}

}; //Namespace pschem


