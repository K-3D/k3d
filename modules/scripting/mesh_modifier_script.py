#python

import k3d

Output.copy(Input)

scale = 1.5

input_points = Input.points()
output_points = Output.points()

for i in range(len(output_points)):
	output_points[i] = input_points[i] * scale

