//
// Created by João Pedro Berton Palharini on 6/23/18.
//

#include "Tile.h"

Tile::Tile(int x, int y, int tw, int th, int sh, int t, bool w, bool b) {
    // Set offsets for drawing
    offsetX = y * (tw/2) + x * (tw/2);
    offsetY = y * (th/2) - x * (th/2) + sh/2;

    // Sets vertices of triangles
    double Ax, Ay, Bx, By, Cx, Cy;

    /*
     * A (tX, tY)
     * B (tX + TW/2, tY - TH/2)
     * C (tX + TW/2, tY + TH/2)
     */

    // Atributes coordinates for vertices of triangles
    Ax = offsetX;
    Ay = offsetY + th/2;

    Bx = offsetX + tw / 2;
    By = offsetY;

    Cx = offsetX + tw / 2;
    Cy = offsetY + th;

    t1 = new Triangle(Ax, Ay, Bx, By, Cx, Cy);

    Ax = offsetX + tw;

    t2 = new Triangle(Ax, Ay, Bx, By, Cx, Cy);

    // Sets other parameters
    texture = t;
    walkable = w;
    bomb = b;
}