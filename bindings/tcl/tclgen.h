static int pladvCmd( ClientData, Tcl_Interp *, int, char **);
static int plaxesCmd( ClientData, Tcl_Interp *, int, char **);
static int plbinCmd( ClientData, Tcl_Interp *, int, char **);
static int plbopCmd( ClientData, Tcl_Interp *, int, char **);
static int plboxCmd( ClientData, Tcl_Interp *, int, char **);
static int plbox3Cmd( ClientData, Tcl_Interp *, int, char **);
static int plcol0Cmd( ClientData, Tcl_Interp *, int, char **);
static int plcol1Cmd( ClientData, Tcl_Interp *, int, char **);
static int pldid2pcCmd( ClientData, Tcl_Interp *, int, char **);
static int pldip2dcCmd( ClientData, Tcl_Interp *, int, char **);
static int plendCmd( ClientData, Tcl_Interp *, int, char **);
static int plend1Cmd( ClientData, Tcl_Interp *, int, char **);
static int plenvCmd( ClientData, Tcl_Interp *, int, char **);
static int pleopCmd( ClientData, Tcl_Interp *, int, char **);
static int plerrxCmd( ClientData, Tcl_Interp *, int, char **);
static int plerryCmd( ClientData, Tcl_Interp *, int, char **);
static int plfamadvCmd( ClientData, Tcl_Interp *, int, char **);
static int plfillCmd( ClientData, Tcl_Interp *, int, char **);
static int plflushCmd( ClientData, Tcl_Interp *, int, char **);
static int plfontCmd( ClientData, Tcl_Interp *, int, char **);
static int plfontldCmd( ClientData, Tcl_Interp *, int, char **);
static int plgchrCmd( ClientData, Tcl_Interp *, int, char **);
static int plgcol0Cmd( ClientData, Tcl_Interp *, int, char **);
static int plgdevCmd( ClientData, Tcl_Interp *, int, char **);
static int plgdidevCmd( ClientData, Tcl_Interp *, int, char **);
static int plgdioriCmd( ClientData, Tcl_Interp *, int, char **);
static int plgdipltCmd( ClientData, Tcl_Interp *, int, char **);
static int plgescCmd( ClientData, Tcl_Interp *, int, char **);
static int plgfamCmd( ClientData, Tcl_Interp *, int, char **);
static int plgfnamCmd( ClientData, Tcl_Interp *, int, char **);
static int plglevelCmd( ClientData, Tcl_Interp *, int, char **);
static int plgpageCmd( ClientData, Tcl_Interp *, int, char **);
static int plgraCmd( ClientData, Tcl_Interp *, int, char **);
static int plgspaCmd( ClientData, Tcl_Interp *, int, char **);
static int plgstrmCmd( ClientData, Tcl_Interp *, int, char **);
static int plgverCmd( ClientData, Tcl_Interp *, int, char **);
static int plgxaxCmd( ClientData, Tcl_Interp *, int, char **);
static int plgyaxCmd( ClientData, Tcl_Interp *, int, char **);
static int plgzaxCmd( ClientData, Tcl_Interp *, int, char **);
static int plhistCmd( ClientData, Tcl_Interp *, int, char **);
static int plhlsCmd( ClientData, Tcl_Interp *, int, char **);
static int plinitCmd( ClientData, Tcl_Interp *, int, char **);
static int pljoinCmd( ClientData, Tcl_Interp *, int, char **);
static int pllabCmd( ClientData, Tcl_Interp *, int, char **);
static int pllightsourceCmd( ClientData, Tcl_Interp *, int, char **);
static int pllineCmd( ClientData, Tcl_Interp *, int, char **);
static int plline3Cmd( ClientData, Tcl_Interp *, int, char **);
static int pllstyCmd( ClientData, Tcl_Interp *, int, char **);
static int plmkstrmCmd( ClientData, Tcl_Interp *, int, char **);
static int plmtexCmd( ClientData, Tcl_Interp *, int, char **);
static int plpatCmd( ClientData, Tcl_Interp *, int, char **);
static int plpoinCmd( ClientData, Tcl_Interp *, int, char **);
static int plpoin3Cmd( ClientData, Tcl_Interp *, int, char **);
static int plpoly3Cmd( ClientData, Tcl_Interp *, int, char **);
static int plprecCmd( ClientData, Tcl_Interp *, int, char **);
static int plpstyCmd( ClientData, Tcl_Interp *, int, char **);
static int plptexCmd( ClientData, Tcl_Interp *, int, char **);
static int plreplotCmd( ClientData, Tcl_Interp *, int, char **);
static int plrgbCmd( ClientData, Tcl_Interp *, int, char **);
static int plrgb1Cmd( ClientData, Tcl_Interp *, int, char **);
static int plschrCmd( ClientData, Tcl_Interp *, int, char **);
static int plscmap0Cmd( ClientData, Tcl_Interp *, int, char **);
static int plscmap0nCmd( ClientData, Tcl_Interp *, int, char **);
static int plscmap1Cmd( ClientData, Tcl_Interp *, int, char **);
static int plscmap1lCmd( ClientData, Tcl_Interp *, int, char **);
static int plscmap1nCmd( ClientData, Tcl_Interp *, int, char **);
static int plscol0Cmd( ClientData, Tcl_Interp *, int, char **);
static int plscolbgCmd( ClientData, Tcl_Interp *, int, char **);
static int plscolorCmd( ClientData, Tcl_Interp *, int, char **);
static int plsdevCmd( ClientData, Tcl_Interp *, int, char **);
static int plsdidevCmd( ClientData, Tcl_Interp *, int, char **);
static int plsdimapCmd( ClientData, Tcl_Interp *, int, char **);
static int plsdioriCmd( ClientData, Tcl_Interp *, int, char **);
static int plsdipltCmd( ClientData, Tcl_Interp *, int, char **);
static int plsdiplzCmd( ClientData, Tcl_Interp *, int, char **);
static int plsescCmd( ClientData, Tcl_Interp *, int, char **);
static int plsfamCmd( ClientData, Tcl_Interp *, int, char **);
static int plsfnamCmd( ClientData, Tcl_Interp *, int, char **);
static int plsmajCmd( ClientData, Tcl_Interp *, int, char **);
static int plsminCmd( ClientData, Tcl_Interp *, int, char **);
static int plsoriCmd( ClientData, Tcl_Interp *, int, char **);
static int plspageCmd( ClientData, Tcl_Interp *, int, char **);
static int plspauseCmd( ClientData, Tcl_Interp *, int, char **);
static int plsstrmCmd( ClientData, Tcl_Interp *, int, char **);
static int plssubCmd( ClientData, Tcl_Interp *, int, char **);
static int plssymCmd( ClientData, Tcl_Interp *, int, char **);
static int plstylCmd( ClientData, Tcl_Interp *, int, char **);
static int plsvpaCmd( ClientData, Tcl_Interp *, int, char **);
static int plsxaxCmd( ClientData, Tcl_Interp *, int, char **);
static int plsyaxCmd( ClientData, Tcl_Interp *, int, char **);
static int plsymCmd( ClientData, Tcl_Interp *, int, char **);
static int plszaxCmd( ClientData, Tcl_Interp *, int, char **);
static int pltextCmd( ClientData, Tcl_Interp *, int, char **);
static int plvaspCmd( ClientData, Tcl_Interp *, int, char **);
static int plvpasCmd( ClientData, Tcl_Interp *, int, char **);
static int plvporCmd( ClientData, Tcl_Interp *, int, char **);
static int plvstaCmd( ClientData, Tcl_Interp *, int, char **);
static int plw3dCmd( ClientData, Tcl_Interp *, int, char **);
static int plwidCmd( ClientData, Tcl_Interp *, int, char **);
static int plwindCmd( ClientData, Tcl_Interp *, int, char **);
static int plxormodCmd( ClientData, Tcl_Interp *, int, char **);
