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

    points.append(MPoint(x0,      y0,      z0     )); // [0] front lower-left
    points.append(MPoint(x0 + dx, y0,      z0     )); // [1] back  lower-left
    points.append(MPoint(x0 + dx, y0 + dy, z0     )); // [2] back  upper-left
    points.append(MPoint(x0,      y0 + dy, z0     )); // [3] front upper-left
    points.append(MPoint(x0,      y0,      z0 + dz)); // [4] front lower-right
    points.append(MPoint(x0 + dx, y0,      z0 + dz)); // [5] back  lower-right
    points.append(MPoint(x0 + dx, y0 + dy, z0 + dz)); // [6] back  upper-right
    points.append(MPoint(x0,      y0 + dy, z0 + dz)); // [7] front upper-right

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

    if (p.stepCount <= 0) return; 

    const double rh = p.totalHeight / p.stepCount;

    //   1. Left stairs
    for (int i = 0; i < p.stepCount; i++)
        appendBox(i * p.treadDepth, i * rh, 0.0,
                  p.treadDepth, rh, p.stairWidth,
                  points, faceCounts, faceConnects);

    //   2. Bridge deck
    appendBox(p.stepCount*p.treadDepth, p.stepCount * rh, 0.0, 
              p.bridgeLength, rh, p.stairWidth,
              points, faceCounts, faceConnects);

    //   3. Right stairs 
    for (int i = 0; i < p.stepCount; i++) {
        appendBox(p.stepCount*p.treadDepth + p.bridgeLength + i*p.treadDepth, 
                  (p.stepCount - 1 - i) * rh, 0.0,
                  p.treadDepth, rh, p.stairWidth,
                  points, faceCounts, faceConnects);
    }
}
