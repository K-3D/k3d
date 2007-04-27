#python

# Load this script into a K-3D RenderManScript object to create
# what is either a Tribble or a really bad-hair-day ...

import k3d
import sys
import ri
from ri import *
from random import *
from cgtypes import vec3
from noise import vsnoise
from sl import mix

message = """You're probably trying to run this script manually, which won't work - this script is meant to be loaded into a RenderManScript node, where it will be run at render-time.

Use the Create > RenderMan > RenderManScript menu item to create the node, then load this file into its Script property."""

try:
      Archive
except:
      k3d.ui.error_message(message)
      raise


# Redirect output to our RIB archive
ri._ribout = open(Archive, "w")

body_size = 5
lumpyness = 1
hair_length = 2
hair_count = 10000
hair_wavyness = 1

control_point_counts = []
control_points = []
widths = []

seed(12345)

for i in range(hair_count):
	control_point_counts.append(4)

	v = vec3(random() - 0.5, random() - 0.5, random() - 0.5).normalize()

	p1 = v * body_size
	p1 += vsnoise(p1) * lumpyness

	p4 = p1 + v * hair_length
	p4 += vsnoise(p4) * hair_wavyness

	p2 = mix(p1, p4, 0.2)
	p2 += vsnoise(p2)

	p3 = mix(p1, p4, 0.8)
	p3 += vsnoise(p3)

	control_points.append(p1)
	control_points.append(p2)
	control_points.append(p3)
	control_points.append(p4)

	widths.append(0.08)
	widths.append(0.01)

RiSurface("k3d_hair")
RiCurves(RI_CUBIC, control_point_counts, RI_NONPERIODIC, "P", control_points, "width", widths)

ri._ribout.flush()

