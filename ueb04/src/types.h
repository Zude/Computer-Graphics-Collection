#ifndef __TYPES_H_
#define __TYPES_H_

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif

GLboolean Pause;
GLboolean Debug;
GLboolean ShowSpheres;
GLboolean ShowNormals;
GLboolean ShowLights;

/* Werte für das Fenster */
#define DEFAULT_WINDOW_WIDTH 900
#define DEFAULT_WINDOW_HEIGHT 900

#define CAMERASPEED 0.1f

/* Begrenzung der vertikalen Kamera bewegung */
/* In Grad zur Y Achse */
#define CAM_UPPER_CUTOFF 2
#define CAM_LOWER_CUTOFF 88

#define CAM_MIN_ZOOM 0.5f
#define CAM_MAX_ZOOM 4.0f

typedef enum { dirDown, dirRight, dirUp, dirLeft } CamDirection;
/* Konstanten und Typen für Vertex Arrays */

#define CX (0)
#define CY (1)
#define CZ (2)
#define CR (3)
#define CG (4)
#define CB (5)
#define CS (6)
#define CT (7)
#define CNX (8)
#define CNY (9)
#define CNZ (10)

typedef GLdouble Vertex[11];

typedef GLdouble Pos[3]; // Für Position der VA nutzen
typedef GLdouble Col[3]; // Für Farben der VA nutzen
typedef GLdouble Tex[2]; // Texture Koordinaten

#define WATER_INIT_FACTOR 4
#define MIN_WATER 4
#define MAX_WATER 100

typedef GLdouble WaterPoint[3];
typedef WaterPoint Water[];

#define ISLAND_SCALE 0.5f
#define SPHERE_SCALE 0.04f
#define SKYBOX_SCALE 3.0f

#endif // __TYPES_H_
