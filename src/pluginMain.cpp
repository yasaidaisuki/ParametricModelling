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
    if (!status) {
        status.perror("registerNode parametricStaircaseNode failed");
        return status;
    }

    status = plugin.registerNode(
        "parametricBridgeNode",
        ParametricBridgeNode::id,
        ParametricBridgeNode::creator,
        ParametricBridgeNode::initialize
    );
    if (!status) {
        // Don't leave the plugin half-registered: roll back the staircase node so
        // loadPlugin reports overall failure instead of silently exposing one node.
        status.perror("registerNode parametricBridgeNode failed");
        plugin.deregisterNode(ParametricStaircaseNode::id);
        return status;
    }

    return status;
}

MStatus uninitializePlugin(MObject obj) {
    MFnPlugin plugin(obj);

    plugin.deregisterNode(ParametricBridgeNode::id);
    return plugin.deregisterNode(ParametricStaircaseNode::id);
}
