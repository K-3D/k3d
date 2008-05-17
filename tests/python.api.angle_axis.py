#python

import k3d
import math

a = k3d.angle_axis(90, k3d.vector3(0, 0, 1))

if math.floor(a.angle * 10000) != 900000:
	raise "angle incorrect"

if math.floor(a.angle_radians * 10000) != 15707:
	raise "internal angle incorrect"

