#python

import k3d

def setup_matrix_modifier_test(source_name, matrix):
	doc = k3d.new_document()

	source = doc.new_node(source_name)
	source.input_matrix = matrix

	class result_object:
		pass

	result = result_object
	result.document = doc
	result.source = source

	return result

def matrix_comparison(value, expected_value):
	#print """<DartMeasurement name="Value" type="numeric/float">""" + str(value) + """</DartMeasurement>"""
	#print """<DartMeasurement name="Expected Value" type="numeric/float">""" + str(expected_value) + """</DartMeasurement>"""
	if value != expected_value:
		raise Exception("matrix does not match expected values")

matrix = k3d.matrix4()
row = matrix[0]; row[0] = 1; row[1] = 5; row[2] =  2; row[3] = 4; matrix[0] = row
row = matrix[1]; row[0] = 2; row[1] = 2; row[2] =  2; row[3] = 3; matrix[1] = row
row = matrix[2]; row[0] = 3; row[1] = 7; row[2] =  2; row[3] = 2; matrix[2] = row
row = matrix[3]; row[0] = 4; row[1] = 8; row[2] =  4; row[3] = 1; matrix[3] = row

setup = setup_matrix_modifier_test("InvertMatrix", matrix)

matrix_compare = k3d.matrix4()
row = matrix_compare[0]; row[0] = -0.525; row[1] = 0.45;  row[2] =  0.475; row[3] = -0.20000000000000007; matrix_compare[0] = row
row = matrix_compare[1]; row[0] = 0.125;  row[1] = -0.25; row[2] =  0.125; row[3] =  0.0; matrix_compare[1] = row
row = matrix_compare[2]; row[0] = 0.25;   row[1] = 0.0;   row[2] = -0.75;  row[3] =  0.5; matrix_compare[2] = row
row = matrix_compare[3]; row[0] = 0.1;    row[1] = 0.2;   row[2] =  0.1;   row[3] = -0.2; matrix_compare[3] = row

matrix_comparison(setup.source.output_matrix, matrix_compare)
