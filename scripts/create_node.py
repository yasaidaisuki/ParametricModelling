"""
Run this in Maya's Script Editor (Python tab) after loading the plug-in.
It creates a parametricStaircaseNode, wires it to a visible mesh shape,
and sets some default parameter values.
"""
import maya.cmds as cmds

# Load the plug-in (adjust the path to where you placed the .mll)
cmds.loadPlugin("ParametricStaircaseNode")

# Create the generator node
node = cmds.createNode("parametricStaircaseNode", name="staircase1")

# Create a transform + mesh shape to receive the output
xform = cmds.createNode("transform", name="staircaseMesh")
shape = cmds.createNode("mesh", parent=xform, name="staircaseMeshShape")

# Connect the node's output mesh to the mesh shape
cmds.connectAttr(f"{node}.outMesh", f"{shape}.inMesh", force=True)

# Set initial parameter values
cmds.setAttr(f"{node}.totalHeight", 3.0)
cmds.setAttr(f"{node}.stairWidth",  2.0)
cmds.setAttr(f"{node}.treadDepth",  0.3)
cmds.setAttr(f"{node}.stepCount",   10)

# Assign a default shader so the mesh is visible
cmds.sets(shape, edit=True, forceElement="initialShadingGroup")

print(f"Created '{node}' connected to '{shape}'")
print("Adjust attributes in the Channel Box or Attribute Editor to update the mesh.")
