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

static void appendArcDeck(
    double xStart, double yBase, double width, double thickness,
    double length, double archHeight, int segments,
    MPointArray& points,
    MIntArray&   faceCounts,
    MIntArray&   faceConnects)
{
    const int base = static_cast<int>(points.length());
    const int N    = segments;

    for (int i = 0; i <= N; ++i) {
        const double t = (double)i / N;
        const double x = xStart + t * length;
        const double y = yBase + archHeight * 3.0 * t * (1.0 - t);
        points.append(MPoint(x, y,             0.0  ));  // v0 bottom near
        points.append(MPoint(x, y + thickness, 0.0  ));  // v1 top near
        points.append(MPoint(x, y,             width));  // v2 bottom far
        points.append(MPoint(x, y + thickness, width));  // v3 top far
    }

    for (int i = 0; i < N; ++i) {
        const int a = base + i*4+0, b = base + i*4+1;
        const int c = base + i*4+2, d = base + i*4+3;
        const int e = base + (i+1)*4+0, f = base + (i+1)*4+1;
        const int g = base + (i+1)*4+2, h = base + (i+1)*4+3;

        const int strip[4][4] = {
            {a, c, g, e},  // bottom -Y
            {b, f, h, d},  // top    +Y
            {a, b, f, e},  // near   -Z
            {c, g, h, d},  // far    +Z
        };
        for (const auto& face : strip) {
            faceCounts.append(4);
            for (int v : face) faceConnects.append(v);
        }
    }

    // end caps
    const int a0 = base,      b0 = base+1, c0 = base+2, d0 = base+3;
    const int aN = base+N*4,  bN = aN+1,   cN = aN+2,   dN = aN+3;
    faceCounts.append(4); faceConnects.append(a0); faceConnects.append(c0); faceConnects.append(d0); faceConnects.append(b0);
    faceCounts.append(4); faceConnects.append(aN); faceConnects.append(bN); faceConnects.append(dN); faceConnects.append(cN);
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

    //   2. Bridge deck (arc)
    appendArcDeck(p.stepCount*p.treadDepth, (p.stepCount - 1) * rh, p.stairWidth, rh,
                  p.bridgeLength, p.archHeight, p.deckSegments,
                  points, faceCounts, faceConnects);

    //   3. Right stairs 
    for (int i = 0; i < p.stepCount; i++) {
        appendBox(p.stepCount*p.treadDepth + p.bridgeLength + i*p.treadDepth, 
                  (p.stepCount - 1 - i) * rh, 0.0,
                  p.treadDepth, rh, p.stairWidth,
                  points, faceCounts, faceConnects);
    }
}
