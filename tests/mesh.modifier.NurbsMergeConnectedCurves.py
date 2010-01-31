#python

import k3d
import testing

document = k3d.new_document()

selection = k3d.geometry.selection.create(1)
circle = document.new_node("NurbsCircle")
split1 = document.new_node("NurbsSplitCurve")
split2 = document.new_node("NurbsSplitCurve")
merge = document.new_node("NurbsMergeConnectedCurves")

split1.mesh_selection = selection
split2.mesh_selection = selection
merge.mesh_selection = selection

document.set_dependency(split1.get_property("input_mesh"), circle.get_property("output_mesh"))
document.set_dependency(split2.get_property("input_mesh"), split1.get_property("output_mesh"))
document.set_dependency(merge.get_property("input_mesh"), split2.get_property("output_mesh"))


testing.require_valid_mesh(document, merge.get_property("output_mesh"))
testing.require_similar_mesh(document, merge.get_property("output_mesh"), "mesh.modifier.NurbsMergeConnectedCurves", 1)
