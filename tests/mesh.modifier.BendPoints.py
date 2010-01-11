#python

import k3d
import math
import testing

setup = testing.setup_mesh_modifier_test("PolyCylinder", "BendPoints")

setup.source.radius = 1

selection = k3d.geometry.selection.create(0)
point_selection = k3d.geometry.point_selection.create(selection, 1)
setup.modifier.mesh_selection = selection
setup.modifier.angle = math.radians(90)
setup.modifier.around = "y"


testing.require_valid_mesh(setup.document, setup.modifier.get_property("output_mesh"))
testing.mesh_reference_comparison(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.BendPoints", 1, testing.platform_specific)

