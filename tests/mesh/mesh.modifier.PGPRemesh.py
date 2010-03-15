#python

import k3d
import testing

document = k3d.new_document()

source = k3d.plugin.create("PolyTorus", document)

triangles = k3d.plugin.create("TriangulateFaces", document)
triangles.mesh_selection = k3d.select_all()
k3d.property.connect(document, source.get_property("output_mesh"), triangles.get_property("input_mesh"))

modifier = k3d.plugin.create("PGPRemesh", document)
modifier.use_smooth = False
modifier.steps = 0
modifier.omega = 1
modifier.div = 2
modifier.triangulate = True
k3d.property.connect(document, triangles.get_property("output_mesh"), modifier.get_property("input_mesh"))

#print "source output: " + repr(source.output_mesh)
#print "triangles output: " + repr(triangles.output_mesh)
#print "modifier output: " + repr(modifier.output_mesh)

testing.require_valid_mesh(document, modifier.get_property("output_mesh"))
testing.require_similar_mesh(document, modifier.get_property("output_mesh"), "mesh.modifier.PGPRemesh", 1)

