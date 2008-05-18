#python

# the following test is intended to server as a comparitive benchmark for the CUDA Subtract plugin.  
# If the test is run in verbose mode (-V) a timing summary table is displayed at the end of the test.

import testing
import k3d

# creat the k3d document
document = k3d.new_document()

# create the reference image
initialBitmap = document.new_node("BitmapSolid")

# create nodes for the regular as well as the CUDA bitmap Subtract implementation
cudaSubtract = document.new_node("CUDABitmapSubtract")
referenceSubtract = document.new_node("BitmapSubtract")

widths = [8, 16, 32, 64, 128, 256, 512, 1024]
heights = widths

SubtractValue = 3.465;
cudaSubtract.value = SubtractValue
referenceSubtract.value = SubtractValue
threshold = 1e-8

timing_data = {};

for i in range(len(widths)):
    initialBitmap.width = widths[i]
    initialBitmap.height = heights[i]
    # set the dependency of the CUDA bitmap to run the test
    document.set_dependency(cudaSubtract.get_property("input_bitmap"), initialBitmap.get_property("output_bitmap"))
    # set the dependency of the reference bitmap
    document.set_dependency(referenceSubtract.get_property("input_bitmap"), initialBitmap.get_property("output_bitmap"))
    # calculate the difference between the CUDA and reference implementation
    testing.bitmap_perceptual_difference(document, cudaSubtract.get_property("output_bitmap"), referenceSubtract.get_property("output_bitmap"), threshold)    
    # read the timing info
    DataLine = [widths[i], heights[i], 0.0, cudaSubtract.host_to_device_time, cudaSubtract.kernel_time, cudaSubtract.device_to_host_time]    
    timing_data[i] = DataLine

print 'Benchmark summary'
print 'Image Width, Image Height, BitmapSubtract Time (s), CUDA host to device time (s), CUDA kernel time (s), CUDA device to host time (s)'
for i in timing_data:    
    print timing_data[i]
