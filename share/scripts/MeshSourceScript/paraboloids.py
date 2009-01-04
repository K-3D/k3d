#python

from math import radians

import k3d
k3d.check_node_environment(locals(), "MeshSourceScript")

# Construct a sphere mesh primitive ...
paraboloids = k3d.paraboloid.create(Output)
color = paraboloids.varying_data().create("Cs", "k3d::color")

# Add two paraboloids ...
paraboloids.matrices().append(k3d.translate3(k3d.vector3(-5, 0, 0)))
paraboloids.materials().append(None)
paraboloids.radii().append(3)
paraboloids.z_min().append(0)
paraboloids.z_max().append(6)
paraboloids.sweep_angles().append(radians(360))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(1, 1, 0))
color.append(k3d.color(1, 1, 0))

paraboloids.matrices().append(k3d.translate3(k3d.vector3(5, 0, 0)))
paraboloids.materials().append(None)
paraboloids.radii().append(3)
paraboloids.z_min().append(0)
paraboloids.z_max().append(6)
paraboloids.sweep_angles().append(radians(360))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(0, 0, 1))
color.append(k3d.color(0, 0, 1))

