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
#include <time.h>

#define WIN_TITLE  "life"	/* Window title */
#define WIN_WIDTH  800		/* Initial window width */
#define WIN_HEIGHT 600		/* Initial window height */
#define COLOR_FG   0x000000	/* Foreground color */
#define COLOR_BG   0xffffff	/* Background color */

static u16 s_siz = 6;		/* Width and height of cell in px */
static f32 s_delay = 0.15;	/* Delay between updates in seconds */
static u8  s_pause = 0;		/* Pause game when non 0 */

static void
update(Life *life)
{
	life_next(life);
}

static void
draw(Display *disp, Drawable win, GC gc, Life *life)
{
	u16 x, y;
	XClearWindow(disp, win);
	XSetForeground(disp, gc, COLOR_FG);
	for (y=0; y < life->h; y++)
	for (x=0; x < life->w; x++) {
		if (!life->arr[life->i][y][x]) {
			continue;
		}
		XFillRectangle(disp, win, gc, x*s_siz, y*s_siz, s_siz, s_siz);
	}
}

static void
resize(Display *disp, Drawable win, Life *life)
{
	XWindowAttributes wa;
	XGetWindowAttributes(disp, win, &wa);
	life_resize(life, wa.width/s_siz, wa.height/s_siz);
}

int
main(void)
{
	clock_t  last;		/* Last update time */
	clock_t  time;		/* Current time */
	Life     life = {0};	/* Game of life instance */
	u8	 quit;		/* Non 0 value it will quit window */
	Display *disp;		/* X display */
	Window   win;		/* Window */
	Atom     wmdel;		/* WM delete window atom */
	XEvent	 event;		/* For capturing window events */
	GC       gc;		/* Graphical context */
	u16     *cell;

	if ((disp = XOpenDisplay(0)) == 0) {
		fprintf(stderr, "ERR: Could not open defult disply");
		return 1;
	}
	win = XCreateSimpleWindow(disp, DefaultRootWindow(disp),
				  0, 0, WIN_WIDTH, WIN_HEIGHT,
				  0, 0, COLOR_BG);
	wmdel = XInternAtom(disp, "WM_DELETE_WINDOW", 0);
	gc = XCreateGC(disp, win, 0, 0);
	XStoreName(disp, win, WIN_TITLE);
	XSetWMProtocols(disp, win, &wmdel, 1);
	XSelectInput(disp, win, ExposureMask | ButtonPressMask | KeyPressMask | StructureNotifyMask);
	XMapWindow(disp, win);	/* Show window */
	resize(disp, win, &life);
	life_rand(&life);
	last = 0;
	quit = 0;
	while (quit == 0) {
		if (XPending(disp) == 0) {
			time = clock();
			if (s_pause || time - last < CLOCKS_PER_SEC*s_delay) {
				continue;
			}
			last = time;
			update(&life);
			draw(disp, win, gc, &life);
			continue;
		}
		XNextEvent(disp, &event);
		if (XFilterEvent(&event, None)) {
			continue;
		}
		switch (event.type) {
		case Expose:
			if (event.xexpose.count) {
				continue;
			}
			draw(disp, win, gc, &life);
			continue;
		case ButtonPress:
			/* Toggle cell with mouse click */
			cell = &life.arr[life.i][event.xbutton.y/s_siz][event.xbutton.x/s_siz];
			*cell = !*cell;
			draw(disp, win, gc, &life);
			continue;
		case KeyPress:
			switch (XLookupKeysym(&event.xkey, 0)) {
			case ' ': /* Start/stop */
				s_pause = !s_pause;
				continue;
			case 'r': /* Random */
				life_rand(&life);
				draw(disp, win, gc, &life);
				continue;
			case '[': /* Slow down */
				s_delay *= 2;
				continue;
			case ']': /* Speed up */
				s_delay /= 2;
				continue;
			case '=': /* Scale up */
				s_siz += 1;
				resize(disp, win, &life);
				draw(disp, win, gc, &life);
				continue;
			case '-': /* Scale down */
				if (s_siz <= 1) {
					continue;
				}
				s_siz -= 1;
				resize(disp, win, &life);
				draw(disp, win, gc, &life);
				continue;
			case 0xff1b: /* Esc */
			case 'q':
				quit = 1; /* Quit */
				continue;
			}
			continue;
		case ConfigureNotify:
			resize(disp, win, &life);
			continue;
		case ClientMessage:
			if ((Atom)event.xclient.data.l[0] == wmdel) {
				quit = 1;
			}
			continue;
		}
	}
	XFreeGC(disp, gc);
	XCloseDisplay(disp);
	XDestroyWindow(disp, win);
	return 0;
}
