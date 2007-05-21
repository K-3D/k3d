#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyCylinder", "LinearWavePoints")

selection = k3d.deselect_all()
selection.points = k3d.component_select_all()

setup.source.u_segments = 4
setup.source.v_segments = 16

setup.modifier.mesh_selection = selection
setup.modifier.axis = "x"
setup.modifier.along = "z"

testing.mesh_comparison(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.LinearWavePoints", 0)

