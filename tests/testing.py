#python

import k3d
import os
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
	
	camera_view = document.new_node("ViewMatrix")
	camera_view.position = k3d.translate3(k3d.vector3(-10, -15, 10))
	camera_view.look = k3d.identity3()
	camera_view.up = k3d.translate3(k3d.vector3(0, 0, 20))
	
	camera = document.new_node("Camera")
	document.set_dependency(camera.get_property("input_matrix"), camera_view.get_property("output_matrix"))

	return camera

def create_opengl_engine(document):
	
	render_engine = document.new_node("OpenGLEngine")
	render_engine.draw_aimpoint = False
	render_engine.draw_crop_window = False
	render_engine.draw_frustum = False

	return render_engine

def create_default_painter(document):
	
	use_vbo = False
	try:
		if (os.environ['USE_VBO']):
			use_vbo = True
	except KeyError:
		pass
	   
	if (not use_vbo):
		edge_painter = document.new_node("OpenGLEdgePainter")
		face_painter = document.new_node("OpenGLFacePainter")
	else:
		edge_painter = document.new_node("VBOEdgePainter")
		face_painter = document.new_node("VBOFacePainterFlatNormals")

		edge_painter.unselected_mesh_color = k3d.color(0, 0, 0)
		edge_painter.selected_mesh_color = k3d.color(1, 1, 1)
		edge_painter.selected_component_color = k3d.color(1, 0, 0)
		
		face_painter.unselected_mesh_color = k3d.color(0.8, 0.8, 0.8)
		face_painter.selected_mesh_color = k3d.color(0.8, 0.8, 0.8)
		face_painter.selected_component_color = k3d.color(1, 0, 0)
		
	nurbs_curve_painter = document.new_node("OpenGLNURBSCurvePainter")
	nurbs_patch_painter = document.new_node("OpenGLNURBSPatchPainter")

	painter = document.new_node("OpenGLMultiPainter")

	painter.create_property("k3d::gl::imesh_painter*", "edges", "Edges", "Edge Painter")
	painter.create_property("k3d::gl::imesh_painter*", "faces", "Faces", "Face Painter")
	painter.create_property("k3d::gl::imesh_painter*", "nurbs_curves", "NURBS Curves", "NURBS Curve Painter")
	painter.create_property("k3d::gl::imesh_painter*", "nurbs_patches", "NURBS Patches", "NURBS Patche Painter")
	
	painter.edges = edge_painter
	painter.faces = face_painter
	painter.nurbs_curves = nurbs_curve_painter
	painter.nurbs_patches = nurbs_patch_painter

	return painter

def setup_bitmap_reader_test(reader_name, source_file):
	doc = k3d.new_document()
	reader = doc.new_node(reader_name)
	reader.file = k3d.generic_path(source_path() + "/bitmaps/" + source_file)

	class result_object:
		pass

	result = result_object
	result.document = doc
	result.reader = reader

	return result

def setup_mesh_reader_test(reader_name, source_file):
	doc = k3d.new_document()
	reader = doc.new_node(reader_name)
	reader.file = k3d.generic_path(source_path() + "/meshes/" + source_file)

	class result_object:
		pass

	result = result_object
	result.document = doc
	result.reader = reader

	return result

def setup_mesh_source_test(source_name):
	doc = k3d.new_document()

	source = doc.new_node(source_name)

	class result_object:
		pass

	result = result_object
	result.document = doc
	result.source = source
	result.output_mesh = source.get_property("output_mesh").pipeline_value()

	return result

def setup_mesh_source_image_test(source_name):
	doc = k3d.new_document()

	axes = doc.new_node("Axes")
	axes.xyplane = False

	painter = create_default_painter(doc)

	source = doc.new_node(source_name)

	mesh_instance = doc.new_node("MeshInstance")
	mesh_instance.gl_painter = painter
	doc.set_dependency(mesh_instance.get_property("input_mesh"), source.get_property("output_mesh"))

	camera = create_camera(doc)
	render_engine = create_opengl_engine(doc)

	camera_to_bitmap = doc.new_node("VirtualCameraToBitmap")
	camera_to_bitmap.camera = camera
	camera_to_bitmap.render_engine = render_engine

	class result_object:
		pass

	result = result_object
	result.document = doc
	result.axes = axes
	result.painter = painter
	result.source = source
	result.mesh_instance = mesh_instance
	result.camera = camera
	result.render_engine = render_engine
	result.camera_to_bitmap = camera_to_bitmap

	return result

def setup_mesh_modifier_test(source_name, modifier_name):
	doc = k3d.new_document()

	source = doc.new_node(source_name)

	modifier = doc.new_node(modifier_name)
	doc.set_dependency(modifier.get_property("input_mesh"), source.get_property("output_mesh"))

	class result_object:
		pass

	result = result_object
	result.document = doc
	result.source = source
	result.modifier = modifier

	return result

def setup_mesh_modifier_test2(source_name, modifier1_name, modifier2_name):
	doc = k3d.new_document()

	source = doc.new_node(source_name)

	modifier1 = doc.new_node(modifier1_name)
	doc.set_dependency(modifier1.get_property("input_mesh"), source.get_property("output_mesh"))

	modifier2 = doc.new_node(modifier2_name)
	doc.set_dependency(modifier2.get_property("input_mesh"), modifier1.get_property("output_mesh"))

	class result_object:
		pass

	result = result_object
	result.document = doc
	result.source = source
	result.modifier1 = modifier1
	result.modifier2 = modifier2

	return result

def setup_mesh_modifier_image_test(source_name, modifier_name):
	doc = k3d.new_document()

	axes = doc.new_node("Axes")
	axes.xyplane = False

	painter = create_default_painter(doc)

	source = doc.new_node(source_name)

	modifier = doc.new_node(modifier_name)
	doc.set_dependency(modifier.get_property("input_mesh"), source.get_property("output_mesh"))

	mesh_instance = doc.new_node("MeshInstance")
	mesh_instance.gl_painter = painter
	doc.set_dependency(mesh_instance.get_property("input_mesh"), modifier.get_property("output_mesh"))

	camera = create_camera(doc)
	render_engine = create_opengl_engine(doc)

	camera_to_bitmap = doc.new_node("CameraToBitmap")
	camera_to_bitmap.camera = camera
	camera_to_bitmap.render_engine = render_engine

	class result_object:
		pass

	result = result_object
	result.document = doc
	result.axes = axes
	result.painter = painter
	result.source = source
	result.modifier = modifier
	result.mesh_instance = mesh_instance
	result.camera = camera
	result.render_engine = render_engine
	result.camera_to_bitmap = camera_to_bitmap

	return result

def setup_scalar_source_test(source_name):
	doc = k3d.new_document()

	source = doc.new_node(source_name)

	class result_object:
		pass

	result = result_object
	result.document = doc
	result.source = source

	return result

def bitmap_size_comparison(bitmap, width, height):

	print """<DartMeasurement name="Bitmap Width" type="numeric/float">""" + str(bitmap.width()) + """</DartMeasurement>"""
	print """<DartMeasurement name="Bitmap Height" type="numeric/float">""" + str(bitmap.height()) + """</DartMeasurement>"""
	print """<DartMeasurement name="Target Width" type="numeric/float">""" + str(width) + """</DartMeasurement>"""
	print """<DartMeasurement name="Target Height" type="numeric/float">""" + str(height) + """</DartMeasurement>"""
	sys.stdout.flush()
	
	if bitmap.width() != width or bitmap.height() != height:
		raise "bitmap dimensions incorrect"

def mesh_comparison_to_reference(document, input_mesh, reference_mesh_name, threshold):

	mesh_writer = document.new_node("K3DMeshWriter")
	mesh_writer.file = k3d.generic_path(binary_path() + "/" + reference_mesh_name + ".output.k3d")
	document.set_dependency(mesh_writer.get_property("input_mesh"), input_mesh)

	reference = document.new_node("K3DMeshReader")

	difference = get_mesh_difference(document, input_mesh, reference.get_property("output_mesh"), threshold)

	for index in range(1, 100):
		reference_file = k3d.generic_path(source_path() + "/meshes/" + reference_mesh_name + ".reference." + str(index) + ".k3d")

		if not os.path.exists(str(reference_file)):
			if index == 1:
				raise Exception("missing reference file: " + str(reference_file))
			else:
				break

		reference.file = reference_file

		if not difference.get_property("input_a").pipeline_value() or not difference.get_property("input_b").pipeline_value():
			raise Exception("missing mesh comparison input")

		if difference.equal:
			return
	
	# if there is a difference, output it
	output_mesh_difference(input_mesh.pipeline_value(), reference.output_mesh, threshold)
	raise Exception("output mesh differs from reference")

"""
	setup a difference node between two meshes
"""
def get_mesh_difference(document, input_mesh, reference_mesh, threshold):
	difference = document.new_node("MeshDiff")
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
	print """<DartMeasurement name="Mesh Difference" type="text/html"><![CDATA[\n"""
	print """<pre>"""
	print k3d.print_diff(input_mesh, reference_mesh, threshold)
	print """</pre>"""
	print """]]></DartMeasurement>\n"""
	sys.stdout.flush()

def mesh_area_comparison(calculated_area, expected_area):
	if calculated_area != expected_area:
		print """<DartMeasurement name="Calculated Area" type="numeric/float">""" + str(calculated_area) + """</DartMeasurement>"""
		print """<DartMeasurement name="Expected Area" type="numeric/float">""" + str(expected_area) + """</DartMeasurement>"""
		raise Exception("incorrect mesh area")

def mesh_volume_comparison(calculated_volume, expected_volume):
	if calculated_volume != expected_volume:
		print """<DartMeasurement name="Calculated Volume" type="numeric/float">""" + str(calculated_volume) + """</DartMeasurement>"""
		print """<DartMeasurement name="Expected Volume" type="numeric/float">""" + str(expected_volume) + """</DartMeasurement>"""
		raise Exception("incorrect mesh volume")

def assert_valid_mesh(mesh):
	if not k3d.validate(mesh):
		raise Exception("output mesh is not valid")

def assert_solid_mesh(mesh):
	if not k3d.is_solid(mesh):
		raise Exception("output mesh is not solid")

def bitmap_perceptual_difference(document, input_image1, input_image2, threshold=1e-8):
	difference = document.new_node("BitmapPerceptualDifference")
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
	
	referenceNode = document.new_node(referenceName)
	testNode = document.new_node(pluginToTest)
	
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

def image_comparison(document, image, image_name, threshold):

	output_file = k3d.generic_path(binary_path() + "/" + image_name + ".output.png")
	reference_file = k3d.generic_path(source_path() + "/bitmaps/" + image_name + ".reference.png")
	difference_file = k3d.generic_path(binary_path() + "/" + image_name + ".difference.png")

	reference = document.new_node("PNGBitmapReader")
	reference.file = reference_file

	difference = document.new_node("BitmapPerceptualDifference")
	difference.field_of_view = 10.0
	difference.luminance = 100
	document.set_dependency(difference.get_property("input_a"), image)
	document.set_dependency(difference.get_property("input_b"), reference.get_property("output_bitmap"))

	image_writer = document.new_node("PNGBitmapWriter")
	image_writer.file = output_file
	document.set_dependency(image_writer.get_property("input_bitmap"), image)

	difference_writer = document.new_node("PNGBitmapWriter")
	difference_writer.file= difference_file
	document.set_dependency(difference_writer.get_property("input_bitmap"), difference.get_property("output"))

	pixel_count = image.internal_value().width() * image.internal_value().height()
	pixel_difference = difference.difference
	difference_measurement = float(pixel_difference) / float(pixel_count)

	print """<DartMeasurement name="Pixel Difference" type="numeric/float">""" + str(pixel_difference) + """</DartMeasurement>"""
	print """<DartMeasurement name="Pixel Count" type="numeric/float">""" + str(pixel_count) + """</DartMeasurement>"""
	print """<DartMeasurement name="Difference" type="numeric/float">""" + str(difference_measurement) + """</DartMeasurement>"""
	print """<DartMeasurement name="Threshold" type="numeric/float">""" + str(threshold) + """</DartMeasurement>"""
	print """<DartMeasurementFile name="Output Image" type="image/png">""" + str(output_file) + """</DartMeasurementFile>"""
	print """<DartMeasurementFile name="Reference Image" type="image/png">""" + str(reference_file) + """</DartMeasurementFile>"""
	print """<DartMeasurementFile name="Difference Image" type="image/png">""" + str(difference_file) + """</DartMeasurementFile>"""
	sys.stdout.flush()

	if difference_measurement > threshold:
		raise "pixel difference exceeds threshold"

def scalar_comparison(value, expected_value):
	print """<DartMeasurement name="Value" type="numeric/float">""" + str(value) + """</DartMeasurement>"""
	print """<DartMeasurement name="Expected Value" type="numeric/float">""" + str(expected_value) + """</DartMeasurement>"""
	if value != expected_value:
		raise Exception("value does not match expected value")
