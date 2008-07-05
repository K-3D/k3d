#python

import k3d
k3d.check_node_environment(locals(), "MeshModifierScript")

from math import fmod
from cgkit.noise import vsnoise

time = Node.time
gravity = Node.gravity
z1 = Node.z1
z2 = Node.z2
dz = z2 - z1

Output.copy(Input)

points = Output.writable_points()
for i in range(len(points)):
	point = points[i]
	wiggle = 10 * vsnoise((point[0], point[1], point[2]), time / 2)
	point[0] += wiggle.x
	point[1] += wiggle.y
	point[2] += wiggle.z
	point[2] = fmod((point[2] + (time * gravity) - z1), dz) + z1
	points[i] = point
