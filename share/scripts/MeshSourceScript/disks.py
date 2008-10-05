#python

from math import radians

import k3d
k3d.check_node_environment(locals(), "MeshSourceScript")

# Construct a sphere mesh primitive ...
disks = Output.primitives().create("disk")
matrices = disks.topology().create("matrices", "k3d::matrix4")
materials = disks.topology().create("materials", "k3d::imaterial*")
heights = disks.topology().create("heights", "k3d::double_t")
radii = disks.topology().create("radii", "k3d::double_t")
sweep_angles = disks.topology().create("sweep_angles", "k3d::double_t")
constant = disks.attributes().create("constant")
uniform = disks.attributes().create("uniform")
varying = disks.attributes().create("varying")
color = varying.create("Cs", "k3d::color")

# Add two disks ...
matrices.append(k3d.translate3(k3d.vector3(-5, 0, 0)))
materials.append(None)
heights.append(0)
radii.append(4)
sweep_angles.append(radians(360))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(1, 1, 0))
color.append(k3d.color(1, 1, 0))

matrices.append(k3d.translate3(k3d.vector3(5, 0, 0)))
materials.append(None)
heights.append(0)
radii.append(4)
sweep_angles.append(radians(360))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(0, 0, 1))
color.append(k3d.color(0, 0, 1))

