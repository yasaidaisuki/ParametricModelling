#include "meshBuilder.h"
#include <maya/MPoint.h>

static void appendBox(
    double x0, double y0, double z0,
    double dx, double dy, double dz,
    MPointArray& points,
    MIntArray&   faceCounts,
    MIntArray&   faceConnects)
{
    const int b = static_cast<int>(points.length());

    points.append(MPoint(x0,      y0,      z0));
    points.append(MPoint(x0 + dx, y0,      z0));
    points.append(MPoint(x0 + dx, y0 + dy, z0));
    points.append(MPoint(x0,      y0 + dy, z0));
    points.append(MPoint(x0,      y0,      z0 + dz));
    points.append(MPoint(x0 + dx, y0,      z0 + dz));
    points.append(MPoint(x0 + dx, y0 + dy, z0 + dz));
    points.append(MPoint(x0,      y0 + dy, z0 + dz));

    const int faces[6][4] = {
        {0, 1, 5, 4},  // bottom  -Y
        {3, 7, 6, 2},  // top     +Y
        {0, 4, 7, 3},  // front   -X (riser)
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

    for (int i = 0; i < p.stepCount; ++i)
        appendBox(i * p.treadDepth, i * rh, 0.0,
                  p.treadDepth, rh, p.stairWidth,
                  points, faceCounts, faceConnects);
}

void buildBridgeMesh(
    const BridgeParams& p,
    MPointArray&        points,
    MIntArray&          faceCounts,
    MIntArray&          faceConnects)
{
    points.clear();
    faceCounts.clear();
    faceConnects.clear();

    // TODO: implement bridge geometry using appendBox
    // Suggested structure:
    //   1. Left stairs  — ascending loop  (appendBox × stepCount)
    //   2. Bridge deck  — one flat box    (appendBox × 1)
    //   3. Right stairs — descending loop (appendBox × stepCount)
}
