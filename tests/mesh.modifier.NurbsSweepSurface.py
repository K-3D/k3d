#python

import testing
import k3d
from math import pi

document = k3d.new_document()

path = document.new_node("NurbsCircle")
to_sweep = document.new_node("NurbsCircle")
rotate = document.new_node("RotatePoints")
translate = document.new_node("TranslatePoints")
merge_mesh = document.new_node("MergeMesh")
modifier = document.new_node("NurbsSweepSurface")

merge_mesh.create_property("k3d::mesh*", "input_mesh1", "Input Mesh 1", "")
merge_mesh.create_property("k3d::mesh*", "input_mesh2", "Input Mesh 2", "")

rotate.mesh_selection = k3d.geometry.selection.create(1)
translate.mesh_selection = k3d.geometry.selection.create(1)
rotate.x = 0.5*pi
translate.x = 5

modifier.mesh_selection = k3d.geometry.selection.create(1)
modifier.align_normal = False

path.thetamax = 0.6*pi
to_sweep.radius = 1.

document.set_dependency(rotate.get_property("input_mesh"), to_sweep.get_property("output_mesh"))
document.set_dependency(translate.get_property("input_mesh"), rotate.get_property("output_mesh"))
document.set_dependency(merge_mesh.get_property("input_mesh1"), path.get_property("output_mesh"))
document.set_dependency(merge_mesh.get_property("input_mesh2"), translate.get_property("output_mesh"))
document.set_dependency(modifier.get_property("input_mesh"), merge_mesh.get_property("output_mesh"))

testing.require_valid_primitives(document, modifier.get_property("output_mesh"))
testing.mesh_reference_comparison(document, modifier.get_property("output_mesh"), "mesh.modifier.NurbsSweepSurface", 1)

