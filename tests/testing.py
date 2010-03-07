#python

import k3d
import difflib
import os
import platform
import sys
import time

def platform_agnostic():
	return ""

def platform_specific():
	return "." + str(platform.system()) + "-" + str(platform.machine())

class timer:
	def __init__(self):
		if sys.platform == "win32":
			self.current_time = time.clock
		else:
			self.current_time = time.time

		self.start_time = self.current_time()

	def restart(self):
		self.start_time = self.current_time()

	def elapsed(self):
		return self.current_time() - self.start_time

def source_path():
	return os.environ["K3D_TEST_SOURCE_PATH"]

def binary_path():
	return os.environ["K3D_TEST_BINARY_PATH"]

def benchmark_path():
	b_path = binary_path() + '/benchmarks'
	try:
		os.listdir(b_path)
	except:
		os.makedirs(b_path);
	
	return b_path

def create_camera(document):
	camera_view = k3d.plugin.create("ViewMatrix", document)
	camera_view.position = k3d.translate3(k3d.vector3(-10, -15, 10))
	camera_view.look = k3d.identity3()
	camera_view.up = k3d.translate3(k3d.vector3(0, 0, 20))
	
	camera = k3d.plugin.create("Camera", document)
	document.set_dependency(camera.get_property("input_matrix"), camera_view.get_property("output_matrix"))

	return camera

def create_opengl_engine(document):
	render_engine = k3d.plugin.create("OpenGLEngine", document)
	render_engine.draw_aimpoint = False
	render_engine.draw_crop_window = False
	render_engine.draw_frustum = False

	return render_engine

#####################################################################################33
# Bitmap-related testing

def setup_bitmap_test(nodes):
	if len(nodes) < 1:
		raise Exception("Bitmap test requires at least one node.")

	class result_object:
		pass

	result = result_object
	result.document = k3d.new_document()
	result.nodes = [] 

	for node in nodes:
		result.nodes.append(k3d.plugin.create(node, result.document))
		if len(result.nodes) > 1:
			result.document.set_dependency(result.nodes[-1].get_property("input_bitmap"), result.nodes[-2].get_property("output_bitmap"))

	result.source = result.nodes[0]

	if len(nodes) == 3:
		result.modifier = result.nodes[1]

	if len(nodes) > 1:
		result.sink = result.nodes[-1]
	
	return result

def setup_bitmap_source_test(source_name):
	result = setup_bitmap_test([source_name])
	return result

def setup_bitmap_reader_test(reader_name, source_file):
	result = setup_bitmap_test([reader_name])
	result.reader = result.source
	result.reader.file = k3d.filesystem.generic_path(source_path() + "/bitmaps/" + source_file)
	return result

def setup_bitmap_modifier_test(source_name, modifier_name):
	result = setup_bitmap_test([source_name, modifier_name])
	result.modifier = result.sink
	return result

def require_bitmap_size(bitmap, width, height):
	print """<DartMeasurement name="Bitmap Width" type="numeric/float">""" + str(bitmap.width()) + """</DartMeasurement>"""
	print """<DartMeasurement name="Bitmap Height" type="numeric/float">""" + str(bitmap.height()) + """</DartMeasurement>"""
	print """<DartMeasurement name="Target Width" type="numeric/float">""" + str(width) + """</DartMeasurement>"""
	print """<DartMeasurement name="Target Height" type="numeric/float">""" + str(height) + """</DartMeasurement>"""
	sys.stdout.flush()
	
	if bitmap.width() != width or bitmap.height() != height:
		raise "bitmap dimensions incorrect"

def require_similar_bitmap(document, image_property, image_name, threshold):
	output_file = k3d.filesystem.generic_path(binary_path() + "/bitmaps/" + image_name + ".reference.png")
	reference_file = k3d.filesystem.generic_path(source_path() + "/bitmaps/" + image_name + ".reference.png")
	difference_file = k3d.filesystem.generic_path(binary_path() + "/bitmaps/differences/" + image_name + ".png")

	reference = k3d.plugin.create("PNGBitmapReader", document)
	reference.file = reference_file
	reference_property = reference.get_property("output_bitmap")

	difference = k3d.plugin.create("BitmapPerceptualDifference", document)
	difference.field_of_view = 10.0
	difference.luminance = 100
	difference_property = difference.get_property("output_bitmap")
	document.set_dependency(difference.get_property("input_a"), image_property)
	document.set_dependency(difference.get_property("input_b"), reference_property)

	image_writer = k3d.plugin.create("PNGBitmapWriter", document)
	image_writer.file = output_file
	document.set_dependency(image_writer.get_property("input_bitmap"), image_property)

	difference_writer = k3d.plugin.create("PNGBitmapWriter", document)
	difference_writer.file= difference_file
	document.set_dependency(difference_writer.get_property("input_bitmap"), difference_property)

	pixel_count = image_property.pipeline_value().width() * image_property.pipeline_value().height()
	pixel_difference = difference.difference
	difference_measurement = float(pixel_difference) / float(pixel_count)

	print """<DartMeasurement name="Source Width" type="numeric/float">""" + str(image_property.pipeline_value().width()) + """</DartMeasurement>"""
	print """<DartMeasurement name="Source Height" type="numeric/float">""" + str(image_property.pipeline_value().height()) + """</DartMeasurement>"""
	print """<DartMeasurement name="Reference Width" type="numeric/float">""" + str(reference_property.pipeline_value().width()) + """</DartMeasurement>"""
	print """<DartMeasurement name="Reference Height" type="numeric/float">""" + str(reference_property.pipeline_value().height()) + """</DartMeasurement>"""
	print """<DartMeasurement name="Difference Width" type="numeric/float">""" + str(difference_property.pipeline_value().width()) + """</DartMeasurement>"""
	print """<DartMeasurement name="Difference Height" type="numeric/float">""" + str(difference_property.pipeline_value().height()) + """</DartMeasurement>"""
	print """<DartMeasurement name="Pixel Difference" type="numeric/float">""" + str(pixel_difference) + """</DartMeasurement>"""
	print """<DartMeasurement name="Pixel Count" type="numeric/float">""" + str(pixel_count) + """</DartMeasurement>"""
	print """<DartMeasurement name="Difference" type="numeric/float">""" + str(difference_measurement) + """</DartMeasurement>"""
	print """<DartMeasurement name="Threshold" type="numeric/float">""" + str(threshold) + """</DartMeasurement>"""
	print """<DartMeasurementFile name="Output Image" type="image/png">""" + str(output_file) + """</DartMeasurementFile>"""
	print """<DartMeasurementFile name="Reference Image" type="image/png">""" + str(reference_file) + """</DartMeasurementFile>"""
	print """<DartMeasurementFile name="Difference Image" type="image/png">""" + str(difference_file) + """</DartMeasurementFile>"""
	sys.stdout.flush()

	if not os.path.exists(str(reference_file)):
		raise Exception("missing reference file: " + str(reference_file))

	if difference_measurement > threshold:
		raise Exception("pixel difference exceeds threshold")

#####################################################################################33
# Mesh-related testing

def setup_mesh_test(nodes):
	if len(nodes) < 1:
		raise Exception("Mesh test requires at least one node.")

	class result_object:
		pass

	result = result_object
	result.document = k3d.new_document()
	result.nodes = [] 

	for node in nodes:
		result.nodes.append(k3d.plugin.create(node, result.document))
		if len(result.nodes) > 1:
			result.document.set_dependency(result.nodes[-1].get_property("input_mesh"), result.nodes[-2].get_property("output_mesh"))

	result.source = result.nodes[0]

	if len(nodes) == 3:
		result.modifier = result.nodes[1]

	if len(nodes) > 1:
		result.sink = result.nodes[-1]
	
	return result

def setup_mesh_source_test(source_name):
	result = setup_mesh_test([source_name])
	return result

def setup_mesh_reader_test(reader_name, source_file):
	result = setup_mesh_test([reader_name])
	result.reader = result.source
	result.reader.file = k3d.filesystem.generic_path(source_path() + "/meshes/" + source_file)
	result.reader.center = False
	result.reader.scale_to_size = False
	return result

def setup_mesh_modifier_test(source_name, modifier_name):
	class result_object:
		pass

	result = result_object
	result.document = k3d.new_document()
	result.source = k3d.plugin.create(source_name, result.document)
	result.add_index_attributes = k3d.plugin.create("AddIndexAttributes", result.document)
	result.modifier = k3d.plugin.create(modifier_name, result.document)
	result.document.set_dependency(result.modifier.get_property("input_mesh"), result.add_index_attributes.get_property("output_mesh"))
	result.document.set_dependency(result.add_index_attributes.get_property("input_mesh"), result.source.get_property("output_mesh"))

	return result

def setup_mesh_modifier_test2(source_name, modifier1_name, modifier2_name):
	class result_object:
		pass

	result = result_object
	result.document = k3d.new_document()
	result.source = k3d.plugin.create(source_name, result.document)
	result.add_index_attributes = k3d.plugin.create("AddIndexAttributes", result.document)
	result.modifier1 = k3d.plugin.create(modifier1_name, result.document)
	result.modifier2 = k3d.plugin.create(modifier2_name, result.document)
	result.document.set_dependency(result.modifier2.get_property("input_mesh"), result.modifier1.get_property("output_mesh"))
	result.document.set_dependency(result.modifier1.get_property("input_mesh"), result.add_index_attributes.get_property("output_mesh"))
	result.document.set_dependency(result.add_index_attributes.get_property("input_mesh"), result.source.get_property("output_mesh"))

	return result

def setup_mesh_writer_test(nodes, reader, target_file):
	if len(nodes) < 2:
		raise Exception("Mesh writer test requires at least two nodes.")

	result = setup_mesh_test(nodes)
	result.writer = result.sink
	result.writer.file = k3d.filesystem.generic_path(binary_path() + "/meshes/" + target_file)
	result.reader = k3d.plugin.create(reader, result.document)
	result.reader.file = k3d.filesystem.generic_path(binary_path() + "/meshes/" + target_file)
	result.reader.center = False
	result.reader.scale_to_size = False

	return result

def require_valid_mesh(document, input_mesh):
	primitives = k3d.plugin.create("ValidMeshes", document)
	primitives.create_property("k3d::mesh*", "input_mesh", "Input Mesh", "First input mesh")
	document.set_dependency(primitives.get_property("input_mesh"), input_mesh)
	if not primitives.valid:
		raise Exception("invalid or unknown primitive type in mesh")

def require_mesh_area(calculated_area, expected_area):
	print """<DartMeasurement name="Calculated Area" type="numeric/float">""" + str(calculated_area) + """</DartMeasurement>"""
	print """<DartMeasurement name="Expected Area" type="numeric/float">""" + str(expected_area) + """</DartMeasurement>"""
	sys.stdout.flush()

	if calculated_area != expected_area:
		raise Exception("incorrect mesh area")

def require_mesh_volume(calculated_volume, expected_volume):
	print """<DartMeasurement name="Calculated Volume" type="numeric/float">""" + str(calculated_volume) + """</DartMeasurement>"""
	print """<DartMeasurement name="Expected Volume" type="numeric/float">""" + str(expected_volume) + """</DartMeasurement>"""
	sys.stdout.flush()

	if calculated_volume != expected_volume:
		raise Exception("incorrect mesh volume")

def require_similar_mesh(document, input_mesh, base_mesh_name, threshold, platform = platform_agnostic):
	mesh_name = base_mesh_name + ".reference" + platform()

	output_path = k3d.filesystem.generic_path(binary_path() + "/meshes/" + mesh_name + ".k3d")
	reference_path = k3d.filesystem.generic_path(source_path() + "/meshes/" + mesh_name + ".k3d")
	difference_path = k3d.filesystem.generic_path(binary_path() + "/meshes/differences/" + mesh_name + ".html")

	output = k3d.plugin.create("K3DMeshWriter", document)
	output.file = output_path
	document.set_dependency(output.get_property("input_mesh"), input_mesh)

	reference = k3d.plugin.create("K3DMeshReader", document)
	reference.center = False
	reference.scale_to_size = False
	reference.material = document.get_node("Material")
	reference.file = reference_path

	if not os.path.exists(str(reference_path)):
		raise Exception("missing reference file: " + str(reference_path))

	difference = get_mesh_difference(document, input_mesh, reference.get_property("output_mesh"), threshold)

	print """<DartMeasurement name="Mesh Equal" type="numeric/integer">""" + str(difference.equal) + """</DartMeasurement>"""
	print """<DartMeasurement name="Mesh Difference" type="numeric/integer">""" + str(difference.difference) + """</DartMeasurement>"""

	if not difference.get_property("input_a").pipeline_value() or not difference.get_property("input_b").pipeline_value():
		raise Exception("missing mesh comparison input")

	if difference.equal:
		return
	
	# if there is a difference, output it
	difference_file = open(str(difference_path), "w")
	difference_file.write(difflib.HtmlDiff().make_file(str(input_mesh.pipeline_value()).splitlines(1), str(reference.output_mesh).splitlines(1), "Test Geometry", "Reference Geometry"))
	difference_file.close()

	output_mesh_difference(input_mesh.pipeline_value(), reference.output_mesh, threshold)
	raise Exception("output mesh differs from reference")

"""
	setup a difference node between two meshes
"""
def get_mesh_difference(document, input_mesh, reference_mesh, threshold):
	difference = k3d.plugin.create("MeshDiff", document)
	difference.threshold = threshold
	difference.create_property("k3d::mesh*", "input_a", "InputA", "First input mesh")
	difference.create_property("k3d::mesh*", "input_b", "InputB", "Second input mesh")
	document.set_dependency(difference.get_property("input_a"), input_mesh)
	document.set_dependency(difference.get_property("input_b"), reference_mesh)
	
	return difference

"""
	Output the mesh difference to the dashboard
"""
def output_mesh_difference(input_mesh, reference_mesh, threshold, name = "Mesh Difference"):
	print """<DartMeasurement name="Geometry Difference" type="text/html"><![CDATA[\n"""
	print difflib.HtmlDiff().make_file(str(input_mesh).splitlines(1), str(reference_mesh).splitlines(1), "Test Geometry", "Reference Geometry")
	print """]]></DartMeasurement>\n"""
	sys.stdout.flush()

#####################################################################################33
# Scalar-related testing

def setup_scalar_source_test(source_name):
	class result_object:
		pass

	result = result_object
	result.document = k3d.new_document()
	result.source = k3d.plugin.create(source_name, result.document)

	return result

def require_scalar_value(value, expected_value):
	print """<DartMeasurement name="Value" type="numeric/float">""" + str(value) + """</DartMeasurement>"""
	print """<DartMeasurement name="Expected Value" type="numeric/float">""" + str(expected_value) + """</DartMeasurement>"""
	if value != expected_value:
		raise Exception("value does not match expected value")


#####################################################################################33
# Deprecated stuff

def bitmap_perceptual_difference(document, input_image1, input_image2, threshold=1e-8):
	difference = k3d.plugin.create("BitmapPerceptualDifference", document)
	difference.field_of_view = 10.0
	difference.luminance = 100
	document.set_dependency(difference.get_property("input_a"), input_image1)
	document.set_dependency(difference.get_property("input_b"), input_image2)
	
	pixel_count = input_image1.internal_value().width() * input_image1.internal_value().height()
	pixel_difference = difference.difference
	difference_measurement = float(pixel_difference) / float(pixel_count)
	
	print """<DartMeasurement name="Pixel Difference" type="numeric/float">""" + str(pixel_difference) + """</DartMeasurement>"""
	print """<DartMeasurement name="Pixel Count" type="numeric/float">""" + str(pixel_count) + """</DartMeasurement>"""
	print """<DartMeasurement name="Difference" type="numeric/float">""" + str(difference_measurement) + """</DartMeasurement>"""
	print """<DartMeasurement name="Threshold" type="numeric/float">""" + str(threshold) + """</DartMeasurement>"""
	sys.stdout.flush()

	if difference_measurement > threshold:
		raise "pixel difference exceeds threshold"
   
# compare the results of two different bitmap plugins applied to the k-3d test image
def bitmap_compare_plugin_outputs(referenceName, pluginToTest, pluginPropertyValues = {}):
	# load the reference image
	inputBitmap = setup_bitmap_reader_test("BitmapReader", "test_rgb_8.bmp")
	document = inputBitmap.document
	
	referenceNode = k3d.plugin.create(referenceName, document)
	testNode = k3d.plugin.create(pluginToTest, document)
	
	for (property, value) in pluginPropertyValues.items():
		referenceNode.get_property(property).set_value(value)
		testNode.get_property(property).set_value(value)
	
	# set the dependency to the input bitmap so that the errors can be measured
	document.set_dependency(referenceNode.get_property("input_bitmap"), inputBitmap.reader.get_property("output_bitmap"))
	document.set_dependency(testNode.get_property("input_bitmap"), inputBitmap.reader.get_property("output_bitmap"))
	
	print """<DartMeasurement name="Description" type="text/string"> Compare """ + pluginToTest + " to " + referenceName + """</DartMeasurement>"""
	if len(pluginPropertyValues) > 0:
		print """<DartMeasurement name="Parameters" type="text/string"> """ + str(pluginPropertyValues) + """</DartMeasurement>"""

	# calculate the perceptual difference
	bitmap_perceptual_difference(document, referenceNode.get_property("output_bitmap"), testNode.get_property("output_bitmap"))

def bitmap_modifier_benchmark(BitmapModifier):
	sys.stdout.write("""<DartMeasurement name="Timing" type="text/html"><![CDATA[\n""")
	sys.stdout.write("""<table>\n""")

	sizes = [(640,480), (800,600), (1024,768), (1280,1024), (1600, 1200)]
	runs = 3

	first_row = True
	for size in sizes:
		for run in range(runs):
			document = k3d.new_document()

			profiler = k3d.plugin.create("PipelineProfiler", document)

			image = k3d.plugin.create("BitmapSolid", document)
			image.width = size[0];
			image.height = size[1];

			modifier = k3d.plugin.create(BitmapModifier, document)

			document.set_dependency(modifier.get_property("input_bitmap"), image.get_property("output_bitmap"))
			modifier.output_bitmap

			if first_row:
				first_row = False

				sys.stdout.write("""<tr>""")
				sys.stdout.write("""<th>Width</th>""")
				sys.stdout.write("""<th>Height</th>""")
				sys.stdout.write("""<th>Run</th>""")

				records = profiler.records
				for node in records:
					if node != modifier:
						continue

					measurements = records[node]
					for measurement in measurements:
						sys.stdout.write("""<th>""")
						sys.stdout.write(measurement)
						sys.stdout.write("""</th>""")

				sys.stdout.write("""</tr>\n""")

			sys.stdout.write("""<tr>""")
			sys.stdout.write("""<td>""" + str(size[0]) + """</td>""")
			sys.stdout.write("""<td>""" + str(size[1]) + """</td>""")
			sys.stdout.write("""<td>""" + str(run) + """</td>""")

			records = profiler.records
			for node in records:
				if node != modifier:
					continue

				measurements = records[node]
				for measurement in measurements:
					sys.stdout.write("""<td>""")
					sys.stdout.write(str(measurements[measurement]))
					sys.stdout.write("""</td>""")

			sys.stdout.write("""</tr>\n""")

			k3d.close_document(document)

	sys.stdout.write("""</table>\n""")
	sys.stdout.write("""]]></DartMeasurement>\n""")

