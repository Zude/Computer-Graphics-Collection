/* ---- Eigene Header einbinden ---- */
#include "types.h"

#ifndef __DRAWING3D_H__
#define __DRAWING3D_H__
void drawBrickGap3D(void);
void drawAllBrickGaps3D(void);

void drawAxes3D(void);
void drawSquare3D(GLint subDivs);
void drawTarget3D(void);
void drawQuad3D(void);
void drawAvatar3D(void);
void drawWaterBlock(WaterBlockList node);

void drawWaterDeko(void);
#endif
