
// $Id: seeddirreg3.h,v 1.1.1.1 2004/06/16 19:46:53 annao Exp $

#ifndef SEED_DIR_REG3_H
#define SEED_DIR_REG3_H

#include "range.h"
#include "seedcells.h"
#include "conplot.h"
#include "data.h"

class Datareg3;

class seedDirReg3 {
   public:
      seedDirReg3(Data &d, SeedCells &s, Conplot &p) : data(d), seeds(s), plot(p) {}
      ~seedDirReg3() {}

      void compSeeds(void);

   private:

      void dirSweep(Datareg3 &reg);

      Data &data;
      SeedCells &seeds;
      Conplot   &plot;
};

#endif
