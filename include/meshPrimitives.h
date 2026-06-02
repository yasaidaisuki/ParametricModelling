#pragma once
#include <maya/MPointArray.h>
#include <maya/MIntArray.h>

// Reusable box primitives shared across mesh builders. Each appends to the
// supplied arrays (points / faceCounts / faceConnects) starting from their
// current length, so callers can stitch many primitives into one mesh.

// Axis-aligned box with the front-top corner (-X, +Y) chamfered off, leaving a
// diagonal nosing face. Always appends 10 vertices. The bottom (-Y) and back
// (+X) faces are optional: drop them when a neighbouring connector seals the gap
// so they would otherwise be internal.
void appendChamferedBox(
    double x0, double y0, double z0,
    double dx, double dy, double dz,
    double c, bool includeBottom, bool includeBack,
    MPointArray& points,
    MIntArray&   faceCounts,
    MIntArray&   faceConnects);

// Mirror of appendChamferedBox: the back-top corner (+X, +Y) is chamfered. The
// bottom (-Y) and full-height front (-X) faces are optional for the same reason.
void appendChamferedBoxMirrored(
    double x0, double y0, double z0,
    double dx, double dy, double dz,
    double c, bool includeBottom, bool includeFront,
    MPointArray& points,
    MIntArray&   faceCounts,
    MIntArray&   faceConnects);

// Tessellated box: each face is subdivided into an (subdiv x subdiv) grid of
// quads. The top face (+Y) is displaced by a sin*cos ripple of the given height
// and frequency; all other faces stay flat.
void appendSubdividedBox(
    double x0, double y0, double z0,
    double dx, double dy, double dz,
    int subdiv,
    double bumpHeight, double bumpFreq,
    MPointArray& points,
    MIntArray&   faceCounts,
    MIntArray&   faceConnects);
