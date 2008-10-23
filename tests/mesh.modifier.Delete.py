#python

import k3d
import testing

document = k3d.new_document()

source = document.new_node("PolyGrid")

modifier = document.new_node("Delete")

# select some faces, distributed along polyhedra
selection = k3d.deselect_all()
selection.faces = [(12, 13, 1)]
modifier.mesh_selection = selection

document.set_dependency(modifier.get_property("input_mesh"), source.get_property("output_mesh"))

testing.mesh_comparison_to_reference(document, modifier.get_property("output_mesh"), "mesh.modifier.Delete", 1)
