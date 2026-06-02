"""
Create a ParametricBridgeNode in Maya and hook it up to a visible mesh.

Run this in the Maya Script Editor (Python tab), or source it with:
    import maya.cmds as cmds; exec(open(r"<path>/create_bridge.py").read())

Set FORCE_RELOAD = True to unload/reload the plugin (picks up a fresh build).
"""

import maya.cmds as cmds

PLUGIN_NAME  = "ParametricStaircaseNode"
PLUGIN_PATH  = r"C:\Users\Dami\OneDrive\Documents\My projects\ParametricModelling\build\Debug\ParametricStaircaseNode.mll"
FORCE_RELOAD = False

# ── 1. Load plugin ────────────────────────────────────────────────────────────
if FORCE_RELOAD and cmds.pluginInfo(PLUGIN_NAME, query=True, loaded=True):
    cmds.unloadPlugin(PLUGIN_NAME)

if not cmds.pluginInfo(PLUGIN_NAME, query=True, loaded=True):
    cmds.loadPlugin(PLUGIN_PATH)
    print(f"Loaded plugin: {PLUGIN_PATH}")
else:
    print("Plugin already loaded.")

# ── 2. Clean up previous run ──────────────────────────────────────────────────
for name in ("bridge1", "bridgeNode1"):
    if cmds.objExists(name):
        cmds.delete(name)

# ── 3. Create the DG node ─────────────────────────────────────────────────────
bridge_node = cmds.createNode("parametricBridgeNode", name="bridgeNode1")

# ── 4. Set parameters ─────────────────────────────────────────────────────────
cmds.setAttr(f"{bridge_node}.stepCount",        6)
cmds.setAttr(f"{bridge_node}.totalHeight",      2.0)
cmds.setAttr(f"{bridge_node}.stairWidth",       2.0)
cmds.setAttr(f"{bridge_node}.treadDepth",       0.4)
cmds.setAttr(f"{bridge_node}.bridgeLength",     8.0)
cmds.setAttr(f"{bridge_node}.deckScale",        1.0)
cmds.setAttr(f"{bridge_node}.deckSegments",     16)
cmds.setAttr(f"{bridge_node}.archHeight",       1.5)
cmds.setAttr(f"{bridge_node}.stepSubdivisions", 2)
cmds.setAttr(f"{bridge_node}.bumpHeight",       0.0)
cmds.setAttr(f"{bridge_node}.bumpFreq",         1.0)
cmds.setAttr(f"{bridge_node}.chamfer",          0.1)

# ── 5. Create mesh transform + shape and wire up outMesh → inMesh ─────────────
mesh_transform = cmds.createNode("transform", name="bridge1")
mesh_shape     = cmds.createNode("mesh", name="bridgeShape1", parent=mesh_transform)

cmds.connectAttr(f"{bridge_node}.outMesh", f"{mesh_shape}.inMesh")

# ── 6. Assign default lambert so it's visible in the viewport ─────────────────
cmds.sets(mesh_shape, edit=True, forceElement="initialShadingGroup")

# ── 7. Frame the new mesh ─────────────────────────────────────────────────────
cmds.select(mesh_transform)
cmds.viewFit()

print(f"Bridge created: {mesh_transform}  (DG node: {bridge_node})")
print("Tweak parameters in the Channel Box or Attribute Editor under 'bridgeNode1'.")
