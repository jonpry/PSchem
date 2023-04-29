#include "main.h"
#include <vector>
#include <stdio.h>
#include <iostream>

using namespace std;

void test_peg(string filename, vector<Line> &lines, vector<Arc> &arcs, vector<Box> &boxes, vector<Text> &texts, vector<Net> &nets, vector<Component> &components, vector<Poly> &polys);

Drawing::Drawing(string filename) {
    test_peg(filename, lines, arcs, boxes, texts, nets, components, polys);
}


