#include "main.h"
#include <vector>
#include <stdio.h>
#include <iostream>

using namespace std;

namespace pschem {

Component::Component(string _symbol, float _x, float _y, int _rot, int _mirror, anydict_t props) : Drawable(props), symbol(_symbol), pt(_x,_y), rot(_rot), mirror(_mirror) {}

void Component::rotate(){
    rot = (rot+1)%4;
}

void Component::flip(){
    mirror = !mirror;
}

void Component::move(float dx, float dy) {
    pt.m_x += dx;
    pt.m_y += dy;
}

std::vector<std::any> Component::getPropPairs() {
    return {GuiProp("x",&pt.m_x,1.0f),GuiProp("y",&pt.m_y,1.0f),GuiProp("rotation",&rot,1,0,3),GuiProp("mirror",&mirror,1,0,1),GuiProp("symbol",&symbol,false)};
}

std::vector<std::any> Component::getExplicitPropPairs() {
    std::vector<std::any> ret;
    for(auto &p : m_props){
        if(p.second.type() == typeid(std::string)){
            string &value = any_cast<string&>(p.second);
            ret.push_back(GuiProp(p.first,&value));
        }
    }
    return ret;
}

}; //Namespace pschem
