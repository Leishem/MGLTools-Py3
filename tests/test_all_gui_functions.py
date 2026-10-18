import tkinter
import sys
import os
import glob
import traceback

root = tkinter.Tk()
root.withdraw()

from Pmv.moleculeViewer import MoleculeViewer

print("="*60)
print("STARTING SYSTEMATIC ADT GUI FUNCTION AUDIT")
print("="*60)

mv = MoleculeViewer(master=root, gui=True, withShell=False)
mv.browseCommands('autotors41Commands', commands=None, package='AutoDockTools')
mv.browseCommands('autoflex41Commands', commands=None, package='AutoDockTools')
mv.browseCommands('autogpf41Commands', commands=None, package='AutoDockTools')
mv.browseCommands('autodpf41Commands', commands=None, package='AutoDockTools')
mv.browseCommands('autostart41Commands', commands=None, package='AutoDockTools')
mv.browseCommands('autoanalyze41Commands', commands=None, package='AutoDockTools')
from AutoDockTools import setADTmode
setADTmode('AD4.2', mv)
mv.browseCommands('selectionCommands', package='Pmv')

# Prevent modal blocking during automated testing
mv.warningMsg = lambda *args, **kwargs: print(f"  [ADT WARNING]: {' '.join(str(a) for a in args)}")
from tkinter.simpledialog import SimpleDialog
SimpleDialog.go = lambda self: getattr(self, 'default', 0)
import tkinter.messagebox
tkinter.messagebox.showinfo = lambda *a, **k: 'ok'
tkinter.messagebox.showwarning = lambda *a, **k: 'ok'
tkinter.messagebox.showerror = lambda *a, **k: 'ok'
tkinter.messagebox.askyesno = lambda *a, **k: True
tkinter.messagebox.askokcancel = lambda *a, **k: True

results = {}

def test_func(name, func, *args, **kwargs):
    print(f"\n[TESTING] {name} ...", end=" ", flush=True)
    try:
        res = func(*args, **kwargs)
        results[name] = ("PASS", None)
        print("PASS", flush=True)
        return res
    except Exception as e:
        results[name] = ("FAIL", str(e))
        print("FAIL:", e, flush=True)
        traceback.print_exc()
        return None

# Cross-platform path resolution for test files
TRIAL_DIR = os.environ.get('MGL_TRIAL_DIR') or (r'E:\Otober_7_Docking\DAY_Trial' if os.name == 'nt' else '/mnt/e/Otober_7_Docking/DAY_Trial')

p_ach = os.path.join(TRIAL_DIR, 'Processed_AChE.pdbqt')
p_lig = os.path.join(TRIAL_DIR, 'Donepezil.pdbqt')
p_dlg = os.path.join(TRIAL_DIR, 'dock.dlg')
p_pdb = os.path.join(TRIAL_DIR, '4EY7.pdb')

audit_ligand = os.path.join(TRIAL_DIR, 'audit_ligand.pdbqt')
audit_flex = os.path.join(TRIAL_DIR, 'audit_flex.pdbqt')
audit_rigid = os.path.join(TRIAL_DIR, 'audit_rigid.pdbqt')
audit_grid = os.path.join(TRIAL_DIR, 'audit_grid.gpf')
audit_dock = os.path.join(TRIAL_DIR, 'audit_dock.dpf')

# ==========================================
# PHASE 1: File & Preferences
# ==========================================
print("\n--- PHASE 1: File & Preferences ---", flush=True)
test_func("File: readMolecule (4EY7.pdb)", mv.readMolecule, p_pdb)
test_func("File: readPDBQT (Processed_AChE.pdbqt)", mv.readPDBQT, p_ach)
test_func("File: readPDBQT (Donepezil.pdbqt)", mv.readPDBQT, p_lig)
if hasattr(mv, 'setUserPreference'):
    test_func("File: Preferences -> Set guiCallback", mv.setUserPreference.guiCallback)
    if hasattr(mv.setUserPreference, 'dismissForm'):
        mv.setUserPreference.dismissForm()

# ==========================================
# PHASE 2: Selection & Editing
# ==========================================
print("\n--- PHASE 2: Selection & Editing ---")
test_func("Select: selectFromString (:TRP86)", mv.selectFromString, ':TRP86')
test_func("Select: clearSelection", mv.clearSelection)
test_func("Select: invertSelection", mv.invertSelection)
test_func("Select: directSelect (:TRP286)", mv.directSelect, ':TRP286')
test_func("Edit: buildBondsByDistance (Processed_AChE)", mv.buildBondsByDistance, 'Processed_AChE')

# ==========================================
# PHASE 3: Display & Color
# ==========================================
print("\n--- PHASE 3: Display & Color ---")
test_func("Display: Lines", mv.displayLines, 'Donepezil')
test_func("Display: Sticks and Balls", mv.displaySticksAndBalls, 'Donepezil')
test_func("Display: CPK Spheres", mv.displayCPK, 'Donepezil')
test_func("Display: Backbone Trace", mv.displayBackboneTrace, 'Processed_AChE')
test_func("Color: By Atom Type", mv.colorByAtomType, 'Donepezil')
test_func("Color: By Residue Type", mv.colorByResidueType, 'Processed_AChE')
test_func("Color: By Chains", mv.colorByChains, 'Processed_AChE')
test_func("Color: By Molecules", mv.colorByMolecules, 'Processed_AChE')

# ==========================================
# PHASE 4: Ligand Menu (AutoTools41Bar -> Ligand)
# ==========================================
print("\n--- PHASE 4: Ligand Menu ---")
test_func("Ligand: Choose Ligand", mv.AD41tors_chooseLigand, 'Donepezil')
test_func("Ligand: Auto Root", mv.AD41tors_autoRoot)
test_func("Ligand: Automatic Formatting", mv.AD41tors_automaticLigandFormatting, 'Donepezil')
test_func("Ligand: Write Formatted PDBQT", mv.AD41tors_writeFormattedPDBQT, audit_ligand)

# ==========================================
# PHASE 5: Flexible Residues Menu (AutoTools41Bar -> Flexible Residues)
# ==========================================
print("\n--- PHASE 5: Flexible Residues Menu ---")
test_func("Flex: Choose Macromolecule", mv.AD41flex_chooseMacro, 'Processed_AChE')

# Select flexible residues TRP86 and TRP286
mol = mv.getMolFromName('Processed_AChE')
if mol:
    res = mol.chains.residues.get(lambda x: x.name in ['TRP86', 'TRP286'])
    if res:
        mv.select(res)
test_func("Flex: Set Residues guiCallback", mv.AD41flex_setResidues.guiCallback)

# Form the flexible residue processor for TRP86 and TRP286
proc = getattr(mv.AD41flex_setResidues, 'residue_processor', None)
if proc and mol:
    res_list = [r for r in mol.chains[0].residues if r.name in ['TRP86', 'TRP286']]
    test_func("Flex: Process Residues (TRP86, TRP286)", proc.buildCol, res_list)
    test_func("Flex: Close Residues Dialog", proc.close_cb)

test_func("Flex: Write Flex File", mv.AD41flex_writeFlexFile, audit_flex)
test_func("Flex: Write Rigid File", mv.AD41flex_writeRigidFile, audit_rigid)

# ==========================================
# PHASE 6: Grid Menu (AutoTools41Bar -> Grid)
# ==========================================
print("\n--- PHASE 6: Grid Menu ---")
test_func("Grid: Choose Macromolecule", mv.AD41gpf_chooseMacromolecule, 'Processed_AChE', preserve_input_charges=True)
test_func("Grid: Set Map Types Directly", mv.AD41gpf_setMapTypes, 'A C OA N')
test_func("Grid: Set Grid Parameters Dialog (guiCallback)", mv.AD41gpf_setGrid.guiCallback)
if hasattr(mv.AD41gpf_setGrid, 'form') and mv.AD41gpf_setGrid.form:
    mv.AD41gpf_setGrid.form.root.withdraw()
mv.gpo['gridcenter']['value'] = (-14.1, -43.8, 27.6)
mv.gpo['npts']['value'] = [40, 40, 40]
test_func("Grid: Write GPF", mv.AD41gpf_writeGPF, audit_grid)
test_func("Grid: Open GPF", mv.AD41gpf_readGPF, audit_grid)

# ==========================================
# PHASE 7: Docking Menu (AutoTools41Bar -> Docking)
# ==========================================
print("\n--- PHASE 7: Docking Menu ---")
test_func("Docking: Set Rigid Macromolecule", mv.AD41dpf_readMacromolecule, os.path.join(TRIAL_DIR, 'Processed_AChE_rigid.pdbqt'))
test_func("Docking: Set Flexible Residues", mv.AD41dpf_readFlexRes, os.path.join(TRIAL_DIR, 'Processed_AChE_flex.pdbqt'))
test_func("Docking: Choose Ligand", mv.AD41dpf_chooseFormattedLigand, 'Donepezil')
# Test GA search parms guiCallback
if hasattr(mv, 'AD41dpf_setGAparameters'):
    test_func("Docking: GA Parameters Dialog (guiCallback)", mv.AD41dpf_setGAparameters.guiCallback)
    if hasattr(mv.AD41dpf_setGAparameters, 'form') and mv.AD41dpf_setGAparameters.form:
        mv.AD41dpf_setGAparameters.form.root.withdraw()

if hasattr(mv, 'AD41dpf_setLSparameters'):
    test_func("Docking: LS Parameters Dialog (guiCallback)", mv.AD41dpf_setLSparameters.guiCallback)
    if hasattr(mv.AD41dpf_setLSparameters, 'form') and mv.AD41dpf_setLSparameters.form:
        mv.AD41dpf_setLSparameters.form.root.withdraw()

if hasattr(mv, 'AD41dpf_setDockingParameters'):
    test_func("Docking: Docking Parameters Dialog (guiCallback)", mv.AD41dpf_setDockingParameters.guiCallback)
    if hasattr(mv.AD41dpf_setDockingParameters, 'form') and mv.AD41dpf_setDockingParameters.form:
        mv.AD41dpf_setDockingParameters.form.root.withdraw()

test_func("Docking: Write Lamarckian GA (4.2)", mv.AD41dpf_writeGALS, audit_dock)
test_func("Docking: Open DPF", mv.AD41dpf_read, audit_dock)

# ==========================================
# PHASE 8: Run Menu
# ==========================================
print("\n--- PHASE 8: Run Menu ---")
if hasattr(mv, 'AD41start_autogrid'):
    test_func("Run: Run AutoGrid Dialog (guiCallback)", mv.AD41start_autogrid.guiCallback)
    if hasattr(mv.AD41start_autogrid, 'form') and mv.AD41start_autogrid.form:
        mv.AD41start_autogrid.form.root.withdraw()

if hasattr(mv, 'AD41start_autodock'):
    test_func("Run: Run AutoDock Dialog (guiCallback)", mv.AD41start_autodock.guiCallback)
    if hasattr(mv.AD41start_autodock, 'form') and mv.AD41start_autodock.form:
        mv.AD41start_autodock.form.root.withdraw()

if hasattr(mv, 'ADstart_manage'):
    test_func("Run: Process Manager Dialog (guiCallback)", mv.ADstart_manage.guiCallback)
    if hasattr(mv.ADstart_manage, 'top') and mv.ADstart_manage.top:
        mv.ADstart_manage.top.withdraw()

# ==========================================
# PHASE 9: Analyze Menu
# ==========================================
print("\n--- PHASE 9: Analyze Menu ---")
test_func("Analyze: Open DLG", mv.AD41analyze_readDLG, p_dlg, ask=0)
test_func("Analyze: Choose Macromolecule", mv.AD41analyze_chooseMacromolecule, 'Processed_AChE')
if hasattr(mv, 'AD41analyze_showStates'):
    test_func("Analyze: Show Conformations", mv.AD41analyze_showStates, 'Donepezil')
    if hasattr(mv.AD41analyze_showStates, 'form') and mv.AD41analyze_showStates.form:
        mv.AD41analyze_showStates.form.withdraw()
if hasattr(mv, 'AD41analyze_showClusteringStates'):
    test_func("Analyze: Show Clusters", mv.AD41analyze_showClusteringStates, 'Donepezil')
    if hasattr(mv.AD41analyze_showClusteringStates, 'form') and mv.AD41analyze_showClusteringStates.form:
        mv.AD41analyze_showClusteringStates.form.withdraw()

# Self-cleaning of test outputs
for f in [audit_ligand, audit_flex, audit_rigid, audit_grid, audit_dock]:
    try:
        if os.path.exists(f):
            os.remove(f)
    except Exception:
        pass

# Summary
print("\n" + "="*60)
print("AUDIT SUMMARY:")
print("="*60)
passed = sum(1 for status, err in results.values() if status == 'PASS')
failed = sum(1 for status, err in results.values() if status == 'FAIL')
print(f"Total Functions Tested: {len(results)}")
print(f"PASSED: {passed}")
print(f"FAILED: {failed}")

if failed > 0:
    print("\nFAILURES:")
    for name, (status, err) in results.items():
        if status == 'FAIL':
            print(f"  - {name}: {err}")
    sys.exit(1)
else:
    print("\nALL GUI FUNCTIONS PASSED WITH ZERO FAILURES!")
    sys.exit(0)
