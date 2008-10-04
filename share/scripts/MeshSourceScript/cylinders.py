#python

from math import radians

import k3d
k3d.check_node_environment(locals(), "MeshSourceScript")

# Construct a sphere mesh primitive ...
cylinders = Output.primitives().create("cylinder")
matrices = cylinders.topology().create("matrices", "k3d::matrix4")
materials = cylinders.topology().create("materials", "k3d::imaterial*")
radii = cylinders.topology().create("radii", "k3d::double_t")
z_min = cylinders.topology().create("z_min", "k3d::double_t")
z_max = cylinders.topology().create("z_max", "k3d::double_t")
sweep_angles = cylinders.topology().create("sweep_angles", "k3d::double_t")
constant = cylinders.attributes().create("constant")
uniform = cylinders.attributes().create("uniform")
varying = cylinders.attributes().create("varying")
color = varying.create("Cs", "k3d::color")

# Add two cylinders ...
matrices.append(k3d.translate3(k3d.vector3(-3, 0, 0)))
materials.append(None)
radii.append(2)
z_min.append(-5)
z_max.append(5)
sweep_angles.append(radians(360))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(1, 1, 0))
color.append(k3d.color(1, 1, 0))

matrices.append(k3d.translate3(k3d.vector3(3, 0, 0)))
materials.append(None)
radii.append(2)
z_min.append(-5)
z_max.append(5)
sweep_angles.append(radians(360))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(0, 0, 1))
color.append(k3d.color(0, 0, 1))

