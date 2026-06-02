#pragma once
#include <maya/MPointArray.h>
#include <maya/MIntArray.h>

struct BridgeParams {
    int    stepCount;
    double totalHeight;
    double stairWidth;
    double treadDepth;
    double bridgeLength;
    int    deckSegments     = 10;
    double archHeight       = 0.0;
    int    stepSubdivisions = 4;
    double bumpHeight       = 0.0;
    double bumpFreq         = 1.0;
    double chamfer          = 0.03;
    // voussoir & arc rythm
    int voussoir            = 1;
    double voussoirTaper    = 0.0;
    double jointGap         = 0.0;
    double keystoneScale    = 0.0;
    // perlin weathering (drives the same bumpHeight amplitude as the sin/cos ripple)
    int noiseOctaves        = 0;    // 0 = no weathering (smooth surface)
    int noiseSeed           = 0;
    int weatherSubdiv       = 4;    // tessellation of weathered faces
};

void buildBridgeMesh(
    const BridgeParams& params,
    MPointArray&        points,
    MIntArray&          faceCounts,
    MIntArray&          faceConnects
);
