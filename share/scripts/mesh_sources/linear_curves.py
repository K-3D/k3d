#python

import k3d
from random import uniform, seed

seed(1234)

count = 25
size = 10

points = Output.create_points()
for i in range(count):
	base = k3d.point3(size * uniform(-1, 1), size * uniform(-1, 1), 0)
	middle = k3d.point3(base)
	middle += k3d.vector3(uniform(-1, 1), uniform(-1, 1), 2)
	tip = k3d.point3(base)
	tip += k3d.vector3(0, 0, 5)

	points.append(base)
	points.append(middle)
	points.append(tip)

groups = Output.create_linear_curve_groups()

first_curves = groups.create_first_curves()
first_curves.append(0)

curve_counts = groups.create_curve_counts()
curve_counts.append(count)

periodic_curves = groups.create_periodic_curves()
periodic_curves.append(False)

materials = groups.create_materials()
materials.append(None)

constantwidth = groups.writable_constant_data.create_array("constantwidth", "double")
constantwidth.append(0.2)

curve_first_points = groups.create_curve_first_points()
for i in range(count):
	curve_first_points.append(i * 3)

curve_point_counts = groups.create_curve_point_counts()
for i in range(count):
	curve_point_counts.append(3)

curve_points = groups.create_curve_points()
for i in range(count):
	curve_points.append(i * 3)
	curve_points.append(i * 3 + 1)
	curve_points.append(i * 3 + 2)

Cs = groups.writable_uniform_data.create_array("Cs", "k3d::color")
for i in range(count):
	Cs.append(k3d.color(uniform(0, 1), uniform(0, 1), uniform(0, 1)))
