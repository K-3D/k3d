#python

from math import radians

import k3d
k3d.check_node_environment(locals(), "MeshSourceScript")

# Construct a sphere mesh primitive ...
tori = Output.primitives().create("torus")
matrices = tori.topology().create("matrices", "k3d::matrix4")
materials = tori.topology().create("materials", "k3d::imaterial*")
major_radii = tori.topology().create("major_radii", "k3d::double_t")
minor_radii = tori.topology().create("minor_radii", "k3d::double_t")
phi_min = tori.topology().create("phi_min", "k3d::double_t")
phi_max = tori.topology().create("phi_max", "k3d::double_t")
sweep_angles = tori.topology().create("sweep_angles", "k3d::double_t")
constant = tori.attributes().create("constant")
uniform = tori.attributes().create("uniform")
varying = tori.attributes().create("varying")
color = varying.create("Cs", "k3d::color")

# Add two tori ...
matrices.append(k3d.translate3(k3d.vector3(-5, 0, 0)))
materials.append(None)
major_radii.append(3)
minor_radii.append(2)
phi_min.append(0)
phi_max.append(radians(360))
sweep_angles.append(radians(360))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(1, 1, 0))
color.append(k3d.color(1, 1, 0))

matrices.append(k3d.translate3(k3d.vector3(5, 0, 0)))
materials.append(None)
major_radii.append(3)
minor_radii.append(2)
phi_min.append(0)
phi_max.append(radians(360))
sweep_angles.append(radians(360))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(0, 0, 1))
color.append(k3d.color(0, 0, 1))

