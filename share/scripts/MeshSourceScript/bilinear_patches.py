#python

import k3d
k3d.check_node_environment(locals(), "MeshSourceScript")

# Perform required one-time setup to store geometric points in the mesh ...
points = Output.create_points()
point_selection = Output.create_point_selection()

# Perform required one-time setup to store bilinear patches in the mesh ...
bilinear_patches = Output.create_bilinear_patches()
patch_selection = bilinear_patches.create_patch_selection()
patch_materials = bilinear_patches.create_patch_materials()
patch_points = bilinear_patches.create_patch_points()

# Create an (optional) array to hold color values at the parametric
# corners of each patch ...
Cs = bilinear_patches.writable_varying_data().create_array("Cs", "k3d::color")

# We will create two identical bilinear patches
for i in range(2):
	patch_selection.append(0)
	patch_materials.append(None)

	for j in range(4):
		patch_points.append(len(points) + j)

	positions = [ (-5, 0, 5), (5, 0, 5), (0, -5, -5), (0, 5, -5) ]

	for position in positions:
		points.append(k3d.point3(position[0] + (12 * i), position[1], position[2]))
		point_selection.append(0.0)

	Cs.append(k3d.color(1, 0, 0))
	Cs.append(k3d.color(0, 1, 0))
	Cs.append(k3d.color(0, 0, 1))
	Cs.append(k3d.color(1, 1, 1))

