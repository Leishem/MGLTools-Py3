
// $Id: regprop.h,v 1.1.1.1 2004/06/16 19:46:53 annao Exp $

#ifndef REG_PROP_H
#define REG_PROP_H

#include "range.h"
#include "seedcells.h"
#include "conplot.h"
#include "data.h"

class Datavol;
class Dataslc;

class regProp {
   public:
      regProp(Data &d, SeedCells &s, Conplot &p) : data(d), seeds(s), plot(p) {}
      ~regProp() {}

      void compSeeds(void);

   private:

      Data &data;
      SeedCells &seeds;
      Conplot   &plot;
};

#endif
