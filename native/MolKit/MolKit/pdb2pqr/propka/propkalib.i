%module propkalib
%feature("python:cdefaultargs");
extern void runPKA(int numatoms, char * pdb, char * outname);
