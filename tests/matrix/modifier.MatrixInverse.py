#python

import k3d

document = k3d.new_document()
modifier = k3d.plugin.create("MatrixInverse", document)
modifier.input_matrix = k3d.matrix4.row_major((1, 5, 2, 4, 2, 2, 2, 3, 3, 7, 2, 2, 4, 8, 4, 1))

reference_matrix = k3d.matrix4.row_major((-0.525, 0.45, 0.475, -0.2, .125, -0.25, 0.125, 0.0, 0.25, 0.0, -0.75, 0.5, 0.1, 0.2, 0.1, -0.2))

result = k3d.difference.accumulator()
k3d.difference.test(modifier.output_matrix, reference_matrix, result)

print """<DartMeasurement name="Input Value" type="text/string">""" + str(modifier.input_matrix) + """</DartMeasurement>"""
print """<DartMeasurement name="Output Value" type="text/string">""" + str(modifier.output_matrix) + """</DartMeasurement>"""
print """<DartMeasurement name="Expected Value" type="text/string">""" + str(reference_matrix) + """</DartMeasurement>"""
print """<DartMeasurement name="ULPS" type="numeric/integer">""" + str(result.ulps_max()) + """</DartMeasurement>"""

if result.ulps_max() > 10:
	raise Exception("matrix does not match expected values")


