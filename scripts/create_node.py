"""
Run this in Maya's Script Editor (Python tab) after building the plugin.

IMPORTANT: Set PLUGIN_PATH to the full path of your built .mll file.
Example: r"C:\Users\Dami\...\build\Release\ParametricStaircaseNode.mll"
"""
import maya.cmds as cmds

PLUGIN_PATH = r"C:\Users\Dami\OneDrive\Documents\My projects\ParametricModelling\build\Release\ParametricStaircaseNode.mll"

# ── Load plugin (safe to call if already loaded) ─────────────────────────────
if not cmds.pluginInfo(PLUGIN_PATH, query=True, loaded=True):
    cmds.loadPlugin(PLUGIN_PATH)

# ── Helpers ───────────────────────────────────────────────────────────────────
def _clean(name):
    """Delete a node/transform if it exists from a previous run."""
    if cmds.objExists(name):
        cmds.delete(name)

def _wire_node(node_type, node_name, xform_name, shape_name, attrs):
    """Create a parametric node, wire it to a mesh shape, set initial attrs."""
    _clean(xform_name)
    _clean(node_name)

    node  = cmds.createNode(node_type, name=node_name)
    xform = cmds.createNode("transform", name=xform_name)
    shape = cmds.createNode("mesh", parent=xform, name=shape_name)

    cmds.connectAttr(f"{node}.outMesh", f"{shape}.inMesh", force=True)
    cmds.sets(shape, edit=True, forceElement="initialShadingGroup")

    for attr, value in attrs.items():
        cmds.setAttr(f"{node}.{attr}", value)

    print(f"Created '{node}' connected to '{shape}'")
    return node, shape

# ── Staircase node ────────────────────────────────────────────────────────────
_wire_node(
    "parametricStaircaseNode", "staircase1", "staircaseMesh", "staircaseMeshShape",
    {"totalHeight": 3.0, "stairWidth": 2.0, "treadDepth": 0.3, "stepCount": 10}
)

# ── Bridge node ───────────────────────────────────────────────────────────────
_wire_node(
    "parametricBridgeNode", "bridge1", "bridgeMesh", "bridgeMeshShape",
    {"totalHeight": 3.0, "stairWidth": 2.0, "treadDepth": 0.3, "stepCount": 10, "bridgeLength": 5.0}
)
