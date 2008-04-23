#python

import k3d
import testing

document = k3d.new_document()

source = document.new_node("PolyCube")
source2 = document.new_node("PolyCube")
modifier = document.new_node("MergeMesh")

modifier.create_property("k3d::mesh*", "input_mesh1", "Input 1", "")
modifier.create_property("k3d::mesh*", "input_mesh2", "Input 2", "")

document.set_dependency(modifier.get_property("input_mesh1"), source.get_property("output_mesh"))
document.set_dependency(modifier.get_property("input_mesh2"), source2.get_property("output_mesh"))

testing.mesh_comparison(document, modifier.get_property("output_mesh"), "mesh.modifier.MergeMesh", 1)

