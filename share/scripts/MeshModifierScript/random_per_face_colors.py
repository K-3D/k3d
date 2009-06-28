#python

import k3d
k3d.check_node_environment(locals(), "MeshModifierScript")

from random import seed, randint

Output.copy(Input)

seed(123)

colors = [ k3d.color(1, 0, 0), k3d.color(1, 1, 0), k3d.color(0, 1, 0), k3d.color(0, 1, 1), k3d.color(0, 0, 1), k3d.color(1, 0, 1), k3d.color(1, 1, 1)]

for prim_idx, const_primitive in enumerate(Input.primitives()):
	if const_primitive.attributes()["uniform"]:
		Cs = Output.prmitives()[prim_idx].attributes()["uniform"].create_array("Cs", "k3d::color")
		#todo: find out how to get the length for uniform data
		#for i in range(len(Output.bilinear_patches().patch_points()) / 4):
		#	Cs.append(colors[i % len(colors)])
