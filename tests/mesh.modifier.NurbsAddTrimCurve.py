#python

import testing
import k3d

document = k3d.new_document()

patch = document.new_node("NurbsGrid")
curve = document.new_node("NurbsCircle")
merge_mesh = document.new_node("MergeMesh")
modifier = document.new_node("NurbsAddTrimCurve")

merge_mesh.create_property("k3d::mesh*", "input_mesh1", "Input Mesh 1", "")
merge_mesh.create_property("k3d::mesh*", "input_mesh2", "Input Mesh 2", "")

modifier.mesh_selection = k3d.geometry.selection.create(1)

document.set_dependency(merge_mesh.get_property("input_mesh1"), patch.get_property("output_mesh"))
document.set_dependency(merge_mesh.get_property("input_mesh2"), curve.get_property("output_mesh"))
document.set_dependency(modifier.get_property("input_mesh"), merge_mesh.get_property("output_mesh"))

testing.mesh_comparison_to_reference(document, modifier.get_property("output_mesh"), "mesh.modifier.NurbsAddTrimCurve", 1)
