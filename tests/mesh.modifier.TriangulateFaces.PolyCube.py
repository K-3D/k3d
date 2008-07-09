#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyCube", "TriangulateFaces")

mesh_selection = k3d.select_all()
setup.modifier.mesh_selection = mesh_selection

testing.mesh_comparison_to_reference(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.TriangulateFaces.PolyCube", 0)

