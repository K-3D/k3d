#python

import k3d
k3d.check_node_environment(locals(), "MeshSourceScript")

positions = [(-5, -5, 0), (5, -5, 0), (5, 5, 0), (-5, 5, 0)]

points = Output.create_points()
point_selection = Output.create_point_selection()

for position in positions:
	points.append(k3d.point3(position[0], position[1], position[2]))
	point_selection.append(0.0)

nurbs_curve_groups = Output.create_nurbs_curve_groups()

first_curves = nurbs_curve_groups.create_first_curves()
first_curves.assign([0])

curve_counts = nurbs_curve_groups.create_curve_counts()
curve_counts.assign([1])

curve_first_points = nurbs_curve_groups.create_curve_first_points()
curve_first_points.assign([0])

curve_point_counts = nurbs_curve_groups.create_curve_point_counts()
curve_point_counts.assign([4])

curve_orders = nurbs_curve_groups.create_curve_orders()
curve_orders.assign([3])

curve_first_knots = nurbs_curve_groups.create_curve_first_knots()
curve_first_knots.assign([0])

curve_materials = nurbs_curve_groups.create_materials()
curve_materials.assign([None])

curve_selection = nurbs_curve_groups.create_curve_selection()
curve_selection.assign([0])

curve_points = nurbs_curve_groups.create_curve_points()
curve_points.assign([0, 1, 2, 3])

curve_point_weights = nurbs_curve_groups.create_curve_point_weights()
curve_point_weights.assign([1, 0.5, 1, 1])

curve_knots = nurbs_curve_groups.create_curve_knots()
curve_knots.assign([1,1,1,2,3,3,3])

