/* ---- Eigene Header einbinden ---- */

#include "logic.h"
#include "levels.h"
#include "stdio.h"
#include "types.h"

/* ---- Konstanten ---- */

// Zeit die der Spieler zwischen den Zuegen warten muss
#define PLAYER_STEP_TIME 0.2f
#define WATER_COUNTDOWN 3.0f
#define SUGAR_COUNTDOWN 1.0f

/* ---- Variablen ---- */

extern GLboolean Usage;
extern GLboolean Pause;

// Aktuelles Level wie von der IO übergeben, zum reseten des Levels
int cLevel = '1';

// Der Spieler bzw. seine Position
Avatar player = {1, 1};

// Aktuelle Level
levelPointer currentLevel;

// Timer fuer die Zug Wartezeit
double currentStepTime;

// Bool ob das Bewegen des Spielers (abhg von der Wartezeit) gestattet ist
GLboolean isStepAllowed = GL_TRUE;

// Bool ob der Spieler tot ist
GLboolean isPlayerDead = GL_FALSE;

// Bool ob der Spieler das level gewonnen hat
GLboolean win = GL_FALSE;

SugarList *head = NULL;

WaterList *wlHead = NULL;

/* ---- Hilfsfunktionen ---- */

/**
 * Setzt die neue Position des Spielers
 * @param x,y: Koordinaten wo gesetzt werden soll
 */
void setPlayerAt(int x, int y) {
  player.x = x;
  player.y = y;
}

/**
 * Gibt den Typen des Levels an den uebergebenden Koordinaten zurueck
 * @param x,y: Koordinaten
 * @param level: Spiellevel
 * @return typ des feldes
 */
lvFieldType getFieldAt(int x, int y) {
  if (x >= 0 && x < LEVELSIZE && y >= 0 && y < LEVELSIZE) {
    return currentLevel[y][x];
  } else {
    return LV_INVALID;
  }
}

/**
 * Setzt das ein spezielles Feld in dem Level
 * @param x,y: Kooridinaten des zu setzendem Feldes
 * @param type: Der Feldtyp der gesetzt werden soll
 */
void setFieldAt(int x, int y, lvFieldType type) { currentLevel[y][x] = type; }

// Gibt zurueck ob der Spieler gewonnen hat
GLboolean getPlayerWin() { return win; }

/**
 * Überprüft ob ein Feld mit den Koordinaten
 * bereits vorhanden ist
 * @param x,y: Koordinaten auf dem Spielfeld
 */
GLboolean inSugarList(int x, int y) {
  SugarList *curr = head;
  while (curr) {
    if (curr->x == x && curr->y == y) {
      return GL_TRUE;
    }
    curr = curr->next;
  }
  return GL_FALSE;
}

/**
 * Neues Zuckerfeld der Liste hinzufügen und
 * auf dem Spielfeld platziern
 * @param x,y: Koordinaten auf dem Spielfeld
 */
void addSugar(int x, int y) {
  if (!inSugarList(x, y)) {  // Keine duplikate zulassen

    setFieldAt(x, y, LV_TOUCHED_SUGAR);

    SugarList *node = malloc(sizeof(*node));
    node->x = x;
    node->y = y;
    node->timer = SUGAR_COUNTDOWN;
    node->next = head;

    head = node;
  }
}

/**
 * Speicher der SugarList freigeben
 */
void freeSugar() {
  while (head) {
    SugarList *node = head;
    head = head->next;
    free(node);
  }
}

/**
 * Findet den Zustand vom Timer eines Zuckerfeldes
 * @param x,y: Koordinaten auf dem Spielfeld
 */
double sugarTimerAt(int x, int y) {
  SugarList *curr = head;
  while (curr) {
    if (curr->x == x && curr->y == y) {
      return curr->timer;
    }
    curr = curr->next;
  }
  // Sollte nicht vorkommen, vollen Timer zurückgeben
  return SUGAR_COUNTDOWN;
}

/**
 * Gehe über die gesamte Liste, erhöhe den Timer, entferne
 * abgelaufene Elemente von der Liste und vom Spielfeld
 */
void advanceSugarTimer(double interval) {
  SugarList *curr, *prev, *rem;
  prev = NULL;

  curr = head;
  while (curr != NULL) {
    if (curr->timer < 0.0f) {
      // Noch kein vorheriges Element
      if (prev == NULL) {
        head = curr->next;
      } else {
        prev->next = curr->next;
      }
      // Spielfeld setzen und Speicher freigeben
      setFieldAt(curr->x, curr->y, LV_FREE);
      rem = curr;
      curr = curr->next;
      free(rem);
    } else {
      // Erhöhe den Timer
      curr->timer = curr->timer - interval;
      curr = curr->next;
    }
  }
}

/**
 * Zaehlt die Zeit hoch um die Pause zwischen Spielerbewegungen freizugeben
 * @param interval ist die Zeit seit letzem Aufruf
 */
void movementTimer(double interval) {
  if (!isStepAllowed) {
    currentStepTime += interval;
    if (currentStepTime >= PLAYER_STEP_TIME) {
      isStepAllowed = GL_TRUE;
      currentStepTime = 0.0f;
    }
  }
}

/**
 * Gibt den Typen des Feldes ausgehend von x,y in Richtung direction eines
 * currLevel zurueck
 * @param x,y: Die x bzw y Koordinate von der wir schauen
 * @param direction: Die Richtung des Neuen Feldes
 * @param currLevel: Das Level in dem wir suchen
 */
fieldInformation getNextField(int x, int y, CGDirection direction) {
  fieldInformation returnField;

  // Abhaengig von der direction ueberpruefen wir ob die Neue position zuleassig
  // ist
  switch (direction) {
    case dirLeft: {
      if (x - 1 >= 0) {
        returnField.x = x - 1;
        returnField.y = y;
        returnField.type = getFieldAt(x - 1, y);
        return returnField;
      } else {
        returnField.x = x - 1;
        returnField.y = y;
        returnField.type = LV_INVALID;
        return returnField;
      }

      break;
    }

    case dirRight: {
      if (x + 1 < LEVELSIZE) {
        returnField.x = x + 1;
        returnField.y = y;
        returnField.type = getFieldAt(x + 1, y);
        ;
        return returnField;
      } else {
        returnField.x = x + 1;
        returnField.y = y;
        returnField.type = LV_INVALID;
        return returnField;
      }

      break;
    }

    case dirUp: {
      if (y - 1 >= 0) {
        returnField.x = x;
        returnField.y = y - 1;
        returnField.type = getFieldAt(x, y - 1);
        ;
        return returnField;
      } else {
        returnField.x = x;
        returnField.y = y - 1;
        returnField.type = LV_INVALID;
        return returnField;
      }

      break;
    }

    case dirDown: {
      if (y + 1 < LEVELSIZE) {
        returnField.x = x;
        returnField.y = y + 1;
        returnField.type = getFieldAt(x, y + 1);
        ;
        return returnField;
      } else {
        returnField.x = x;
        returnField.y = y + 1;
        returnField.type = LV_INVALID;
        return returnField;
      }
    }
    default:
      returnField.x = x;
      returnField.y = y;
      returnField.type = LV_INVALID;
      return returnField;
  }
}

/**
 * Fuehrt die Bewegung des Spielers und die eventuellen Verschiebungen aus
 * @param direction: Die Bewegungsrichtung
 * @param nextField: Die Informationen des Feldes wo der Spieler hin moechte
 * @param leve: Der Pointer auf das Aktuelle Level
 * @param player: Der Pointer auf den Spieler
 */
void performPlayerMovement(CGDirection direction, fieldInformation nextField,
                           Avatar *player) {
  // Falls das angestrebte Feld FREE oder Target ist
  if (nextField.type == LV_FREE || nextField.type == LV_TARGET) {
    // Setzt die Position des Spielers neu
    setPlayerAt(nextField.x, nextField.y);
    // Fall fuer Sand
  } else if (nextField.type == LV_SAND) {
    // Speichert das Feld hinter dem Sandsack
    fieldInformation behindSand;
    behindSand = getNextField(nextField.x, nextField.y, direction);

    // Setzt das Feld Hinter dem Sandsack als Sandsack
    setFieldAt(behindSand.x, behindSand.y, LV_SAND);

    // Setzt das Feld vom Sandsack als Free
    setFieldAt(nextField.x, nextField.y, LV_FREE);

    // Setzt die Position des Spielers neu
    setPlayerAt(nextField.x, nextField.y);
  }
}

/**
 * Ueberprueft ob die angestrebte Bewegung moeglich ist
 * @param x,y: Position von der wir ausgehen
 * @param direction: Richtung der angestrebten Bewegung
 * @param level: Das Level in dem wir nachschauen
 * @return Bool ob die bewegung Zuleassig ist oder eben nicht
 */
GLboolean isMovementPossible(int x, int y, CGDirection direction,
                             fieldInformation *fieldInfomationReturn) {
  // Feldtyp des angestrebten Feldes
  fieldInformation fieldToMove = getNextField(x, y, direction);

  // Wenn der Typ des Feldes nicht invalid ist
  if (fieldToMove.type) {
    // Ist das Feld  Free oder Target?
    if (fieldToMove.type == LV_FREE || fieldToMove.type == LV_TARGET) {
      (*fieldInfomationReturn) = fieldToMove;
      return GL_TRUE;
      // Ist es Sand
    } else if (fieldToMove.type == LV_SAND) {
      fieldInformation fieldBehindSand =
          getNextField(fieldToMove.x, fieldToMove.y, direction);
      // Das Feld Hinter dem Sandsack ueberpruefen
      if (fieldBehindSand.type == LV_FREE ||
          fieldBehindSand.type == LV_NEWWATER ||
          fieldBehindSand.type == LV_WATER) {
        (*fieldInfomationReturn) = fieldToMove;
        return GL_TRUE;
      } else {
        (*fieldInfomationReturn) = fieldToMove;
        return GL_FALSE;
      }
    }
  }
  (*fieldInfomationReturn) = fieldToMove;
  return GL_FALSE;
}

GLboolean inWaterList(int x, int y) {
  WaterList *curr = wlHead;
  while (curr) {
    if (curr->x == x && curr->y == y) {
      return GL_TRUE;
    }
    curr = curr->next;
  }
  return GL_FALSE;
}

/**
 * Fügt Wasserfeld der Liste hinzu
 */
void addWater(int x, int y) {
  if (!inWaterList(x, y)) {  // Keine duplikate zulassen
    setFieldAt(x, y, LV_NEWWATER);
    WaterList *node = malloc(sizeof(*node));
    node->x = x;
    node->y = y;
    node->next = wlHead;
    wlHead = node;
  }
}

/**
 * Speicher der gesamten Liste freigeben
 */
void freeWater() {
  while (wlHead) {
    WaterList *node = wlHead;
    wlHead = wlHead->next;
    free(node);
  }
}

/**
 * Platziere alle gespeicherten Felder, falls
 * der Timer abgelaufen ist
 */
void placeNewWater() {
  WaterList *curr = wlHead;
  while (curr) {
    setFieldAt(curr->x, curr->y, LV_WATER);
    /* printf("Platziere Wasser X:%d Y:%d \n", curr->x, curr->y); */
    curr = curr->next;
  }
  freeWater();  // Nun veraltete Elemente entfernen
  /* printf("C: %f \n", currentWaterCountdown); */
}

/**
 * Berechnet das Spawnen vom neuem Wasser.
 * Wenn ein Wasserfeld gefunden wird, so wird geschaut ob neue gespawnt werden
 * muessen
 */
void updateWater(double interval) {
  static double waterTimer = WATER_COUNTDOWN;
  /* Wasser nur nach ablauf des Timers aktuallisieren
   * NewWater platzieren
   * Zucker aktuallisieren
   */
  if (waterTimer < 0.0f) {
    for (int y = 0; y < LEVELSIZE; y++) {
      for (int x = 0; x < LEVELSIZE; x++) {
        if (getFieldAt(x, y) == LV_WATER) {
          fieldInformation nextField = getNextField(x, y, dirLeft);
          if (nextField.type == LV_FREE) {
            addWater(nextField.x, nextField.y);
          } else if (nextField.type == LV_SUGAR) {
            addSugar(nextField.x, nextField.y);
          }
          nextField = getNextField(x, y, dirUp);
          if (nextField.type == LV_FREE) {
            addWater(nextField.x, nextField.y);
          } else if (nextField.type == LV_SUGAR) {
            addSugar(nextField.x, nextField.y);
          }
          nextField = getNextField(x, y, dirRight);
          if (nextField.type == LV_FREE) {
            addWater(nextField.x, nextField.y);
          } else if (nextField.type == LV_SUGAR) {
            addSugar(nextField.x, nextField.y);
          }
          nextField = getNextField(x, y, dirDown);
          if (nextField.type == LV_FREE) {
            addWater(nextField.x, nextField.y);
          } else if (nextField.type == LV_SUGAR) {
            addSugar(nextField.x, nextField.y);
          }
        }
      }
    }
    placeNewWater();
    waterTimer = WATER_COUNTDOWN;
  } else {
    waterTimer = waterTimer - interval;
  }
}

/**
 * Checkt ob der Spieler noch lebt oder nicht
 */
void checkPlayerAlive() {
  lvFieldType playerField = getFieldAt(player.x, player.y);
  if (playerField == LV_WATER || playerField == LV_NEWWATER) {
    isPlayerDead = GL_TRUE;
  }
}

/**
 * Checkt ob der Spieler noch lebt oder nicht
 */
void checkPlayerWin() {
  lvFieldType playerField = getFieldAt(player.x, player.y);
  if (playerField == LV_TARGET) {
    win = GL_TRUE;
  }
}
/**
 * Resetet alle Globalen Variablen
 */
void newGame() {
  // Startpunkt ist immer an 1,1
  setPlayerAt(1, 1);

  currentStepTime = 0.0f;

  isStepAllowed = GL_TRUE;
  isPlayerDead = GL_FALSE;
  win = GL_FALSE;

  freeSugar();
  freeWater();
}

/* ---- Globale Funktionen ---- */
int getPlayerX() { return player.x; }

int getPlayerY() { return player.y; }

levelPointer getCurrentLevel() { return currentLevel; }

void movePlayer(CGDirection inputDir) {
  // Wenn zwischen der Letzen Bewegung mindestens PLAYER_STEP_TIME vergangen ist
  if (isStepAllowed && !Pause && !Usage) {
    // Bewegungserlaubnis auf false setzen (fuer die naechste Bewegung)
    isStepAllowed = GL_FALSE;
    fieldInformation empty = {0, 0, LV_INVALID};
    // Speichert die Informationen des Zielfeldes
    fieldInformation *nextField = &empty;

    // Wenn eine Bewegung zulaessig ist, dann mache Bewegung
    if (isMovementPossible(player.x, player.y, inputDir, nextField)) {
      performPlayerMovement(inputDir, (*nextField), (&player));
    }
  }
}

/**
 * Updated alle Elemente der Logik
 */
void updateLogic(double interval) {
  // Zaehlt den Timer fuer die Movementpause hoch
  checkPlayerAlive();
  checkPlayerWin();
  movementTimer(interval);
  updateWater(interval);

  advanceSugarTimer(interval);
  // updateSugar(interval);
}

void initLv(int level) {
  cLevel = level;  // Fürs einfache neustarten
  currentLevel = loadLevel(level);
  newGame();
}

void resetLevel() { initLv(cLevel); }

GLboolean isAlive() { return !isPlayerDead; }

/**
 * Lädt das nächste Level, falls der Spieler gewonnen hat
 * Falls es das letzte Level war, wird das erste geladen
 */
void nextLevel() {
  if (win) {
    if (cLevel >= LEVELCOUNT) {
      initLv('1');
    } else {
      initLv(cLevel + 1);
    }
  }
}
