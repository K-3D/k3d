#python

import k3d
k3d.check_node_environment(locals(), "MeshModifierScript")

from random import seed, randint

Output.copy(Input)

seed(123)

colors = [ k3d.color(1, 0, 0), k3d.color(1, 1, 0), k3d.color(0, 1, 0), k3d.color(0, 1, 1), k3d.color(0, 0, 1), k3d.color(1, 0, 1), k3d.color(1, 1, 1)]

if Output.polyhedra() and Output.polyhedra().edge_points():
	Cs = Output.writable_polyhedra().writable_face_varying_data().create_array("Cs", "k3d::color")
	for i in range(len(Output.polyhedra().edge_points())):
		Cs.append(colors[i % len(colors)])

