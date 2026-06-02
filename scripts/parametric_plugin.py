"""
Shared plugin lifecycle helpers for the ParametricModelling Maya plug-in.

The demo scripts (create_bridge.py, create_node.py) all go through reload_plugin()
so they behave identically and survive the common pitfalls of iterating on a
native plug-in inside a long-running Maya session:

  * Maya refuses to unload a plug-in while any node it defines still exists, so a
    naive unload silently fails and leaves a STALE build resident. We delete all
    nodes of our custom types (by type, not by hard-coded name) first.
  * A node TYPE can be pinned by a plug-in loaded under a different file name in a
    previous session, so its MTypeId stays in use and a fresh load can't
    re-register it. We unload EVERY loaded plug-in that registers our types.
  * loadPlugin can appear to succeed while only some nodes registered. We verify
    the expected node types are present after loading and raise if not.

Usage from a demo script:

    import parametric_plugin as pp
    pp.reload_plugin()                 # uses the default Release path
    pp.reload_plugin(r"C:\\...\\Debug\\ParametricStaircaseNode.mll")
"""

import os
import maya.cmds as cmds

# The node types this plug-in registers. Used to find/clean stale instances and
# to verify a successful load.
NODE_TYPES = ("parametricStaircaseNode", "parametricBridgeNode")

# Default build to load. Override by passing a path to reload_plugin().
DEFAULT_PLUGIN_PATH = os.path.join(
    os.path.dirname(os.path.dirname(os.path.abspath(__file__))),
    "build", "Release", "ParametricStaircaseNode.mll",
)


def _plugins_owning_our_types():
    """Names of all loaded plug-ins that register any of our node types."""
    owners = []
    for p in (cmds.pluginInfo(query=True, listPlugins=True) or []):
        registered = cmds.pluginInfo(p, query=True, dependNode=True) or []
        if any(t in registered for t in NODE_TYPES):
            owners.append(p)
    return owners


def delete_our_nodes():
    """Delete every node of our custom types so plug-ins can be unloaded."""
    doomed = []
    for t in NODE_TYPES:
        doomed += cmds.ls(type=t) or []
    if doomed:
        cmds.delete(doomed)
    # Undo history also holds references that pin a plug-in; clear it so the
    # subsequent unload can actually succeed.
    cmds.flushUndo()
    return doomed


def unload_all(verbose=True):
    """Delete our nodes, then force-unload every plug-in registering our types.

    Returns the list of plug-in names that were unloaded. After this, none of
    NODE_TYPES should remain known to Maya; if one does, a plug-in we couldn't
    identify still owns it (reported via a warning).
    """
    delete_our_nodes()

    unloaded = []
    for p in _plugins_owning_our_types():
        if verbose:
            print("unloading plug-in:", p)
        cmds.unloadPlugin(p, force=True)
        unloaded.append(p)

    known = cmds.allNodeTypes() or []
    still = [t for t in NODE_TYPES if t in known]
    if still:
        cmds.warning(
            "These node types are still registered after unloading known plug-ins: "
            f"{still}. A plug-in loaded under an unexpected name still owns them. "
            "Restart Maya to clear it, then re-run."
        )
    return unloaded


def reload_plugin(plugin_path=None, verbose=True):
    """Cleanly reload the plug-in from `plugin_path` (default: Release build).

    Always unloads first so a fresh build is picked up, then verifies that every
    type in NODE_TYPES registered. Raises RuntimeError with a clear message if
    the binary is missing or a node type failed to register.
    """
    path = plugin_path or DEFAULT_PLUGIN_PATH
    if not os.path.isfile(path):
        raise RuntimeError(
            f"Plug-in not found: {path}\n"
            "Build it first (cmake --build build --config Release) or pass the "
            "correct path to reload_plugin()."
        )

    unload_all(verbose=verbose)

    cmds.loadPlugin(path)
    if verbose:
        print("loaded plug-in:", path)

    # Verify the load actually exposed every node type we expect.
    known = cmds.allNodeTypes() or []
    missing = [t for t in NODE_TYPES if t not in known]
    if missing:
        raise RuntimeError(
            f"Plug-in loaded from {path} but these node types did not register: "
            f"{missing}. This usually means their MTypeId is still held by a stale "
            "plug-in from an earlier session. Restart Maya and re-run."
        )
    return path


def build_mesh(node_type, node_name, xform_name, shape_name, attrs):
    """Create a parametric node, wire outMesh -> a visible mesh shape, set attrs.

    Existing nodes/transforms with the same names are removed first so re-running
    is idempotent. Unknown attribute names are reported rather than aborting the
    whole build, so a script that lists an attribute the current build lacks
    fails with a precise message.
    """
    for name in (xform_name, shape_name, node_name):
        if cmds.objExists(name):
            cmds.delete(name)

    node  = cmds.createNode(node_type, name=node_name)
    xform = cmds.createNode("transform", name=xform_name)
    shape = cmds.createNode("mesh", parent=xform, name=shape_name)

    cmds.connectAttr(f"{node}.outMesh", f"{shape}.inMesh", force=True)
    cmds.sets(shape, edit=True, forceElement="initialShadingGroup")

    available = set(cmds.listAttr(node) or [])
    for attr, value in attrs.items():
        if attr not in available:
            cmds.warning(f"{node_type} has no attribute '{attr}' in this build; skipping.")
            continue
        cmds.setAttr(f"{node}.{attr}", value)

    print(f"created '{node}' -> '{shape}'")
    return node, xform, shape
