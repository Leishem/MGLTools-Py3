/* efit_globals.h */

#ifdef EFIT_MAIN
#define WHERE
#else
#define WHERE extern
#endif

WHERE struct efit_info efit_info;
WHERE int debug,verbose,testflag;
WHERE char *programname;
WHERE struct datum ellipsedata[MAXPOINTS];	/* input points and weights */

/* FUNCTIONS */
/*char *index(); */
float gyration_of_ellipsoid();

/* end of file */
