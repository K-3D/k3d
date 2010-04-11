#python

import k3d

context.document.start_change_set()
try:
	floor = context.document.new_node("PolyCube")
	floor.name = "Floor"
	floor.width = 10
	floor.height = 10
	floor.depth = 2

	floor_instance = context.document.new_node("MeshInstance")
	floor_instance.name = "Floor Instance"
	floor_instance.gl_painter = k3d.node.lookup_one(context.document, "GL Default Painter")
	floor_instance.ri_painter = k3d.node.lookup_one(context.document, "RenderMan Default Painter")
	k3d.property.connect(context.document, floor.get_property("output_mesh"), floor_instance.get_property("input_mesh"))

	k3d.dynamic_cast(floor_instance, "isnappable").add_snap_target("-x", k3d.point3(-5, 0, 0))
	k3d.dynamic_cast(floor_instance, "isnappable").add_snap_target("+x", k3d.point3(5, 0, 0))
	k3d.dynamic_cast(floor_instance, "isnappable").add_snap_target("-y", k3d.point3(0, -5, 0))
	k3d.dynamic_cast(floor_instance, "isnappable").add_snap_target("+y", k3d.point3(0, 5, 0))
	k3d.dynamic_cast(floor_instance, "isnappable").add_snap_target("-z", k3d.point3(0, 0, -1))
	k3d.dynamic_cast(floor_instance, "isnappable").add_snap_target("+z", k3d.point3(0, 0, 1))

	chair = context.document.new_node("PolyCube")
	chair.name = "Chair"

	chair_instance = context.document.new_node("MeshInstance")
	chair_instance.name = "Chair Instance"
	chair_instance.gl_painter = k3d.node.lookup_one(context.document, "GL Default Painter")
	chair_instance.ri_painter = k3d.node.lookup_one(context.document, "RenderMan Default Painter")
	k3d.property.connect(context.document, chair.get_property("output_mesh"), chair_instance.get_property("input_mesh"))

	k3d.dynamic_cast(chair_instance, "isnappable").add_snap_source("bottom", k3d.point3(0, 0, -2.5))

	context.document.finish_change_set("Create Snap Cube")

except:
	context.document.cancel_change_set()
	raise

