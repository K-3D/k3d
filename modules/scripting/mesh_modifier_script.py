#python

import k3d

context.output.copy(context.input)

scale = 1.5

input_points = context.input.points()
output_points = context.output.points()

for i in range(len(output_points)):
	output_points[i] = input_points[i] * scale

