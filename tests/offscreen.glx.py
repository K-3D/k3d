#python

import sys
sys.path.append("@k3d-tests_BINARY_DIR@")

import k3d
import testing

doc = k3d.application.new_document()

axes = doc.new_node("Axes")
axes.xyplane = False

painter = testing.create_default_painter(doc)

cylinder = doc.new_node("PolyCylinder")
cylinder_instance = doc.new_node("MeshInstance")
cylinder_instance.gl_painter = painter
doc.set_dependency(cylinder_instance.get_property("input_mesh"), cylinder.get_property("output_mesh"))

camera = testing.create_camera(doc)
render_engine = testing.create_opengl_engine(doc)

camera_to_bitmap = doc.new_node("GLXCameraToBitmap")
camera_to_bitmap.camera = camera
camera_to_bitmap.render_engine = render_engine

testing.image_comparison(doc, camera_to_bitmap.get_property("output_bitmap"), "offscreen.glx", 0.013)

