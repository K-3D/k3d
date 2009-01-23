#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("K3DMeshReader", "MergeCollinearEdges")
setup.source.file = k3d.generic_path(testing.source_path() + "/meshes/mesh.modifier.MergeCoplanarFaces.reference.1.k3d")

setup.modifier.mesh_selection = k3d.mesh_selection.select_all()

testing.mesh_comparison_to_reference(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.MergeCollinearEdges", 1)

