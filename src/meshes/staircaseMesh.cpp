#include "staircaseMesh.h"
#include "noise.h"
#include <maya/MPoint.h>
#include <cmath>
#include <vector>

// 10 corner vertices always appended first (indices [b+0]-[b+9]); the connector
// logic in buildStaircaseMesh relies on this fixed layout. The tread (+Y face)
// is then replaced by a `subdiv`-resolution grid displaced in +Y by fractal
// Perlin noise (fBm) for weathering. Pass includeBottom=false to omit the
// horizontal bottom face so slanted inter-step connectors can replace it.
static void appendChamferedBox(
    double x0, double y0, double z0,
    double dx, double dy, double dz,
    double c, bool includeBottom,
    double bumpHeight, double bumpFreq,
    int noiseOctaves, int noiseSeed, int subdiv,
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

    // All quads except the tread (+Y) stay flat on the 10 corners; the tread is
    // emitted below as a weathered grid, so it is omitted here.
    const int quads[4][4] = {
        {0, 1, 6, 5},  // bottom  -Y  (index 0 — conditional)
        {0, 5, 9, 4},  // riser   -X
        {4, 9, 8, 3},  // chamfer diagonal nosing face
        {1, 2, 7, 6},  // back    +X
    };
    const int startFace = includeBottom ? 0 : 1;
    for (int fi = startFace; fi < 4; ++fi) {
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

    // --- weathered tread: subdivided +Y grid over the old {3,8,7,2} quad -------
    // Corners: [3] front-near, [8] front-far, [7] back-far, [2] back-near.
    // fi runs front->back, fj runs near->far. Displace each interior vertex in
    // +Y by fBm, faded to zero on the borders so the grid stays welded to the
    // riser/chamfer/back faces and the pentagon caps.
    const int   n  = subdiv < 1 ? 1 : subdiv;
    const int   gb = static_cast<int>(points.length());
    const MPoint cFrontNear = points[b + 3], cFrontFar = points[b + 8];
    const MPoint cBackNear  = points[b + 2], cBackFar  = points[b + 7];
    constexpr double kPi = 3.14159265358979323846;

    for (int gj = 0; gj <= n; ++gj) {
        const double fj = (double)gj / n;
        for (int gi = 0; gi <= n; ++gi) {
            const double fi = (double)gi / n;
            const MPoint near = cFrontNear + (cBackNear - cFrontNear) * fi;
            const MPoint far  = cFrontFar  + (cBackFar  - cFrontFar)  * fi;
            MPoint pos        = near + (far - near) * fj;

            const double edgeFade = std::sin(fi * kPi) * std::sin(fj * kPi);
            pos.y += bumpHeight * edgeFade *
                fbm(pos.x * bumpFreq, pos.z * bumpFreq, 0.0, noiseOctaves, noiseSeed);
            points.append(pos);
        }
    }

    const int stride = n + 1;
    for (int gj = 0; gj < n; ++gj)
        for (int gi = 0; gi < n; ++gi) {
            const int v0 = gb + gj*stride + gi;
            const int v1 = gb + gj*stride + gi + 1;
            const int v2 = gb + (gj+1)*stride + gi + 1;
            const int v3 = gb + (gj+1)*stride + gi;
            // winding matches the old {3,8,7,2} tread (front-near -> front-far ...)
            faceCounts.append(4);
            faceConnects.append(v0);
            faceConnects.append(v3);
            faceConnects.append(v2);
            faceConnects.append(v1);
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

    // Each box now appends a variable number of tread-grid vertices after its 10
    // corners, so the old fixed `i*10` stride no longer holds. Record where each
    // step's corner block begins (its first 10 vertices) for the connector pass.
    std::vector<int> stepBase(p.stepCount);

    // Keep the horizontal bottom face only on the first and last step; intermediate
    // steps use the slanted connector below instead so the underside is smooth.
    for (int i = 0; i < p.stepCount; ++i) {
        stepBase[i] = static_cast<int>(points.length());
        const bool includeBottom = (i == 0 || i == p.stepCount - 1);
        appendChamferedBox(i * p.treadDepth, i * rh, 0.0,
                           p.treadDepth, rh, p.stairWidth, c,
                           includeBottom,
                           p.bumpHeight, p.bumpFreq,
                           p.noiseOctaves, p.noiseSeed, p.weatherSubdiv,
                           points, faceCounts, faceConnects);
    }

    // Each quad connects step (i-1)'s back-lower edge to step i's back-lower edge,
    // forming the slanted underside surface between consecutive steps. Corner
    // indices +1 / +6 are the first 10 vertices of each step's block.
    for (int i = 1; i < p.stepCount; ++i) {
        const int prev = stepBase[i-1];
        const int curr = stepBase[i];
        faceCounts.append(4);
        faceConnects.append(prev + 1);  // step i-1 back-lower-left
        faceConnects.append(curr + 1);  // step i   back-lower-left
        faceConnects.append(curr + 6);  // step i   back-lower-right
        faceConnects.append(prev + 6);  // step i-1 back-lower-right
    }
}
