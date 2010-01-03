#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyGrid", "ScalePoints")

setup.source.rows = 1
setup.source.columns = 1

mesh_selection = k3d.geometry.selection.create(1.0)
setup.modifier.mesh_selection = mesh_selection
setup.modifier.x = 2

testing.require_valid_primitives(setup.document, setup.modifier.get_property("output_mesh"))
testing.mesh_reference_comparison(setup.document, setup.modifier.get_property("output_mesh"), "mesh.selection.all", 1)

