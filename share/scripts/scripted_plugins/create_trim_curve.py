#python

# k3d:plugin-class="document"
# k3d:plugin-type="MeshModifierScript"
# k3d:plugin-name="CreateTrimCurve"
# k3d:plugin-description="Adds a circular trim curve to the input"

import k3d
import math

Output.copy(Input)

nurbs_patches = Output.writable_nurbs_patches()

#create the output arrays
patch_trim_curve_loop_counts = nurbs_patches.create_patch_trim_curve_loop_counts()
patch_first_trim_curve_loops = nurbs_patches.create_patch_first_trim_curve_loops()
trim_curve_loops = nurbs_patches.create_trim_curve_loops()
trim_points = nurbs_patches.create_trim_points()
trim_point_selection = nurbs_patches.create_trim_point_selection()
first_trim_curves = nurbs_patches.create_first_trim_curves()
trim_curve_counts = nurbs_patches.create_trim_curve_counts()
trim_curve_loop_selection = nurbs_patches.create_trim_curve_loop_selection()
trim_curve_first_points = nurbs_patches.create_trim_curve_first_points()
trim_curve_point_counts = nurbs_patches.create_trim_curve_point_counts()
trim_curve_orders = nurbs_patches.create_trim_curve_orders()
trim_curve_first_knots = nurbs_patches.create_trim_curve_first_knots()
trim_curve_selection = nurbs_patches.create_trim_curve_selection()
trim_curve_points = nurbs_patches.create_trim_curve_points()
trim_curve_point_weights = nurbs_patches.create_trim_curve_point_weights()
trim_curve_knots = nurbs_patches.create_trim_curve_knots()

#circle parameters
segments = 4
start_angle = 0.0
end_angle = 2*math.pi
theta = (end_angle - start_angle) / segments
weight = math.cos(theta * 0.5)

first_trim_curves.append(0)
trim_curve_counts.append(1)
trim_curve_first_points.append(0)
trim_curve_loop_selection.append(0)
trim_curve_point_counts.append(2*segments+1)
trim_curve_orders.append(3)
trim_curve_first_knots.append(0)
trim_curve_selection.append(0.0)

#knots
for i in range(0, 3):
  trim_curve_knots.append(0)
for i in range(1, segments):
  trim_curve_knots.append(i)
  trim_curve_knots.append(i)
for i in range(0, 3):
  trim_curve_knots.append(segments)

#weights
trim_curve_point_weights.append(1)
for i in range(segments):
  trim_curve_point_weights.append(weight)
  trim_curve_point_weights.append(1)
trim_curve_point_weights.append(weight)

#control points
X = k3d.point3(1, 0, 0)
Y = k3d.point3(0, 1, 0)

trim_points.append(math.cos(start_angle) * k3d.point2(0.75, 0.5) + math.sin(start_angle) * k3d.point2(0.5, 0.75))
for i in range(segments):
  a0 = start_angle + (theta * (i))
  a2 = start_angle + (theta * (i + 1))

  p0 = k3d.point3(math.cos(a0) * X + math.sin(a0) * Y)
  p2 = k3d.point3(math.cos(a2) * X + math.sin(a2) * Y)

  t0 = k3d.point3(-math.sin(a0) * X + math.cos(a0) * Y)
  t2 = k3d.point3(-math.sin(a2) * X + math.cos(a2) * Y)

  p1 = k3d.point3(0, 0, 0)
  k3d.intersect_lines(p0, k3d.to_vector3(t0), p2, k3d.to_vector3(t2), p1)

  trim_points.append(k3d.point2(0.25*p1[0] + 0.5, 0.25*p1[1] + 0.5))
  trim_points.append(k3d.point2(0.25*p2[0] + 0.5, 0.25*p2[1] + 0.5))

for i in range(len(trim_points)):
  trim_point_selection.append(0)

for i in range(2*segments):
  trim_curve_points.append(i)
trim_curve_points.append(0)

patch_trim_curve_loop_counts.append(1)
patch_first_trim_curve_loops.append(0)
trim_curve_loops.append(0)
