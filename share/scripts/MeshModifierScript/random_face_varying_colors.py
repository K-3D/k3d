#python

import k3d
k3d.check_node_environment(locals(), "MeshModifierScript")

from random import seed, randint

Output.copy(Input)

seed(123)

colors = [ k3d.color(1, 0, 0), k3d.color(1, 1, 0), k3d.color(0, 1, 0), k3d.color(0, 1, 1), k3d.color(0, 0, 1), k3d.color(1, 0, 1), k3d.color(1, 1, 1)]

for prim_idx, const_primitive in enumerate(Input.primitives()):
	if const_primitive.type() == "polyhedron":
		polyhedron = k3d.polyhedron.validate(Output.primitives()[prim_idx])
		if polyhedron:
			Cs = polyhedron.face_varying_data().create("Cs", "k3d::color")
			for i in range(len(polyhedron.edge_points())):
				Cs.append(colors[i % len(colors)])

