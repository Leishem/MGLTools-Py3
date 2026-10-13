
// $Id: regprop2.h,v 1.1.1.1 2004/06/16 19:46:53 annao Exp $

#ifndef REG_PROP2_H
#define REG_PROP2_H

#include "range.h"
#include "seedcells.h"
#include "conplot.h"
#include "data.h"

class Datavol;
class Dataslc;

class regProp2 {
   public:
      regProp2(Data &d, SeedCells &s, Conplot &p) : data(d), seeds(s), plot(p) {}
      ~regProp2() {}

      void compSeeds(void);

   private:

      Data &data;
      SeedCells &seeds;
      Conplot   &plot;
};

#endif
