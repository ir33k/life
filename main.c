/**
 * Conway's Game of Life
 * https://en.wikipedia.org/wiki/Conway's_Game_of_Life
 *
 *	$ ./build
 *	$ ./life
 */
#include <stdio.h>
#include <X11/Xlib.h>
#include "life.h"
#include "type.h"

int
main(void)
{
	struct life life = {0};	/* Game of life instance */
	u8	 run;		/* Value of 0 will close window */
	Display *disp;		/* X display */
	Window   root;		/* Parent window */
	Window   win;		/* Window */
	Atom     wmdel;		/* WM delete window atom */
	XEvent	 event;		/* For capturing window events */
	KeySym	 key;		/* Pressed keyboard key */
	GC       gc;		/* Graphical context */
	u64      fg, bg;	/* Foreground and background color */

	/* Game of life */
	life.w = 800;
	life.h = 600;
	/* Glider */
	life.arr[life.i][1][2] = 1;
	life.arr[life.i][2][3] = 1;
	life.arr[life.i][2][4] = 1;
	life.arr[life.i][3][2] = 1;
	life.arr[life.i][3][3] = 1;

	/* init */
	if ((disp = XOpenDisplay(0)) == 0) {
		fprintf(stderr, "ERR: Could not open defult disply");
		return 1;
	}
	root = DefaultRootWindow(disp);
	fg = 0x000000;
	bg = 0xffffff;
	win = XCreateSimpleWindow(disp, root, 0, 0, 800, 600, 2, fg, bg);
	wmdel = XInternAtom(disp, "WM_DELETE_WINDOW", 0);
	gc = XCreateGC(disp, win, 0, 0);
	XStoreName(disp, win, "life"); /* Set window title */
	XSetWMProtocols(disp, win, &wmdel, 1);
	XSelectInput(disp, win, ExposureMask | ButtonPressMask | KeyPressMask);
	XMapWindow(disp, win);	       /* Show window */
	run = 1;
	while (run) {
		if (XPending(disp) == 0) {
			continue;
		}
		XNextEvent(disp, &event);
		switch (event.type) {
		case Expose:
			if (event.xexpose.count) {
				continue;
			}
			/* No events, we can draw now. */
			XClearWindow(disp, win);
			life_next(&life);
			continue;
		case ButtonPress:
			XClearWindow(disp, win);
 			XSetForeground(disp, gc, fg);
			XFillRectangle(disp, win, gc,
				       event.xbutton.x - event.xbutton.x%8 - 4,
				       event.xbutton.y - event.xbutton.y%8 - 4,
				       8, 8);
			continue;
		case KeyPress:
			key = XLookupKeysym(&event.xkey, 0);

			/* Print pressed key as number and char. */
			printf("xkey:\t%ld\t%c\n", key, (char)key);

			switch (key) {
			case 65307:		/* Esc */
			case 'q': case 'Q':
			case 'c': case 'C':
			case 'x': case 'X':
				run = 0;        /* Quit */
				continue;
			}
			continue;
		case ClientMessage:
			if ((Atom)event.xclient.data.l[0] == wmdel) {
				run = 0;
			}
			continue;
		}
	}
	/* close */
	XFreeGC(disp, gc);
	XCloseDisplay(disp);
	XDestroyWindow(disp, win);
	return 0;
}
