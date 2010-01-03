#python

import testing
import k3d
from math import pi

document = k3d.new_document()

curve1 = document.new_node("NurbsCurve")
curve2 = document.new_node("NurbsCircle")
transform = document.new_node("RotatePoints")
merge_mesh = document.new_node("MergeMesh")
modifier = document.new_node("NurbsRuledSurface")

merge_mesh.create_property("k3d::mesh*", "input_mesh1", "Input Mesh 1", "")
merge_mesh.create_property("k3d::mesh*", "input_mesh2", "Input Mesh 2", "")

transform_selection = k3d.geometry.selection.create(0)
k3d.geometry.point_selection.create(transform_selection, 1)
transform.mesh_selection = transform_selection
transform.y = 0.5*pi

modifier.mesh_selection = k3d.geometry.selection.create(1)

document.set_dependency(transform.get_property("input_mesh"), curve2.get_property("output_mesh"))
document.set_dependency(merge_mesh.get_property("input_mesh1"), curve1.get_property("output_mesh"))
document.set_dependency(merge_mesh.get_property("input_mesh2"), transform.get_property("output_mesh"))
document.set_dependency(modifier.get_property("input_mesh"), merge_mesh.get_property("output_mesh"))

testing.mesh_reference_comparison(document, modifier.get_property("output_mesh"), "mesh.modifier.NurbsRuledSurface", 1)
