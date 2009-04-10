#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("NurbsCurve","NurbsSplitCurve")

setup.modifier.mesh_selection = k3d.mesh_selection.select_all()
setup.modifier.u_value = 0.5

testing.mesh_comparison_to_reference(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.NurbsSplitCurve", 1)

