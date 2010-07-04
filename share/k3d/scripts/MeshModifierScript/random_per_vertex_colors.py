#python

import k3d
k3d.check_node_environment(locals(), "MeshModifierScript")

from random import seed, uniform

Output.copy(Input)

seed(123)

Cs = Output.vertex_data().create_array("Cs", "k3d::color")
for point in range(len(Output.points())):
	Cs.append(k3d.color(uniform(0, 1), uniform(0, 1), uniform(0, 1)))

