# Features

Detailed feature and attribute reference for the ParametricModelling Maya plug-in.
For project background and the high-level "what it is / how it works", see
[../README.md](../README.md). For build, install, and driving instructions, see
[USAGE.md](USAGE.md).

## Feature summary

- Real-time procedural polygon mesh generation from numeric attributes.
- Custom DG nodes implemented in C++ with the Maya API.
- Chamfered step nosings on both ascending and descending stair runs.
- Parabolic arch profile on the bridge deck, controlled by a single `archHeight` parameter.
- Segmented voussoir arch on the bridge deck — discrete arch blocks with mortar joints, per-block taper, and a widenable keystone (`voussoirCount`, `voussoirTaper`, `jointGap`, `keystoneScale`).
- Fractal Perlin-noise weathering on weathered surfaces — voussoir tops (displaced along the arch normal), the smooth deck strip, and staircase treads. Driven by `bumpHeight`/`bumpFreq` with `noiseOctaves` (roughness) and `noiseSeed` (per-instance variation); `weatherSubdiv` controls tessellation. `noiseOctaves = 0` leaves surfaces smooth.
- Mesh output created as dependency graph data using `MFnMeshData` and `MFnMesh`.
- Native Maya plug-in loading through `loadPlugin`, Plug-in Manager, or a configured plug-in path.

## Nodes

### `parametricStaircaseNode`

Generates a staircase from repeated chamfered steps. Inputs:

| Attribute | Type | Description |
|---|---|---|
| `totalHeight` | double | Total rise of the staircase |
| `stairWidth` | double | Depth-wise width of each step |
| `treadDepth` | double | Horizontal run of each tread |
| `stepCount` | int | Number of steps |
| `chamfer` | double | Nosing bevel size (clamped to prevent degeneracy) |
| `bumpHeight` | double | Amplitude of Perlin tread weathering (0 = flat treads) |
| `bumpFreq` | double | Base noise frequency |
| `noiseOctaves` | int | fBm detail layers; `0` = no weathering |
| `noiseSeed` | int | Seed for per-instance noise variation |
| `weatherSubdiv` | int | Tessellation of the weathered tread face |

Each step is a 10-vertex chamfered box (4 flat quads + a subdivided tread grid + 2 pentagons). A diagonal underside face connects adjacent steps into a solid continuous mesh. The tread (`+Y`) face is built as a `weatherSubdiv`-resolution grid and displaced in `+Y` by fractal Perlin noise, faded to zero on its borders so it stays welded to the surrounding faces.

### `parametricBridgeNode`

Generates a full bridge: ascending stairs on the left, an arc deck, and descending stairs on the right. Inputs:

| Attribute | Type | Description |
|---|---|---|
| `totalHeight` | double | Height of each stair run |
| `stairWidth` | double | Width of the structure |
| `treadDepth` | double | Horizontal run per step |
| `stepCount` | int | Steps on each side |
| `chamfer` | double | Nosing bevel on stair steps |
| `bridgeLength` | double | Length of the flat deck span |
| `deckScale` | double | Multiplier applied to `bridgeLength` |
| `deckSegments` | int | Longitudinal tessellation of the deck |
| `archHeight` | double | Peak rise of the parabolic arch |
| `stepSubdivisions` | int | Grid tessellation on deck-step faces |
| `bumpHeight` | double | Amplitude of Perlin surface weathering |
| `bumpFreq` | double | Base noise frequency |
| `voussoirCount` | int | Number of arch blocks; `≤ 1` falls back to a smooth deck strip |
| `voussoirTaper` | double | Sideways lean of each block, from 0 (straight up) to following the arch normal |
| `jointGap` | double | Width of the mortar joint carved between adjacent voussoirs |
| `keystoneScale` | double | Widens the centre keystone block (odd `voussoirCount` only) |
| `noiseOctaves` | int | fBm detail layers for weathering; `0` = smooth |
| `noiseSeed` | int | Seed for per-instance noise variation |
| `weatherSubdiv` | int | Tessellation of weathered voussoir tops |

The deck follows a parabolic profile `y = base + archHeight × 3t(1−t)`. When `voussoirCount > 1` the span is built as discrete arch blocks (voussoirs) with mortar gaps and an optional widened keystone, rather than a continuous strip. Weathered surfaces are displaced by fractal Perlin noise (fBm): voussoir tops push outward along the arch normal, while the smooth deck strip displaces its top edge. Displacement fades to zero on face borders so subdivided grids stay welded to adjoining flat faces.

## v1 Roadmap

Targeted for the v1 launch:

- **UV mapping** — texturing-ready UVs baked directly into the mesh output; U follows the length axis, V the cross-section.

### Later

- **Stone coursing gaps** — horizontal offset joints between stacked voussoir *rows* on the arch face. The current arch is a single row of blocks; multi-row coursing is not yet implemented.

### Known limitations

- Faces are emitted with their own vertices rather than a shared/welded vertex pool. Weathered grids stay visually seamless because border displacement is faded to zero, but the boundary vertices are coincident-but-distinct, so edge-based mesh operations in Maya will see split edges.
- The smooth deck strip (`voussoirCount ≤ 1`) displaces its top edge without an edge fade, matching the original ripple behavior; its end caps can shift slightly when `noiseOctaves > 0`.
