#python

import k3d
k3d.check_node_environment(locals(), "MeshSourceScript")

# Perform required one-time setup to store geometric points in the mesh ...
points = Output.create_points()
point_selection = Output.create_point_selection()

# Perform required one-time setup to store bicubic patches in the mesh ...
patches = k3d.bicubic_patch.create(Output)

# Create an (optional) array to hold color values at the parametric
# corners of each patch ...
Cs = patches.varying_data().create("Cs", "k3d::color")

# We will create two identical bicubic patches ...
for i in range(2):
	patches.patch_selections().append(0)
	patches.patch_materials().append(None)

	for j in range(16):
		patches.patch_points().append(len(points) + j)

	positions = [
		(-5, -5, 0), (-2, -5, 2), (2, -5, -2), (5, -5, 0),
		(-5, -2, 2), (-2, -2, 5), (2, -2, -5), (5, -2, -2),
		(-5, 2, 2), (-2, 2, 5), (2, 2, -5), (5, 2, -2),
		(-5, 5, 0), (-2, 5, 2), (2, 5, -2), (5, 5, 0)]

	for position in positions:
		points.append(k3d.point3(position[0] + (12 * i), position[2], -position[1]))
		point_selection.append(0.0)

	Cs.append(k3d.color(1, 0, 0))
	Cs.append(k3d.color(0, 1, 0))
	Cs.append(k3d.color(0, 0, 1))
	Cs.append(k3d.color(1, 1, 1))

