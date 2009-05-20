#python

import k3d
import testing

document = k3d.new_document()

source = document.new_node("FrozenMesh")
mesh = source.create_mesh()

positions = [(-5, 5, 0), (5, 5, 0), (-5, -5, 0), (5, -5, 0)]

points = mesh.create_points()
point_selection = mesh.create_point_selection()

for position in positions:
	points.append(k3d.point3(position[0], position[1], position[2]))
	point_selection.append(0.0)

polyhedron = k3d.polyhedron.create(mesh)
polyhedron.shell_first_faces().append(0)
polyhedron.shell_face_counts().append(1)
polyhedron.shell_types().append(k3d.polyhedron.shell_type.POLYGONS)
polyhedron.face_first_loops().append(0)
polyhedron.face_loop_counts().append(1)
polyhedron.face_materials().append(None)
polyhedron.face_selections().append(0.0)
polyhedron.loop_first_edges().append(0)
polyhedron.edge_points().assign([0, 1, 2, 3])
polyhedron.clockwise_edges().assign([1, 2, 3, 0])
polyhedron.edge_selections().assign([0.0, 0.0, 0.0, 0.0])

Cs = polyhedron.face_varying_data().create_array("Cs", "k3d::color")
Cs.assign([k3d.color(1, 0, 0), k3d.color(0, 1, 0), k3d.color(0, 0, 1), k3d.color(1, 1, 1)])

modifier = document.new_node("TriangulateFaces")
mesh_selection = k3d.mesh_selection.select_all()
modifier.mesh_selection = mesh_selection
document.set_dependency(modifier.get_property("input_mesh"), source.get_property("output_mesh"))

testing.mesh_comparison_to_reference(document, modifier.get_property("output_mesh"), "mesh.modifier.TriangulateFaces.self_intersection", 0)

