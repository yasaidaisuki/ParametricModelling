#include "parametricBridgeNode.h"
#include "meshBuilder.h"

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

    outMesh = tAttr.create("outMesh", "om", MFnData::kMesh);
    tAttr.setStorable(false);
    tAttr.setWritable(false);
    addAttribute(outMesh);

    attributeAffects(inTotalHeight,  outMesh);
    attributeAffects(inStairWidth,   outMesh);
    attributeAffects(inTreadDepth,   outMesh);
    attributeAffects(inStepCount,    outMesh);
    attributeAffects(inBridgeLength, outMesh);
    attributeAffects(inDeckScale,    outMesh);

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
    params.bridgeLength = data.inputValue(inBridgeLength).asDouble()
                        * data.inputValue(inDeckScale).asDouble();

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
