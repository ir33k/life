/**
 * Conway's Game of Life
 * https://en.wikipedia.org/wiki/Conway's_Game_of_Life
 *
 *	$ ./build
 *	$ ./life.x
 */
#include "life.h"
#include "type.h"
#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TITLE   "life"		/* Window title */
#define HEIGHT  600		/* Initial window height */
#define WIDTH   800		/* Initial window width */
#define C_BG    0xffffff	/* Background color */
#define C_FG    0x000000	/* Foreground color */

static u16      s_siz = 6;	/* Width and height of cell in px */
static f32      s_delay = 0.15;	/* Delay between updates in seconds */
static u8       s_pause = 0;	/* Pause game when non 0 */
static Life     s_game = {0};	/* Game of life instance */
static Display *s_disp;		/* X display */
static Window   s_win;		/* Window */
static GC       s_gc;		/* Graphical context */

static void
update(void)
{
	life_next(&s_game);
}

static void
draw(void)
{
	u16 x, y;
	XClearWindow(s_disp, s_win);
	XSetForeground(s_disp, s_gc, C_FG);
	for (y=0; y < s_game._h; y++)
	for (x=0; x < s_game._w; x++) {
		if (!s_game.arr[s_game._i][y][x]) {
			continue;
		}
		XFillRectangle(s_disp, s_win, s_gc, x*s_siz, y*s_siz, s_siz, s_siz);
	}
}

/* Set game of life board size using X window size. */
static void
resize(void)
{
	XWindowAttributes wa;
	XGetWindowAttributes(s_disp, s_win, &wa);
	life_resize(&s_game, wa.width/s_siz, wa.height/s_siz);
}

static void
quit(void)
{
	XFreeGC(s_disp, s_gc);
	XDestroyWindow(s_disp, s_win);
	XCloseDisplay(s_disp);
	exit(0);
}

int
main(void)
{
	clock_t  next = 0;	/* Time of next update */
	clock_t  time;		/* Current CPU clock time */
	u16     *cell;		/* Pointer to single Life cell */
	Atom     wmdel;		/* WM delete window atom */
	XEvent	 event;		/* For capturing window events */

	if ((s_disp = XOpenDisplay(0)) == 0) {
		fprintf(stderr, "ERR: Could not open defult disply");
		return 1;
	}
	s_win = XCreateSimpleWindow(s_disp, DefaultRootWindow(s_disp),
				  0, 0, WIDTH, HEIGHT,
				  0, 0, C_BG);
	wmdel = XInternAtom(s_disp, "WM_DELETE_WINDOW", 0);
	s_gc = XCreateGC(s_disp, s_win, 0, 0);
	XStoreName(s_disp, s_win, TITLE);
	XSetWMProtocols(s_disp, s_win, &wmdel, 1);
	XSelectInput(s_disp, s_win,
		     ExposureMask | ButtonPressMask | KeyPressMask |
		     StructureNotifyMask);
	XMapWindow(s_disp, s_win); /* Show window */
	resize();
	life_rand(&s_game);
	while (1) {
		if (XPending(s_disp) == 0) {
			if (s_pause) {
				continue;
			}
			if ((time = clock()) < next) {
				continue;
			}
			next = time + CLOCKS_PER_SEC*s_delay;
			update();
			draw();
			continue;
		}
		XNextEvent(s_disp, &event);
		if (XFilterEvent(&event, None)) {
			continue;
		}
		switch (event.type) {
		case Expose:
			if (event.xexpose.count) {
				break;
			}
			draw();
			break;
		case ButtonPress:
			/* Toggle cell with mouse click */
			cell = &s_game.arr[s_game._i][event.xbutton.y/s_siz][event.xbutton.x/s_siz];
			*cell = !*cell;
			draw();
			break;
		case KeyPress:
			switch (XLookupKeysym(&event.xkey, 0)) {
			case ' ': /* Start/stop */
				s_pause = !s_pause;
				break;
			case 'r': /* Random */
				life_rand(&s_game);
				break;
			case 'c': /* clear */
				life_clear(&s_game);
				break;
			case '[': /* Slow down */
				s_delay *= 2;
				break;
			case ']': /* Speed up */
				s_delay /= 2;
				break;
			case '=': /* Scale up */
				s_siz += 1;
				resize();
				break;
			case '-': /* Scale down */
				if (s_siz <= 1) {
					break;
				}
				s_siz -= 1;
				resize();
				break;
			case 0xff1b: /* Esc */
			case 'q':
				quit();
				break;
			}
			draw();
			break;
		case ConfigureNotify:
			resize();
			break;
		case ClientMessage:
			if ((Atom)event.xclient.data.l[0] == wmdel) {
				quit();
			}
			break;
		}
	}
	return 0;
}
