#python

from math import radians

import k3d
k3d.check_node_environment(locals(), "MeshSourceScript")

# Construct a sphere mesh primitive ...
torus = k3d.torus.create(Output)
color = torus.varying_data().create("Cs", "k3d::color")

# Add two tori ...
torus.matrices().append(k3d.translate3(k3d.vector3(-5, 0, 0)))
torus.materials().append(None)
torus.major_radii().append(3)
torus.minor_radii().append(2)
torus.phi_min().append(0)
torus.phi_max().append(radians(360))
torus.sweep_angles().append(radians(360))
torus.selections().append(0)
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(1, 1, 0))
color.append(k3d.color(1, 1, 0))

torus.matrices().append(k3d.translate3(k3d.vector3(5, 0, 0)))
torus.materials().append(None)
torus.major_radii().append(3)
torus.minor_radii().append(2)
torus.phi_min().append(0)
torus.phi_max().append(radians(360))
torus.sweep_angles().append(radians(360))
torus.selections().append(0)
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(0, 0, 1))
color.append(k3d.color(0, 0, 1))

