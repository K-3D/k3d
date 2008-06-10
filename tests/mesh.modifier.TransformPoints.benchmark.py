#python

import testing
import k3d

current_count = [1,1,1]

for k in range(20):
    try:
        benchmark_mesh = testing.benchmarkMesh(current_count)
        testing.mesh_modifier_benchmark("TransformPoints", benchmark_mesh, 10, {"input_matrix" : k3d.translate3(k3d.vector3(0, 0, 1))})
        current_count[k % 3] *= 2
    except:
        break