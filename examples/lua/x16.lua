-- $Id$

--	plshade demo, using color fill.

-- initialise Lua bindings to PLplot
if string.sub(_VERSION,1,7)=='Lua 5.0' then
	lib=loadlib('./plplotluac.so','luaopen_plplotluac') or loadlib('plplotluac.dll','luaopen_plplotluac')
	assert(lib)()
else
	require('plplotluac')
end


-- Fundamental settings.  See notes[] for more info. 
ns = 20		-- Default number of shade levels 
nx = 35		-- Default number of data points in x 
ny = 46		-- Default number of data points in y 
exclude = 0   -- By default do not plot a page illustrating
              -- exclusion.  API is probably going to change
              -- anyway, and cannot be reproduced by any
				      -- front end other than the C one. 

-- polar plot data 
PERIMETERPTS = 100

-- Transformation function 
tr = {}

function mypltr(x, y, pltr_data)
	tx = tr[1] * x + tr[2] * y + tr[3]
	ty = tr[4] * x + tr[5] * y + tr[6]
	
	return tx, ty
end

----------------------------------------------------------------------------
-- f2mnmx
--
-- Returns min & max of input 2d array.
----------------------------------------------------------------------------
function f2mnmx(f, nx, ny)
  fmax = f[1][1] 
  fmin = fmax

  for i = 1, nx do
    for j = 1,ny do
      fmax = math.max(fmax, f[i][j])
      fmin = math.min(fmin, f[i][j])
    end
  end
  
  return fmin, fmax
end


function zdefined(x, y)
  z = math.sqrt(x^2 + y^2)

  return z<0.4 or z>0.6
end


----------------------------------------------------------------------------
-- main
--
-- Does several shade plots using different coordinate mappings.
----------------------------------------------------------------------------

px = {}
py = {}

fill_width = 2
cont_color = 0
cont_width = 0

-- Parse and process command line arguments 
pl.parseopts(arg, pl.PL_PARSE_FULL)

-- Reduce colors in cmap 0 so that cmap 1 is useful on a 16-color display 
pl.scmap0n(3)

-- Initialize plplot 
pl.init()

-- Set up transformation function 
tr[1] = 2/(nx-1)
tr[2] = 0
tr[3] = -1
tr[4] = 0
tr[5] = 2/(ny-1)
tr[6] = -1

-- Allocate data structures 
clevel = {}
shedge = {}
z = {}
w = {}

-- Set up data array 
for i = 1, nx do
	x = (i-1 - math.floor(nx/2)) / math.floor(nx/2)
  z[i] = {}
  w[i] = {}
	for j = 1, ny do
    y = (j-1 - math.floor(ny/2)) / math.floor(ny/2) - 1

    z[i][j] = - math.sin(7*x) * math.cos(7*y) + x^2 - y^2
    w[i][j] = - math.cos(7*x) * math.sin(7*y) + 2*x*y
	end
end

zmin, zmax = f2mnmx(z, nx, ny)
for i = 1, ns do
	clevel[i] = zmin + (zmax-zmin)*(i-0.5)/ns
end

for i = 1, ns+1 do
	shedge[i] = zmin + (zmax-zmin)*(i-1)/ns
end

-- Set up coordinate grids 
cgrid1["xg"] = {}
cgrid1["yg"] = {}
cgrid1["nx"] = nx
cgrid1["ny"] = ny

cgrid2["xg"] = {}
cgrid2["yg"] = {}
cgrid2["nx"] = nx
cgrid2["ny"] = ny

for i = 1, nx do
  cgrid2["xg"][i] = {}
  cgrid2["yg"][i] = {}
	for j = 1, ny do
    x, y = mypltr(i, j, nil)

    argx = x*math.pi/2
    argy = y*math.pi/2
    distort = 0.4

    cgrid1["xg"][i] = x + distort * math.cos(argx)
    cgrid1["yg"][j] = y - distort * math.cos(argy)

    cgrid2["xg"][i][j] = x + distort * math.cos(argx) * math.cos(argy)
    cgrid2["yg"][i][j] = y - distort * math.cos(argx) * math.cos(argy)
  end
end

-- Plot using identity transform 
pl.adv(0)
pl.vpor(0.1, 0.9, 0.1, 0.9)
pl.wind(-1, 1, -1, 1)

pl.psty(0)

pl.shades(z, {}, -1, 1, -1, 1, 
   shedge, fill_width,
   cont_color, cont_width,
   plfill, 1, {}, {})

pl.col0(1)
pl.box("bcnst", 0, 0, "bcnstv", 0, 0)
pl.col0(2)

--plcont(w, 1, nx, 1, ny, clevel, mypltr, {})
pl.lab("distance", "altitude", "Bogon density")

-- Plot using 1d coordinate transform 
    
    pladv(0)
    plvpor(0.1, 0.9, 0.1, 0.9)
    plwind(-1.0, 1.0, -1.0, 1.0)

    plpsty(0)

    plshades(z, nx, ny, NULL, -1., 1., -1., 1., 
	     shedge, ns+1, fill_width,
	     cont_color, cont_width,
	     plfill, 1, pltr1, cgrid1)

    plcol0(1)
    plbox("bcnst", 0.0, 0, "bcnstv", 0.0, 0)
    plcol0(2)
--
    plcont(w, nx, ny, 1, nx, 1, ny, clevel, ns, pltr1, cgrid1)
    
    pllab("distance", "altitude", "Bogon density")

-- Plot using 2d coordinate transform 

    pladv(0)
    plvpor(0.1, 0.9, 0.1, 0.9)
    plwind(-1.0, 1.0, -1.0, 1.0)

    plpsty(0)

    plshades(z, nx, ny, NULL, -1., 1., -1., 1., 
	     shedge, ns+1, fill_width,
	     cont_color, cont_width,
	     plfill, 0, pltr2, cgrid2)

    plcol0(1)
    plbox("bcnst", 0.0, 0, "bcnstv", 0.0, 0)
    plcol0(2)
    plcont(w, nx, ny, 1, nx, 1, ny, clevel, ns, pltr2, cgrid2)

    pllab("distance", "altitude", "Bogon density, with streamlines")

-- Plot using 2d coordinate transform 

    pladv(0)
    plvpor(0.1, 0.9, 0.1, 0.9)
    plwind(-1.0, 1.0, -1.0, 1.0)

    plpsty(0)

    plshades(z, nx, ny, NULL, -1., 1., -1., 1., 
	     shedge, ns+1, fill_width,
	     2, 3,
	     plfill, 0, pltr2, cgrid2)

    plcol0(1)
    plbox("bcnst", 0.0, 0, "bcnstv", 0.0, 0)
    plcol0(2)
--    plcont(w, nx, ny, 1, nx, 1, ny, clevel, ns, pltr2, (void *) &cgrid2)

    pllab("distance", "altitude", "Bogon density")

-- Note this exclusion API will probably change. 

-- Plot using 2d coordinate transform and exclusion

    if exclude~=0 then
    pladv(0)
    plvpor(0.1, 0.9, 0.1, 0.9)
    plwind(-1.0, 1.0, -1.0, 1.0)

    plpsty(0)

    plshades(z, nx, ny, zdefined, -1., 1., -1., 1., 
	     shedge, ns+1, fill_width,
	     cont_color, cont_width,
	     plfill, 0, pltr2, cgrid2)

    plcol0(1)
    plbox("bcnst", 0.0, 0, "bcnstv", 0.0, 0)

    pllab("distance", "altitude", "Bogon density with exclusion")

    end
-- Example with polar coordinates. 

    pladv(0)
    plvpor( .1, .9, .1, .9 )
    plwind( -1., 1., -1., 1. )

    plpsty(0)

    -- Build new coordinate matrices. 
    
    for (i = 0 i < nx i++) {
        r = ((PLFLT) i)/ (nx-1)
	for (j = 0 j < ny j++) {
	   t = (2.*M_PI/(ny-1.))*j
	   cgrid2.xg[i][j] = r*cos(t)
	   cgrid2.yg[i][j] = r*sin(t)
	   z[i][j] = exp(-r*r)*cos(5.*M_PI*r)*cos(5.*t)
	}
    }

    -- Need a new shedge to go along with the new data set. 

    f2mnmx(z, nx, ny, &zmin, &zmax)

    for (i = 0 i < ns+1 i++)
	shedge[i] = zmin + (zmax - zmin) * (PLFLT) i / (PLFLT) ns

    --  Now we can shade the interior region. 
    plshades(z, nx, ny, NULL, -1., 1., -1., 1., 
	     shedge, ns+1, fill_width,
	     cont_color, cont_width,
	     plfill, 0, pltr2, (void *) &cgrid2)

-- Now we can draw the perimeter.  (If do before, shade stuff may overlap.) 
      for (i = 0 i < PERIMETERPTS i++) {
	       t = (2.*M_PI/(PERIMETERPTS-1))*(double)i
	       px[i] = cos(t)
	       py[i] = sin(t)
      }
      plcol0(1)
      plline(PERIMETERPTS, px, py)
                  
      -- And label the plot.

      plcol0(2)
      pllab( "", "",  "Tokamak Bogon Instability" )


-- Clean up 

    free((void *) clevel)
    free((void *) shedge)
    free((void *) xg1)
    free((void *) yg1)
    plFree2dGrid(z, nx, ny)
    plFree2dGrid(w, nx, ny)
    plFree2dGrid(cgrid2.xg, nx, ny)
    plFree2dGrid(cgrid2.yg, nx, ny)

    plend()

    exit(0)
}

