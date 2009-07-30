#python

import k3d
import sys
import os

# Create an article listing primitive components ...
print """Creating primitive components reference ..."""
article = file("@CMAKE_CURRENT_BINARY_DIR@/wikitext/primitives/reference/PrimitiveComponents", "w")
article.write("""<!-- Machine-generated file, do not edit by hand! -->\n""")
article.write("""<table frame="box" rules="all" cellpadding="5">\n""")
article.write("""<tr><th>Primitive Type</th><th>Structure</th><th>Attributes</th></tr>\n""")

document = k3d.new_document()
source = document.new_node("FrozenMesh")
mesh = source.create_mesh()

primitive_types = [k3d.bezier_triangle_patch, k3d.bicubic_patch, k3d.bilinear_patch, k3d.blobby, k3d.cone, k3d.cubic_curve, k3d.cylinder, k3d.disk, k3d.hyperboloid, k3d.linear_curve, k3d.nurbs_curve, k3d.nurbs_patch, k3d.paraboloid, k3d.point_group, k3d.polyhedron, k3d.sphere, k3d.teapot, k3d.torus]
for primitive_type in primitive_types:
	primitive_type.create(mesh)

for primitive in mesh.primitives():
	article.write("""<tr>""")
	article.write("""<td>""" + primitive.type() + """</td>""")
	article.write("""<td>""" + ", ".join(primitive.structure().keys()) + """</td>""")
	article.write("""<td>""" + ", ".join(primitive.attributes().keys()) + """</td>""")
	article.write("""</tr>\n""")

article.write("""</table>\n""")

k3d.close_document(document)

article.write("""<!-- Machine-generated file, do not edit by hand! -->\n""")


