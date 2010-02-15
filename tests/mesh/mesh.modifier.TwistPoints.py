#python

import k3d
import math
import testing

setup = testing.setup_mesh_modifier_test("PolyCube", "TwistPoints")

selection = k3d.geometry.selection.create(0)
selection.points = k3d.geometry.point_selection.create(selection, 1)

setup.modifier.mesh_selection = selection
setup.modifier.angle = math.radians(20.0)


testing.require_valid_mesh(setup.document, setup.modifier.get_property("output_mesh"))
testing.require_similar_mesh(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.TwistPoints", 1, testing.platform_specific)

