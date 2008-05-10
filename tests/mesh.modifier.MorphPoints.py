#python

import k3d
import testing

document = k3d.new_document()

source1 = document.new_node("PolyCone")
source2 = document.new_node("PolySphere")
source3 = document.new_node("PolyCube")
modifier = document.new_node("MorphPoints")

modifier.create_property("k3d::mesh*", "input1", "input1", "input1")
modifier.create_property("k3d::mesh*", "input2", "input2", "input2")
modifier.create_property("k3d::double_t", "amount1", "amount1", "amount1")
modifier.create_property("k3d::double_t", "amount2", "amount2", "amount2")

document.set_dependency(modifier.get_property("input_mesh"), source1.get_property("output_mesh"))
document.set_dependency(modifier.get_property("input1"), source2.get_property("output_mesh"))
document.set_dependency(modifier.get_property("input2"), source3.get_property("output_mesh"))

selection = k3d.deselect_all()
selection.points = k3d.component_select_all()

modifier.mesh_selection = selection

modifier.amount1 = 0.1
modifier.amount2 = 0.1 

testing.mesh_comparison(document, modifier.get_property("output_mesh"), "mesh.modifier.MorphPoints", 4)

