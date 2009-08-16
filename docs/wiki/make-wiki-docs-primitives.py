#python

import k3d
import sys
import os

primitive_types = [k3d.bezier_triangle_patch, k3d.bicubic_patch, k3d.bilinear_patch, k3d.blobby, k3d.cone, k3d.cubic_curve, k3d.cylinder, k3d.disk, k3d.hyperboloid, k3d.linear_curve, k3d.nurbs_curve, k3d.nurbs_patch, k3d.paraboloid, k3d.particle, k3d.polyhedron, k3d.sphere, k3d.teapot, k3d.torus]
primitive_names = ["Bezier Triangle Patch", "Bicubic Patch", "Bilinear Patch", "Blobby", "Cone", "Cubic Curve", "Cylinder", "Disk", "Hyperboloid", "Linear Curve", "NURBS Curve", "NURBS Patch", "Paraboloid", "Particle", "Polyhedron", "Sphere", "Teapot", "Torus"]

# Create a table listing primitive components ...
print """Creating primitive components table ..."""
article = file("@CMAKE_CURRENT_BINARY_DIR@/wikitext/templates/Primitive Components Table", "w")
article.write("""<!-- Machine-generated file, do not edit by hand! -->\n""")
article.write("""<table frame="box" rules="all" cellpadding="5">\n""")
article.write("""<tr><th>Primitive Type</th><th>Structure</th><th>Attributes</th></tr>\n""")

for i in range(len(primitive_types)):
	primitive_type = primitive_types[i]
	primitive_name = primitive_names[i]

	document = k3d.new_document()
	source = document.new_node("FrozenMesh")
	mesh = source.create_mesh()
	primitive_type.create(mesh)
	primitive = mesh.primitives()[0]

	article.write("""<tr>""")
	article.write("""<td>[[""" + primitive_name + """ Primitive]]</td>""")
	article.write("""<td>""" + ", ".join(primitive.structure().keys()) + """</td>""")
	article.write("""<td>""" + ", ".join(primitive.attributes().keys()) + """</td>""")
	article.write("""</tr>\n""")

	k3d.close_document(document)

article.write("""</table>\n""")
article.write("""<!-- Machine-generated file, do not edit by hand! -->\n""")

# Create the main article for each primitive type ...
for primitive_name in primitive_names:
	print """Creating main article for """ + primitive_name + """ Primitive ..."""
	article = file("@CMAKE_CURRENT_BINARY_DIR@/wikitext/articles/" + primitive_name + " Primitive", "w")
	article.write("""{{""" + primitive_name + """ Primitive}}\n""")

# Create the reference material for each primitive type ...
for i in range(len(primitive_types)):
	primitive_type = primitive_types[i]
	primitive_name = primitive_names[i]

	print """Creating reference documentation for """ + primitive_name + """ Primitive ..."""
	article = file("@CMAKE_CURRENT_BINARY_DIR@/wikitext/templates/" + primitive_name + " Primitive", "w")
	article.write("""<!-- Machine-generated file, do not edit by hand! -->\n""")

	article.write("""== Data Structure ==\n""")
	article.write("<graphviz>\n")

	document = k3d.new_document()
	source = document.new_node("FrozenMesh")
	mesh = source.create_mesh()
	primitive_type.create(mesh)

	need_points = False
	primitive = mesh.primitives()[0]
	for table_name in primitive.structure().keys():
		table = primitive.structure()[table_name]
		for array_name in table.keys():
			array = table[array_name]
			if array.get_metadata_value("k3d:domain") == "/points/indices()":
				need_points = True

	if need_points:
		mesh.create_points()
		mesh.create_point_selection()

	writer = document.new_node("GraphVizMeshWriter")
	document.set_dependency(writer.get_property("input_mesh"), source.get_property("output_mesh"))
	article.write(writer.output_string)

	k3d.close_document(document)

	article.write("</graphviz>\n")
	article.write("""<!-- Machine-generated file, do not edit by hand! -->\n""")

