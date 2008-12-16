#python

import k3d
import testing

document = k3d.new_document()

reader = document.new_node("K3DMeshReader")
reader.file = k3d.generic_path(testing.source_path() + "/meshes/testmesh.polyhedra.k3d")
reader.center = False
reader.scale_to_size = False

script_path = testing.source_path() + "/../share/scripts/MeshModifierScript/random_face_varying_colors.py"
script_file = open(script_path, "r")
script = ""
line = script_file.readline()
while line:
  script += line
  line = script_file.readline()
varying_colors = document.new_node("MeshModifierScript")
varying_colors.script = script

document.set_dependency(varying_colors.get_property("input_mesh"), reader.get_property("output_mesh"))

modifier = document.new_node("SubdivideEdges")
# select some edges, distributed along polyhedra
selection = k3d.mesh_selection.deselect_all()
selection.edges =[(0, 4294967295, 0), (0, 1, 1), (1, 2, 1), (2, 3, 1), (3, 4, 1), (52, 53, 1), (53, 54, 1), (54, 55, 1), (55, 56, 1), (56, 57, 1), (57, 58, 1), (58, 59, 1), (59, 60, 1), (60, 61, 1), (61, 62, 1), (62, 63, 1), (63, 64, 1), (64, 65, 1), (74, 75, 1), (75, 76, 1), (76, 77, 1), (77, 78, 1), (78, 79, 1), (79, 80, 1), (80, 81, 1), (81, 82, 1)]
modifier.mesh_selection = selection
modifier.vertices = 2

document.set_dependency(modifier.get_property("input_mesh"), varying_colors.get_property("output_mesh"))

testing.mesh_comparison_to_reference(document, modifier.get_property("output_mesh"), "mesh.modifier.SubdivideEdges", 1)
