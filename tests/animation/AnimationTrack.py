#python

import k3d
import testing

document = k3d.new_document()

time_source = k3d.plugin.create("TimeSource", document)
instance = k3d.plugin.create("MeshInstance", document)
transform = k3d.plugin.create("FrozenMatrix", document)
track = k3d.plugin.create("AnimationTrackDoubleMatrix4", document)
interpolator = k3d.plugin.create("InterpolatorDoubleMatrix4Linear", document)
track.interpolator = interpolator
transform.matrix = k3d.translate3(3, 0, 0)

time = k3d.get_time(document)

document.set_dependency(instance.get_property("input_matrix"), track.get_property("output_value"))
document.set_dependency(track.get_property("value_input"), transform.get_property("output_matrix"))
document.set_dependency(track.get_property("time_input"), time_source.get_property("time"))

time.set_value(0.0)
transform.matrix = k3d.translate3(0, 0, 0)
track.keyframe()
time.set_value(4.0)
transform.matrix = k3d.translate3(2, 0, 0)
track.keyframe()
time.set_value(2.0)
position = k3d.world_position(instance)

reference = k3d.point3(1, 0, 0)

if position != reference:
  raise Exception("Position differs from expected value, expected: " + str(reference) + ", result: " + str(position))
