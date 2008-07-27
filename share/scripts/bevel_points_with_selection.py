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
selection_node = Document.get_node_by_metadata("inode_selection", "ngui:unique_node", "node_selection")
selection_node.select(mesh_instance, 1.0)
mesh_instance.show_component_selection = True
mesh_instance.gl_painter = doc.get_node("GL Default Painter")

doc.set_dependency(bevel_points.get_property("input_mesh"), cube.get_property("output_mesh"))
doc.set_dependency(mesh_instance.get_property("input_mesh"), bevel_points.get_property("output_mesh"))

doc.finish_change_set("Create Cube with 4 Bevels")

