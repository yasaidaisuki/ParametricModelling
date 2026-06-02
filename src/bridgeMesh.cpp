#include "bridgeMesh.h"
#include <maya/MPoint.h>
#include <cmath>

// Chamfer on front-top corner (-X, +Y): used for left (ascending) stairs.
static void appendChamferedBox(
    double x0, double y0, double z0,
    double dx, double dy, double dz,
    double c,
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
        {0, 1, 6, 5},  // bottom -Y
        {3, 8, 7, 2},  // tread  +Y
        {0, 5, 9, 4},  // riser  -X
        {4, 9, 8, 3},  // chamfer nosing
        {1, 2, 7, 6},  // back   +X
    };
    for (const auto& face : quads) {
        faceCounts.append(4);
        for (int v : face) faceConnects.append(b + v);
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

// Chamfer on back-top corner (+X, +Y): used for right (descending) stairs.
static void appendChamferedBoxMirrored(
    double x0, double y0, double z0,
    double dx, double dy, double dz,
    double c,
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
        {0, 1, 6, 5},  // bottom  -Y
        {2, 7, 8, 3},  // tread   +Y
        {0, 5, 7, 2},  // front   -X (full height)
        {3, 8, 9, 4},  // chamfer nosing
        {1, 4, 9, 6},  // back    +X (partial)
    };
    for (const auto& face : quads) {
        faceCounts.append(4);
        for (int v : face) faceConnects.append(b + v);
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

// Tessellated box with optional top-face displacement using sin*cos ripple.
// Each face is subdivided into an (n x n) grid of quads.
static void appendSubdividedBox(
    double x0, double y0, double z0,
    double dx, double dy, double dz,
    int subdiv,
    double bumpHeight, double bumpFreq,
    MPointArray& points,
    MIntArray&   faceCounts,
    MIntArray&   faceConnects)
{
    const int n = subdiv < 1 ? 1 : subdiv;

    auto quad = [&](int a, int b, int c, int d) {
        faceCounts.append(4);
        faceConnects.append(a);
        faceConnects.append(b);
        faceConnects.append(c);
        faceConnects.append(d);
    };

    auto addPt = [&](double x, double y, double z) -> int {
        int idx = static_cast<int>(points.length());
        points.append(MPoint(x, y, z));
        return idx;
    };

    // --- Top face (+Y): displaced by bump ---
    {
        const int base = static_cast<int>(points.length());
        for (int j = 0; j <= n; ++j) {
            const double tz = (double)j / n;
            const double wz = z0 + tz * dz;
            for (int i = 0; i <= n; ++i) {
                const double tx = (double)i / n;
                const double wx = x0 + tx * dx;
                const double disp = bumpHeight * std::sin(wx * bumpFreq) * std::cos(wz * bumpFreq);
                addPt(wx, y0 + dy + disp, wz);
            }
        }
        const int stride = n + 1;
        for (int j = 0; j < n; ++j)
            for (int i = 0; i < n; ++i) {
                int a = base + j*stride + i;
                int b = base + j*stride + i+1;
                int c = base + (j+1)*stride + i+1;
                int d = base + (j+1)*stride + i;
                quad(a, d, c, b);
            }
    }

    // --- Bottom face (-Y): flat grid in XZ at y = y0 ---
    {
        const int base = static_cast<int>(points.length());
        for (int j = 0; j <= n; ++j) {
            const double tz = (double)j / n;
            for (int i = 0; i <= n; ++i) {
                const double tx = (double)i / n;
                addPt(x0 + tx*dx, y0, z0 + tz*dz);
            }
        }
        const int stride = n + 1;
        for (int j = 0; j < n; ++j)
            for (int i = 0; i < n; ++i) {
                int a = base + j*stride + i;
                int b = base + j*stride + i+1;
                int c = base + (j+1)*stride + i+1;
                int d = base + (j+1)*stride + i;
                quad(a, b, c, d);
            }
    }

    // --- Front face (-X riser): grid in YZ at x = x0 ---
    {
        const int base = static_cast<int>(points.length());
        for (int j = 0; j <= n; ++j) {
            const double ty = (double)j / n;
            for (int i = 0; i <= n; ++i) {
                const double tz = (double)i / n;
                addPt(x0, y0 + ty*dy, z0 + tz*dz);
            }
        }
        const int stride = n + 1;
        for (int j = 0; j < n; ++j)
            for (int i = 0; i < n; ++i) {
                int a = base + j*stride + i;
                int b = base + j*stride + i+1;
                int c = base + (j+1)*stride + i+1;
                int d = base + (j+1)*stride + i;
                quad(a, d, c, b);
            }
    }

    // --- Back face (+X): grid in YZ at x = x0+dx ---
    {
        const int base = static_cast<int>(points.length());
        for (int j = 0; j <= n; ++j) {
            const double ty = (double)j / n;
            for (int i = 0; i <= n; ++i) {
                const double tz = (double)i / n;
                addPt(x0 + dx, y0 + ty*dy, z0 + tz*dz);
            }
        }
        const int stride = n + 1;
        for (int j = 0; j < n; ++j)
            for (int i = 0; i < n; ++i) {
                int a = base + j*stride + i;
                int b = base + j*stride + i+1;
                int c = base + (j+1)*stride + i+1;
                int d = base + (j+1)*stride + i;
                quad(a, b, c, d);
            }
    }

    // --- Left face (-Z): grid in XY at z = z0 ---
    {
        const int base = static_cast<int>(points.length());
        for (int j = 0; j <= n; ++j) {
            const double ty = (double)j / n;
            for (int i = 0; i <= n; ++i) {
                const double tx = (double)i / n;
                addPt(x0 + tx*dx, y0 + ty*dy, z0);
            }
        }
        const int stride = n + 1;
        for (int j = 0; j < n; ++j)
            for (int i = 0; i < n; ++i) {
                int a = base + j*stride + i;
                int b = base + j*stride + i+1;
                int c = base + (j+1)*stride + i+1;
                int d = base + (j+1)*stride + i;
                quad(a, b, c, d);
            }
    }

    // --- Right face (+Z): grid in XY at z = z0+dz ---
    {
        const int base = static_cast<int>(points.length());
        for (int j = 0; j <= n; ++j) {
            const double ty = (double)j / n;
            for (int i = 0; i <= n; ++i) {
                const double tx = (double)i / n;
                addPt(x0 + tx*dx, y0 + ty*dy, z0 + dz);
            }
        }
        const int stride = n + 1;
        for (int j = 0; j < n; ++j)
            for (int i = 0; i < n; ++i) {
                int a = base + j*stride + i;
                int b = base + j*stride + i+1;
                int c = base + (j+1)*stride + i+1;
                int d = base + (j+1)*stride + i;
                quad(a, d, c, b);
            }
    }
}

static void appendArcDeck(
    double xStart, double yBase, double width, double thickness,
    double length, double archHeight, int segments,
    double bumpHeight, double bumpFreq,
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
        const double dispNear = bumpHeight * std::sin(x * bumpFreq) * std::cos(0.0   * bumpFreq);
        const double dispFar  = bumpHeight * std::sin(x * bumpFreq) * std::cos(width * bumpFreq);
        points.append(MPoint(x, y,                        0.0  ));  // v0 bottom near
        points.append(MPoint(x, y + thickness + dispNear, 0.0  ));  // v1 top near
        points.append(MPoint(x, y,                        width));  // v2 bottom far
        points.append(MPoint(x, y + thickness + dispFar,  width));  // v3 top far
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

    const double c = std::min(p.chamfer, std::min(rh * 0.5, p.treadDepth * 0.5));

    //   1. Left stairs
    for (int i = 0; i < p.stepCount; i++)
        appendChamferedBox(i * p.treadDepth, i * rh, 0.0,
                           p.treadDepth, rh, p.stairWidth, c,
                           points, faceCounts, faceConnects);

    //   1b. Slanted underside connectors between consecutive left steps.
    //   Each quad joins step i-1's back-lower edge (verts 1,6 = b,d) to step i's
    //   (b_2,d_2), so n steps yield n-1 faces. Layout is 10 verts/step.
    for (int i = 1; i < p.stepCount; ++i) {
        const int prev = (i - 1) * 10;
        const int curr = i * 10;
        faceCounts.append(4);
        faceConnects.append(prev + 1);  // b   : step i-1 back-lower-left
        faceConnects.append(curr + 1);  // b_2 : step i   back-lower-left
        faceConnects.append(curr + 6);  // d_2 : step i   back-lower-right
        faceConnects.append(prev + 6);  // d   : step i-1 back-lower-right
    }

    //   2. Bridge deck (arc)
    appendArcDeck(p.stepCount*p.treadDepth, (p.stepCount - 1) * rh, p.stairWidth, rh,
                  p.bridgeLength, p.archHeight, p.deckSegments,
                  p.bumpHeight, p.bumpFreq,
                  points, faceCounts, faceConnects);

    //   3. Right stairs (descending — chamfer on back-top corner)
    for (int i = 0; i < p.stepCount; i++)
        appendChamferedBoxMirrored(p.stepCount*p.treadDepth + p.bridgeLength + i*p.treadDepth,
                                   (p.stepCount - 1 - i) * rh, 0.0,
                                   p.treadDepth, rh, p.stairWidth, c,
                                   points, faceCounts, faceConnects);
}
