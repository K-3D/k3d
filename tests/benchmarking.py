#python

import testing
import k3d

CALC_AVERAGE = -1

def benchmark_path():
    return testing.benchmark_path()

class ResultSet(object):
    def __init__(self, x, y, label, plot_style):
        self.x = x
        self.y = y
        self.label = label
        self.plot_style = plot_style
    
class ResultsProcessor(object):
    def __init__(self, operation = CALC_AVERAGE):
        self.__operation = operation
        self.__results = []
    
    def add_dataset(self, benchmarkPluginName, ColumnTitle, plot_style = 'o-'):
        print "Adding Dataset %s : %s" % (benchmarkPluginName, ColumnTitle)
        (x, y) = extract_data(benchmarkPluginName, ColumnTitle, self.__operation)
        self.__results += [ResultSet(x, y, (benchmarkPluginName, ColumnTitle, self.__operation), plot_style)]
        
    def plot_data(self, xlabel=None, ylabel=None):
        import pylab as P
        legend = ()
        for result in self.__results:
            P.plot(result.x, result.y, result.plot_style)
            legend += ("%s : %s" % (result.label[0], result.label[1]),)
        
        
        if xlabel:
            P.xlabel(xlabel)
        if ylabel:
            P.ylabel(ylabel)
        
        P.legend(legend, loc='upper left')
        # set the legend fontsize    
        ltexts = P.gca().get_legend().get_texts()
        for lt in ltexts:
            P.setp(lt, fontsize = 10)
            

        
# run a mesh modifier benchmark for the specified node
def mesh_modifier_benchmark(benchmarkPluginName, maxSize = 15):
    current_count = [1,1,1]
    append = True
    for k in range(maxSize):
        try:
            benchmark_mesh = testing.benchmarkMesh(current_count)
            testing.mesh_modifier_benchmark(benchmarkPluginName, benchmark_mesh, 10, {"input_matrix" : k3d.translate3(k3d.vector3(0, 0, 1))}, append, k == 0)
            current_count[k % 3] *= 2
        except:
            break
        
def bitmap_modifier_benchmark(benchmarkPluginName):
    # benchmark benchmarkPluginName for various image sizes
    sizes = [(640,480), (800,600), (1024,768), (1280,1024), (1600, 1200)]
    runsPerBenchmark = 10
    append = True
    for k in range(len(sizes)):
        try:
            testing.bitmap_benchmark(benchmarkPluginName, sizes[k], runsPerBenchmark, append, k == 0)
        except:
            break
        
def convert_dim_string_to_size_measure(dimString):
    # image where dimension is given as (width)x(height)
    if 'x' in dimString:
        dims = dimString.split('x')
        return int(dims[0])*int(dims[1])
    
    # as default try to convert string to int and return
    # return 0 if it fails
    try:
        dims = int(dimString)
        return dims
    except:
        pass

    return 0
 
 
"""
    Read the data from a benchmark file generated py parsing the pipeline profiler output
    @param - bencmarkPluginName : the name of the plugin that was benchmarked
    @param - ColumnTitle : the column of interest
    @param - operation : specify the operation to perform on the data.  If operation >= 0 then a specific run is 
                         selected.
"""        
def extract_data(benchmarkPluginName, ColumnTitle = "Total", operation = CALC_AVERAGE):
    """
        Open the benchmark file and read the lines
    """
    def read_benchmark_file(pluginName):
        benchmarkFilename = benchmark_path() + '/' + benchmarkPluginName + '.benchmark.txt'
        f = open(benchmarkFilename, 'r')
        lines = f.readlines()
        f.close()
        return (lines, len(lines))
    
    def process_header_lines(lines, k):
        splitLine = lines[k].strip().split(',')
        dimension = convert_dim_string_to_size_measure(splitLine[1])
        number_of_runs = int(splitLine[2])
        
        splitLine = lines[k+1].strip().split(',')
        if ('"' + ColumnTitle + '"') in splitLine:
            column_of_interest_index = splitLine.index('"' + ColumnTitle + '"')
            
        return (k+2, dimension, number_of_runs, column_of_interest_index)
    
    def process_data_lines(lines, k, number_of_runs, column_of_interest_index, operation):    
        """
            Retrive the desired data from the lines
            @param operation - specify the row of interest or average, max, min
        """
        if operation > 0:
            splitLine = lines[k+operation-1].strip().split(',')
            datum = float(splitLine[column_of_interest_index])
        elif operation == CALC_AVERAGE:
            datum = 0
            for data_row in range(k, k+number_of_runs):
                splitLine = lines[data_row].strip().split(',')
                datum += float(splitLine[column_of_interest_index])
            datum /= number_of_runs
        else:
            datum = -1
        
        return (k+number_of_runs, datum)
        
    (lines, number_of_lines) = read_benchmark_file(benchmarkPluginName)
    k = 0;
    data_list = []
    dimension_list = []
    
    while k < number_of_lines:
        (k, dimension, number_of_runs, column_of_interest_index) = process_header_lines(lines, k)
        if k >= number_of_lines:
            break
        (k, datum) = process_data_lines(lines, k, number_of_runs, column_of_interest_index, operation)
        dimension_list += [dimension, ]
        data_list += [datum, ]
    
    return (dimension_list, data_list)
        
    
    
