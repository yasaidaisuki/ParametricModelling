Implement the geometry for a `build<Name>Mesh` function in `src/meshBuilder.cpp`.

If the user didn't specify which function to implement, ask them which mesh builder they want to implement (e.g. `buildBridgeMesh`).

## Project conventions to follow

**Coordinate system:** X = depth/length, Y = height, Z = width.

**`appendBox` helper** (already defined as `static` at the top of `meshBuilder.cpp`):
```cpp
appendBox(x0, y0, z0,   // origin corner
          dx, dy, dz,   // dimensions
          points, faceCounts, faceConnects);
```
It appends 8 vertices and 6 quads (CCW winding, outward normals) to the arrays. The base index is computed automatically from `points.length()` so boxes can be appended in any order without manual offset tracking.

**Winding order:** counter-clockwise from outside = outward normal. Maya computes smooth normals from winding automatically.

**Face arrays:** `faceCounts` gets one `4` per quad; `faceConnects` gets the 4 vertex indices per face in CCW order — both appended via `appendBox`.

## Steps

1. Read the current stub and the `<Name>Params` struct in `include/meshBuilder.h` to understand available parameters.
2. Ask the user to describe the shape if it isn't obvious from the function name and params — e.g. "ascending stairs on the left, flat deck in the middle, descending stairs on the right."
3. Plan the geometry as a sequence of `appendBox` calls. For repeated shapes (e.g. stair steps), use a loop. For one-off shapes (e.g. a flat deck), call `appendBox` directly.
4. Implement the function body, replacing the TODO stub. Do not add any code outside `meshBuilder.cpp` and `meshBuilder.h`.
5. After writing, summarize: total vertex count, total face count, and the X extent of the final mesh given the default parameter values.
