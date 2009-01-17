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
	patches.patch_orders().append(4)
	patches.patch_selections().append(0)
	patches.patch_materials().append(None)

	# from The Platonic Spheroids at http://www.cise.ufl.edu/research/SurfLab/pre99-papers/97.spheroids.ps.gz
	# The tetroid:
	# (3,-3,-3) (1,-1,-7) (-1,1,-7) (-3,3,-3)
	#    (1,-7,-1) (-11,-11,-11) (-7,1,-1)
	#           (-1,-7,1) (-7,-1,1)
	#                (-3,-3,3)
	positions = [
		(3,-3,-3),
		(1,-7,-1), (1,-1,-7),
		(-1,-7,1), (-11,-11,-11), (-1,1,-7),
		(-3,-3,3), (-7,-1,1), (-7,1,-1), (-3,3,-3) ]

	# Assign patch_points and patch_weights here ...
	for j in range(0,len(positions)):
		patches.patch_points().append(len(points) + j)
		patches.patch_point_weights().append(1)

	for position in positions:
		points.append(k3d.point3(position[0] + (12 * i), position[2], -position[1]))
		point_selection.append(0.0)

