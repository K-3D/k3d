#python

import k3d
k3d.check_node_environment(locals(), "MeshSourceScript")

# Perform required one-time setup to store geometric points in the mesh ...
points = Output.create_points()
point_selection = Output.create_point_selection()

# Perform required one-time setup to store bezier triangle patches in the mesh ...
patches = k3d.bezier_triangle_patch.create(Output)

# We will create two identical bezier triangle patches ...
for i in range(2):
	patches.patch_first_points().append(len(points))
	patches.patch_selections().append(0)
	patches.patch_materials().append(None)
	patches.patch_orders().append(5)

	# Assign patch_points and patch_weights here ...
	#for j in range(something_or_other):
	#	patches.patch_points().append(len(points) + j)
	#	patches.patch_weights().append(1)

	#positions = [
	#	(-5, -5, 0), (-2, -5, 2), (2, -5, -2), (5, -5, 0),
	#	(-5, -2, 2), (-2, -2, 5), (2, -2, -5), (5, -2, -2),
	#	(-5, 2, 2), (-2, 2, 5), (2, 2, -5), (5, 2, -2),
	#	(-5, 5, 0), (-2, 5, 2), (2, 5, -2), (5, 5, 0)]

	#for position in positions:
	#	points.append(k3d.point3(position[0] + (12 * i), position[2], -position[1]))
	#	point_selection.append(0.0)



