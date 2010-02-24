#python

import benchmarking

benchmarking.mesh_source_benchmark("CUDAPolyGrid", properties = {"rows" : benchmarking.polyGridRange, "columns" : benchmarking.polyGridRange})