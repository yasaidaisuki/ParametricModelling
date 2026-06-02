#pragma once
#include <maya/MPointArray.h>
#include <maya/MIntArray.h>

// One flight of bridge stairs: `stepCount` chamfered-box steps plus the slanted
// underside connectors that smooth the gap between consecutive steps. Both
// builders append to the supplied arrays from their current length and lay out
// exactly 10 vertices per step (see meshPrimitives appendChamferedBox*).

// Ascending (left) flight. Step i sits at x = xStart + i*treadDepth and rises by
// `riserHeight` per step. The slant connectors replace each step's underside, so
// the flat bottom is kept only on step 0 and the back face only on the last step.
void appendBridgeStairsLeft(
    double xStart, double riserHeight,
    int stepCount, double treadDepth, double stairWidth, double chamfer,
    MPointArray& points,
    MIntArray&   faceCounts,
    MIntArray&   faceConnects);

// Descending (right) flight — mirror of the left flight, chamfered on the
// back-top corner. Step i sits at x = xStart + i*treadDepth and drops by
// `riserHeight` per step. The flat bottom is kept only on the last (lowest) step
// and the front face only on step 0.
void appendBridgeStairsRight(
    double xStart, double riserHeight,
    int stepCount, double treadDepth, double stairWidth, double chamfer,
    MPointArray& points,
    MIntArray&   faceCounts,
    MIntArray&   faceConnects);
