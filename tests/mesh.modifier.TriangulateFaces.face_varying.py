#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("FrozenMesh", "TriangulateFaces")

mesh = setup.source.create_mesh()

points = mesh.create_points()
point_selection = mesh.create_point_selection()

polyhedron = k3d.polyhedron.create(mesh)
Cs = polyhedron.edge_attributes().create("Cs", "k3d::color")

positions = [(0, 0, 1), (1, 0, 1), (2, 0, 0), (2, 0, 1), (1, 0, 0), (0, 0, 0)]
for position in positions:
	points.append(k3d.point3(position[0], position[1], position[2]))
	point_selection.append(0)

polyhedron.shell_types().append(k3d.polyhedron.shell_type.POLYGONS)

polyhedron.face_shells().append(0)
polyhedron.face_first_loops().append(len(polyhedron.loop_first_edges()))
polyhedron.face_loop_counts().append(1)
polyhedron.face_materials().append(None)
polyhedron.face_selections().append(0)

polyhedron.loop_first_edges().append(len(polyhedron.clockwise_edges()))

polyhedron.clockwise_edges().append(1)
polyhedron.edge_selections().append(0)
polyhedron.vertex_points().append(0)
polyhedron.vertex_selections().append(0)
Cs.append(k3d.color(1, 0, 0))

polyhedron.clockwise_edges().append(2)
polyhedron.edge_selections().append(0)
polyhedron.vertex_points().append(1)
polyhedron.vertex_selections().append(0)
Cs.append(k3d.color(0, 1, 0))

polyhedron.clockwise_edges().append(3)
polyhedron.edge_selections().append(0)
polyhedron.vertex_points().append(4)
polyhedron.vertex_selections().append(0)
Cs.append(k3d.color(0, 0, 1))

polyhedron.clockwise_edges().append(0)
polyhedron.edge_selections().append(0)
polyhedron.vertex_points().append(5)
polyhedron.vertex_selections().append(0)
Cs.append(k3d.color(1, 1, 1))

polyhedron.face_shells().append(0)
polyhedron.face_first_loops().append(len(polyhedron.loop_first_edges()))
polyhedron.face_loop_counts().append(1)
polyhedron.face_materials().append(None)
polyhedron.face_selections().append(0)

polyhedron.loop_first_edges().append(len(polyhedron.clockwise_edges()))

polyhedron.clockwise_edges().append(5)
polyhedron.edge_selections().append(0)
polyhedron.vertex_points().append(1)
polyhedron.vertex_selections().append(0)
Cs.append(k3d.color(1, 0, 0))

polyhedron.clockwise_edges().append(6)
polyhedron.edge_selections().append(0)
polyhedron.vertex_points().append(2)
polyhedron.vertex_selections().append(0)
Cs.append(k3d.color(0, 1, 0))

polyhedron.clockwise_edges().append(7)
polyhedron.edge_selections().append(0)
polyhedron.vertex_points().append(3)
polyhedron.vertex_selections().append(0)
Cs.append(k3d.color(0, 0, 1))

polyhedron.clockwise_edges().append(4)
polyhedron.edge_selections().append(0)
polyhedron.vertex_points().append(4)
polyhedron.vertex_selections().append(0)
Cs.append(k3d.color(1, 1, 1))

mesh_selection = k3d.geometry.selection.create(1)
setup.modifier.mesh_selection = mesh_selection

testing.require_valid_primitives(setup.document, setup.modifier.get_property("output_mesh"))
testing.mesh_reference_comparison(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.TriangulateFaces.face_varying", 0)

