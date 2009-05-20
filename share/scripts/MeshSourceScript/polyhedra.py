#python

import k3d
k3d.check_node_environment(locals(), "MeshSourceScript")

# Perform required one-time setup to store geometric points in the mesh ...
points = Output.create_points()
point_selection = Output.create_point_selection()

# Create two polyhedra ...
for i in range(2):
	polyhedron = k3d.polyhedron.create(Output)

	# Create an (optional) array to store uniform (per-face) colors ...
	Cs = polyhedron.uniform_data().create("Cs", "k3d::color")

	polyhedron.shell_first_faces().append(len(polyhedron.face_first_loops()))
	polyhedron.shell_face_counts().append(3)
	polyhedron.shell_types().append(k3d.polyhedron.shell_type.POLYGONS)

	# Create three faces in each polyhedron ...
	for j in range(3):
		# Each face has a single loop (its exterior boundary) ...
		polyhedron.face_first_loops().append(len(polyhedron.loop_first_edges()))
		polyhedron.face_loop_counts().append(1)
		polyhedron.face_materials().append(None)
		polyhedron.face_selections().append(0.0)

		Cs.append(k3d.color(1, j / 2.0, i / 1.0))

		polyhedron.loop_first_edges().append(len(polyhedron.edge_points()))

		# Each loop has four edges, each of which points to the next edge in clockwise-order ...
		polyhedron.clockwise_edges().append(len(polyhedron.edge_points()) + 1)
		polyhedron.clockwise_edges().append(len(polyhedron.edge_points()) + 2)
		polyhedron.clockwise_edges().append(len(polyhedron.edge_points()) + 3)
		polyhedron.clockwise_edges().append(len(polyhedron.edge_points()) + 0)

		# Each edge refers to its starting-point ...
		polyhedron.edge_points().append(len(points) + 0)
		polyhedron.edge_points().append(len(points) + 1)
		polyhedron.edge_points().append(len(points) + 2)
		polyhedron.edge_points().append(len(points) + 3)

		polyhedron.edge_selections().append(0.0)
		polyhedron.edge_selections().append(0.0)
		polyhedron.edge_selections().append(0.0)
		polyhedron.edge_selections().append(0.0)

		positions = [(-5, 0, 5), (5, 0, 5), (5, 0, -5), (-5, 0, -5)]

		for position in positions:
			points.append(k3d.point3(position[0] + (j * 11.0), position[1] + (i * -11.0), position[2]))
			point_selection.append(0.0)

