/* $Id$

	PLplot cgm device driver.
*/

/*
 * This driver generates CGM (computer graphics metafiles) files (bit of a
 * tautology that... 'computer graphics metaFILES FILES' - oh well).
 *  
 * The CGM format is vector-based and is widely used as an interchange
 * format between drawing and plotting programs. Although I have never
 * looked at them, there are apparently both Java applets and browser
 * plug-ins for displaying CGM files on web pages.
 * 
 * This plplot driver supports lines, polylines (they make a difference to
 * CGM files), fills, and line widths. It is limited to 256 colours, which
 * should not be a problem. The plplot CGM driver's source (cgm.c) was
 * derived largely from the gd driver (gd.c).
 * 
 * The plplot driver requires libcd.a. libcd.a is very similar to libgd.a
 * and has a similar licencing agreement behind it. Unlike libgd,
 * development of libcd seems to have crawled to a halt back in 1998 with 
 * V1.3 of the library. The original host site for the library no longer
 * exists, so probably the best source of the library presently is:
 * 
 * http://www.pa.msu.edu/reference/cgmdraw_ref.html
 * http://www.pa.msu.edu/ftp/pub/unix/ 
 */

/*
 * Two options are supported by the driver via the -drvopt command line
 * toggle.
 * 
 * By default CGM files don't have a background as such. The driver adds
 * support for different backgrounds by colouring in a large rectangle
 * underneath everything else. If for some reason you want the "raw plotted
 * junk" and aren't really interested in having an obtrusive piece of paper
 * in the back, use the command line toggle "-drvopt no_paper=1" to turn off
 * this background paper.
 * 
 * By default the CGM files generated by this driver try to make edges of
 * polygons (ie fills) "invisible", which is something CGM files can do.
 * Some programs (ie CoreDraw) ignore this field and draw edges in anyway.
 * By setting "-drvopt force_edges=1" the driver will render edges on all
 * filled polygons, and will set their colour to the same as the polygon.
 * Most drivers should not need this, but if you see lines that you don't
 * think you should be seeing in your viewer, try this toggle. 
 */

/*
 * Driver supports a hack that manipulates the colour palette when 
 * a light background is selected. This is basically to make sure 
 * there are not two "whites" when -bg ffffff is issued at the
 * command line.
 *
 * Related to this change, there is an ability to swap the "new"
 * black colour (index 15) with the red colour (index 2) by issuing
 * the command line "-hack" option. I like this for web pages, because
 * I think that black looks nicer than red (on white) for default
 * plotting. That is why it can be enabled with -hack, in case you
 * don't like it working this way. 
 *
 * For me, these two changes make it easy to switch from a "screen friendly"
 * black background with red first plotting colour, to a "web friendly"
 * white background with a black first plotting colour. 
 *
 * These features are enabled on a driver level by defining 
 * "SWAP_BALCK_WHEN_WHITE". If you wan't the driver to behave 100% like other
 * drivers, comment out the define
 */
 
#define SWAP_BALCK_WHEN_WHITE 

#include "plplot/plDevs.h"

#ifdef PLD_cgm

#include "plplot/plplotP.h"
#include "plplot/drivers.h"

#include <cd.h>

/* Prototypes for functions in this file. */

static void	fill_polygon	(PLStream *pls);
static void	setcmap		(PLStream *pls);
static void     plD_init_cgm_Dev(PLStream *pls);

/* top level declarations */

/* In an attempt to fix a problem with the hidden line removal functions
 * that results in hidden lines *not* being removed from "small" plot
 * pages (ie, like a normal video screen), a "virtual" page of much
 * greater size is used to trick the algorithm into working correctly.
 * If, in future, this gets fixed on its own, then don't define
 * "use_experimental_hidden_line_hack"
 */

#define use_experimental_hidden_line_hack

static int force_edges=0;
static int disable_background=0;

static DrvOpt cgm_options[] = {{"force_edges", DRV_INT, &force_edges, "Force edges to be drawn on filled polygongs (0|1)"},
                               {"no_paper", DRV_INT, &disable_background, "Disable background (0|1)"},
			      {NULL, DRV_INT, NULL, NULL}};


/* Struct to hold device-specific info. */

typedef struct {

	cdImagePtr im_out;                      /* Graphics pointer */
        PLINT cgmx;               
        PLINT cgmy;

/* GD does "funny" things with the colour map.
 * It can't guarantee that the colours will be where you think they are.
 * So we need this "colour_index" table to store where the colour we
 * requested happens to be. Messy, but it works.
 */

        int colour_index[256];                  /* Colour "index" table         */

/*
 * I use two colours for both fill and line drawing - a "last colour" and
 * "current colour". The driver only switches colours if they have changed
 * and are used. If no fills are ever done, then the instruction to set the
 * fill colour is never sent to the CGM file. Should make for smaller and
 * more efficient files (I guess).
 */

        int colour;                             /* Current Colour               */
        int last_line_colour;                   /* Last line colour used        */
        int fill_colour;                        /* Current Fill colour          */
        int last_fill_colour;                   /* Last Fill colour used        */
       
        int totcol;                             /* Total number of colours      */
        int ncol1;                              /* Actual size of ncol1 we got  */
	int scale;                              /* scaling factor to "blow up" to */
	                                        /* the "virtual" page in removing hidden lines*/
        int force_edges;                        /* Forces edges to be drawn in fills */
        int disable_background;                 /* Turns off background rectangle */

} cgm_Dev;

void plD_init_cgm		(PLStream *);
void plD_line_cgm		(PLStream *, short, short, short, short);
void plD_polyline_cgm		(PLStream *, short *, short *, PLINT);
void plD_eop_cgm		(PLStream *);
void plD_bop_cgm		(PLStream *);
void plD_tidy_cgm		(PLStream *);
void plD_state_cgm		(PLStream *, PLINT);
void plD_esc_cgm		(PLStream *, PLINT, void *);


void plD_dispatch_init_cgm( PLDispatchTable *pdt )
{
    pdt->pl_MenuStr  = "CGM (Computer Graphics metafile) file";
    pdt->pl_DevName  = "cgm";
    pdt->pl_type     = plDevType_FileOriented;
    pdt->pl_seq      = 40;
    pdt->pl_init     = (plD_init_fp)     plD_init_cgm;
    pdt->pl_line     = (plD_line_fp)     plD_line_cgm;
    pdt->pl_polyline = (plD_polyline_fp) plD_polyline_cgm;
    pdt->pl_eop      = (plD_eop_fp)      plD_eop_cgm;
    pdt->pl_bop      = (plD_bop_fp)      plD_bop_cgm;
    pdt->pl_tidy     = (plD_tidy_fp)     plD_tidy_cgm;
    pdt->pl_state    = (plD_state_fp)    plD_state_cgm;
    pdt->pl_esc      = (plD_esc_fp)      plD_esc_cgm;
}

        
/*--------------------------------------------------------------------------*\
 * plD_init_cgm_Dev()
 *
\*--------------------------------------------------------------------------*/

static void
plD_init_cgm_Dev(PLStream *pls)
{
    cgm_Dev *dev;
    int i;
    
/* Allocate and initialize device-specific data */

    if (pls->dev != NULL)
	free((void *) pls->dev);

    pls->dev = calloc(1, (size_t) sizeof(cgm_Dev));
    if (pls->dev == NULL)
	plexit("plD_init_cgm_Dev: Out of memory.");

    dev = (cgm_Dev *) pls->dev;

/* Check for and set up driver options */

    plParseDrvOpts(cgm_options);    
    dev->force_edges=force_edges;  /* force edges (for corel draw etc...) */
    dev->disable_background=disable_background; /* Disable background */

    dev->colour=1;  /* Set a fall back pen colour in case user doesn't */
    dev->fill_colour=dev->colour; /* initially set fill and line colour the same */
    dev->last_fill_colour=-1;  /* set to -1 = unallocated */
    dev->last_line_colour=-1;  /* set to -1 = unallocated */


}

/*----------------------------------------------------------------------*\
 * plD_init_cgm()
 *
 * Initialize device.
\*----------------------------------------------------------------------*/

void plD_init_cgm(PLStream *pls)
{
    cgm_Dev *dev=NULL;

    pls->termin = 0;            /* Not an interactive device */
    pls->icol0 = 1;
    pls->bytecnt = 0;
    pls->page = 0;
    pls->dev_fill0 = 1;         /* Can do solid fills */

    if (!pls->colorset)
	pls->color = 1;         /* Is a color device */

    if (pls->width<1) pls->width = 1;         /* set a legal line width */

/* Initialize family file info */
    plFamInit(pls);

/* Prompt for a file name if not already set */
    plOpenFile(pls);

/* Allocate and initialize device-specific data */
    plD_init_cgm_Dev(pls);
    dev=(cgm_Dev *)pls->dev;

      if (pls->xlength <= 0 || pls->ylength <=0)
      {
/* use default width, height of 800x600 if not specifed by -geometry option
 * or plspage */
	 plspage(0., 0., 800, 600, 0, 0);
      }

     pls->graphx = GRAPHICS_MODE;

     dev->cgmx = pls->xlength - 1;	/* should I use -1 or not??? */
     dev->cgmy = pls->ylength - 1;

#ifdef use_experimental_hidden_line_hack

     if (dev->cgmx>dev->cgmy)    /* Work out the scaling factor for the  */
        {                        /* "virtual" (oversized) page           */
        dev->scale=PIXELS_X/dev->cgmx;
        }
     else
        {
        dev->scale=PIXELS_Y/dev->cgmy;
        }
#else

     dev->scale=1;

#endif   

     if (pls->xdpi<=0) 
     {
/* This corresponds to a typical monitor resolution of 4 pixels/mm. */
        plspage(4.*25.4, 4.*25.4, 0, 0, 0, 0);
     }
     else
     {
        pls->ydpi=pls->xdpi;        /* Set X and Y dpi's to the same value */
     } 
/* Convert DPI to pixels/mm */
     plP_setpxl(dev->scale*pls->xdpi/25.4,dev->scale*pls->ydpi/25.4);

     plP_setphy(0, dev->scale*dev->cgmx, 0, dev->scale*dev->cgmy);

}

/*----------------------------------------------------------------------*\
 * plD_line_cgm()
 *
 * Draw a line in the current color from (x1,y1) to (x2,y2).
\*----------------------------------------------------------------------*/

void
plD_line_cgm(PLStream *pls, short x1a, short y1a, short x2a, short y2a)
{
    cgm_Dev *dev=(cgm_Dev *)pls->dev;
    int x1 = x1a/dev->scale, y1 = y1a/dev->scale, x2 = x2a/dev->scale, y2 = y2a/dev->scale;
    y1 = y1;
    y2 = y2;

/*
 * Determine if the colour has changed since the last time a line was
 * drawn. If it has, then set the colour NOW otherwise, keep on going like
 * "nuthin happened".
 */

    if (dev->last_line_colour!=dev->colour)
       {
       cdSetLineColor(dev->im_out,dev->colour);
       dev->last_line_colour=dev->colour;
       }

    cdLine(dev->im_out, x1, y1, x2, y2);

}

/*----------------------------------------------------------------------*\
 * plD_polyline_cgm()
 *
 * Draw a polyline in the current color.
\*----------------------------------------------------------------------*/

void
plD_polyline_cgm(PLStream *pls, short *xa, short *ya, PLINT npts)
{
    cgm_Dev *dev=(cgm_Dev *)pls->dev;
    PLINT i;
    cdPoint *points=NULL;

    if ((points=calloc(npts, (size_t)sizeof(cdPoint)))==NULL)
      {   
       plexit("Memory allocation error in \"plD_polyline_cgm\"");
      }

    for (i = 0; i < npts ; i++) 
        {
         points[i].x=xa[i]/dev->scale;
         points[i].y=(ya[i]/dev->scale);
        }

/*
 * Determine if the colour has changed since the last time a line was
 * drawn. If it has, then set the colour NOW otherwise, keep on going like
 * "nuthin happened".
 */

    if (dev->last_line_colour!=dev->colour)
       {
       cdSetLineColor(dev->im_out,dev->colour);
       dev->last_line_colour=dev->colour;
       }

    cdPolyLine(dev->im_out, points, npts);
    free(points);
}


/*----------------------------------------------------------------------*\
 * fill_polygon()
 *
 * Fill polygon described in points pls->dev_x[] and pls->dev_y[].
\*----------------------------------------------------------------------*/

static void
fill_polygon(PLStream *pls)
{
cgm_Dev *dev=(cgm_Dev *)pls->dev;

    PLINT i;
    cdPoint *points=NULL;
    
    if (pls->dev_npts < 1)
	return;

    if ((points=calloc(pls->dev_npts, (size_t)sizeof(cdPoint)))==NULL)
      {   
       plexit("Memory allocation error in \"plD_fill_polygon_cgm\"");
      }

     for (i = 0; i < pls->dev_npts; i++) 
         {
	   points[i].x = pls->dev_x[i]/dev->scale;
	   points[i].y = (pls->dev_y[i]/dev->scale);
         }


/*
 * Determine if the fill colour has changed since the last time a fill was
 * done. If it has, then set the colour NOW otherwise, keep on going like
 * "nuthin happened". If it's the first time, we will know 'cause the the
 * "last_fill_colour" will be -1. 
 */

   if ((dev->fill_colour!=dev->last_fill_colour)||(dev->force_edges==1))
      {
      cdSetFillColor(dev->im_out,dev->fill_colour);

/*
 * Due to a bug in cd V1.3, we have to set the edge colour to the fill
 * colour. This is despite telling the library edges should be invisible.
 * Seems the invisible edges only work with rectangles. 
 */

     if (dev->force_edges==1)
        {
         cdSetEdgeColor(dev->im_out,dev->fill_colour);
         cdSetEdgeVis(dev->im_out,1); 
        }

      dev->last_fill_colour=dev->fill_colour;
      }

   cdPolygon(dev->im_out, points, pls->dev_npts);

   if (dev->force_edges==1)   cdSetEdgeVis(dev->im_out,0);     /* Turn edges off now */

   free(points);

}

/*----------------------------------------------------------------------*\
 * setcmap()
 *
 * Sets up color palette.
\*----------------------------------------------------------------------*/

static void
setcmap(PLStream *pls)
{
    int i, ncol1=pls->ncol1;
    int ncol0=pls->ncol0, total_colours;
    PLColor cmap1col;
    cgm_Dev *dev=(cgm_Dev *)pls->dev;
    PLFLT tmp_colour_pos;

    cdImageColorClear(dev->im_out);  /* UNDOCUMENTED FUNCTION TO RESET THE
                                      * INTERNAL COLOUR TABLE OF THE 
                                      * CD DRIVER. Seems to work and fix
                                      * the errors                         */

    if (ncol0>cdMaxColors/2)     /* Check for ridiculous number of colours */
       {                         /* in ncol0, and appropriately adjust the */ 
        plwarn("Too many colours in cmap0.");        /* number, issuing a  */ 
        ncol0=cdMaxColors/2;                         /* warning if it does */
        pls->ncol0=ncol0;
       }

    dev->totcol=0;       /* Reset the number of colours counter to zero */
    
    total_colours=ncol0+ncol1;  /* Work out how many colours are wanted */

    if (total_colours>cdMaxColors)     /* Do some rather modest error      */
       {                               /* checking to make sure that       */
        total_colours=cdMaxColors;     /* we are not defining more colours */
        ncol1=total_colours-ncol0;     /* than we have room for.           */

        if (ncol1<=0)
           {
            plexit("Problem setting colourmap in CGM driver.");
           }
       }
 
 dev->ncol1=ncol1;  /* The actual size of ncol1, regardless of what was asked. 
                     * This is dependent on colour slots available.
                     * It might well be the same as ncol1.
                     */
 
/* Initialize cmap 0 colors */

if (ncol0>0)  /* make sure the program actually asked for cmap0 first */
   {

#ifdef SWAP_BALCK_WHEN_WHITE

/* 
 * Do a kludge to add a "black" colour back to the palette if the
 * background is "almost white" (ie changed through -bg).
 * 
 * Also includes an "optional" change to swap the red colour (1) with the 
 * black colour (15), which is off by default. (I don't like the red being
 * the 'default' colour "1" on a "white" background, or for that matter
 * yellow being "2", but I can live more with yellow at number two.)
 * Just use "-hack" from the command line to make it take effect.
 *
 */

if ((pls->cmap0[0].r>227)&&(pls->cmap0[0].g>227)&&(pls->cmap0[0].b>227))
   {
    if (pls->hack!=1)   /* just set colour 15 to black */
       {
        pls->cmap0[15].r=0;
        pls->cmap0[15].g=0;
        pls->cmap0[15].b=0;
       }
    else   /* swap colour 15 and colour 1 */
       {
       pls->cmap0[15].r=pls->cmap0[1].r;
       pls->cmap0[15].g=pls->cmap0[1].g;
       pls->cmap0[15].b=pls->cmap0[1].b;

       pls->cmap0[1].r=0;
       pls->cmap0[1].g=0;
       pls->cmap0[1].b=0;
      }
   }

#endif

    for (i = 0; i < ncol0; i++)
        {
         if ((
              dev->colour_index[i]=cdImageColorAllocate(dev->im_out,
                                   pls->cmap0[i].r, pls->cmap0[i].g, pls->cmap0[i].b)
              )==-1)
            {
            plwarn("Problem setting cmap0 in CGM driver.");
            }
        else
        ++dev->totcol; /* count the number of colours we use as we use them */
        }


  }

/* Initialize any remaining slots for cmap1 */


if (ncol1>0)    /* make sure that we want to define cmap1 first */
   {
    for (i = 0; i < ncol1; i++) 
        {
         if (ncol1<pls->ncol1)       /* Check the dynamic range of colours */
            {

             /*
              * Ok, now if we have less colour slots available than are being
              * defined by pls->ncol1, then we still want to use the full
              * dynamic range of cmap1 as best we can, so what we do is work
              * out an approximation to the index in the full dynamic range
              * in cases when pls->ncol1 exceeds the number of free colours.
              */

             tmp_colour_pos= i>0 ? pls->ncol1*((PLFLT)i/ncol1) : 0;
             plcol_interp(pls, &cmap1col, (int) tmp_colour_pos, pls->ncol1);
             
            }
         else
            {
             plcol_interp(pls, &cmap1col, i, ncol1);
            }


         if ((
              dev->colour_index[i + pls->ncol0]=cdImageColorAllocate(dev->im_out,
                                   cmap1col.r, cmap1col.g, cmap1col.b)
              )==-1)
            {
            plwarn("Problem setting cmap1 in CGM driver.");
            }
        else
        ++dev->totcol; /* count the number of colours we use as we use them */

        }
   }
}


/*----------------------------------------------------------------------*\
 * plD_state_cgm()
 *
 * Handle change in PLStream state (color, pen width, fill attribute, etc).
\*----------------------------------------------------------------------*/

void 
plD_state_cgm(PLStream *pls, PLINT op)
{
cgm_Dev *dev=(cgm_Dev *)pls->dev;
PLFLT tmp_colour_pos;

    switch (op) {

    case PLSTATE_WIDTH:
        cdSetLineWidth(dev->im_out, pls->width);
	break;

    case PLSTATE_COLOR0:
	dev->colour = pls->icol0;
	if (dev->colour == PL_RGB_COLOR) {
	    int r = pls->curcolor.r;
	    int g = pls->curcolor.g;
	    int b = pls->curcolor.b;
	    if (dev->totcol < cdMaxColors) 
	       {
                if ((
                     dev->colour_index[++dev->totcol]=cdImageColorAllocate(dev->im_out,r, g, b)
                    )==-1)
                   plwarn("Problem changing colour in \"PLSTATE_COLOR0\"");
                else
                   dev->colour = dev->totcol;
	       }

	}
        dev->fill_colour=dev->colour;
	break;

    case PLSTATE_COLOR1:
        /*
         * Start by checking to see if we have to compensate for cases where
         * we don't have the full dynamic range of cmap1 at our disposal
         */
        if (dev->ncol1<pls->ncol1)   
           {
           tmp_colour_pos=dev->ncol1*((PLFLT)pls->icol1/(pls->ncol1>0 ? pls->ncol1 : 1));
           dev->colour = pls->ncol0 + (int)tmp_colour_pos;
           }
        else
           dev->colour = pls->ncol0 + pls->icol1;

           dev->fill_colour=dev->colour;
	break;
	
	
    case PLSTATE_CMAP0:
    case PLSTATE_CMAP1:
    /*
     *  Code to redefine the entire palette
     */
	if (pls->color)
	    setcmap(pls);
	break;
    }
}


/*----------------------------------------------------------------------*\
 * plD_esc_cgm()
 *
 * Escape function.
\*----------------------------------------------------------------------*/

void plD_esc_cgm(PLStream *pls, PLINT op, void *ptr)
{
    switch (op) {

      case PLESC_FILL:  /* fill */
	fill_polygon(pls);
	break;
    }
}

/*----------------------------------------------------------------------*\
 * plD_bop_cgm()
 *
 * Set up for the next page.
 * Advance to next family file if necessary (file output).
\*----------------------------------------------------------------------*/

void plD_bop_cgm(PLStream *pls)
{
    cgm_Dev *dev;

    plGetFam(pls);
/* force new file if pls->family set for all subsequent calls to plGetFam 
 * n.b. putting this after plGetFam call is important since plinit calls
 * bop, and you don't want the familying sequence started until after
 * that first call to bop.*/

    pls->famadv = 1;

    pls->page++;

/* n.b. pls->dev can change because of an indirect call to plD_init_cgm 
 * from plGetFam if familying is enabled.  Thus, wait to define dev until
 * now. */

    dev = (cgm_Dev *) pls->dev;

    if (pls->page==1) {
        dev->im_out = cdImageCreate(pls->xlength, pls->ylength);
    }
    else if (pls->family != 1) {
        cdCgmNewPic(dev->im_out,0);
    }

    setcmap(pls);

/* Continue to initialise the driver */

cdSetFillStyle(dev->im_out,1);   /* Set solid fills */

/*
 * Due to an apparent bug in libcd, it is necessary to turn ON edges
 * manually, then turn them off again to make edges turn off. By
 * default the driver thinks they are off, so when we tell the driver
 * to turn them off it says "they are already off, I wont do anything"
 * but in reality they are on by default. So what we do is turn them ON
 * manually, then turn them OFF later.
 * 
 * Due to a border being drawn around the edge of the image, we also
 * want the edges turned on so we can lay down a rectangle coloured in
 * the background colour at the start. Once we have drawn our
 * background box, we then turn edges off for the rest of the page.
 */

cdSetEdgeVis(dev->im_out,1);     /* turn edges on so we can turn them off! */

if (dev->disable_background!=1)
   {
    cdSetEdgeWidth(dev->im_out,pls->xlength/5); /* set edge to *really* wide so we can cover the edge of the page completelt */
    cdSetEdgeColor(dev->im_out,0);   /* set the edge colour to the background colour so we can make a coloured page */
    cdSetFillColor(dev->im_out,0);   /* set fill colour to background colour so we have a coloured page */
    cdRectangle(dev->im_out, 0, 0, pls->xlength-1, pls->ylength-1);   /* Draw a coloured rectangle to act as our "paper" */
   }

cdSetEdgeVis(dev->im_out,0);     /* Turn edges off now */
cdSetEdgeWidth(dev->im_out,0);   /* Just to be 100% sure */

cdSetLineType(dev->im_out,1);           /* set solid lines */
cdSetLineWidth(dev->im_out,pls->width); /* set initial line width for each page */

}

/*----------------------------------------------------------------------*\
 * plD_tidy_cgm()
 *
 * Close graphics file or otherwise clean up.
\*----------------------------------------------------------------------*/

void plD_tidy_cgm(PLStream *pls)
{
   cgm_Dev *dev=(cgm_Dev *)pls->dev;

    if (pls->family != 1) 
       {
        cdImageCgm(dev->im_out, pls->OutFile);
       }

   cdImageDestroy(dev->im_out);
   fclose(pls->OutFile);
   free_mem(pls->dev);
}

/*----------------------------------------------------------------------*\
 * plD_eop_cgm()
 *
 * End of page.
\*----------------------------------------------------------------------*/

void plD_eop_cgm(PLStream *pls)
{
cgm_Dev *dev=(cgm_Dev *)pls->dev;
int i;

    if (pls->family==1) 
       {
        cdImageCgm(dev->im_out, pls->OutFile);
       }
   for (i=0;i<cdMaxColors;++i) dev->colour_index[i]=-1;

}


/*#endif*/


#else
int 
pldummy_cgm()
{
    return 0;
}

#endif				/* cgm */
