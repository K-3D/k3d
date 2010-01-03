#python

import k3d
import testing

document = k3d.new_document()

source1 = document.new_node("PolyCube")
source2 = document.new_node("PolyCube")

merge = document.new_node("MergeMesh")
merge.create_property("k3d::mesh*", "input_mesh1", "Input Mesh 1", "")
merge.create_property("k3d::mesh*", "input_mesh2", "Input Mesh 2", "")
document.set_dependency(merge.get_property("input_mesh1"), source1.get_property("output_mesh"))
document.set_dependency(merge.get_property("input_mesh2"), source1.get_property("output_mesh"))

weld = document.new_node("Weld")
weld.distance = 0.5
document.set_dependency(weld.get_property("input_mesh"), merge.get_property("output_mesh"))

testing.require_valid_primitives(document, weld.get_property("output_mesh"))
testing.mesh_reference_comparison(document, weld.get_property("output_mesh"), "mesh.modifier.Weld", 1)

