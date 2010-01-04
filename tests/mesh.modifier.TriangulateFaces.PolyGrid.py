#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyGrid", "TriangulateFaces")
setup.source.rows = 1
setup.source.columns = 1

mesh_selection = k3d.geometry.selection.create(1)
setup.modifier.mesh_selection = mesh_selection


testing.require_valid_mesh(setup.document, setup.modifier.get_property("output_mesh"))
testing.mesh_reference_comparison(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.TriangulateFaces.PolyGrid", 0)

