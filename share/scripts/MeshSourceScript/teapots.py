#python

import k3d
k3d.check_node_environment(locals(), "MeshSourceScript")

# Construct a teapot mesh primitive ...
teapots = Output.primitives().create("teapot")
matrices = teapots.topology().create("matrices", "k3d::matrix4")
materials = teapots.topology().create("materials", "k3d::imaterial*")
constant = teapots.attributes().create("constant")
uniform = teapots.attributes().create("uniform")
color = uniform.create("Cs", "k3d::color")

# Add three teapots ...
matrices.append(k3d.translate3(k3d.vector3(-7, 0, 0)))
materials.append(None)
color.append(k3d.color(1, 0, 0))

matrices.append(k3d.translate3(k3d.vector3(0, 0, 0)))
materials.append(None)
color.append(k3d.color(0, 1, 0))

matrices.append(k3d.translate3(k3d.vector3(7, 0, 0)))
materials.append(None)
color.append(k3d.color(0, 0, 1))

