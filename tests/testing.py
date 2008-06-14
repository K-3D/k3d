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

# this class is used to setup an input mesh for mesh benchmarks
class benchmarkMesh(object):
    def __init__(self, counts = (1,1,1)):
        self.__counts = counts
        
    def set_document(self, document):
        self.__document = document
        self.initialize_mesh()
        
    def initialize_mesh(self):
        self.__cube = self.__document.new_node("PolyCube")
        self.__cube.columns = 1;
        self.__cube.rows = 1;
        self.__cube.slices = 1;
        self.__cube.width = 1;
        self.__cube.height = 1;
        self.__cube.depth = 1;
        
        self.__mesh_array_3D = self.__document.new_node("MeshArray3D")
        self.__mesh_array_3D.count1 = self.__counts[0]
        self.__mesh_array_3D.count2 = self.__counts[1]
        self.__mesh_array_3D.count3 = self.__counts[2]
    
        self.__mesh_array_3D.layout = self.__document.new_node("TranslateArray3D")
    
        self.__document.set_dependency(self.__mesh_array_3D.get_property("input_mesh"), self.__cube.get_property("output_mesh"))
    
    def set_counts(self, counts):
        # counts is a tuple of values
        self.__counts = counts
        
    def get_mesh(self):
        return self.__mesh_array_3D
    
    def get_size_metric(self):
    	return self.__counts[0]*self.__counts[1]*self.__counts[2]*4

# class to handle profiler output for benchmarking
class k3dProfilingProcessor(object):
	def __init__(self):
		self.__results = {}
		self.__results['Total'] = []
		self.__nodeName = ''
	
	def number_of_results(self):
		try:
			return len(self.__results['Total'])
		except:
			return 0
	
	def number_of_columns(self):
		return len(self.__results)
		
	def clear_results(self):
		self.__init__()
		
	def add_profiler_results_for_node(self, nodeName, profilerRecords):
		self.__nodeName = nodeName
		for (node, timing) in profilerRecords.items():
			if node.name == self.__nodeName:
				total = 0.0
				for t in timing:
					total += timing[t]
					if t in self.__results:
						self.__results[t] += [timing[t],]
					else:
						self.__results[t] = [timing[t],]
				self.__results['Total'] += [total,]
				break
	
	# oputput the results as a table to display on the dashboard
	def output_as_dart_table(self, description = ''):
		if description:
			print '<DartMeasurement name="Description" type="text/string">' + description + '</DartMeasurement>'
	
		tmpString = '';
		headingOrder = []
		averages = []
		maximums = []
	
		for heading in self.__results:
			if heading != 'Total':
				tmpString += heading + ' : '
				headingOrder += [heading,]
			averages += [0.0,]
			maximums += [0.0,]
			
		tmpString += 'Total'
		headingOrder += ['Total',]
	
		print '<DartMeasurement name="HEADING" type="text/string">' + tmpString + '</DartMeasurement>'
		
		# get the number of self.__results
		numResults = self.number_of_results()
		numColumns = self.number_of_columns()
		
		for row in range(numResults):
			tmpString = ''
			for col_index in range(numColumns):
				heading = headingOrder[col_index]
				# update the averages and the maximums
				averages[col_index] += self.__results[heading][row]
				if self.__results[heading][row] > maximums[col_index]:
					maximums[col_index] = self.__results[heading][row]
				
				tmpString += str(self.__results[heading][row])
				if heading != 'Total':
					tmpString += ' : '
	
			print '<DartMeasurement name="run' + str(row) + '" type="text/string">' + tmpString + '</DartMeasurement>'
			
		# now handle the averages and the maximums
		tmpString = ''
		tmpStringMax = ''
		for col_index in range(numColumns):
			tmpString += str(averages[col_index]/numResults)
			tmpStringMax += str(maximums[col_index])
			if col_index < ( numColumns - 1 ):
				tmpString += ' : '
				tmpStringMax += ' : '
		
		print '<DartMeasurement name="Average" type="text/string">' + tmpString + '</DartMeasurement>'
		print '<DartMeasurement name="Maximums" type="text/string">' + tmpStringMax + '</DartMeasurement>'
	
	# output the results to a comma-separated file
	def output_as_CSV_file(self, filename, description = '', appendFile = False):
		def output_headings(file):
			tmpString = '';
			headingOrder = []
	
			for heading in self.__results:
				if heading != 'Total':
					tmpString += '"' + heading + '",'
					headingOrder += [heading,]
				
			tmpString += '"Total"'
			headingOrder += ['Total',]
		
			file.write(tmpString + '\n')
			return headingOrder
		
		def output_result_data(file, headingOrder):
			numResults = self.number_of_results()
			numColumns = self.number_of_columns()
			
			for row in range(numResults):
				tmpString = ''
				for col_index in range(numColumns):
					heading = headingOrder[col_index]
					
					tmpString += str(self.__results[heading][row])
					if heading != 'Total':
						tmpString += ','
		
				file.write(tmpString + '\n')
		
		if appendFile:
			out_file = open(filename, 'w+')
		else:
			out_file = open(filename, 'w')
			
		try:
			if description:
				out_file.write('"' + self.__nodeName + '","' +  description + '",' + str(self.number_of_results()) + '\n')
			headingOrder = output_headings(out_file)
			output_result_data(out_file, headingOrder)
		finally:
	  		out_file.close()
       
	def output_file_to_Dart(self, filename):
	 	print """<DartMeasurementFile name="CSV_file" type="text/plain">""" + str(filename) + """</DartMeasurementFile>"""
     	
def source_path():
	return os.environ["K3D_TEST_SOURCE_PATH"]

def binary_path():
	return os.environ["K3D_TEST_BINARY_PATH"]

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

def mesh_comparison(document, mesh, mesh_name, threshold):

	mesh_writer = document.new_node("K3DMeshWriter")
	mesh_writer.file = k3d.generic_path(binary_path() + "/" + mesh_name + ".output.k3d")
	document.set_dependency(mesh_writer.get_property("input_mesh"), mesh)

	reference = document.new_node("K3DMeshReader")

	difference = document.new_node("MeshDiff")
	difference.threshold = threshold
	difference.create_property("k3d::mesh*", "input_a", "InputA", "First input mesh")
	difference.create_property("k3d::mesh*", "input_b", "InputB", "Second input mesh")
	document.set_dependency(difference.get_property("input_a"), mesh)
	document.set_dependency(difference.get_property("input_b"), reference.get_property("output_mesh"))

	for index in range(1, 100):
		reference_file = k3d.generic_path(source_path() + "/meshes/" + mesh_name + ".reference." + str(index) + ".k3d")

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

	# Send the mesh difference to the dashboard as preformatted HTML, so the
	# columns all line-up.
	print """<DartMeasurement name="Mesh Difference" type="text/html"><![CDATA[\n"""
	print """<pre>"""
	print k3d.print_diff(mesh.pipeline_value(), reference.output_mesh, threshold)
	print """</pre>"""
	print """]]></DartMeasurement>\n"""
	sys.stdout.flush()

	raise Exception("output mesh differs from reference")

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

def mesh_modifier_benchmark(meshModifierNodeName, benchmarkMesh, numberOfRuns = 1, properties = {}):
	document = k3d.new_document()
		
	profiler = document.new_node("PipelineProfiler")
	
	benchmarkMesh.set_document(document)
	
	inputNode = benchmarkMesh.get_mesh()
	
	selection = k3d.deselect_all()
	selection.points = k3d.component_select_all()
	
	benchmarkNode = document.new_node(meshModifierNodeName)
	for (p, val) in properties.items():
		benchmarkNode.get_property(p).set_value(val)
	benchmarkNode.mesh_selection = selection
	
	profilingResults = k3dProfilingProcessor()
	for n in range(numberOfRuns):
		document.set_dependency(benchmarkNode.get_property("input_mesh"), inputNode.get_property("output_mesh"))
		benchmarkNode.output_mesh
		profilingResults.add_profiler_results_for_node(meshModifierNodeName, profiler.records)
	
	description = "%s Benchmark : %d" % (meshModifierNodeName, benchmarkMesh.get_size_metric())
	profilingResults.output_as_dart_table(description)
	# save to CSV file
	CSV_output_file = k3d.generic_path(binary_path() + "/" + meshModifierNodeName + str(benchmarkMesh.get_size_metric()) + ".benchmark.txt")
	profilingResults.output_as_CSV_file(str(CSV_output_file), description)
	
# Benchmark the performance of the Bitmap plugins using a solid as input 
def bitmap_benchmark(BitmapNodeName, imageDimensions, numberOfRuns = 1):
	document = k3d.new_document()
		
	profiler = document.new_node("PipelineProfiler")
	inputSolid = document.new_node("BitmapSolid")
	inputSolid.width = imageDimensions[0];
	inputSolid.height = imageDimensions[1];
	
	benchmarkNode = document.new_node(BitmapNodeName)
	
	profilingResults = k3dProfilingProcessor()
	for n in range(numberOfRuns):
		document.set_dependency(benchmarkNode.get_property("input_bitmap"), inputSolid.get_property("output_bitmap"))
		benchmarkNode.output_bitmap
		profilingResults.add_profiler_results_for_node(BitmapNodeName, profiler.records)
	
	description = "%s Benchmark : %d x %d" % (BitmapNodeName, imageDimensions[0], imageDimensions[1])
	profilingResults.output_as_dart_table(description)
	
	CSV_output_file = k3d.generic_path(binary_path() + "/" + BitmapNodeName + str(imageDimensions[0]) + 'x' + str(imageDimensions[1]) + ".benchmark.txt")
	profilingResults.output_as_CSV_file(str(CSV_output_file), description)
	#profilingResults.output_file_to_Dart(str(CSV_output_file))								    
	
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
