#python

import k3d
import testing

document = k3d.new_document()

source1= document.new_node("PolyCube")
source1.width = 2.0
source1.height = 2.0
source1.depth = 2.0

source2 = document.new_node("PolyCube")
source2.width = 4.0
source2.height = 4.0
source2.depth = 4.0

modifier1 = document.new_node("MakePointGroup")
modifier1.width = 2.0

modifier2 = document.new_node("MakePointGroup")
modifier2.width = 3.0

document.set_dependency(modifier1.get_property("input_mesh"), source1.get_property("output_mesh"))
document.set_dependency(modifier2.get_property("input_mesh"), source2.get_property("output_mesh"))

merge_mesh = document.new_node("MergeMesh")
merge_mesh.create_property("k3d::mesh*", "input_mesh1", "Input Mesh 1", "")
merge_mesh.create_property("k3d::mesh*", "input_mesh2", "Input Mesh 2", "")

document.set_dependency(merge_mesh.get_property("input_mesh1"), modifier1.get_property("output_mesh"))
document.set_dependency(merge_mesh.get_property("input_mesh2"), modifier2.get_property("output_mesh"))

testing.mesh_comparison_to_reference(document, merge_mesh.get_property("output_mesh"), "mesh.modifier.MergeMesh.generic", 2)

