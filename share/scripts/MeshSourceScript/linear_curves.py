#python

import k3d
k3d.check_node_environment(locals(), "MeshSourceScript")

# Perform required one-time setup to store geometric points in the mesh ...
points = Output.create_points()
point_selection = Output.create_point_selection()

# Perform required one-time setup to store linear curves in the mesh ...
curves = k3d.linear_curve.create(Output)

# Create an (optional) array to store curve widths
constantwidth = curves.constant_data().create("constantwidth", "k3d::double_t")

# Create an (optional) array to store per-curve curve colors
Cs = curves.uniform_data().create("Cs", "k3d::color")

# Add curves ...
curves.periodic().append(False)
curves.material().append(None)
constantwidth.append(0.5)

for j in range(5):
	curves.curve_first_points().append(len(curves.curve_points()))
	curves.curve_point_counts().append(3)
	curves.curve_selections().append(0.0)

	curves.curve_points().append(len(points) + 0)
	curves.curve_points().append(len(points) + 1)
	curves.curve_points().append(len(points) + 2)

	positions = [(0, 0, 5), (5, 0, 0), (0, 0, -5)]

	for position in positions:
		points.append(k3d.point3(position[0] + (j * 5), position[1], position[2]))
		point_selection.append(0.0)

	Cs.append(k3d.color(1, 1, j * 0.2))

