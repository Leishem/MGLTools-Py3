import sys
from mglutil.regression import testplus

def loadExtension():
    import stride


def test_stride_run():
    import stride
    print "stride importrd from: ", stride.__file__
    s = stride.STRIDE()

    # read a PDB file
    f = open('1crn.pdb')
    pdbRec = f.readlines()
    f.close()

    s.getPDBRecords(pdbRec, len(pdbRec))
    #s.run(report = 0)
    s.run()
    for i in range(s.NChain):
        print getAsn(s.getChain(i)) 

def getAsn(chain):

    asn =[]
    for j in range(chain.NRes):
        res = chain.getResidue(j)
        asn.append( (res.ResType+res.PDB_ResNumb, res.Prop.Asn) )
    return asn



harness = testplus.TestHarness( __name__,
                                connect = loadExtension,
                                funs = testplus.testcollect( globals()),
                                )

if __name__ == '__main__':
    print harness
    sys.exit( len( harness))
