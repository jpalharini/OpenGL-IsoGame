//
// Created by João Pedro Berton Palharini on 6/23/18.
//

#ifndef OPENGL_ISOGAME_TILE_H
#define OPENGL_ISOGAME_TILE_H


#include "Triangle.h"

class Tile {
public:
    Triangle* t1;
    Triangle* t2;
    float offsetX;
    float offsetY;
    int texture;
    bool walkable;
    bool bomb;

    Tile (int x, int y, int w, int h, int t, bool wa, bool b);
};


#endif //OPENGL_ISOGAME_TILE_H
