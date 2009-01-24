#python

from math import radians

import k3d
k3d.check_node_environment(locals(), "MeshSourceScript")

# Construct a sphere mesh primitive ...
sphere = k3d.sphere.create(Output)
color = sphere.varying_data().create("Cs", "k3d::color")

# Add two spheres ...
sphere.matrices().append(k3d.translate3(k3d.vector3(-3, 0, 0)))
sphere.materials().append(None)
sphere.radii().append(2)
sphere.z_min().append(-1)
sphere.z_max().append(1)
sphere.sweep_angles().append(radians(360))
sphere.selections().append(0)
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(1, 1, 0))
color.append(k3d.color(1, 1, 0))

sphere.matrices().append(k3d.translate3(k3d.vector3(3, 0, 0)))
sphere.materials().append(None)
sphere.radii().append(2)
sphere.z_min().append(-1)
sphere.z_max().append(1)
sphere.sweep_angles().append(radians(360))
sphere.selections().append(0)
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(0, 0, 1))
color.append(k3d.color(0, 0, 1))

