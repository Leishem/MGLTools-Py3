//------------------------------------------------------------------------
//
// data.C - class for scalar data
//
// Copyright (c) 1997 Dan Schikore
//------------------------------------------------------------------------

// $Id: data.cpp,v 1.3 2008/09/19 22:04:17 annao Exp $

#include <stdio.h>
#if ! defined (__APPLE__)
#include <malloc.h>
#else
#include <stdlib.h>
#endif
#include <string.h>
#include "data.h"

int Data::funtopol1;
int Data::funtopol2;
