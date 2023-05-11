#include "life.h"
#include <stdlib.h>

/* Return number of alive neighbor for X,Y cell in LIFE context. */
static u8
neighbors(Life *life, u16 x, u16 y)
{
	static const i8 map_y[8] = {-1, -1, -1, +0, +0, +1, +1, +1};
	static const i8 map_x[8] = {-1, +0, +1, -1, +1, -1, +0, +1};
	u8 res = 0;		/* Result, number of neighbours */
	i32 i, dx, dy;		/* Index and delta coordinates */
	for (i=0; i < 8; i++) {
		dy = y + map_y[i];
		dx = x + map_x[i];
		if (dy < 0 || dy >= life->_h) continue;
		if (dx < 0 || dx >= life->_w) continue;
		res += life->arr[life->_i][dy][dx];
	}
	return res;
}

void
life_resize(Life *life, u16 width, u16 height)
{
	life->_w = width  >= LIFE_MAX ? LIFE_MAX-1 : width;
	life->_h = height >= LIFE_MAX ? LIFE_MAX-1 : height;
}

void
life_next(Life *life)
{
	u8 next = !life->_i;	/* Index of next board */
	u8 nth;			/* Number of neighbours */
	u8 alive;		/* Non 0 when cell is alive */
	u16 x,y;
	for (y=0; y < life->_h; y++)
	for (x=0; x < life->_w; x++) {
		nth = neighbors(life, x, y);
		alive = life->arr[life->_i][y][x];
		life->arr[next][y][x] = nth == 3 || (alive && nth == 2);
	}
	life->_i = next;		/* Switch boards */
}

void
life_rand(Life *life)
{
	static i32 half = RAND_MAX/2;
	u16 x,y;
	for (y=0; y < life->_h; y++)
	for (x=0; x < life->_w; x++) {
		life->arr[life->_i][y][x] = rand() > half;
	}
}
