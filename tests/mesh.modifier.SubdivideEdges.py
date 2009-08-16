#python

import k3d
import testing

document = k3d.new_document()

reader = document.new_node("K3DMeshReader")
reader.file = k3d.filesystem.generic_path(testing.source_path() + "/meshes/testmesh.polyhedra.k3d")
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
selection = k3d.geometry.selection.create(0)
edge_selection = k3d.geometry.primitive_selection.create(selection, k3d.selection.type.FACE_VARYING)
k3d.geometry.primitive_selection.append(edge_selection, 0, 4294967295, 0)
k3d.geometry.primitive_selection.append(edge_selection, 0, 4, 1)
k3d.geometry.primitive_selection.append(edge_selection, 52, 65, 1)
k3d.geometry.primitive_selection.append(edge_selection, 74, 82, 1)
modifier.mesh_selection = selection
modifier.vertices = 2

document.set_dependency(modifier.get_property("input_mesh"), varying_colors.get_property("output_mesh"))

testing.mesh_comparison_to_reference(document, modifier.get_property("output_mesh"), "mesh.modifier.SubdivideEdges", 1)
