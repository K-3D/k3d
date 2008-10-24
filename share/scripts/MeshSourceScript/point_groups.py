#python

import k3d
k3d.check_node_environment(locals(), "MeshSourceScript")

# Perform required one-time setup to store geometric points in the mesh ...
points = Output.create_points()
point_selection = Output.create_point_selection()

# Construct a point group mesh primitive ...
point_group = k3d.point_group.create(Output)

# Create an (optional) array to store per-group point widths
constantwidth = point_group.constant_data().create("constantwidth", "k3d::double_t")

# Create an (optional) array to store per-point point colors
Cs = point_group.varying_data().create("Cs", "k3d::color")

# Create two point groups ...
for i in range(2):
	point_group.first_points().append(len(point_group.points()))
	point_group.point_counts().append(30)
	point_group.materials().append(None)

	constantwidth.append(i + 0.5)

	# Add 30 points to each group ...
	for j in range(30):
		point_group.points().append(len(points))

		points.append(k3d.point3(j - 15, 0 - (5 * i), 0))
		point_selection.append(0.0)

		Cs.append(k3d.color(1, j / 30.0, 0))

