#pragma once
#include <maya/MPointArray.h>
#include <maya/MIntArray.h>

struct StaircaseParams {
    int    stepCount;
    double totalHeight;
    double stairWidth;
    double treadDepth;
    double chamfer       = 0.03;
    // perlin tread weathering
    double bumpHeight    = 0.0;     // displacement amplitude (0 = flat treads)
    double bumpFreq      = 1.0;     // base noise frequency
    int    noiseOctaves  = 0;       // 0 = no weathering
    int    noiseSeed     = 0;
    int    weatherSubdiv = 4;       // tessellation of the tread face
};

void buildStaircaseMesh(
    const StaircaseParams& params,
    MPointArray&           points,
    MIntArray&             faceCounts,
    MIntArray&             faceConnects
);
