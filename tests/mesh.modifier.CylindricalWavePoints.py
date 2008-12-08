#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyCylinder", "CylindricalWavePoints")

selection = k3d.mesh_selection.deselect_all()
selection.points = k3d.mesh_selection.component_select_all()

setup.source.u_segments = 4
setup.source.v_segments = 16

setup.modifier.mesh_selection = selection
setup.modifier.amplitude = 1

testing.mesh_comparison_to_reference(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.CylindricalWavePoints", 0)

