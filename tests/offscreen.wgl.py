#python

import k3d
import testing

doc = k3d.new_document()

axes = doc.new_node("Axes")

edge_painter = doc.new_node("OpenGLEdgePainter")
face_painter = doc.new_node("OpenGLFacePainter")
painter = doc.new_node("OpenGLMultiPainter")
painter.create_property("k3d::gl::imesh_painter*", "edges", "Edges", "Edge Painter")
painter.create_property("k3d::gl::imesh_painter*", "faces", "Faces", "Face Painter")
painter.edges = edge_painter
painter.faces = face_painter

cylinder = doc.new_node("PolyCylinder")
cylinder_instance = doc.new_node("MeshInstance")
cylinder_instance.gl_painter = painter
doc.set_dependency(cylinder_instance.get_property("input_mesh"), cylinder.get_property("output_mesh"))

camera = doc.new_node("Camera")

camera_position = doc.new_node("Position")
camera_position.x = -20 
camera_position.y = -20
camera_position.z = 10

camera_up = doc.new_node("Position")
camera_up.x = 0
camera_up.y = 0
camera_up.z = 20

camera_view = doc.new_node("ViewMatrix")
doc.set_dependency(camera_view.get_property("position"), camera_position.get_property("output_matrix"))
doc.set_dependency(camera_view.get_property("look"), cylinder_instance.get_property("output_matrix"))
doc.set_dependency(camera_view.get_property("up"), camera_up.get_property("output_matrix"))
doc.set_dependency(camera.get_property("input_matrix"), camera_view.get_property("output_matrix"))

render_engine = doc.new_node("OpenGLEngine")

camera_to_bitmap = doc.new_node("WGLCameraToBitmap")
camera_to_bitmap.camera = camera
camera_to_bitmap.render_engine = render_engine

bitmap_writer = doc.new_node("PNGBitmapWriter")
bitmap_writer.file = testing.binary_path() + "/offscreen.wgl.png"

doc.set_dependency(bitmap_writer.get_property("input_bitmap"), camera_to_bitmap.get_property("output_bitmap"));

