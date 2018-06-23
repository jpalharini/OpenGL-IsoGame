//
// Created by João Pedro Berton Palharini on 6/23/18.
//

#include "Tile.h"

Tile::Tile(int x, int y, int tw, int th, int t, bool w, bool b) {
    // Set offsets for drawing
    offsetX = y * (tw/2) + x * (tw/2);
    offsetY = y * (th/2) - x * (th/2) + 500/2;

    // Sets triangles
    double Ax, Ay, Bx, By, Cx, Cy;

    /*
     * A (tX, tY)
     * B (tX + tw/2, tY - th/2)
     * C (tX + tw/2, tY + th/2)
     */

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