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


columns = [#"Create:Validate input",
           #"Merge selection",
           "Calculate companions",
           "Calculate indices",
           "Allocate memory",
           "Update indices",
           "Split edges",
           #"Update:Validate input",
           "Calculate positions",
           "Update Mesh",
           "Total"]

benchmarking.generate_component_image("SubdivideEdgesBreakdown", "SubdivideEdges", columns )


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

columns = ["Deform Mesh",
           "Update Mesh",
           "Create Mesh",
           "Copy points",
           "Total"
           ]
           

benchmarking.generate_component_image("TransformPointsBreakdown", "TransformPoints", columns )