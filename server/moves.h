#if not defined MOVES_H
#define MOVES_H

typedef struct Move {
  char player[10];
  int value;
} Move;

typedef struct Moves {
  Move *move; // Init it with 100 moves, call moves_reallocate() to double size
              // if not enough.
  int next_index;
  int size;
} Moves;

#endif
