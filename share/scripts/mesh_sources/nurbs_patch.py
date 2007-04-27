#python

import k3d

positions = [(-5, 5, 0), (5, 5, 0), (5, 5, 5)]

points = Output.create_points()
for position in positions:
	points.append(k3d.point3(position[0], position[1], position[2]))
for position in positions:
	points.append(k3d.point3(position[0], position[1] - 10, position[2]))

nurbs_patches = Output.create_nurbs_patches()

patch_first_points = nurbs_patches.create_patch_first_points()
patch_first_points.assign([0])

patch_u_point_counts = nurbs_patches.create_patch_u_point_counts()
patch_u_point_counts.assign([3])

patch_v_point_counts = nurbs_patches.create_patch_v_point_counts()
patch_v_point_counts.assign([2])

patch_u_orders = nurbs_patches.create_patch_u_orders()
patch_u_orders.assign([3])

patch_v_orders = nurbs_patches.create_patch_v_orders()
patch_v_orders.assign([2])

patch_u_first_knots = nurbs_patches.create_patch_u_first_knots()
patch_u_first_knots.assign([0])

patch_v_first_knots = nurbs_patches.create_patch_v_first_knots()
patch_v_first_knots.assign([0])

patch_selection = nurbs_patches.create_patch_selection()
patch_selection.assign([0])

patch_materials = nurbs_patches.create_patch_materials()
patch_materials.assign([None])

patch_points = nurbs_patches.create_patch_points()
patch_points.assign([0, 1, 2, 3, 4, 5])

patch_point_weights = nurbs_patches.create_patch_point_weights()
patch_point_weights.assign([1, 10, 1, 1, 1, 1])

patch_u_knots = nurbs_patches.create_patch_u_knots()
patch_u_knots.assign([1,1,1,2,2,2])

patch_v_knots = nurbs_patches.create_patch_v_knots()
patch_v_knots.assign([1,1,2,2])

Cs = nurbs_patches.writable_varying_data.create_array("Cs", "k3d::color")
Cs.assign([k3d.color(1, 0, 0), k3d.color(0, 1, 0), k3d.color(0, 0, 1), k3d.color(1, 1, 1)])

