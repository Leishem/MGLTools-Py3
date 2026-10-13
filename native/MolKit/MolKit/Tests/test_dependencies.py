#
#################################################################
#       Author: Sowjanya Karnati
#################################################################
#
#Purpose:To update dependencies list
#
# $Id: test_dependencies.py,v 1.4 2006/09/11 17:35:35 sowjanya Exp $
from mglutil.TestUtil.Tests.dependenciestest import DependencyTester
import unittest,sys
d = DependencyTester()
result_expected =[]
class test_dep(unittest.TestCase):
    
    def test_dep_1(self):
      if sys.platform != 'win32':
        result = d.rundeptester('MolKit')    
        if result !=[]:
            print "\nThe Following Packages are not present in CRITICAL or NONCRITICAL DEPENDENCIES of MolKit :\n  %s" %result
            self.assertEqual(result,result_expected) 
        else:
            self.assertEqual(result,result_expected)
    

if __name__ == '__main__':
    unittest.main()


