#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("NurbsGrid","NurbsSplitPatch")

setup.modifier.mesh_selection = k3d.mesh_selection.select_all()
setup.modifier.u_value = 0.147
setup.modifier.insert_to_v = True

testing.mesh_comparison_to_reference(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.NurbsSplitPatch2", 16)

