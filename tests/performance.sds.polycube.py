#python

import k3d
import math
import testing

document = k3d.documents()[0]

edge_painter = document.new_node("GLEdgePainter")
edge_painter.name = "Test edge painter"

face_painter = document.new_node("GLFacePainter")
face_painter.name = "Test face painter"

painter = document.new_node("OpenGLMultiPainter")
painter.name = "Test painter"
painter.add_user_property("k3d::gl::imesh_painter*", "edges", "Edges", "Edge Painter")
painter.add_user_property("k3d::gl::imesh_painter*", "faces", "Faces", "Face Painter")
painter.edges = edge_painter
painter.faces = face_painter

mesh_instance = document.new_node("MeshInstance")
mesh_instance.name = "Test mesh instance"
mesh_instance.gl_painter = painter

source = document.new_node("PolyCube");
source.name = "Test source"
source.rows = 25
source.columns = 25
source.slices = 25
document.set_dependency(mesh_instance.get_property("input_mesh"), source.get_property("output_mesh"))

orientation = document.new_node("Orientation")
orientation.name = "Test orientation"
document.set_dependency(mesh_instance.get_property("input_matrix"), orientation.get_property("output_matrix"))

timer = testing.timer()

frame_count = 0
for theta in range(0, 360, 5):
	frame_count += 1
	orientation.z = math.radians(theta)
	k3d.ui().synchronize()

elapsed_time = timer.elapsed()
average_time = elapsed_time / frame_count

print "frames: ", frame_count, " elapsed: ", elapsed_time, " average fps: ", 1 / average_time

