#python

import k3d
import testing

setup = testing.setup_mesh_modifier_image_test("PolyGrid", "ExtrudeFaces")

setup.source.rows = 3
setup.source.columns = 3

mesh_selection = k3d.deselect_all()
mesh_selection.faces = [(4, 5, 1)]
setup.modifier.mesh_selection = mesh_selection
setup.modifier.distance = 5.0

setup.mesh_instance.show_component_selection = True

testing.image_comparison(setup.document, setup.camera_to_bitmap.get_property("output_bitmap"), "mesh.modifier.extrude_faces", 0.003)

