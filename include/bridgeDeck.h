#pragma once
#include <maya/MPointArray.h>
#include <maya/MIntArray.h>

// Bridge deck spans: the curved walkway connecting the two stair flights. Both
// builders follow the same parabolic profile (y = yBase + archHeight*3*t*(1-t),
// t in [0,1]) and append to the supplied arrays from their current length.

// Smooth deck: one swept strip of `segments` quad rings along the arch profile,
// with end caps. The top face is displaced by the sin*cos ripple bump.
void appendArcDeck(
    double xStart, double yBase, double width, double thickness,
    double length, double archHeight, int segments,
    double bumpHeight, double bumpFreq,
    MPointArray& points,
    MIntArray&   faceCounts,
    MIntArray&   faceConnects);

// Segmented arch built from `count` discrete wedge voussoirs instead of one
// smooth strip. Each block is an independent watertight wedge separated from its
// neighbours by `jointGap` so the stone joints read on the arch face. The wedge
// taper comes from offsetting the extrados along the parabola normal (blended
// toward vertical by `taper`); the central block (odd count only) is widened by
// `keystoneScale`. The top (extrados) is displaced by the ripple bump.
void appendVoussoirArch(
    double xStart, double yBase, double width, double thickness,
    double length, double archHeight, int count,
    double taper, double jointGap, double keystoneScale,
    double bumpHeight, double bumpFreq,
    MPointArray& points,
    MIntArray&   faceCounts,
    MIntArray&   faceConnects);
