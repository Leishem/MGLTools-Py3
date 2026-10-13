/* "Stride Module" */
%module stride
%include Misc.i  /* used for FILE * */
#define %immutable   %feature("immutable")

%text %{
This module wraps the Stride program to analyze protein structures and assign
secondary structures.

...

example:

      import stridec

References:

    - Sanner, M.F., Spehner, J.-C., and Olson, A.J. (1996) Reduced surface: 
      an efficient way to compute molecular surfaces. Biopolymers, Vol. 38, 
      (3), 305-320.
    - Michel F. Sanner and Arthur J. olson. (1997) Real Time Surface 
      Reconstruction For moving Molecular Fragments. Proc. Second Pacific 
      Symposium in Biocomputing.
%}


%{
#include <stride.h>
%}

/*------------------------------------------------------------------------
 * 
 *   constant definitions
 * 
 *----------------------------------------------------------------------*/
#define BREAKDIST                 2.5
#define SSDIST                    3.0

%immutable;
#define SUCCESS                   1
#define FAILURE                   0
#define YES                       1
#define NO                        0
#define ERR                      -1

#define BUFSZ                     1024
#define MAX_FIELD                 50

#define MAX_AtomType              200
#define MAX_ResType               50
#define MAXNONSTAND               4.0
#define MAX_CHAIN                 100
#define MAX_RES                   10000
#define MAX_HETRES                10000
#define MAX_HET                   100
#define MAX_HELIX                 100
#define MAX_SHEET                 100
#define MAX_STRAND_IN_SHEET       20
#define MAX_TURN                  100
#define MAX_BOND                  100
#define MAX_ASSIGN                300
#define MAX_INFO                  100
#define MAX_AT_IN_RES             50
#define MAX_AT_IN_HETERORES       200
#define MAXRESDNR                 6
#define MAXRESACC                 6 

#define RES_FIELD                 6
#define AT_FIELD                  5

#define MAX_X                180.000
#define MAX_Y                180.000
#define MAX_Z                180.000
#define MIN_X               -100.000
#define MIN_Y               -100.000
#define MIN_Z               -100.000
#define MAX_Occupancy           1.00
#define MIN_Occupancy           0.00
#define MAX_TempFactor       1000.00
#define MIN_TempFactor          0.00

#define OUTPUTWIDTH               80
#define MAXCONDITIONS             20

#define MAXHYDRBOND               10000
#define MAXDONOR                  MAX_RES
#define MAXACCEPTOR               MAX_RES

#define	MINPHIPSI     	         -180.0
#define MAXPHIPSI      	          180.0
#define DEFNUMPIXEL               18

#define DIST_N_H                  1.0
#define RmGRID                    3.0
#define EmGRID                   -2.8

#define MINACCANG_SP2   90.0
#define MAXACCANG_SP2   180.0
#define MINACCANG_SP3   60.0
#define MAXACCANG_SP3   180.0
#define MINDONANG_SP2   90.0
#define MAXDONANG_SP2   180.0
#define MINDONANG_SP3   90.0
#define MAXDONANG_SP3   180.0
#define ACCDONANG       60.0
#define DONACCANG       90.0

#define NAcd            20

enum ASNSOURCE {Stride, Pdb, Dssp};
enum METHOD {XRay, NMR, Model};
enum HYBRID {Nsp2, Nsp3, Osp2, Osp3, Ssp3};
enum GROUP  {Peptide, Trp, Asn, Gln, Arg, His, Lys, Ser, Thr, Tyr, Asp, Glu, Met, Cys};
enum HBONDTYPE {MM, MS, SM, SS };

#define MAX_FILE 500

%mutable;

/*------------------------------------------------------------------------
 * 
 *   data structures
 * 
 *----------------------------------------------------------------------*/

/* typedef char BUFFER[BUFSZ+1];
   replaced them by char Type[BUFSZ+1] */

%{
#include <string.h>
%}
%typemap(memberin) char [BUFSZ+1] {
  strcpy( $1, $input );
}

typedef struct {
                 char *FileName[MAX_FILE];
		 char *ChainId[MAX_FILE];
	       } CHAINLIST;

typedef struct {
                 float Phi, Psi, Omega;
		 int PhiZn, PsiZn;
		 float Solv, DsspSolv;
		 char Asn, PdbAsn, DsspAsn;
	       } PROPERTY;

typedef struct {
                 int HBondDnr[MAXRESDNR];
                 int HBondAcc[MAXRESACC];
		 int NBondDnr, NBondAcc;
		 int InterchainHBonds;
	       } INVOLVED;

%typemap (out) char OUTPUT[RES_FIELD]
%{
  $result = PyString_FromString($1);
%}

%apply char OUTPUT[RES_FIELD] {char PDB_ResNumb[RES_FIELD],
				 char ResType[RES_FIELD]} 
typedef struct {
                 int NAtom;
                 char PDB_ResNumb[RES_FIELD];
                 char ResType[RES_FIELD];
		 char AtomType[MAX_AT_IN_RES][AT_FIELD];
		 float Coord[MAX_AT_IN_RES][3];
		 float Occupancy[MAX_AT_IN_RES];
		 float TempFactor[MAX_AT_IN_RES];
		 PROPERTY *Prop;
		 INVOLVED *Inv;
	       } RESIDUE;

%extend RESIDUE {
 void _Coord( int i, float OUT_VECTOR[3] ) {
    OUT_VECTOR[0] = self->Coord[i][0];
    OUT_VECTOR[1] = self->Coord[i][1];
    OUT_VECTOR[2] = self->Coord[i][2];
 }
 char *getAtomType(int i) {
   if (i < self->NAtom) return self->AtomType[i];
   else return NULL;
 }
 
}

typedef struct {
                 int NAtom;
		 char PDB_ResNumb[RES_FIELD];
		 char ResType[RES_FIELD];
		 char AtomType[MAX_AT_IN_HETERORES][AT_FIELD];
		 char Mendeleev[MAX_AT_IN_HETERORES];
		 float Coord[MAX_AT_IN_HETERORES][3];
		 float Occupancy[MAX_AT_IN_HETERORES];
		 float TempFactor[MAX_AT_IN_HETERORES];
	       } HETERORESIDUE;

typedef struct {
                 char HetId[4];
		 char PDB_ResNumb[RES_FIELD];
		 char InsCode;
		 int AtomNumb;
	       } HET;

typedef struct {
                 char Res1[RES_FIELD];
		 char Res2[RES_FIELD];
		 char PDB_ResNumb1[RES_FIELD];
		 char PDB_ResNumb2[RES_FIELD];
		 char InsCode1;
                 char InsCode2;
		 int Class;
	       } HELIX;

typedef struct {
                 int  NStrand;
		 char SheetId[RES_FIELD];
		 char ResType1[MAX_STRAND_IN_SHEET][RES_FIELD];
		 char ResType2[MAX_STRAND_IN_SHEET][RES_FIELD];
		 char PDB_ResNumb1[MAX_STRAND_IN_SHEET][RES_FIELD];
		 char PDB_ResNumb2[MAX_STRAND_IN_SHEET][RES_FIELD];
		 char InsCode1[MAX_STRAND_IN_SHEET];
		 char InsCode2[MAX_STRAND_IN_SHEET];
		 int  Sence[MAX_STRAND_IN_SHEET];
		 int  RegYN[MAX_STRAND_IN_SHEET];
		 char AtomNameReg1[MAX_STRAND_IN_SHEET][AT_FIELD];
		 char AtomNameReg2[MAX_STRAND_IN_SHEET][AT_FIELD];
		 char ResTypeReg1[MAX_STRAND_IN_SHEET][RES_FIELD];
		 char ResTypeReg2[MAX_STRAND_IN_SHEET][RES_FIELD];
		 char PDB_ResNumbReg1[MAX_STRAND_IN_SHEET][RES_FIELD];
		 char PDB_ResNumbReg2[MAX_STRAND_IN_SHEET][RES_FIELD];
		 char InsCodeReg1[MAX_STRAND_IN_SHEET];
		 char InsCodeReg2[MAX_STRAND_IN_SHEET];
	       } SHEET;

typedef struct {
                 char Res1[RES_FIELD];
		 char Res2[RES_FIELD];
		 char PDB_ResNumb1[RES_FIELD];
                 char PDB_ResNumb2[RES_FIELD];
		 char InsCode1;
                 char InsCode2;
		 char TurnType;
	       } TURN;

typedef struct {
                 char PDB_ResNumb1[RES_FIELD];
                 char PDB_ResNumb2[RES_FIELD];
                 char InsCode1, InsCode2;
		 char ChainId1, ChainId2;
		 enum ASNSOURCE AsnSource;
	       } SSBOND;

typedef struct {
                 int NRes, NHetRes, NonStandRes, Ter;
		 int NHet, NAtom, NonStandAtom, NHelix, NSheet;
		 int NTurn, NAssignedTurn, NBond, NHydrBond, NHydrBondInterchain, NHydrBondTotal, NInfo;
		 char Id, *File;
		 float Resolution;
		 enum METHOD Method;
		 int Valid, Published, DsspAssigned;

		 RESIDUE **Rsd;
		 HETERORESIDUE **HetRsd;
		 HET     **Het;
		 HELIX   **Helix;
		 SHEET   **Sheet;
		 TURN    **Turn;
		 TURN    **AssignedTurn;
		 SSBOND  **SSbond;
		 char    **Info;
		 char    PdbIdent[5];

	       } CHAIN;

%extend CHAIN {
    RESIDUE *getResidue(int i) {
      if (i < self->NRes) return self->Rsd[i];
      else return NULL;
    }
    HETERORESIDUE *getHeteroResidue(int i) {
      if (i < self->NHetRes) return self->HetRsd[i];
      else return NULL;
    }
    HET *getHet(int i) {
      if (i < self->NHet) return self->Het[i];
      else return NULL;
    }
    HELIX *getHelix(int i) {
      if (i < self->NHelix) return self->Helix[i];
      else return NULL;
    }
    SHEET *getSheet(int i) {
      if (i < self->NSheet) return self->Sheet[i];
      else return NULL;
    }
    TURN *getTurn(int i) {
      if (i < self->NTurn) return self->Turn[i];
      else return NULL;
    }
    TURN *getAssignedTurn(int i) {
      if (i < self->NAssignedTurn) return self->AssignedTurn[i];
      else return NULL;
    }
    SSBOND *getSsbond(int i) { /* ???? NBOND ??? */
      if (i < self->NBond) return self->SSbond[i];
      else return NULL;
    }
}

/*  %{ */
/*  static PyObject *getResidues(PyObject *self, PyObject *args, PyObject *kwargs) { */
/*    PyObject * _argo0 = 0; */
/*    PyObject * result; */
/*    char *_kwnames[] = { "self", NULL }; */
/*    CHAIN * chain; */
/*    RESIDUE * res; */
/*    char _ptemp[128]; */
/*    int i; */

/*    if(!PyArg_ParseTupleAndKeywords(args,kwargs,"O:CHAIN_Rsd_get",_kwnames,&_argo0))  */
/*      return NULL; */
/*    if (_argo0) { */
/*      if (_argo0 == Py_None) { chain = NULL; } */
/*      else if (SWIG_GetPtrObj(_argo0,(void **) &chain,"_CHAIN_p")) { */
/*        PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of CHAIN_Rsd_get. Expected _CHAIN_p."); */
/*        return NULL; */
/*      } */
/*    } */
/*    result = PyList_New(0); */
/*    for (i=0; i<chain->NRes; i++) { */
/*      SWIG_MakePtr(_ptemp, (char *) chain->Rsd[i],"_RESIDUE_p"); */
/*      PyList_Append(result, Py_BuildValue("s",_ptemp)); */
/*    } */
/*    return result; */
/*  } */
/*  %} */
/*  %native (getResidues) getResidues; */

typedef struct {
                 CHAIN *Chain;
		 int D_Res, DD_Res, DDI_Res;
 		 int D_At, DD_At, DDI_At, H;
		 enum HYBRID Hybrid;
		 enum GROUP Group;
		 float HB_Radius;
	       } DONOR;

                 
typedef struct {
                 CHAIN *Chain;
                 int A_Res, AA_Res, AA2_Res;
		 int A_At, AA_At, AA2_At;
		 enum HYBRID Hybrid;
		 enum GROUP Group;
		 float HB_Radius;
	       } ACCEPTOR;

typedef struct { 
                 char InputFile[BUFSZ+1];
		 char OutFile[BUFSZ+1];
                 char SeqFile[BUFSZ+1];
		 char MapFileHelix[BUFSZ+1];
		 char MapFileSheet[BUFSZ+1];
		 char MolScriptFile[BUFSZ+1];
		 char DsspFile[BUFSZ+1];
		 char EnergyType, Active[MAX_CHAIN+1]; 
		 char Processed[MAX_CHAIN+1];
                 char Cond[MAXCONDITIONS];

                 int NPixel, NActive, NProcessed;
		 int MinLength, MaxLength;

		 float PhiPsiStep, DistCutOff;
		 float Treshold_H1, Treshold_H2, Treshold_H3, Treshold_H4;
		 float Treshold_E1, Treshold_E2, Treshold_E3, Treshold_E4;
		 float MinResolution, MaxResolution;
		 float C1_H, C2_H, C1_E, C2_E;

		 int SideChainHBond, MainChainHBond, MainChainPolarInt;
		 int Published, DsspAssigned, UseResolution, Info, Truncate;
		 int ExposedArea, ReportSummaryOnly, ReportBonds, BrookhavenAsn, DsspAsn;
		 int MolScript, OutSeq, Stringent, Measure;

		 /* Not used by STRIDE */
		 int Shrink;
		 char CarteFile[BUFSZ+1];
		 char MapFile[BUFSZ+1];
		 char MathFile[BUFSZ+1];
		 char AsnSource, Mode, SecStrType;
		 int FilterOrder;
		 int NStepA, NStepB, NStepC, NStepD;
		 float Treshold_From_A, Treshold_To_A, StepA;
		 float Treshold_From_B, Treshold_To_B, StepB;
		 float Treshold_From_C, Treshold_To_C, StepC;
		 float Treshold_From_D, Treshold_To_D, StepD;

	       } COMMAND;

typedef struct {
                 DONOR *Dnr;
		 ACCEPTOR *Acc;
		 int ExistPolarInter, ExistHydrBondRose, ExistHydrBondBaker;
		 float Energy, Er, Et, Ep, ti, to, p;
		 float AccDonDist, OHDist, AngNHO, AngCOH;
		 float AccAng, DonAng, AccDonAng, DonAccAng;
	       } HBOND;

typedef struct PAT {
                 HBOND *Hb1, *Hb2;
		 struct PAT *Nei1, *Nei2;
		 int ExistPattern;
		 char Type[BUFSZ+1];
	       } PATTERN;

typedef struct {
		 char File[BUFSZ+1];
		 char Id;
                 int NRes;
		 char **ResType;
		 char *SecondStr;
		 char **PDB_ResNumb;
		 float *Accessibility;
	       } DSSP;

typedef struct {
                 int TP, TN, FP, FN;
		 float Corr, Perc;
	       } QUALITY;

%{
#include <stdio.h>
#include "protot.h"
#include "nsc.h"

typedef struct {
  CHAIN **Chain;
  HBOND **HBond;
  COMMAND *Cmd;
  int      NChain;
  int      NHBond;
  int      ValidChain;
  int      report;
} STRIDE ;


void DefaultCmd(COMMAND *Cmd)
{

  Cmd->SideChainHBond    = NO;
  Cmd->MainChainHBond    = YES;
  Cmd->MainChainPolarInt = YES;
  Cmd->Published         = NO;
  Cmd->DsspAssigned      = NO;
  Cmd->UseResolution     = NO;
  Cmd->Info              = NO;
  Cmd->Truncate          = YES;
  Cmd->ExposedArea       = YES;
  Cmd->ReportSummaryOnly = NO;
  Cmd->ReportBonds       = NO;
  Cmd->BrookhavenAsn     = NO;
  Cmd->DsspAsn           = NO;
  Cmd->MolScript         = NO;
  Cmd->OutSeq            = NO;
  Cmd->Stringent         = NO;
  Cmd->Measure           = NO;

  Cmd->EnergyType        = 'G';

  Cmd->DistCutOff        =  6.0;
  Cmd->PhiPsiStep        =  0.0;

  Cmd->C1_H              = -1.0;
  Cmd->C2_H              =  1.0;
  Cmd->C1_E              = -0.2;
  Cmd->C2_E              =  0.2;

  Cmd->Treshold_H1       = -230.0;
  Cmd->Treshold_H3       =  0.12;
  Cmd->Treshold_H4       =  0.06;
  Cmd->Treshold_E1       = -240.0;
  Cmd->Treshold_E2       = -310.0;

  Cmd->MinResolution     =  0.1;
  Cmd->MaxResolution     =  100.0;

  Cmd->MinLength         = 0;
  Cmd->MaxLength         = MAX_RES;

  Cmd->NPixel            = 0;
  Cmd->NActive           = 0;
  Cmd->NProcessed        = 0;

  strcpy(Cmd->MapFileHelix,""); 
  strcpy(Cmd->MapFileSheet,""); 
  strcpy(Cmd->OutFile,"");
  strcpy(Cmd->Active,"");
  strcpy(Cmd->Processed,"");
  strcpy(Cmd->Cond,"");
}

%}

typedef struct {
  CHAIN **Chain;
  HBOND **HBond;
  COMMAND *Cmd;
  int      NChain;
  int      NHBond;
  int      ValidChain;
} STRIDE ;

%extend STRIDE {
    STRIDE() {
      STRIDE *self = (STRIDE *)ckalloc(sizeof(STRIDE));
      self->Chain = (CHAIN  **)ckalloc(MAX_CHAIN*sizeof(CHAIN *));
      self->HBond = (HBOND  **)ckalloc(MAXHYDRBOND*sizeof(HBOND *));
      self->Cmd = (COMMAND *)ckalloc(sizeof(COMMAND));
      self->NChain = 0;
      self->NHBond = 0;
      self->ValidChain = 0;
      DefaultCmd(self->Cmd);
      return self;
    }

    ~STRIDE() {
      int i;
      for( i=0; i<self->NChain; i++ ) free(self->Chain[i]);
      for( i=0; i<self->NHBond; i++ ) free(self->HBond[i]);
      free(self->Cmd);
    }

    CHAIN *getChain(int i) {
      if (i < self->NChain) return self->Chain[i];
      else return NULL;
    }

    HBOND *getHBond(int i) {
      if (i < self->NHBond) return self->HBond[i];
      else return NULL;
    }

    int run(int report =0) {
      int Cn;
      float **PhiPsiMapHelix, **PhiPsiMapSheet;
      register int i;

      for( Cn=0; Cn<self->NChain; Cn++ )
	self->ValidChain += CheckChain(self->Chain[Cn],self->Cmd);

      if( !self->ValidChain ) {
	fprintf(stderr, "No valid chain in %s\n",self->Chain[0]->File);
	return -2;
      }

      if( self->Cmd->BrookhavenAsn )
	GetPdbAsn(self->Chain,self->NChain);

      if( self->Cmd->DsspAsn )
	GetDsspAsn(self->Chain,self->NChain,self->Cmd);

      BackboneAngles(self->Chain,self->NChain);

      if( self->Cmd->OutSeq )
	OutSeq(self->Chain,self->NChain,self->Cmd);

      if( !strlen(self->Cmd->MapFileHelix) )
	PhiPsiMapHelix = DefaultHelixMap(self->Cmd);
      else
	ReadPhiPsiMap(self->Cmd->MapFileHelix,&PhiPsiMapHelix,self->Cmd);

      if( !strlen(self->Cmd->MapFileSheet) )
	PhiPsiMapSheet = DefaultSheetMap(self->Cmd);
      else
	ReadPhiPsiMap(self->Cmd->MapFileSheet,&PhiPsiMapSheet,self->Cmd);
  
      for( Cn=0; Cn<self->NChain; Cn++ )
	PlaceHydrogens(self->Chain[Cn]);
  
      if( (self->NHBond = FindHydrogenBonds(self->Chain,Cn,self->HBond,self->Cmd)) == 0 ) {
	fprintf(stderr, "No hydrogen bonds found in %s\n",self->Cmd->InputFile);
	return -3;
      }
  
      NoDoubleHBond(self->HBond,self->NHBond);
  
      DiscrPhiPsi(self->Chain,self->NChain,self->Cmd);
  
      if(self->Cmd->ExposedArea)
	Area(self->Chain,self->NChain,self->Cmd);

      for( Cn=0; Cn<self->NChain; Cn++ ) {

	if( self->Chain[Cn]->Valid ) {
    
	  Helix(self->Chain,Cn,self->HBond,self->Cmd,PhiPsiMapHelix);
      
	  for( i=0; i<self->NChain; i++ ) 
	    if( self->Chain[i]->Valid )
	      Sheet(self->Chain,Cn,i,self->HBond,self->Cmd,PhiPsiMapSheet);    
      
	  BetaTurn(self->Chain,Cn);
	  GammaTurn(self->Chain,Cn,self->HBond);
      
	}
      }
    
      if (report)
	Report(self->Chain,self->NChain,self->HBond,self->Cmd);

      if( self->Cmd->MolScript )
	MolScript(self->Chain,self->NChain,self->Cmd);

      return(SUCCESS);
    }

    int ReadPDBFile(char *filename) {
      if (filename!=NULL) {
	strcpy(self->Cmd->InputFile, filename);
	if( !ReadPDBFile(self->Chain,&self->NChain,self->Cmd) || 
	    !self->NChain ) {
	  fprintf(stderr, "Error reading PDB file %s\n",self->Cmd->InputFile);
	  return -1;
	}
      }
      return(SUCCESS);
    }

    /*
      derived from rdpdb to get pdb records from a list of strings rather than
      a  file
    */
    int getPDBRecords(char **records, int nbr) {
      CHAIN **Chain=self->Chain;
      int *Cn = &self->NChain;
      COMMAND *Cmd = self->Cmd;

      int ChainCnt, InfoCnt, i, recCount;
      enum METHOD Method = XRay;
      BOOLEAN First_ATOM, Published=YES, DsspAssigned=NO;
      float Resolution = 0.0;
      BUFFER Buffer;
      char *Info[MAX_INFO], PdbIdent[5];
      RESIDUE *r;
      CHAIN *c;

      *Cn= 0;
      InfoCnt = 0;
      strcpy(PdbIdent,"~~~~");

      /*    if( !(pdb = fopen(Cmd->InputFile,"r")) ) */
      /*      return(FAILURE);  */
      if (nbr<1) return(FAILURE); 

      First_ATOM = YES;
  
      for ( recCount=0; recCount < nbr; recCount++ ) {
	strcpy(Buffer, records[recCount]);
	if(!strncmp(Buffer,"HEADER",6)) {
	  Info[InfoCnt] = (char *)ckalloc(BUFSZ*sizeof(char));
	  strcpy(Info[InfoCnt],"HDR  ");
	  strcat(Info[InfoCnt++],Buffer+10);
	  strncpy(PdbIdent,Buffer+62,4);
	  PdbIdent[4] = '\0';
	}
	else
	  if(!strncmp(Buffer,"AUTHOR",6)) {
	    Info[InfoCnt] = (char *)ckalloc(BUFSZ*sizeof(char));
	    strcpy(Info[InfoCnt],"AUT  ");
	    strcat(Info[InfoCnt++],Buffer+10);
	  }
	  else
	    if(!strncmp(Buffer,"SOURCE",6)) {
	      Info[InfoCnt] = (char *)ckalloc(BUFSZ*sizeof(char));
	      strcpy(Info[InfoCnt],"SRC  ");
	      strcat(Info[InfoCnt++],Buffer+10);
	    }
	    else
	      if(!strncmp(Buffer,"COMPND",6)) {
		if( !Process_COMPND(Buffer,&Method) ) 
		  return(FAILURE);
		else {
		  Info[InfoCnt] = (char *)ckalloc(BUFSZ*sizeof(char));
		  strcpy(Info[InfoCnt],"CMP  ");
		  strcat(Info[InfoCnt++],Buffer+10);
		}
	      }
	      else if(!strncmp(Buffer,"JRNL",4) && !Process_JRNL(Buffer,&Published)) 
		return(FAILURE);
	      else if(!strncmp(Buffer,"REMARK",6) && !Process_REMARK(Buffer,&Method,&Resolution,
								       &DsspAssigned)) 
		return(FAILURE);
	      else if(!strncmp(Buffer,"EXPDTA",6) && !Process_EXPDTA(Buffer,&Method)) 
		return(FAILURE);
	      else if(!strncmp(Buffer,"MODEL",5) && !Process_MODEL(&Method)) 
		return(FAILURE);
	      else if(!strncmp(Buffer,"ENDMDL",6)) {
		Process_ENDMDL(Buffer,Chain,Cn);
		break;
	      }
	      else if(!strncmp(Buffer,"HELIX",5) && !Process_HELIX(Buffer,Chain,Cn,Cmd)) 
		return(FAILURE);
	      else if(!strncmp(Buffer,"SHEET",5) && !Process_SHEET(Buffer,Chain,Cn,Cmd)) 
		return(FAILURE);
	      else if(!strncmp(Buffer,"TURN",4) && !Process_TURN(Buffer,Chain,Cn,Cmd)) 
		return(FAILURE);
	      else if(!strncmp(Buffer,"SSBOND",6) && !Process_SSBOND(Buffer,Chain,Cn,Cmd)) 
		return(FAILURE);
	      else if(!strncmp(Buffer,"ATOM",4) && !Process_ATOM(Buffer,Chain,Cn,&First_ATOM,Cmd)) 
		return(FAILURE);
      }
	
      for( ChainCnt=0; ChainCnt< *Cn; ChainCnt++ ) {
	c = Chain[ChainCnt];
	if( c->NRes != 0  && !FindAtom(c,c->NRes,"CA",&i) )
	  c->NRes--;
	strcpy(c->File,Cmd->InputFile);
	  
	strcpy(c->PdbIdent,PdbIdent);
	if( c->NRes != 0 )  c->NRes++;
	if( c->NSheet != -1 ) c->NSheet++;
	c->Resolution = Resolution;
	c->Method = Method;
	c->Published = Published;
	c->DsspAssigned = DsspAssigned;
	c->NInfo = InfoCnt;
	for(i=0; i<InfoCnt; i++) {
	  c->Info[i] = (char *)ckalloc(BUFSZ*sizeof(char));
	  strcpy(c->Info[i],Info[i]);
	  c->Info[i][71] = '\0';
	}
	for( i=0; i<c->NRes; i++ ) {
	  r = c->Rsd[i];
	  r->Inv =  (INVOLVED *)ckalloc(sizeof(INVOLVED));
	  r->Prop = (PROPERTY *)ckalloc(sizeof(PROPERTY));
	  r->Inv->NBondDnr = 0;
	  r->Inv->NBondAcc = 0;
	  r->Inv->InterchainHBonds = NO;
	  r->Prop->Asn     = 'C';
	  r->Prop->PdbAsn  = 'C';
	  r->Prop->DsspAsn = 'C';
	  r->Prop->Solv    = 0.0;
	  r->Prop->Phi     = 360.0;
	  r->Prop->Psi     = 360.0;
	}
      }
	
      for(i=0; i<InfoCnt; i++)
	free(Info[i]);
  
      return(SUCCESS);
    }
}









