#python

from math import radians

import k3d
k3d.check_node_environment(locals(), "MeshSourceScript")

# Construct a sphere mesh primitive ...
hyperboloid = k3d.hyperboloid.create(Output)
color = hyperboloid.varying_data().create("Cs", "k3d::color")

# Add two hyperboloids ...
hyperboloid.matrices().append(k3d.translate3(k3d.vector3(-5, 0, 0)))
hyperboloid.materials().append(None)
hyperboloid.start_points().append(k3d.point3(-2, 2, 2))
hyperboloid.end_points().append(k3d.point3(2, 2, -2))
hyperboloid.sweep_angles().append(radians(360))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(1, 1, 0))
color.append(k3d.color(1, 1, 0))

hyperboloid.matrices().append(k3d.translate3(k3d.vector3(5, 0, 0)))
hyperboloid.materials().append(None)
hyperboloid.start_points().append(k3d.point3(-2, 2, 2))
hyperboloid.end_points().append(k3d.point3(2, 2, -2))
hyperboloid.sweep_angles().append(radians(360))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(0, 0, 1))
color.append(k3d.color(0, 0, 1))

