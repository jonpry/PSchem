#include "main.h"
#include <vector>
#include <stdio.h>
#include <iostream>

using namespace std;


Poly::Poly(int _layer, int npoints) : layer(_layer) {
    xs.resize(npoints);
    ys.resize(npoints);
}


