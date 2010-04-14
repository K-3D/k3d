#python

import k3d
import difflib
import os
import platform
import sys
import time

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
	k3d.property.connect(document, camera_view.get_property("output_matrix"), camera.get_property("input_matrix"))

	return camera

def create_opengl_engine(document):
	render_engine = k3d.plugin.create("OpenGLEngine", document)
	render_engine.draw_aimpoint = False
	render_engine.draw_crop_window = False
	render_engine.draw_frustum = False

	return render_engine

def dart_measurement(name, value):
	if type(value).__name__ == "str":
		value_type = "text/string"
	elif type(value).__name__ == "bool":
		value_type = "text/string"
	elif type(value).__name__ == "int":
		value_type = "numeric/integer"
	elif type(value).__name__ == "long":
		value_type = "numeric/integer"
	elif type(value).__name__ == "float":
		value_type = "numeric/float"
	else:
		raise Exception("Unknown type: " + repr(value))

	print "<DartMeasurement name=\"" + name + "\" type=\"" + value_type + "\">" + str(value) + "</DartMeasurement>"
	sys.stdout.flush()

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
			k3d.property.connect(result.document, result.nodes[-2].get_property("output_bitmap"), result.nodes[-1].get_property("input_bitmap"))

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
	dart_measurement("bitmap_width", bitmap.width())
	dart_measurement("bitmap_height", bitmap.height())
	dart_measurement("expected_width", width)
	dart_measurement("expected_height", height)
	
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
	k3d.property.connect(document, image_property, difference.get_property("input_a"))
	k3d.property.connect(document, reference_property, difference.get_property("input_b"))

	image_writer = k3d.plugin.create("PNGBitmapWriter", document)
	image_writer.file = output_file
	k3d.property.connect(document, image_property, image_writer.get_property("input_bitmap"))

	difference_writer = k3d.plugin.create("PNGBitmapWriter", document)
	difference_writer.file= difference_file
	k3d.property.connect(document, difference_property, difference_writer.get_property("input_bitmap"))

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
			k3d.property.connect(result.document, result.nodes[-2].get_property("output_mesh"), result.nodes[-1].get_property("input_mesh"))

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
	k3d.property.connect(result.document, result.source.get_property("output_mesh"), result.add_index_attributes.get_property("input_mesh"))
	k3d.property.connect(result.document, result.add_index_attributes.get_property("output_mesh"), result.modifier.get_property("input_mesh"))

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
	k3d.property.connect(result.document, result.source.get_property("output_mesh"), result.add_index_attributes.get_property("input_mesh"))
	k3d.property.connect(result.document, result.add_index_attributes.get_property("output_mesh"), result.modifier1.get_property("input_mesh"))
	k3d.property.connect(result.document, result.modifier1.get_property("output_mesh"), result.modifier2.get_property("input_mesh"))

	return result

def add_point_attributes_test(setup, points, vertices, parameters):
	setup.add_point_attributes = k3d.plugin.create("AddPointAttributes", setup.document)
	setup.add_point_attributes.add_point_attributes = points
	setup.add_point_attributes.add_vertex_attributes = vertices
	setup.add_point_attributes.add_parameter_attributes = parameters
	k3d.property.connect(setup.document, setup.add_index_attributes.get_property("output_mesh"), setup.add_point_attributes.get_property("input_mesh"))
	k3d.property.connect(setup.document, setup.add_point_attributes.get_property("output_mesh"), setup.modifier.get_property("input_mesh"))

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
	k3d.property.create(primitives, "k3d::mesh*", "input_mesh", "Input Mesh", "First input mesh")
	k3d.property.connect(document, input_mesh, primitives.get_property("input_mesh"))
	if not primitives.valid:
		raise Exception("invalid or unknown primitive type in mesh")

def require_mesh_area(calculated_area, expected_area):
	dart_measurement("calculated_area", calculated_area)
	dart_measurement("expected_area", expected_area)

	if calculated_area != expected_area:
		raise Exception("incorrect mesh area")

def require_mesh_volume(calculated_volume, expected_volume):
	dart_measurement("calculated_volume", calculated_volume)
	dart_measurement("expected_volume", expected_volume)

	if calculated_volume != expected_volume:
		raise Exception("incorrect mesh volume")

def require_similar_mesh(document, input_mesh, base_mesh_name, ulps_threshold, custom_platforms = []):
	mesh_name = base_mesh_name + ".reference"

	platform_id = str(platform.system()) + "-" + str(platform.machine())
	for custom_platform in custom_platforms:
		if custom_platform == platform_id:
			mesh_name = mesh_name + "." + platform_id
			break

	output_path = k3d.filesystem.generic_path(binary_path() + "/meshes/" + mesh_name + ".k3d")
	reference_path = k3d.filesystem.generic_path(source_path() + "/meshes/" + mesh_name + ".k3d")
	difference_path = k3d.filesystem.generic_path(binary_path() + "/meshes/differences/" + mesh_name + ".html")

	output = k3d.plugin.create("K3DMeshWriter", document)
	output.file = output_path
	k3d.property.connect(document, input_mesh, output.get_property("input_mesh"))

	reference = k3d.plugin.create("K3DMeshReader", document)
	reference.center = False
	reference.scale_to_size = False
	reference.material = k3d.node.lookup_one(document, "Material")
	reference.file = reference_path

	if not os.path.exists(str(reference_path)):
		raise Exception("missing reference file: " + str(reference_path))

	result = k3d.difference.accumulator()
	k3d.difference.test(input_mesh.internal_value(), reference.output_mesh, result)

	dart_measurement("exact_count", result.exact_count())
	dart_measurement("exact_min", result.exact_min())
	dart_measurement("exact_max", result.exact_max())

	dart_measurement("ulps_count", result.ulps_count())
	dart_measurement("ulps_min", result.ulps_min())
	dart_measurement("ulps_max", result.ulps_max())
	dart_measurement("ulps_mean", result.ulps_mean())
	dart_measurement("ulps_median", result.ulps_median())
	dart_measurement("ulps_standard_deviation", result.ulps_standard_deviation())
	dart_measurement("ulps_variance", result.ulps_variance())

	dart_measurement("ulps_threshold", ulps_threshold)

	if (result.exact_count() and result.exact_min() == 0) or result.ulps_max() > ulps_threshold:
		print """<DartMeasurement name="geometry_difference" type="text/html"><![CDATA[\n"""
		print difflib.HtmlDiff().make_file(str(input_mesh.internal_value()).splitlines(1), str(reference.output_mesh).splitlines(1), "Test Geometry", "Reference Geometry")
		print """]]></DartMeasurement>\n"""
		sys.stdout.flush()

		raise Exception("output mesh differs from reference")

def points_to_string(points):
	result = ''
	for point in points:
		result += str(point) + '\n'
	return result

def print_difference(accumulator, prefix=""):
	dart_measurement(prefix+"exact_count", accumulator.exact_count())
	dart_measurement(prefix+"exact_min", accumulator.exact_min())
	dart_measurement(prefix+"exact_max", accumulator.exact_max())

	dart_measurement(prefix+"ulps_count", accumulator.ulps_count())
	dart_measurement(prefix+"ulps_min", accumulator.ulps_min())
	dart_measurement(prefix+"ulps_max", accumulator.ulps_max())
	dart_measurement(prefix+"ulps_mean", accumulator.ulps_mean())
	dart_measurement(prefix+"ulps_median", accumulator.ulps_median())
	dart_measurement(prefix+"ulps_standard_deviation", accumulator.ulps_standard_deviation())
	dart_measurement(prefix+"ulps_variance", accumulator.ulps_variance())


def require_valid_point_attributes(document, input_mesh, ulps_threshold = 10):
	point_attributes = input_mesh.point_attributes()["points"]
	
	result = k3d.difference.accumulator()
	k3d.difference.test(input_mesh.points(), point_attributes, result)

	print_difference(result, "point_attributes_")

	if (result.exact_count() and result.exact_min() == 0) or result.ulps_max() > ulps_threshold:
		print """<DartMeasurement name="geometry_difference" type="text/html"><![CDATA[\n"""
		print difflib.HtmlDiff().make_file(points_to_string(point_attributes).splitlines(1), points_to_string(input_mesh.points()).splitlines(1), "Test Points", "Reference Points")
		print """]]></DartMeasurement>\n"""
		sys.stdout.flush()
		raise Exception("Point attributes don't match points array")

def require_valid_vertex_attributes(document, input_mesh, ulps_threshold = 10):
	points = input_mesh.points()
	for (i, prim) in zip(range(len(input_mesh.primitives())), input_mesh.primitives()):
		struc = prim.structure()
		refpoints = []
		if struc.keys().count('vertex'):
			vertex_table = struc['vertex']
			for array in vertex_table:
				if array.get_metadata_value('k3d:domain') == 'k3d:point-indices':
					for idx in array:
						refpoints.append(points[idx])
		if len(refpoints):
			attribs = prim.attributes()
			if attribs.keys().count('vertex'):
				vertex_attribs = attribs['vertex']
				if vertex_attribs.keys().count('points'):
					vertex_point_attribs = vertex_attribs['points']
					result = k3d.difference.accumulator()
					k3d.difference.test(vertex_point_attribs, refpoints, result)
					print_difference(result, "vertex_attributes_")
					if (result.exact_count() and result.exact_min() == 0) or result.ulps_max() > ulps_threshold:
						print """<DartMeasurement name="geometry_difference" type="text/html"><![CDATA[\n"""
						print difflib.HtmlDiff().make_file(points_to_string(vertex_point_attribs).splitlines(1), points_to_string(refpoints).splitlines(1), "Test Points", "Reference Points")
						print """]]></DartMeasurement>\n"""
						sys.stdout.flush()
						raise Exception("Vertex point attributes don't match points array for primitive {0}".format(i))

def require_valid_parameter_attributes(document, input_mesh, ulps_threshold = 10):
	points = input_mesh.points()
	for (i, prim) in zip(range(len(input_mesh.primitives())), input_mesh.primitives()):
		curve_prim = k3d.nurbs_curve.validate(input_mesh, prim)
		if curve_prim:
			refpoints = []
			for (first_point, point_count) in zip(curve_prim.curve_first_points(), curve_prim.curve_point_counts()):
				last_point = first_point + point_count - 1
				refpoints.append(points[first_point])
				refpoints.append(points[last_point])
			if len(refpoints):
				if curve_prim.parameter_attributes().keys().count('points'):
					testpoints = curve_prim.parameter_attributes()['points']
					result = k3d.difference.accumulator()
					k3d.difference.test(testpoints, refpoints, result)
					print_difference(result, "parameter_attributes_")
					if (result.exact_count() and result.exact_min() == 0) or result.ulps_max() > ulps_threshold:
						print """<DartMeasurement name="geometry_difference" type="text/html"><![CDATA[\n"""
						print difflib.HtmlDiff().make_file(points_to_string(testpoints).splitlines(1), points_to_string(refpoints).splitlines(1), "Test Points", "Reference Points")
						print """]]></DartMeasurement>\n"""
						sys.stdout.flush()
						raise Exception("Parameter point attributes don't match points array for primitive {0}".format(i))

		patch_prim = k3d.nurbs_patch.validate(input_mesh, prim)
		if patch_prim:
			refpoints = []
			for (c1, u_count, v_count) in zip(patch_prim.patch_first_points(), patch_prim.patch_u_point_counts(), patch_prim.patch_v_point_counts()):
				c2 = c1 + u_count-1;
				c3 = c1 + u_count*(v_count-1);
				c4 = c3 + u_count - 1;
				refpoints.append(points[c1]);
				refpoints.append(points[c2]);
				refpoints.append(points[c3]);
				refpoints.append(points[c4]);
			if len(refpoints):
				if patch_prim.parameter_attributes().keys().count('points'):
					testpoints = patch_prim.parameter_attributes()['points']
					result = k3d.difference.accumulator()
					k3d.difference.test(testpoints, refpoints, result)
					print_difference(result, "parameter_attributes_")
					if (result.exact_count() and result.exact_min() == 0) or result.ulps_max() > ulps_threshold:
						print """<DartMeasurement name="geometry_difference" type="text/html"><![CDATA[\n"""
						print difflib.HtmlDiff().make_file(points_to_string(testpoints).splitlines(1), points_to_string(refpoints).splitlines(1), "Test Points", "Reference Points")
						print """]]></DartMeasurement>\n"""
						sys.stdout.flush()
						raise Exception("Parameter point attributes don't match points array for primitive {0}".format(i))

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
	dart_measurement("value", value);
	dart_measurement("expected_value", expected_value);
	if value != expected_value:
		raise Exception("value does not match expected value")

#####################################################################################33
# Shader-related testing

def surface_shader_test(render_engine_name, shader_name):
	class result_object:
		def render(self):
			self.render_engine.render_camera_frame(self.camera, self.output, False)

	result = result_object
	result.document = k3d.open_document(k3d.filesystem.generic_path(source_path() + "/shaders/surface_shader_test.k3d"))
	result.camera = k3d.node.lookup_one(result.document, "Camera")
	result.render_engine = k3d.node.lookup_one(result.document, "RenderMan Engine")
	result.material = k3d.node.lookup_one(result.document, "RenderManMaterial")
	result.shader = k3d.node.lookup_one(result.document, "Surface Shader")

	result.shader.shader_path = k3d.share_path() / k3d.filesystem.generic_path("shaders/surface/" + shader_name + ".sl")
	result.output = k3d.filesystem.generic_path(binary_path() + "/shaders/surface/" + shader_name + "." + render_engine_name + ".tiff")
	result.render_engine.render_engine = k3d.node.lookup_one(result.document, render_engine_name)

	result.render_engine.render_camera_frame(result.camera, result.output, False)

	return result

def displacement_shader_test(render_engine_name, shader_name):
	class result_object:
		def render(self):
			self.render_engine.render_camera_frame(self.camera, self.output, False)

	result = result_object
	result.document = k3d.open_document(k3d.filesystem.generic_path(source_path() + "/shaders/displacement_shader_test.k3d"))
	result.camera = k3d.node.lookup_one(result.document, "Camera")
	result.render_engine = k3d.node.lookup_one(result.document, "RenderMan Engine")
	result.material = k3d.node.lookup_one(result.document, "RenderManMaterial")
	result.shader = k3d.node.lookup_one(result.document, "RenderManDisplacementShader")

	result.shader.shader_path = k3d.share_path() / k3d.filesystem.generic_path("shaders/displacement/" + shader_name + ".sl")
	result.output = k3d.filesystem.generic_path(binary_path() + "/shaders/displacement/" + shader_name + "." + render_engine_name + ".tiff")
	result.render_engine.render_engine = k3d.node.lookup_one(result.document, render_engine_name)

	result.render_engine.render_camera_frame(result.camera, result.output, False)

	return result

#####################################################################################33
# Deprecated stuff

def bitmap_perceptual_difference(document, input_image1, input_image2, threshold=1e-8):
	difference = k3d.plugin.create("BitmapPerceptualDifference", document)
	difference.field_of_view = 10.0
	difference.luminance = 100
	k3d.property.connect(document, input_image1, difference.get_property("input_a"))
	k3d.property.connect(document, input_image2, difference.get_property("input_b"))
	
	pixel_count = input_image1.internal_value().width() * input_image1.internal_value().height()
	pixel_difference = difference.difference
	difference_measurement = float(pixel_difference) / float(pixel_count)

	dart_measurement("pixel_difference", pixel_difference)
	dart_measurement("pixel_count", pixel_count)
	dart_measurement("difference", difference_measurement)
	dart_measurement("threshold", threshold)
	
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
	k3d.property.connect(document, inputBitmap.reader.get_property("output_bitmap"), referenceNode.get_property("input_bitmap"))
	k3d.property.connect(document, inputBitmap.reader.get_property("output_bitmap"), testNode.get_property("input_bitmap"))
	
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

			k3d.property.connect(document, image.get_property("output_bitmap"), modifier.get_property("input_bitmap"))
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

