#pragma once
#include <maya/MPxNode.h>
#include <maya/MObject.h>
#include <maya/MTypeId.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MStatus.h>

class ParametricBridgeNode : public MPxNode {
public:
    ParametricBridgeNode()           = default;
    ~ParametricBridgeNode() override = default;

    static void*   creator();
    static MStatus initialize();
    MStatus        compute(const MPlug& plug, MDataBlock& data) override;

    static MTypeId id;

    static MObject inTotalHeight;
    static MObject inStairWidth;
    static MObject inTreadDepth;
    static MObject inStepCount;
    static MObject inBridgeLength;
    static MObject inDeckScale;
    static MObject inDeckSegments;
    static MObject inArchHeight;
    static MObject inStepSubdivisions;
    static MObject inBumpHeight;
    static MObject inBumpFreq;
    static MObject inChamfer;
    static MObject inVoussoirCount;
    static MObject inVoussoirTaper;
    static MObject inJointGap;
    static MObject inKeystoneScale;
    static MObject inNoiseOctaves;
    static MObject inNoiseSeed;
    static MObject inWeatherSubdiv;
    static MObject outMesh;
};
