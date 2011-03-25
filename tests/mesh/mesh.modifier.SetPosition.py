#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyCube", "SetPosition")

selection = k3d.geometry.selection.create(0)
selection.points = k3d.geometry.point_selection.create(selection, 1)

setup.modifier.mesh_selection = selection
setup.modifier.use_y = False
setup.modifier.use_z = False


testing.require_valid_mesh(setup.document, setup.modifier.get_property("output_mesh"))
testing.require_similar_mesh(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.SetPosition", 1)
