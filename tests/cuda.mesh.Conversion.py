#python 

import testing, k3d

doc = k3d.new_document()

source = doc.new_node("PolyGrid")

k3d_to_cuda = doc.new_node("CUDAMeshK3dToCUDAConverter")
cuda_to_k3d = doc.new_node("CUDAMeshCUDAToK3dConverter")

doc.set_dependency(k3d_to_cuda.get_property("input_mesh"), source.get_property("output_mesh"))
doc.set_dependency(cuda_to_k3d.get_property("device_input_mesh"), k3d_to_cuda.get_property("device_output_mesh"))

difference = document.new_node("MeshDiff")
difference.threshold = threshold
difference.create_property("k3d::mesh*", "input_a", "InputA", "First input mesh")
difference.create_property("k3d::mesh*", "input_b", "InputB", "Second input mesh")
document.set_dependency(difference.get_property("input_a"), source)
document.set_dependency(difference.get_property("input_b"), cuda_to_k3d.get_property("output_mesh"))

if not difference.equal: 
    # Send the mesh difference to the dashboard as preformatted HTML, so the
    # columns all line-up.
    print """<DartMeasurement name="Mesh Difference" type="text/html"><![CDATA[\n"""
    print """<pre>"""
    print k3d.print_diff(mesh.pipeline_value(), reference.output_mesh, threshold)
    print """</pre>"""
    print """]]></DartMeasurement>\n"""
    sys.stdout.flush()
    
    raise Exception("output mesh differs from reference")
