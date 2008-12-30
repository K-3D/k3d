#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyCylinder", "SimplifyPolyhedra")

selection = k3d.mesh_selection.deselect_all()
selection.faces = k3d.mesh_selection.component_select_all()
setup.modifier.mesh_selection = selection

setup.source.top_segments = 3
setup.source.bottom_segments = 3

testing.mesh_comparison_to_reference(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.SimplifyPolyhedra", 1)

