try:  # Tk 8.4 pack/grid leniency for the MGLTools widgets (see mglcompat.tkcompat)
    import mglcompat.tkcompat  # noqa: F401
except ImportError:  # Python built without tkinter: non-GUI use still works
    pass

CRITICAL_DEPENDENCIES = []
NONCRITICAL_DEPENDENCIES = ['numpy', 'MolKit', 'Pmw', 'bhtree', '_subprocess', 'Pmv', 'ViewerFramework', 'PIL', 'FlexTree', 'idlelib', 'DejaVu','distutils', 'Support']
#__revision__= 'Jun_07_13'
__revision__= 'Sep_17_14'
