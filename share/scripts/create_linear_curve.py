#python

doc = Document
doc.start_change_set()
try:
	material = doc.new_node("RenderManMaterial")
	material.name = "Curve Material"
	material.color = (1, 1, 1)

	frozen_mesh = doc.new_node("FrozenMesh")
	frozen_mesh.name = "Linear Curve"

	mesh = frozen_mesh.new_mesh()

	positions = ((-5, 0, 5), (5, 0, 5), (5, 0, -5), (-5, 0, -5))
	for position in positions:
		mesh.new_point(position)

	group = mesh.new_linear_curve_group()
	group.material = material

	curve = group.new_curve()
	for i in range(4):
		curve.control_points[i] = mesh.points[i]

	curve.varying_data[0].set_color("Cs", (1, 1, 0))
	curve.varying_data[0].set_real("width", 1)
	curve.varying_data[1].set_color("Cs", (0, 1, 0))
	curve.varying_data[1].set_real("width", 0.5)
	curve.varying_data[2].set_color("Cs", (0, 0, 1))
	curve.varying_data[2].set_real("width", 1)
	curve.varying_data[3].set_color("Cs", (1, 0, 0))
	curve.varying_data[3].set_real("width", 0.5)

	mesh_instance = doc.new_node("MeshInstance")
	mesh_instance.name = "Linear Curve Instance"
	doc.set_dependency(mesh_instance.get_property("input_mesh"), frozen_mesh.get_property("output_mesh"))

	doc.finish_change_set("Create Linear Curve")

except:
	doc.cancel_change_set()
	raise

