#python

import testing
import k3d
from math import pi

document = k3d.new_document()

path = k3d.plugin.create("NurbsCircle", document)
to_sweep = k3d.plugin.create("NurbsCircle", document)
rotate = k3d.plugin.create("RotatePoints", document)
translate = k3d.plugin.create("TranslatePoints", document)
merge_mesh = k3d.plugin.create("MergeMesh", document)
modifier = k3d.plugin.create("NurbsSweepSurface", document)

k3d.property.create(merge_mesh, "k3d::mesh*", "input_mesh1", "Input Mesh 1", "")
k3d.property.create(merge_mesh, "k3d::mesh*", "input_mesh2", "Input Mesh 2", "")

rotate.mesh_selection = k3d.geometry.selection.create(1)
translate.mesh_selection = k3d.geometry.selection.create(1)
rotate.x = 0.5*pi
translate.x = 5

modifier.mesh_selection = k3d.geometry.selection.create(1)
modifier.align_normal = False

path.thetamax = 0.6*pi
to_sweep.radius = 1.

k3d.property.connect(document, to_sweep.get_property("output_mesh"), rotate.get_property("input_mesh"))
k3d.property.connect(document, rotate.get_property("output_mesh"), translate.get_property("input_mesh"))
k3d.property.connect(document, path.get_property("output_mesh"), merge_mesh.get_property("input_mesh1"))
k3d.property.connect(document, translate.get_property("output_mesh"), merge_mesh.get_property("input_mesh2"))
k3d.property.connect(document, merge_mesh.get_property("output_mesh"), modifier.get_property("input_mesh"))

testing.require_valid_mesh(document, modifier.get_property("output_mesh"))
testing.require_similar_mesh(document, modifier.get_property("output_mesh"), "mesh.modifier.NurbsSweepSurface", 20)

