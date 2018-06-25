//
// Created by João Pedro Berton Palharini on 6/23/18.
//

#ifndef OPENGL_ISOGAME_GAMEOBJECT_H
#define OPENGL_ISOGAME_GAMEOBJECT_H

#include "Includes.h"

class GameObject {
public:
    // Attributes
    GLfloat vertices[16];
    GLfloat translation[16];
    GLfloat textureCoord[8];
    unsigned int indices[6];
    GLuint vao, vbo, vboTexture, ebo;
    const char *textureFile;
    int row, column; // Used for objects to be drawn over tilemap

    // Methods
    GameObject(GLfloat* v, GLfloat* t, const char *filename);
    GameObject(GLfloat* v, GLfloat* t, const char *filename, int r, int c);
    void load();
};


#endif //OPENGL_ISOGAME_GAMEOBJECT_H
