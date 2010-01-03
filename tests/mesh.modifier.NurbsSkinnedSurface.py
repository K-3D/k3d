#python

import testing
import k3d

document = k3d.new_document()

curve1 = document.new_node("NurbsCurve")
curve2 = document.new_node("NurbsCurve")
curve3 = document.new_node("NurbsCurve")
transform1 = document.new_node("TranslatePoints")
transform2 = document.new_node("TranslatePoints")
merge_mesh = document.new_node("MergeMesh")
modifier = document.new_node("NurbsSkinnedSurface")

merge_mesh.create_property("k3d::mesh*", "input_mesh1", "Input Mesh 1", "")
merge_mesh.create_property("k3d::mesh*", "input_mesh2", "Input Mesh 2", "")
merge_mesh.create_property("k3d::mesh*", "input_mesh3", "Input Mesh 3", "")

transform1.mesh_selection = k3d.geometry.selection.create(1)
transform1.y = 3
transform2.mesh_selection = k3d.geometry.selection.create(1)
transform2.z = 3

modifier.mesh_selection = k3d.geometry.selection.create(1)

document.set_dependency(transform1.get_property("input_mesh"), curve2.get_property("output_mesh"))
document.set_dependency(transform2.get_property("input_mesh"), curve3.get_property("output_mesh"))
document.set_dependency(merge_mesh.get_property("input_mesh1"), curve1.get_property("output_mesh"))
document.set_dependency(merge_mesh.get_property("input_mesh2"), transform1.get_property("output_mesh"))
document.set_dependency(merge_mesh.get_property("input_mesh3"), transform2.get_property("output_mesh"))
document.set_dependency(modifier.get_property("input_mesh"), merge_mesh.get_property("output_mesh"))

testing.require_valid_primitives(document, modifier.get_property("output_mesh"))
testing.mesh_reference_comparison(document, modifier.get_property("output_mesh"), "mesh.modifier.NurbsSkinnedSurface", 1)
