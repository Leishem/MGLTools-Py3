from __future__ import division
## Automatically adapted for mglcompat.oldnumeric Jul 23, 2007 by 

from mglcompat.pydiv import old_div
import mglcompat.oldnumeric as Numeric
N=Numeric
import types

def crossProduct (A, B, normal=True):
    """     Return cross product of two vectors A and B
normal: return normalized vector
"""
    res=[ A[1]*B[2] - A[2]*B[1],
          A[2]*B[0] - A[0]*B[2],
          A[0]*B[1] - A[1]*B[0] ]
    if normal:
        return norm(res)
    else:
        return res

def norm (A):
    """     Return normalized vector A.
"""
    if type(A) == list:
        A=Numeric.array(A,'f')
        res= old_div(A,Numeric.sqrt(Numeric.dot(A,A)))
        return res.tolist()    
    elif type(A)==Numeric.ArrayType:    
        return old_div(A,Numeric.sqrt(Numeric.dot(A,A)))    
    else:
        print("Need a list or Numeric array")
        return None

def getCenter(coords):
    """ get center of all the coords """
    coords=N.array(coords, 'f')    
    return (old_div(N.sum(coords),len(coords))).tolist()
