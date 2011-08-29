#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyCube", "MakeHole")

selection = k3d.geometry.selection.create(1)

setup.modifier.mesh_selection = selection


testing.require_valid_mesh(setup.document, setup.modifier.get_property("output_mesh"))
testing.require_similar_mesh(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.MakeHole", 1)
