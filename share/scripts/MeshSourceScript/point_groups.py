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

# Add some points ...
point_group.material().append(None)
constantwidth.append(0.5)

for x in range(-5, 6):
	for z in range (-5, 6):
		point_group.points().append(len(points))

		points.append(k3d.point3(x, 0, z))
		point_selection.append(0.0)
		Cs.append(k3d.color((x / 10.0) + 0.5, 1, (z / 10.0) + 0.5))

