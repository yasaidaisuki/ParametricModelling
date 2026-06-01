Scaffold a complete new parametric Maya DG node for this project.

If the user didn't provide a node name and parameter list in their message, ask for:
- Node name (e.g. "Arch", "Wall", "Column")
- Parameters: for each one, the name, C++ type (double or int), default value, and min (and max if int)

Then implement ALL of the following in one pass — do not ask for confirmation between steps:

## Files to create

**`src/parametric<Name>Node.h`**
Copy the structure of `src/parametricBridgeNode.h` exactly. Change the class name, declare a `static MObject` for every input parameter plus `outMesh`, and set `MTypeId id` to the next available ID (check `pluginMain.cpp` for the highest registered ID and increment by 1).

**`src/parametric<Name>Node.cpp`**
Copy the structure of `src/parametricBridgeNode.cpp`. In `initialize()`, register each parameter using `MFnNumericAttribute` with the correct type (`kDouble` or `kInt`), default, min, max, and `setKeyable(true)`. Add one `attributeAffects(inX, outMesh)` per input. In `compute()`, pack all inputs into the params struct and call `build<Name>Mesh()`. Keep the `points.length() > 0` guard before `MFnMesh::create()`.

## Files to modify

**`include/meshBuilder.h`**
Add a `<Name>Params` struct with all the parameters, and declare:
```cpp
void build<Name>Mesh(
    const <Name>Params& params,
    MPointArray&        points,
    MIntArray&          faceCounts,
    MIntArray&          faceConnects
);
```

**`src/meshBuilder.cpp`**
Add a stub `build<Name>Mesh` that clears the three arrays and returns, with a `// TODO: implement geometry using appendBox` comment. The `appendBox` helper is already defined at the top of this file — remind the user they can use it.

**`src/pluginMain.cpp`**
Add `#include "parametric<Name>Node.h"`, a `plugin.registerNode(...)` call in `initializePlugin`, and a `plugin.deregisterNode(...)` call in `uninitializePlugin`.

**`CMakeLists.txt`**
Add `src/parametric<Name>Node.cpp` to the `add_library` source list.

**`scripts/create_node.py`**
Add a `_wire_node(...)` call for the new node at the bottom, with all parameters set to their default values.

## After implementing
Tell the user:
1. Rebuild with CMake to pick up the new source file
2. The node will appear in Maya but produce an empty mesh until `build<Name>Mesh` is implemented
3. Use `/implement-mesh` to implement the geometry
