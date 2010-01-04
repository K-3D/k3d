#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyCube", "TriangulateFaces")

mesh_selection = k3d.geometry.selection.create(1)
setup.modifier.mesh_selection = mesh_selection


testing.require_valid_mesh(setup.document, setup.modifier.get_property("output_mesh"))
testing.mesh_reference_comparison(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.TriangulateFaces.PolyCube", 0)

