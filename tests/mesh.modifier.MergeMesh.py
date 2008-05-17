#python

import k3d
import testing

document = k3d.new_document()

source = document.new_node("PolyCube")
source2 = document.new_node("PolyCube")
source3 = document.new_node("NurbsSphere")
source4 = document.new_node("NurbsSphere")
source5 = document.new_node("NurbsCurve")
source6 = document.new_node("NurbsCurve")
source7 = document.new_node("LissajousCurve")
source8 = document.new_node("KnotCurve")
source9 = document.new_node("Newell")
source10 = document.new_node("Newell")
modifier = document.new_node("MergeMesh")

modifier.create_property("k3d::mesh*", "input_mesh1", "Input Mesh 1", "")
modifier.create_property("k3d::mesh*", "input_mesh2", "Input Mesh 2", "")
modifier.create_property("k3d::mesh*", "input_mesh3", "Input Mesh 3", "")
modifier.create_property("k3d::mesh*", "input_mesh4", "Input Mesh 4", "")
modifier.create_property("k3d::mesh*", "input_mesh5", "Input Mesh 5", "")
modifier.create_property("k3d::mesh*", "input_mesh6", "Input Mesh 6", "")
modifier.create_property("k3d::mesh*", "input_mesh7", "Input Mesh 7", "")
modifier.create_property("k3d::mesh*", "input_mesh8", "Input Mesh 8", "")
modifier.create_property("k3d::mesh*", "input_mesh9", "Input Mesh 9", "")
modifier.create_property("k3d::mesh*", "input_mesh10", "Input Mesh 10", "")

document.set_dependency(modifier.get_property("input_mesh1"), source.get_property("output_mesh"))
document.set_dependency(modifier.get_property("input_mesh2"), source2.get_property("output_mesh"))
document.set_dependency(modifier.get_property("input_mesh3"), source3.get_property("output_mesh"))
document.set_dependency(modifier.get_property("input_mesh4"), source4.get_property("output_mesh"))
document.set_dependency(modifier.get_property("input_mesh5"), source5.get_property("output_mesh"))
document.set_dependency(modifier.get_property("input_mesh6"), source6.get_property("output_mesh"))
document.set_dependency(modifier.get_property("input_mesh7"), source7.get_property("output_mesh"))
document.set_dependency(modifier.get_property("input_mesh8"), source8.get_property("output_mesh"))
document.set_dependency(modifier.get_property("input_mesh9"), source9.get_property("output_mesh"))
document.set_dependency(modifier.get_property("input_mesh10"), source10.get_property("output_mesh"))

testing.mesh_comparison(document, modifier.get_property("output_mesh"), "mesh.modifier.MergeMesh", 1)

