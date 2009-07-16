#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyGrid", "LinearPointNoise")

selection = k3d.geometry.uniform_selection(0)
selection.points = k3d.geometry.point_selection.uniform(selection, 1)

setup.modifier.mesh_selection = selection
setup.modifier.amplitude_x = 2
setup.modifier.amplitude_y = 2
setup.modifier.amplitude_z = 2

testing.mesh_comparison_to_reference(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.LinearPointNoise", 16)

