#include "bridgeDeck.h"
#include "noise.h"
#include <maya/MPoint.h>
#include <cmath>
#include <vector>

void appendArcDeck(
    double xStart, double yBase, double width, double thickness,
    double length, double archHeight, int segments,
    double bumpHeight, double bumpFreq,
    int noiseOctaves, int noiseSeed,
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
        const double yTop = y + thickness;
        const double dispNear = bumpHeight * fbm(x * bumpFreq, yTop * bumpFreq, 0.0,
                                                 noiseOctaves, noiseSeed);
        const double dispFar  = bumpHeight * fbm(x * bumpFreq, yTop * bumpFreq, width * bumpFreq,
                                                 noiseOctaves, noiseSeed);
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

void appendVoussoirArch(
    double xStart, double yBase, double width, double thickness,
    double length, double archHeight, int count,
    double taper, double jointGap, double keystoneScale,
    double bumpHeight, double bumpFreq,
    int noiseOctaves, int noiseSeed, int weatherSubdiv,
    MPointArray& points,
    MIntArray&   faceCounts,
    MIntArray&   faceConnects)
{
    // --- profile + normal helpers (parabola: y = yBase + archHeight*3*t*(1-t)) ---
    auto yAt = [&](double t) { return yBase + archHeight * 3.0 * t * (1.0 - t); };
    auto xAt = [&](double t) { return xStart + t * length; };
    // upward unit normal N(t) = normalize(-T.y, T.x), T = (length, 3*archHeight*(1-2t))
    auto normalAt = [&](double t, double& nx, double& ny) {
        const double tx = length;
        const double ty = 3.0 * archHeight * (1.0 - 2.0 * t);
        double vx = -ty, vy = tx;                       // rotate tangent +90°
        const double len = std::sqrt(vx*vx + vy*vy);
        if (len > 1e-9) { vx /= len; vy /= len; } else { vx = 0.0; vy = 1.0; }
        nx = vx; ny = vy;
    };

    // --- block t-boundaries (the joint lines between stones) -------------------
    // Every block has weight 1, except the centre keystone (odd count) which is
    // widened by keystoneScale. Normalise so the widths sum to 1.
    const double gapT = (length > 1e-9) ? jointGap / length : 0.0;
    const int mid = (count % 2 == 1) ? count / 2 : -1;   // -1 = no keystone
    double totalW = 0.0;
    for (int k = 0; k < count; k++) 
        totalW += (k == mid) ? keystoneScale : 1.0;
    
    // Compute t_i, where i=0,1,...,k
    std::vector<double> t(count + 1); 
    t[0] = 0.0;
    for (int k = 0; k < count; k++) {
        const double w = (k == mid) ? keystoneScale : 1.0;
        t[k+1] = t[k] + w / totalW;
    }
    t[count] = 1.0; 

    // --- build one wedge block per [t[k], t[k+1]] ------------------------------
    // intrados corners sit on the parabola; extrados corners are pushed outward
    // along the curve normal (blended toward straight-up by `taper`). The two
    // outermost springer columns stay vertical so the end stones meet the stairs.
    for (int k = 0; k < count; k++) {
        // Carve half a joint gap off each interior edge; keep the two outer
        // springer edges flush at t=0 / t=1 so the end stones meet the stairs.
        const double a  = (k == 0)         ? 0.0 : t[k]     + gapT * 0.5;
        const double bT = (k == count - 1) ? 1.0 : t[k + 1] - gapT * 0.5;

        // offset from an intrados point to its extrados point at parameter tt
        auto offsetAt = [&](double tt, bool springer, double& ox, double& oy) {
            if (springer) { ox = 0.0; oy = thickness; return; }   // flat top at the ends
            double nx, ny; normalAt(tt, nx, ny);
            ox = taper * nx * thickness;                          // sideways lean (0 if taper=0)
            oy = (1.0 - taper) * thickness + taper * ny * thickness;
        };
        double oxA, oyA, oxB, oyB;
        offsetAt(a,  k == 0,         oxA, oyA);     // first block's start = left springer
        offsetAt(bT, k == count - 1, oxB, oyB);     // last block's end   = right springer

        const double xa = xAt(a),  ya = yAt(a);
        const double xb = xAt(bT), yb = yAt(bT);
        const int b = static_cast<int>(points.length());

        // intrados (on the parabola)
        points.append(MPoint(xa,       ya,       0.0  ));  // b+0  start near
        points.append(MPoint(xa,       ya,       width));  // b+1  start far
        points.append(MPoint(xb,       yb,       0.0  ));  // b+2  end   near
        points.append(MPoint(xb,       yb,       width));  // b+3  end   far
        // extrados (intrados pushed out along the normal)
        points.append(MPoint(xa + oxA, ya + oyA, 0.0  ));  // b+4  start near
        points.append(MPoint(xa + oxA, ya + oyA, width));  // b+5  start far
        points.append(MPoint(xb + oxB, yb + oyB, 0.0  ));  // b+6  end   near
        points.append(MPoint(xb + oxB, yb + oyB, width));  // b+7  end   far

        // All faces except the extrados stay flat quads on the 8 corners. The
        // extrados (visible top of the stone) is replaced below by a subdivided,
        // weathered grid, so it is omitted here.
        const int q[5][4] = {
            {0, 2, 3, 1},  // intrados -Y (curved underside)
            {0, 1, 5, 4},  // start joint
            {2, 6, 7, 3},  // end joint
            {0, 4, 6, 2},  // near side -Z (visible arch face)
            {1, 3, 7, 5},  // far  side +Z
        };
        for (const auto& f : q) {
            faceCounts.append(4);
            for (int v : f) faceConnects.append(b + v);
        }

        // --- weathered extrados: subdivided grid pushed along the arch normal ---
        // The four extrados corners (b+4 start-near, b+5 start-far, b+6 end-near,
        // b+7 end-far) define a bilinear patch. fi runs start->end along the arc,
        // fj runs near->far across the width. We sample fBm at each grid vertex and
        // displace along the local arch normal. Displacement is faded to zero on
        // all four borders so the grid stays welded to the adjoining flat faces.
        const int   n  = weatherSubdiv < 1 ? 1 : weatherSubdiv;
        const int   gb = static_cast<int>(points.length());
        const MPoint p4 = points[b + 4], p5 = points[b + 5];
        const MPoint p6 = points[b + 6], p7 = points[b + 7];

        for (int gj = 0; gj <= n; ++gj) {
            const double fj = (double)gj / n;
            for (int gi = 0; gi <= n; ++gi) {
                const double fi = (double)gi / n;
                // bilinear position on the flat extrados patch
                const MPoint near = p4 + (p6 - p4) * fi;   // start-near -> end-near
                const MPoint far  = p5 + (p7 - p5) * fi;   // start-far  -> end-far
                MPoint pos        = near + (far - near) * fj;

                // arch normal at this arc position (interpolate the block's t-range)
                double nx, ny;
                normalAt(a + (bT - a) * fi, nx, ny);

                // fade displacement to zero on the borders to avoid seams
                constexpr double kPi = 3.14159265358979323846;
                const double edgeFade = std::sin(fi * kPi) * std::sin(fj * kPi);
                const double d = bumpHeight * edgeFade *
                    fbm(pos.x * bumpFreq, pos.y * bumpFreq, pos.z * bumpFreq,
                        noiseOctaves, noiseSeed);

                pos.x += nx * d;
                pos.y += ny * d;   // normal has no Z component (arch lies in X/Y)
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
                // winding matches the old {4,5,7,6} extrados (near->far->far->near)
                faceCounts.append(4);
                faceConnects.append(v0);
                faceConnects.append(v3);
                faceConnects.append(v2);
                faceConnects.append(v1);
            }
    }
}
