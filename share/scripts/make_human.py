#python

import k3d
import locale

# Make sure the script works with all locales
# (especially when parsing floating point values from text)
locale.setlocale(locale.LC_ALL, '')

# Experimental
# Loads Make Human base mesh

def LoadBasePoints(BaseMeshFile):

	point_list = []

	file = open(BaseMeshFile)
	vertexData = file.readline()
	while vertexData:
		coords = vertexData.split(',')
		vx, vy, vz = float(coords[0]), float(coords[1]), float(coords[2])

		p = vx, vy, vz
		point_list.append(p)

		vertexData = file.readline()

	file.close()

	return point_list


def CreateBaseMesh(Document, BaseVertices, BaseFacesFile):
	
	Document.start_change_set()
	try:
		material = Document.new_node("RenderManMaterial")
		material.name = "MakeHuman Material"

		frozen_mesh = Document.new_node("FrozenMesh");
		frozen_mesh.name = "MakeHuman";

		mesh = frozen_mesh.new_mesh();

		# Create points
		for i in range(len(BaseVertices)):
			mesh.new_point(BaseVertices[i])
		points = mesh.points

		# Create faces
		polyhedron = mesh.new_polyhedron()

		file = open(BaseFacesFile)
		faceData = file.readline()
		while faceData:
			face_indices = []
			for idx in faceData.split(','):
				face_indices.append(long(idx))

			edges = []
			for idx in face_indices:
				edges.append(polyhedron.new_edge(points[idx]))

			for i in range(len(edges)):
				edges[i].face_clockwise = edges[(i+1) % len(edges)]

			face = polyhedron.new_face(edges[0])
			face.material = material
	
			faceData = file.readline()
	
		file.close()

		mesh_instance = Document.new_node("MeshInstance")
		mesh_instance.name = "MakeHuman Instance"
		Document.set_dependency(mesh_instance.get_property("input_mesh"), frozen_mesh.get_property("output_mesh"))

		Document.finish_change_set("Load MakeHuman Mesh")

	except:
		Document.cancel_change_set()
		raise
	
	return mesh


def Morph(BaseMesh, BaseVertices, MorphPointsFile, MorphValue):

	file = open(MorphPointsFile)
	morphData = file.readline()
	while morphData:
		if morphData.find("#") == -1 and morphData.find(",") != -1:
			vals = morphData.split(',')
			index, vx, vy, vz = long(vals[0]), float(vals[1]), float(vals[2]), float(vals[3])

			old_v0, old_v1, old_v2 = BaseMesh.get_point_position(index)

			v0 = vx
			v1 = vy
			v2 = vz

			new_v0 = old_v0 + MorphValue * (v0 - BaseVertices[index][0])
			new_v1 = old_v1 + MorphValue * (v1 - BaseVertices[index][1])
			new_v2 = old_v2 + MorphValue * (v2 - BaseVertices[index][2])

			p = new_v0, new_v1, new_v2
			BaseMesh.set_point_position(index, p)

		morphData = file.readline()

base_mesh_file = "/home/romain/targets/base.mesh"
base_faces_file = "/home/romain/targets/faces.mesh"
morph_points_file = "~/MH/targets/head/brain-head_brain.target"

base_vertices = LoadBasePoints(base_mesh_file)
base_mesh = CreateBaseMesh(Document, base_vertices, base_faces_file)
#Morph(base_mesh, base_vertices, morph_points_file, 0.5)


