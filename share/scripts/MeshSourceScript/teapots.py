#python

import k3d
k3d.check_node_environment(locals(), "MeshSourceScript")

# Construct a teapot mesh primitive ...
teapot = k3d.teapot.create(Output)
color = teapot.uniform_data().create("Cs", "k3d::color")

# Add three teapots ...
teapot.matrices().append(k3d.translate3(k3d.vector3(-7, 0, 0)))
teapot.materials().append(None)
color.append(k3d.color(1, 0, 0))

teapot.matrices().append(k3d.translate3(k3d.vector3(0, 0, 0)))
teapot.materials().append(None)
color.append(k3d.color(0, 1, 0))

teapot.matrices().append(k3d.translate3(k3d.vector3(7, 0, 0)))
teapot.materials().append(None)
color.append(k3d.color(0, 0, 1))

