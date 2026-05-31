#include "meshBuilder.h"
#include <maya/MPoint.h>

void buildStaircaseMesh(
    const StaircaseParams& p,
    MPointArray&           points,
    MIntArray&             faceCounts,
    MIntArray&             faceConnects)
{
    points.clear();
    faceCounts.clear();
    faceConnects.clear();

    if (p.stepCount <= 0) return;

    const double rh = p.totalHeight / p.stepCount;
    const double td = p.treadDepth;
    const double sw = p.stairWidth;

    for (int i = 0; i < p.stepCount; ++i) {
        const double x0 = i * td;
        const double y0 = i * rh;

        // 8 corners of this step's box
        points.append(MPoint(x0,      y0,      0));   // 0 front-bottom-left
        points.append(MPoint(x0 + td, y0,      0));   // 1 back-bottom-left
        points.append(MPoint(x0 + td, y0 + rh, 0));   // 2 back-top-left
        points.append(MPoint(x0,      y0 + rh, 0));   // 3 front-top-left
        points.append(MPoint(x0,      y0,      sw));  // 4 front-bottom-right
        points.append(MPoint(x0 + td, y0,      sw));  // 5 back-bottom-right
        points.append(MPoint(x0 + td, y0 + rh, sw));  // 6 back-top-right
        points.append(MPoint(x0,      y0 + rh, sw));  // 7 front-top-right

        const int b = i * 8;

        // 6 quads with outward CCW winding (verified via cross-product)
        const int faces[6][4] = {
            {0, 1, 5, 4},  // bottom  -Y
            {3, 7, 6, 2},  // top     +Y
            {0, 4, 7, 3},  // front   -X (riser face)
            {1, 2, 6, 5},  // back    +X
            {0, 3, 2, 1},  // left    -Z
            {4, 5, 6, 7},  // right   +Z
        };

        for (const auto& face : faces) {
            faceCounts.append(4);
            for (int v : face)
                faceConnects.append(b + v);
        }
    }
}
