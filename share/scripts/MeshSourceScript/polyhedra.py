#python

import k3d
k3d.check_node_environment(locals(), "MeshSourceScript")

# Perform required one-time setup to store geometric points in the mesh ...
points = Output.create_points()
point_selection = Output.create_point_selection()

# Perform required one-time setup to store polyhedra in the mesh ...
polyhedra = Output.create_polyhedra()
first_faces = polyhedra.create_first_faces()
face_counts = polyhedra.create_face_counts()
types = polyhedra.create_types()
face_first_loops = polyhedra.create_face_first_loops()
face_loop_counts = polyhedra.create_face_loop_counts()
face_materials = polyhedra.create_face_materials()
face_selection = polyhedra.create_face_selection()
loop_first_edges = polyhedra.create_loop_first_edges()
edge_points = polyhedra.create_edge_points()
clockwise_edges = polyhedra.create_clockwise_edges()
edge_selection = polyhedra.create_edge_selection()

# Create an (optional) array to store uniform (per-face) colors ...
Cs = polyhedra.writable_uniform_data().create_array("Cs", "k3d::color")

# Create two polyhedra ...
for i in range(2):
	first_faces.append(len(face_first_loops))
	face_counts.append(2)
	types.append(k3d.polyhedron_type.polygons)

	# Create three faces in each polyhedron ...
	for j in range(3):
		# Each face has a single loop (its exterior boundary) ...
		face_first_loops.append(len(loop_first_edges))
		face_loop_counts.append(1)
		face_materials.append(None)
		face_selection.append(0.0)

		Cs.append(k3d.color(1, j / 2.0, i / 1.0))

		loop_first_edges.append(len(edge_points))

		# Each loop has four edges, each of which points to the next edge in clockwise-order ...
		clockwise_edges.append(len(edge_points) + 1)
		clockwise_edges.append(len(edge_points) + 2)
		clockwise_edges.append(len(edge_points) + 3)
		clockwise_edges.append(len(edge_points) + 0)

		# Each edge refers to its starting-point ...
		edge_points.append(len(points) + 0)
		edge_points.append(len(points) + 1)
		edge_points.append(len(points) + 2)
		edge_points.append(len(points) + 3)

		edge_selection.append(0.0)
		edge_selection.append(0.0)
		edge_selection.append(0.0)
		edge_selection.append(0.0)

		positions = [(-5, 0, 5), (5, 0, 5), (5, 0, -5), (-5, 0, -5)]

		for position in positions:
			points.append(k3d.point3(position[0] + (j * 11.0), position[1] + (i * -11.0), position[2]))
			point_selection.append(0.0)

