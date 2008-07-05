#python

import k3d
k3d.check_node_environment(locals(), "MeshModifierScript")

from random import seed, randint

Output.copy(Input)

seed(123)

colors = [ k3d.color(1, 0, 0), k3d.color(1, 1, 0), k3d.color(0, 1, 0), k3d.color(0, 1, 1), k3d.color(0, 0, 1), k3d.color(1, 0, 1), k3d.color(1, 1, 1)]

if Output.bilinear_patches() and Output.bilinear_patches().patch_points():
	Cs = Output.writable_bilinear_patches().writable_uniform_data().create_array("Cs", "k3d::color")
	for i in range(len(Output.bilinear_patches().patch_points()) / 4):
		Cs.append(colors[i % len(colors)])

if Output.bicubic_patches() and Output.bicubic_patches().patch_points():
	Cs = Output.writable_bicubic_patches().writable_uniform_data().create_array("Cs", "k3d::color")
	for i in range(len(Output.bicubic_patches().patch_points()) / 16):
		Cs.append(colors[i % len(colors)])

if Output.nurbs_patches() and Output.nurbs_patches().patch_first_points():
	Cs = Output.writable_nurbs_patches().writable_uniform_data().create_array("Cs", "k3d::color")
	for i in range(len(Output.nurbs_patches().patch_first_points())):
		Cs.append(colors[i % len(colors)])

if Output.polyhedra() and Output.polyhedra().face_first_loops():
	Cs = Output.writable_polyhedra().writable_uniform_data().create_array("Cs", "k3d::color")
	for i in range(len(Output.polyhedra().face_first_loops())):
		Cs.append(colors[i % len(colors)])

