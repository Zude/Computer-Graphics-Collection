#ifndef __SCENE_H_
#define __SCENE_H_

/**
 * Bereitet die Szene vor.
 */
void initScene(void);

/**
 * Zeichen-Funktion.
 * Stellt die Szene dar.
 * Ausgabe eines Rechtecks.
 */
void drawScene(void);

/**
 * Alle Daten eines Vertexes.
 */
typedef struct {
    float x, y, z; /**< Position */
    float s, t;    /**< Textur-Koordinate */
} Vertex;


GLboolean gPause;
GLboolean gSine;
GLboolean gTex;
GLboolean gHeightBool;
GLboolean gShadingModel;
GLboolean gDebug;

#endif // __SCENE_H_
