## Automatically adapted for numpy.oldnumeric Jul 30, 2007 by 

import os
from mglutil.regression import testplus
import numpy.oldnumeric as Numeric

def test_0010( ):
    print "##### test_0010: import amber ######"
    from sff import amber
    print "amber imported from: ", amber.__file__
    
def test_0015( ):
    print "##### test_0015:  create AmberParm instance from .prm file #####"
    from sff import amber
    oprm = amber.AmberParm( 'tripeptide.prm')

def test_0020( ):
    print "##### test_0020: create AmberParm instance from parameters dict #### "
    from sff import amber
    from tripepparmdict import parmDict
    oprm = amber.AmberParm( 'sampletest', parmDict)

def test_0025():
    print "##### test_0025: #####"
    from sff import amber
    oprm = amber.AmberParm( 'tripeptide.prm')
    print list(oprm.DihHAt1)

    
def test_0030( ):
    print "##### test_0030: amber minimization #####"
    # try an amber minimization
    from sff import amber
    from tripepparmdict import parmDict
    from operator import add
    oprm = amber.AmberParm( 'tripeptide.prm')
    from p1H_crd import coords

    lcoords = reduce( add, coords)
    coords = Numeric.array( lcoords).astype(amber.realType )

    ar = Numeric.zeros( oprm.Natom).astype(amber.intType)
    anchor = Numeric.zeros( 3*oprm.Natom).astype( amber.realType )

    f = None
    sff_opts = amber.prmlib.init_sff_options()
    amber.prmlib.mme_init(ar, ar, anchor, f, oprm._parmptr_, sff_opts)


    nbVar = Numeric.array( [3*oprm.Natom,]).astype(amber.intType )

    objFunc = Numeric.zeros( 1).astype(amber.realType )

    # return when sum of squares of gradient is less than dgrad
    drms = 0.1
    dgrad = Numeric.array([drms*3*oprm.Natom]).astype(amber.realType)

    # expected decrease in the function on the first iteration
    dfpred = Numeric.array( [10.0,]).astype( amber.realType )

    maxIter = Numeric.array([500,]).astype(amber.intType)

    energies = Numeric.zeros(20).astype(amber.realType )

    amber.prmlib.mm_options('ntpr', 10, sff_opts)
    amber.prmlib.mm_options('verbose', 1, sff_opts)
    #amber.prmlib.setccallback(amber.prmlib.sanityCb, 50, 0)

    i = amber.prmlib.conjgrad( coords, nbVar, objFunc, amber.prmlib.mme_fun,
                     dgrad, dfpred, maxIter, oprm._parmptr_, energies, sff_opts )
    print 'DONE RUNNING in main thread', i



def test_0035():
    print "##### test_0035: amber minimization in a different thread"
    # try an amber minimization in a different thread
    from sff import amber
    from thread import start_new_thread
    from tripepparmdict import parmDict
    from operator import add
    oprm = amber.AmberParm( 'tripeptide.prm')
    from p1H_crd import coords

    lcoords = reduce( add, coords)
    coords = Numeric.array( lcoords).astype(amber.realType )

    ar = Numeric.zeros( oprm.Natom).astype(amber.intType)
    anchor = Numeric.zeros( 3*oprm.Natom).astype( amber.realType )

    sff_opts = amber.prmlib.init_sff_options()

    f = None
    amber.prmlib.mme_init(ar, ar, anchor, f, oprm._parmptr_, sff_opts)

    nbVar = Numeric.array( [3*oprm.Natom,]).astype(amber.intType )

    # will contain the value of the objective function at the end
    objFunc = Numeric.zeros( 1).astype(amber.realType )

    # return when sum of squares of gradient is less than dgrad
    drms = 0.1
    dgrad = Numeric.array([drms*3*oprm.Natom]).astype(amber.realType)

    # expected decrease in the function on the first iteration
    dfpred = Numeric.array( [10.0,]).astype( amber.realType )
    
    maxIter = Numeric.array([500,]).astype(amber.intType)

    energies = Numeric.zeros(20).astype(amber.realType )

    amber.prmlib.mm_options('ntpr', 10, sff_opts)
    amber.prmlib.mm_options('verbose', 1, sff_opts)

    start_new_thread( amber.prmlib.conjgrad,
                      ( coords, nbVar, objFunc, amber.prmlib.mme_fun,
                        dgrad, dfpred, maxIter, oprm._parmptr_,
                        energies, sff_opts) )
    print "minimization thread started"

    import time
    time.sleep(2)

def test_0040( ):
    print "##### test_0040: create two instances of Amber94 class #####"
    # test Amber94 class
    try:
        from MolKit import Read
    except:
        print "could not import MolKit - unable to do test_0040"
        return
    mol = Read("p1H.pdb")[0]
    atoms = mol.allAtoms
    from sff.amber import Amber94
    amber1 = Amber94(atoms, prmfile="tripeptide.prm")
    amber2 = Amber94(atoms, prmfile="tripeptide.prm")
    amber1.setMinimizeOptions(ntpr =10, verbosemm = 1)
    amber2.setMinimizeOptions(ntpr = 20, cut = 3.0, verbosemm =1)
    c1 = amber1.minimize()
    c2 = amber2.minimize()
    print "result_code: ", c1, c2
    
        
harness = testplus.TestHarness( __name__,
                                funs = testplus.testcollect( globals()),)
if __name__ == '__main__':
    print harness
    sys.exit( len( harness))                              

