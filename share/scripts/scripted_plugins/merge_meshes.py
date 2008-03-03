#python

# k3d:plugin-class="application"
# k3d:plugin-type="ActionScript"
# k3d:plugin-name="Merge Meshes"
# ngui:action=""

#Action script to apply a MergeMeshScript scripted plugin to the current selection

import k3d

Document.start_change_set()


merger = Document.new_node("MergeMeshScript")
instance = Document.new_node("MeshInstance")

counter = 1
for node in Document.nodes():
  if node.selection_weight:
    if node.has_property("transformed_mesh"):
      propname = "mesh_" + str(counter)
      merger.create_property("k3d::mesh*", propname, "Mesh " + str(counter), "")
      Document.set_dependency(merger.get_property(propname), node.get_property("transformed_mesh"))
      counter += 1
      node.viewport_visible = False
      node.selection_weight = 0
  if node.name == "GL Default Painter":
    instance.gl_painter = node

Document.set_dependency(instance.get_property("input_mesh"), merger.get_property("output_mesh"))
instance.selection_weight = 1

Document.finish_change_set("Merge Meshes")

#except:
#  Document.cancel_change_set()