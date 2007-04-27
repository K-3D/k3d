#python

import k3d

doc = Document
doc.start_change_set()

cube = doc.new_node("PolyCube")

bevel_points = doc.new_node("BevelPoints")
selection = k3d.deselect_all()
selection.points = [(0, 4, 1), (4, 1000, 0)]
bevel_points.mesh_selection = selection

mesh_instance = doc.new_node("MeshInstance")
mesh_instance.selection_weight = 1.0
mesh_instance.show_component_selection = True

doc.set_dependency(bevel_points.get_property("input_mesh"), cube.get_property("output_mesh"))
doc.set_dependency(mesh_instance.get_property("input_mesh"), bevel_points.get_property("output_mesh"))

doc.finish_change_set("Create Cube with 4 Bevels")

