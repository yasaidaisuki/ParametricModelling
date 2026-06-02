#include "staircaseMesh.h"
#include <maya/MPoint.h>
#include <cmath>

// 10 vertices always appended (indices [0]-[9]).  Pass includeBottom=false to
// omit the horizontal bottom face so slanted inter-step connectors can replace it.
static void appendChamferedBox(
    double x0, double y0, double z0,
    double dx, double dy, double dz,
    double c, bool includeBottom,
    MPointArray& points,
    MIntArray&   faceCounts,
    MIntArray&   faceConnects)
{
    const int b = static_cast<int>(points.length());

    points.append(MPoint(x0,      y0,      z0     )); // [0] front lower-left
    points.append(MPoint(x0 + dx, y0,      z0     )); // [1] back  lower-left
    points.append(MPoint(x0 + dx, y0 + dy, z0     )); // [2] back  upper-left
    points.append(MPoint(x0 + c,  y0 + dy, z0     )); // [3] tread front-left
    points.append(MPoint(x0,      y0+dy-c, z0     )); // [4] riser top-left
    points.append(MPoint(x0,      y0,      z0 + dz)); // [5] front lower-right
    points.append(MPoint(x0 + dx, y0,      z0 + dz)); // [6] back  lower-right
    points.append(MPoint(x0 + dx, y0 + dy, z0 + dz)); // [7] back  upper-right
    points.append(MPoint(x0 + c,  y0 + dy, z0 + dz)); // [8] tread front-right
    points.append(MPoint(x0,      y0+dy-c, z0 + dz)); // [9] riser top-right

    const int quads[5][4] = {
        {0, 1, 6, 5},  // bottom  -Y  (index 0 — conditional)
        {3, 8, 7, 2},  // tread   +Y
        {0, 5, 9, 4},  // riser   -X
        {4, 9, 8, 3},  // chamfer diagonal nosing face
        {1, 2, 7, 6},  // back    +X
    };
    const int startFace = includeBottom ? 0 : 1;
    for (int fi = startFace; fi < 5; ++fi) {
        faceCounts.append(4);
        for (int v : quads[fi])
            faceConnects.append(b + v);
    }

    // left and right caps are planar pentagons
    const int pentagons[2][5] = {
        {0, 4, 3, 2, 1},  // left  -Z
        {5, 6, 7, 8, 9},  // right +Z
    };
    for (const auto& face : pentagons) {
        faceCounts.append(5);
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
    const double c  = std::min(p.chamfer, std::min(rh * 0.5, p.treadDepth * 0.5));

    // Keep the horizontal bottom face only on the first and last step; intermediate
    // steps use the slanted connector below instead so the underside is smooth.
    for (int i = 0; i < p.stepCount; ++i) {
        const bool includeBottom = (i == 0 || i == p.stepCount - 1);
        appendChamferedBox(i * p.treadDepth, i * rh, 0.0,
                           p.treadDepth, rh, p.stairWidth, c,
                           includeBottom,
                           points, faceCounts, faceConnects);
    }

    // Each quad connects step (i-1)'s back-lower edge to step i's back-lower edge,
    // forming the slanted underside surface between consecutive steps.
    // Vertex layout is always 10 per step, so step i starts at index i*10.
    for (int i = 1; i < p.stepCount; ++i) {
        const int prev = (i-1) * 10;
        const int curr = i * 10;
        faceCounts.append(4);
        faceConnects.append(prev + 1);  // step i-1 back-lower-left
        faceConnects.append(curr + 1);  // step i   back-lower-left
        faceConnects.append(curr + 6);  // step i   back-lower-right
        faceConnects.append(prev + 6);  // step i-1 back-lower-right
    }
}
