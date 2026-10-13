//------------------------------------------------------------------------
//
// cellQueue.C - queue of cell identifiers.  The circular queue dyanmically
//               resizes itself when full.  Elements in the queue are of
//               indicies of i,j,k (specialized for 3d structured grids)
//
// Copyright (c) 1997 Dan Schikore
//------------------------------------------------------------------------

// $Id: cellqueue.cpp,v 1.3 2008/09/19 22:04:17 annao Exp $

#include <stdio.h>
#include <math.h>
#include <memory.h>
#if ! defined (__APPLE__)
#include <malloc.h>
#else
#include <stdlib.h>
#endif
#include <string.h>
#include "cellqueue.h"
