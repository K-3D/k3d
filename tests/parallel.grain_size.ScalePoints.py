#python

import k3d
import testing

# Create a mesh source and our test modifier ...
setup = testing.setup_mesh_modifier_test("PolyGrid", "TBBScalePoints")
setup.source.rows = 300
setup.source.columns = 300

selection = k3d.deselect_all()
selection.points = k3d.component_select_all()
setup.modifier.mesh_selection = selection

# Force an update of the mesh source output, so it doesn't interfere with our timing
mesh = setup.source.output_mesh

for grain_size in range(5000, 100000, 5000):
	k3d.set_parallel_grain_size(grain_size)
	k3d.set_parallel_thread_count(2)
	setup.modifier.x = 1.0
	setup.modifier.x = 2.0

	timer = testing.timer()
	mesh = setup.modifier.output_mesh
	print "Grain Size:", grain_size, "Time:", timer.elapsed()

