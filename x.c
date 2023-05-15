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
#define FPS     30		/* Drawing FPS */
#define C_BG    0xffffff	/* Background color */
#define C_FG    0x000000	/* Foreground color */
#define C_MOUSE 0xff0000	/* Mouse cursor color */

/* TODO(irek): Turn all of this into xctx for future easy copy and
 * paste.  I imagine it like this: in some next project I need to
 * render something on the screen.  Then I go here, copy and paste
 * boilerplate and without any knowledge of x11 I only edit some
 * settings like WIDTH, HEIGHT, FPS etc and define UPDATE, DRAW and
 * RESIZE functions along with key bindings and mouse handling. */
static u16      s_siz = 6;	/* Width and height of cell in px */
static f32      s_delay = 0.15;	/* Delay between updates in seconds */
static u8       s_pause = 0;	/* Pause game when non 0 */
static Life     s_game = {0};	/* Game of life instance */
static Display *s_disp;		/* X display */
static Window   s_win;		/* Window */
static Pixmap   s_buf;		/* Pixmap buffer for drawing */
static GC       s_gc;		/* Graphical context */
static XEvent   s_event;	/* For capturing window events */

/* Update game of life state after S_DELAY. */
static void
update(void)
{
	life_next(&s_game);
}

/* Draws window content. */
static void
draw(void)
{
	u16 x, y;
	XSetForeground(s_disp, s_gc, C_BG);
	XFillRectangle(s_disp, s_buf, s_gc, 0, 0, WIDTH, HEIGHT);
	XSetForeground(s_disp, s_gc, C_FG);
	/* Draw game of life grid. */
	for (y=0; y < s_game._h; y++)
	for (x=0; x < s_game._w; x++) {
		if (!s_game.arr[s_game._i][y][x]) {
			continue;
		}
		XFillRectangle(s_disp, s_buf, s_gc, x*s_siz, y*s_siz, s_siz, s_siz);
	}
	/* Draw mouse cursor when paused. */
	if (s_pause) {
 		XSetForeground(s_disp, s_gc, C_MOUSE);
		XSetLineAttributes(s_disp, s_gc, 2, LineSolid, CapNotLast, JoinMiter);
		XDrawRectangle(s_disp, s_buf, s_gc,
			       s_event.xbutton.x - s_event.xbutton.x%s_siz,
			       s_event.xbutton.y - s_event.xbutton.y%s_siz,
			       s_siz, s_siz);
	}
	/* Drawing everything to Pixmap and then using XCopyArea()
	 * eliminates problem of screen tearing entirly even without
	 * double buffering.  Double buffering is implemented for
	 * performance reasons but it is not necessary to fix screen
	 * tearing so this implementation use sinle buffer only. */
	XCopyArea(s_disp, s_buf, s_win, s_gc,
		  0, 0,
		  WIDTH, HEIGHT,
		  0, 0);
}

/* Set game of life board size using window size. */
static void
resize(void)
{
	XWindowAttributes wa;
	XGetWindowAttributes(s_disp, s_win, &wa);
	life_resize(&s_game, wa.width/s_siz, wa.height/s_siz);
}

/* Close window. */
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
	clock_t  next = 0;	/* CPU clock time of next update */
	clock_t  time;		/* Current CPU clock time */
	u16     *cell;		/* Pointer to single Life cell */
	Atom     wmdel;		/* WM delete window atom */
	i32      screen;	/* Default screen */

	if ((s_disp = XOpenDisplay(0)) == 0) {
		fprintf(stderr, "ERR: Could not open defult disply");
		return 1;
	}
	s_win = XCreateSimpleWindow(s_disp, DefaultRootWindow(s_disp),
				  0, 0, WIDTH, HEIGHT,
				  0, 0, C_BG);
	screen = XDefaultScreen(s_disp);
	/* TODO(irek): Width and height are not dynamic, hmm how to adjust that on the fly? */
	s_buf = XCreatePixmap(s_disp, s_win, WIDTH, HEIGHT,
			      DefaultDepth(s_disp, screen));
	wmdel = XInternAtom(s_disp, "WM_DELETE_WINDOW", 0);
	s_gc = XCreateGC(s_disp, s_win, 0, 0);
	XStoreName(s_disp, s_win, TITLE);
	XSetWMProtocols(s_disp, s_win, &wmdel, 1);
	XSelectInput(s_disp, s_win,
		     ButtonPressMask | KeyPressMask |
		     PointerMotionMask | StructureNotifyMask);
	XMapWindow(s_disp, s_win); /* Show window */
	XSync(s_disp, 0);
	resize();	      /* First set game of life board size */
	life_rand(&s_game);   /* Initial game of life board content */
	while (1) {
		while (XPending(s_disp)) {
			XNextEvent(s_disp, &s_event);
			switch (s_event.type) {
			case ButtonPress:
				/* Toggle cell with mouse click. */
				cell = &s_game.arr[s_game._i][s_event.xbutton.y/s_siz][s_event.xbutton.x/s_siz];
				*cell = !*cell;
				break;
			case MotionNotify:
				/* Do nothing here but draw() function
				 * will use mouse coordinates. */
				break;
			case KeyPress:
				switch (XLookupKeysym(&s_event.xkey, 0)) {
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
					next = clock() + CLOCKS_PER_SEC*s_delay;
					break;
				case ']': /* Speed up */
					s_delay /= 2;
					next = clock() + CLOCKS_PER_SEC*s_delay;
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
				break;
			case ConfigureNotify:
				resize();
				break;
			case ClientMessage:
				if ((Atom)s_event.xclient.data.l[0] == wmdel) {
					quit();
				}
				break;
			}
		}
		if ((time = clock()) > next) {
			if (!s_pause) {
				update();
			}
			next = time + CLOCKS_PER_SEC*s_delay;
			draw();
		}
		/* TODO(irek): Problem of 100% CPU usage still remains. */
	}
	return 0;
}
