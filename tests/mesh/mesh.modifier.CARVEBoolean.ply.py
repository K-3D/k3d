#python

import k3d
import testing

document = k3d.new_document()

small_cylinder = document.new_node("PLYMeshReader")
small_cylinder.file = k3d.filesystem.generic_path(testing.source_path() + "/meshes/mesh.modifier.CARVEBoolean.input.b.ply")
small_cylinder.center = False
small_cylinder.scale_to_size = False
big_cylinder = document.new_node("PLYMeshReader")
big_cylinder.file = k3d.filesystem.generic_path(testing.source_path() + "/meshes/mesh.modifier.CARVEBoolean.input.a.ply")
big_cylinder.center = False
big_cylinder.scale_to_size = False

carve_boolean = document.new_node("CARVEBoolean")
carve_boolean.type = "difference"
carve_boolean.create_property("k3d::mesh*", "input_1", "Input 1", "")
carve_boolean.create_property("k3d::mesh*", "input_2", "Input 2", "")

document.set_dependency(carve_boolean.get_property("input_1"), big_cylinder.get_property("output_mesh"))
document.set_dependency(carve_boolean.get_property("input_2"), small_cylinder.get_property("output_mesh"))

testing.require_valid_mesh(document, carve_boolean.get_property("output_mesh"))
testing.require_similar_mesh(document, carve_boolean.get_property("output_mesh"), "mesh.modifier.CARVEBoolean.ply", 10)

