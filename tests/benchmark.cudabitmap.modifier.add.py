#python

# the following test is intended to server as a comparitive benchmark for the CUDA Add plugin.  
# If the test is run in verbose mode (-V) a timing summary table is displayed at the end of the test.

import testing
import k3d

# creat the k3d document
document = k3d.new_document()

# create the reference image
initialBitmap = document.new_node("BitmapSolid")

# create nodes for the regular as well as the CUDA bitmap add implementation
cudaAdd = document.new_node("CUDABitmapAdd")
referenceAdd = document.new_node("BitmapAdd")

widths = [8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096]
heights = widths

addValue = 3.465;
cudaAdd.value = addValue
referenceAdd.value = addValue
threshold = 1e-8

timing_data = {};

for i in range(len(widths)):
    initialBitmap.width = widths[i]
    initialBitmap.height = heights[i]
    # set the dependency of the CUDA bitmap to run the test
    document.set_dependency(cudaAdd.get_property("input_bitmap"), initialBitmap.get_property("output_bitmap"))
    # set the dependency of the reference bitmap
    document.set_dependency(referenceAdd.get_property("input_bitmap"), initialBitmap.get_property("output_bitmap"))
    # calculate the difference between the CUDA and reference implementation
    testing.bitmap_perceptual_difference(document, cudaAdd.get_property("output_bitmap"), referenceAdd.get_property("output_bitmap"), threshold)    
    # read the timing info
    DataLine = [widths[i], heights[i], referenceAdd.timer, cudaAdd.host_to_device_time, cudaAdd.kernel_time, cudaAdd.device_to_host_time]    
    timing_data[i] = DataLine

print 'Benchmark summary'
print 'Image Width, Image Height, BitmapAdd Time (s), CUDA host to device time (s), CUDA kernel time (s), CUDA device to host time (s)'
for i in timing_data:    
    print timing_data[i]
