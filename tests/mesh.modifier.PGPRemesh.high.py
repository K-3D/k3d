#python

import k3d
import testing

document = k3d.new_document()

source = document.new_node("PolyTorus")

triangles = document.new_node("TriangulateFaces")
triangles.mesh_selection = k3d.select_all()
document.set_dependency(triangles.get_property("input_mesh"), source.get_property("output_mesh"))

modifier = document.new_node("PGPRemesh")
modifier.use_smooth = False
modifier.steps = 0
modifier.omega = 5
modifier.div = 17
modifier.triangulate = True
document.set_dependency(modifier.get_property("input_mesh"), triangles.get_property("output_mesh"))

#print "source output: " + repr(source.output_mesh)
#print "triangles output: " + repr(triangles.output_mesh)
#print "modifier output: " + repr(modifier.output_mesh)

testing.mesh_comparison_to_reference(document, modifier.get_property("output_mesh"), "mesh.modifier.PGPRemesh.high", 1)

