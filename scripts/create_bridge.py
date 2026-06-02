"""
Create a ParametricBridgeNode in Maya and hook it up to a visible mesh.

Run in the Maya Script Editor (Python tab), or:
    exec(open(r"<path>/create_bridge.py").read())

Plug-in loading/reloading is handled by parametric_plugin.reload_plugin(), which
always picks up a fresh build and verifies both node types registered. To load a
Debug build instead, pass its path to reload_plugin() below.
"""

import os
import sys

# Make the shared helper importable no matter how this file is executed.
_HERE = os.path.dirname(os.path.abspath(__file__)) if "__file__" in globals() \
    else os.path.dirname(r"C:\Users\Dami\OneDrive\Documents\My projects\ParametricModelling\scripts\create_bridge.py")
if _HERE not in sys.path:
    sys.path.insert(0, _HERE)

import maya.cmds as cmds
import parametric_plugin as pp

# ── 1. Clean reload (default = Release build) ─────────────────────────────────
pp.reload_plugin()
# For a Debug build instead:
# pp.reload_plugin(os.path.join(os.path.dirname(_HERE), "build", "Debug", "ParametricStaircaseNode.mll"))

# ── 2. Build the bridge ───────────────────────────────────────────────────────
bridge_node, bridge_xform, _ = pp.build_mesh(
    "parametricBridgeNode", "bridgeNode1", "bridge1", "bridgeShape1",
    {
        # structure
        "stepCount": 6, "totalHeight": 2.0, "stairWidth": 2.0, "treadDepth": 0.4,
        "bridgeLength": 8.0, "deckScale": 1.0, "deckSegments": 16,
        "archHeight": 1.5, "chamfer": 0.1,
        # voussoir arch  (voussoirCount > 1 => segmented arch; 1 => smooth deck)
        "voussoirCount": 9,      # number of arch blocks
        "voussoirTaper": 0.6,    # 0 = flat slab, 1 = full radial wedge
        "jointGap": 0.04,        # mortar gap between stones
        "keystoneScale": 1.6,    # central stone width multiplier
        # perlin weathering  (noiseOctaves is the master switch; 0 = smooth)
        "bumpHeight": 0.06,      # displacement amplitude (0 = no effect)
        "bumpFreq": 2.0,         # base noise frequency (higher = finer)
        "noiseOctaves": 4,       # fBm detail layers (0 = off, 4-5 = rough)
        "noiseSeed": 0,          # change for a different erosion pattern
        "weatherSubdiv": 8,      # tessellation of weathered voussoir tops (6-12)
    },
)

# ── 3. Frame it ───────────────────────────────────────────────────────────────
cmds.select(bridge_xform)
cmds.viewFit()

print(f"Bridge created: {bridge_xform}  (DG node: {bridge_node})")
print(f"Tweak parameters in the Attribute Editor under '{bridge_node}'.")
