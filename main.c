/**
 * Conway's Game of Life
 * https://en.wikipedia.org/wiki/Conway's_Game_of_Life
 */
#include <stdio.h>
#include "main.h"

static u8
cell_neighbours(i32 x, i32 y)
{
	u8 res = 0;		/* Result, number of neighbours */
	i32 i, dx, dy;		/* Index and delta coordinates */
	for (i=0; i<9; i++) {
		dy = y + (i/3 -1);
		dx = x + (i%3 -1);
		if (dy < 0 || dy >= SIZ_Y) continue;
		if (dx < 0 || dx >= SIZ_X) continue;
		res += s_board[s_i][dy][dx];
	}
	return res - s_board[s_i][y][x];
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
main(void)
{
	u32 i;

	/* Glaider */
	s_board[s_i][1][2] = 1;
	s_board[s_i][2][3] = 1;
	s_board[s_i][2][4] = 1;
	s_board[s_i][3][2] = 1;
	s_board[s_i][3][3] = 1;

	for (i=0; i < 5; i++) {
		printf("%d\n", i);
		board_print(stdout);
		board_next();
	}
	return 0;
}
