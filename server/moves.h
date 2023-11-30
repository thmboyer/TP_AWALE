#ifndef MOVES_H
#define MOVES_H

typedef struct Move {
  char player[10];
  int value;
  struct Move *next;
  struct Move *previous;
} Move;

typedef struct Moves {
  Move *first;
  int size;
  Move *last;
} Moves;

#endif
