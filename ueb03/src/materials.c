/* ---- System Header einbinden ---- */
#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif

#include <string.h>

/* ---- Eigene Header einbinden ---- */
#include "materials.h"

/* Liste mit verschiedenen Beispielen für Materialien: */
/* http://www.it.hiof.no/~borres/j3d/explain/light/p-materials.html */
stMaterial white = { .ambient = { 0.25f, 0.20725f, 0.20725f, 0.922f },
                     .diffuse = { 1.0f, 0.829f, 0.829f, 0.922f },
                     .specular = { 0.296648f, 0.296648f, 0.296648f, 0.922f },
                     .shininess = { 11.264f } };
stMaterial red = { .ambient = { 0.1745f, 0.01175f, 0.01175f, 0.55f },
                   .diffuse = { 0.61424f, 0.04136f, 0.04136f, 0.55f },
                   .specular = { 0.727811f, 0.626959f, 0.626959f, 0.55f },
                   .shininess = { 76.8f } };
stMaterial grey = { .ambient = { 0.25f, 0.25f, 0.25f, 1.0f },
                    .diffuse = { 0.4f, 0.4f, 0.4f, 1.0f },
                    .specular = { 0.774597f, 0.774597f, 0.774597f, 1.0f },
                    .shininess = { 76.8f } };
stMaterial sand = { .ambient = { 0.24725f, 0.1995f, 0.0745f, 1.0f },
                    .diffuse = { 0.75164f, 0.60648f, 0.22648f, 1.0f },
                    .specular = { 0.628281f, 0.555802f, 0.366065f, 1.0f },
                    .shininess = { 51.2f } };
stMaterial water = {
  .ambient = { 0.105882f, 0.058824f, 0.113725f, 0.15f },
  .diffuse = { 6.0f / 255.0f, 39.0f / 255.0f, 91.0f / 255.0f, 0.65f },
  .specular = { 6.0f / 255.0f, 39.0f / 255.0f, 91.0f / 255.0f, 0.65f },
  .shininess = { 9.84615f }
};
stMaterial green = { .ambient = { 0.0215f, 0.1745f, 0.0215f, 0.55f },
                     .diffuse = { 0.07568f, 0.61424f, 0.07568f, 0.55f },
                     .specular = { 0.633f, 0.727811f, 0.633f, 0.55f },
                     .shininess = { 76.8f } };
stMaterial avatar1 = { .ambient = { 0.05f, 0.0f, 0.0f, 1.0f },
                       .diffuse = { 0.5f, 0.4f, 0.4f, 1.0f },
                       .specular = { 0.7f, 0.04f, 0.04f, 1.0f },
                       .shininess = { 10.0f } };
stMaterial avatar2 = { .ambient = { 0.0215f, 0.1745f, 0.0215f, 0.55f },
                       .diffuse = { 0.07568f, 0.61424f, 0.07568f, 0.55f },
                       .specular = { 0.633f, 0.727811f, 0.633f, 0.55f },
                       .shininess = { 76.8f } };
stMaterial avatar3 = { .ambient = { 0.05375f, 0.05f, 0.06625f, 0.82f },
                       .diffuse = { 0.18275f, 0.17f, 0.22525f, 0.82f },
                       .specular = { 0.332741f, 0.328634f, 0.346435f, 0.82f },
                       .shininess = { 38.4f } };

static void
updateMaterial(stMaterial mat)
{
  glMaterialfv(GL_FRONT, GL_AMBIENT, mat.ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat.diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat.specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat.shininess);
}

void
setMaterial(clMaterial material)
{

  switch (material) {
    case WHITE:
      updateMaterial(white);
      break;
    case RED:
      updateMaterial(red);
      break;
    case GREY:
      updateMaterial(grey);
      break;
    case SAND:
      updateMaterial(sand);
      break;
    case WATER:
      updateMaterial(water);
      break;
    case GREEN:
      updateMaterial(green);
      break;
    case AVATAR1:
      updateMaterial(avatar1);
      break;
    case AVATAR2:
      updateMaterial(avatar2);
      break;
    case AVATAR3:
      updateMaterial(avatar3);
      break;
  }
}
