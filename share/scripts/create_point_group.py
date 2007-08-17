#python

import k3d
from random import uniform

doc = Document
doc.start_change_set()
try:
	shader = doc.new_node("RenderManSurfaceShader")
	shader.name = "Group Shader"
	shader.shader_path = k3d.share_path() + "/shaders/surface/k3d_constant.sl"

	material = doc.new_node("RenderManMaterial")
	material.name = "Group Material"
	material.color = k3d.color(1, 1, 1)
	material.surface_shader = shader

	frozen_mesh = doc.new_node("FrozenMesh")
	frozen_mesh.name = "Point Group"

	mesh = k3d.dynamic_cast(frozen_mesh, "imesh_storage").reset_mesh()

	count = 100
	size = 10

	points = mesh.create_points()
	point_selection = mesh.create_point_selection()

	for i in range(count):
		points.append(k3d.point3(uniform(-size, size), uniform(-size, size), uniform(-size, size)))
		point_selection.append(0.0)

	point_groups = mesh.create_point_groups()

	first_points = point_groups.create_first_points()
	first_points.append(0)

	point_counts = point_groups.create_point_counts()
	point_counts.append(len(points))

	materials = point_groups.create_materials()
	materials.append(None)

	constantwidth = point_groups.writable_constant_data().create_array("constantwidth", "double")
	constantwidth.append(0.2)

	group_points = point_groups.create_points()
	for i in range(len(points)):
		group_points.append(i)

	Cs = point_groups.writable_varying_data().create_array("Cs", "k3d::color")
	for i in range(len(points)):
		Cs.append(k3d.color(uniform(0, 1), uniform(0, 1), uniform(0, 1)))

	
	mesh_instance = doc.new_node("MeshInstance")
	mesh_instance.name = "Point Group Instance"
	mesh_instance.gl_painter = doc.get_node("GL Default Painter")
	mesh_instance.ri_painter = doc.get_node("RenderMan Default Painter")
	doc.set_dependency(mesh_instance.get_property("input_mesh"), frozen_mesh.get_property("output_mesh"))

	doc.finish_change_set("Create Point Group")

except:
	doc.cancel_change_set()
	raise

