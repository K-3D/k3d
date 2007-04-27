#python

doc = Document
doc.start_change_set()
try:
	material = doc.new_node("RenderManMaterial")
	material.name = "Square Material"
	material.color = (1, 1, 1)
	frozen_mesh = doc.new_node("FrozenMesh")

	frozen_mesh.name = "Square"

	mesh = frozen_mesh.new_mesh()

	positions = ((-5, 0, 5), (5, 0, 5), (5, 0, -5), (-5, 0, -5))
	points = []
	for position in positions:
		points.append(mesh.new_point(position))

	polyhedron = mesh.new_polyhedron()
	edges = []
	for point in points:
		edges.append(polyhedron.new_edge(point))

	for i in range(len(edges)):
		edges[i].face_clockwise = edges[(i+1)%len(edges)]

	face = polyhedron.new_face(edges[0])
	face.material = material

#	face.uniform_data.set_color("Cs", (1, 0, 0))

	points[0].vertex_data.set_color("Cs", (1, 0, 0))
	points[1].vertex_data.set_color("Cs", (0, 1, 0))
	points[2].vertex_data.set_color("Cs", (0, 0, 1))
	points[3].vertex_data.set_color("Cs", (1, 1, 1))

	mesh_instance = doc.new_node("MeshInstance")
	mesh_instance.name = "Square Instance"
	doc.set_dependency(mesh_instance.get_property("input_mesh"), frozen_mesh.get_property("output_mesh"))

	doc.finish_change_set("Create Square")

except:
	doc.cancel_change_set()
	raise

