#ifndef __MATERIALS_H_
#define __MATERIALS_H_

typedef struct stMaterial {
  float ambient[4];
  float diffuse[4];
  float specular[4];
  float shininess[1];
} stMaterial;

typedef enum clMaterial {
  RED,
  WHITE,
  GREY,
  SAND,
  WATER,
  GREEN,
  AVATAR1,
  AVATAR2,
  AVATAR3
} clMaterial;

void setMaterial(clMaterial material);

#endif // __MATERIALS_H_
