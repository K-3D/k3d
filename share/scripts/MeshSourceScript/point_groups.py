#python

import k3d
k3d.check_node_environment(locals(), "MeshSourceScript")

# Perform required one-time setup to store geometric points in the mesh ...
points = Output.create_points()
point_selection = Output.create_point_selection()

# Perform required one-time setup to store point groups in the mesh ...
point_groups = Output.create_point_groups()
first_points = point_groups.create_first_points()
point_counts = point_groups.create_point_counts()
materials = point_groups.create_materials()
group_points = point_groups.create_points()

# Create an (optional) array to store per-group point widths
constantwidth = point_groups.writable_constant_data().create_array("constantwidth", "k3d::double_t")

# Create an (optional) array to store per-point point colors
Cs = point_groups.writable_varying_data().create_array("Cs", "k3d::color")

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

