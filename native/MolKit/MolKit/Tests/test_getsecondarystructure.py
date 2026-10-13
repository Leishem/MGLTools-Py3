#############################################################################
#
# Author: Sophie I. COON, Michel F. SANNER
#
# Copyright: M. Sanner TSRI 2000
#
#############################################################################
#
# $Header: /opt/cvs/python/packages/share1.5/MolKit/Tests/test_getsecondarystructure.py,v 1.3 2004/01/27 00:50:38 sophiec Exp $
#
# $Id: test_getsecondarystructure.py,v 1.3 2004/01/27 00:50:38 sophiec Exp $
#

import sys
from mglutil.regression import testplus
"""
This module implements a set of function to test the functionalities
implemented in the protein module.
"""

def test_simplestride():
    from MolKit import Read
    mol = Read("./Data/1crn.pdb")[0]
    mol.secondaryStructureFromStride()
    assert hasattr(mol.chains[0], 'secondarystructureset') and \
           len(mol.chains[0].secondarystructureset)==11

def test_simplefile():
    from MolKit import Read
    mol = Read("./Data/1crn.pdb")[0]
    mol.secondaryStructureFromFile()
    assert hasattr(mol.chains[0], 'secondarystructureset') and \
           len(mol.chains[0].secondarystructureset)==10

def test_getsecondarystructureChain():
    from MolKit import Read
    mol = Read("./Data/fxnohtatm.pdb")[0]
    from MolKit.getsecondarystructure import GetSecondaryStructureFromStride
    ssb = GetSecondaryStructureFromStride(mol)
    mol.secondaryStructure(ssb)
    

harness = testplus.TestHarness( __name__,
                                #funs = testplus.testcollect( globals()),
                                funs = [test_getsecondarystructureChain,]
                                )

if __name__ == '__main__':
    print harness
    sys.exit( len( harness))

