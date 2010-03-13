#python 

import testing, k3d, sys

doc = k3d.new_document()

source = k3d.plugin.create("PolyGrid", doc)

k3d_to_cuda = k3d.plugin.create("CUDAMeshK3dToCUDAConverter", doc)
cuda_to_k3d = k3d.plugin.create("CUDAMeshCUDAToK3dConverter", doc)

k3d.property.connect(doc, source.get_property("output_mesh"), k3d_to_cuda.get_property("input_mesh"))
k3d.property.connect(doc, k3d_to_cuda.get_property("device_output_mesh"), cuda_to_k3d.get_property("device_input_mesh"))

difference = k3d.plugin.create("MeshDiff", doc)
difference.threshold = 1
difference.create_property("k3d::mesh*", "input_a", "InputA", "First input mesh")
difference.create_property("k3d::mesh*", "input_b", "InputB", "Second input mesh")
k3d.property.connect(doc, source.get_property("output_mesh"), difference.get_property("input_a"))
k3d.property.connect(doc, cuda_to_k3d.get_property("output_mesh"), difference.get_property("input_b"))

if not difference.equal: 
    # Send the mesh difference to the dashboard as preformatted HTML, so the
    # columns all line-up.
    print """<DartMeasurement name="Mesh Difference" type="text/html"><![CDATA[\n"""
    print """<pre>"""
    print k3d.print_diff(cuda_to_k3d.output_mesh, source.output_mesh, 1)
    print """</pre>"""
    print """]]></DartMeasurement>\n"""
    sys.stdout.flush()
    
    raise Exception("output mesh differs from reference")
