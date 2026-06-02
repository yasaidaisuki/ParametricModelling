"""
Create both parametric nodes (a standalone staircase and a bridge) in Maya,
each wired to a visible mesh.

Run in the Maya Script Editor (Python tab), or:
    exec(open(r"<path>/create_node.py").read())

Plug-in loading/reloading is handled by parametric_plugin.reload_plugin(), which
always picks up a fresh build and verifies both node types registered.
"""

import os
import sys

# Make the shared helper importable no matter how this file is executed.
_HERE = os.path.dirname(os.path.abspath(__file__)) if "__file__" in globals() \
    else os.path.dirname(r"C:\Users\Dami\OneDrive\Documents\My projects\ParametricModelling\scripts\create_node.py")
if _HERE not in sys.path:
    sys.path.insert(0, _HERE)

import parametric_plugin as pp

# ── 1. Clean reload (default = Release build) ─────────────────────────────────
pp.reload_plugin()

# ── 2. Staircase node ─────────────────────────────────────────────────────────
# Perlin tread weathering: noiseOctaves is the master switch (0 = smooth) and
# bumpHeight must be non-zero to see it. Treads are small flat faces, so a small
# amplitude + higher frequency reads as worn stone rather than big lumps.
pp.build_mesh(
    "parametricStaircaseNode", "staircase1", "staircaseMesh", "staircaseMeshShape",
    {
        "totalHeight": 3.0, "stairWidth": 2.0, "treadDepth": 0.3, "stepCount": 10,
        "bumpHeight": 0.015,     # displacement amplitude (0 = flat treads)
        "bumpFreq": 6.0,         # base noise frequency (higher = finer pitting)
        "noiseOctaves": 4,       # fBm detail layers (0 = off)
        "noiseSeed": 0,          # change for a different erosion pattern
        "weatherSubdiv": 8,      # tessellation of the weathered tread face
    },
)

# ── 3. Bridge node ────────────────────────────────────────────────────────────
# Same weathering controls; here the noise displaces the voussoir tops along the
# arch normal (and the smooth deck strip when voussoirCount <= 1).
pp.build_mesh(
    "parametricBridgeNode", "bridge1", "bridgeMesh", "bridgeMeshShape",
    {
        "totalHeight": 3.0, "stairWidth": 2.0, "treadDepth": 0.3, "stepCount": 10,
        "bridgeLength": 5.0,
        "bumpHeight": 0.06,      # displacement amplitude (0 = flat)
        "bumpFreq": 2.0,         # base noise frequency
        "noiseOctaves": 4,       # fBm detail layers (0 = off, 4-5 = rough)
        "noiseSeed": 0,          # change for a different erosion pattern
        "weatherSubdiv": 8,      # tessellation of weathered surfaces
    },
)

print("Done. Tweak parameters in the Attribute Editor under 'staircase1' / 'bridge1'.")
