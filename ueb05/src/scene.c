/* ---- System Header einbinden ---- */
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>  // Bibliothek um Bilder zu laden


/* ---- Eigene Header einbinden ---- */
#include "utility.h"
#include "debugGL.h"
#include "scene.h"


#define SUBDIVS 8 /*Anzahl der Aufteilungen der vordefinierten Dreiecken*/
#define NRVERT (pow(4, SUBDIVS) * 3) * 2 /*Tatsächliche Number an Vertices im Array-Buffer. Start mit 2 Dreicken, die jeweils auf 4 aufgeteilt werden*/

// Zwischenspeichern der vergangenen Zeit fuer den idle Timer
int lastCallTime = 0;

/* ---- Globale Variablen ---- */
/** Buffer-Objekt, um die Vertizes zu speichern. */
static GLuint g_arrayBuffer;
static Vertex * vertices;

/** Vertex-Array-Objekt, um die Attribut-Pointer zu speichern. */
static GLuint g_vertexArrayObject;

/** Programm-Objekt zum Rendern */
static GLuint g_program;

/** Location der uniform-Variable "ModelView" */
static GLint g_locationModelViewMatrix;

   /* ----------------------------------------------- */
  /* ----------------------------------------------- */
 /* Texturen */
static GLuint g_locUpperTex;
static GLuint g_locLowerTex;

static GLuint g_dRed;
static GLuint g_dBlue;
static GLuint g_Snow;
static GLuint g_Land;

static GLuint g_locHeightMap;
static GLuint g_heightmap;

static GLuint g_locTexBool;
static GLuint g_locHeightBool;


   /* ----------------------------------------------- */
  /* ----------------------------------------------- */
 /* Sinuswelle */
static GLuint g_locationSine;
static GLuint g_locationTime;

   /* ----------------------------------------------- */
  /* ----------------------------------------------- */
 /* Normalen */
static GLuint g_locationStepSize;
static GLuint g_locationTexStepSize;

   /* ----------------------------------------------- */
  /* ----------------------------------------------- */
 /* Lichtberechnung */
static GLuint g_locShadingModel;
static GLuint g_locCamPosition;

void
loadTextures(void){
    /* Laden der Textur. */
    int width, height, comp;
    GLubyte* data = stbi_load("../content/textures/blue.png", &width, &height, &comp, 4);

    /* Erstellen des Textur-Objekts. */
    glGenTextures(1, &g_dRed);
    glBindTexture(GL_TEXTURE_2D, g_dRed);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);


    /* Alle texturen Manuell definieren und laden weil es anders aus irgendeinem Grund eben nicht geht */
    data = stbi_load("../content/textures/red.png", &width, &height, &comp, 4);
    glGenTextures(1, &g_dBlue);
    glBindTexture(GL_TEXTURE_2D, g_dBlue);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);


    data = stbi_load("../content/textures/heightmap.png", &width, &height, &comp, 4);
    glGenTextures(1, &g_heightmap);
    glBindTexture(GL_TEXTURE_2D, g_heightmap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);

    data = stbi_load("../content/textures/rock.jpg", &width, &height, &comp, 4);
    glGenTextures(1, &g_Land);
    glBindTexture(GL_TEXTURE_2D, g_Land);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);

    data = stbi_load("../content/textures/snow.jpg", &width, &height, &comp, 4);
    glGenTextures(1, &g_Snow);
    glBindTexture(GL_TEXTURE_2D, g_Snow);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);


}


/**
 * Zeichen-Funktion.
 * Stellt die Szene dar.
 * Ausgabe eines Rechtecks.
 */
void
drawScene (void)
{

  float viewMatrix[16];
  double interval;

  if(!gPause){
  // Aktuell vergangene Zeit nach Programmstart
  double thisCallTime = (double)glutGet(GLUT_ELAPSED_TIME);

  interval = (double)(thisCallTime) / 4000;
  } else {
    interval = 0.0f;
  }

  lastCallTime = (double)glutGet(GLUT_ELAPSED_TIME);

  const float distance = 3;
  float camPos[] = {distance*sinf(interval), 1, distance*cosf(interval)};
  lookAt(camPos[0],camPos[1], camPos[2], 0, 0, 0, 0, 1, 0, viewMatrix);

  /* Aktivieren des Programms. Ab jetzt ist die Fixed-Function-Pipeline
   * inaktiv und die Shader des Programms aktiv. */
  glUseProgram(g_program);

  /* Kamera Position für Licht berechnung */
  glUniform3f(g_locCamPosition,camPos[0],camPos[1],camPos[2]);

  /* Übermitteln der View-Matrix an den Shader.
   * OpenGL arbeitet intern mit Matrizen in column-major-layout. D.h.
   * nicht die Reihen, sondern die Spalten liegen hintereinander im
   * Speicher. Die Funktionen zur Erzeugen von Matrizen in diesem
   * Programm berücksichtigen dies. Deswegen müssen die Matrizen nicht
   * transponiert werden und es kann GL_FALSE übergeben werden. Beim
   * Schreiben eigener Matrix-Funktionen oder beim Verwenden von
   * Mathematik-Bibliotheken muss dies jedoch berücksichtigt werden. */
  glUniformMatrix4fv(g_locationModelViewMatrix, 1, GL_FALSE, viewMatrix);

   /* ----------------------------------------------- */
  /* ----------------------------------------------- */
 /* Texturen */


  glActiveTexture(GL_TEXTURE0);
  if(gDebug){
  glBindTexture(GL_TEXTURE_2D, g_dRed);
  }
  else{
    glBindTexture(GL_TEXTURE_2D, g_Snow);
  }
  glUniform1i(g_locUpperTex, 0);

  glActiveTexture(GL_TEXTURE1);
  if(gDebug){
  glBindTexture(GL_TEXTURE_2D, g_dBlue);
  }
  else{
    glBindTexture(GL_TEXTURE_2D, g_Land);
  }
  
  glUniform1i(g_locLowerTex, 1);

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, g_heightmap);
  glUniform1i(g_locHeightMap, 2);

  //Texture toggle
  glUniform1i(g_locTexBool, gTex);

  glUniform1i(g_locHeightBool, gHeightBool);

  //Heightmap toggle

   /* ----------------------------------------------- */
  /* ----------------------------------------------- */
 /* Werte für die Sinuswelle */
  glUniform1f(g_locationTime, interval);
  glUniform1i(g_locationSine, gSine);

   /* ----------------------------------------------- */
  /* ----------------------------------------------- */
 /* Werte für Normalen berechnung */
  float step = 2.0f/(pow(2,SUBDIVS));
  glUniform1f(g_locationStepSize, step);
  glUniform1f(g_locationTexStepSize, coordToTex(step));


  glUniform1i(g_locShadingModel, gShadingModel);


  /* Aktivieren des Vertex-Array-Objekts (VAO).
   * Hiermit werden alle Attribut-Pointer aktiv, die auf diesem VAO
   * gesetzt wurden. */
  glBindVertexArray(g_vertexArrayObject);

  /* Rendern der Dreiecke.
   * Ab dem ersten Dreieck im Buffer werden alle 8 Dreiecke gerendert.
   * Dem Draw-Command wird jedoch die Anzahl der Vertizes übergeben, die
   * gezeichnet werden sollen. */
  glDrawArrays(GL_TRIANGLES, 0, NRVERT);

  /* Zurücksetzen des OpenGL-Zustands, um Seiteneffekte zu verhindern */
  glBindVertexArray(0);
  glUseProgram(0);
}


static void
addToVertBuffer(float v1[3], float v2[3], float v3[3]) {
  static int index = 0;

  assert(index + 2 < NRVERT);

  vertices[index].x = v1[0];
  vertices[index].y = v1[1];
  vertices[index].z = v1[2];
  vertices[index].s = coordToTex(v1[0]);
  vertices[index].t = coordToTex(v1[2]);

  vertices[index + 1].x = v2[0];
  vertices[index + 1].y = v2[1];
  vertices[index + 1].z = v2[2];
  vertices[index + 1].s = coordToTex(v2[0]);
  vertices[index + 1].t = coordToTex(v2[2]);

  vertices[index + 2].x = v3[0];
  vertices[index + 2].y = v3[1];
  vertices[index + 2].z = v3[2];
  vertices[index + 2].s = coordToTex(v3[0]);
  vertices[index + 2].t = coordToTex(v3[2]);

  index += 3;

}

static void
subdivide(float v1[3], float v2[3], float v3[3], int depth){

  if(depth == 0){
    addToVertBuffer(v1, v2, v3);
    return;
  }
  //Tex coord von mapRange der 3d coords
  float v12[3], v23[3], v31[3];

  for (int i = 0; i < 3; i++) {
    v12[i] = (v1[i] + v2[i])/2;
    v23[i] = (v2[i] + v3[i])/2;
    v31[i] = (v3[i] + v1[i])/2;
  }

  subdivide(v1, v12, v31, depth - 1);
  subdivide(v2, v23, v12, depth - 1);
  subdivide(v3, v31, v23, depth - 1);
  subdivide(v12, v23, v31, depth - 1);

}

/**
 * Bereitet die Szene vor.
 */
void
initScene (void)
{
  /* 2 Dreiecke von denen ausgehen das Mesh erstellt wird */
  float aTri[][3] = {{ -1.0f, 0.0f, 1.0f}, /*4*/
                     { 1.0f, 0.0f, 1.0f}, /*3*/
                     { 1.0f, 0.0f, -1.0f} /*2*/};

  float bTri[][3] = {{ -1.0f, 0.0f, 1.0f}, /*4*/
                     { 1.0f, 0.0f, -1.0f}, /*2*/
                     { -1.0f, 0.0f, -1.0f} /*1*/};

  vertices = malloc(sizeof(Vertex) * NRVERT);

  if(!vertices){exit(1);}

  subdivide(aTri[0], aTri[1], aTri[2], SUBDIVS);
  subdivide(bTri[0], bTri[1], bTri[2], SUBDIVS);

  {
    /* Erstellen eines Buffer-Objektes.
     * In modernem OpenGL werden alle Vertex-Daten in Buffer-Objekten
     * gespeichert. Dabei handelt es sich um Speicherbereiche die von
     * der OpenGL-Implementierung verwaltet werden und typischerweise
     * auf der Grafikkarte angelegt werden.
     *
     * Mit der Funktion glGenBuffers können Namen für Buffer-Objekte
     * erzeugt werden. Mit glBindBuffer werden diese anschließend
     * erzeugt, ohne jedoch einen Speicherbereich für die Nutzdaten
     * anzulegen. Dies geschieht nachfolgend mit einem Aufruf der
     * Funktion glBufferData.
     *
     * Um mit Buffer-Objekten zu arbeiten, mussen diese an Targets
     * gebunden werden. Hier wird das Target GL_ARRAY_BUFFER verwendet.
     *
     * Der OpenGL-Implementierung wird zusätzlich ein Hinweis mitgegeben,
     * wie die Daten eingesetzt werden. Hier wird GL_STATIC_DRAW
     * übergeben. OpenGL kann diesen Hinweis nutzen, um Optimierungen
     * vorzunehmen. */
    glGenBuffers(1, &g_arrayBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, g_arrayBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * NRVERT, vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  {
    const GLuint positionLocation = 0;
    const GLuint texCoordLocation = 1;
    /* Erstellen eines Vertex-Array-Objektes (VAO).
     * Damit die OpenGL-Implementierung weiß welche Daten der Pipeline
     * bereitgestellt werden müssen, werden Attribut-Pointer gesetzt.
     * Um mehrere Datenquellen (unterschiedliche Meshes) zu verwalten
     * können die Attribut-Pointer in VAOs gruppiert werden.
     *
     * Die Erzeugung von VAOs geschieht prinzipiell genauso wie bei
     * Buffern oder anderen OpenGL-Objekten. */
    glGenVertexArrays(1, &g_vertexArrayObject);
    glBindVertexArray(g_vertexArrayObject);

    /* Die Pointer werden immer in den Buffer gesetzt, der am
     * GL_ARRAY_BUFFER-Target gebunden ist! */
    glBindBuffer(GL_ARRAY_BUFFER, g_arrayBuffer);

    /* Im Vertex-Shader existieren folgende Zeilen:
     * > layout (location = 0) in vec4 vPosition;
     * > layout (location = 1) in vec2 vTexCoord;
     *
     * Beim Aufruf einen Draw-Command, müssen diesen beiden Attributen
     * Daten bereitgestellt werden. Diese sollen aus dem oben erstellten
     * Buffer gelesen werden. Dafür müssen zwei Attribut-Pointer aktiviert
     * und eingerichtet werden. */
    glEnableVertexAttribArray(positionLocation);
    glVertexAttribPointer(
        positionLocation,                           /* location (siehe Shader) */
        3,                           /* Dimensionalität */
        GL_FLOAT,                    /* Datentyp im Buffer */
        GL_FALSE,                    /* Keine Normierung notwendig */
        sizeof(Vertex),              /* Offset zum nächsten Vertex */
        (void*)offsetof(Vertex, x)); /* Offset zum ersten Vertex */

    /* Zweiter Attribut-Pointer für die Textur-Koordinate */
    glEnableVertexAttribArray(texCoordLocation);
    glVertexAttribPointer(texCoordLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, s));

    /* Um Programmierfehler leichter zu finden, sollte der OpenGL-
     * Zustand wieder zurückgesetzt werden. Wird beispielweise das Binden
     * eines Vertex-Array-Objekts vergessen werden, arbeitet OpenGL
     * auf dem vorher gebundenen. Vor allem bei starker Modularisierung
     * sind diese Fehler schwer zu finden. */
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

  {
    /* Erstellen des Programms */
    g_program = createProgram("../content/shaders/color.vert", "../content/shaders/color.frag");

    /* Abfragen der uniform-locations.
     * Dies kann auch zur Laufzeit gemacht werden, es ist jedoch
     * performanter dies vorab zu tun, da String-Vergleiche gemacht
     * werden müssen. */
    g_locationModelViewMatrix = glGetUniformLocation(g_program, "ModelView");
    g_locationTime = glGetUniformLocation(g_program, "Time");
    g_locationSine = glGetUniformLocation(g_program, "Sine");
    g_locationStepSize = glGetUniformLocation(g_program, "StepSize");
    g_locationTexStepSize = glGetUniformLocation(g_program, "TexStepSize");


    g_locCamPosition = glGetUniformLocation(g_program, "CamPosition");
    g_locShadingModel = glGetUniformLocation(g_program, "ShadingBool");


    g_locHeightMap = glGetUniformLocation(g_program, "HeightMap");
    g_locUpperTex = glGetUniformLocation(g_program, "UpperTex");
    g_locLowerTex = glGetUniformLocation(g_program, "LowerTex");
    g_locTexBool = glGetUniformLocation(g_program, "TexBool");
    g_locHeightBool = glGetUniformLocation(g_program, "HeightBool");

    /* DEBUG-Ausgabe */
    printf("UpperTex hat 'location': %i\n", g_locUpperTex);
    printf("LowerTex hat 'location': %i\n", g_locLowerTex);
    printf("HeightMap hat 'location': %i\n", g_locHeightMap);

    printf("ModelView hat 'location': %i\n", g_locationModelViewMatrix);
    printf("Time hat 'location': %i\n", g_locationTime);

    printf("Sine hat 'location': %i\n", g_locationSine);
    printf("StepSize hat 'location': %i\n", g_locationStepSize);
    printf("TexStepSize hat 'location': %i\n", g_locationTexStepSize);

  }

  {
    /* Erstellen der Projektions-Matrix.
     * Da die Projektions-Matrix für die Laufzeit des Programms konstant
     * ist, wird sie einmalig gesetzt. */
    float projectionMatrix[16];
    perspective(60, (float)glutGet(GLUT_WINDOW_WIDTH) / glutGet(GLUT_WINDOW_HEIGHT), 0.01f, 5.0f, projectionMatrix);
    glUseProgram(g_program);
    glUniformMatrix4fv(glGetUniformLocation(g_program, "Projection"), 1, GL_FALSE, projectionMatrix);
    glUseProgram(0);
  }

  /*Laden aller Texturen*/
  loadTextures();
}
