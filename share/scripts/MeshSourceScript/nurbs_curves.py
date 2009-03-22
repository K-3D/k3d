#python

import k3d
k3d.check_node_environment(locals(), "MeshSourceScript")

# Perform required one-time setup to store geometric points in the mesh ...
points = Output.create_points()
point_selection = Output.create_point_selection()

# Perform required one-time setup to store NURBS curves in the mesh ...
curves = k3d.nurbs_curve.create(Output)

# Add some curves ...
curves.material().append(None)

for j in range(3):
	curves.curve_first_points().append(len(curves.curve_points()))
	curves.curve_point_counts().append(4)
	curves.curve_orders().append(3)
	curves.curve_first_knots().append(len(curves.curve_knots()))
	curves.curve_selections().append(0.0)

	curves.curve_points().append(len(points) + 0)
	curves.curve_points().append(len(points) + 1)
	curves.curve_points().append(len(points) + 2)
	curves.curve_points().append(len(points) + 3)
	
	curves.curve_point_weights().append(1)
	curves.curve_point_weights().append(0.5)
	curves.curve_point_weights().append(1)
	curves.curve_point_weights().append(1)
	
	curves.curve_knots().append(1)
	curves.curve_knots().append(1)
	curves.curve_knots().append(1)
	curves.curve_knots().append(2)
	curves.curve_knots().append(3)
	curves.curve_knots().append(3)
	curves.curve_knots().append(3)

	positions = [(-5, -5, 0), (5, -5, 0), (5, 5, 0), (-5, 5, 0)]

	for position in positions:
		points.append(k3d.point3(position[0] + (j * 10), position[1], position[2]))
		point_selection.append(0.0)

