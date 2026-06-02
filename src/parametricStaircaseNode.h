#pragma once
#include <maya/MPxNode.h>
#include <maya/MObject.h>
#include <maya/MTypeId.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MStatus.h>

class ParametricStaircaseNode : public MPxNode {
public:
    ParametricStaircaseNode()           = default;
    ~ParametricStaircaseNode() override = default;

    static void*   creator();
    static MStatus initialize();
    MStatus        compute(const MPlug& plug, MDataBlock& data) override;

    static MTypeId id;

    static MObject inTotalHeight;
    static MObject inStairWidth;
    static MObject inTreadDepth;
    static MObject inStepCount;
    static MObject inChamfer;
    static MObject inBumpHeight;
    static MObject inBumpFreq;
    static MObject inNoiseOctaves;
    static MObject inNoiseSeed;
    static MObject inWeatherSubdiv;
    static MObject outMesh;
};
