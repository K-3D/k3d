#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyGrid", "ScalePoints")

setup.source.rows = 1
setup.source.columns = 1

mesh_selection = k3d.select_all()
setup.modifier.mesh_selection = mesh_selection
setup.modifier.x = 2

testing.image_comparison(setup.document, setup.camera_to_bitmap.get_property("output_bitmap"), "mesh.selection.all", 0.008)

