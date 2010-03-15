#python

import k3d
import testing

document = k3d.new_document()

small_cylinder = k3d.plugin.create("PLYMeshReader", document)
small_cylinder.file = k3d.filesystem.generic_path(testing.source_path() + "/meshes/mesh.modifier.CARVEBoolean.input.b.ply")
small_cylinder.center = False
small_cylinder.scale_to_size = False
big_cylinder = k3d.plugin.create("PLYMeshReader", document)
big_cylinder.file = k3d.filesystem.generic_path(testing.source_path() + "/meshes/mesh.modifier.CARVEBoolean.input.a.ply")
big_cylinder.center = False
big_cylinder.scale_to_size = False

carve_boolean = k3d.plugin.create("CARVEBoolean", document)
carve_boolean.type = "difference"
k3d.property.create(carve_boolean, "k3d::mesh*", "input_1", "Input 1", "")
k3d.property.create(carve_boolean, "k3d::mesh*", "input_2", "Input 2", "")

k3d.property.connect(document, big_cylinder.get_property("output_mesh"), carve_boolean.get_property("input_1"))
k3d.property.connect(document, small_cylinder.get_property("output_mesh"), carve_boolean.get_property("input_2"))

testing.require_valid_mesh(document, carve_boolean.get_property("output_mesh"))
testing.require_similar_mesh(document, carve_boolean.get_property("output_mesh"), "mesh.modifier.CARVEBoolean.ply", 10)

