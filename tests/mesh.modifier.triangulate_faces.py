#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyTorus", "TriangulateFaces")

mesh_selection = k3d.select_all()
setup.modifier.mesh_selection = mesh_selection

testing.image_comparison(setup.document, setup.camera_to_bitmap.get_property("output_bitmap"), "mesh.modifier.triangulate_faces", 0.003)

