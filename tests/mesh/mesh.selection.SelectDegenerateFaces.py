#python

import k3d
import testing

document = k3d.new_document()

grid = k3d.plugin.create("PolyGrid", document)
scale = k3d.plugin.create("ScalePoints", document)
select = k3d.plugin.create("SelectDegenerateFaces", document)

selection = k3d.geometry.selection.create(0)
point_selection = k3d.geometry.point_selection.create(selection)
k3d.geometry.point_selection.append(point_selection, 0, 10000, 0.)
k3d.geometry.point_selection.append(point_selection, 0, 12, 1.)

scale.mesh_selection = selection
scale.x = 0.

k3d.property.connect(document, grid.get_property("output_mesh"), scale.get_property("input_mesh"))
k3d.property.connect(document, scale.get_property("output_mesh"), select.get_property("input_mesh"))

testing.require_valid_mesh(document, select.get_property("output_mesh"))
testing.require_similar_mesh(document, select.get_property("output_mesh"), "mesh.selection.SelectDegenerateFaces", 1)

