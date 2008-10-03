#python

from math import radians

import k3d
k3d.check_node_environment(locals(), "MeshSourceScript")

# Construct a sphere mesh primitive ...
spheres = Output.primitives().create("sphere")
matrices = spheres.topology().create("matrices", "k3d::matrix4")
materials = spheres.topology().create("materials", "k3d::imaterial*")
radii = spheres.topology().create("radii", "k3d::double_t")
z_min = spheres.topology().create("z_min", "k3d::double_t")
z_max = spheres.topology().create("z_max", "k3d::double_t")
sweep_angles = spheres.topology().create("sweep_angles", "k3d::double_t")
constant = spheres.attributes().create("constant")
uniform = spheres.attributes().create("uniform")
varying = spheres.attributes().create("varying")
color = varying.create("Cs", "k3d::color")

# Add two spheres ...
matrices.append(k3d.translate3(k3d.vector3(-3, 0, 0)))
materials.append(None)
radii.append(2)
z_min.append(-1)
z_max.append(1)
sweep_angles.append(radians(360))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(1, 1, 0))
color.append(k3d.color(1, 1, 0))

matrices.append(k3d.translate3(k3d.vector3(3, 0, 0)))
materials.append(None)
radii.append(2)
z_min.append(-1)
z_max.append(1)
sweep_angles.append(radians(360))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(0, 0, 1))
color.append(k3d.color(0, 0, 1))

