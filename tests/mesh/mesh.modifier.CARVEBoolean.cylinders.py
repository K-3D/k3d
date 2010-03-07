#python

import k3d
import testing

document = k3d.new_document()

small_cylinder = k3d.plugin.create("PolyCylinder", document)
small_cylinder.radius = 2.0
small_cylinder.zmax = 7.5
small_cylinder.zmin = -7.5
small_cylinder.u_segments = 8
big_cylinder = k3d.plugin.create("PolyCylinder", document)
big_cylinder.u_segments = 8
torus = k3d.plugin.create("PolyTorus", document)
torus.u_segments = 8
torus.v_segments = 4

first_boolean = k3d.plugin.create("CARVEBoolean", document)
first_boolean.type = "difference"
first_boolean.create_property("k3d::mesh*", "input_1", "Input 1", "")
first_boolean.create_property("k3d::mesh*", "input_2", "Input 2", "")
second_boolean = k3d.plugin.create("CARVEBoolean", document)
second_boolean.type = "reverse_difference"
second_boolean.create_property("k3d::mesh*", "input_1", "Input 1", "")
second_boolean.create_property("k3d::mesh*", "input_2", "Input 2", "")

document.set_dependency(first_boolean.get_property("input_1"), big_cylinder.get_property("output_mesh"))
document.set_dependency(first_boolean.get_property("input_2"), small_cylinder.get_property("output_mesh"))
document.set_dependency(second_boolean.get_property("input_1"), torus.get_property("output_mesh"))
document.set_dependency(second_boolean.get_property("input_2"), first_boolean.get_property("output_mesh"))


testing.require_valid_mesh(document, second_boolean.get_property("output_mesh"))
testing.require_similar_mesh(document, second_boolean.get_property("output_mesh"), "mesh.modifier.CARVEBoolean.cylinders", 1)
