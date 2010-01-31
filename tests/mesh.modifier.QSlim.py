#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyTorus", "QSlim")

setup.modifier.face_count = 100


testing.require_valid_mesh(setup.document, setup.modifier.get_property("output_mesh"))
testing.require_similar_mesh(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.QSlim", 1)

