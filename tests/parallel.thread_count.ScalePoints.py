#python

import k3d
import testing

# Create a mesh source and our test modifier ...
setup = testing.setup_mesh_modifier_test("PolyGrid", "ScalePoints")
setup.source.rows = 300
setup.source.columns = 300

selection = k3d.mesh_selection.deselect_all()
selection.points = k3d.mesh_selection.component_select_all()
setup.modifier.mesh_selection = selection

# Force an update of the mesh source output, so it doesn't interfere with our timing
mesh = setup.source.output_mesh


for thread_count in range(1, 9):
	k3d.parallel.set_grain_size(10000)
	k3d.parallel.set_thread_count(thread_count)
	setup.modifier.x = 1.0
	setup.modifier.x = 2.0

	timer = testing.timer()
	mesh = setup.modifier.output_mesh
	print "Thread Count:", thread_count, "Time:", timer.elapsed()

