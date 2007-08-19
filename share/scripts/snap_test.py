#python

import k3d
import sys

doc = Document
doc.start_change_set()
try:
	floor = doc.new_node("PolyCube")
	floor.name = "Floor"
	floor.width = 10
	floor.height = 10
	floor.depth = 2

	floor_instance = doc.new_node("MeshInstance")
	floor_instance.name = "Floor Instance"
	floor_instance.gl_painter = doc.get_node("GL Default Painter")
	floor_instance.ri_painter = doc.get_node("RenderMan Default Painter")
	doc.set_dependency(floor_instance.get_property("input_mesh"), floor.get_property("output_mesh"))

	k3d.dynamic_cast(floor_instance, "isnappable").add_snap_target("-x", k3d.point3(-5, 0, 0))
	k3d.dynamic_cast(floor_instance, "isnappable").add_snap_target("+x", k3d.point3(5, 0, 0))
	k3d.dynamic_cast(floor_instance, "isnappable").add_snap_target("-y", k3d.point3(0, -5, 0))
	k3d.dynamic_cast(floor_instance, "isnappable").add_snap_target("+y", k3d.point3(0, 5, 0))
	k3d.dynamic_cast(floor_instance, "isnappable").add_snap_target("-z", k3d.point3(0, 0, -1))
	k3d.dynamic_cast(floor_instance, "isnappable").add_snap_target("+z", k3d.point3(0, 0, 1))

	chair = doc.new_node("PolyCube")
	chair.name = "Chair"

	chair_instance = doc.new_node("MeshInstance")
	chair_instance.name = "Chair Instance"
	chair_instance.gl_painter = doc.get_node("GL Default Painter")
	chair_instance.ri_painter = doc.get_node("RenderMan Default Painter")
	doc.set_dependency(chair_instance.get_property("input_mesh"), chair.get_property("output_mesh"))

	k3d.dynamic_cast(chair_instance, "isnappable").add_snap_source("bottom", k3d.point3(0, 0, -2.5))

	doc.finish_change_set("Create Snap Cube")

except:
	doc.cancel_change_set()
	raise

sys.stdout.flush()
