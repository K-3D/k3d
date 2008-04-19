#python

import k3d
k3d.check_node_environment(locals(), "MeshSourceScript")

# Perform required one-time setup to store geometric points in the mesh ...
points = Output.create_points()
point_selection = Output.create_point_selection()

# Perform required one-time setup to store NURBS curves in the mesh ...
nurbs_curve_groups = Output.create_nurbs_curve_groups()
first_curves = nurbs_curve_groups.create_first_curves()
curve_counts = nurbs_curve_groups.create_curve_counts()
curve_first_points = nurbs_curve_groups.create_curve_first_points()
curve_point_counts = nurbs_curve_groups.create_curve_point_counts()
curve_orders = nurbs_curve_groups.create_curve_orders()
curve_first_knots = nurbs_curve_groups.create_curve_first_knots()
curve_materials = nurbs_curve_groups.create_materials()
curve_selection = nurbs_curve_groups.create_curve_selection()
curve_points = nurbs_curve_groups.create_curve_points()
curve_point_weights = nurbs_curve_groups.create_curve_point_weights()
curve_knots = nurbs_curve_groups.create_curve_knots()

# Create two curve groups ...
for i in range(2):
	first_curves.append(len(curve_first_points))
	curve_counts.append(3)
	curve_materials.append(None)

	# Create three curves in each group ...
	for j in range(3):
		curve_first_points.append(len(curve_points))
		curve_point_counts.append(4)
		curve_orders.append(3)
		curve_first_knots.append(len(curve_knots))
		curve_selection.append(0)

		curve_points.append(len(points) + 0)
		curve_points.append(len(points) + 1)
		curve_points.append(len(points) + 2)
		curve_points.append(len(points) + 3)
		
		curve_point_weights.append(1)
		curve_point_weights.append(0.5)
		curve_point_weights.append(1)
		curve_point_weights.append(1)
		
		curve_knots.append(1)
		curve_knots.append(1)
		curve_knots.append(1)
		curve_knots.append(2)
		curve_knots.append(3)
		curve_knots.append(3)
		curve_knots.append(3)

		positions = [(-5, -5, 0), (5, -5, 0), (5, 5, 0), (-5, 5, 0)]

		for position in positions:
			points.append(k3d.point3(position[0] + (j * 10), position[1], position[2] + (i * -10)))
			point_selection.append(0.0)

