#python

import k3d
k3d.check_node_environment(locals(), "MeshSourceScript")

# Perform required one-time setup to store geometric points in the mesh.
points = Output.create_points()
point_selection = Output.create_point_selection()

# Perform required one-time setup to store NURBS patches in the mesh.
nurbs_patches = Output.create_nurbs_patches()
patch_first_points = nurbs_patches.create_patch_first_points()
patch_u_point_counts = nurbs_patches.create_patch_u_point_counts()
patch_v_point_counts = nurbs_patches.create_patch_v_point_counts()
patch_u_orders = nurbs_patches.create_patch_u_orders()
patch_v_orders = nurbs_patches.create_patch_v_orders()
patch_u_first_knots = nurbs_patches.create_patch_u_first_knots()
patch_selection = nurbs_patches.create_patch_selection()
patch_v_first_knots = nurbs_patches.create_patch_v_first_knots()
patch_materials = nurbs_patches.create_patch_materials()
patch_points = nurbs_patches.create_patch_points()
patch_point_weights = nurbs_patches.create_patch_point_weights()
patch_u_knots = nurbs_patches.create_patch_u_knots()
patch_v_knots = nurbs_patches.create_patch_v_knots()

# Create an (optional) array to hold color values at the parametric
# corners of each patch.
Cs = nurbs_patches.writable_varying_data().create_array("Cs", "k3d::color")

# We will create two identical NURBS patches.
for i in range(2):
	patch_first_points.append(len(patch_points))
	patch_u_point_counts.append(3)
	patch_v_point_counts.append(2)
	patch_u_orders.append(3)
	patch_v_orders.append(2)
	patch_u_first_knots.append(len(patch_u_knots))
	patch_v_first_knots.append(len(patch_v_knots))
	patch_selection.append(0)
	patch_materials.append(None)

	patch_points.append(len(points) + 0)
	patch_points.append(len(points) + 1)
	patch_points.append(len(points) + 2)
	patch_points.append(len(points) + 3)
	patch_points.append(len(points) + 4)
	patch_points.append(len(points) + 5)

	patch_point_weights.append(1)
	patch_point_weights.append(10)
	patch_point_weights.append(1)
	patch_point_weights.append(1)
	patch_point_weights.append(0.5)
	patch_point_weights.append(1)

	patch_u_knots.append(1)
	patch_u_knots.append(1)
	patch_u_knots.append(1)
	patch_u_knots.append(2)
	patch_u_knots.append(2)
	patch_u_knots.append(2)

	patch_v_knots.append(1)
	patch_v_knots.append(1)
	patch_v_knots.append(2)
	patch_v_knots.append(2)

	Cs.append(k3d.color(1, 0, 0))
	Cs.append(k3d.color(0, 1, 0))
	Cs.append(k3d.color(0, 0, 1))
	Cs.append(k3d.color(1, 1, 1))

	positions = [(-5, 5, 5), (5, 5, 5), (5, 5, 10), (-5, -5, 5), (5, -5, 5), (5, -5, 10)]

	for position in positions:
		points.append(k3d.point3(position[0], position[1], position[2] + (i * -10)))
		point_selection.append(0.0)


