#pragma once
#include <maya/MPointArray.h>
#include <maya/MIntArray.h>

struct StaircaseParams {
    int    stepCount;
    double totalHeight;
    double stairWidth;
    double treadDepth;
};

void buildStaircaseMesh(
    const StaircaseParams& params,
    MPointArray&           points,
    MIntArray&             faceCounts,
    MIntArray&             faceConnects
);

struct BridgeParams {
    int    stepCount;
    double totalHeight;
    double stairWidth;
    double treadDepth;
    double bridgeLength;
    int    deckSegments = 10;
    double archHeight   = 0.0;
};

void buildBridgeMesh(
    const BridgeParams& params,
    MPointArray&        points,
    MIntArray&          faceCounts,
    MIntArray&          faceConnects
);
