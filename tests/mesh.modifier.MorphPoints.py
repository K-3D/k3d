#python

import k3d
import testing

document = k3d.new_document()

source = document.new_node("PolyCone")
source2 = document.new_node("PolySphere")
source3 = document.new_node("PolyCube")
modifier = document.new_node("MorphPoints")

modifier.create_property("k3d::mesh*", "input_mesh2", "input_mesh2", "input_mesh2")
modifier.create_property("k3d::double_t", "input_mesh2_percent", "input_mesh2_percent", "input_mesh2_percent")

document.set_dependency(modifier.get_property("input_mesh"), source.get_property("output_mesh"))
document.set_dependency(modifier.get_property("input_mesh1"), source2.get_property("output_mesh"))
document.set_dependency(modifier.get_property("input_mesh2"), source3.get_property("output_mesh"))

selection = k3d.deselect_all()
selection.points = k3d.component_select_all()

modifier.mesh_selection = selection

modifier.input_mesh1_percent = 10.
modifier.input_mesh2_percent = 10.

#mesh = modifier.output_mesh

testing.mesh_comparison(document, modifier.get_property("output_mesh"), "mesh.modifier.MorphPoints", 1)
