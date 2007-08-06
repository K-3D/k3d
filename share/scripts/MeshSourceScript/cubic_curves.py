#python

import k3d
k3d.check_node_environment(locals(), "MeshSourceScript")

from random import uniform, seed

seed(1234)

count = 10
size = 10

points = Output.create_points()
point_selection = Output.create_point_selection()

for i in range(count):
	points.append(k3d.point3(i * 3, 0, 0))
	point_selection.append(0.0)

	for j in range(6):
		p = k3d.point3(points[len(points)-1])
		p += k3d.vector3(uniform(-1, 1), uniform(-1, 1), 1)
		points.append(p)
		point_selection.append(0.0)

groups = Output.create_cubic_curve_groups()

first_curves = groups.create_first_curves()
first_curves.append(0)

curve_counts = groups.create_curve_counts()
curve_counts.append(count)

periodic_curves = groups.create_periodic_curves()
periodic_curves.append(False)

materials = groups.create_materials()
materials.append(None)

constantwidth = groups.writable_constant_data().create_array("constantwidth", "double")
constantwidth.append(0.2)

curve_first_points = groups.create_curve_first_points()
for i in range(count):
	curve_first_points.append(i * 7)

curve_point_counts = groups.create_curve_point_counts()
for i in range(count):
	curve_point_counts.append(7)

curve_selection = groups.create_curve_selection()
for i in range(count):
	curve_selection.append(0.0)

curve_points = groups.create_curve_points()
for i in range(count):
	for j in range(7):
		curve_points.append(i * 7 + j)

Cs = groups.writable_uniform_data().create_array("Cs", "k3d::color")
for i in range(count):
	Cs.append(k3d.color(uniform(0, 1), uniform(0, 1), uniform(0, 1)))

