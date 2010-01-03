#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("NurbsGrid","NurbsPatchInsertKnot")

setup.modifier.mesh_selection = k3d.geometry.selection.create(1)
setup.modifier.u_value = 0.735
setup.modifier.multiplicity = 2
setup.modifier.insert_to_v = False


testing.require_valid_primitives(setup.document, setup.modifier.get_property("output_mesh"))
testing.mesh_reference_comparison(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.NurbsPatchInsertKnot", 32)

