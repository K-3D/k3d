#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("MergeCoplanarFaces", "MergeCollinearEdges")
plane = k3d.plugin.create("PolyGrid", setup.document)
k3d.property.connect(setup.document, plane.get_property("output_mesh"), setup.source.get_property("input_mesh"))

setup.source.mesh_selection = k3d.geometry.selection.create(1)
setup.modifier.mesh_selection = k3d.geometry.selection.create(1)

testing.require_valid_mesh(setup.document, setup.modifier.get_property("output_mesh"))
testing.require_similar_mesh(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.MergeCollinearEdges", 1)

