#python

import testing
import k3d

document = k3d.new_document()

patch = k3d.plugin.create("NurbsGrid", document)
curve = k3d.plugin.create("NurbsCircle", document)
merge_mesh = k3d.plugin.create("MergeMesh", document)
modifier = k3d.plugin.create("NurbsAddTrimCurve", document)

k3d.property.create(merge_mesh, "k3d::mesh*", "input_mesh1", "Input Mesh 1", "")
k3d.property.create(merge_mesh, "k3d::mesh*", "input_mesh2", "Input Mesh 2", "")

modifier.mesh_selection = k3d.geometry.selection.create(1)
modifier.offset_u = 0.5
modifier.offset_v = 0.5

k3d.property.connect(document, patch.get_property("output_mesh"), merge_mesh.get_property("input_mesh1"))
k3d.property.connect(document, curve.get_property("output_mesh"), merge_mesh.get_property("input_mesh2"))
k3d.property.connect(document, merge_mesh.get_property("output_mesh"), modifier.get_property("input_mesh"))

testing.require_valid_mesh(document, modifier.get_property("output_mesh"))
testing.require_similar_mesh(document, modifier.get_property("output_mesh"), "mesh.modifier.NurbsAddTrimCurve", 1, ["Darwin-i386"]
)
