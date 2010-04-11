#python

import benchmarking

benchmarking.mesh_source_benchmark("CUDAPolyGridAndTransformPoints", properties = {"rows" : benchmarking.polyGridRange, "columns" : benchmarking.polyGridRange})