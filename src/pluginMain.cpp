#include "parametricStaircaseNode.h"
#include "parametricBridgeNode.h"
#include <maya/MFnPlugin.h>
#include <maya/MObject.h>

MStatus initializePlugin(MObject obj) {
    MFnPlugin plugin(obj, "ParametricModelling", "1.0", "Any");

    MStatus status;

    status = plugin.registerNode(
        "parametricStaircaseNode",
        ParametricStaircaseNode::id,
        ParametricStaircaseNode::creator,
        ParametricStaircaseNode::initialize
    );
    if (!status) return status;

    status = plugin.registerNode(
        "parametricBridgeNode",
        ParametricBridgeNode::id,
        ParametricBridgeNode::creator,
        ParametricBridgeNode::initialize
    );
    return status;
}

MStatus uninitializePlugin(MObject obj) {
    MFnPlugin plugin(obj);

    plugin.deregisterNode(ParametricBridgeNode::id);
    return plugin.deregisterNode(ParametricStaircaseNode::id);
}
