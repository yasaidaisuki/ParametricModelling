#include "parametricStaircaseNode.h"
#include "staircaseMesh.h"

#include <maya/MFnMesh.h>
#include <maya/MFnMeshData.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnData.h>
#include <maya/MFnNumericData.h>
#include <maya/MDataHandle.h>
#include <maya/MPointArray.h>
#include <maya/MIntArray.h>

MTypeId ParametricStaircaseNode::id(0x00080001);
MObject ParametricStaircaseNode::inTotalHeight;
MObject ParametricStaircaseNode::inStairWidth;
MObject ParametricStaircaseNode::inTreadDepth;
MObject ParametricStaircaseNode::inStepCount;
MObject ParametricStaircaseNode::inChamfer;
MObject ParametricStaircaseNode::inBumpHeight;
MObject ParametricStaircaseNode::inBumpFreq;
MObject ParametricStaircaseNode::inNoiseOctaves;
MObject ParametricStaircaseNode::inNoiseSeed;
MObject ParametricStaircaseNode::inWeatherSubdiv;
MObject ParametricStaircaseNode::outMesh;

void* ParametricStaircaseNode::creator() {
    return new ParametricStaircaseNode();
}

MStatus ParametricStaircaseNode::initialize() {
    MFnNumericAttribute nAttr;
    MFnTypedAttribute   tAttr;

    inTotalHeight = nAttr.create("totalHeight", "th", MFnNumericData::kDouble, 3.0);
    nAttr.setMin(0.01);
    nAttr.setKeyable(true);
    addAttribute(inTotalHeight);

    inStairWidth = nAttr.create("stairWidth", "sw", MFnNumericData::kDouble, 2.0);
    nAttr.setMin(0.01);
    nAttr.setKeyable(true);
    addAttribute(inStairWidth);

    inTreadDepth = nAttr.create("treadDepth", "td", MFnNumericData::kDouble, 0.3);
    nAttr.setMin(0.01);
    nAttr.setKeyable(true);
    addAttribute(inTreadDepth);

    inStepCount = nAttr.create("stepCount", "sc", MFnNumericData::kInt, 10);
    nAttr.setMin(1);
    nAttr.setMax(200);
    nAttr.setKeyable(true);
    addAttribute(inStepCount);

    inChamfer = nAttr.create("chamfer", "ch", MFnNumericData::kDouble, 0.03);
    nAttr.setMin(0.0);
    nAttr.setKeyable(true);
    addAttribute(inChamfer);

    inBumpHeight = nAttr.create("bumpHeight", "bh", MFnNumericData::kDouble, 0.0);
    nAttr.setKeyable(true);
    addAttribute(inBumpHeight);

    inBumpFreq = nAttr.create("bumpFreq", "bf", MFnNumericData::kDouble, 1.0);
    nAttr.setMin(0.001);
    nAttr.setKeyable(true);
    addAttribute(inBumpFreq);

    inNoiseOctaves = nAttr.create("noiseOctaves", "no", MFnNumericData::kInt, 0);
    nAttr.setMin(0);
    nAttr.setMax(8);
    nAttr.setKeyable(true);
    addAttribute(inNoiseOctaves);

    inNoiseSeed = nAttr.create("noiseSeed", "nsd", MFnNumericData::kInt, 0);
    nAttr.setKeyable(true);
    addAttribute(inNoiseSeed);

    inWeatherSubdiv = nAttr.create("weatherSubdiv", "wsd", MFnNumericData::kInt, 4);
    nAttr.setMin(1);
    nAttr.setMax(32);
    nAttr.setKeyable(true);
    addAttribute(inWeatherSubdiv);

    outMesh = tAttr.create("outMesh", "om", MFnData::kMesh);
    tAttr.setStorable(false);
    tAttr.setWritable(false);
    addAttribute(outMesh);

    attributeAffects(inTotalHeight,  outMesh);
    attributeAffects(inStairWidth,   outMesh);
    attributeAffects(inTreadDepth,   outMesh);
    attributeAffects(inStepCount,    outMesh);
    attributeAffects(inChamfer,      outMesh);
    attributeAffects(inBumpHeight,    outMesh);
    attributeAffects(inBumpFreq,      outMesh);
    attributeAffects(inNoiseOctaves,  outMesh);
    attributeAffects(inNoiseSeed,     outMesh);
    attributeAffects(inWeatherSubdiv, outMesh);

    return MS::kSuccess;
}

MStatus ParametricStaircaseNode::compute(const MPlug& plug, MDataBlock& data) {
    if (plug != outMesh)
        return MS::kUnknownParameter;

    StaircaseParams params;
    params.totalHeight  = data.inputValue(inTotalHeight).asDouble();
    params.stairWidth   = data.inputValue(inStairWidth).asDouble();
    params.treadDepth   = data.inputValue(inTreadDepth).asDouble();
    params.stepCount    = data.inputValue(inStepCount).asInt();
    params.chamfer      = data.inputValue(inChamfer).asDouble();
    params.bumpHeight   = data.inputValue(inBumpHeight).asDouble();
    params.bumpFreq     = data.inputValue(inBumpFreq).asDouble();
    params.noiseOctaves = data.inputValue(inNoiseOctaves).asInt();
    params.noiseSeed    = data.inputValue(inNoiseSeed).asInt();
    params.weatherSubdiv = data.inputValue(inWeatherSubdiv).asInt();

    MPointArray points;
    MIntArray   faceCounts;
    MIntArray   faceConnects;
    buildStaircaseMesh(params, points, faceCounts, faceConnects);

    MFnMeshData meshDataFn;
    MObject     meshData = meshDataFn.create();

    MFnMesh meshFn;
    meshFn.create(
        static_cast<int>(points.length()),
        static_cast<int>(faceCounts.length()),
        points, faceCounts, faceConnects,
        meshData);

    MDataHandle outHandle = data.outputValue(outMesh);
    outHandle.set(meshData);
    data.setClean(plug);

    return MS::kSuccess;
}
