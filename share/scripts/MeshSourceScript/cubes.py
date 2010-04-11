#python

import k3d
k3d.check_node_environment(context, "MeshSourceScript")

# Construct a cube mesh primitive ...
cubes = context.output.primitives().create("cube")
matrices = cubes.topology().create("matrices", "k3d::matrix4")
materials = cubes.topology().create("materials", "k3d::imaterial*")
uniform = cubes.attributes().create("uniform")
color = uniform.create("Cs", "k3d::color")

# Add three cubes ...
matrices.append(k3d.translate3(k3d.vector3(-7, 0, 0)))
materials.append(None)
color.append(k3d.color(1, 0, 0))

matrices.append(k3d.translate3(k3d.vector3(0, 0, 0)))
materials.append(None)
color.append(k3d.color(0, 1, 0))

matrices.append(k3d.translate3(k3d.vector3(7, 0, 0)))
materials.append(None)
color.append(k3d.color(0, 0, 1))

print repr(context.output)
