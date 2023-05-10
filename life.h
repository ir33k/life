/**
 * Conway's Game of Life
 * https://en.wikipedia.org/wiki/Conway's_Game_of_Life
 */
#ifndef _LIFE_H
#define _LIFE_H

#include "type.h"

#define LIFE_MAX	1024	/* Max board width and height */

struct life {
	u16     w,h;		/* Board width and height */
	u16     arr[2][LIFE_MAX][LIFE_MAX]; /* Boards */
	u8      i;		/* Index of current board */
};

/* Calculate next game of life board state in given LIFE context. */
void life_next(struct life *life);

/*  */
void life_rand(struct life *life);

#endif /* _LIFE_H */
