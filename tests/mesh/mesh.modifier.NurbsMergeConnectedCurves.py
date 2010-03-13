#python

import k3d
import testing

document = k3d.new_document()

selection = k3d.geometry.selection.create(1)
circle = k3d.plugin.create("NurbsCircle", document)
split1 = k3d.plugin.create("NurbsSplitCurve", document)
split2 = k3d.plugin.create("NurbsSplitCurve", document)
merge = k3d.plugin.create("NurbsMergeConnectedCurves", document)

split1.mesh_selection = selection
split2.mesh_selection = selection
merge.mesh_selection = selection

k3d.property.connect(document, circle.get_property("output_mesh"), split1.get_property("input_mesh"))
k3d.property.connect(document, split1.get_property("output_mesh"), split2.get_property("input_mesh"))
k3d.property.connect(document, split2.get_property("output_mesh"), merge.get_property("input_mesh"))

testing.require_valid_mesh(document, merge.get_property("output_mesh"))
testing.require_similar_mesh(document, merge.get_property("output_mesh"), "mesh.modifier.NurbsMergeConnectedCurves", 1)

