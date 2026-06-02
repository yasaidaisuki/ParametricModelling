#include "bridgeMesh.h"
#include "bridgeStairs.h"
#include "bridgeDeck.h"
#include <algorithm>

// A bridge is two stair flights joined by an arched deck:
//
//   left stairs ▁▂▃     deck ╭───╮     right stairs ▃▂▁
//   (ascending)        (arch)         (descending)
//
// Each piece is built by a dedicated helper (bridgeStairs / bridgeDeck) using
// the shared box primitives in meshPrimitives. buildBridgeMesh just lays them
// out end to end along +X.
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
    const double c  = std::min(p.chamfer, std::min(rh * 0.5, p.treadDepth * 0.5));

    const double deckStartX = p.stepCount * p.treadDepth;
    const double deckTopY    = (p.stepCount - 1) * rh;  // height the deck springs from
    const double rightStartX = deckStartX + p.bridgeLength;

    //   1. Left stairs (ascending).
    appendBridgeStairsLeft(0.0, rh,
                           p.stepCount, p.treadDepth, p.stairWidth, c,
                           points, faceCounts, faceConnects);

    //   2. Bridge deck — segmented voussoir arch, or smooth strip when voussoir <= 1.
    if (p.voussoir > 1) {
        appendVoussoirArch(deckStartX, deckTopY, p.stairWidth, rh,
                           p.bridgeLength, p.archHeight, p.voussoir,
                           p.voussoirTaper, p.jointGap, p.keystoneScale,
                           p.bumpHeight, p.bumpFreq,
                           p.noiseOctaves, p.noiseSeed, p.weatherSubdiv,
                           points, faceCounts, faceConnects);
    } else {
        appendArcDeck(deckStartX, deckTopY, p.stairWidth, rh,
                      p.bridgeLength, p.archHeight, p.deckSegments,
                      p.bumpHeight, p.bumpFreq,
                      p.noiseOctaves, p.noiseSeed,
                      points, faceCounts, faceConnects);
    }

    //   3. Right stairs (descending).
    appendBridgeStairsRight(rightStartX, rh,
                            p.stepCount, p.treadDepth, p.stairWidth, c,
                            points, faceCounts, faceConnects);
}
