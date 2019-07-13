#ifndef __SPHERE_H_
#define __SPHERE_H_

typedef enum { sGREEN, sBLUE, sWHITE, sYELLOW, sRED } sphereColor;

void initSphereDL(int subDivs);

void drawIcoSphere(sphereColor col);

#endif // __SPHERE_H_
