#python
import testing

# load the reference image
initialBitmap = testing.setup_bitmap_reader_test("BitmapReader", "test_rgb_8.bmp")
document = initialBitmap.document

# create nodes for the regular as well as the CUDA bitmap Multiply implementation
cudaMultiply = document.new_node("CUDABitmapMultiply")
document.set_dependency(cudaMultiply.get_property("input_bitmap"), initialBitmap.reader.get_property("output_bitmap"))
referenceMultiply = document.new_node("BitmapMultiply")
document.set_dependency(referenceMultiply.get_property("input_bitmap"), initialBitmap.reader.get_property("output_bitmap"))

	
# Specify a list of values to be used in the Multiply tests as well as the threshold
MultiplyValueList = [0.0, 0.12, 0.23, 0.5, 1.34, 24]

threshold = 1e-8
# Change the Multiply value using each method and compare the results using BitmapPerceptualDifference
for MultiplyValue in MultiplyValueList:
    print """ Multiply Value """ + str(MultiplyValue)
    cudaMultiply.value = MultiplyValue
    referenceMultiply.value = MultiplyValue
    testing.bitmap_perceptual_difference(document, cudaMultiply.get_property("output_bitmap"), referenceMultiply.get_property("output_bitmap"), threshold)
    print """<DartMeasurement name="BitmapMultiplyValue" type="numeric/float">""" + str(MultiplyValue) + """</DartMeasurement>"""
