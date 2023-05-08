/**
 * Conway's Game of Life
 * https://en.wikipedia.org/wiki/Conway's_Game_of_Life
 */
#include <stdio.h>
#include <stdlib.h>
#include "main.h"

static u8
cell_neighbours(i32 x, i32 y)
{
	static const i8 map_y[8] = {-1, -1, -1, +0, +0, +1, +1, +1};
	static const i8 map_x[8] = {-1, +0, +1, -1, +1, -1, +0, +1};
	u8 res = 0;		/* Result, number of neighbours */
	i32 i, dx, dy;		/* Index and delta coordinates */
	for (i=0; i<8; i++) {
		dy = y + map_y[i];
		dx = x + map_x[i];
		if (dy < 0 || dy >= SIZ_Y) continue;
		if (dx < 0 || dx >= SIZ_X) continue;
		res += s_board[s_i][dy][dx];
	}
	return res;
}

static void
board_next(void)
{
	u8 next = !s_i;		/* Index of next board */
	u8 nth;			/* Number of neighbours */
	u8 alive;		/* Non 0 when cell is alive */
	i32 x,y;
	for (y=0; y < SIZ_Y; y++)
	for (x=0; x < SIZ_X; x++) {
		nth = cell_neighbours(x, y);
		alive = s_board[s_i][y][x];
		s_board[next][y][x] = nth == 3 || (alive && nth == 2);
	}
	s_i = next;		/* Switch boards */
}

static void
board_print(FILE *out)
{
	i32 x,y;
	for (y=0; y < SIZ_Y; y++) {
		for (x=0; x < SIZ_X; x++) {
			fputs(s_board[s_i][y][x] ? "# " : ". ", out);
		}
		fputs("\b\n", out);
	}
}

int
main(int argc, char **argv)
{
	u32 i, steps = argc > 1 ? atoi(argv[1]) : 5;

	/* Glaider */
	s_board[s_i][1][2] = 1;
	s_board[s_i][2][3] = 1;
	s_board[s_i][2][4] = 1;
	s_board[s_i][3][2] = 1;
	s_board[s_i][3][3] = 1;

	for (i=0; i < steps; i++) {
		printf("%d\n", i);
		board_print(stdout);
		board_next();
	}
	return 0;
}
