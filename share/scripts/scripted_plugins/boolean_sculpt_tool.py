#python

# k3d:plugin-class="document"
# k3d:plugin-type="NullOutputScript"
# k3d:plugin-name="BooleanSculptor"
# k3d:plugin-description="Sculpt a mesh with a tool, using boolean operations"

import k3d

def reset_mesh(FrozenMesh):
  storage_node = k3d.dynamic_cast(FrozenMesh, "imesh_storage")
  storage_node.clear_mesh()
  pipeline_trigger = FrozenMesh.output_mesh # make sure the frozen mesh is really reset

doc = Node.document()

if not Node.has_property("sculpted_mesh"):
  Node.create_property("k3d::mesh*", "sculpted_mesh", "Sculpted Mesh", "The mesh that will be sculpted")
if not Node.has_property("sculpt_tool"):
  Node.create_property("k3d::mesh*", "sculpt_tool", "Sculpt Tool", "The mesh that will be used as tool")

modifier = doc.get_node("CGALBoolean")
frozen_mesh_1 = doc.get_node("FrozenMesh")
frozen_mesh_2 = doc.get_node("FrozenMesh 2")
if not doc.has_node("CGALBoolean"): 
  if k3d.is_solid(Node.sculpted_mesh) and k3d.is_solid(Node.sculpt_tool):
    modifier = doc.new_node("CGALBoolean")
    modifier.create_property("k3d::mesh*", "input_1", "Input 1", "")
    modifier.create_property("k3d::mesh*", "input_2", "Input 2", "")
    modifier.type = "difference"
    
    frozen_mesh_1 = doc.new_node("FrozenMesh")
    frozen_mesh_2 = doc.new_node("FrozenMesh")
    
    doc.set_dependency(frozen_mesh_2.get_property("input_mesh"), modifier.get_property("output_mesh"))
    doc.set_dependency(modifier.get_property("input_1"), frozen_mesh_1.get_property("output_mesh"))
    
    doc.set_dependency(modifier.get_property("input_2"), Node.get_property("sculpt_tool"))
    instance = Document.new_node("MeshInstance")
    doc.set_dependency(instance.get_property("input_mesh"), modifier.get_property("output_mesh"))
    instance.gl_painter = doc.get_node("GL Default Painter")
    
    doc.set_dependency(frozen_mesh_1.get_property("input_mesh"), Node.get_property("sculpted_mesh"))
    reset_mesh(frozen_mesh_1)
    doc.set_dependency(frozen_mesh_1.get_property("input_mesh"), frozen_mesh_2.get_property("output_mesh"))
  
doc.set_dependency(modifier.get_property("input_2"), Node.get_property("sculpt_tool"))

if not k3d.is_uninitialized(modifier.output_mesh):
  reset_mesh(frozen_mesh_2)
  if (k3d.validate(frozen_mesh_2.output_mesh) and not k3d.is_uninitialized(frozen_mesh_2.output_mesh) and k3d.is_solid(frozen_mesh_2.output_mesh)):
    reset_mesh(frozen_mesh_1)