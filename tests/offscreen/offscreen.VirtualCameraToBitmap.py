#python

import k3d
import testing

doc = k3d.new_document()

axes = k3d.plugin.create("Axes", doc)
axes.xyplane = False

material = k3d.plugin.create("RenderManMaterial", doc)

torus = k3d.plugin.create("Torus", doc)
torus.material = material

mesh_instance = k3d.plugin.create("MeshInstance", doc)
mesh_instance.gl_painter = k3d.plugin.create("OpenGLTorusPainter", doc)
doc.set_dependency(mesh_instance.get_property("input_mesh"), torus.get_property("output_mesh"))

camera = testing.create_camera(doc)
render_engine = testing.create_opengl_engine(doc)

camera_to_bitmap = k3d.plugin.create("VirtualCameraToBitmap", doc)
camera_to_bitmap.camera = camera
camera_to_bitmap.render_engine = render_engine

testing.require_similar_bitmap(doc, camera_to_bitmap.get_property("output_bitmap"), "offscreen.VirtualCameraToBitmap", 0.009)

