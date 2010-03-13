#python

import k3d
import testing

document = k3d.new_document()

grid = k3d.plugin.create("PolyGrid"), document;
grid.columns = 1
grid.rows = 1

modifier = k3d.plugin.create("SubdivideEdges", document)
cuda_modifier = k3d.plugin.create("CUDASubdivideEdges", document)

modifier.mesh_selection = k3d.geometry.selection.create(1)
modifier.vertices = 1

cuda_modifier.mesh_selection = modifier.mesh_selection
cuda_modifier.vertices = 1

k3d.property.connect(document, grid.get_property("output_mesh"), modifier.get_property("input_mesh"))
k3d.property.connect(document, grid.get_property("output_mesh"), cuda_modifier.get_property("input_mesh"))

testing.output_mesh_difference(cuda_modifier.output_mesh, modifier.output_mesh, 1)

difference = testing.get_mesh_difference(document, cuda_modifier.get_property("output_mesh"), modifier.get_property("output_mesh"), 1)

if not difference.equal:
    raise "Meshes not equal"
