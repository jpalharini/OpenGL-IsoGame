//
// Created by João Pedro Berton Palharini on 6/23/18.
//

#ifndef OPENGL_ISOGAME_TRIANGLE_H
#define OPENGL_ISOGAME_TRIANGLE_H


class Triangle {
public:
    double A[2];
    double B[2];
    double C[2];

    Triangle();
    Triangle(double Ax, double Ay, double Bx, double By, double Cx, double Cy);

    double area();
    static double triangleArea(double Ao[2], double Bo[2], double Co[2]);
};


#endif //OPENGL_ISOGAME_TRIANGLE_H
