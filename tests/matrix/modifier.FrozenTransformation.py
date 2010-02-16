#python

import k3d
import testing

document = k3d.new_document()

instance = document.new_node("MeshInstance")
transform = document.new_node("FrozenTransformation")

transform.matrix = k3d.translate3(1, 0, 0)
document.set_dependency(instance.get_property("input_matrix"), transform.get_property("output_matrix"))

position = k3d.world_position(instance)
reference = k3d.point3(1, 0, 0)

if position != reference:
  raise Exception("Position differs from expected value, expected: " + str(reference) + ", result: " + str(position))
