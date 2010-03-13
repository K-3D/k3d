#python

import testing
import k3d

document = k3d.new_document()

curve1 = k3d.plugin.create("NurbsCurve", document)
curve2 = k3d.plugin.create("NurbsCurve", document)
curve3 = k3d.plugin.create("NurbsCurve", document)
transform1 = k3d.plugin.create("TranslatePoints", document)
transform2 = k3d.plugin.create("TranslatePoints", document)
merge_mesh = k3d.plugin.create("MergeMesh", document)
modifier = k3d.plugin.create("NurbsSkinnedSurface", document)

merge_mesh.create_property("k3d::mesh*", "input_mesh1", "Input Mesh 1", "")
merge_mesh.create_property("k3d::mesh*", "input_mesh2", "Input Mesh 2", "")
merge_mesh.create_property("k3d::mesh*", "input_mesh3", "Input Mesh 3", "")

transform1.mesh_selection = k3d.geometry.selection.create(1)
transform1.y = 3
transform2.mesh_selection = k3d.geometry.selection.create(1)
transform2.z = 3

modifier.mesh_selection = k3d.geometry.selection.create(1)

k3d.property.connect(document, curve2.get_property("output_mesh"), transform1.get_property("input_mesh"))
k3d.property.connect(document, curve3.get_property("output_mesh"), transform2.get_property("input_mesh"))
k3d.property.connect(document, curve1.get_property("output_mesh"), merge_mesh.get_property("input_mesh1"))
k3d.property.connect(document, transform1.get_property("output_mesh"), merge_mesh.get_property("input_mesh2"))
k3d.property.connect(document, transform2.get_property("output_mesh"), merge_mesh.get_property("input_mesh3"))
k3d.property.connect(document, merge_mesh.get_property("output_mesh"), modifier.get_property("input_mesh"))

testing.require_valid_mesh(document, modifier.get_property("output_mesh"))
testing.require_similar_mesh(document, modifier.get_property("output_mesh"), "mesh.modifier.NurbsSkinnedSurface", 1)
