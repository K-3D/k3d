#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyGrid", "ScalePoints")

setup.source.rows = 1
setup.source.columns = 1

mesh_selection = k3d.select_all()
setup.modifier.mesh_selection = mesh_selection
setup.modifier.x = 2

testing.mesh_comparison_to_reference(setup.document, setup.modifier.get_property("output_mesh"), "mesh.selection.all", 1)

