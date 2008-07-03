#python

import benchmarking

def test_google_charts_api (xdata = [0, 1], ydata = [0, 1]):
    import pygooglechart as pgc
    
    # the maximum number of pixels in a chart is 300 000 - 600x450 = 270 000
    chart = pgc.XYLineChart(600, 450, x_range=(min(xdata), max(xdata)), y_range=(min(ydata), max(ydata)))
    
    chart.add_data(xdata);
    chart.add_data(ydata);
    
    chart.set_title('Title')
    chart.set_axis_labels('x', 'X-Axis')
    chart.set_axis_labels('y', 'Y-Axis')
    
    chart.download('test_google_charts_api.png')
    
    
    


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
    

def run_compare_test():

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
    
run_compare_test()