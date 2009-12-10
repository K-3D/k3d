#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyTorus", "QSlim")

setup.modifier.face_count = 100

testing.mesh_comparison_to_reference(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.QSlim", 1)

