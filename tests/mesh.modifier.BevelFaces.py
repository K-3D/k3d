#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyCube", "BevelFaces")

selection = k3d.geometry.selection.create(0)
selection.faces = [(4, 5, 1)]
setup.modifier.mesh_selection = selection

testing.mesh_reference_comparison(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.BevelFaces", 1)

