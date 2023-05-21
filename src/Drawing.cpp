#include "main.h"
#include <vector>
#include <stdio.h>
#include <iostream>

using namespace std;

namespace pschem {
void test_peg(string filename, vector<Line> &lines, vector<Arc> &arcs, vector<Box> &boxes, vector<Text> &texts, vector<Net> &nets, vector<Component> &components, vector<Poly> &polys);

Drawing::Drawing(string filename) {
    test_peg(filename, lines, arcs, boxes, texts, nets, components, polys);
}

void Drawing::DeriveConnectivity(){
    std::vector<Sweep::Query> queries;
    for(auto &l : nets){
        queries.push_back({l.p1,&l});
        queries.push_back({l.p2,&l});
    }
    
    Sweep sweep;
    sweep.SetLines(nets);
    
    sweep.QueryMany(queries);
    
    printf("Qs\n");
    for(auto q : queries){
        if(q.m_hits.size() > 2){
            printf("%ld %f %f\n", q.m_hits.size(), q.m_x, q.m_y);
        }
    }
}

}; //Namespace pschem


