from __future__ import division
## Automatically adapted for mglcompat.oldnumeric Jul 23, 2007 by 

from mglcompat.pydiv import old_div
import mglcompat.oldnumeric as N
import os, math; from mglcompat import pystring as string
from struct import unpack
from Volume.Grid3D import Grid3DF

class ReadAutoGrid:
    """Read a AutoGrid map file"""
    def read(self, filename, normalize):
        """ Read from AutoGrid map file"""   
        
        self.SPACING=1.0
        self.CENTER=(0.,0.,0.)
     
        self.header = {'title': 'AutoGrid from %s'%filename}
        
        # Pmv/Grid.py, Class Grid, function ReadAutoGridMap()        
        f = open(filename, 'r')
        try:
            GRID_PARAMETER_FILE = string.split(f.readline())[1]
        except:
            GRID_PARAMETER_FILE = ''
        GRID_DATA_FILE = string.split(f.readline())[1]
        MACROMOLECULE = string.split(f.readline())[1]
    
        # spacing
        SPACING = float(string.split(f.readline())[1])
    
        # number of points and center
        (nx,ny,nz) = string.split(f.readline())[1:4]
        NELEMENTS = (nx,ny,nz) = (int(nx)+1, int(ny)+1, int(nz)+1)
        (cx,cy,cz) = string.split(f.readline())[1:4]
        CENTER = ( float(cx),float(cy), float(cz))
        
    
        # read grid points
        points = [float(x) for x in f.readlines()]
       
        # data read as z,y,z, swapaxes to make the data x,y,z
        TMPGRIDS = N.swapaxes(N.reshape( points,(nz,ny,nx)), 0, 2)
        GRIDS = N.array(TMPGRIDS).astype('f')
        f.close()
        self.data = GRIDS
        #print "shape***:",self.data.shape
        #print "origin***:",CENTER
        
        origin = (CENTER[0]-(old_div(nx,2))*SPACING, CENTER[1]-(old_div(ny,2))*SPACING,
                  CENTER[2]-(old_div(nz,2))*SPACING)
        stepSize = (SPACING,SPACING,SPACING)
        
        #def __init__(self, data, origin, stepSize, header):
        grid = Grid3DF(self.data, origin, stepSize, self.header)
        
        #print "**", grid.dimensions
       
        return grid
