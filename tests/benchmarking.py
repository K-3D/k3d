#python

import testing
import k3d

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