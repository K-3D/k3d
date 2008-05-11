#python
import testing

# load the reference image
initialBitmap = testing.setup_bitmap_reader_test("BitmapReader", "test_rgb_8.bmp")
document = initialBitmap.document

# create nodes for the regular as well as the CUDA bitmap Subtract implementation
cudaSubtract = document.new_node("CUDABitmapSubtract")
document.set_dependency(cudaSubtract.get_property("input_bitmap"), initialBitmap.reader.get_property("output_bitmap"))
referenceSubtract = document.new_node("BitmapSubtract")
document.set_dependency(referenceSubtract.get_property("input_bitmap"), initialBitmap.reader.get_property("output_bitmap"))

	
# Specify a list of values to be used in the Subtract tests as well as the threshold
SubtractValueList = [0.0, 1.0, 10, 12.5, 55, 100.5]

threshold = 1e-8
# Change the Subtract value using each method and compare the results using BitmapPerceptualDifference
for SubtractValue in SubtractValueList:
    print """ Subtract Value """ + str(SubtractValue)
    cudaSubtract.value = SubtractValue
    referenceSubtract.value = SubtractValue
    testing.bitmap_perceptual_difference(document, cudaSubtract.get_property("output_bitmap"), referenceSubtract.get_property("output_bitmap"), threshold)
    print """<DartMeasurement name="BitmapSubtractValue" type="numeric/float">""" + str(SubtractValue) + """</DartMeasurement>"""
