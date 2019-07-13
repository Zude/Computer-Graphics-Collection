#ifndef __SCENE_H_
#define __SCENE_H_

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif

#include "types.h"

int initScene(void);

void drawScene(void);
void drawPickingScene(void);

void zoomCamera(GLboolean zoom);
void camDirMove(CamDirection dir);

GLdouble *currentCamera(void);

void toggleWireframeMode(void);

#endif // __SCENE_H_
