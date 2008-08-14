#python

import benchmarking

compare_list = ["PolyGrid", 
                ]

for run in compare_list:
    try:
        benchmarking.generate_comparison_image(run, (run, "CUDA"+run))
    except:
        print "Error with: " + run
