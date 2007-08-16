#python

import k3d

doc = Document
doc.start_change_set()
try:
	material = doc.new_node("RenderManMaterial")
	material.name = "Curve Material"
	material.color = k3d.color(1, 1, 1)

	frozen_mesh = doc.new_node("FrozenMesh")
	frozen_mesh.name = "Linear Curve"

	mesh = k3d.dynamic_cast(frozen_mesh, "imesh_storage").reset_mesh()

	points = mesh.create_points()
	point_selection = mesh.create_point_selection()

	positions = [k3d.point3(-5, 0, -5), k3d.point3(5, 0, -5), k3d.point3(-5, 0, 5), k3d.point3(5, 0, 5)]
	colors = [k3d.color(1, 0, 0), k3d.color(0, 1, 0), k3d.color(0, 0, 1), k3d.color(1, 1, 1)]
	widths = [0.1, 0.8, 0.8, 0.1]

	for position in positions:
		points.append(position)
		point_selection.append(0.0)

	Cs = mesh.writable_vertex_data().create_array("Cs", "k3d::color")
	for color in colors:
		Cs.append(color)

	width = mesh.writable_vertex_data().create_array("width", "double")
	for w in widths:
		width.append(w)

	groups = mesh.create_linear_curve_groups()

	first_curves = groups.create_first_curves()
	first_curves.append(0)

	curve_counts = groups.create_curve_counts()
	curve_counts.append(1)

	periodic_curves = groups.create_periodic_curves()
	periodic_curves.append(False)

	materials = groups.create_materials()
	materials.append(None)

#	constantwidth = groups.writable_constant_data().create_array("constantwidth", "double")
#	constantwidth.append(0.2)

	curve_first_points = groups.create_curve_first_points()
	curve_first_points.append(0)

	curve_point_counts = groups.create_curve_point_counts()
	curve_point_counts.append(len(positions))

	curve_selection = groups.create_curve_selection()
	curve_selection.append(0.0)

	curve_points = groups.create_curve_points()
	for i in range(len(positions)):
		curve_points.append(i)

	mesh_instance = doc.new_node("MeshInstance")
	mesh_instance.name = "Linear Curve Instance"
	mesh_instance.gl_painter = doc.get_node("GL Default Painter")
	mesh_instance.ri_painter = doc.get_node("RenderMan Default Painter")
	doc.set_dependency(mesh_instance.get_property("input_mesh"), frozen_mesh.get_property("output_mesh"))

	doc.finish_change_set("Create Linear Curve")

except:
	doc.cancel_change_set()
	raise

