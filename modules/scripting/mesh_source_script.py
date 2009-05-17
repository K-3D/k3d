#python

import k3d

positions = [(-5, -5, 0), (5, -5, 0), (5, 5, 0), (-5, 5, 0)]

points = Output.create_points()
point_selection = Output.create_point_selection()
for position in positions:
	points.append(k3d.point3(position[0], position[1], position[2]))
	point_selection.append(0.0)

polyhedra = k3d.polyhedron.create(Output)
polyhedra.shell_first_faces().append(0)
polyhedra.shell_face_counts().append(1)
polyhedra.shell_types().append(k3d.polyhedron.shell_type.POLYGONS)
polyhedra.face_first_loops().append(0)
polyhedra.face_loop_counts().append(1)
polyhedra.face_materials().append(None)
polyhedra.face_selections().append(0.0)
polyhedra.loop_first_edges().append(0)
polyhedra.edge_points().assign([0, 1, 2, 3])
polyhedra.clockwise_edges().assign([1, 2, 3, 0])
polyhedra.edge_selections().assign([0.0, 0.0, 0.0, 0.0])

print Output

