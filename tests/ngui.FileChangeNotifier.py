#python

import k3d
import testing
import time

document = k3d.documents()[0]

polycube1 = document.new_node("PolyCube")
polysphere1 = document.new_node("PolySphere")
polycube2 = document.new_node("PolyCube")
polysphere2 = document.new_node("PolySphere")
polycube2.columns = 2
polysphere2.v_segments = 32

cube_path = k3d.generic_path(testing.binary_path() + "/ngui.FileChangeNotifier.cube.output.k3d")
sphere_path = k3d.generic_path(testing.binary_path() + "/ngui.FileChangeNotifier.sphere.output.k3d")

cube_writer = document.new_node("K3DMeshWriter")
sphere_writer = document.new_node("K3DMeshWriter")
cube_writer.file = cube_path
sphere_writer.file = sphere_path

document.set_dependency(cube_writer.get_property("input_mesh"), polycube1.get_property("output_mesh"))
document.set_dependency(sphere_writer.get_property("input_mesh"), polysphere1.get_property("output_mesh"))

# TODO: set up readers before writers, to see if creation is watched (not working with inotify currently)
cube_reader = document.new_node("K3DMeshReader")
sphere_reader = document.new_node("K3DMeshReader")
cube_reader.scale_to_size = False
sphere_reader.scale_to_size = False
cube_reader.file = cube_path
sphere_reader.file = sphere_path

# test initial read
difference = testing.get_mesh_difference(document, cube_reader.get_property("output_mesh"), polycube1.get_property("output_mesh"), 1)
if not difference.equal:
  testing.output_mesh_difference(cube_reader.output_mesh, polycube1.output_mesh, 1)
  raise Exception("Read test 1 failed")

difference = testing.get_mesh_difference(document, sphere_reader.get_property("output_mesh"), polysphere1.get_property("output_mesh"), 1)
if not difference.equal:
  testing.output_mesh_difference(sphere_reader.output_mesh, polysphere1.output_mesh, 1)
  raise Exception("Read test 2 failed")

#change input, see if the read file was changed
document.set_dependency(cube_writer.get_property("input_mesh"), polycube2.get_property("output_mesh"))
document.set_dependency(sphere_writer.get_property("input_mesh"), polysphere2.get_property("output_mesh"))
time.sleep(1) # give the poller the time to catch up
k3d.ui().synchronize()

difference = testing.get_mesh_difference(document, cube_reader.get_property("output_mesh"), polycube2.get_property("output_mesh"), 1)
if not difference.equal:
  #testing.output_mesh_difference(cube_reader.output_mesh, polycube2.output_mesh, 1)
  raise Exception("Read test 3 failed")

difference = testing.get_mesh_difference(document, sphere_reader.get_property("output_mesh"), polysphere2.get_property("output_mesh"), 1)
if not difference.equal:
  #testing.output_mesh_difference(sphere_reader.output_mesh, polysphere2.output_mesh, 1)
  raise Exception("Read test 4 failed")

#now watch the same file on both readers
sphere_reader.file = cube_path
document.set_dependency(cube_writer.get_property("input_mesh"), polycube1.get_property("output_mesh"))
time.sleep(1) # give the poller the time to catch up
k3d.ui().synchronize()

difference = testing.get_mesh_difference(document, cube_reader.get_property("output_mesh"), polycube1.get_property("output_mesh"), 1)
if not difference.equal:
  #testing.output_mesh_difference(cube_reader.output_mesh, polycube1.output_mesh, 1)
  raise Exception("Read test 5 failed")

difference = testing.get_mesh_difference(document, sphere_reader.get_property("output_mesh"), polycube1.get_property("output_mesh"), 1)
if not difference.equal:
  #testing.output_mesh_difference(sphere_reader.output_mesh, polycube1.output_mesh, 1)
  raise Exception("Read test 6 failed")

# stop watching one file
cube_reader.watch_file = False
cube_reader.output_mesh
k3d.ui().synchronize()
document.set_dependency(cube_writer.get_property("input_mesh"), polycube2.get_property("output_mesh"))
time.sleep(1) # give the poller the time to catch up
k3d.ui().synchronize()

difference = testing.get_mesh_difference(document, cube_reader.get_property("output_mesh"), polycube1.get_property("output_mesh"), 1)
if not difference.equal:
  #testing.output_mesh_difference(cube_reader.output_mesh, polycube1.output_mesh, 1)
  raise Exception("Read test 7 failed")

difference = testing.get_mesh_difference(document, sphere_reader.get_property("output_mesh"), polycube2.get_property("output_mesh"), 1)
if not difference.equal:
  #testing.output_mesh_difference(sphere_reader.output_mesh, polycube2.output_mesh, 1)
  raise Exception("Read test 8 failed")
