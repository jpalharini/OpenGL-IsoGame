//
// Created by João Pedro Berton Palharini on 6/23/18.
//

#include "Includes.h"
#include "Triangle.h"

Triangle::Triangle() {
    A[0] = 0;
    A[1] = 0;

    B[0] = 0;
    B[1] = 0;

    C[0] = 0;
    C[1] = 0;
}

Triangle::Triangle(double Ax, double Ay, double Bx, double By, double Cx, double Cy) {
    A[0] = Ax;
    A[1] = Ay;

    B[0] = Bx;
    B[1] = By;

    C[0] = Cx;
    C[1] = Cy;
}

double Triangle::area() {
    return abs(((B[0] - A[0]) * (C[1] - A[1]) - (C[0] - A[0]) * (B[1] - A[1]))/2);
}

double Triangle::triangleArea(double* Ao, double* Bo, double* Co) {
    return abs(((Bo[0] - Ao[0]) * (Co[1] - Ao[1]) - (Co[0] - Ao[0]) * (Bo[1] - Ao[1]))/2);
}