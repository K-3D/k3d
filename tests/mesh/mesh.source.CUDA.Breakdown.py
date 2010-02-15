#python

import benchmarking

columns = ["Update Topology:init",
           "Update Topology:DeviceAllocate",
           "Update Topology:Kernel",
           "Update Topology:Copy",
           "Update Geometry:init",
           "Update Geometry:Kernel",
           "Update Geometry:Copy",
           "Total"]
           

benchmarking.generate_component_image("CUDAPolyGridBreakdown", "CUDAPolyGrid", columns )

columns = ["Update Topology",
           "Update Mesh Topology",
           "Update Geometry",
           "Update Mesh Geometry",
           "Total"]

benchmarking.generate_component_image("PolyGridBreakdown", "PolyGrid", columns )



columns = ["Update Topology:init",
           "Update Topology:DeviceAllocate",
           "Update Topology:Kernel",
           "Update Topology:Copy",
           "Update Geometry:init",
           "Update Geometry:Kernel",
           "Update Geometry:Transform Points",
           "Update Geometry:Copy",
           "Total"]


benchmarking.generate_component_image("CUDAPolyGridAndTransformPointsBreakdown", "CUDAPolyGridAndTransformPoints", columns )
