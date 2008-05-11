#python
import testing

# load the reference image
initialBitmap = testing.setup_bitmap_reader_test("BitmapReader", "test_rgb_8.bmp")
document = initialBitmap.document

# create nodes for the regular as well as the CUDA bitmap add implementation
cudaAdd = document.new_node("CUDABitmapAdd")
document.set_dependency(cudaAdd.get_property("input_bitmap"), initialBitmap.reader.get_property("output_bitmap"))
referenceAdd = document.new_node("BitmapAdd")
document.set_dependency(referenceAdd.get_property("input_bitmap"), initialBitmap.reader.get_property("output_bitmap"))

	
# Specify a list of values to be used in the add tests as well as the threshold
addValueList = [0.0, 1.0, 10, 12.5, 55, 100.5]

threshold = 1e-8
# Change the add value using each method and compare the results using BitmapPerceptualDifference
for addValue in addValueList:
    print """ Add Value """ + str(addValue)
    cudaAdd.value = addValue
    referenceAdd.value = addValue
    testing.bitmap_perceptual_difference(document, cudaAdd.get_property("output_bitmap"), referenceAdd.get_property("output_bitmap"), threshold)
    print """<DartMeasurement name="BitmapAddValue" type="numeric/float">""" + str(addValue) + """</DartMeasurement>"""
