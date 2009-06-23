#python

import k3d
import testing

document = k3d.new_document()

source = document.new_node("PolyGrid")

modifier = document.new_node("Delete")

# select some faces, distributed along polyhedra
selection = k3d.mesh_selection.deselect_all()
face_selection = k3d.mesh_selection.component(0, 1000, k3d.selection.type.UNIFORM, 12, 13, 1)
selection.add_component(face_selection)
modifier.mesh_selection = selection

document.set_dependency(modifier.get_property("input_mesh"), source.get_property("output_mesh"))

testing.mesh_comparison_to_reference(document, modifier.get_property("output_mesh"), "mesh.modifier.Delete", 1)
