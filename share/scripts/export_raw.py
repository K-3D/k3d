#python

import k3d
import sys

def dump_node(Node, File):
	try:
		mesh = Node.output_mesh
	except:
		return

	File.write("# " + Node.name + "\n")

	for polyhedron in mesh.polyhedra:
		File.write(str(len(mesh.points)) + " " + str(len(polyhedron.faces)) + "\n")

		point_map = {}

		for point in mesh.points:
			point_map[point] = len(point_map)
			position = point.position
			File.write(str(position[0]) + " " + str(position[1]) + " " + str(position[2]) + "\n")

		for face in polyhedron.faces:
			edge = face.first_edge
			while edge:
				File.write(str(point_map[edge.vertex]) + " ")
				edge = edge.face_clockwise
				if edge == face.first_edge:
					break
			File.write("\n")

	File.write("\n")

def export():
	if not k3d.Document:
		k3d.Application.ui.error_message("No document to export!")
		return

	filepath = k3d.Application.ui.get_file_path("write", "documents", "Export RAW:", "")
	if filepath == "":
		return

	try:
		exportfile = open(filepath, "w")
	except IOError:
		k3d.Application.ui.error_message("Couldn't open file!")
		return

	for node in k3d.Document.nodes():
		dump_node(node, exportfile)

	exportfile.close()

export()

