#include "parametricBridgeNode.h"
#include "bridgeMesh.h"

#include <maya/MFnMesh.h>
#include <maya/MFnMeshData.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnData.h>
#include <maya/MFnNumericData.h>
#include <maya/MDataHandle.h>
#include <maya/MPointArray.h>
#include <maya/MIntArray.h>

MTypeId ParametricBridgeNode::id(0x00080002);
MObject ParametricBridgeNode::inTotalHeight;
MObject ParametricBridgeNode::inStairWidth;
MObject ParametricBridgeNode::inTreadDepth;
MObject ParametricBridgeNode::inStepCount;
MObject ParametricBridgeNode::inBridgeLength;
MObject ParametricBridgeNode::inDeckScale;
MObject ParametricBridgeNode::inDeckSegments;
MObject ParametricBridgeNode::inArchHeight;
MObject ParametricBridgeNode::inStepSubdivisions;
MObject ParametricBridgeNode::inBumpHeight;
MObject ParametricBridgeNode::inBumpFreq;
MObject ParametricBridgeNode::inChamfer;
MObject ParametricBridgeNode::inVoussoirCount;
MObject ParametricBridgeNode::inVoussoirTaper;
MObject ParametricBridgeNode::inJointGap;
MObject ParametricBridgeNode::inKeystoneScale;
MObject ParametricBridgeNode::inNoiseOctaves;
MObject ParametricBridgeNode::inNoiseSeed;
MObject ParametricBridgeNode::inWeatherSubdiv;
MObject ParametricBridgeNode::outMesh;

void* ParametricBridgeNode::creator() {
    return new ParametricBridgeNode();
}

MStatus ParametricBridgeNode::initialize() {
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

    inBridgeLength = nAttr.create("bridgeLength", "bl", MFnNumericData::kDouble, 5.0);
    nAttr.setMin(0.01);
    nAttr.setKeyable(true);
    addAttribute(inBridgeLength);

    inDeckScale = nAttr.create("deckScale", "ds", MFnNumericData::kDouble, 1.0);
    nAttr.setMin(0.001);
    nAttr.setKeyable(true);
    addAttribute(inDeckScale);

    inDeckSegments = nAttr.create("deckSegments", "dsg", MFnNumericData::kInt, 10);
    nAttr.setMin(1);
    nAttr.setMax(200);
    nAttr.setKeyable(true);
    addAttribute(inDeckSegments);

    inArchHeight = nAttr.create("archHeight", "ah", MFnNumericData::kDouble, 0.0);
    nAttr.setKeyable(true);
    addAttribute(inArchHeight);

    inStepSubdivisions = nAttr.create("stepSubdivisions", "ss", MFnNumericData::kInt, 4);
    nAttr.setMin(1);
    nAttr.setMax(32);
    nAttr.setKeyable(true);
    addAttribute(inStepSubdivisions);

    inBumpHeight = nAttr.create("bumpHeight", "bh", MFnNumericData::kDouble, 0.0);
    nAttr.setKeyable(true);
    addAttribute(inBumpHeight);

    inBumpFreq = nAttr.create("bumpFreq", "bf", MFnNumericData::kDouble, 1.0);
    nAttr.setMin(0.001);
    nAttr.setKeyable(true);
    addAttribute(inBumpFreq);

    inChamfer = nAttr.create("chamfer", "ch", MFnNumericData::kDouble, 0.03);
    nAttr.setMin(0.0);
    nAttr.setKeyable(true);
    addAttribute(inChamfer);

    inVoussoirCount = nAttr.create("voussoirCount", "vc", MFnNumericData::kInt, 1);
    nAttr.setMin(1);
    nAttr.setMax(99);
    nAttr.setKeyable(true);
    addAttribute(inVoussoirCount);

    inVoussoirTaper = nAttr.create("voussoirTaper", "vt", MFnNumericData::kDouble, 0.0);
    nAttr.setMin(0.0);
    nAttr.setMax(1.0);
    nAttr.setKeyable(true);
    addAttribute(inVoussoirTaper);

    inJointGap = nAttr.create("jointGap", "jg", MFnNumericData::kDouble, 0.0);
    nAttr.setMin(0.0);
    nAttr.setKeyable(true);
    addAttribute(inJointGap);

    inKeystoneScale = nAttr.create("keystoneScale", "ks", MFnNumericData::kDouble, 1.0);
    nAttr.setMin(0.1);
    nAttr.setKeyable(true);
    addAttribute(inKeystoneScale);

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
    attributeAffects(inBridgeLength,  outMesh);
    attributeAffects(inDeckScale,     outMesh);
    attributeAffects(inDeckSegments,  outMesh);
    attributeAffects(inArchHeight,         outMesh);
    attributeAffects(inStepSubdivisions,   outMesh);
    attributeAffects(inBumpHeight,         outMesh);
    attributeAffects(inBumpFreq,           outMesh);
    attributeAffects(inChamfer,            outMesh);
    attributeAffects(inVoussoirCount,      outMesh);
    attributeAffects(inVoussoirTaper,      outMesh);
    attributeAffects(inJointGap,           outMesh);
    attributeAffects(inKeystoneScale,      outMesh);
    attributeAffects(inNoiseOctaves,       outMesh);
    attributeAffects(inNoiseSeed,          outMesh);
    attributeAffects(inWeatherSubdiv,      outMesh);

    return MS::kSuccess;
}

MStatus ParametricBridgeNode::compute(const MPlug& plug, MDataBlock& data) {
    if (plug != outMesh)
        return MS::kUnknownParameter;

    BridgeParams params;
    params.totalHeight  = data.inputValue(inTotalHeight).asDouble();
    params.stairWidth   = data.inputValue(inStairWidth).asDouble();
    params.treadDepth   = data.inputValue(inTreadDepth).asDouble();
    params.stepCount    = data.inputValue(inStepCount).asInt();
    params.bridgeLength  = data.inputValue(inBridgeLength).asDouble()
                         * data.inputValue(inDeckScale).asDouble();
    params.deckSegments     = data.inputValue(inDeckSegments).asInt();
    params.archHeight       = data.inputValue(inArchHeight).asDouble();
    params.stepSubdivisions = data.inputValue(inStepSubdivisions).asInt();
    params.bumpHeight       = data.inputValue(inBumpHeight).asDouble();
    params.bumpFreq         = data.inputValue(inBumpFreq).asDouble();
    params.chamfer          = data.inputValue(inChamfer).asDouble();
    params.voussoir         = data.inputValue(inVoussoirCount).asInt();
    params.voussoirTaper    = data.inputValue(inVoussoirTaper).asDouble();
    params.jointGap         = data.inputValue(inJointGap).asDouble();
    params.keystoneScale    = data.inputValue(inKeystoneScale).asDouble();
    params.noiseOctaves     = data.inputValue(inNoiseOctaves).asInt();
    params.noiseSeed        = data.inputValue(inNoiseSeed).asInt();
    params.weatherSubdiv    = data.inputValue(inWeatherSubdiv).asInt();

    MPointArray points;
    MIntArray   faceCounts;
    MIntArray   faceConnects;
    buildBridgeMesh(params, points, faceCounts, faceConnects);

    MFnMeshData meshDataFn;
    MObject     meshData = meshDataFn.create();

    if (points.length() > 0) {
        MFnMesh meshFn;
        meshFn.create(
            static_cast<int>(points.length()),
            static_cast<int>(faceCounts.length()),
            points, faceCounts, faceConnects,
            meshData);
    }

    MDataHandle outHandle = data.outputValue(outMesh);
    outHandle.set(meshData);
    data.setClean(plug);

    return MS::kSuccess;
}
