/* $Id$
 * $Log$
 * Revision 1.10  1995/06/01 21:40:07  mjl
 * All C demo files: changed file inclusion to use quotes instead of angle
 * brackets so that dependencies are retained during development.
 *
 * Revision 1.9  1995/04/12  08:18:55  mjl
 * Changes to all C demos: now include "plcdemos.h" to get all startup
 * definitions and includes that are useful to share between them.
 *
 * Revision 1.8  1995/03/16  23:18:51  mjl
 * All example C programs: changed plParseInternalOpts() call to plParseOpts().
 *
 * Revision 1.7  1994/06/30  17:57:29  mjl
 * All C example programs: made another pass to eliminate warnings when using
 * gcc -Wall.  Lots of cleaning up: got rid of includes of math.h or string.h
 * (now included by plplot.h), eliminated redundant casts, put in more
 * uniform comments, and other minor changes.
*/

/*	x06c.c

	Font demo.
*/

#include "plcdemos.h"

/*--------------------------------------------------------------------------*\
 * main
 *
 * Displays the entire "plpoin" symbol (font) set.
\*--------------------------------------------------------------------------*/

int
main(int argc, char *argv[])
{
    char text[3];
    int i, j, k;
    PLFLT x, y;

/* Parse and process command line arguments */

    (void) plParseOpts(&argc, argv, PL_PARSE_FULL);

/* Initialize plplot */

    plinit();

    pladv(0);

/* Set up viewport and window */

    plcol(2);
    plvpor(0.1, 1.0, 0.1, 0.9);
    plwind(0.0, 1.0, 0.0, 1.3);

/* Draw the grid using plbox */

    plbox("bcgt", 0.1, 0, "bcgt", 0.1, 0);

/* Write the digits below the frame */

    plcol(15);
    for (i = 0; i <= 9; i++) {
	sprintf(text, "%d", i);
	plmtex("b", 1.5, (0.1 * i + 0.05), 0.5, text);
    }

    k = 0;
    for (i = 0; i <= 12; i++) {

    /* Write the digits to the left of the frame */

	sprintf(text, "%d", 10 * i);
	plmtex("lv", 1.0, (1.0 - (2 * i + 1) / 26.0), 1.0, text);
	for (j = 0; j <= 9; j++) {
	    x = 0.1 * j + 0.05;
	    y = 1.25 - 0.1 * i;

	/* Display the symbols (plpoin expects that x and y are arrays so */
	/* pass pointers) */

	    if (k < 128)
		plpoin(1, &x, &y, k);
	    k = k + 1;
	}
    }

    plmtex("t", 1.5, 0.5, 0.5, "PLplot Example 6 - plpoin symbols");
    plend();
    exit(0);
}
