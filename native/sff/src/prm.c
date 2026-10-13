
/*
 * COPYRIGHT 1992, REGENTS OF THE UNIVERSITY OF CALIFORNIA
 *
 *  prm.c - read information from an amber PARM topology file: 
 *	atom/residue/bond/charge info, plus force field data. 
 *	This file and the accompanying prm.h may be distributed 
 *	provided this notice is retained unmodified and provided 
 *	that any modifications to the rest of the file are noted 
 *	in comments.
 *
 *	Bill Ross, UCSF 1994
 */

/* MS modified to work with sff.c */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "prm.h"

#ifdef WIN32
#define popen _popen
#define pclose _pclose
#endif

extern int 	errno;

static int	debug = 0;	/* set it if you want */
static int  compressed = 0;

/*	fortran formats 
 *	 9118 FORMAT(12I6)
 *	 9128 FORMAT(5E16.8)
 */
char	*f9118 = "%6d%6d%6d%6d%6d%6d%6d%6d%6d%6d%6d%6d\n";


 
/***********************************************************************
                            SKIPEOLN()
************************************************************************/
 
/*
 *  skip to end of line; exit if eof encountered
 */
 
skipeoln(file)
FILE    *file;
{
    int i;
 
    while((i=getc(file)) != 10) {
        if (i==EOF) {
            printf("unexpected end in parm file\n");
            exit(1);
        }
    }
}

/***********************************************************************
                            ISCOMPRESSED()
************************************************************************/
 
/*
 *  iscompressed() - look for .Z at end of name
 */
 
int
iscompressed(name)
char    *name;
{
    int     i = strlen(name) - 1;   /* last char in name */
 
    if (i < 0) {
        fprintf(stderr, "programming error: name w/ length %d\n", i);
        exit(1);
    }
    if (i < 3)
        return(0);
    if (name[i] == 'Z'  &&  name[i-1] == '.')
        return(1);
    return(0);
}

/***********************************************************************
 							GENOPEN()
************************************************************************/
/*
 *  genopen() - fopen regular or popen compressed file for reading
 */

FILE *
genopen(name)
char	*name;
{
	struct stat	buf;
	char		cbuf[120];
	int		length;
	FILE		*fp;

	length = strlen(name);
	compressed = iscompressed(name);
	strcpy(cbuf, name);

	/*
	 *  if file doesn't exist, maybe it has been compressed/decompressed
	 */

	if (stat(cbuf, &buf) == -1) {
		switch (errno) {
		case ENOENT:	{
			if (!compressed) {
				strcat(cbuf, ".Z");
				if (stat(cbuf, &buf) == -1) {
					printf("%s, %s: does not exist\n", 
						name, cbuf);
					return(NULL);
				}
				compressed++;
				strcat(name, ".Z"); /* TODO: add protection */
			} else {
				cbuf[length-2] = '\0';
				if (stat(cbuf, &buf) == -1) {
					printf("%s, %s: does not exist\n", 
							name, cbuf);
					return(NULL);
				}
				compressed = 0;
			}
			break;
		}
		default:
		  printf("%s: sys err", name);
		  return(NULL);
		}
	}

	/*
	 *  open the file
	 */

	if (compressed) {
		char pcmd[120];

		sprintf(pcmd, "zcat %s", cbuf);
		if ((fp = popen(pcmd, "r")) == NULL) {
			perror(pcmd);
			exit(1);
		}
	} else {
		if ((fp = fopen(cbuf, "r")) == NULL) {
			perror(cbuf);
			exit(1);
		}
	}
	return(fp);
}

/***********************************************************************
 							GENCLOSE()
************************************************************************/

/*
 *  genclose() - close fopened or popened file
 */

genclose(fileptr, popn)
FILE	*fileptr;
{
	if (popn) {
		if (pclose(fileptr) == -1)
			perror("pclose");
	} else {
		if (fclose(fileptr) == -1)
			perror("fclose");
	}
}


/***********************************************************************
 							GET()
************************************************************************/

char *
get(size)
{
	char	*ptr;

#ifdef DEBUG
	printf("malloc %d\n", size);
	fflush(stdout);
#endif
	if (size ==0)
		return((char *) NULL);

	if ((ptr = (char *) malloc((unsigned)size)) == NULL) {
		printf("malloc %d", size);
		fflush(stdout);
		perror("malloc err:");
		exit(1);
	}
	return(ptr);
}

/***********************************************************************
 							PREADLN()
************************************************************************/

int
preadln(file, name, string)
FILE	*file;
char	*name, *string;
{
	int 	i, j;

	for (i=0; i<81; i++) {
		if ((j = getc(file)) == EOF) {
			printf("Error: unexpected EOF in %s\n", name);
			exit(1);
		}
		string[i] = (char) j;
		if (string[i] == '\n') {
			break;
		}
	}
	if (i == 80  &&  string[i] != '\n') {
		printf("Error: line too long in %s:\n%.80s", name, string);
		exit(1);
	}
}

/***************************************************************************
								READPARM()
****************************************************************************/

/* like strtok but one specifies the number of caracters to be read */
int get_int(char *buf, int nc)
{
  static char *line;
  char rec[1024];

  rec[0] = '\0';
  if (buf) line = buf;
  strncpy(rec, line, nc);
  rec[nc] = '\0';
  line += nc;
  return atoi(rec);
}

static void parreadmalloc( struct parm* prm)
{
  /*
   * get most of the indirect stuff; some extra allowed for char arrays
   */

  /* see out typemap and python wrapper for memory deallocation */

  prm->AtomNames = (char *) get(4*prm->Natom+81);
  prm->Charges = (_REAL *) get(sizeof(_REAL)*prm->Natom);
  prm->Masses = (_REAL *) get(sizeof(_REAL)*prm->Natom);
  prm->Iac = (int *) get(sizeof(int)*prm->Natom);
  prm->Iblo = (int *) get(sizeof(int)*prm->Natom);
  prm->Cno = (int *) get(sizeof(int)* prm->Ntype2d);
  prm->ResNames = (char *) get(4* prm->Nres+81);
  prm->Ipres = (int *) get(sizeof(int)*( prm->Nres+1));
  prm->Rk = (_REAL *) get(sizeof(_REAL)* prm->Numbnd);
  prm->Req = (_REAL *) get(sizeof(_REAL)* prm->Numbnd);
  prm->Tk = (_REAL *) get(sizeof(_REAL)* prm->Numang);
  prm->Teq = (_REAL *) get(sizeof(_REAL)* prm->Numang);
  prm->Pk = (_REAL *) get(sizeof(_REAL)* prm->Nptra);
  prm->Pn = (_REAL *) get(sizeof(_REAL)* prm->Nptra);
  prm->Phase = (_REAL *) get(sizeof(_REAL)* prm->Nptra);
  prm->Solty = (_REAL *) get(sizeof(_REAL)* prm->Natyp);
  prm->Cn1 = (_REAL *) get(sizeof(_REAL)* prm->Nttyp);
  prm->Cn2 = (_REAL *) get(sizeof(_REAL)* prm->Nttyp);
  prm->BondHAt1 = (int *) get(sizeof(int)* prm->Nbonh);
  prm->BondHAt2 = (int *) get(sizeof(int)* prm->Nbonh);
  prm->BondHNum = (int *) get(sizeof(int)* prm->Nbonh);
  prm->BondAt1 = (int *) get(sizeof(int)* prm->Nbona);
  prm->BondAt2 = (int *) get(sizeof(int)* prm->Nbona);
  prm->BondNum = (int *) get(sizeof(int)* prm->Nbona);
  prm->AngleHAt1 = (int *) get(sizeof(int)* prm->Ntheth);
  prm->AngleHAt2 = (int *) get(sizeof(int)* prm->Ntheth);
  prm->AngleHAt3 = (int *) get(sizeof(int)* prm->Ntheth);
  prm->AngleHNum = (int *) get(sizeof(int)* prm->Ntheth);
  prm->AngleAt1 = (int *) get(sizeof(int)* prm->Ntheta);
  prm->AngleAt2 = (int *) get(sizeof(int)*prm->Ntheta);
  prm->AngleAt3 = (int *) get(sizeof(int)*prm->Ntheta);
  prm->AngleNum = (int *) get(sizeof(int)*prm->Ntheta);
  prm->DihHAt1 = (int *) get(sizeof(int)*prm->Nphih);
  prm->DihHAt2 = (int *) get(sizeof(int)*prm->Nphih);
  prm->DihHAt3 = (int *) get(sizeof(int)*prm->Nphih);
  prm->DihHAt4 = (int *) get(sizeof(int)*prm->Nphih);
  prm->DihHNum = (int *) get(sizeof(int)*prm->Nphih);
  prm->DihAt1 = (int *) get(sizeof(int)*prm->Nphia);
  prm->DihAt2 = (int *) get(sizeof(int)*prm->Nphia);
  prm->DihAt3 = (int *) get(sizeof(int)*prm->Nphia);
  prm->DihAt4 = (int *) get(sizeof(int)*prm->Nphia);
  prm->DihNum = (int *) get(sizeof(int)*prm->Nphia);
  prm->ExclAt = (int *) get(sizeof(int)*prm->Nnb);
  prm->HB12 = (_REAL *) get(sizeof(_REAL)*prm->Nphb);
  prm->HB10 = (_REAL *) get(sizeof(_REAL)*prm->Nphb);
  prm->AtomSym = (char *) get(4*prm->Natom+81);
  prm->AtomTree = (char *) get(4*prm->Natom+81);
  prm->TreeJoin = (int *) get(sizeof(int)*prm->Natom);
  prm->AtomRes = (int *) get(sizeof(int)*prm->Natom);
  prm->N14pairs = (int *) get(sizeof(int)*prm->Natom);
  prm->N14pairlist = (int *) get(sizeof(int)*10*prm->Natom);
}

/*
 * readparm() - instantiate a given PARMSTRUCT
 */

parmstruct *
readparm(name)
char		*name;
{
	_REAL 		*H;
	int		i, k, idum, res, ifpert, iat, kat, lat, atype;
	int		*iptmp;
	FILE 		*file;
	parmstruct	*prm;
	char            buf[132], fld[20];

	printf("Reading parm file (%s)\n", name);

	if ((file = genopen(name, "parm")) == NULL) {
		fprintf( stderr, "Cannot open parm file %s\n", name );
		return(NULL);
	      }

	prm = (parmstruct *) get(sizeof(parmstruct));
	/* READ TITLE */

	{
	  char ititl[ 81];
	  preadln(file, name, ititl);
	  ititl[ sizeof ititl - 1] = '\0';
	  prm->ititl = strdup( ititl);
	}

	/* READ CONTROL INTEGERS */

	fgets(buf, 80, file);
	prm->Natom =  get_int(buf, 6);
	prm->Ntypes = get_int(NULL, 6);
	prm->Nbonh =  get_int(NULL, 6);
	prm->Mbona =  get_int(NULL, 6);
	prm->Ntheth = get_int(NULL, 6);
	prm->Mtheta = get_int(NULL, 6);
	prm->Nphih =  get_int(NULL, 6);
	prm->Mphia =  get_int(NULL, 6);
	prm->Nhparm = get_int(NULL, 6);
	prm->Nparm =  get_int(NULL, 6);
	prm->Nnb =    get_int(NULL, 6);
	prm->Nres =   get_int(NULL, 6);

	fgets(buf, 80, file);
	prm->Nbona  = get_int(buf, 6);
	prm->Ntheta = get_int(NULL, 6);
	prm->Nphia  = get_int(NULL, 6);
	prm->Numbnd = get_int(NULL, 6);
	prm->Numang = get_int(NULL, 6);
	prm->Nptra  = get_int(NULL, 6);
	prm->Natyp  = get_int(NULL, 6);
	prm->Nphb   = get_int(NULL, 6);
	ifpert = get_int(NULL, 6);
	idum = get_int(NULL, 6);
	idum = get_int(NULL, 6);
	idum = get_int(NULL, 6);

/* MS June 25 because in 2tmv9 Nnb=119232 ==> breaks reading scheme
	fscanf(file, f9118, 
		&prm->Natom,  &prm->Ntypes, &prm->Nbonh, &prm->Mbona, 
		&prm->Ntheth, &prm->Mtheta, &prm->Nphih, &prm->Mphia, 
		&prm->Nhparm, &prm->Nparm,  &prm->Nnb,   &prm->Nres);

	fscanf(file, f9118, 
		&prm->Nbona,  &prm->Ntheta, &prm->Nphia, &prm->Numbnd, 
		&prm->Numang, &prm->Nptra,  &prm->Natyp, &prm->Nphb, 
		&ifpert,      &idum,        &idum,       &idum);
*/
	if (ifpert) {
		printf("not equipped to read perturbation prmtop\n");
		free(prm);
		return(NULL);
	}
	fscanf(file, " %d %d %d %d %d %d", 
		&idum, &idum,&idum,&prm->IfBox,&prm->Nmxrs,&prm->IfCap);

	skipeoln(file);

	/* ALLOCATE MEMORY */

	prm->Nat3 = 3 * prm->Natom;
	prm->Ntype2d = prm->Ntypes * prm->Ntypes;
	prm->Nttyp = prm->Ntypes*(prm->Ntypes+1)/2;

	parreadmalloc( prm);
	iptmp = (int *) get(sizeof(int)*12*prm->Natom);

	/* 
	 * READ ATOM NAMES -IH(M04)
	 */

	for (i=0; i<(prm->Natom/20 + (prm->Natom%20 ? 1 : 0)); i++)
		preadln(file, "", &prm->AtomNames[i*80]);

	/* 
	 * READ ATOM CHARGES -X(L15)
	 *	(pre-multiplied by an energy factor of 18.2223 == sqrt(332)
	 *	 for faster force field calculations)
	 */

	for (i=0; i<prm->Natom; i++)
#ifdef DOUBLE
		fscanf(file, " %lf", &prm->Charges[i]);
#else
		fscanf(file, " %f", &prm->Charges[i]);
#endif
	skipeoln(file);

	/* 
	 * READ ATOM MASSES -X(L20)
	 */

	for (i=0; i<prm->Natom; i++)
#ifdef DOUBLE
		fscanf(file, " %le", &prm->Masses[i]);
#else
		fscanf(file, " %e", &prm->Masses[i]);
#endif
	skipeoln(file);

	/* 
	 * READ ATOM L-J TYPES -IX(I04)
	 */

	for (i=0; i<prm->Natom; i++)
		fscanf(file, " %d", &prm->Iac[i]);
	skipeoln(file);

	/* 
	 * READ ATOM INDEX TO 1st IN EXCLUDED ATOM LIST "NATEX" -IX(I08)
	 */

	for (i=0; i<prm->Natom; i++)
		fscanf(file, " %d", &prm->Iblo[i]);
	skipeoln(file);

	/* 
	 * READ TYPE INDEX TO N-B TYPE -IX(I06)
	 */

	for (i=0; i<prm->Ntype2d; i++){
		fscanf(file, " %d", &prm->Cno[i]);
#ifdef CHECK_10_12
			/* add check that this is positive, since current routines
			   in sff.c do not support 10-12 potential terms    */
		if( prm->Cno[i] < 0 ){
			fprintf( stderr, "parameter topology requires 10-12 terms\n" );
			fprintf( stderr, "  these are not currently supported\n" );
			exit( 1 );
		}
#endif
	}
	skipeoln(file);

	/* 
	 * READ RES NAMES (4 chars each, 4th blank) -IH(M02)
	 */

	for (i=0; i<(prm->Nres/20 + (prm->Nres%20 ? 1 : 0)); i++)
		preadln(file, "", &prm->ResNames[i*80]);

	/* 
	 * READ RES POINTERS TO 1st ATOM 		-IX(I02)
	 */

	for (i=0; i<prm->Nres; i++) 
		fscanf(file, " %d", &prm->Ipres[i]);
	prm->Ipres[prm->Nres] = prm->Natom + 1;
	skipeoln(file);

	/* 
	 * READ BOND FORCE CONSTANTS 			-RK()
	 */

	for (i=0; i< prm->Numbnd; i++) 
#ifdef DOUBLE
		fscanf(file, " %lf", &prm->Rk[i]);
#else
		fscanf(file, " %f", &prm->Rk[i]);
#endif
	skipeoln(file);

	/* 
	 * READ BOND LENGTH OF MINIMUM ENERGY  		-REQ()
	 */

	for (i=0; i< prm->Numbnd; i++) 
#ifdef DOUBLE
		fscanf(file, " %lf", &prm->Req[i]);
#else
		fscanf(file, " %f", &prm->Req[i]);
#endif
	skipeoln(file);

	/* 
	 * READ BOND ANGLE FORCE CONSTANTS (following Rk nomen) -TK()
	 */

	for (i=0; i< prm->Numang; i++) 
#ifdef DOUBLE
		fscanf(file, " %lf", &prm->Tk[i]);
#else
		fscanf(file, " %f", &prm->Tk[i]);
#endif
	skipeoln(file);

	/* 
	 * READ BOND ANGLE OF MINIMUM ENERGY (following Req nomen) -TEQ()
	 */

	for (i=0; i< prm->Numang; i++) 
#ifdef DOUBLE
		fscanf(file, " %lf", &prm->Teq[i]);
#else
		fscanf(file, " %f", &prm->Teq[i]);
#endif
	skipeoln(file);

	/* 
	 * READ DIHEDRAL PEAK MAGNITUDE 		-PK()
	 */

	for (i=0; i< prm->Nptra; i++) 
#ifdef DOUBLE
		fscanf(file, " %lf", &prm->Pk[i]);
#else
		fscanf(file, " %f", &prm->Pk[i]);
#endif
	skipeoln(file);

	/* 
	 * READ DIHEDRAL PERIODICITY 			-PN()
	 */

	for (i=0; i< prm->Nptra; i++) 
#ifdef DOUBLE
		fscanf(file, " %lf", &prm->Pn[i]);
#else
		fscanf(file, " %f", &prm->Pn[i]);
#endif
	skipeoln(file);

	/* 
	 * READ DIHEDRAL PHASE  			-PHASE()
	 */

	for (i=0; i< prm->Nptra; i++) 
#ifdef DOUBLE
		fscanf(file, " %lf", &prm->Phase[i]);
#else
		fscanf(file, " %f", &prm->Phase[i]);
#endif
	skipeoln(file);

	/* 
	 * ?? "RESERVED" 				-SOLTY()
	 */

	for (i=0; i< prm->Natyp; i++) 
#ifdef DOUBLE
		fscanf(file, " %lf", &prm->Solty[i]);
#else
		fscanf(file, " %f", &prm->Solty[i]);
#endif
	skipeoln(file);

	/* 
	 * READ L-J R**12 FOR ALL PAIRS OF ATOM TYPES  	-CN1()
	 *	(SHOULD BE 0 WHERE H-BONDS)
	 */

	for (i=0; i< prm->Nttyp; i++) 
#ifdef DOUBLE
		fscanf(file, " %lf", &prm->Cn1[i]);
#else
		fscanf(file, " %f", &prm->Cn1[i]);
#endif
	skipeoln(file);

	/* 
	 * READ L-J R**6 FOR ALL PAIRS OF ATOM TYPES 	-CN2()
	 *	(SHOULD BE 0 WHERE H-BONDS)
	 */

	for (i=0; i< prm->Nttyp; i++) 
#ifdef DOUBLE
		fscanf(file, " %lf", &prm->Cn2[i]);
#else
		fscanf(file, " %f", &prm->Cn2[i]);
#endif
	skipeoln(file);

	/* 
	 * READ COVALENT BOND W/ HYDROGEN (3*(atnum-1)): 
	 *	IBH = ATOM1 		-IX(I12)
	 *	JBH = ATOM2 		-IX(I14)
	 *	ICBH = BOND ARRAY PTR	-IX(I16)
	 */

	for (i=0; i<prm->Nbonh; i++) 
		fscanf(file, " %d %d %d", 
		    &prm->BondHAt1[i], &prm->BondHAt2[i], &prm->BondHNum[i]);
	skipeoln(file);

	/* 
	 * READ COVALENT BOND W/OUT HYDROGEN (3*(atnum-1)):
	 *	IB = ATOM1		-IX(I18)
	 *	JB = ATOM2		-IX(I20)
	 *	ICB = BOND ARRAY PTR	-IX(I22)
	 */

	for (i=0; i<prm->Nbona; i++)
		fscanf(file, " %d %d %d", 
			&prm->BondAt1[i], &prm->BondAt2[i], &prm->BondNum[i]);
	skipeoln(file);

	/* 
	 * READ ANGLE W/ HYDROGEN: 
	 *	ITH = ATOM1			-IX(I24)
	 *	JTH = ATOM2			-IX(I26)
	 *	KTH = ATOM3			-IX(I28)
	 *	ICTH = ANGLE ARRAY PTR		-IX(I30)
	 */

	for (i=0; i<prm->Ntheth; i++)
		fscanf(file, " %d %d %d %d", 
		    		&prm->AngleHAt1[i], &prm->AngleHAt2[i], 
				&prm->AngleHAt3[i], &prm->AngleHNum[i]);
	skipeoln(file);

	/* 
	 * READ ANGLE W/OUT HYDROGEN: 
	 *	IT = ATOM1			-IX(I32)
	 *	JT = ATOM2			-IX(I34)
	 *	KT = ATOM3			-IX(I36)
	 *	ICT = ANGLE ARRAY PTR		-IX(I38)
	 */

	for (i=0; i<prm->Ntheta; i++)
		fscanf(file, " %d %d %d %d", 
		    		&prm->AngleAt1[i], &prm->AngleAt2[i], 
				&prm->AngleAt3[i], &prm->AngleNum[i]);
	skipeoln(file);

	/* 
	 * READ DIHEDRAL W/ HYDROGEN: 
	 *	ITH = ATOM1			-IX(40)
	 *	JTH = ATOM2			-IX(42)
	 *	KTH = ATOM3			-IX(44)
	 *	LTH = ATOM4			-IX(46)
	 *	ICTH = DIHEDRAL ARRAY PTR	-IX(48)
	 */

	for (i=0; i<prm->Nphih; i++)
		fscanf(file, " %d %d %d %d %d", 
		    	&prm->DihHAt1[i], &prm->DihHAt2[i], &prm->DihHAt3[i], 
			&prm->DihHAt4[i], &prm->DihHNum[i]);
	skipeoln(file);

	/* 
	 * READ DIHEDRAL W/OUT HYDROGEN: 
	 *	IT = ATOM1
	 *	JT = ATOM2
	 *	KT = ATOM3
	 *	LT = ATOM4
	 *	ICT = DIHEDRAL ARRAY PTR
	 */

	for (i=0; i<prm->Nphia; i++) {
		fscanf(file, " %d %d %d %d %d", 
		    	&prm->DihAt1[i], &prm->DihAt2[i], &prm->DihAt3[i], 
			&prm->DihAt4[i], &prm->DihNum[i]);
	}
	skipeoln(file);

	/*
	 * READ EXCLUDED ATOM LIST	-IX(I10)
	 */
	for (i=0; i<prm->Nnb; i++)
		fscanf(file, " %d", &prm->ExclAt[i]);
	skipeoln(file);

	/*
	 * READ H-BOND R**12 TERM FOR ALL N-B TYPES	-ASOL()
	 */

	for (i=0; i<prm->Nphb; i++) 
#ifdef DOUBLE
		fscanf(file, " %lf", &prm->HB12[i]);
#else
		fscanf(file, " %f", &prm->HB12[i]);
#endif
	skipeoln(file);

	/*
	 * READ H-BOND R**10 TERM FOR ALL N-B TYPES	-BSOL()
	 */

	for (i=0; i<prm->Nphb; i++) 
#ifdef DOUBLE
		fscanf(file, " %lf", &prm->HB10[i]);
#else
		fscanf(file, " %f", &prm->HB10[i]);
#endif
	skipeoln(file);
      
	/*
	 * READ H-BOND CUTOFF (NOT USED) ??		-HBCUT()
	 */

	H = (_REAL *) get(prm->Nphb * sizeof(_REAL));
	for (i=0; i<prm->Nphb; i++) 
#ifdef DOUBLE
		fscanf(file, " %lf", &H[i]);
#else
		fscanf(file, " %f", &H[i]);
#endif
	free((char *)H);
	
	skipeoln(file);
      
	/*
	 * READ ATOM SYMBOLS (FOR ANALYSIS PROGS)	-IH(M06)
	 */

	for (i=0; i<(prm->Natom/20 + (prm->Natom%20 ? 1 : 0)); i++)
		preadln(file, "", &prm->AtomSym[i*80]);

	/*
	 * READ TREE SYMBOLS (FOR ANALYSIS PROGS)	-IH(M08)
	 */

	for (i=0; i<(prm->Natom/20 + (prm->Natom%20 ? 1 : 0)); i++)
		preadln(file, "", &prm->AtomTree[i*80]);
      
	/*
	 * READ TREE JOIN INFO (FOR ANALYSIS PROGS)	-IX(I64)
	 */

	for (i=0; i<prm->Natom; i++)
		fscanf(file, " %d", &prm->TreeJoin[i]);
	skipeoln(file);
      
	/*
	 * READ PER-ATOM RES NUMBER			-IX(I66)
	 *	NOTE: this appears to be something entirely different
	 *	NOTE: overwriting this with correct PER-ATOM RES NUMBERs
	 */

	for (i=0; i<prm->Natom; i++)
		fscanf(file, " %d", &prm->AtomRes[i]);
	res = 0;
	for (i=0; i<prm->Natom; i++) {
		if (i+1 == prm->Ipres[res+1])	/* atom is 1st of next res */
			res++;
		prm->AtomRes[i] = res;
	}
      
	/*
	 * BOUNDARY CONDITION STUFF
	 */

	if (!prm->IfBox) {
		prm->Nspm = 1;
		prm->Boundary = (int *) get(sizeof(int)*prm->Nspm);
		prm->Boundary[0] = prm->Natom;
	} else {
		skipeoln(file);
		fscanf(file, " %d %d %d", &prm->Iptres, &prm->Nspm, 
								&prm->Nspsol);
		skipeoln(file);
		prm->Boundary = (int *) get(sizeof(int)*prm->Nspm);
		for (i=0; i<prm->Nspm; i++)
			fscanf(file, " %d", &prm->Boundary[i]);
		skipeoln(file);
#ifdef DOUBLE
		fscanf(file, " %lf %lf %lf", 
#else
		fscanf(file, " %f %f %f", 
#endif
				&prm->Box[0], &prm->Box[1], &prm->Box[2]);
		skipeoln(file);
		if (prm->Iptres)
			prm->Ipatm = prm->Ipres[prm->Iptres] - 1; 
      		/* IF(IPTRES.GT.0) IPTATM = IX(I02+IPTRES-1+1)-1 */
	}

	/*
	 * ----- LOAD THE CAP INFORMATION IF NEEDED -----
	 */

	if (prm->IfCap) {
		/* if (prm->IfBox) 
			skipeoln(file); */
#ifdef DOUBLE
		fscanf(file, " %d %lf %lf %lf %lf", 
#else
		fscanf(file, " %d %f %f %f %f", 
#endif
				&prm->Natcap, &prm->Cutcap, 
				&prm->Xcap, &prm->Ycap, &prm->Zcap);
	}
	genclose(file, compressed);
        if (debug) {
		printf("rdprm done\n");
		fflush(stdout);
	}

	/*
	 * -------CONSTRUCT A 1-4 LIST -------
	 */

	for( i=0; i<prm->Natom; i++ ) prm->N14pairs[i] = 0;
	for( i=0; i<prm->Nphih; i++ ){
		iat = prm->DihHAt1[i]/3; 
		kat = prm->DihHAt3[i]/3;
		lat = prm->DihHAt4[i]/3;
		atype = prm->DihHNum[ i ] - 1;
		if( kat >= 0 && lat >= 0 ) 
			iptmp[12*iat + prm->N14pairs[iat]++] = lat;
	}
	for( i=0; i<prm->Mphia; i++ ){
		iat = prm->DihAt1[i]/3; 
		kat = prm->DihAt3[i]/3;
		lat = prm->DihAt4[i]/3;
		atype = prm->DihNum[ i ] - 1;
		if( kat >= 0 && lat >= 0 ) 
			iptmp[12*iat + prm->N14pairs[iat]++] = lat;
	}
	idum = 0;
	for( i=0; i<prm->Natom; i++ ){
		for( k=0; k<prm->N14pairs[i]; k++ )
			 prm->N14pairlist[idum++] = iptmp[12*i + k];
	}
#ifdef PRINT_14PAIRS
    printf( "npairs:\n" );
    for( k=0; k<prm->Natom; k++ ){
        printf( "%4d", prm->N14pairs[k] );
        if( (k+1)%20 == 0 ) printf( "\n" );
    }
    printf( "\npairlist:\n" );
    for( k=0; k<idum-1; k++ ){
        printf( "%4d", prm->N14pairlist[k] );
        if( (k+1)%20 == 0 ) printf( "\n" );
    }
    printf( "\n" );
#endif
	free( iptmp );

	return(prm);
}

int
firstwat(prm)
parmstruct *prm;
{
	char	*restr = prm->ResNames; 
	char	*lastres = prm->ResNames + prm->Nres * 4 + 1;
	int	res = 0;

	/*
	 *  find 1st water residue
	 */

	for (; restr<lastres; restr+=4) {
		if (!strncmp(restr, "WAT ", 4)) {
		  printf("first water: res = %d, atom = %d (%.4s)\n", 
					res+1, prm->Ipres[res],
					&prm->AtomNames[prm->Ipres[res]]);
		  fflush(stdout);
		  return(prm->Ipres[res]-1);
		}
		res++;
	}
	return(0);
}


int readcrd(char *name, _REAL ***coord, parmstruct *parm)
{
  FILE 	*file;
  _REAL **c;
  int   i, j, nat;
  char	ititl[81];
  
  printf("Reading crd file (%s)\n", name);
  (*coord) = NULL;

  if ((file = genopen(name, "parm")) == NULL) 
    return(0);

  preadln(file, name, ititl);
  ititl[80] = '\0';
  if (strcmp(ititl, parm->ititl)!=0) {
    printf("WARNING: crd file title different from top file title\n");
  }

  /* READ NUMBER OF ATOMS */
  i = fscanf(file, "%d", &nat);
  if (i!=1) {
    printf("Error on line 2 of %s (wrong number of atoms)\n",name);
    genclose(file, name);
    return(0);
  }
  if (nat != parm->Natom) {
    printf("ERROR: number of atoms in crd file doesn't match\n");
    genclose(file, name);
    return(0);
  }
    
  /* ALLOCATE MEMORY */
  c = (_REAL **) get(nat*sizeof(_REAL *));
  for (i=0; i<nat; i++)
    c[i] = (_REAL *) get(3*sizeof(_REAL));

  /* READ THE COORDINATES */
  for (i=0; i<nat; i++) {
#ifdef DOUBLE
    j = fscanf(file, " %lf %lf %lf", &c[i][0], &c[i][1], &c[i][2]);
#else
    j = fscanf(file, " %f %f %f", &c[i][0], &c[i][1], &c[i][2]);
#endif
    if (j != 3) {
      for (j=0; j<nat; j++) free(c[j]);
      free(c);
      genclose(file, name);
      return(0);
    }
  }
  (*coord) = c;
  genclose(file, name);
  return(nat);
}


_REAL **readcrdvec(char *name, parmstruct *parm, int *nat)
{
  FILE 	*file;
  _REAL **c;
  int   i, j, n;
  char	ititl[81];

  printf("Reading crd file (%s)\n", name);
  (*nat) = 0;

  if ((file = genopen(name, "parm")) == NULL) 
    return(0);

  preadln(file, name, ititl);
  ititl[80] = '\0';
  if (strcmp(ititl, parm->ititl)!=0) {
    printf("WARNING: crd file title different from top file title\n");
  }

  /* READ NUMBER OF ATOMS */
  i = fscanf(file, "%d", nat);
  if (i!=1) {
    printf("Error on line 2 of %s (wrong number of atoms)\n",name);
    genclose(file, name);
    return(0);
  }
  if (*nat != parm->Natom) {
    printf("ERROR: number of atoms in crd file doesn't match\n");
    genclose(file, name);
    return(0);
  }
    
  /* ALLOCATE MEMORY */
  c = (_REAL **) get((*nat)*sizeof(_REAL *));

  /* READ THE COORDINATES */
  for (i=0; i<(*nat); i++) {
    c[i] = (_REAL *) get(3*sizeof(_REAL));
#ifdef DOUBLE
    j = fscanf(file, " %lf %lf %lf", &c[i][0], &c[i][1], &c[i][2]);
#else
    j = fscanf(file, " %f %f %f", &c[i][0], &c[i][1], &c[i][2]);
#endif
    if (j != 3) {
      for (j=0; j<i; j++) free(c[j]);
      free(c);
      genclose(file, compressed);
      return(NULL);
    }
  }
  genclose(file, compressed);
  return(c);
}
