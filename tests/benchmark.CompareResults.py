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
    
def run_compare_test():

    filename = benchmarking.compare_and_output_image("TransformPointsComparison", [("TransformPoints", "Total"),("CUDATransformPoints", "Total"),("CUDATransformPointsAsynchronous", "Total")], ('Number of Points', 'Time [s]'))
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
    selected += [("CUDATransformPoints", "SETUP"),]
    selected += [("CUDATransformPoints", "CLEANUP"),]
    selected += [("CUDATransformPoints", "Create Mesh"),]
    selected += [("CUDATransformPoints", "Update Mesh"),]
    selected += [("CUDATransformPoints", "Total"),]

    filename = benchmarking.compare_and_output_image("CUDATransformPointsBreakdown", selected, ('Number of Pixels', 'Time [s]'))
    if filename != 0:
        out_filename = benchmarking.k3d.generic_path(filename)
        print """<DartMeasurementFile name="Benchmark Comparison" type="image/png">""" + str(out_filename) + """</DartMeasurementFile>"""
    
    selected = [("CUDASubdivideEdges", "64to32: Host"), ("CUDASubdivideEdges", "64to32: Device")]
    filename = benchmarking.compare_and_output_image("CUDASubdivideConversion", selected, ('Mesh Size', 'Time [s]'))
    if filename != 0:
        out_filename = benchmarking.k3d.generic_path(filename)
        print """<DartMeasurementFile name="Benchmark Comparison" type="image/png">""" + str(out_filename) + """</DartMeasurementFile>"""
    
run_compare_test()