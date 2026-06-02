# Usage

How to build, install, and drive the ParametricModelling Maya plug-in. The plug-in
registers two custom DG nodes — `parametricStaircaseNode` and `parametricBridgeNode` —
that generate procedural polygon meshes from numeric attributes.

This guide targets **Maya 2027** (the SDK and Maya version the project is developed and
tested against). Maya plug-ins are version-coupled: a `.mll` built against the 2027
DevKit will only load in Maya 2027. To target another version, build against that
version's DevKit.

---

## 1. Prerequisites

| Requirement | Notes |
|---|---|
| Maya 2027 | The application you load the plug-in into. |
| Maya 2027 DevKit | Headers + libs (`Foundation.lib`, `OpenMaya.lib`, `OpenMayaUI.lib`). Download from the [Maya Developer Center](https://aps.autodesk.com/developer/overview/maya-api). |
| Visual Studio 2022 (MSVC) | Maya 2027 plug-ins build with the MSVC toolset; C++17. |
| CMake ≥ 3.20 | Build system generator. |

The DevKit unzips to a `devkitBase/` folder containing `include/` and `lib/`. Point the
build at it with the `MAYA_LOCATION` CMake variable (default in
[CMakeLists.txt](../CMakeLists.txt) is `C:/Program Files/Maya_SDK/devkitBase`).

---

## 2. Build (Windows)

From the project root:

```powershell
# Configure (override MAYA_LOCATION if your DevKit lives elsewhere)
cmake -B build -DMAYA_LOCATION="C:/path/to/devkitBase"

# Build the Release plug-in
cmake --build build --config Release
```

The result is:

```
build/Release/ParametricStaircaseNode.mll
```

Despite the file name, this single `.mll` registers **both** node types
(`parametricStaircaseNode` and `parametricBridgeNode`) — see
[src/pluginMain.cpp](../src/pluginMain.cpp).

For a debug build, use `--config Debug`; the output lands in `build/Debug/`.

> **Runtime note:** Maya requires the multi-threaded **DLL** C runtime (`/MD`). The
> CMake config sets this; don't switch it to `/MT` or `/MTd` or the plug-in won't load.

---

## 3. Install / load the plug-in

You have three standard options (Autodesk documents all three):

### Option A — Plug-in Manager (GUI)

1. Launch Maya.
2. **Windows → Settings/Preferences → Plug-in Manager**.
3. **Browse** to `build/Release/ParametricStaircaseNode.mll`.
4. Tick **Loaded** (and optionally **Auto Load**).

### Option B — Script Editor (Python)

```python
import maya.cmds as cmds
cmds.loadPlugin(r"C:\path\to\ParametricModelling\build\Release\ParametricStaircaseNode.mll")
```

### Option C — `MAYA_PLUG_IN_PATH`

Copy the `.mll` into any directory on `MAYA_PLUG_IN_PATH` (or a Maya `plug-ins`
folder), then load it by name:

```python
cmds.loadPlugin("ParametricStaircaseNode")
```

### Clean reload during development

When iterating on the C++ and rebuilding, a naive reload often fails because Maya
won't unload a plug-in while any of its nodes still exist, or because a stale build
from an earlier session still owns the node type IDs. Use the project helper, which
deletes existing nodes, force-unloads every owning plug-in, reloads the fresh build,
and verifies both node types registered:

```python
import sys
sys.path.insert(0, r"C:\path\to\ParametricModelling\scripts")

import parametric_plugin as pp
pp.reload_plugin()                      # loads build/Release/...mll by default
# pp.reload_plugin(r"...\build\Debug\ParametricStaircaseNode.mll")   # debug build
```

See [scripts/parametric_plugin.py](../scripts/parametric_plugin.py) for the full
lifecycle logic.

---

## 4. Create and drive a node

### Quick start — the demo script

The fastest way to see geometry is the bundled demo, which reloads the plug-in,
creates a bridge wired to a visible mesh shape, sets a full parameter set, and frames
it in the viewport. Run from the Maya Script Editor (Python tab):

```python
exec(open(r"C:\path\to\ParametricModelling\scripts\create_bridge.py").read())
```

See [scripts/create_bridge.py](../scripts/create_bridge.py).

### Manual creation

```python
import maya.cmds as cmds

# Create the DG node
node = cmds.createNode("parametricBridgeNode")

# Wire its mesh output into a visible mesh shape
xform = cmds.createNode("transform", name="bridge1")
shape = cmds.createNode("mesh", parent=xform, name="bridgeShape1")
cmds.connectAttr(f"{node}.outMesh", f"{shape}.inMesh", force=True)
cmds.sets(shape, edit=True, forceElement="initialShadingGroup")

# Set parameters — Maya re-evaluates and regenerates the mesh on each change
cmds.setAttr(f"{node}.totalHeight",  2.0)
cmds.setAttr(f"{node}.stairWidth",   2.0)
cmds.setAttr(f"{node}.bridgeLength", 8.0)
cmds.setAttr(f"{node}.archHeight",   1.5)
cmds.setAttr(f"{node}.deckSegments", 16)
```

The reusable `pp.build_mesh(node_type, node_name, xform_name, shape_name, attrs)`
helper in [scripts/parametric_plugin.py](../scripts/parametric_plugin.py) does all of the
above idempotently (it removes same-named nodes first and skips attributes the current
build doesn't expose).

---

## 5. Node reference

The two node types and their full attribute tables are documented in
[FEATURES.md](FEATURES.md):

- [`parametricStaircaseNode`](FEATURES.md#parametricstaircasenode) — a staircase built
  from repeated chamfered steps.
- [`parametricBridgeNode`](FEATURES.md#parametricbridgenode) — ascending stairs, an
  arc/voussoir deck, and descending stairs.

Set any of these attributes with `cmds.setAttr(f"{node}.<attribute>", value)`, as shown
in [§4](#4-create-and-drive-a-node).

---

## 6. Releases (prebuilt `.mll`)

Tagged releases attach a prebuilt `ParametricStaircaseNode.mll` on the
[Releases page](../../releases), built by CI against the Maya 2027 DevKit. Download the
asset and load it via any method in [§3](#3-install--load-the-plug-in) — no local build
or DevKit required, as long as your Maya version matches the one the release was built
against (currently Maya 2027).

To cut a release, push a version tag:

```powershell
git tag v1.0.0
git push origin v1.0.0
```

The CI workflow ([.github/workflows/release.yml](../.github/workflows/release.yml))
downloads the Maya 2027.1 DevKit from Autodesk's public CDN, builds Release, and
creates a GitHub Release with the `.mll` attached. A manual **Run workflow** dispatch
builds and uploads the `.mll` as an artifact without cutting a release — useful for
testing the pipeline. To target a different Maya version, set a `MAYA_DEVKIT_URL`
repository secret to that version's DevKit download URL (overrides the default).

See [README.md](../README.md) for project background, and
[FEATURES.md](FEATURES.md) for the full feature and attribute reference.

---

## 7. Troubleshooting

| Symptom | Likely cause / fix |
|---|---|
| `loadPlugin` fails with an API version error | The `.mll` was built against a different Maya version. Rebuild against your Maya's DevKit. |
| Node types don't register after load | A stale plug-in from an earlier session still owns the type IDs. Use `pp.reload_plugin()`, or restart Maya. |
| Plug-in won't load at all (no clear error) | Built with `/MT` instead of `/MD`, or against a mismatched runtime. Rebuild Release with the project's CMake config. |
| `cmake` can't find `Foundation.lib` / `OpenMaya.lib` | `MAYA_LOCATION` doesn't point at a folder with `lib/`. Pass `-DMAYA_LOCATION=".../devkitBase"`. |
| Mesh appears but looks black | The shape isn't assigned to a shading group. The demo/helper assigns `initialShadingGroup` for you. |
