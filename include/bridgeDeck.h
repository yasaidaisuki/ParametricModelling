#pragma once
#include <maya/MPointArray.h>
#include <maya/MIntArray.h>

// Bridge deck spans: the curved walkway connecting the two stair flights. Both
// builders follow the same parabolic profile (y = yBase + archHeight*3*t*(1-t),
// t in [0,1]) and append to the supplied arrays from their current length.

// Smooth deck: one swept strip of `segments` quad rings along the arch profile,
// with end caps. The top edge is displaced by fractal Perlin noise (fBm) for
// surface weathering; noiseOctaves <= 0 leaves it smooth.
void appendArcDeck(
    double xStart, double yBase, double width, double thickness,
    double length, double archHeight, int segments,
    double bumpHeight, double bumpFreq,
    int noiseOctaves, int noiseSeed,
    MPointArray& points,
    MIntArray&   faceCounts,
    MIntArray&   faceConnects);

// Segmented arch built from `count` discrete wedge voussoirs instead of one
// smooth strip. Each block is an independent watertight wedge separated from its
// neighbours by `jointGap` so the stone joints read on the arch face. The wedge
// taper comes from offsetting the extrados along the parabola normal (blended
// toward vertical by `taper`); the central block (odd count only) is widened by
// `keystoneScale`. The extrados (visible top of each stone) is built as a
// `weatherSubdiv`-resolution grid and displaced along the arch normal by fractal
// Perlin noise (fBm); noiseOctaves <= 0 leaves the stones smooth.
void appendVoussoirArch(
    double xStart, double yBase, double width, double thickness,
    double length, double archHeight, int count,
    double taper, double jointGap, double keystoneScale,
    double bumpHeight, double bumpFreq,
    int noiseOctaves, int noiseSeed, int weatherSubdiv,
    MPointArray& points,
    MIntArray&   faceCounts,
    MIntArray&   faceConnects);
