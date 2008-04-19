#python

import k3d
k3d.check_node_environment(locals(), "MeshSourceScript")

# Perform required one-time setup to store geometric points in the mesh ...
points = Output.create_points()
point_selection = Output.create_point_selection()

# Perform required one-time setup to store linear curves in the mesh ...
groups = Output.create_linear_curve_groups()
first_curves = groups.create_first_curves()
curve_counts = groups.create_curve_counts()
periodic_curves = groups.create_periodic_curves()
materials = groups.create_materials()
curve_first_points = groups.create_curve_first_points()
curve_point_counts = groups.create_curve_point_counts()
curve_selection = groups.create_curve_selection()
curve_points = groups.create_curve_points()

# Create an (optional) array to store per-group curve widths
constantwidth = groups.writable_constant_data().create_array("constantwidth", "k3d::double_t")

# Create an (optional) array to store per-curve curve colors
Cs = groups.writable_uniform_data().create_array("Cs", "k3d::color")

# Create two curve groups ...
for i in range(2):
	first_curves.append(len(curve_first_points))
	curve_counts.append(5)
	periodic_curves.append(False)
	materials.append(None)

	constantwidth.append(i + 0.5)

	# Create five curves in each group ...
	for j in range(5):
		curve_first_points.append(len(curve_points))
		curve_point_counts.append(3)
		curve_selection.append(0.0)

		curve_points.append(len(points) + 0)
		curve_points.append(len(points) + 1)
		curve_points.append(len(points) + 2)

		positions = [(0, 0, 5), (5, 0, 0), (0, 0, -5)]

		for position in positions:
			points.append(k3d.point3(position[0] + (j * 5), position[1] + (i * -5), position[2]))
			point_selection.append(0.0)

		Cs.append(k3d.color(1, 1, j * 0.2))

