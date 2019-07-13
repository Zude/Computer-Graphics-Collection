#ifndef __MAP_H_
#define __MAP_H_

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif

GLdouble coordToTex(GLdouble coord);

GLdouble indexToCoord(int index);

#endif // __MAP_H_
