## Automatically adapted for numpy.oldnumeric Jul 30, 2007 by 

import numpy.oldnumeric as Numeric
import unittest
from time import sleep

count = 0
display = 0
try:
    from DejaVu.IndexedPolygons import IndexedPolygons
    from DejaVu import Viewer
    print "Viewer imported"
    display = 1
except:
    print "could not import DejaVu, display = 0"
    display = 0



class TestQSlimLib(unittest.TestCase):

    def setUp(self):
        """Create a model and initialize slim. """
        global count
        count = count +1
        if count <2:
            from QSlimLib import qslimlib
            #smf = self.smf = qslimlib.MxSMFReader()
            #model = self.model = qslimlib.MxStdModel(8, 16)
            #smf.bind_colors(model, face = 0) #bind to vertex
            from cv_msms import verts, faces, colors
            # bind colors to vertex (bindtoface = False)
            qsmodel = self.qsmodel = qslimlib.QSlimModel(verts, faces, bindtoface=False,
                                                     colors = colors)
            
            #smf.createModel(verts, faces, colors = colors, model = model)
            nverts = qsmodel.get_vert_count()
            nfaces = qsmodel.get_face_count()
            print "verts in model = ", nverts, "faces in model = ", nfaces
            self.assertEqual(nverts, len(verts))
            self.assertEqual(nfaces, len(faces))
            #slim = self.slim = qslimlib.MxEdgeQSlim(model) #MxPropSlim(model)
            #qslimlib.slim_init(slim)
            self.newverts = Numeric.zeros((len(verts), 3)).astype("f")
            self.newfaces = Numeric.zeros((len(faces), 3)).astype("i")
            self.newcolors =  Numeric.zeros((len(verts), 3)).astype("f")
            self.nf_ori = len(faces)
            self.nf_last = 0
            if display:
                cv= IndexedPolygons("cv", vertices=verts, faces = faces,
                                    materials=colors,
                                    visible=1, inheritMaterial=0)
                vi = Viewer()
                vi.AddObject(cv)
                for i in range(3):
                    vi.Redraw()
                    vi.cameras[0].update()
                    sleep(1)

            self.writer = qslimlib.MxSMFWriter()
            self.writer.writeSMF("cv.smf", qsmodel.model)
            


    def test_operate(self):
        self.reduce_triangles()
        self.increase_triangles()

        
    def reduce_triangles(self):
        """ Reduce number of triangles. """
        from QSlimLib import qslimlib
        nf = self.nf_ori
        while nf >= 170:
            nf = nf-50
            print "reduce, target faces: ", nf 
            #qslimlib.slim_to_target(nf, self.model, self.slim)
            #validverts = self.slim.valid_verts
            #validfaces = self.slim.valid_faces
            self.qsmodel.slim_to_target(nf)
            validverts = self.qsmodel.num_valid_verts()
            validfaces = self.qsmodel.num_valid_faces()
            print "valid verts = ", validverts , "valid faces = ", validfaces
    
            self.qsmodel.outmodel(self.newverts, self.newfaces,
                              outcolors = self.newcolors)
            indmax = max(self.newfaces.ravel())
            d = nf*15/100
            print d
            assert abs(nf - validfaces) < d
        self.nf_last = indmax
        
        if display:
            cv= IndexedPolygons("cv1", vertices=self.newverts[:indmax+1],
                                faces = self.newfaces[:validfaces],
                                materials=self.newcolors[:indmax+1],
                                visible=1, inheritMaterial=0)
            vi = Viewer()
            vi.AddObject(cv)
            for i in range(3):
                vi.Redraw()
                vi.cameras[0].update()
                sleep(1)
        
        self.writer.writeSMF("cv_reduced.smf",self.qsmodel.model)
            
        
    def increase_triangles(self):
        """Go back to the original number of triangles."""
        from QSlimLib import qslimlib
        nf = self.nf_last+50
        while nf <= self.nf_ori:
            print "increase, target faces: ", nf 
            #qslimlib.slim_to_target(nf, self.model, self.slim)
            #validverts = self.slim.valid_verts
            #validfaces = self.slim.valid_faces
            self.qsmodel.slim_to_target(nf)
            validverts = self.qsmodel.num_valid_verts()
            validfaces = self.qsmodel.num_valid_faces()
            print "valid verts = ", validverts , "valid faces = ", validfaces
            self.qsmodel.outmodel(self.newverts, self.newfaces,
                                  outcolors = self.newcolors)
            indmax = max(self.newfaces.ravel())
            d = nf*15/100
            assert abs(nf-validfaces) < d
            nf = nf + 50
            
        if display:
            cv= IndexedPolygons("cv2", vertices=self.newverts[:indmax+1],
                                faces = self.newfaces[:validfaces],
                                materials=self.newcolors[:indmax+1],
                                visible=1, inheritMaterial=0, redo = 1)
            vi = Viewer()
            vi.AddObject(cv)
            for i in range(3):
                vi.Redraw()
                vi.cameras[0].update()
                sleep(1)

    def test_wrongargs(self):
        
        from QSlimLib import qslimlib
        #smf = qslimlib.MxSMFReader()
        #model = qslimlib.MxStdModel(8, 16)
        verts = []
        faces = []
        try:
            #smf.createModel(verts, faces)
            qsmodel = qslimlib.QSlimModel(verts, faces)
            raise RuntimeError('failed to catch Value error in wrapper')
        except ValueError:
            print 'Value Error caught succefully in wrapper'

            

if __name__ == '__main__':
    unittest.main(argv=([__name__,]) )
    
