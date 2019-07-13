#ifndef __SCENE_H__
#define __SCENE_H__

/**
 * Zeichen-Funktion.
 * Stellt die Szene dar. Ein weisses Rechteck wird an der vorher im Logik-Modul
 * berechnet Position gezeichnet.
 * Ausgabe eines Quadrats.
 */
void drawScene(void);

/**
 * Initialisierung der Szene (inbesondere der OpenGL-Statusmaschine).
 * Setzt Hintergrund- und Zeichenfarbe.
 * @return Rueckgabewert: im Fehlerfall 0, sonst 1.
 */
int initScene(void);

/**
 * (De-)aktiviert den Wireframe-Modus.
 */
void toggleWireframeMode(void);

#endif
