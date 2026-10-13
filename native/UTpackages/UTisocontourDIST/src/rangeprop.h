
// $Id: rangeprop.h,v 1.1.1.1 2004/06/16 19:46:53 annao Exp $

#ifndef RANGE_PROP_H
#define RANGE_PROP_H

#include "range.h"
#include "squeue.h"
#include "seedcells.h"
#include "conplot.h"
#include "data.h"

class RangePropRec {
   public:
      int operator <(RangePropRec&r2)  { return(this->cellid < r2.cellid); }
      int operator ==(RangePropRec&r2) { return(this->cellid == r2.cellid); }

      int cellid;
      Range resp;
      Range comp;
};

class rangeProp {
   public:
      rangeProp(Data &d, SeedCells &s, Conplot &p) : data(d), seeds(s), plot(p) {}
      ~rangeProp() {}

      void compSeeds(void);

   private:
      SQueue<RangePropRec> queue;
      Data &data;
      SeedCells &seeds;
      Conplot   &plot;
};

#endif
