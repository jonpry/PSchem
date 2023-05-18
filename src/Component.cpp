#include "main.h"
#include <vector>
#include <stdio.h>
#include <iostream>

using namespace std;

namespace pschem {

Component::Component(string _symbol, float _x, float _y, int _rot, int _mirror, anydict_t props) : Drawable(props), symbol(_symbol), x(_x), y(_y), rot(_rot), mirror(_mirror) {}

void Component::rotate(){
    rot = (rot+1)%4;
}

void Component::flip(){
    mirror = !mirror;
}

void Component::move(float dx, float dy) {
    x += dx;
    y += dy;
}

}; //Namespace pschem
