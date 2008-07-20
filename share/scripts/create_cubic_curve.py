#python

import k3d

doc = Document
doc.start_change_set()
try:
	frozen_mesh = doc.new_node("FrozenMesh")
	frozen_mesh.name = "Cubic Curve"

	mesh = k3d.dynamic_cast(frozen_mesh, "imesh_storage").reset_mesh()

	points = mesh.create_points()
	point_selection = mesh.create_point_selection()
	Cs = mesh.writable_vertex_data().create_array("Cs", "k3d::color")

	groups = mesh.create_cubic_curve_groups()
	first_curves = groups.create_first_curves()
	curve_counts = groups.create_curve_counts()
	periodic_curves = groups.create_periodic_curves()
	materials = groups.create_materials()
	constantwidth = groups.writable_constant_data().create_array("constantwidth", "k3d::double_t")
	curve_first_points = groups.create_curve_first_points()
	curve_point_counts = groups.create_curve_point_counts()
	curve_selection = groups.create_curve_selection()
	curve_points = groups.create_curve_points()

	positions = [k3d.point3(-5, 0, -5), k3d.point3(5, 0, -5), k3d.point3(-5, 0, 5), k3d.point3(5, 0, 5)]
	colors = [k3d.color(1, 0, 0), k3d.color(0, 1, 0), k3d.color(0, 0, 1), k3d.color(1, 1, 1)]

	for position in positions:
		points.append(position)
		point_selection.append(0.0)

	for color in colors:
		Cs.append(color)

	first_curves.append(len(curve_first_points))
	curve_counts.append(1)
	periodic_curves.append(False)
	materials.append(None)
	constantwidth.append(0.2)
	curve_first_points.append(len(curve_points))
	curve_point_counts.append(len(positions))
	curve_selection.append(0.0)

	for i in range(len(positions)):
		curve_points.append(i)

	mesh_instance = doc.new_node("MeshInstance")
	mesh_instance.name = "Cubic Curve Instance"
	mesh_instance.gl_painter = doc.get_node("GL Default Painter")
	mesh_instance.ri_painter = doc.get_node("RenderMan Default Painter")
	doc.set_dependency(mesh_instance.get_property("input_mesh"), frozen_mesh.get_property("output_mesh"))

	doc.finish_change_set("Create Cubic Curve")

except:
	doc.cancel_change_set()
	raise

