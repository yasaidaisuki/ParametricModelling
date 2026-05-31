# Custom Parametric Geometry Node for Maya

A C++ Maya plug-in that adds a custom dependency graph node for generating procedural polygon geometry in real time. Users control parameters such as height, width, step count, segment count, and optional curve input, and the node computes a clean polygon mesh through the OpenMaya API. [1][2][3]

## Overview

This project implements a native Maya plug-in in C++ using the Maya Developer Kit and OpenMaya. Maya’s dependency graph plug-in model is built around custom nodes derived from `MPxNode`, where `compute()` defines the node’s behavior and `registerNode()` exposes the new node type to Maya. [1][4][5]

Instead of building a procedural asset inside a higher-level graph tool, this project creates the generator directly at the API level. The node reads artist-friendly attributes, performs geometric calculations in C++, creates mesh data with `MFnMeshData`, and outputs polygon geometry that can participate in Maya’s normal construction-history workflow. [2][6][3]

## Features

- Real-time procedural polygon mesh generation from numeric attributes. [4][7]
- Custom DG node implemented in C++ with the Maya API. [1][8]
- Optional NURBS curve input for path-based generation, such as bridges or ramps; Maya supports curve data objects in DG nodes, and curve values can be read from typed handles with `asNurbsCurve()`. [9][10]
- Mesh output created as dependency graph data using `MFnMeshData` and `MFnMesh`. [2][3]
- Native Maya plug-in loading through `loadPlugin`, Plug-in Manager, or a configured plug-in path; Autodesk documents `loadPlugin` and standard plug-in search behavior through `MAYA_PLUG_IN_PATH`. [11][12][13]

## Project Ideas

### `parametricStaircaseNode`

A staircase generator with inputs such as:

- Total height
- Stair width
- Tread depth
- Step count
- Landing length
- Optional side rail toggle

The node computes repeated treads and risers, assembles the vertex list and face connectivity, and outputs a single polygon mesh. This matches Autodesk’s documented examples of custom nodes that generate polygonal geometry in `compute()`. [14][15]

### `parametricBridgeNode`

A bridge generator with inputs such as:

- Deck width
- Deck thickness
- Segment count
- Rail height
- Support spacing
- Input curve

This version uses a curve as the procedural guide and generates repeated structural segments along it. Maya’s API supports NURBS curve data objects for dependency graph workflows, making curve-driven procedural geometry a natural extension of the node design. [9][16][10]

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

    static MObject inputWidth;
    static MObject inputHeight;
    static MObject inputSegments;
    static MObject inputCurve;
    static MObject outputMesh;
};
```

The plug-in also needs entry points for registration and deregistration. Autodesk’s plug-in documentation describes `initializePlugin()` and `uninitializePlugin()` as the required functions used to register and remove custom functionality from Maya. [5][17][18]

```cpp
MStatus initializePlugin(MObject obj);
MStatus uninitializePlugin(MObject obj);
```

## Project Structure

```text
custom-parametric-node/
├── README.md
├── src/
│   ├── pluginMain.cpp
│   ├── parametricBridgeNode.cpp
│   ├── parametricBridgeNode.h
│   └── meshBuilder.cpp
├── include/
│   └── meshBuilder.h
├── CMakeLists.txt
└── scripts/
    └── create_node.py
```

A structure like this keeps Maya registration code separate from geometry-generation logic. The plug-in entry point handles node registration, while the node source focuses on attributes and `compute()`. [5][1]

## Build

The exact build steps depend on your Maya version, platform, and compiler toolchain, but the common setup is the same: point your project at the Maya SDK headers and libraries, compile a shared library, and produce the platform-specific Maya plug-in binary. Autodesk’s `loadPlugin` documentation notes the expected binary extensions: `.mll` on Windows, `.so` on Linux, and `.bundle` on macOS. [12]

### General build steps

1. Install Maya and the Maya Developer Kit for your target version; Autodesk’s plug-in docs assume version-matched plug-ins are built and loaded into a compatible Maya installation. [5][13]
2. Configure include and library paths for the Maya SDK.
3. Compile your node as a shared plug-in library.
4. Output the resulting binary into a Maya plug-in directory or a folder referenced by `MAYA_PLUG_IN_PATH`; Autodesk documents both Plug-in Manager loading and plug-in path-based discovery. [12][13]

### Example CMake outline

```cmake
cmake_minimum_required(VERSION 3.20)
project(CustomParametricNode)

add_library(CustomParametricNode SHARED
    src/pluginMain.cpp
    src/parametricBridgeNode.cpp
    src/meshBuilder.cpp
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

## Installation

Autodesk documents several ways to install Maya plug-ins, including using the Plug-in Manager, browsing directly to the binary, or placing the plug-in in a directory searched through `MAYA_PLUG_IN_PATH`. [11][12][13]

### Option 1: Plug-in Manager

1. Launch Maya.
2. Open **Window > Settings/Preferences > Plug-in Manager**; Autodesk’s installation docs identify this as the standard UI for loading plug-ins. [13]
3. Browse to your compiled plug-in file.
4. Enable **Loaded** and optionally **Auto Load**; similar Autodesk installation guidance describes loading and auto-loading through the Plug-in Manager. [19][13]

### Option 2: Script Editor

```python
import maya.cmds as cmds
cmds.loadPlugin("/full/path/to/CustomParametricNode.mll")
```

Autodesk documents the `loadPlugin` command for loading plug-ins by name or full path. It also notes Maya’s platform-specific file extensions and plug-in path search behavior. [11][12]

## Usage

Once the plug-in is loaded, Maya can create instances of the custom node type. Autodesk’s DG documentation shows that after a node type is registered, it can be created directly with `createNode("mynode")`. [1][20]

### Create the node

```python
import maya.cmds as cmds
node = cmds.createNode("parametricBridgeNode")
```

### Adjust attributes

```python
cmds.setAttr(f"{node}.width", 6.0)
cmds.setAttr(f"{node}.height", 2.0)
cmds.setAttr(f"{node}.segments", 24)
```

### Connect a curve input

```python
cmds.connectAttr("curveShape1.worldSpace[0]", f"{node}.inputCurve", force=True)
```

For curve-driven generation, the node reads the connected curve data and rebuilds the output mesh when the curve or parameters change. Maya’s DG model is specifically designed so upstream changes dirty dependent outputs and trigger recomputation. [6][8]

### Connect mesh output

In many Maya workflows, the generated mesh is connected to a mesh shape node or wrapped in helper logic that makes the output visible in the viewport. Autodesk’s mesh-producing examples show custom nodes constructing polygon output inside `compute()`. [14][15]

## Example Workflow

1. Load the plug-in into Maya; Autodesk documents `loadPlugin` and Plug-in Manager as standard entry points for plug-in activation. [11][13]
2. Create a `parametricStaircaseNode` or `parametricBridgeNode` instance; registered node types can be instantiated from Maya commands once the plug-in is loaded. [1][20]
3. Adjust numeric inputs such as width, height, or step count.
4. Optionally connect a NURBS curve for path-based generation; Maya supports NURBS curve data in dependency graph nodes. [9][10]
5. Let Maya re-evaluate the node and regenerate the polygon mesh output in real time; this is the expected DG behavior for custom computation nodes. [6][8]

## Future Work

- UV generation for texturing-ready assets.
- Hard-edge and smoothing controls.
- Procedural supports, rails, bolts, or trim pieces.
- Preset profiles for different staircase or bridge styles.
- Multiple outputs, such as render mesh and low-res collision mesh.

These are practical extensions because the custom node already owns the procedural rules and the mesh construction stage. Maya’s mesh data workflow supports building more advanced procedural outputs on top of the same node architecture. [2][3]

## Screenshots

Add images here once the node is running in Maya:

```md
![Viewport screenshot](docs/images/viewport.png)
![Attribute editor](docs/images/attributes.png)
![Wireframe output](docs/images/wireframe.png)
```

## License

Add your preferred license here, such as MIT.

## Short Repo Description

A Maya C++ plug-in that creates a custom dependency graph node for procedural polygon generation, with artist-controlled parameters and optional curve-driven mesh construction through the OpenMaya API. [1][2][10]

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