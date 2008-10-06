#python

from math import radians

import k3d
k3d.check_node_environment(locals(), "MeshSourceScript")

# Construct a sphere mesh primitive ...
hyperboloids = Output.primitives().create("hyperboloid")
matrices = hyperboloids.topology().create("matrices", "k3d::matrix4")
materials = hyperboloids.topology().create("materials", "k3d::imaterial*")
start_points = hyperboloids.topology().create("start_points", "k3d::point3")
end_points = hyperboloids.topology().create("end_points", "k3d::point3")
sweep_angles = hyperboloids.topology().create("sweep_angles", "k3d::double_t")
constant = hyperboloids.attributes().create("constant")
uniform = hyperboloids.attributes().create("uniform")
varying = hyperboloids.attributes().create("varying")
color = varying.create("Cs", "k3d::color")

# Add two hyperboloids ...
matrices.append(k3d.translate3(k3d.vector3(-5, 0, 0)))
materials.append(None)
start_points.append(k3d.point3(-2, 2, 2))
end_points.append(k3d.point3(2, 2, -2))
sweep_angles.append(radians(360))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(1, 1, 0))
color.append(k3d.color(1, 1, 0))

matrices.append(k3d.translate3(k3d.vector3(5, 0, 0)))
materials.append(None)
start_points.append(k3d.point3(-2, 2, 2))
end_points.append(k3d.point3(2, 2, -2))
sweep_angles.append(radians(360))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(0, 0, 1))
color.append(k3d.color(0, 0, 1))

