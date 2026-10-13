
// $Id: seedall.h,v 1.1.1.1 2004/06/16 19:46:53 annao Exp $

#ifndef SEED_ALL_H
#define SEED_ALL_H

#include "range.h"
#include "seedcells.h"
#include "conplot.h"
#include "data.h"

class seedAll {
   public:
      seedAll(Data &d, SeedCells &s, Conplot &p) : data(d), seeds(s), plot(p) {}
      ~seedAll() {}

      void compSeeds(void);

   private:

      Data &data;
      SeedCells &seeds;
      Conplot   &plot;
};

#endif
