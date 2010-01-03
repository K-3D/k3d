#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("Polyhedron", "SelectNSided")

setup.source.number = 2
setup.modifier.sides = 4
setup.modifier.operator = "greater_or_equal"


testing.require_valid_primitives(setup.document, setup.modifier.get_property("output_mesh"))
testing.mesh_reference_comparison(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.SelectNSided", 1)

