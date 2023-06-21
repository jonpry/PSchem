
#include "main.h"
#include <vector>
#include <stdio.h>
#include <iostream>

using namespace std;

namespace pschem {

any* PriorityDict::operator[](string &key) {
    for(auto &d : m_dicts){
        if(d->find(key) != d->end())
            return &d->at(key);
    }    
    return 0;
}

}; //Namespace pschem
