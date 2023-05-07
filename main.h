#define SIZ_X   12		/* Board width */
#define SIZ_Y    6		/* Baord height */

typedef unsigned char   u8;
typedef unsigned int    u32;
typedef signed   char   i8;
typedef signed   int    i32;

/* 2 boards with S_I index pointing at board with current state.
 * When new board state is calculated S_I changes to second board. */
static u8 s_i = 0;
static u32 s_board[2][SIZ_Y][SIZ_X] = {0};

/* Return number of alive neighbours for X,Y cell. */
static u8 cell_neighbours(i32 x, i32 y);

/* Compute nest board state, BI will point at new board. */
static void board_next(void);

/* Print whole board to OUT file. */
static void board_print(FILE *out);
