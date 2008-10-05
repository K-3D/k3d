#python

from math import radians

import k3d
k3d.check_node_environment(locals(), "MeshSourceScript")

# Construct a sphere mesh primitive ...
paraboloids = Output.primitives().create("paraboloid")
matrices = paraboloids.topology().create("matrices", "k3d::matrix4")
materials = paraboloids.topology().create("materials", "k3d::imaterial*")
radii = paraboloids.topology().create("radii", "k3d::double_t")
z_min = paraboloids.topology().create("z_min", "k3d::double_t")
z_max = paraboloids.topology().create("z_max", "k3d::double_t")
sweep_angles = paraboloids.topology().create("sweep_angles", "k3d::double_t")
constant = paraboloids.attributes().create("constant")
uniform = paraboloids.attributes().create("uniform")
varying = paraboloids.attributes().create("varying")
color = varying.create("Cs", "k3d::color")

# Add two paraboloids ...
matrices.append(k3d.translate3(k3d.vector3(-5, 0, 0)))
materials.append(None)
radii.append(3)
z_min.append(0)
z_max.append(6)
sweep_angles.append(radians(360))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(1, 1, 0))
color.append(k3d.color(1, 1, 0))

matrices.append(k3d.translate3(k3d.vector3(5, 0, 0)))
materials.append(None)
radii.append(3)
z_min.append(0)
z_max.append(6)
sweep_angles.append(radians(360))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(0, 0, 1))
color.append(k3d.color(0, 0, 1))

