#python

import benchmarking
# a test to compare the benchmark results for a number of bitmap plugins

compare_list = ["SubdivideEdges",
                "TransformPoints", 
                ]

for run in compare_list:
    try:
        benchmarking.generate_comparison_image(run, (run, "CUDA"+run), plotLabels=('Mesh Size', 'Time [s]'), calculate_speedup = True)
    except:
        print "Error with: " + run
