#include "main.h"
#include <vector>
#include <stdio.h>
#include <iostream>

using namespace std;


Component::Component(string _symbol, float _x, float _y, int _rot, int _mirror, anydict_t props) : Drawable(props), symbol(_symbol), x(_x), y(_y), rot(_rot), mirror(_mirror) {}


