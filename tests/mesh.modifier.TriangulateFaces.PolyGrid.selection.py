#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyGrid", "TriangulateFaces")
setup.source.rows = 3
setup.source.columns = 3

selection = k3d.geometry.selection.create(0)
face_selection = k3d.geometry.primitive_selection.create(selection)
face_selection.primitive_begin().append(0)
face_selection.primitive_end().append(1)
face_selection.primitive_selection_type().append(k3d.selection.type.FACE)
face_selection.primitive_first_range().append(0)
face_selection.primitive_range_count().append(1)
face_selection.index_begin().append(0)
face_selection.index_end().append(3)
face_selection.weight().append(1)

setup.modifier.mesh_selection = selection

testing.mesh_comparison_to_reference(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.TriangulateFaces.PolyGrid.selection", 2)

