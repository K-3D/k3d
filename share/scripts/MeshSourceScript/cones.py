#python

from math import radians

import k3d
k3d.check_node_environment(locals(), "MeshSourceScript")

# Construct a sphere mesh primitive ...
cones = Output.primitives().create("cone")
matrices = cones.topology().create("matrices", "k3d::matrix4")
materials = cones.topology().create("materials", "k3d::imaterial*")
heights = cones.topology().create("heights", "k3d::double_t")
radii = cones.topology().create("radii", "k3d::double_t")
sweep_angles = cones.topology().create("sweep_angles", "k3d::double_t")
constant = cones.attributes().create("constant")
uniform = cones.attributes().create("uniform")
varying = cones.attributes().create("varying")
color = varying.create("Cs", "k3d::color")

# Add two cones ...
matrices.append(k3d.translate3(k3d.vector3(-5, 0, 0)))
materials.append(None)
heights.append(5)
radii.append(4)
sweep_angles.append(radians(360))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(1, 1, 0))
color.append(k3d.color(1, 1, 0))

matrices.append(k3d.translate3(k3d.vector3(5, 0, 0)))
materials.append(None)
heights.append(5)
radii.append(4)
sweep_angles.append(radians(360))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(0, 0, 1))
color.append(k3d.color(0, 0, 1))

