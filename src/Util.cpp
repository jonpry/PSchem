#include <stdint.h>
#include <limits.h>
#include "include/core/SkColor.h"

namespace pschem {

static uint32_t reverseBits(uint32_t n) {
    n = (n >> 1) & 0x55555555 | (n << 1) & 0xaaaaaaaa;
    n = (n >> 2) & 0x33333333 | (n << 2) & 0xcccccccc;
    n = (n >> 4) & 0x0f0f0f0f | (n << 4) & 0xf0f0f0f0;
    n = (n >> 8) & 0x00ff00ff | (n << 8) & 0xff00ff00;
    n = (n >> 16) & 0x0000ffff | (n << 16) & 0xffff0000;
    return n;
}

static uint32_t morton_compact3(uint64_t w) {
    w &=                  0x1249249249249249;
    w = (w ^ (w >> 2))  & 0x30c30c30c30c30c3;
    w = (w ^ (w >> 4))  & 0xf00f00f00f00f00f;
    w = (w ^ (w >> 8))  & 0x00ff0000ff0000ff;
    w = (w ^ (w >> 16)) & 0x00ff00000000ffff;
    w = (w ^ (w >> 32)) & 0x00000000001fffff;
    return (uint32_t)w;
}

static void morton_decode3d(uint64_t code, uint32_t *xindex, uint32_t *yindex, uint32_t *zindex){
    *xindex = morton_compact3(code);
    *yindex = morton_compact3(code >> 1);
    *zindex = morton_compact3(code >> 2);
}

SkColor mortonColor(uint32_t i){
    i+=1;
    i = reverseBits(i) >> 8;
    uint32_t x=0,y=0,z=0;
    morton_decode3d(i,&x,&y,&z);
    return SkColorSetARGB(255,x,y,z);
}

}; //Namespace pschem
