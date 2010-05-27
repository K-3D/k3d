#python

import k3d
import testing

doc = k3d.new_document()
source = k3d.plugin.create("PolyGrid", doc)
centroid = k3d.plugin.create("PointsCentroid", doc)

source.rows = 1
source.columns = 1

selection = k3d.geometry.selection.create(0)
point_selection = k3d.geometry.point_selection.create(selection)
k3d.geometry.point_selection.append(point_selection, 0, 2, 1)
centroid.mesh_selection = selection

k3d.property.connect(doc, source.get_property("output_mesh"), centroid.get_property("input_mesh"))

if centroid.centroid != k3d.point3(0, 5, 0):
	raise "unexpected output_vector: " + str(centroid.centroid)

