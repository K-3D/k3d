#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("K3DMeshReader", "MergeCoplanarFaces")
setup.source.file = k3d.generic_path(testing.source_path() + "/meshes/mesh.modifier.MergeCoplanarFaces.input.k3d")

selection = k3d.mesh_selection.deselect_all()
selection.faces = k3d.mesh_selection.component_select_all()
setup.modifier.mesh_selection = selection 

testing.mesh_comparison_to_reference(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.MergeCoplanarFaces", 1)

