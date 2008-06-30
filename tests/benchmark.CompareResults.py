#python

import benchmarking

"""
    selected is a list of tuples (PluginName, ColumnName)
"""
def compare_and_output_image(filename, selected_benchmarks, plotLabels = (None, None)):
    try:
        import pylab as P
    except:
        return 0
    
    import os
    
    processor = benchmarking.ResultsProcessor()
    for selected in selected_benchmarks:
        processor.add_dataset(selected[0], selected[1])
        
    processor.plot_data(plotLabels[0], plotLabels[1])

    image_filename = benchmarking.benchmark_path() + '/' + filename + '.png';
    try:
        os.delete(image_filename)
    except:
        pass
    
    P.savefig(benchmarking.benchmark_path() + '/' + filename + '.png', format='png')
    
    P.close()
    return benchmarking.benchmark_path() + '/' + filename  + '.png'
    


filename = compare_and_output_image("TransformPointsComparison", [("TransformPoints", "Total"),("CUDATransformPoints", "Total"),("CUDATransformPointsAsynchronous", "Total")], ('Number of Points', 'Time [s]'))
if filename != 0:
    out_filename = benchmarking.k3d.generic_path(filename)
    print """<DartMeasurementFile name="Benchmark Comparison" type="image/png">""" + str(out_filename) + """</DartMeasurementFile>"""


    selected = []
    selected += [("CUDATransformPoints", "BIND_TEXTURE"),]
    selected += [("CUDATransformPoints", "CONVERT_PRE"),]
    selected += [("CUDATransformPoints", "TO_DEVICE"),]
    selected += [("CUDATransformPoints", "EXECUTE"),]
    selected += [("CUDATransformPoints", "TO_HOST"),]
    selected += [("CUDATransformPoints", "CONVERT_POST"),]
    selected += [("CUDATransformPoints", "OTHER"),]
    selected += [("CUDATransformPoints", "Create Mesh"),]
    selected += [("CUDATransformPoints", "Update Mesh"),]
    selected += [("CUDATransformPoints", "Total"),]

filename = compare_and_output_image("CUDATransformPointsBreakdown", selected, ('Number of Pixels', 'Time [s]'))
if filename != 0:
    out_filename = benchmarking.k3d.generic_path(filename)
    print """<DartMeasurementFile name="Benchmark Comparison" type="image/png">""" + str(out_filename) + """</DartMeasurementFile>"""


filename = compare_and_output_image("BitmapComparison", [("BitmapAdd", "Total"),("CUDABitmapAdd", "Total")], ('Number of Pixels', 'Time [s]'))
if filename != 0:
    out_filename = benchmarking.k3d.generic_path(filename)
    print """<DartMeasurementFile name="Benchmark Comparison" type="image/png">""" + str(out_filename) + """</DartMeasurementFile>"""

