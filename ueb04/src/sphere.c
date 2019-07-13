#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif

#include <math.h>

#include "debugGL.h"
#include "sphere.h"

/* Quelle: */
/* http://www.opengl.org.ru/docs/pg/0208.html */

#define X .525731112119133606
#define Z .850650808352039932

#define DLRANGE 6
static GLuint dlSphere;
static GLuint dlSphereGreen;
static GLuint dlSphereBlue;
static GLuint dlSphereWhite;
static GLuint dlSphereYellow;
static GLuint dlSphereRed;

void normalize(float v[3]) {
  GLfloat d = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);

  if (d == 0.0) {

    CG_ERROR(("zero length vector"));

    return;
  }

  v[0] /= d;
  v[1] /= d;
  v[2] /= d;
}

void drawtriangle(float *v1, float *v2, float *v3) {

  glBegin(GL_TRIANGLES);

  glNormal3fv(v1);
  glVertex3fv(v1);

  glNormal3fv(v2);
  glVertex3fv(v2);

  glNormal3fv(v3);
  glVertex3fv(v3);

  glEnd();
}

void subdivide(float *v1, float *v2, float *v3, long depth) {

  GLfloat v12[3], v23[3], v31[3];
  GLint i;

  if (depth == 0) {
    drawtriangle(v1, v2, v3);
    return;
  }

  for (i = 0; i < 3; i++) {
    v12[i] = v1[i] + v2[i];
    v23[i] = v2[i] + v3[i];
    v31[i] = v3[i] + v1[i];
  }

  normalize(v12);
  normalize(v23);
  normalize(v31);

  subdivide(v1, v12, v31, depth - 1);
  subdivide(v2, v23, v12, depth - 1);
  subdivide(v3, v31, v23, depth - 1);
  subdivide(v12, v23, v31, depth - 1);
}

void createIcoSphere(int subDivs) {
  /* Vordefinierte Vertices auf der Sphere */
  static GLfloat vdata[12][3] = {{-X, 0.0, Z}, {X, 0.0, Z},   {-X, 0.0, -Z},
                                 {X, 0.0, -Z}, {0.0, Z, X},   {0.0, Z, -X},
                                 {0.0, -Z, X}, {0.0, -Z, -X}, {Z, X, 0.0},
                                 {-Z, X, 0.0}, {Z, -X, 0.0},  {-Z, -X, 0.0}};

  /* Verbindung der Vertices zu Dreiecken */
  static GLuint tindices[20][3] = {
      {0, 4, 1},  {0, 9, 4},  {9, 5, 4},  {4, 5, 8},  {4, 8, 1},
      {8, 10, 1}, {8, 3, 10}, {5, 3, 8},  {5, 2, 3},  {2, 7, 3},
      {7, 10, 3}, {7, 6, 10}, {7, 11, 6}, {11, 0, 6}, {0, 1, 6},
      {6, 1, 10}, {9, 0, 11}, {9, 11, 2}, {9, 2, 5},  {7, 2, 11}};

  for (int i = 0; i < 20; i++) {
    /* 2-1-0 für richtige reihenfolge der vertices */
    subdivide(&vdata[tindices[i][2]][0], &vdata[tindices[i][1]][0],
              &vdata[tindices[i][0]][0], subDivs);
  }
}
void createIcoSphereDL(int subDivs) {
  glNewList(dlSphere, GL_COMPILE);
  /* Vordefinierte Vertices auf der Sphere */
  static GLfloat vdata[12][3] = {{-X, 0.0, Z}, {X, 0.0, Z},   {-X, 0.0, -Z},
                                 {X, 0.0, -Z}, {0.0, Z, X},   {0.0, Z, -X},
                                 {0.0, -Z, X}, {0.0, -Z, -X}, {Z, X, 0.0},
                                 {-Z, X, 0.0}, {Z, -X, 0.0},  {-Z, -X, 0.0}};

  /* Verbindung der Vertices zu Dreiecken */
  static GLuint tindices[20][3] = {
      {0, 4, 1},  {0, 9, 4},  {9, 5, 4},  {4, 5, 8},  {4, 8, 1},
      {8, 10, 1}, {8, 3, 10}, {5, 3, 8},  {5, 2, 3},  {2, 7, 3},
      {7, 10, 3}, {7, 6, 10}, {7, 11, 6}, {11, 0, 6}, {0, 1, 6},
      {6, 1, 10}, {9, 0, 11}, {9, 11, 2}, {9, 2, 5},  {7, 2, 11}};

  for (int i = 0; i < 20; i++) {
    /* 2-1-0 für richtige reihenfolge der vertices */
    subdivide(&vdata[tindices[i][2]][0], &vdata[tindices[i][1]][0],
              &vdata[tindices[i][0]][0], subDivs);
  }
  glEndList();
}

void createGreenDL(int subDivs) {
  glNewList(dlSphereGreen, GL_COMPILE);

  glColor3f(0.0f, 1.0f, 0.0f);
  createIcoSphere(subDivs);

  glEndList();
}
void createBlueDL(int subDivs) {
  glNewList(dlSphereBlue, GL_COMPILE);

  glColor3f(0.0f, 0.0f, 1.0f);
  createIcoSphere(subDivs);

  glEndList();
}
void createWhiteDL(int subDivs) {
  glNewList(dlSphereWhite, GL_COMPILE);

  glColor3f(1.0f, 1.0f, 1.0f);
  createIcoSphere(subDivs);

  glEndList();
}
void createYellowDL(int subDivs) {
  glNewList(dlSphereYellow, GL_COMPILE);

  glColor3f(1.0f, 1.0f, 0.0f);
  createIcoSphere(subDivs);

  glEndList();
}
void createRedDL(int subDivs) {
  glNewList(dlSphereRed, GL_COMPILE);

  glColor3f(1.0f, 0.0f, 0.0f);
  createIcoSphere(subDivs);

  glEndList();
}

void initSphereDL(int subDivs) {
  dlSphereGreen = glGenLists((GLsizei)DLRANGE);
  if (dlSphereGreen != 0) {
    createGreenDL(subDivs);
    dlSphereBlue = dlSphereGreen + 1;
    createBlueDL(subDivs);
    dlSphereWhite = dlSphereGreen + 2;
    createWhiteDL(subDivs);
    dlSphereYellow = dlSphereGreen + 3;
    createYellowDL(subDivs);
    dlSphereRed = dlSphereGreen + 4;
    createRedDL(subDivs);

    dlSphere = dlSphereGreen + 5;
    createIcoSphereDL(subDivs);
  } else {
    CG_ERROR(("Konnte Displaylisten nicht erzeugen\n"));
  }
}

void drawIcoSphere(sphereColor col) {

  switch (col) {
  case sGREEN:
    glCallList(dlSphereGreen);
    break;
  case sBLUE:
    glCallList(dlSphereBlue);
    break;
  case sWHITE:
    glCallList(dlSphereWhite);
    break;
  case sYELLOW:
    glCallList(dlSphereYellow);
    break;
  case sRED:
    glCallList(dlSphereRed);
    break;
  default:
    glCallList(dlSphere);
  }
}
