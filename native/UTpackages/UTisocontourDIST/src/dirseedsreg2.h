
// $Id: dirseedsreg2.h,v 1.1.1.1 2004/06/16 19:46:53 annao Exp $

#ifndef DIR_SEEDS_REG2_H
#define DIR_SEEDS_REG2_H

#include "range.h"
#include "seedcells.h"
#include "conplot.h"
#include "data.h"

class Datareg2;

class dirSeedsReg2 {
   public:
      dirSeedsReg2(Data &d, SeedCells &s, Conplot &p) : data(d), seeds(s), plot(p) {}
      ~dirSeedsReg2() {}

      void compSeeds(void);

   private:

      void dirSweep(Datareg2 &reg);

      Data &data;
      SeedCells &seeds;
      Conplot   &plot;
};

#endif
