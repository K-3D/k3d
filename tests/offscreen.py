#python

import sys
sys.path.append("@k3d-tests_BINARY_DIR@")

import k3d
import testing

doc = k3d.new_document()

axes = doc.new_node("Axes")
axes.xyplane = False

material = doc.new_node("RenderManMaterial")

torus = doc.new_node("Torus")
torus.material = material

camera = testing.create_camera(doc)
render_engine = testing.create_opengl_engine(doc)

camera_to_bitmap = doc.new_node("CameraToBitmap")
camera_to_bitmap.camera = camera
camera_to_bitmap.render_engine = render_engine

testing.image_comparison(doc, camera_to_bitmap.get_property("output_bitmap"), "offscreen", 0.009)

