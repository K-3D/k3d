#python

import k3d

context.document.start_change_set()
try:
	# Create a FrozenMesh node to act as a mesh source ...
	frozen_mesh = k3d.plugin.create("FrozenMesh", context.document)
	frozen_mesh.name = "Square"

	# Create a mesh ...
	mesh = frozen_mesh.create_mesh()

	# Add geometric points to the mesh ...
	points = mesh.create_points()
	point_selection = mesh.create_point_selection()

	positions = [(-5, 0, 5), (5, 0, 5), (5, 0, -5), (-5, 0, -5)]
	for position in positions:
		points.append(k3d.point3(position[0], position[1], position[2]))
		point_selection.append(0.0)

	# Create a polyhedron primitive ...
	polyhedron = k3d.polyhedron.create(mesh)

	# Create a custom attribute array to store color values at each polyhedron vertex ...
	Cs = polyhedron.vertex_attributes().create("Cs", "k3d::color")

	# Add a single quadrilateral to the polyhedron ...
	polyhedron.shell_types().append(k3d.polyhedron.shell_type.POLYGONS)
	polyhedron.face_shells().append(0)
	polyhedron.face_first_loops().append(len(polyhedron.loop_first_edges()))
	polyhedron.face_loop_counts().append(1)
	polyhedron.face_materials().append(None)
	polyhedron.face_selections().append(0.0)
	polyhedron.loop_first_edges().append(len(polyhedron.clockwise_edges()))

	polyhedron.clockwise_edges().append(1)
	polyhedron.clockwise_edges().append(2)
	polyhedron.clockwise_edges().append(3)
	polyhedron.clockwise_edges().append(0)

	polyhedron.edge_selections().append(0.0)
	polyhedron.edge_selections().append(0.0)
	polyhedron.edge_selections().append(0.0)
	polyhedron.edge_selections().append(0.0)
	
	polyhedron.vertex_points().append(0)
	polyhedron.vertex_points().append(1)
	polyhedron.vertex_points().append(2)
	polyhedron.vertex_points().append(3)

	polyhedron.vertex_selections().append(0.0)
	polyhedron.vertex_selections().append(0.0)
	polyhedron.vertex_selections().append(0.0)
	polyhedron.vertex_selections().append(0.0)
	
	# Assign colors to the face vertices ...
	Cs.append(k3d.color(1, 0, 0))
	Cs.append(k3d.color(0, 1, 0))
	Cs.append(k3d.color(0, 0, 1))
	Cs.append(k3d.color(1, 1, 1))

	# Connect the FrozenMesh to a MeshInstance to place it in the scene ...
	mesh_instance = k3d.plugin.create("MeshInstance", context.document)
	mesh_instance.name = "Square Instance"
	mesh_instance.gl_painter = context.document.get_node("GL Default Painter")
	mesh_instance.ri_painter = context.document.get_node("RenderMan Default Painter")
	context.document.set_dependency(mesh_instance.get_property("input_mesh"), frozen_mesh.get_property("output_mesh"))

	# Make the MeshInstance visible to render engines ...
	k3d.node.show(context.document, mesh_instance)

	context.document.finish_change_set("Create Square")

except:
	context.document.cancel_change_set()
	raise

