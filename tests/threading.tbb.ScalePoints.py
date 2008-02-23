#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyGrid", "TBBScalePoints")
setup.source.rows = 300
setup.source.columns = 300

selection = k3d.deselect_all()
selection.points = k3d.component_select_all()

setup.modifier.mesh_selection = selection
setup.modifier.x = 2.0

print "point_count,thread_count,grain_size,seconds"

point_count = (setup.source.rows + 1) * (setup.source.columns + 1)

for thread_count in range(1, 5):
	setup.modifier.thread_count = thread_count
	for grain_size in range(point_count / 20, point_count, point_count / 20):
		setup.modifier.grain_size = grain_size
		mesh = setup.modifier.output_mesh

