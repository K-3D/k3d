#python

import testing
import k3d

document = k3d.new_document()

curve1 = document.new_node("NurbsCurve")
curve2 = document.new_node("NurbsCurve")
transform = document.new_node("TranslatePoints")
merge_mesh = document.new_node("MergeMesh")
modifier = document.new_node("NurbsConnectCurves")

merge_mesh.create_property("k3d::mesh*", "input_mesh1", "Input Mesh 1", "")
merge_mesh.create_property("k3d::mesh*", "input_mesh2", "Input Mesh 2", "")

transform_selection = k3d.geometry.selection.create(0)
k3d.geometry.point_selection.create(transform_selection, 1)
transform.mesh_selection = transform_selection
transform.y = 3

selection = k3d.geometry.selection.create(0)
point_selection = k3d.geometry.point_selection.create(selection)
k3d.geometry.point_selection.append(point_selection, 0, 8, 0)
k3d.geometry.point_selection.append(point_selection, 0, 1, 1)
k3d.geometry.point_selection.append(point_selection, 4, 5, 1)
modifier.mesh_selection = selection

document.set_dependency(transform.get_property("input_mesh"), curve2.get_property("output_mesh"))
document.set_dependency(merge_mesh.get_property("input_mesh1"), curve1.get_property("output_mesh"))
document.set_dependency(merge_mesh.get_property("input_mesh2"), transform.get_property("output_mesh"))
document.set_dependency(modifier.get_property("input_mesh"), merge_mesh.get_property("output_mesh"))

testing.mesh_reference_comparison(document, modifier.get_property("output_mesh"), "mesh.modifier.NurbsConnectCurves", 1)
