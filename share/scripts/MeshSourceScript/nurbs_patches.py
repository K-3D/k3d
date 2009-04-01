#python

import k3d
k3d.check_node_environment(locals(), "MeshSourceScript")

# Perform required one-time setup to store geometric points in the mesh ...
points = Output.create_points()
point_selection = Output.create_point_selection()

# Perform required one-time setup to store NURBS patches in the mesh ...
patches = k3d.nurbs_patch.create(Output)

# Create an (optional) array to hold color values at the parametric
# corners of each patch ...
#Cs = nurbs_patches.writable_varying_data().create("Cs", "k3d::color")

# We will create two identical NURBS patches ...
for i in range(2):
	patches.patch_first_points().append(len(patches.patch_points()))
	patches.patch_u_point_counts().append(3)
	patches.patch_v_point_counts().append(2)
	patches.patch_u_orders().append(3)
	patches.patch_v_orders().append(2)
	patches.patch_u_first_knots().append(len(patches.patch_u_knots()))
	patches.patch_v_first_knots().append(len(patches.patch_v_knots()))
	patches.patch_selections().append(0)
	patches.patch_materials().append(None)

	patches.patch_points().append(len(points) + 0)
	patches.patch_points().append(len(points) + 1)
	patches.patch_points().append(len(points) + 2)
	patches.patch_points().append(len(points) + 3)
	patches.patch_points().append(len(points) + 4)
	patches.patch_points().append(len(points) + 5)

	patches.patch_point_weights().append(1)
	patches.patch_point_weights().append(10)
	patches.patch_point_weights().append(1)
	patches.patch_point_weights().append(1)
	patches.patch_point_weights().append(0.5)
	patches.patch_point_weights().append(1)

	patches.patch_u_knots().append(1)
	patches.patch_u_knots().append(1)
	patches.patch_u_knots().append(1)
	patches.patch_u_knots().append(2)
	patches.patch_u_knots().append(2)
	patches.patch_u_knots().append(2)

	patches.patch_v_knots().append(1)
	patches.patch_v_knots().append(1)
	patches.patch_v_knots().append(2)
	patches.patch_v_knots().append(2)

	patches.patch_first_trim_loops().append(0)
	patches.patch_trim_loop_counts().append(0)

#	Cs.append(k3d.color(1, 0, 0))
#	Cs.append(k3d.color(0, 1, 0))
#	Cs.append(k3d.color(0, 0, 1))
#	Cs.append(k3d.color(1, 1, 1))

	positions = [(-5, 5, 5), (5, 5, 5), (5, 5, 10), (-5, -5, 5), (5, -5, 5), (5, -5, 10)]

	for position in positions:
		points.append(k3d.point3(position[0], position[1], position[2] + (i * -10)))
		point_selection.append(0.0)


