#python

import k3d
import testing

document = k3d.new_document()

instance = k3d.plugin.create("MeshInstance", document)
transform = k3d.plugin.create("FrozenMatrix", document)

transform.matrix = k3d.translate3(1, 0, 0)
k3d.property.connect(document, transform.get_property("output_matrix"), instance.get_property("input_matrix"))

position = k3d.world_position(instance)
reference = k3d.point3(1, 0, 0)

if position != reference:
  raise Exception("Position differs from expected value, expected: " + str(reference) + ", result: " + str(position))
