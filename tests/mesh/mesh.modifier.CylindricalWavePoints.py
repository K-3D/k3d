#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyCylinder", "CylindricalWavePoints")

selection = k3d.geometry.selection.create(0)
selection.points = k3d.geometry.point_selection.create(selection, 1)

setup.source.u_segments = 4
setup.source.v_segments = 16

setup.modifier.mesh_selection = selection
setup.modifier.amplitude = 1


testing.require_valid_mesh(setup.document, setup.modifier.get_property("output_mesh"))
testing.require_similar_mesh(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.CylindricalWavePoints", 1)

