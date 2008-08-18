#python

import benchmarking


columns = [#"Create:Validate input",
           #"Merge selection",
           "Calculate companions",
           "Calculate indices",
           "Allocate memory",
           "Update indices",
           "Split edges",
           #"Update:Validate input",
           "Calculate positions",
           #"Update:Copy",
           "Total"]
           

benchmarking.generate_component_image("CUDASubdivideEdgesBreakdown", "CUDASubdivideEdges", columns )


columns = ["BIND_TEXTURE",
           "SETUP",
           "CONVERT_PRE",
           "TO_DEVICE",
           "EXECUTE",
           "TO_HOST",
           "CONVERT_POST",
           "CLEANUP",
           "Update Mesh",
           "Create Mesh",
           "Copy points",
           "Total"
           ]
           

benchmarking.generate_component_image("CUDATransformPointsBreakdown", "CUDATransformPoints", columns )