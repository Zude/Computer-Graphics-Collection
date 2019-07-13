/* ---- System Header einbinden ---- */
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/* ---- Eigene Header einbinden ---- */
#include "debugGL.h"
#include "io.h"
#include "types.h"

int main(int argc, char **argv) {
  (void)argc;
  (void)argv;

  /* Initialisierung des I/O-Sytems
     (inkl. Erzeugung des Fensters und Starten der Ereignisbehandlung). */
  if (!initAndStartIO("Zu viele Kugeln im Wasser", DEFAULT_WINDOW_WIDTH,
                      DEFAULT_WINDOW_HEIGHT)) {
    fprintf(stderr, "Initialisierung fehlgeschlagen!\n");
    return 1;
  } else {
    return 0;
  }
}
