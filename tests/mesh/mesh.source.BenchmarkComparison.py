#python

import benchmarking

compare_list = ["PolyGrid", 
                ]

for run in compare_list:
    try:
        benchmarking.generate_comparison_image(run, (run, "CUDA"+run), plotLabels=('Mesh Size', 'Time [s]'), calculate_speedup = True)
    except:
        print "Error with: " + run
