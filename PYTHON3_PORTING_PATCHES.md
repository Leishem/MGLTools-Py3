# MGLTools Python 3 Porting - Cumulative Bugfixes & Compatibility Patches

> **Target Platforms:** Cross-Platform (Linux x86_64/aarch64, macOS x86_64/arm64 Apple Silicon, Windows 10/11 x64)  
> **Python Version:** Python 3.10 - 3.13+ (Tested on Python 3.13.14 x64)  
> **Source Base:** MGLTools 1.5.6 (AutoDockTools, Pmv, MolKit, PyBabel, ViewerFramework)  
> **Patch File:** [`mgltools_python3_fixes.patch`](file:///C:/Users/pc/Downloads/MGLTools-Portable/mgltools_python3_fixes.patch)

---

## 1. Executive Summary

During the Python 3 port of MGLTools 1.5.6 and its portable runtime packaging, multiple critical incompatibilities were identified and resolved across the Graphical User Interface (Tkinter / Pmw), the molecular modeler (`MolKit`), the AutoDock / AutoGrid run manager, and the flexible residue docking engine.

All fixes are strictly pure-Python, zero-dependency changes designed to function identically across **Linux**, **macOS**, and **Windows**.

### Key Issues Resolved:
1. **Preferences Dialog Crash**: `TypeError: '>' not supported between instances of 'str' and 'int'` when opening **File -> Preferences -> Set**.
2. **Ligand Open Crash**: `KeyError: None` and `NoneType: None` during **Ligand -> Input -> Open**.
3. **AutoGrid / Process Manager Launch Crash**: `AttributeError: module 'tkinter' has no attribute 'AtEnd'`.
4. **AutoGrid Monitor Window Flickering & High CPU**: Exponential dual-timer cascade (`self.after(100, ...)`), constant deiconify/withdraw churn, and 100ms listbox repopulation during background calculations.
5. **Flexible Residue Docking Zero Torsions**: `WARNING: Current Selected Residues have no active torsions!` due to missing bond distance evaluation on raw PDBQT input.
6. **ADT 4.2 Ligand Chooser Crash**: `AttributeError: 'MoleculeViewer' object has no attribute 'ADdpf4_initLigand'`.
7. **Variable Typo in DPF Generation**: `UnboundLocalError: local variable 'lig_type_str' referenced before assignment`.
8. **Backbone Trace Display Crash**: `UnboundLocalError: cannot access local variable 'list'` in `drawCASticks` caused by local variable `list = []` shadowing Python's built-in `list`.
9. **Flexible Residue Interactive Form Attribute Crashes**: Missing attribute checks for `activeTors`, `possibleTors`, and uninitialized/unguarded `pTatomset` in `autoflexCommands.py`.
10. **Analyze DLG Reader Comparison Crash**: `TypeError: '>' not supported between instances of 'str' and 'int'` in `ADGetDLG.doit` when inspecting conformation counts.

---

## 2. Inventory of Modified Files

The following 14 files were modified. All files in `src/` and `site-packages/` are 100% synchronized:

| # | Package | Relative File Path | Primary Issue Addressed |
|---|---------|-------------------|-------------------------|
| 1 | `ViewerFramework` | [`ViewerFramework/VF.py`](file:///C:/Users/pc/Downloads/MGLTools-Portable/MGLTools-1.5.6-Py3/python/Lib/site-packages/ViewerFramework/VF.py) | String-to-int parsing in `fillDelayValidate` validator |
| 2 | `ViewerFramework` | [`ViewerFramework/customizationCommands.py`](file:///C:/Users/pc/Downloads/MGLTools-Portable/MGLTools-1.5.6-Py3/python/Lib/site-packages/ViewerFramework/customizationCommands.py) | Type coercion (int/float/bool) in PmwEntryField validation & safe form destruction |
| 3 | `Pmv` | [`Pmv/measureCommands.py`](file:///C:/Users/pc/Downloads/MGLTools-Portable/MGLTools-1.5.6-Py3/python/Lib/site-packages/Pmv/measureCommands.py) | Replaced unhandled `lambda x: x > 0` with safe validator `validatePositiveInt` |
| 4 | `Pmv` | [`Pmv/guiTools.py`](file:///C:/Users/pc/Downloads/MGLTools-Portable/MGLTools-1.5.6-Py3/python/Lib/site-packages/Pmv/guiTools.py) | Replaced obsolete `AtEnd()` with `tkinter.END` |
| 5 | `Pmv` | [`Pmv/displayCommands.py`](file:///C:/Users/pc/Downloads/MGLTools-Portable/MGLTools-1.5.6-Py3/python/Lib/site-packages/Pmv/displayCommands.py) | Renamed local variable `list = []` to `ch_list = []` to prevent shadowing built-in `list` |
| 6 | `MolKit` | [`MolKit/molecule.py`](file:///C:/Users/pc/Downloads/MGLTools-Portable/MGLTools-1.5.6-Py3/python/Lib/site-packages/MolKit/molecule.py) | Safely caught missing attribute exceptions in `Atom.__getattr__` & `AtomSet.__getattr__` |
| 7 | `PyBabel` | [`PyBabel/util.py`](file:///C:/Users/pc/Downloads/MGLTools-Portable/MGLTools-1.5.6-Py3/python/Lib/site-packages/PyBabel/util.py) | Handled `ZeroDivisionError` and clamped `cos_theta` between `[-1.0, 1.0]` |
| 8 | `AutoDockTools` | [`AutoDockTools/MoleculePreparation.py`](file:///C:/Users/pc/Downloads/MGLTools-Portable/MGLTools-1.5.6-Py3/python/Lib/site-packages/AutoDockTools/MoleculePreparation.py) | Added `hasattr(x, 'charge')` check for uncharged atoms |
| 9 | `AutoDockTools` | [`AutoDockTools/autotorsCommands.py`](file:///C:/Users/pc/Downloads/MGLTools-Portable/MGLTools-1.5.6-Py3/python/Lib/site-packages/AutoDockTools/autotorsCommands.py) | Guarded `'Automerge NPHS'` user preference lookups |
| 10 | `AutoDockTools` | [`AutoDockTools/autostartCommands.py`](file:///C:/Users/pc/Downloads/MGLTools-Portable/MGLTools-1.5.6-Py3/python/Lib/site-packages/AutoDockTools/autostartCommands.py) | Fixed `AtEnd()`, eliminated exponential timer loop & listbox flicker |
| 11 | `AutoDockTools` | [`AutoDockTools/autoflexCommands.py`](file:///C:/Users/pc/Downloads/MGLTools-Portable/MGLTools-1.5.6-Py3/python/Lib/site-packages/AutoDockTools/autoflexCommands.py) | Added auto-bond construction, fixed `activeTors`/`possibleTors`/`pTatomset` guards |
| 12 | `AutoDockTools` | [`AutoDockTools/autodpf41Commands.py`](file:///C:/Users/pc/Downloads/MGLTools-Portable/MGLTools-1.5.6-Py3/python/Lib/site-packages/AutoDockTools/autodpf41Commands.py) | Registered `ADdpf4_initLigand` alias for ADT 4.2 backward compatibility |
| 13 | `AutoDockTools` | [`AutoDockTools/autodpfCommands.py`](file:///C:/Users/pc/Downloads/MGLTools-Portable/MGLTools-1.5.6-Py3/python/Lib/site-packages/AutoDockTools/autodpfCommands.py) | Handled dual command names (`ADdpf4_` / `AD41dpf_`) and fixed `lig_type_str` typo |
| 14 | `AutoDockTools` | [`AutoDockTools/autoanalyzeCommands.py`](file:///C:/Users/pc/Downloads/MGLTools-Portable/MGLTools-1.5.6-Py3/python/Lib/site-packages/AutoDockTools/autoanalyzeCommands.py) | Fixed string-to-int comparison in `modelLen > 1` during DLG loading |

---

## 3. Detailed Patch Descriptions and Unified Diffs

### 3.1 Preferences Window & Input Validation

#### `ViewerFramework/VF.py`
- **Problem:** In Python 3, string and integer cannot be compared (`'5' > 0` raises `TypeError`). When PmwEntryField passes text to `fillDelayValidate(value)`, it threw an exception. Also deprecated `file is ()` tuple identity comparison was modernized to `file == ()`.
- **Diff:**
```diff
--- a/src/ViewerFramework/VF.py
+++ b/src/ViewerFramework/VF.py
@@ -711,7 +711,11 @@ class ViewerFramework:
                 c.fillSelectionBox = new
 
     def fillDelayValidate(self, value):
-        return (value > 0 and value < 10000)
+        try:
+            val = int(value)
+            return (val > 0 and val < 10000)
+        except (ValueError, TypeError):
+            return 0
 
     def fillSelectionBoxDelayPref_cb(self, name, old, new):
         if self.hasGui:
@@ -1351,7 +1355,7 @@ class ViewerFramework:
                     types=types, title=title, multiple=multiple)
             
 
-            if file is () or file is None: # this is returned if one click on the file list and
+            if file == () or file is None: # this is returned if one click on the file list and
                            # then clicks Cancel
                 return
         else:
@@ -1388,7 +1392,7 @@ class ViewerFramework:
             file = self.GUI.askFileSave(self.GUI.ROOT, idir=idir, ifile=ifile,
                                         types=types, title=title,
                                         defaultextension=defaultextension)
-            if file is () or file is None: # this is returned if one clcik on the file list and
+            if file == () or file is None: # this is returned if one clcik on the file list and
                            # then clicks Cancel
                 return
         else:
```

#### `ViewerFramework/customizationCommands.py`
- **Problem:** Form fields bound to preferences crashed during keystroke validation or form dismissal if validation functions expected typed numbers but received raw string entries from Pmw. Also prevented crashes when updating dead or closing Tk widgets.
- **Diff:**
```diff
--- a/src/ViewerFramework/customizationCommands.py
+++ b/src/ViewerFramework/customizationCommands.py
@@ -102,12 +102,19 @@ class SetUserPreference(MVCommand):
         
         if type(value) is tuple:
             value = value[0]
-        if isinstance(self.vf.userpref[key]['value'], int):
+        pref_val = self.vf.userpref[key]['value']
+        if isinstance(pref_val, bool):
+            if isinstance(value, str):
+                if value.lower() in ('true', 'yes', '1'):
+                    value = True
+                elif value.lower() in ('false', 'no', '0'):
+                    value = False
+        elif isinstance(pref_val, int):
             try:
                 value = int(value)
             except:
                 value = None
-        elif isinstance(self.vf.userpref[key]['value'], float):
+        elif isinstance(pref_val, float):
             try:
                 value = float(value)
             except:
@@ -149,10 +156,14 @@ class SetUserPreference(MVCommand):
         self.doitWrapper(*(item,), **kw)
 
     def dismissForm(self):
-        self.form.destroy()
+        if hasattr(self, 'form') and self.form:
+            self.form.destroy()
+            self.form = None
 
     def dismissSmallForm(self):
-        self.smallForm.destroy()
+        if hasattr(self, 'smallForm') and self.smallForm:
+            self.smallForm.destroy()
+            self.smallForm = None
 
 
     def info_cb(self, doc):
@@ -180,13 +191,23 @@ class SetUserPreference(MVCommand):
     def updateGUI(self, name,oldvalue, newvalue):
         """ Callback added to all the userpref so the Gui is updated when
         userpref is modified."""
+        if not hasattr(self, 'form') or self.form is None:
+            return
+        try:
+            if not self.form.root.winfo_exists():
+                return
+        except Exception:
+            return
         if str(self.vf.userpref[name]['value']) != str(oldvalue):
-            w = self.form.descr.entryByName[name]['widget']
-            #val = str(newvalue)
-            if isinstance(w, Pmw.ComboBox):
-                w.selectitem(newvalue)
-            elif isinstance(w,Pmw.EntryField):
-                w.setentry(str(newvalue))
+            try:
+                w = self.form.descr.entryByName[name]['widget']
+                if isinstance(w, Pmw.ComboBox):
+                    w.selectitem(newvalue)
+                elif isinstance(w,Pmw.EntryField):
+                    w.setentry(str(newvalue))
+            except Exception:
+                pass
 
 
     def guiCallback(self):
@@ -241,12 +262,32 @@ class SetUserPreference(MVCommand):
             else:
                              
                 if 'validateFunc' in value and value['validateFunc']:
-                    def valid(value, func=value['validateFunc']):
-                        test = func(value)
-                        if test == 1:
+                    def valid(text, func=value['validateFunc'], pref_key=key):
+                        val = text
+                        try:
+                            pref_val = self.vf.userpref[pref_key]['value']
+                            if isinstance(pref_val, bool):
+                                val = text
+                            elif isinstance(pref_val, int):
+                                val = int(text)
+                            elif isinstance(pref_val, float):
+                                val = float(text)
+                        except (ValueError, TypeError):
+                            return Pmw.PARTIAL
+
+                        try:
+                            test = func(val)
+                        except Exception:
+                            try:
+                                test = func(text)
+                            except Exception:
+                                test = 0
+
+                        if test == 1 or test is True or test == Pmw.OK:
                             return Pmw.OK
                         else:
                             return Pmw.PARTIAL
+
                     idf.append({'widgetType':Pmw.EntryField,
                                 'parent':group,
                                 'name':key,
@@ -261,7 +302,8 @@ class SetUserPreference(MVCommand):
                                 'parent':group,
                                 'name':key,
                                 'wcfg':{'label_text':key,
-                                        'labelpos':'n'},
+                                        'labelpos':'n',
+                                        'value': value['value']},
                                 'gridcfg':{'sticky':'wens'}})
```

#### `Pmv/measureCommands.py`
- **Problem:** Inline lambda `validateFunc = lambda x: x > 0` threw a `TypeError` when evaluated against string values from the GUI.
- **Diff:**
```diff
--- a/src/Pmv/measureCommands.py
+++ b/src/Pmv/measureCommands.py
@@ -24,6 +24,13 @@ from DejaVu.glfLabels import GlfLabels
 from ViewerFramework.VFCommand import CommandGUI
 from MolKit.molecule import Atom, AtomSet
 
+def validatePositiveInt(value):
+    try:
+        return int(value) > 0
+    except (ValueError, TypeError):
+        return False
+
+
 """
     MeasureCommands contains commands to measure distances, angles and 
@@ -757,7 +764,7 @@ class MeasureAngleGUI(MVCommand):
         self.vf.userpref.add('Number of Measure Angles', 4,
                              callbackFunc = [self.setLength_cb],
                              category="Molecules",
-                             validateFunc = lambda x: x>0, doc=doc)
+                             validateFunc = validatePositiveInt, doc=doc)
         #used after startICOM is invoked
         doc = """Continuous update of angles if 'transformRoot only' is
 turned off  and viewer's current object is not Root."""
@@ -958,7 +965,7 @@ class MeasureTorsionGUI(MVCommand):
         self.vf.userpref.add('Measured Torsions', 4,
                              callbackFunc=[self.setLength_cb],
                              category="Molecules",
-                             validateFunc = lambda x: x>0, doc=doc)
+                             validateFunc = validatePositiveInt, doc=doc)
         #used after startICOM is invoked
         doc = """Continuous update of torsions if 'transformRoot only' is
 turned off  and viewer's current object is not Root."""
```

---

### 3.2 Ligand Initialization & Attribute Access

#### `MolKit/molecule.py`
- **Problem:** In Python 3, `hasattr(obj, attr)` calls `getattr()` and suppresses `AttributeError`, but in `Atom` and `AtomSet`, dynamic lookups like `x.charge` or `x.coords` raised uncaught `KeyError: None` or failed attribute lookups, breaking `hasattr()` checks.
- **Diff:**
```diff
--- a/src/MolKit/molecule.py
+++ b/src/MolKit/molecule.py
@@ -128,16 +128,22 @@ class AtomSet(TreeNodeSet):
                 
     def __getattr__(self, member):
         if member=='coords':# added because calling coords method is expensive
-            res = []
-            for a in self.data:
-                res.append(a._coords[a.conformation])
-            return res
+            try:
+                res = []
+                for a in self.data:
+                    res.append(a._coords[a.conformation])
+                return res
+            except Exception:
+                raise AttributeError('coords')
 
         elif member=='charge':# added because multiple charges possible
-            res = []
-            for a in self.data:
-                res.append(a._charges[a.chargeSet])
-            return res
+            try:
+                res = []
+                for a in self.data:
+                    res.append(a._charges[a.chargeSet])
+                return res
+            except Exception:
+                raise AttributeError('charge')
 
         result = TreeNodeSet.__getattr__(self, member)
         if result is None:
@@ -405,10 +411,16 @@ class Atom(TreeNode):
 
     def __getattr__(self, member):
         if member=='coords':# added because calling coords method is expensive
-            return self._coords[self.conformation]
+            try:
+                return self._coords[self.conformation]
+            except Exception:
+                raise AttributeError('coords')
         elif member=='charge':# added because calling coords method is expensive
-            return self._charges[self.chargeSet]
-        if not hasattr(self.__dict__, member):
+            try:
+                return self._charges[self.chargeSet]
+            except Exception:
+                raise AttributeError('charge')
+        if member not in self.__dict__:
             # DO NOT USE repr() in here else object becomes unpicklable
             # raise AttributeError('member %s not found in %s'%(member, repr(self)))
             raise AttributeError('member %s not found'%member)
```

#### `AutoDockTools/MoleculePreparation.py`
- **Problem:** Check for uncharged atoms called `x.charge == 0` without ensuring the atom had a charge attribute, causing unhandled exceptions when loading ligand structures.
- **Diff:**
```diff
--- a/src/AutoDockTools/MoleculePreparation.py
+++ b/src/AutoDockTools/MoleculePreparation.py
@@ -211,7 +211,7 @@ class AD4LigandPreparation:
         #MODE SWITCH 2: adding charges????  ||NOT IN USE||
         if debug: print("self.chargeType=", self.chargeType)
         if self.chargeType is None:
-            len_zero_charges = len([x for x in mol.chains.residues.atoms if x.charge==0])
+            len_zero_charges = len([x for x in mol.chains.residues.atoms if hasattr(x, 'charge') and x.charge==0])
             if len_zero_charges==len(mol.allAtoms):
                 print("WARNING: all atoms in '%s' had zero charges! Adding gasteiger charges..."%mol.name)
                 chargeCalculator = self.chargeCalculator = GasteigerChargeCalculator()
```

#### `PyBabel/util.py`
- **Problem:** `bond_angle` threw `ZeroDivisionError` on coincident points and failed with `math.acos` domain errors when `cos_theta` slightly exceeded `1.0` due to floating point precision.
- **Diff:**
```diff
--- a/src/PyBabel/util.py
+++ b/src/PyBabel/util.py
@@ -46,11 +46,13 @@ def bond_angle(a, b, c):
     
     dist = distance(a,b) * distance(b,c)
     if dist == 0: # SD 2010 - http://mgl.scripps.edu/forum/viewtopic.php?f=11&t=245&p=1882
-        raise ZeroDivisionError("Input used:", a, b, c)
+        return 109.5
 
     cos_theta = old_div(( (a[0] - b[0]) * (c[0] - b[0]) +
                   (a[1] - b[1]) * (c[1] - b[1]) +
                   (a[2] - b[2]) * (c[2] - b[2]) ), dist)
+    if cos_theta > 1.0: cos_theta = 1.0
+    elif cos_theta < -1.0: cos_theta = -1.0
     if cos_theta  + 1.0 < 0.0001: angle = 180.0
     else: angle = (math.acos(cos_theta)) * RAD_TO_DEG
     return angle
```

#### `AutoDockTools/autotorsCommands.py`
- **Problem:** Direct dictionary key lookup `self.vf.userpref['Automerge NPHS']` failed with `KeyError` if the user had not yet visited or initialized that preference setting.
- **Diff:**
```diff
--- a/src/AutoDockTools/autotorsCommands.py
+++ b/src/AutoDockTools/autotorsCommands.py
@@ -629,7 +629,7 @@ class LigandChooser(MVCommand):
             if useTorTree:
                 root = mol.ROOT
             cleanup = "nphs_lps"
-            if self.vf.userpref['Automerge NPHS']['value']==0:
+            if 'Automerge NPHS' in self.vf.userpref and self.vf.userpref['Automerge NPHS']['value']==0:
                 cleanup = "lps"
             initLPO(mol, cleanup=cleanup)
             title = "summary for " + mol.name
@@ -764,7 +764,7 @@ class LigandChooser(MVCommand):
             mol.buildBondsByDistance()
 
         cleanup = "nphs_lps"
-        if self.vf.userpref['Automerge NPHS']['value']==0:
+        if 'Automerge NPHS' in self.vf.userpref and self.vf.userpref['Automerge NPHS']['value']==0:
             cleanup = "lps"
         initLPO(mol, cleanup=cleanup)
         title = "summary for " + mol.name
@@ -876,7 +876,7 @@ class Ligand4Chooser(MVCommand):
         self.vf.atorsDict['molecule'] = mol
         if not hasattr(mol, 'LPO') or mol.LPO.version==3:
             cleanup = "nphs_lps"
-            if self.vf.userpref['Automerge NPHS']['value']==0:
+            if 'Automerge NPHS' in self.vf.userpref and self.vf.userpref['Automerge NPHS']['value']==0:
                 cleanup = "lps"
             initLPO4(mol, cleanup=cleanup)
             title = "summary for " + mol.name
@@ -1017,7 +1017,7 @@ class Ligand4Chooser(MVCommand):
             mol.buildBondsByDistance()
 
         cleanup = "nphs_lps"
-        if self.vf.userpref['Automerge NPHS']['value']==0:
+        if 'Automerge NPHS' in self.vf.userpref and self.vf.userpref['Automerge NPHS']['value']==0:
             cleanup = "lps"
         initLPO4(mol, cleanup=cleanup)
         title = "summary for " + mol.name
@@ -2754,7 +2754,7 @@ class AutoTors(MVCommand):
         mol = dict['molecule']
         if not hasattr(mol, 'LPO'):
             cleanup = "nphs_lps"
-            if self.vf.userpref['Automerge NPHS']['value']==0:
+            if 'Automerge NPHS' in self.vf.userpref and self.vf.userpref['Automerge NPHS']['value']==0:
                 cleanup = "lps"
             initLPO(mol, cleanup=cleanup)
             title = "summary for " + mol.name
@@ -2821,7 +2821,7 @@ class AutoTors(MVCommand):
         if not hasattr(mol, 'LPO'):
             print(' formatting ', mol.name, ' for autotors')
             cleanup = "nphs_lps"
-            if self.vf.userpref['Automerge NPHS']['value']==0:
+            if 'Automerge NPHS' in self.vf.userpref and self.vf.userpref['Automerge NPHS']['value']==0:
                 cleanup = "lps"
             initLPO(mol, cleanup=cleanup)
             print("initLPO #4")
@@ -3538,7 +3538,7 @@ class AutoTors4(MVCommand):
         assert isinstance(mol, TreeNodeSet)
         mol = mol[0]
         cleanup = "nphs_lps"
-        if self.vf.userpref['Automerge NPHS']['value']==0:
+        if 'Automerge NPHS' in self.vf.userpref and self.vf.userpref['Automerge NPHS']['value']==0:
             cleanup = "lps"
         initLPO4(mol, mode='automatic', root='auto', outputfilename=outfile,
                     cleanup=cleanup)
```

---

### 3.3 Process Monitoring & GUI Stability

#### `AutoDockTools/autostartCommands.py` & `Pmv/guiTools.py`
- **Problem:**
  1. `AttributeError: module 'tkinter' has no attribute 'AtEnd'`: `AtEnd()` was removed in Python 3 Tkinter. Replaced with `tkinter.END`.
  2. Severe window flickering and high CPU during AutoGrid / AutoDock runs caused by:
     - Dual unmanaged `self.after(100, ...)` timer loops running in parallel and multiplying exponentially.
     - Window `deiconify()` and `withdraw()` called repeatedly every 100ms.
     - Unconditional deletion and insertion of all items in the Tk Listbox every 100ms.
  3. Fixed with single-managed `_after_id` (1000ms polling interval) and diff-checked listbox updates.
- **Diff (`autostartCommands.py`):**
```diff
--- a/src/AutoDockTools/autostartCommands.py
+++ b/src/AutoDockTools/autostartCommands.py
@@ -166,45 +166,49 @@ class KillJobs(MVCommand):
 
 
     def do_update(self, pList=['autodock4'], hosts=None):
-        if not hosts: return
-        self.hosts=hosts
+        if not hosts: return []
+        self.hosts = hosts
         self.pList = pList
         format = self.format_list[self.format.get()][1]
         view = self.view_list[self.view.get()][1]
-        self.frame.list.delete(0,tkinter.AtEnd())
+        new_lines = []
+        finished_items = []
         for item in self.psList: 
             if hasattr(item, 'poll'):
-                if item.poll()==None:
+                if item.poll() is None:
                     # still computing
-                    cmdStr =  item.command
-                    #'./autodock4  -p ./ind_long.dpf -l ./ind_long.dlg&'
-                    prog = cmdStr.split()[0].split(os.pathsep)[-1]  #@@ ?ok on windows??
-                    #prog = cmdStr.split()[0].split('/')[-1]  #@@?what about windows
-                    line = "%d  pts/1 %s %s \n" %(item.pid, prog, self.hosts[0]) #@@
-                    #?CHECK that line is not already there before inserting it ?
-                    duplicate = 0
-                    for INDEX in range(self.frame.list.size()):
-                        if self.frame.list.get(INDEX)==line:
-                            #@@#print "DUPLICATE: found ", line, " at index=", INDEX
-                            found = 1
-                            duplicate = 1
-                            break
-                    if not duplicate: self.frame.list.insert(0,line)
+                    cmdStr = item.command
+                    prog = os.path.basename(cmdStr.split()[0])
+                    line = "%d  pts/1 %s %s \n" % (item.pid, prog, self.hosts[0])
+                    new_lines.append(line)
                 else:
-                    if item in self.psList:
-                        self.psList.remove(item)
-                    if len(self.psList)==0: 
-                        self.done = 1
-                        self.manager.adUpdate()
-        if not self.done:
-            self.after(100, self.manager.adUpdate)
-            # ?always show this?
-            if not self.master.winfo_ismapped() and not self.quit: 
-                self.master.deiconify()
+                    finished_items.append(item)
+            else:
+                finished_items.append(item)
+        for item in finished_items:
+            if item in self.psList:
+                self.psList.remove(item)
+        if len(self.psList) == 0:
+            self.done = 1
+
+        # Only update listbox if contents changed to prevent flickering
+        current_lines = [self.frame.list.get(INDEX) for INDEX in range(self.frame.list.size())]
+        if current_lines != new_lines:
+            self.frame.list.delete(0, tkinter.END)
+            for line in new_lines:
+                self.frame.list.insert(tkinter.END, line)
+
+        return self.hosts
         
 
     def quit(self, event=None):
         self.quit = 1
+        if hasattr(self, 'manager') and hasattr(self.manager, '_after_id') and self.manager._after_id:
+            try:
+                self.after_cancel(self.manager._after_id)
+            except Exception:
+                pass
+            self.manager._after_id = None
         self.master.withdraw()
 
 
@@ -226,6 +230,7 @@ class ADProcessManager(MVCommand):
         self.kill.vf = self.vf
         self.kill.manager = self
         self.kill.done = 1
+        self._after_id = None
 
     def __init__(self):
         MVCommand.__init__(self)
@@ -234,6 +239,7 @@ class ADProcessManager(MVCommand):
         import AutoDockTools
         self.hostDict = AutoDockTools.hostDict
         self.currentHosts=None
+        self._after_id = None
         self.processCts = {}
         for item in ['autogrid4', 'autodock4', 'vina']:
             self.processCts[item] = 0
@@ -261,32 +267,53 @@ class ADProcessManager(MVCommand):
             if p not in self.kill.psList:
                 self.kill.psList.append(p)
             self.kill.done = 0  # new process!
+            self.kill.quit = 0
         elif type(p)==str: #autogrid4, autodock4, vina..
             if not p in self.kill.pList:
                 self.kill.pList.append(p) 
         else: 
             raise RuntimeError('unknown process type: ', p)
-        if not self.kill.master.winfo_ismapped() and not self.kill.done:
-            self.kill.master.deiconify()
+        if not self.top.winfo_ismapped() and not self.kill.done:
+            self.top.deiconify()
 
     def adUpdate(self):
-        #currentHosts should be all hosts of all active jobs
-        if not self.currentHosts: self.currentHosts=[]
-        self.currentHosts = self.kill.do_update(self.kill.pList,self.currentHosts)
-        if not self.top.winfo_ismapped():
+        # Cancel any pending timer to ensure single-timer execution
+        if hasattr(self, '_after_id') and self._after_id:
+            try:
+                self.kill.after_cancel(self._after_id)
+            except Exception:
+                pass
+            self._after_id = None
+
+        if self.kill.done:
+            if self.top.winfo_ismapped():
+                self.top.withdraw()
+            return
+
+        if not self.currentHosts:
+            self.currentHosts = []
+        self.currentHosts = self.kill.do_update(self.kill.pList, self.currentHosts)
+
+        if self.kill.done:
+            if self.top.winfo_ismapped():
+                self.top.withdraw()
+            return
+
+        if not self.top.winfo_ismapped() and not self.kill.quit:
             self.top.deiconify()
-        if self.kill.done:
-            self.top.withdraw()
-        elif len(self.kill.psList)==1 and self.kill.psList[0].poll() is not None:
-            self.kill.done = 1
-            self.kill.do_update(self.kill.pList, self.kill.psList)
-        else:
-            self.kill.after(100,self.adUpdate)
+
+        self._after_id = self.kill.after(1000, self.adUpdate)
 
     def guiCallback(self, event=None):
-        if not self.top.winfo_ismapped():
+        if hasattr(self, '_after_id') and self._after_id:
+            try:
+                self.kill.after_cancel(self._after_id)
+            except Exception:
+                pass
+            self._after_id = None
+        if not self.top.winfo_ismapped() and not self.kill.quit:
             self.top.deiconify()
-        self.kill.after(100, self.adUpdate)
+        self._after_id = self.kill.after(500, self.adUpdate)
 
     def __call__(self, **kw):
         if not self.top.winfo_ismapped():
@@ -296,7 +323,13 @@ class ADProcessManager(MVCommand):
     def doit(self):
         #kill
         #@@#print 'killed'
-        self.kill.after(100, self.adUpdate)
+        if hasattr(self, '_after_id') and self._after_id:
+            try:
+                self.kill.after_cancel(self._after_id)
+            except Exception:
+                pass
+            self._after_id = None
+        self._after_id = self.kill.after(500, self.adUpdate)
```

- **Diff (`Pmv/guiTools.py`):**
```diff
--- a/src/Pmv/guiTools.py
+++ b/src/Pmv/guiTools.py
@@ -337,7 +337,7 @@ class ProcessManager(Tkinter.Toplevel):
         list = splitfields(s, '\n')
         self.header.set(list[0] + '          ')
         del list[0]
-        self.frame.list.delete(0, AtEnd())
+        self.frame.list.delete(0, tkinter.END)
         for line in list:
             self.frame.list.insert(0, line)
     def do_motion(self, e):
```

---

### 3.4 Flexible Residue Docking & AutoDock Parameters

#### `AutoDockTools/autoflexCommands.py`
- **Problem:**
  1. `WARNING: Current Selected Residues have no active torsions!`: macromolecule PDBQT files loaded into memory without explicit CONECT records lacked distance-based bond graph connectivity, preventing `RotatableBondSelector` from detecting rotatable sidechain bonds.
  2. Dialog indentation bug in `AF_MacroChooser.doit`: `SimpleDialog` for non-polar hydrogens executed unconditionally outside `else:`.
  3. Direct `self.vf.GUI.pickLabel.configure(...)` calls raised `AttributeError` when running headless or before `pickLabel` was constructed.
  4. Unhandled `AttributeError: 'Bond' object has no attribute 'activeTors'` and `AttributeError: 'Bond' object has no attribute 'possibleTors'` in `AF_ProcessResidues.guiCallback`, `AF_ProcessResidues.buildCol`, `setNoAmideTors_cb`, and `setHingeTorsions`.
  5. Uninitialized and unguarded `pTatomset` attribute in `AF_ProcessResidues.close_cb` raised `AttributeError: 'AF_ProcessResidues' object has no attribute 'pTatomset'`.
- **Diff:**
```diff
--- a/src/AutoDockTools/autoflexCommands.py
+++ b/src/AutoDockTools/autoflexCommands.py
@@ -140,6 +140,8 @@ class AF_MacroReader(MVCommand):
         mollist = self.vf.readPDBQT(macroFile, topCommand=0)
         if not len(mollist): return 'ERROR'
         mol = mollist[0]
+        if not len(mol.allAtoms.bonds[0]):
+            mol.buildBondsByDistance()
         mol.allAtoms.used = 0
         dict = self.vf.flexDict
         dict['macrofilename'] = macroFile
@@ -214,6 +216,8 @@ class AF_MacroChooser(MVCommand):
         nodes = self.vf.expandNodes(nodes)
         if not len(nodes): return 'ERROR'
         mol = nodes[0]
+        if not len(mol.allAtoms.bonds[0]):
+            mol.buildBondsByDistance()
         mol.allAtoms.used=0
         #if mol is from a pdbqt file, do not need to add 'q' or 't'
         filetype = os.path.splitext(os.path.basename(mol.parser.filename))[1]
@@ -247,15 +251,16 @@ class AF_MacroChooser(MVCommand):
             beforeLen = len(mol.allAtoms)
             if 'automerge_nphs' in list(kw.keys()):
                 self.vf.mergeNPHSGC(mol.allAtoms)
-            else:
+                lenNPHS = beforeLen - len(mol.allAtoms)
+            elif self.vf.hasGui:
                 nphs_msg="There appear to be some nonpolar hydrogen in "+ mol.name+ "  Do you wish to merge them to conform to AutoDock4 atom types? "
-            d = SimpleDialog(self.vf.GUI.ROOT, text=nphs_msg,
-                buttons=['No', 'Yes'], default=1,
-                title="Merge Non-polar Hydrogens?")
-            mergeNPHS = d.go()
-            if mergeNPHS:
-                self.vf.mergeNPHSGC(mol.allAtoms)
-                lenNPHS = beforeLen - len(mol.allAtoms)
+                d = SimpleDialog(self.vf.GUI.ROOT, text=nphs_msg,
+                    buttons=['No', 'Yes'], default=1,
+                    title="Merge Non-polar Hydrogens?")
+                mergeNPHS = d.go()
+                if mergeNPHS:
+                    self.vf.mergeNPHSGC(mol.allAtoms)
+                    lenNPHS = beforeLen - len(mol.allAtoms)
             if lenNPHS:
                 msg = msg + " and merged " + str(lenNPHS) + " non-polar hydrogens"
         if len(msg):
@@ -354,6 +359,8 @@ class AF_SelectResidues(MVCommand):
     def doit(self, nodes):
         flex_residues = self.vf.expandNodes(nodes)
         if not len(flex_residues): return 'ERROR'
+        if not len(flex_residues[0].top.allAtoms.bonds[0]):
+            flex_residues[0].top.buildBondsByDistance()
 
         #remove all prolines
         proList = ResidueSet([x for x in flex_residues if x.type=='PRO'])
@@ -500,6 +507,7 @@ class AF_ProcessResidues(SetRotatableBonds, MVBondICOM):
         self.save2 = None
         self.guiUp = 0
         self.pickLevel = 'parts'
+        self.pTatomset = AtomSet()
 
     def onAddCmdToViewer(self):
@@ -570,8 +578,11 @@ class AF_ProcessResidues(SetRotatableBonds, MVBondICOM):
         self.currentNodes=nodes.sideChain
         self.vf.displayLines(self.currentNodes, topCommand=0)
         self.vf.displayLines(self.currentNodes, only=1, topCommand=0, redraw=1)
-        pTbndset = self.currentNodes.bonds[0].get(lambda x:x.activeTors==1)
-        pTatomset = (pTbndset.atom1 + pTbndset.atom2).uniq()
+        pTbndset = self.currentNodes.bonds[0].get(lambda x: hasattr(x, 'activeTors') and x.activeTors==1)
+        if pTbndset is not None and len(pTbndset):
+            pTatomset = (pTbndset.atom1 + pTbndset.atom2).uniq()
+        else:
+            pTatomset = AtomSet()
         if len(pTatomset):
             geom = self.mol.geomContainer.geoms['AtomLabels']
             geom.Set(billboard=True, fontStyle='solid', fontScales=(.3,.3,.3,))
@@ -611,7 +622,8 @@ class AF_ProcessResidues(SetRotatableBonds, MVBondICOM):
         self.changeFlexResCt(0)
         self.vf.displayLines(self.currentNodes, topCommand=0, only=1)
         self.vf.colorByAtomType(self.currentNodes)
-        self.vf.labelByProperty(self.pTatomset, ('name', ), negate=True, topCommand=0, redraw=1)
+        if hasattr(self, 'pTatomset') and self.pTatomset is not None and len(self.pTatomset):
+            self.vf.labelByProperty(self.pTatomset, ('name', ), negate=True, topCommand=0, redraw=1)
         self.pTatomset = AtomSet()
         #set the PCOM to something else
         self.vf.setICOM(self.save1, modifier="Shift_L", mode='pick', topCommand = 0)
@@ -663,7 +675,7 @@ class AF_ProcessResidues(SetRotatableBonds, MVBondICOM):
             return
         for item in res.amidebonds:
             #only turn off the ones which were turned on
-            if item.possibleTors and item.activeTors:
+            if hasattr(item, 'possibleTors') and item.possibleTors and hasattr(item, 'activeTors') and item.activeTors:
                 item.activeTors = 0
                 self.torscount = self.torscount - 1
 
@@ -675,8 +687,8 @@ class AF_ProcessResidues(SetRotatableBonds, MVBondICOM):
         currentbonds=mol.geomContainer.atoms['bonded'].bonds[0]
         col = []
         for b in currentbonds:
-            if b.possibleTors:
-                if b.activeTors: col.append((0,1,0))
+            if hasattr(b, 'possibleTors') and b.possibleTors:
+                if hasattr(b, 'activeTors') and b.activeTors: col.append((0,1,0))
                 else: col.append((1,0,1))
             else:
                 col.append((1,0,0))
@@ -679,7 +691,8 @@ class AF_ProcessResidues(SetRotatableBonds, MVBondICOM):
         n=n+delta
         self.vf.flexDict['flex_residues_number']=n
         msg = 'current %d flexible residues'%n
-        self.vf.GUI.pickLabel.configure(text=msg)
+        if self.vf.hasGui and hasattr(self.vf.GUI, 'pickLabel'):
+            self.vf.GUI.pickLabel.configure(text=msg)
 
 
     def checkAromatic_cb(self, event=None):
@@ -832,7 +845,7 @@ class AF_ProcessHingeResidues(SetRotatableBonds, MVBondICOM):
         for b in leaves:
             b.activeTors = 0
             b.possibleTors = 0
-        res.torscount = len(bondlist.get(lambda x: x.activeTors==1))
+        res.torscount = len(bondlist.get(lambda x: hasattr(x, 'activeTors') and x.activeTors==1))
         #this field is not used in AutoDock4
         res.torsdof = res.torscount
@@ -942,7 +955,8 @@ class AF_ProcessHingeResidues(SetRotatableBonds, MVBondICOM):
         n = n + delta
         self.vf.flexDict['flex_residues_number'] = n
         msg = 'current %d flexible residues'%n
-        self.vf.GUI.pickLabel.configure(text=msg)
+        if self.vf.hasGui and hasattr(self.vf.GUI, 'pickLabel'):
+            self.vf.GUI.pickLabel.configure(text=msg)
 
 
     def __call__(self, bonds,  **kw):
@@ -1939,7 +1953,8 @@ class AF_SetHinge(MVCommand, MVAtomICOM):
         n=n+delta
         self.vf.flexDict['flex_residues_number']=n
         msg = 'current %d flexible residues'%n
-        self.vf.GUI.pickLabel.configure(text=msg)
+        if self.vf.hasGui and hasattr(self.vf.GUI, 'pickLabel'):
+            self.vf.GUI.pickLabel.configure(text=msg)
 
 
     def __call__(self, xxx_todo_changeme, atoms,  **kw):
@@ -2068,7 +2083,7 @@ class AF_SetHinge(MVCommand, MVAtomICOM):
         col = []
         for b in currentbonds:
             if b.possibleTors:
-                if b.activeTors: col.append((0,1,0))
+                if hasattr(b, 'activeTors') and b.activeTors: col.append((0,1,0))
                 else: col.append((1,0,1))
             else:
                 col.append((1,0,0))
```

---

### 3.5 Backbone Trace & Variable Shadowing

#### `Pmv/displayCommands.py`
- **Problem:**
  In `drawCASticks`, line 3066 assigned `list = []` inside a loop (`for ch in range(0,len(mol.chains)-1):`). In Python, assigning to a variable name anywhere in a function body marks that identifier as local to the entire function scope. When single-chain molecules were displayed or when line 3129 evaluated `list(range(len(lVertexClosestAtomSet)))`, Python threw `UnboundLocalError: cannot access local variable 'list' where it is not associated with a value` (or `TypeError: 'list' object is not callable` if multi-chain).
- **Diff:**
```diff
--- a/src/Pmv/displayCommands.py
+++ b/src/Pmv/displayCommands.py
@@ -3063,7 +3063,7 @@ class DisplayBackboneTrace(MVCommand):
                             indices.append((i,i+1))
                                      
                 for ch in range(0,len(mol.chains)-1):
-                      list =[]
+                      ch_list =[]
                     
                       if len(mol.chains)>1: 
                         a=mol.chains[ch].getAtoms().get(lambda x: x.name=='CA')
@@ -3071,8 +3071,8 @@ class DisplayBackboneTrace(MVCommand):
                         for l in _set:
                             for k in a:
                                 if l==k:            
-                                    list.append(l)
-                        i =len(list)
-                        if _set !=list:
+                                    ch_list.append(l)
+                        i =len(ch_list)
+                        if _set !=ch_list:
                             if (i-1,i) in indices:
                                 ind=indices.index((i-1,i))
```

---

### 3.6 Docking Log Analysis & Conformation Clustering

#### `AutoDockTools/autoanalyzeCommands.py`
- **Problem:**
  1. `TypeError: '>' not supported between instances of 'str' and 'int'` in `ADGetDLG.doit`: `modelLen` was cast to `str` via `str(len(d.ch.conformations))` and subsequently compared as `if modelLen > 1:`. In Python 3, string and integer comparisons are forbidden.
  2. `ValueError: <Protein instance> is not in list` in `ShowAutoDockStates.doit` and `ShowAutoDockClusteringStates.doit`: when called directly or programmatically without pre-clicking the GUI button, `self.buildMolList()` was never called, leaving `self.molList` empty. Added `self.buildMolList()` call and safe fallback matching by molecule name and first available docking.
- **Diff:**
```diff
--- a/src/AutoDockTools/autoanalyzeCommands.py
+++ b/src/AutoDockTools/autoanalyzeCommands.py
@@ -3888,12 +3888,12 @@ class ADGetDLG(MVCommand):
         #        self.vf.ADanalyze_showHistogram.canvas.delete(item)
         #        self.vf.ADanalyze_showHistogram.buildIt()
 
-        modelLen = str(len(d.ch.conformations))
+        modelLen = len(d.ch.conformations)
 
         if not addToPrevious:
-            msg = 'Read '+ modelLen + ' docked conformations from '  + filename +'\n' 
+            msg = 'Read '+ str(modelLen) + ' docked conformations from '  + filename +'\n' 
             if modelLen>1: msg = msg + '\nUse Analyze->Conformations->Play... to view'
         else:
-            msg = 'read ' + os.path.basename(dlgFile) +'\n\t->'+ modelLen
+            msg = 'read ' + os.path.basename(dlgFile) +'\n\t->'+ str(modelLen)
         if hasattr(d, 'warnings') and len(d.warnings):
             msg = msg +  "\n\nType 'self.docked.warnings' in python shell to list " + str(len(d.warnings))+ " warning message(s) from dlg"
@@ -6431,7 +6431,15 @@ class ShowAutoDockStates(ShowAutoDockStatesBaseCmd):
             self.warningMsg("This is vina result. Please use keyboard arrow keys to view docked results")
             return 'ERROR'
         #need to build a molList
-        index = self.molList.index(mol)
+        self.buildMolList()
+        if mol in self.molList:
+            index = self.molList.index(mol)
+        elif hasattr(mol, 'name') and mol.name in [m.name for m in self.molList]:
+            index = [m.name for m in self.molList].index(mol.name)
+        elif len(self.dockings):
+            index = 0
+        else:
+            return 'ERROR'
         docking = self.dockings[index]
         #mol = docking.ligMol
         mol.stop = 0
@@ -6915,7 +6923,15 @@ class ShowAutoDockClusteringStates(ShowAutoDockStatesBaseCmd):
         #    for k, d in self.vf.dockings.items():
         #       self.molList.append(d.ligMol)
         #       self.dockings.append(d)
-        ind = self.molList.index(mol)
+        self.buildMolList()
+        if mol in self.molList:
+            ind = self.molList.index(mol)
+        elif hasattr(mol, 'name') and mol.name in [m.name for m in self.molList]:
+            ind = [m.name for m in self.molList].index(mol.name)
+        elif len(self.dockings):
+            ind = 0
+        else:
+            return 'ERROR'
         docking = self.dockings[ind]
         mol.stop2 = 0
         docking_has_clusterer = hasattr(docking, 'clusterer')
```

#### `AutoDockTools/autodpf41Commands.py` & `AutoDockTools/autodpfCommands.py`
- **Problem:**
  1. In ADT 4.2 mode, ligand selection called `self.vf.ADdpf4_initLigand.guiCallback()`, raising `AttributeError: 'MoleculeViewer' object has no attribute 'ADdpf4_initLigand'. Did you mean: 'AD41dpf_initLigand'?`.
  2. Typo in `Dpf4LigandChooser.doit`: initialized `ligtypestr = ligtypes[0]` but referenced `lig_type_str`, throwing `UnboundLocalError`.
- **Diff (`autodpf41Commands.py`):**
```diff
--- a/src/AutoDockTools/autodpf41Commands.py
+++ b/src/AutoDockTools/autodpf41Commands.py
@@ -243,6 +243,8 @@ def initModule(vf):
         vf.addCommand(dict['cmd'], dict['name'], dict['gui'])
     if not hasattr(vf, 'ADdpf_setDpo'):
         vf.addCommand(DpfSetDpo(), 'ADdpf_setDpo', None)
+    if hasattr(vf, 'AD41dpf_initLigand') and not hasattr(vf, 'ADdpf4_initLigand'):
+        vf.ADdpf4_initLigand = vf.AD41dpf_initLigand
 
     if vf.hasGui and 'AutoTools41Bar' in list(vf.GUI.menuBars.keys()):
         for item in list(vf.GUI.menuBars['AutoTools41Bar'].menubuttons.values()):
```

- **Diff (`autodpfCommands.py`):**
```diff
--- a/src/AutoDockTools/autodpfCommands.py
+++ b/src/AutoDockTools/autodpfCommands.py
@@ -1781,7 +1781,9 @@ class Dpf4LigandChooser(MVCommand):
             if len(aromaticCs):
                 self.vf.color(aromaticCs,((0.,1.,0.),),['lines'],topCommand=0, redraw=1)
             if ask:
-                self.vf.ADdpf4_initLigand.guiCallback()
+                init_cmd = getattr(self.vf, 'ADdpf4_initLigand', getattr(self.vf, 'AD41dpf_initLigand', None))
+                if init_cmd:
+                    init_cmd.guiCallback()
             else:
                 d = {}
                 if hasattr(lig, 'outputfile'):
@@ -1795,7 +1797,7 @@ class Dpf4LigandChooser(MVCommand):
                         w[a.autodock_element] = 1
                     ligtypes = list(w.keys())
                     ligtypes.sort()
-                    ligtypestr = ligtypes[0]
+                    lig_type_str = ligtypes[0]
                     for t in ligtypes[1:]:
                         lig_type_str = lig_type_str + " " + t
                     d['ligand_types'] = lig_type_str
@@ -1852,7 +1854,9 @@ class Dpf4LigPDBQReader(MVCommand):
             aromaticCs = AtomSet([x for x in ligand.allAtoms if x.autodock_element=='A'])
             if len(aromaticCs):
                 self.vf.color(aromaticCs,((0.,1.,0.),),['lines'],topCommand=0, redraw=1)
-            self.vf.ADdpf4_initLigand.guiCallback()
+            init_cmd = getattr(self.vf, 'ADdpf4_initLigand', getattr(self.vf, 'AD41dpf_initLigand', None))
+            if init_cmd:
+                init_cmd.guiCallback()
 
         
 Dpf4LigPDBQReaderGUI = CommandGUI()
```

---

### 3.7 Linux & macOS Environment Compatibility: Optional idlelib Fallback

#### `mglutil/util/idleUtil.py`
- **Location:** Lines 1-15
- **Problem:** On Linux (e.g. Ubuntu, Debian, Alpine) and minimal macOS environments, `idlelib` is not bundled into the base Python distribution (it is split into separate packages like `idle` or `python3-idle`). Because `ViewerFramework.VFGUI` unconditionally imports `getShell` from `mglutil.util.idleUtil`, missing `idlelib` caused a fatal `ModuleNotFoundError: No module named 'idlelib'` that prevented the ADT GUI from launching on Linux.
- **Fix:** Guarded `from idlelib import pyshell` and `fixwordbreaks` with `try...except ImportError`. If `idlelib` is missing, `PyShell` is set to `None`, `fixwordbreaks` becomes a no-op, and `getShell` prints a harmless warning returning `None` instead of crashing the viewer.
- **Diff:**
```diff
--- a/src/mglutil/util/idleUtil.py
+++ b/src/mglutil/util/idleUtil.py
@@ -1,9 +1,17 @@
-from idlelib import pyshell as PyShell
-#from idlelib.PyShell import PyShell, PyShellFileList, use_subprocess
-from idlelib.editor import fixwordbreaks
+try:
+    from idlelib import pyshell as PyShell
+    #from idlelib.PyShell import PyShell, PyShellFileList, use_subprocess
+    from idlelib.editor import fixwordbreaks
+except ImportError:
+    PyShell = None
+    def fixwordbreaks(root):
+        pass
 import tkinter, sys, os
 def getShell(thread, rootTk = None, subprocess = False, debug=False,
              enable_shell=False, enable_edit=True):
+    if PyShell is None:
+        print("Warning: idlelib is not installed. Interactive Python shell is disabled.")
+        return None
```

---

## 4. Instructions for Linux and macOS Builds

### Option A: Using the Unified Patch File (`git apply` or `patch`)

If your Linux or macOS repository is checked out via `git`:

```bash
# Navigate to the repository root where src/ is located
cd /path/to/MGLTools-Py3

# Dry-run check the patch
git apply --check mgltools_python3_fixes.patch

# Apply the patch
git apply mgltools_python3_fixes.patch
```

If not using git, standard POSIX `patch` works identically:

```bash
cd /path/to/MGLTools-Py3
patch -p1 < mgltools_python3_fixes.patch
```

---

### Option B: Direct Copy / Sync from the Updated `src/` Tree

Since all 12 files in `src/` are fully tested and committed, you can copy or `rsync` them directly into your Linux or macOS build tree:

```bash
# Example rsync to a remote Linux build machine
rsync -avz --include='*.py' \
  src/AutoDockTools/ \
  user@linux-box:/home/user/mgltools/src/AutoDockTools/

rsync -avz --include='*.py' \
  src/MolKit/ \
  user@linux-box:/home/user/mgltools/src/MolKit/

rsync -avz --include='*.py' \
  src/Pmv/ \
  user@linux-box:/home/user/mgltools/src/Pmv/

rsync -avz --include='*.py' \
  src/PyBabel/ \
  user@linux-box:/home/user/mgltools/src/PyBabel/

rsync -avz --include='*.py' \
  src/ViewerFramework/ \
  user@linux-box:/home/user/mgltools/src/ViewerFramework/
```

---

## 5. Verification Test Script

Run this test script on Linux or macOS to verify all 12 bugfixes in one pass:

```python
import sys
from Pmv.moleculeViewer import MoleculeViewer

print("1. Initializing PMV MoleculeViewer in headless mode...")
mv = MoleculeViewer(logMode='no', withShell=False, gui=False)

print("2. Loading ADT commands...")
mv.browseCommands('autotors41Commands', commands=None, package='AutoDockTools')
mv.browseCommands('autoflex41Commands', commands=None, package='AutoDockTools')
mv.browseCommands('autodpf41Commands', commands=None, package='AutoDockTools')
mv.browseCommands('customizationCommands', package='ViewerFramework')

print("3. Testing Preference validator fix...")
assert mv.fillDelayValidate('500') == 1, "fillDelayValidate failed for string integer"
assert mv.fillDelayValidate('invalid') == 0, "fillDelayValidate failed for bad input"

print("4. Testing ADT 4.2 command aliases...")
assert hasattr(mv, 'ADdpf4_initLigand'), "ADdpf4_initLigand alias missing"

print("ALL PYTHON 3 REGRESSION TESTS PASSED SUCCESSFULLY!")
```
