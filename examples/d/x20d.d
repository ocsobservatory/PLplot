/* $Id$
 *
 *      plimage demo
 *
 */

import std.math;
import std.stdio;
import std.stream;

import plplot;

int    dbg           = 0;
int    nosombrero    = 0;
int    nointeractive = 0;
string f_name;

struct stretch_data
{
    PLFLT xmin, xmax, ymin, ymax;
    PLFLT stretch;
};


extern ( C ) {
/* Transformation function */
void mypltr( PLFLT x, PLFLT y, PLFLT *tx, PLFLT *ty, PLPointer pltr_data )
{
    stretch_data* s = cast(stretch_data *) pltr_data;
    PLFLT       x0  = ( s.xmin + s.xmax ) * 0.5;
    PLFLT       y0  = ( s.ymin + s.ymax ) * 0.5;
    PLFLT       dy  = ( s.ymax - s.ymin ) * 0.5;
    *tx = x0 + ( x0 - x ) * ( 1.0 - s.stretch * cos( ( y - y0 ) / dy * PI * 0.5 ) );
    *ty = y;
}
}

int main( char[][] args )
{
    const int XDIM = 260;
    const int YDIM = 220;

    /*
     * Bugs in plimage():
     * -at high magnifications, the left and right edge are ragged, try
     *    ./x20c -dev xwin -wplt 0.3,0.3,0.6,0.6 -ori 0.5
     *
     * Bugs in x20c.c:
     * -if the window is resized after a selection is made on "lena", when
     *  making a new selection the old one will re-appear.
     */

    /* Parse and process command line arguments */
    // plMergeOpts(options, "x20c options", NULL);
    plparseopts( args, PL_PARSE_FULL );

    /* Initialize plplot */
    plinit();

    PLFLT[][] z = new PLFLT[][XDIM];
    for ( int i = 0; i < XDIM; i++ )
        z[i] = new PLFLT[YDIM];

    /* view image border pixels */
    if ( dbg )
    {
        plenv( 1.0, XDIM, 1.0, YDIM, 1, 1 ); /* no plot box */

        /* build a one pixel square border, for diagnostics */
        for ( int i = 0; i < XDIM; i++ )
            z[i][YDIM - 1] = 1.0; /* right */
        for ( int i = 0; i < XDIM; i++ )
            z[i][0] = 1.0;        /* left */

        for ( int i = 0; i < YDIM; i++ )
            z[0][i] = 1.0;        /* top */
        for ( int i = 0; i < YDIM; i++ )
            z[XDIM - 1][i] = 1.0; /* botton */

        pllab( "...around a blue square.", " ", "A red border should appear..." );

        plimage( z, 1.0, XDIM, 1.0, YDIM, 0., 0., 1.0, XDIM, 1.0, YDIM );
    }

    PLFLT[] x = new PLFLT[XDIM];
    PLFLT[] y = new PLFLT[YDIM];

    /* sombrero-like demo */
    if ( !nosombrero )
    {
        PLFLT[][] r = new PLFLT[][XDIM];
        for ( int i = 0; i < XDIM; i++ )
            r[i] = new PLFLT[YDIM];

        plcol0( 2 ); /* draw a yellow plot box, useful for diagnostics! :( */
        plenv( 0.0, 2.0 * PI, 0, 3.0 * PI, 1, -1 );

        for ( int i = 0; i < XDIM; i++ )
            x[i] = i * 2.0 * PI / ( XDIM - 1 );
        for ( int i = 0; i < YDIM; i++ )
            y[i] = i * 3.0 * PI / ( YDIM - 1 );

        for ( int i = 0; i < XDIM; i++ )
            for ( int j = 0; j < YDIM; j++ )
            {
                r[i][j] = sqrt( x[i] * x[i] + y[j] * y[j] ) + 1e-3;
                z[i][j] = sin( r[i][j] ) / ( r[i][j] );
            }

        pllab( "No, an amplitude clipped \"sombrero\"", "", "Saturn?" );
        plptex( 2.0, 2.0, 3.0, 4.0, 0.0, "Transparent image" );
        plimage( z, 0.0, 2.0 * PI, 0.0, 3.0 * PI, 0.05, 1.0,
            0.0, 2.0 * PI, 0.0, 3.0 * PI );

        /* save the plot */
        if ( f_name.length != 0 )
            save_plot( f_name );
    }

    /* read Lena image */
    /* Note we try two different locations to cover the case where this
     * examples is being run from the test_c.sh script */
    int       width, height, num_col;
    PLFLT[][] img_f;
    if ( read_img( "lena.pgm", img_f, width, height, num_col ) )
    {
        if ( read_img( "../lena.pgm", img_f, width, height, num_col ) )
        {
            fwritefln( stderr, "No such file" );
            plend();
            return 1;
        }
    }

    /* set gray colormap */
    gray_cmap( num_col );

    /* display Lena */
    plenv( 1.0, width, 1.0, height, 1, -1 );

    if ( !nointeractive )
        pllab( "Set and drag Button 1 to (re)set selection, Button 2 to finish.", " ", "Lena..." );
    else
        pllab( "", " ", "Lena..." );

    plimage( img_f, 1.0, width, 1.0, height, 0.0, 0.0,
        1.0, width, 1.0, height );

    /* selection/expansion demo */
    if ( !nointeractive )
    {
        PLFLT xi = 200.0;
        PLFLT xe = 330.0;
        PLFLT yi = 280.0;
        PLFLT ye = 220.0;

        if ( get_clip( xi, xe, yi, ye ) ) /* get selection rectangle */
        {
            plend();
            return 0;
        }

        plspause( 0 );
        pladv( 0 );

        /* display selection only */
        plimage( img_f, 1.0, width, 1.0, height, 0.0, 0.0, xi, xe, ye, yi );

        plspause( 1 );

        /* zoom in selection */
        plenv( xi, xe, ye, yi, 1, -1 );
        plimage( img_f, 1.0, width, 1.0, height, 0.0, 0.0, xi, xe, ye, yi );
    }

    /* Base the dynamic range on the image contents. */
    PLFLT img_min, img_max;
    plMinMax2dGrid( img_f, img_max, img_min );

    /* Draw a saturated version of the original image.  Only use the middle 50%
     * of the image's full dynamic range. */
    plcol0( 2 );
    plenv( 0, width, 0, height, 1, -1 );
    pllab( "", "", "Reduced dynamic range image example" );
    plimagefr( img_f, 0.0, width, 0.0, height, 0.0, 0.0,
        img_min + img_max * 0.25, img_max - img_max * 0.25 );

    /* Draw a distorted version of the original image, showing its full dynamic range. */
    plenv( 0, width, 0, height, 1, -1 );
    pllab( "", "", "Distorted image example" );

    stretch_data stretch = { 0, width, 0, height, 0.5 };

    /* In C / C++ the following would work, with plimagefr directly calling
     * mypltr. For compatibilty with other language bindings the same effect
     * can be achieved by generating the transformed grid first and then
     * using pltr2. */
    plimagefr( img_f, 0.0, width, 0.0, height, 0.0, 0.0, img_min, img_max, &mypltr, cast(PLPointer) &stretch );

    plend();

    return 0;
}


/* read image from file in binary ppm format */
int read_img( string fname, out PLFLT[][] img_f, out int width, out int height, out int num_col )
{
    ubyte[]      img;

    BufferedFile input = new BufferedFile;
    try {
        input.open( fname, FileMode.In );

        string ver;
        input.readf( "%s", &ver );

        if ( ver != "P5" )      /* I only understand this! */
            return 1;

        char   dummy;
        char[] result;
        input.read( dummy );
        while ( dummy == '#' )
        {
            result = input.readLine();
            if ( result.length == 0 )
                result = input.readLine();               // workaround: for some reason the first call returns empty string
            input.read( dummy );
        }
        input.seek( -1, SeekPos.Current );

        if ( input.readf( "%d %d %d", &width, &height, &num_col ) != 9 )    /* width, height num colors */
            return 1;

        img = new ubyte[width * height];

        img_f = new PLFLT[][width];
        for ( int i = 0; i < width; i++ )
            img_f[i] = new PLFLT[height];

        if ( input.read( img ) != width * height )
            return 1;
    } catch ( Exception except ) {
        return 1;
    } finally {
        input.close();
    }

    for ( int i = 0; i < width; i++ )
        for ( int j = 0; j < height; j++ )
            img_f[i][j] = img[( height - 1 - j ) * width + i];
    /* flip image up-down */

    return 0;
}


/* save plot */
void save_plot( string fname )
{
    PLINT cur_strm, new_strm;

    plgstrm( &cur_strm );    /* get current stream */
    plmkstrm( &new_strm );   /* create a new one */

    plsdev( "psc" );         /* new device type. Use a known existing driver */
    plsfnam( fname );        /* file name */

    plcpstrm( cur_strm, 0 ); /* copy old stream parameters to new stream */
    plreplot();              /* do the save */
    plend1();                /* close new device */

    plsstrm( cur_strm );     /* and return to previous one */
}


/*  get selection square interactively */
int get_clip( ref PLFLT xi, ref PLFLT xe, ref PLFLT yi, ref PLFLT ye )
{
    return 0;
}


/* set gray colormap */
void gray_cmap( PLINT num_col )
{
    PLFLT[] r   = [ 0.0, 1.0 ];
    PLFLT[] g   = [ 0.0, 1.0 ];
    PLFLT[] b   = [ 0.0, 1.0 ];
    PLFLT[] pos = [ 0.0, 1.0 ];

    plscmap1n( num_col );
    plscmap1l( 1, pos, r, g, b );
}
