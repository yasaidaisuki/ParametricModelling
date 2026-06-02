#include "meshPrimitives.h"
#include <maya/MPoint.h>
#include <cmath>

void appendChamferedBox(
    double x0, double y0, double z0,
    double dx, double dy, double dz,
    double c, bool includeBottom, bool includeBack,
    MPointArray& points,
    MIntArray&   faceCounts,
    MIntArray&   faceConnects)
{
    const int b = static_cast<int>(points.length());
    points.append(MPoint(x0,      y0,      z0     ));  // [0] front lower-left
    points.append(MPoint(x0 + dx, y0,      z0     ));  // [1] back  lower-left
    points.append(MPoint(x0 + dx, y0 + dy, z0     ));  // [2] back  upper-left
    points.append(MPoint(x0 + c,  y0 + dy, z0     ));  // [3] tread front-left
    points.append(MPoint(x0,      y0+dy-c, z0     ));  // [4] riser top-left
    points.append(MPoint(x0,      y0,      z0 + dz));  // [5] front lower-right
    points.append(MPoint(x0 + dx, y0,      z0 + dz));  // [6] back  lower-right
    points.append(MPoint(x0 + dx, y0 + dy, z0 + dz));  // [7] back  upper-right
    points.append(MPoint(x0 + c,  y0 + dy, z0 + dz));  // [8] tread front-right
    points.append(MPoint(x0,      y0+dy-c, z0 + dz));  // [9] riser top-right

    const int quads[5][4] = {
        {0, 1, 6, 5},  // bottom -Y  (index 0 — conditional)
        {3, 8, 7, 2},  // tread  +Y
        {0, 5, 9, 4},  // riser  -X
        {4, 9, 8, 3},  // chamfer nosing
        {1, 2, 7, 6},  // back   +X  (index 4 — conditional)
    };
    for (int fi = 0; fi < 5; ++fi) {
        if (fi == 0 && !includeBottom) continue;
        if (fi == 4 && !includeBack)   continue;
        faceCounts.append(4);
        for (int v : quads[fi]) faceConnects.append(b + v);
    }
    const int pentagons[2][5] = {
        {0, 4, 3, 2, 1},  // left  -Z
        {5, 6, 7, 8, 9},  // right +Z
    };
    for (const auto& face : pentagons) {
        faceCounts.append(5);
        for (int v : face) faceConnects.append(b + v);
    }
}

void appendChamferedBoxMirrored(
    double x0, double y0, double z0,
    double dx, double dy, double dz,
    double c, bool includeBottom, bool includeFront,
    MPointArray& points,
    MIntArray&   faceCounts,
    MIntArray&   faceConnects)
{
    const int b = static_cast<int>(points.length());
    points.append(MPoint(x0,        y0,      z0     ));  // [0] front lower-left
    points.append(MPoint(x0 + dx,   y0,      z0     ));  // [1] back  lower-left
    points.append(MPoint(x0,        y0 + dy, z0     ));  // [2] front upper-left
    points.append(MPoint(x0+dx-c,   y0 + dy, z0     ));  // [3] tread back-left
    points.append(MPoint(x0 + dx,   y0+dy-c, z0     ));  // [4] chamfer back-left
    points.append(MPoint(x0,        y0,      z0 + dz));  // [5] front lower-right
    points.append(MPoint(x0 + dx,   y0,      z0 + dz));  // [6] back  lower-right
    points.append(MPoint(x0,        y0 + dy, z0 + dz));  // [7] front upper-right
    points.append(MPoint(x0+dx-c,   y0 + dy, z0 + dz));  // [8] tread back-right
    points.append(MPoint(x0 + dx,   y0+dy-c, z0 + dz));  // [9] chamfer back-right

    const int quads[5][4] = {
        {0, 1, 6, 5},  // bottom  -Y          (index 0 — conditional)
        {2, 7, 8, 3},  // tread   +Y
        {0, 5, 7, 2},  // front   -X (full)   (index 2 — conditional)
        {3, 8, 9, 4},  // chamfer nosing
        {1, 4, 9, 6},  // back    +X (partial)
    };
    for (int fi = 0; fi < 5; ++fi) {
        if (fi == 0 && !includeBottom) continue;
        if (fi == 2 && !includeFront)  continue;
        faceCounts.append(4);
        for (int v : quads[fi]) faceConnects.append(b + v);
    }
    const int pentagons[2][5] = {
        {0, 1, 4, 3, 2},  // left  -Z
        {5, 6, 9, 8, 7},  // right +Z
    };
    for (const auto& face : pentagons) {
        faceCounts.append(5);
        for (int v : face) faceConnects.append(b + v);
    }
}

namespace {

// Append an (n x n) quad grid over vertices laid out row-major with the given
// stride (n+1), starting at `base`. `flip` reverses winding so the face points
// outward regardless of how its parametric axes are oriented.
void appendQuadGrid(int base, int n, bool flip,
                    MIntArray& faceCounts, MIntArray& faceConnects)
{
    const int stride = n + 1;
    for (int j = 0; j < n; ++j)
        for (int i = 0; i < n; ++i) {
            const int a = base + j*stride + i;
            const int b = base + j*stride + i+1;
            const int c = base + (j+1)*stride + i+1;
            const int d = base + (j+1)*stride + i;
            faceCounts.append(4);
            if (flip) { faceConnects.append(a); faceConnects.append(d); faceConnects.append(c); faceConnects.append(b); }
            else      { faceConnects.append(a); faceConnects.append(b); faceConnects.append(c); faceConnects.append(d); }
        }
}

} // namespace

void appendSubdividedBox(
    double x0, double y0, double z0,
    double dx, double dy, double dz,
    int subdiv,
    double bumpHeight, double bumpFreq,
    MPointArray& points,
    MIntArray&   faceCounts,
    MIntArray&   faceConnects)
{
    const int n = subdiv < 1 ? 1 : subdiv;

    // Emit one (n+1)x(n+1) vertex grid then its quads. `vertex(i, j)` maps grid
    // coordinates (0..n along each axis) to a world position for the face.
    auto appendFace = [&](bool flip, auto&& vertex) {
        const int base = static_cast<int>(points.length());
        for (int j = 0; j <= n; ++j)
            for (int i = 0; i <= n; ++i)
                points.append(vertex((double)i / n, (double)j / n));
        appendQuadGrid(base, n, flip, faceCounts, faceConnects);
    };

    // Top face (+Y): displaced by the ripple bump.
    appendFace(true, [&](double tx, double tz) {
        const double wx = x0 + tx*dx, wz = z0 + tz*dz;
        const double disp = bumpHeight * std::sin(wx * bumpFreq) * std::cos(wz * bumpFreq);
        return MPoint(wx, y0 + dy + disp, wz);
    });
    // Bottom face (-Y).
    appendFace(false, [&](double tx, double tz) { return MPoint(x0 + tx*dx, y0, z0 + tz*dz); });
    // Front face (-X riser).
    appendFace(true,  [&](double tz, double ty) { return MPoint(x0,      y0 + ty*dy, z0 + tz*dz); });
    // Back face (+X).
    appendFace(false, [&](double tz, double ty) { return MPoint(x0 + dx, y0 + ty*dy, z0 + tz*dz); });
    // Left face (-Z).
    appendFace(false, [&](double tx, double ty) { return MPoint(x0 + tx*dx, y0 + ty*dy, z0); });
    // Right face (+Z).
    appendFace(true,  [&](double tx, double ty) { return MPoint(x0 + tx*dx, y0 + ty*dy, z0 + dz); });
}
