#python

from math import radians

import k3d
k3d.check_node_environment(context, "MeshSourceScript")

# Construct a sphere mesh primitive ...
cylinder = k3d.cylinder.create(context.output)
color = cylinder.parameter_attributes().create("Cs", "k3d::color")

# Add two cylinders ...
cylinder.matrices().append(k3d.translate3(k3d.vector3(-3, 0, 0)))
cylinder.materials().append(None)
cylinder.radii().append(2)
cylinder.z_min().append(-5)
cylinder.z_max().append(5)
cylinder.sweep_angles().append(radians(360))
cylinder.selections().append(0)
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(1, 1, 0))
color.append(k3d.color(1, 1, 0))

cylinder.matrices().append(k3d.translate3(k3d.vector3(3, 0, 0)))
cylinder.materials().append(None)
cylinder.radii().append(2)
cylinder.z_min().append(-5)
cylinder.z_max().append(5)
cylinder.sweep_angles().append(radians(360))
cylinder.selections().append(0)
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(0, 0, 1))
color.append(k3d.color(0, 0, 1))

