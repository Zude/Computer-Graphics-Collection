/* ---- System Header einbinden ---- */
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>


/* ---- Eigene Header einbinden ---- */
#include "debugGL.h"
#include "io.h"


int
main (int argc, char **argv)
{
	(void) argc;
	(void) argv; 

  /* Initialisierung des I/O-Sytems
     (inkl. Erzeugung des Fensters und Starten der Ereignisbehandlung). */
  if (!initAndStartIO ("Pong | GOTY 2019", 500, 500))
    {
      fprintf (stderr, "Initialisierung fehlgeschlagen!\n");
      return 1;
    }
  else
    {
      return 0;
    }
}

