#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("NurbsGrid","NurbsSplitPatch")

setup.modifier.mesh_selection = k3d.geometry.selection.create(1)
setup.modifier.u_value = 0.567
setup.modifier.insert_to_v = False

testing.require_valid_primitives(setup.document, setup.modifier.get_property("output_mesh"))
testing.mesh_reference_comparison(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.NurbsSplitPatch1", 2)

