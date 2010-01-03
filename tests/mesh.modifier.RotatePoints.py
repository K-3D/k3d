#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyCube", "RotatePoints")

selection = k3d.geometry.selection.create(0)
selection.points = k3d.geometry.point_selection.create(selection, 1)

setup.modifier.mesh_selection = selection
setup.modifier.z = 1.0


testing.require_valid_primitives(setup.document, setup.modifier.get_property("output_mesh"))
testing.mesh_reference_comparison(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.RotatePoints", 1, testing.platform_specific)

