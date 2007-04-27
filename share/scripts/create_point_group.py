#python

import k3d

doc = Document
doc.start_change_set()
try:
	shader = doc.new_node("RenderManSurfaceShader")
	shader.name = "Group Shader"
	shader.shader_path = k3d.share_path + "/shaders/surface/k3d_constant.sl"

	material = doc.new_node("RenderManMaterial")
	material.name = "Group Material"
	material.color = (1, 1, 1)
	material.surface_shader = shader

	frozen_mesh = doc.new_node("FrozenMesh")
	frozen_mesh.name = "Point Group"

	mesh = frozen_mesh.new_mesh()

	positions = ((-5, 0, 5), (5, 0, 5), (5, 0, -5), (-5, 0, -5))
	for position in positions:
		mesh.new_point(position)

	group = mesh.new_point_group()
	group.material = material

	for i in range(4):
		group.points[i] = mesh.points[i]

	group.constant_data.set_real("constantwidth", 0.5);
	
	mesh_instance = doc.new_node("MeshInstance")
	mesh_instance.name = "Point Group Instance"
	doc.set_dependency(mesh_instance.get_property("input_mesh"), frozen_mesh.get_property("output_mesh"))

	doc.finish_change_set("Create Point Group")

except:
	doc.cancel_change_set()
	raise

