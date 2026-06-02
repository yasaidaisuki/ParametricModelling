#include "bridgeStairs.h"
#include "meshPrimitives.h"

void appendBridgeStairsLeft(
    double xStart, double riserHeight,
    int stepCount, double treadDepth, double stairWidth, double chamfer,
    MPointArray& points,
    MIntArray&   faceCounts,
    MIntArray&   faceConnects)
{
    const int flightBase = static_cast<int>(points.length());

    //   Steps. The slant connectors below replace the underside, so each step's
    //   flat bottom and back face become internal except at the two ends: keep
    //   the bottom only on step 0 and the back only on the last step.
    for (int i = 0; i < stepCount; ++i)
        appendChamferedBox(xStart + i*treadDepth, i*riserHeight, 0.0,
                           treadDepth, riserHeight, stairWidth, chamfer,
                           /*includeBottom*/ i == 0,
                           /*includeBack  */ i == stepCount - 1,
                           points, faceCounts, faceConnects);

    //   Slanted underside connectors between consecutive steps. Each quad joins
    //   step i-1's back-lower edge (verts 1,6) to step i's, so n steps yield n-1
    //   faces. The two triangles cap the -Z/+Z ends of the pocket the slant
    //   seals off (prev's back-upper corner verts 2,7 coincide with step i's
    //   front-lower).
    for (int i = 1; i < stepCount; ++i) {
        const int prev = flightBase + (i - 1) * 10;
        const int curr = flightBase + i * 10;
        faceCounts.append(4);
        faceConnects.append(prev + 1);  // step i-1 back-lower-left
        faceConnects.append(curr + 1);  // step i   back-lower-left
        faceConnects.append(curr + 6);  // step i   back-lower-right
        faceConnects.append(prev + 6);  // step i-1 back-lower-right

        faceCounts.append(3);           // -Z side cap
        faceConnects.append(prev + 1);  // step i-1 back-lower-left
        faceConnects.append(prev + 2);  // step i-1 back-upper-left (shared corner)
        faceConnects.append(curr + 1);  // step i   back-lower-left
        faceCounts.append(3);           // +Z side cap
        faceConnects.append(curr + 6);  // step i   back-lower-right
        faceConnects.append(prev + 7);  // step i-1 back-upper-right (shared corner)
        faceConnects.append(prev + 6);  // step i-1 back-lower-right
    }
}

void appendBridgeStairsRight(
    double xStart, double riserHeight,
    int stepCount, double treadDepth, double stairWidth, double chamfer,
    MPointArray& points,
    MIntArray&   faceCounts,
    MIntArray&   faceConnects)
{
    const int flightBase = static_cast<int>(points.length());

    //   Steps (descending — chamfer on back-top corner). Mirror of the left
    //   flight: the slant connectors replace the underside, so keep the bottom
    //   only on the last (lowest) step and the front only on step 0.
    for (int i = 0; i < stepCount; ++i)
        appendChamferedBoxMirrored(xStart + i*treadDepth,
                                   (stepCount - 1 - i) * riserHeight, 0.0,
                                   treadDepth, riserHeight, stairWidth, chamfer,
                                   /*includeBottom*/ i == stepCount - 1,
                                   /*includeFront */ i == 0,
                                   points, faceCounts, faceConnects);

    //   Slanted underside connectors between consecutive steps. The descending
    //   underside is the front-lower edge (verts 0,5), so each quad joins step
    //   i-1's (0,5) to step i's, giving n-1 faces. The two triangles cap the
    //   -Z/+Z ends of the pocket (prev's back-lower verts 1,6 coincide with step
    //   i's front-upper).
    for (int i = 1; i < stepCount; ++i) {
        const int prev = flightBase + (i - 1) * 10;
        const int curr = flightBase + i * 10;
        faceCounts.append(4);
        faceConnects.append(prev + 0);  // step i-1 front-lower-left
        faceConnects.append(curr + 0);  // step i   front-lower-left
        faceConnects.append(curr + 5);  // step i   front-lower-right
        faceConnects.append(prev + 5);  // step i-1 front-lower-right

        faceCounts.append(3);           // -Z side cap
        faceConnects.append(prev + 0);  // step i-1 front-lower-left
        faceConnects.append(prev + 1);  // step i-1 back-lower-left (shared corner)
        faceConnects.append(curr + 0);  // step i   front-lower-left
        faceCounts.append(3);           // +Z side cap
        faceConnects.append(curr + 5);  // step i   front-lower-right
        faceConnects.append(prev + 6);  // step i-1 back-lower-right (shared corner)
        faceConnects.append(prev + 5);  // step i-1 front-lower-right
    }
}
