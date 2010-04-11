#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("UniformPolyhedron", "SelectNSided")

setup.source.type = "gyroelongated_pentagonal_bicupola"
setup.modifier.sides = 4
setup.modifier.operator = "greater_or_equal"

testing.require_valid_mesh(setup.document, setup.modifier.get_property("output_mesh"))
testing.require_similar_mesh(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.SelectNSided", 1)

