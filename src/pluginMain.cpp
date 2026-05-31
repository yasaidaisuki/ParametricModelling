#include "parametricStaircaseNode.h"
#include <maya/MFnPlugin.h>
#include <maya/MObject.h>

MStatus initializePlugin(MObject obj) {
    MFnPlugin plugin(obj, "ParametricModelling", "1.0", "Any");
    return plugin.registerNode(
        "parametricStaircaseNode",
        ParametricStaircaseNode::id,
        ParametricStaircaseNode::creator,
        ParametricStaircaseNode::initialize
    );
}

MStatus uninitializePlugin(MObject obj) {
    MFnPlugin plugin(obj);
    return plugin.deregisterNode(ParametricStaircaseNode::id);
}
