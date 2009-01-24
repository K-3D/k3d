#python

from math import radians

import k3d
k3d.check_node_environment(locals(), "MeshSourceScript")

# Construct a sphere mesh primitive ...
disk = k3d.disk.create(Output)
color = disk.varying_data().create("Cs", "k3d::color")

# Add two disks ...
disk.matrices().append(k3d.translate3(k3d.vector3(-5, 0, 0)))
disk.materials().append(None)
disk.heights().append(0)
disk.radii().append(4)
disk.sweep_angles().append(radians(360))
disk.selections().append(0)
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(1, 1, 0))
color.append(k3d.color(1, 1, 0))

disk.matrices().append(k3d.translate3(k3d.vector3(5, 0, 0)))
disk.materials().append(None)
disk.heights().append(0)
disk.radii().append(4)
disk.sweep_angles().append(radians(360))
disk.selections().append(0)
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(1, 0, 0))
color.append(k3d.color(0, 0, 1))
color.append(k3d.color(0, 0, 1))

