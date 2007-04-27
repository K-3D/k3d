#python

import k3d
from random import seed, uniform

Output.copy(Input)

seed(123)

Cs = Output.writable_vertex_data.create_array("Cs", "k3d::color")
for point in range(len(Output.points)):
	Cs.append(k3d.color(uniform(0, 1), uniform(0, 1), uniform(0, 1)))

