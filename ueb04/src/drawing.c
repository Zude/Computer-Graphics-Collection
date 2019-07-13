#include <math.h>
#include <stdio.h>
#include <string.h>

#include "drawing.h"
#include "map.h"
#include "sphere.h"
#include "texture.h"
#include "types.h"
#include "waterSimulation.h"

#define ISLAND_HEIGHT 0.3f
#define ISLAND_DEPTH -1.2f
#define ISLAND_POINTS 15

extern GLboolean Debug;
extern GLboolean ShowSpheres;
extern GLboolean ShowNormals;

Pos *gNormPos = NULL;
Pos *gNormals = NULL;

Pos *gIslandNormPos = NULL;
Pos *gIslandNormals = NULL;

/**
 * Hilffunktion um alle Werte eines Vertexarrays zu setzen
 * @param V: Pointer zum Vertex Array
 * @param index: Zu setzender Index
 * @param pos: Position vom Vertex
 * @param col: Farbe vom Vertex
 * @param tex: Textur Koordinaten
 * @param norm: Normale für Vertex
 */
static void setVertex(Vertex *V, int index, Pos pos, Col col, Tex tex,
                      Pos norm) {
  V[index][CX] = pos[0];
  V[index][CY] = pos[1];
  V[index][CZ] = pos[2];
  V[index][CR] = col[0];
  V[index][CG] = col[1];
  V[index][CB] = col[2];
  V[index][CS] = tex[0];
  V[index][CT] = tex[1];
  V[index][CNX] = norm[0];
  V[index][CNY] = norm[1];
  V[index][CNZ] = norm[2];
}
/**
 * Normalisiert den übergeben Vektor
 * Für Normale nutzen
 * @param v: Vektor ver normalisiert wird
 */
static void normalizeVector(GLdouble v[3]) {
  GLdouble length = sqrt(pow(v[0], 2) + pow(v[1], 2) + pow(v[2], 2));
  v[0] = v[0] / length;
  v[1] = v[1] / length;
  v[2] = v[2] / length;
}
/**
 * Berechnet das Kreuzprodukt von 2 Vektoren
 * @param r: Produkt aus berechnung
 * @param v1: erster Vektor
 * @param v2: zweiter Vektor
 */
static void crossProduct(GLdouble r[3], GLdouble v1[3], GLdouble v2[3]) {
  r[0] = v1[1] * v2[2] - v1[2] * v2[1];
  r[1] = v1[2] * v2[0] - v1[0] * v2[2];
  r[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

/**
 * Findet die beiden Vektoren für die normalen Berechnung
 * an einem Punkt
 * @param water: Array mit Positionen
 * @param index: Index vom Vertex für die Normale
 * @param factor: Größe des Array als Faktor
 * @param v1: erster Vektor
 * @param v2: zweiter Vektor
 */
static void findCrossingVectors(Water water, int index, int factor,
                                GLdouble v1[3], GLdouble v2[3]) {
  int up, down, left, right;

  up = getNeighborIndex(index, factor, dirUp);
  down = getNeighborIndex(index, factor, dirDown);
  left = getNeighborIndex(index, factor, dirLeft);
  right = getNeighborIndex(index, factor, dirRight);

  v1[0] = water[up][0] - water[down][0];
  v1[1] = water[up][1] - water[down][1];
  v1[2] = water[up][2] - water[down][2];

  v2[0] = water[left][0] - water[right][0];
  v2[1] = water[left][1] - water[right][1];
  v2[2] = water[left][2] - water[right][2];
}

/**
 * Zeichnet die übergebenen Normalen als GL_LINES
 * @param normalPos: Positionen der Normalen
 * @param normals: normalisierte Normalen
 * @param length: Länge der Arrays
 */
static void drawNormalArrays(Pos normalPos[], Pos normals[], int length) {
  glBegin(GL_LINES);
  for (int i = 0; i < length; i++) {
    glVertex3f(normalPos[i][0], normalPos[i][1], normalPos[i][2]);
    glVertex3f(normalPos[i][0] + normals[i][0], normalPos[i][1] + normals[i][1],
               normalPos[i][2] + normals[i][2]);
  }
  glEnd();
}

/**
 * Zeichnet die normalen für das Wasser und die Insel
 */
void drawNormals(void) {

  glColor3f(1.0f, 0.0f, 0.0f);
  drawNormalArrays(gNormPos, gNormals, getArrayFactor() * getArrayFactor());

  glPushMatrix();
  {
    glScalef(ISLAND_SCALE, ISLAND_SCALE, ISLAND_SCALE);
    glColor3f(0.0f, 1.0f, 0.0f);
    drawNormalArrays(gIslandNormPos, gIslandNormals, 2 * ISLAND_POINTS + 1);
  }
  glPopMatrix();

  free(gIslandNormals);
  gIslandNormals = NULL;
  free(gIslandNormPos);
  gIslandNormPos = NULL;

  free(gNormals);
  gNormals = NULL;
  free(gNormPos);
  gNormPos = NULL;
}

/**
 * Hilfsfunktion um Normalen und deren Position für die Insel zu setzen
 * @param index: Index der Normale
 * @param pos: Position der Normale
 * @param norm: normalisierte normale
 */
static void setIslandNorm(int index, Pos pos, Pos norm) {
  gIslandNormPos[index][0] = pos[0];
  gIslandNormPos[index][1] = pos[1];
  gIslandNormPos[index][2] = pos[2];

  gIslandNormals[index][0] = norm[0];
  gIslandNormals[index][1] = norm[1];
  gIslandNormals[index][2] = norm[2];
}

/**
 * Zeichnet eine Insel
 */
void drawIsland(void) {
  Vertex *Vertices;
  /* Temporäre Farbe für alle Vertices */
  Col color = {1.0f, 1.0f, 1.0f};

  GLuint numberOfVertices = 2 * ISLAND_POINTS + 1;

  Pos upper_outer_points[ISLAND_POINTS]; // Äußere Punkte für Oberfläche
  Pos lower_outer_points[ISLAND_POINTS]; // Untere Punkte für Umrandung

  int upper_Indices_Length = ISLAND_POINTS + 2;
  GLuint upper_Indices[upper_Indices_Length];

  int lower_Indices_Length = ISLAND_POINTS * 2 + 2;
  GLuint lower_Indices[lower_Indices_Length];

  /* Mittelpunkt und erster Vertex */
  Pos pc = {0.0f, ISLAND_HEIGHT, 0.0f};
  Tex tc = {0.5f, 0.5f};
  Pos nor = {0.0f, 1.0f, 0.0f};

  Tex tex[numberOfVertices];

  Vertices = malloc(sizeof(Vertex) * numberOfVertices);
  gIslandNormals = malloc(sizeof(Pos) * numberOfVertices);
  gIslandNormPos = malloc(sizeof(Pos) * numberOfVertices);

  /* Kein Speicher -> exit */
  if (!Vertices || !gIslandNormPos || !gIslandNormals) {
    exit(1);
  }

  /* ----------------------------------------- */
  /* Oberfläche und Umrandung der Insel */
  /* ----------------------------------------- */
  setIslandNorm(0, pc, nor);
  setVertex(Vertices, 0, pc, color, tc, nor);

  float slice = 2 * 3.14 / ISLAND_POINTS;
  for (int i = 0; i < ISLAND_POINTS; i++) {
    float angle = slice * i;

    GLdouble x = 1.0f * sin(angle);
    GLdouble z = 1.0f * cos(angle);

    tex[i][0] = coordToTex(x);
    tex[i][1] = coordToTex(z);

    Pos upPos, lowPos, lowNorm;
    upPos[0] = x;
    upPos[1] = ISLAND_HEIGHT;
    upPos[2] = z;

    lowPos[0] = x;
    lowPos[1] = ISLAND_DEPTH;
    lowPos[2] = z;

    lowNorm[0] = x;
    lowNorm[1] = 0;
    lowNorm[2] = z;

    normalizeVector(lowNorm);

    /* Vertices für oberen Ring */
    upper_outer_points[i][0] = upPos[0];
    upper_outer_points[i][1] = upPos[1];
    upper_outer_points[i][2] = upPos[2];
    /* Vertices für unteren Ring */
    lower_outer_points[i][0] = lowPos[0];
    lower_outer_points[i][1] = lowPos[1];
    lower_outer_points[i][2] = lowPos[2];

    /* Normalen für oberen Ring */
    setIslandNorm(i + 1, upPos, nor);

    /* Normalen für unteren Ring */
    setIslandNorm(i + 1 + ISLAND_POINTS, lowPos, lowNorm);
  }

  for (int i = 0; i < ISLAND_POINTS; i++) {
    /* Alle Vertices von upper_outer_points setzen */
    setVertex(Vertices, 1 + i, upper_outer_points[i], color, tex[i],
              gIslandNormals[i + 1]);

    /* Alle Vertices von lower_outer_points setzen */
    setVertex(Vertices, 1 + i + ISLAND_POINTS, lower_outer_points[i], color,
              tex[i], gIslandNormals[i + 1 + ISLAND_POINTS]);
  }

  /* ----------------------------------------- */
  /* Indices*/
  /* ----------------------------------------- */
  for (int i = 0; i < upper_Indices_Length; i++) {
    upper_Indices[i] = i;
  }
  upper_Indices[upper_Indices_Length - 1] = 1;

  for (int l = ISLAND_POINTS + 1, u = 1, i = 0; i < lower_Indices_Length - 2;
       i++) {
    if (i % 2) {
      lower_Indices[i] = l;
      l++;
    } else {
      lower_Indices[i] = u;
      u++;
    }
  }
  /* Umrandung schließen */
  lower_Indices[lower_Indices_Length - 1] = ISLAND_POINTS + 1;
  lower_Indices[lower_Indices_Length - 2] = 1;

  /* ----------------------------------------- */
  /* Vertex Array setzen und zeichen*/
  /* ----------------------------------------- */

  /* Vertex Array und Color Array setzen */
  glVertexPointer(3, GL_DOUBLE, sizeof(Vertex), &(Vertices[0][CX]));
  glColorPointer(3, GL_DOUBLE, sizeof(Vertex), &(Vertices[0][CR]));
  glTexCoordPointer(2, GL_DOUBLE, sizeof(Vertex), &(Vertices[0][CS]));
  glNormalPointer(GL_DOUBLE, sizeof(Vertex), &(Vertices[0][CNX]));

  /* Vertices zeichnen */
  glDrawElements(GL_TRIANGLE_FAN, upper_Indices_Length, GL_UNSIGNED_INT,
                 upper_Indices);

  glDrawElements(GL_QUAD_STRIP, lower_Indices_Length, GL_UNSIGNED_INT,
                 lower_Indices);

  free(Vertices);
  Vertices = NULL;
}

/**
 * Zeichnet eine Sphere an der gegebenen Position
 * @param position: Position als XYZ für Translate operation
 * @param name: Name bzw. Index für Picking
 */
static void drawSphere(Pos position, GLuint name) {

  sphereColor col = sWHITE;

  if (position[1] < -0.2f) {
    if (position[1] < -0.5f) {
      col = sGREEN;
    } else {
      col = sBLUE;
    }
  } else if (position[1] > 0.2f) {
    if (position[1] > 0.5f) {
      col = sRED;
    } else {
      col = sYELLOW;
    }
  }

  glPushMatrix();
  glPushName(name);
  {
    glTranslatef(position[0], position[1], position[2]);
    glScalef(SPHERE_SCALE, SPHERE_SCALE, SPHERE_SCALE);
    drawIcoSphere(col);
  }
  glPopName();
  glPopMatrix();
}
/**
 * Zeichnet Spheren an allen Position im gegebenem Array
 * @param water: Array mit Positionen
 * @param factor: Größe es Array als Factor (Größe: factor^2)
 */
void drawSpheres(Water water, int factor) {

  glPushName(99);
  int numberOfSpheres = factor * factor;
  for (int i = 0; i < numberOfSpheres; i++) {

    drawSphere(water[i], i);
  }
  glPopName();
}

/**
 * Zeichnet die simulierte Wasseroberfläche
 * @param water: Array mit allen Positionen simulierten Punkte
 * @param factor: größe des Arrays als Faktor
 */
void drawWater(Water water, int factor) {
  Vertex *Vertices;

  int numberOfVertices = factor * factor;

  Vertices = malloc(sizeof(Vertex) * numberOfVertices);

  if (!Vertices) {
    exit(1);
  }

  Col color[numberOfVertices];
  Tex tex[numberOfVertices];

  /* Texture Koordinaten berechnen */
  for (int i = 0; i < numberOfVertices; i++) {
    GLdouble x = coordToTex(water[i][0]);
    GLdouble y = coordToTex(water[i][2]);

    tex[i][0] = x;
    tex[i][1] = y;

    if (Debug) {
      color[i][0] = x + y;
      color[i][1] = x + y;
      color[i][2] = x + y;

    } else {
      color[i][0] = 1.0f;
      color[i][1] = 1.0f;
      color[i][2] = 1.0f;
    }
  }

  gNormPos = malloc(sizeof(Pos) * numberOfVertices);
  gNormals = malloc(sizeof(Pos) * numberOfVertices);
  if (!gNormals || !gNormPos) {
    exit(1);
  }

  /* Normalen berechnen */
  for (int i = 0; i < numberOfVertices; i++) {

    GLdouble norm[3];
    GLdouble v1[3];
    GLdouble v2[3];

    findCrossingVectors(water, i, factor, v1, v2);
    crossProduct(norm, v1, v2);
    normalizeVector(norm);

    gNormals[i][0] = norm[0];
    gNormals[i][1] = norm[1];
    gNormals[i][2] = norm[2];

    gNormPos[i][0] = water[i][0];
    gNormPos[i][1] = water[i][1];
    gNormPos[i][2] = water[i][2];
  }

  /* Werte dem Vertexarray hinzufügen */
  for (int i = 0; i < numberOfVertices; i++) {
    setVertex(Vertices, i, water[i], color[i], tex[i], gNormals[i]);
  }
  /* Vertex Array Pointer setzen */
  glVertexPointer(3, GL_DOUBLE, sizeof(Vertex), &(Vertices[0][CX]));
  glColorPointer(3, GL_DOUBLE, sizeof(Vertex), &(Vertices[0][CR]));
  glTexCoordPointer(2, GL_DOUBLE, sizeof(Vertex), &(Vertices[0][CS]));
  glNormalPointer(GL_DOUBLE, sizeof(Vertex), &(Vertices[0][CNX]));

  GLuint Indices[factor - 1][factor - 1][6]; // Anzahl der Reihen,  Anzahl der
                                             // Dreiecke, Anzahl der Vertices

  for (int y = 0; y < factor - 1; y++) {
    for (int x = 0; x < factor - 1; x++) {
      Indices[y][x][0] = (y * factor) + factor + x;
      Indices[y][x][1] = (y * factor) + x + 1;
      Indices[y][x][2] = (y * factor) + x;

      Indices[y][x][3] = (y * factor) + factor + x;
      Indices[y][x][4] = (y * factor) + factor + x + 1;
      Indices[y][x][5] = (y * factor) + x + 1;
    }
  }

  for (int i = 0; i < factor - 1; i++) {
    glDrawElements(GL_TRIANGLES, (factor - 1) * 6, GL_UNSIGNED_INT, Indices[i]);
  }
  free(Vertices);
  Vertices = NULL;
}

/**
 * Zeichnet eine Seite von der Skybox
 * @param side: Positionen der Vertecies
 * @param tex: Textur Koordinaten
 */
static void drawSide(Pos *side, Tex *tex, Pos norm) {
  glBegin(GL_QUADS);
  {
    for (int i = 0; i < 4; i++) {
      glTexCoord2f(tex[i][0], tex[i][1]);
      glNormal3f(norm[0], norm[1], norm[2]);
      glVertex3f(side[i][0], side[i][1], side[i][2]);
    }
  }
  glEnd();
}

/**
 * Zeichnet einen großen Würfel mit Himmelstextur um die gesamte Szene
 */
void drawSkyBox(void) {

  Pos top[] = {{-0.5f, 0.5f, -0.5f},
               {0.5f, 0.5f, -0.5f},
               {0.5f, 0.5f, 0.5f},
               {-0.5f, 0.5f, 0.5f}};
  Pos bottom[] = {{0.5f, -0.5f, -0.5f},
                  {-0.5f, -0.5f, -0.5f},
                  {-0.5f, -0.5f, 0.5f},
                  {0.5f, -0.5f, 0.5f}};
  Pos front[] = {{0.5f, 0.5f, -0.5f},
                 {-0.5f, 0.5f, -0.5f},
                 {-0.5f, -0.5f, -0.5f},
                 {0.5f, -0.5f, -0.5f}};
  Pos back[] = {{-0.5f, 0.5f, 0.5f},
                {0.5f, 0.5f, 0.5f},
                {0.5f, -0.5f, 0.5f},
                {-0.5f, -0.5f, 0.5f}};
  Pos left[] = {{0.5f, 0.5f, 0.5f},
                {0.5f, 0.5f, -0.5f},
                {0.5f, -0.5f, -0.5f},
                {0.5f, -0.5f, 0.5f}};
  Pos right[] = {{-0.5f, 0.5f, -0.5f},
                 {-0.5f, 0.5f, 0.5f},
                 {-0.5f, -0.5f, 0.5f},
                 {-0.5f, -0.5f, -0.5f}};

  Pos normals[] = {{0.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f},
                   {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 1.0f},
                   {1.0f, 0.0f, 0.0f},  {-1.0f, 0.0f, 0.0f}};

  Pos *sides[6];
  sides[0] = top;
  sides[1] = bottom;
  sides[2] = front;
  sides[3] = back;
  sides[4] = left;
  sides[5] = right;

  Tex face[] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}};

  glPushMatrix();
  {

    glColor3f(1.0f, 1.0f, 1.0f);
    glScalef(SKYBOX_SCALE, SKYBOX_SCALE, SKYBOX_SCALE);

    for (int i = 0; i < 6; i++) {
      drawSide(sides[i], face, normals[i]);
    }
  }
  glPopMatrix();
}
