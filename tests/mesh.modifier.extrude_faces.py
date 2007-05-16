#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyGrid", "ExtrudeFaces")

setup.source.rows = 3
setup.source.columns = 3

mesh_selection = k3d.deselect_all()
mesh_selection.faces = [(4, 5, 1)]
setup.modifier.mesh_selection = mesh_selection

testing.mesh_comparison(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.extrude_faces")

