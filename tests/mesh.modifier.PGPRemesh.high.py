#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test2("PolyTorus", "TriangulateFaces", "PGPRemesh")
setup.modifier1.mesh_selection = k3d.select_all()
setup.modifier2.use_smooth = False
setup.modifier2.steps = 0
setup.modifier2.omega = 5
setup.modifier2.div = 17
setup.modifier2.triangulate = True

testing.require_valid_mesh(setup.document, setup.modifier2.get_property("output_mesh"))
testing.mesh_reference_comparison(setup.document, setup.modifier2.get_property("output_mesh"), "mesh.modifier.PGPRemesh.high", 1)

