#python

import k3d

doc = Document
doc.start_change_set()
try:
	frozen_mesh = doc.new_node("FrozenMesh")
	frozen_mesh.name = "Simple Polyhedron"

	mesh = k3d.dynamic_cast(frozen_mesh, "imesh_storage").reset_mesh()

	vertices = [k3d.point3(-3, -3, 0), k3d.point3(3, -3, 0), k3d.point3(3, 3, 0), k3d.point3(-3, 3, 0)]
	vertex_counts = [3, 3]
	vertex_indices = [0, 1, 3, 1, 2, 3]
	polyhedron = k3d.polyhedron.create(mesh, vertices, vertex_counts, vertex_indices)

	mesh_instance = doc.new_node("MeshInstance")
	mesh_instance.name = "Simple Polyhedron Instance"
	mesh_instance.gl_painter = doc.get_node("GL Default Painter")
	mesh_instance.ri_painter = doc.get_node("RenderMan Default Painter")
	doc.set_dependency(mesh_instance.get_property("input_mesh"), frozen_mesh.get_property("output_mesh"))

	doc.finish_change_set("Create Simple Polyhedron")

except:
	doc.cancel_change_set()
	raise

