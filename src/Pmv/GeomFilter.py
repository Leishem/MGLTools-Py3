class GeomFilter:

    def __init__(self, pmv):
        self.pmv = pmv
        
    def filter(self, viewer):
        geoms = [ ('All', [viewer.rootObject]) ]

        for mol in self.pmv.Mols:
            gc = mol.geomContainer
            geoms.append( ("  "+mol.name, [gc.masterGeom])) # put the molecule

            # build a dict of all geoms that do display something
            gd = {}
            for gname, g in list(gc.geoms.items()):
                if len(g.getVertices())>0:
                    gd[gname] = g

            # lines
            if 'bonded' in gd:
                geoms.append( ("    "+'lines', [gc.geoms['lines']]))
                #del gd['lines']
                #del gd['bonded']
                #del gd['nobnds']
                #del gd['bondorder']

            # surface
            for gname, x in list(gc.msms.items()):
                if gname in gd:
                    geoms.append( ("    "+gname, [gd[gname]]))
                    #del gd[gname]

            # cpk
            if 'cpk' in gd:
                geoms.append( ("    "+'cpk', [gd['cpk']]))
                #del gd['cpk']
            
            # sticks and balls
            if 'sticks' in gd and 'balls' in gd:
                g = gd['sticks']
                if len(g.getFaces()):
                    geoms.append( ("    "+'ball & stick',
                                   [gd['sticks'], gd['balls']]))
                #del gd['sticks']
                #del gd['balls']

            # CA trace
            if 'CAsticks' in gd and 'CAballs' in gd:                
                geoms.append( ("    "+'CA trace',
                               [gd['CAsticks'], gd['CAballs']]))
                #del gd['CAsticks']
                #del gd['CAballs']

            # secondary structure
            if 'secondarystructure' in gc.geoms:
                # check if any of it is displayed
                # only add it if there is at least 1 triangle
                found = False
                for g in gc.geoms['secondarystructure'].children:
                    if len(g.faceSet):
                        found = True
                        break
                if found:
                    geoms.append( ("    secondary struct",
                                   [gc.geoms['secondarystructure']]))

            # labels
            if 'AtomLabels' in gd:
                geoms.append( ("    Atom labels", [gd['AtomLabels']]))
                #del gd['AtomLabels']
            
            if 'ResidueLabels' in gd:
                geoms.append( ("    Residue labels", [gd['ResidueLabels']]))
                #del gd['ResidueLabels']
            
            if 'ChainLabels' in gd:
                geoms.append( ("    Chain labels", [gd['ChainLabels']]))
                #del gd['ChainLabels']
            
            if 'ProteinLabels' in gd:
                geoms.append( ("    Protein labels", [gd['ProteinLabels']]))
                #del gd['ProteinLabels']

            # coarse molecular surface
            if 'CoarseMolSurface' in gd:
                geoms.append( ("    Coarse surface", [gd['CoarseMolSurface']]))
                #del gd['CoarseMolSurface']

        return geoms
