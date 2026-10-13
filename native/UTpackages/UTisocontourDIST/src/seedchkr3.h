
// $Id: seedchkr3.h,v 1.1.1.1 2004/06/16 19:46:53 annao Exp $

#ifndef SEED_CHKR3_H
#define SEED_CHKR3_H

#include "range.h"
#include "seedcells.h"
#include "conplot.h"
#include "data.h"

class Datavol;
class Dataslc;

class seedChkr3 {
   public:
      seedChkr3(Data &d, SeedCells &s, Conplot &p) : data(d), seeds(s), plot(p) {}
      ~seedChkr3() {}

      void compSeeds(void);

   private:

      Data &data;
      SeedCells &seeds;
      Conplot   &plot;
};

#endif
