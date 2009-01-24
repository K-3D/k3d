#python

from math import radians

import k3d
k3d.check_node_environment(locals(), "MeshSourceScript")

# Construct a cone mesh primitive ...
cone = k3d.cone.create(Output)
color = cone.varying_data().create("Cs", "k3d::color")

# Add two cones ...
cone.matrices().append(k3d.translate3(k3d.vector3(-5, 0, 0)))
cone.materials().append(None)
cone.heights().append(5)
cone.radii().append(4)
cone.sweep_angles().append(radians(360))
cone.selections().append(0)
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(1, 1, 0))
color.append(k3d.color(1, 1, 0))

cone.matrices().append(k3d.translate3(k3d.vector3(5, 0, 0)))
cone.materials().append(None)
cone.heights().append(5)
cone.radii().append(4)
cone.sweep_angles().append(radians(360))
cone.selections().append(0)
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(0, 0, 1))
color.append(k3d.color(0, 0, 1))

