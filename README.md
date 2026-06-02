# Custom Parametric Geometry Node for Maya

A C++ Maya plug-in that adds custom dependency graph nodes for generating procedural
polygon geometry in real time. Users control numeric parameters — height, width, step
count, segment count, arch height, surface displacement, and more — and the node
computes a clean polygon mesh through the OpenMaya API. [1][2][3]

📖 **[Features & node reference](docs/FEATURES.md)** · 🛠️ **[Build & usage guide](docs/USAGE.md)**

**Demo Video:**
[![Watch the video](https://youtu.be/WY59ia8HdZk)](https://youtu.be/WY59ia8HdZk)

## Overview

This project implements a native Maya plug-in in C++ using the Maya Developer Kit and OpenMaya. Maya’s dependency graph plug-in model is built around custom nodes derived from `MPxNode`, where `compute()` defines the node’s behavior and `registerNode()` exposes the new node type to Maya. [1][4][5]

Instead of building a procedural asset inside a higher-level graph tool, this project creates the generator directly at the API level. The node reads artist-friendly attributes, performs geometric calculations in C++, creates mesh data with `MFnMeshData`, and outputs polygon geometry that can participate in Maya’s normal construction-history workflow. [2][6][3]

The plug-in ships two node types — a parametric staircase and a parametric bridge with
a parabolic/voussoir arch deck — both with Perlin-noise surface weathering. For the full
feature list, per-node attribute tables, and known limitations, see
**[docs/FEATURES.md](docs/FEATURES.md)**.

## Tech Stack

| Component | Purpose |
|---|---|
| C++ | Core implementation language for the plug-in. [1] |
| Maya Developer Kit | Provides headers, libraries, and plug-in infrastructure for Maya development. [5][13] |
| OpenMaya API | Supplies node, attribute, mesh, and data APIs such as `MPxNode`, `MFnMeshData`, and `MFnMesh`. [4][2][3] |
| Maya Dependency Graph | Evaluates the custom node when inputs change and the output mesh becomes dirty. [6][8] |

## How It Works

The node class derives from `MPxNode`, defines static input and output attributes, and registers dependency relationships with `attributeAffects()` so Maya knows which outputs must be recomputed when an input changes. Autodesk’s DG documentation describes this model as the standard pattern for custom dependency nodes. [4][7][8]

When the output mesh plug is evaluated, Maya calls `compute()`. The node reads values from the datablock, performs procedural math, creates a fresh mesh data object with `MFnMeshData`, constructs the polygon mesh with `MFnMesh`, and assigns the result to the output plug. [2][3]

### Typical node skeleton

```cpp
class ParametricBridgeNode : public MPxNode {
public:
    ParametricBridgeNode() = default;
    ~ParametricBridgeNode() override = default;

    static void* creator();
    static MStatus initialize();
    MStatus compute(const MPlug& plug, MDataBlock& data) override;

    static MTypeId id;

    static MObject inTotalHeight;
    static MObject inStairWidth;
    static MObject inTreadDepth;
    static MObject inStepCount;
    static MObject inBridgeLength;
    static MObject inArchHeight;
    static MObject inBumpHeight;
    static MObject inBumpFreq;
    static MObject outMesh;
};
```

The plug-in also needs entry points for registration and deregistration. Autodesk’s plug-in documentation describes `initializePlugin()` and `uninitializePlugin()` as the required functions used to register and remove custom functionality from Maya. [5][17][18]

```cpp
MStatus initializePlugin(MObject obj);
MStatus uninitializePlugin(MObject obj);
```

## Project Structure

```text
ParametricModelling/
├── README.md
├── docs/
│   ├── FEATURES.md             # feature list + per-node attribute reference
│   └── USAGE.md                # build, install, and driving the nodes
├── CMakeLists.txt
├── src/
│   ├── pluginMain.cpp
│   ├── parametricBridgeNode.cpp / .h
│   ├── parametricStaircaseNode.cpp / .h
│   └── meshes/
│       ├── bridgeMesh.cpp
│       ├── bridgeStairs.cpp
│       ├── bridgeDeck.cpp        # arc deck + voussoir arch (Perlin-weathered)
│       ├── staircaseMesh.cpp     # chamfered steps (Perlin-weathered treads)
│       ├── meshPrimitives.cpp
│       └── noise.cpp             # Perlin + fBm weathering noise
├── include/
│   ├── bridgeMesh.h
│   ├── bridgeStairs.h
│   ├── bridgeDeck.h
│   ├── staircaseMesh.h
│   ├── meshPrimitives.h
│   └── noise.h
└── scripts/
    └── create_bridge.py
```

A structure like this keeps Maya registration code separate from geometry-generation logic. The plug-in entry point handles node registration, while the node source focuses on attributes and `compute()`. [5][1]

## Build & Install

The detailed, version-specific guide — prerequisites, a Windows CMake build, three load
options, and troubleshooting — lives in **[docs/USAGE.md](docs/USAGE.md)**. The general
shape of any Maya plug-in build is: point your project at the Maya SDK headers and
libraries, compile a shared library, and produce the platform-specific Maya plug-in
binary. Autodesk’s `loadPlugin` documentation notes the expected binary extensions:
`.mll` on Windows, `.so` on Linux, and `.bundle` on macOS. [12]

### General build steps

1. Install Maya and the Maya Developer Kit for your target version; Autodesk’s plug-in docs assume version-matched plug-ins are built and loaded into a compatible Maya installation. [5][13]
2. Configure include and library paths for the Maya SDK.
3. Compile your node as a shared plug-in library.
4. Output the resulting binary into a Maya plug-in directory or a folder referenced by `MAYA_PLUG_IN_PATH`; Autodesk documents both Plug-in Manager loading and plug-in path-based discovery. [12][13]

### Example CMake outline

```cmake
cmake_minimum_required(VERSION 3.20)
project(CustomParametricNode)

add_library(ParametricStaircaseNode SHARED
    src/pluginMain.cpp
    src/parametricBridgeNode.cpp
    src/parametricStaircaseNode.cpp
    src/bridgeMesh.cpp
    src/staircaseMesh.cpp
)

target_include_directories(CustomParametricNode PRIVATE
    ${MAYA_INCLUDE_DIR}
    include
)

target_link_libraries(CustomParametricNode
    ${MAYA_LIBRARIES}
)
```

This is a starter outline, not a drop-in cross-platform solution. Maya plug-in builds typically require version-specific SDK paths, compiler settings, and platform naming conventions. [12][13]

## Usage

Once the plug-in is loaded, Maya can create instances of the custom node type. After a
node type is registered, it can be created directly with `createNode("parametricBridgeNode")`,
wired to a visible mesh shape, and driven by `setAttr`. The full create-and-drive
walkthrough, the bundled demo script, and a node/attribute reference are in
**[docs/USAGE.md](docs/USAGE.md)** and **[docs/FEATURES.md](docs/FEATURES.md)**. [1][20]

```python
import maya.cmds as cmds
node = cmds.createNode("parametricBridgeNode")
cmds.setAttr(f"{node}.archHeight", 1.5)
```

## Screenshots

Add images here once the node is running in Maya:

```md
![Viewport screenshot](docs/images/viewport.png)
![Attribute editor](docs/images/attributes.png)
![Wireframe output](docs/images/wireframe.png)
```

## Short Repo Description

A Maya C++ plug-in implementing two custom dependency graph nodes — a parametric staircase and a bridge with a parabolic arch deck — built directly against the OpenMaya API with artist-controlled attributes and real-time mesh recomputation. [1][2][3]

## References

- <https://download.autodesk.com/us/maya/2011help/api/class_m_px_node.html>
- <https://download.autodesk.com/us/maya/2011help/api/class_m_fn_mesh_data.html>
- <https://download.autodesk.com/us/maya/2010help/api/class_m_fn_mesh.html>
- <https://download.autodesk.com/global/docs/maya2014/en_us/files/Nodes_and_attributes_Dependency_graph.htm>
- <https://help.autodesk.com/cloudhelp/2018/ENU/Maya-SDK/files-to-wrap/GUID-A9070270-9B5D-4511-8012-BC948149884D.htm>
- <https://help.autodesk.com/cloudhelp/2018/ENU/Maya-SDK/files-to-wrap/Dependency_graph_plugins_MPxNode_and_its_derived_classes.htm>
- <https://help.autodesk.com/view/MAYADEV/2025/ENU/?guid=Maya_DEVHELP_Maya_Python_API_PluginEntryPoints_html>
- <https://help.autodesk.com/cloudhelp/2016/ENU/Maya-SDK/files/Command_plugins_initializePlugin.htm>
- <https://download.autodesk.com/global/docs/Mayasdk2012/en_us/files/Command_plugins_uninitializePlugin.htm>
- <https://help.autodesk.com/cloudhelp/2024/ENU/Maya-Tech-Docs/Commands/loadPlugin.html>
- <https://download.autodesk.com/us/maya/2011help/CommandsPython/loadPlugin.html>
- <https://help.autodesk.com/cloudhelp/2018/ENU/Maya-SDK/files-to-wrap/GUID-FA51BD26-86F3-4F41-9486-2C3CF52B9E17.htm>
- <https://download.autodesk.com/us/maya/2010help/api/class_m_fn_nurbs_curve_data.html>
