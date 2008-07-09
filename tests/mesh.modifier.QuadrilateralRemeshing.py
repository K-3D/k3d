#python

import k3d
import testing

document = k3d.new_document()

source = document.new_node("PolySphere")

triangles = document.new_node("TriangulateFaces")
triangles.mesh_selection = k3d.select_all()
document.set_dependency(triangles.get_property("input_mesh"), source.get_property("output_mesh"))

modifier = document.new_node("QuadrilateralRemeshing")
modifier.min_index = 0
modifier.max_index = 481
modifier.gradient_spacing = 1.1
modifier.iso_spacing = 1.1
document.set_dependency(modifier.get_property("input_mesh"), triangles.get_property("output_mesh"))

testing.mesh_comparison_to_reference(document, modifier.get_property("output_mesh"), "mesh.modifier.QuadrilateralRemeshing", 1)
