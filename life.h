/**
 * Conway's Game of Life
 * https://en.wikipedia.org/wiki/Conway's_Game_of_Life
 */
#ifndef _LIFE_H
#define _LIFE_H

#include "type.h"

#define LIFE_MAX	1024	/* Max board width and height */

/* Fields prefixed with "_" are consider readonly. */
typedef struct {
	u16     arr[2][LIFE_MAX][LIFE_MAX]; /* Boards [0-1][y][x] */
	u16     _w, _h;		/* Board width and height */
	u8      _i;		/* Index of current board */
} Life;

/* Change max LIFE WIDTH and HEIGHT. */
void life_resize(Life *life, u16 width, u16 height);

/* Calculate next game of life board state in given LIFE context. */
void life_next(Life *life);

/* Randomize LIFE board data. */
void life_rand(Life *life);

#endif /* _LIFE_H */
