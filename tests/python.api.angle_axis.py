#python

import k3d
import math

a = k3d.angle_axis(math.radians(90), k3d.vector3(0, 0, 1))

if math.degrees(a.angle) != 90:
	raise "angle incorrect"

