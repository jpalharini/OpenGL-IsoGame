//
// Created by João Pedro Berton Palharini on 6/23/18.
//

#ifndef OPENGL_ISOGAME_TILEMAP_H
#define OPENGL_ISOGAME_TILEMAP_H


#include "Tile.h"

class TileMap {
public:
    Tile* map[10][10];

    TileMap();
};


#endif //OPENGL_ISOGAME_TILEMAP_H
