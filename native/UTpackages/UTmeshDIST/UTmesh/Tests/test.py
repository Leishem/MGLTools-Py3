## Automatically adapted for numpy.oldnumeric Jul 30, 2007 by 


from os import path
import numpy.oldnumeric as Numeric
from struct import unpack

file = "head65.rawiv"
mesher = None
display = 0

def readfile(filename):
        
        myfile = open(filename, 'rb')
        #the header
        header=myfile.read(68)
        # unpack header following rawiv format,big endian
        h = unpack('>6f5I6f',header)
        width=int(h[8])
        height=int(h[9])
        depth =int(h[10])
        nverts = int(h[6])
        ncells = int(h[7])
        origin = h[11:14]
        step = h[14:17]
        size = width*height*depth
        print "header: ", h
        print "nverts: ", nverts, "ncells: ", ncells
        # load the data
        #l = myfile.read(width*height*depth)
        l = myfile.read() # read the rest of the file
        myfile.close()
        nbytes = len(l)
        elsize = nbytes/size
        if elsize == 1:
            mode = 1
        elif elsize == 4:
            mode = 9
        else:
            print "Error: in ReadRawiv - unsupported data type, size= %d"%elsize
            return None
        
        size = 4
        unpackType = 'f'
        arraytype = Numeric.Float32
        frm = ">%i%s"%(width*height*depth, unpackType)
        data = Numeric.reshape(Numeric.array(unpack(frm, l), arraytype), (width,height,depth))

        return [data, width,height,depth, origin, step]

def output(mesher, meshtype):
    from UTpackages.UTmesh import lbiemesher
    nverts = mesher.getNumVerts()
    nfaces = mesher.getNumFaces()
    print "in output: nverts: ", nverts, " nfaces: ", nfaces
    vertarr = Numeric.zeros((nverts, 3), "f")
    if meshtype == lbiemesher.SINGLE or meshtype == lbiemesher.DOUBLE :
        facearr = Numeric.zeros((nfaces, 3), "i")
        mesher.outTriangle(vertarr, facearr)
    elif  meshtype == lbiemesher.TETRA or  meshtype == lbiemesher.TETRA2:
        facearr = Numeric.zeros((nfaces, 4), "i")
        mesher.outTetra(vertarr, facearr )
    elif meshtype == lbiemesher.HEXA:
        facearr = Numeric.zeros((nfaces, 8), "i")
        mesher.outHexa(vertarr, facearr )
    elif meshtype == lbiemesher.T_4_H :
        facearr = Numeric.zeros((nfaces, 4), "i")
        mesher.outQuad(vertarr, facearr )
    return vertarr, facearr

def setUpSuite():
    from UTpackages.UTmesh import lbiemesher
    global mesher
    if not mesher:
        mesher = lbiemesher.LBIE_Mesher()
        data, dx, dy, dz, origin, step = readfile(file)
        nverts = dx * dy * dz
        ncells = (dx-1) * (dy-1) * (dz-1)
	print "in setUpSuite: origin:", origin, "stepSize:", step
        mesher.inputData(data.ravel(), (dx, dy, dz), nverts, ncells, origin, step)

def test_01_SingleMesh():
    from UTpackages.UTmesh import lbiemesher
    meshtype = lbiemesher.SINGLE
    global mesher
    mesher.setMesh(meshtype)
    mesher.isovalueChange(0.5)
    mesher.errorChange(0.2)
    print "single mesh : isoval = 0.5, error tolerance=0.2 "
    vertarr, facearr = output(mesher, meshtype)
    if display:
        from DejaVu import Viewer
        from DejaVu.IndexedPolygons import IndexedPolygons
        vi = Viewer()
        obj = IndexedPolygons("pol", vertices = vertarr, faces = facearr)
        vi.AddObject(obj)
        vi.NormalizeCurrentObject()
        vi.stopAutoRedraw()
        vi.OneRedraw()

def test_02_HexaMesh():
    from UTpackages.UTmesh import lbiemesher
    meshtype = lbiemesher.HEXA
    mesher.setMesh(meshtype)
    mesher.isovalueChange(0.5)
    mesher.errorChange(0.2)
    print "Hexa mesh : isoval = 0.5, error tolerance=0.2 "
    vertarr, facearr = output(mesher, meshtype)
    if display:
        from DejaVu import Viewer
        from DejaVu.IndexedPolygons import IndexedPolygons
        vi = Viewer()
        obj = IndexedPolygons("pol", vertices = vertarr, faces = facearr)
        vi.AddObject(obj)
        vi.NormalizeCurrentObject()
        vi.stopAutoRedraw()
        vi.OneRedraw()

def test_03_TetraMesh():
    from UTpackages.UTmesh import lbiemesher
    meshtype = lbiemesher.TETRA
    mesher.setMesh(meshtype)
    mesher.isovalueChange(0.5)
    mesher.errorChange(0.2)
    print "Tetra mesh : isoval = 0.5, error tolerance=0.2 "
    vertarr, facearr = output(mesher, meshtype)
    if display:
        from DejaVu import Viewer
        from DejaVu.IndexedPolygons import IndexedPolygons
        vi = Viewer()
        obj = IndexedPolygons("pol", vertices = vertarr, faces = facearr)
        vi.AddObject(obj)
        vi.NormalizeCurrentObject()
        vi.stopAutoRedraw()
        vi.OneRedraw()

def test_04_T_4_HMesh():
    from UTpackages.UTmesh import lbiemesher
    meshtype = lbiemesher.T_4_H
    mesher.setMesh(meshtype)
    mesher.isovalueChange(0.5)
    mesher.errorChange(0.2)
    print "T_4_H mesh : isoval = 0.5, error tolerance=0.2 "
    vertarr, facearr = output(mesher, meshtype)
    if display:
        from DejaVu import Viewer
        from DejaVu.IndexedPolygons import IndexedPolygons
        vi = Viewer()
        obj = IndexedPolygons("pol", vertices = vertarr, faces = facearr)
        vi.AddObject(obj)
        vi.NormalizeCurrentObject()
        vi.stopAutoRedraw()
        vi.OneRedraw()

def test_05_DoubleMesh():
    from UTpackages.UTmesh import lbiemesher
    meshtype = lbiemesher.DOUBLE
    mesher.setMesh(meshtype)
    mesher.isovalueChange(0.5)
    mesher.isovalueChange_in(-0.1)
    mesher.errorChange(0.2)
    mesher.errorChange_in(-0.3)
    print "double mesh : isoval = 0.5, isoval_in = -0.1, error tolerance = 0.2, error_in = -0.3"
    vertarr, facearr = output(mesher, meshtype)
    if display:
        from DejaVu import Viewer
        from DejaVu.IndexedPolygons import IndexedPolygons
        vi = Viewer()
        obj = IndexedPolygons("pol", vertices = vertarr, faces = facearr)
        vi.AddObject(obj)
        vi.NormalizeCurrentObject()
        vi.stopAutoRedraw()
        vi.OneRedraw()

def test_06_Tetra2Mesh():
    from UTpackages.UTmesh import lbiemesher
    meshtype = lbiemesher.TETRA2
    mesher.setMesh(meshtype)
    mesher.isovalueChange(0.5)
    mesher.isovalueChange_in(-0.1)
    mesher.errorChange(0.2)
    mesher.errorChange_in(-0.3)
    print "tetra2 mesh : isoval = 0.5, isoval_in = -0.1, error tolerance= 0.2, error_in = -0.3"
    vertarr, facearr = output(mesher, meshtype)
    if display:
        from DejaVu import Viewer
        from DejaVu.IndexedPolygons import IndexedPolygons
        vi = Viewer()
        obj = IndexedPolygons("pol", vertices = vertarr, faces = facearr)
        vi.AddObject(obj)
        vi.NormalizeCurrentObject()
        vi.stopAutoRedraw()
        vi.OneRedraw()


