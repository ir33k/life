/**
 * Conway's Game of Life
 * https://en.wikipedia.org/wiki/Conway's_Game_of_Life
 *
 *	$ ./build
 *	$ ./life
 */
#include "life.h"
#include "type.h"
#include <X11/Xlib.h>
#include <stdio.h>
#include <time.h>

#define SIZE 8

static void
draw(Display *disp, Drawable win, GC gc, struct life *life)
{
	u16 x, y;
	XClearWindow(disp, win);
	XSetForeground(disp, gc, 0xffffff);
	for (y=0; y < life->h; y++)
	for (x=0; x < life->w; x++) {
		if (!life->arr[life->i][y][x]) {
			continue;
		}
		XFillRectangle(disp, win, gc,
			       x*SIZE,
			       y*SIZE,
			       SIZE, SIZE);
	}
}

int
main(void)
{
	static const float fps_clock = ((float)CLOCKS_PER_SEC/1000.0) * (1000.0/60.0);
	clock_t  last_frame, last_update, time;
	struct life life = {0};	/* Game of life instance */
	u8	 quit;		/* Non 0 value it will quit window */
	Display *disp;		/* X display */
	Window   root;		/* Parent window */
	Window   win;		/* Window */
	Atom     wmdel;		/* WM delete window atom */
	XEvent	 event;		/* For capturing window events */
	GC       gc;		/* Graphical context */
	XWindowAttributes wa;	/* Window attributes */
	u16     *cell;
	int      update;

	if ((disp = XOpenDisplay(0)) == 0) {
		fprintf(stderr, "ERR: Could not open defult disply");
		return 1;
	}
	root = DefaultRootWindow(disp);
	win = XCreateSimpleWindow(disp, root, 0, 0, 800, 600, 2, 0, 0);
	wmdel = XInternAtom(disp, "WM_DELETE_WINDOW", 0);
	gc = XCreateGC(disp, win, 0, 0);
	XStoreName(disp, win, "life"); /* Set window title */
	XSetWMProtocols(disp, win, &wmdel, 1);
	XSelectInput(disp, win, ExposureMask | ButtonPressMask | KeyPressMask | StructureNotifyMask);
	XMapWindow(disp, win);	       /* Show window */
	XGetWindowAttributes(disp, win, &wa);
	life.w = wa.width/SIZE; /* TODO */
	life.h = wa.height/SIZE;
	life_rand(&life);
	last_frame = clock();
	last_update = last_frame;
	update = 1;
	quit = 0;
	while (quit == 0) {
		if (XPending(disp) == 0) {
			time = clock();
			if (time - last_frame >= fps_clock) {
				last_frame = time;
				draw(disp, win, gc, &life);
			}
			if (time - last_update >= CLOCKS_PER_SEC/8 && update) {
				last_update = time;
				life_next(&life);
			}
			continue;
		}
		XNextEvent(disp, &event);
		switch (event.type) {
		case Expose:
			if (event.xexpose.count) {
				continue;
			}
			draw(disp, win, gc, &life);
			continue;
		case ButtonPress:
			cell = &life.arr[life.i][event.xbutton.y/SIZE][event.xbutton.x/SIZE];
			*cell = !*cell;
			continue;
		case KeyPress:
			switch (XLookupKeysym(&event.xkey, 0)) {
			case ' ':
				update = !update;
				continue;
			case 0xff1b: /* Esc */
			case 'q':
				quit = 1; /* Quit */
				continue;
			}
			continue;
		case ConfigureNotify:
			XGetWindowAttributes(disp, win, &wa);
			life.w = wa.width/SIZE; /* TODO */
			life.h = wa.height/SIZE;
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
