#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyCube", "TBBScalePoints")

selection = k3d.deselect_all()
selection.points = k3d.component_select_all()

setup.modifier.mesh_selection = selection
setup.modifier.x = 2.0

timer = testing.timer()
print setup.modifier.output_mesh
print timer.elapsed()

