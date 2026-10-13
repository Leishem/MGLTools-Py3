#!/usr/bin/env bash
# Re-place the mglcompat.pyclasses imports added for the slice/cmp mixins.
cd "$(dirname "$0")/../src"
. ~/mglenv/bin/activate
A=../tools/add_import.py
python $A "from mglcompat.pyclasses import SliceMixin" MolKit/listSet.py MolKit/PROSS.py
python $A "from mglcompat.pyclasses import CmpMixin, cmp" MolKit/tree.py mglutil/math/VectorModule.py mglutil/math/TensorModule.py Pmv/Grid.py
python $A "from mglcompat.pyclasses import CmpMixin" Pmv/setangleCommands.py
python $A "from mglcompat.pyclasses import cmp" mglutil/gui/BasicWidgets/Tk/multiListbox.py MolKit/getsecondarystructure.py Pmv/genparserCommands.py Vision/VPE.py mglutil/regression/testplus.py
