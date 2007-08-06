#python

import k3d
k3d.check_node_environment(locals(), "MeshSourceScript")

from random import uniform

count = 100
size = 10

points = Output.create_points()
point_selection = Output.create_point_selection()

for i in range(count):
	points.append(k3d.point3(uniform(-size, size), uniform(-size, size), uniform(-size, size)))
	point_selection.append(0.0)

point_groups = Output.create_point_groups()

first_points = point_groups.create_first_points()
first_points.append(0)

point_counts = point_groups.create_point_counts()
point_counts.append(len(points))

materials = point_groups.create_materials()
materials.append(None)

constantwidth = point_groups.writable_constant_data().create_array("constantwidth", "double")
constantwidth.append(0.2)

group_points = point_groups.create_points()
for i in range(len(points)):
	group_points.append(i)

Cs = point_groups.writable_varying_data().create_array("Cs", "k3d::color")
for i in range(len(points)):
	Cs.append(k3d.color(uniform(0, 1), uniform(0, 1), uniform(0, 1)))

