#python

import benchmarking
# a test to compare the benchmark results for a number of bitmap plugins

compare_list = ["BitmapAdd", 
                "BitmapSubtract", 
                "BitmapMultiply", 
                "BitmapColorMonochrome", 
                "BitmapGamma", 
                "BitmapInvert", 
                "BitmapMatteColorDiff", 
                "BitmapMatteInvert",
                "BitmapThreshold"]

for run in compare_list:
    try:
        benchmarking.generate_comparison_image(run, (run, "CUDA"+run))
    except:
        print "Error with: " + run
