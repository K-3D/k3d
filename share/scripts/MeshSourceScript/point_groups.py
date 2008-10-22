#python

import k3d
k3d.check_node_environment(locals(), "MeshSourceScript")

# Perform required one-time setup to store geometric points in the mesh ...
points = Output.create_points()
point_selection = Output.create_point_selection()

# Construct a point group mesh primitive ...
point_groups = Output.primitives().create("point_group")
first_points = point_groups.topology().create("first_points", "k3d::uint_t")
point_counts = point_groups.topology().create("point_counts", "k3d::uint_t")
materials = point_groups.topology().create("materials", "k3d::imaterial*")
group_points = point_groups.topology().create("points", "k3d::uint_t")
constant = point_groups.attributes().create("constant")
varying = point_groups.attributes().create("varying")

# Create an (optional) array to store per-group point widths
constantwidth = constant.create("constantwidth", "k3d::double_t")

# Create an (optional) array to store per-point point colors
Cs = varying.create("Cs", "k3d::color")

# Create two point groups ...
for i in range(2):
	first_points.append(len(group_points))
	point_counts.append(30)
	materials.append(None)

	constantwidth.append(i + 0.5)

	# Add 30 points to each group ...
	for j in range(30):
		group_points.append(len(points))

		points.append(k3d.point3(j - 15, 0 - (5 * i), 0))
		point_selection.append(0.0)

		Cs.append(k3d.color(1, j / 30.0, 0))

