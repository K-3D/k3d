#python

import k3d

doc = Document
doc.start_change_set()
try:
	frozen_mesh = doc.new_node("FrozenMesh")
	frozen_mesh.name = "Blobby"

	mesh = k3d.dynamic_cast(frozen_mesh, "imesh_storage").reset_mesh()

	blobbies = mesh.create_blobbies()
	first_primitives = blobbies.create_first_primitives()
	primitive_counts = blobbies.create_primitive_counts()
	first_operators = blobbies.create_first_operators()
	operator_counts = blobbies.create_operator_counts()
	materials = blobbies.create_materials()
	primitives = blobbies.create_primitives()
	primitive_first_floats = blobbies.create_primitive_first_floats()
	primitive_float_counts = blobbies.create_primitive_float_counts()
	floats = blobbies.create_floats()
	Cs = blobbies.writable_varying_data().create_array("Cs", "k3d::color")
	operators = blobbies.create_operators()
	operator_first_operands = blobbies.create_operator_first_operands()
	operator_operand_counts = blobbies.create_operator_operand_counts()
	operands = blobbies.create_operands()

	ellipsoids = [k3d.point3(-1, 0, 1), k3d.point3(1, 0, 1), k3d.point3(1, 0, -1), k3d.point3(-1, 0, -1)]

	first_primitives.append(len(primitives))
	primitive_counts.append(len(ellipsoids) + 1)
	first_operators.append(len(operators))
	operator_counts.append(1)
	materials.append(k3d.dynamic_cast(Document.get_node("Material"), "imaterial"))

	for center in ellipsoids:
		primitives.append(k3d.primitive_type.ellipsoid)
		primitive_first_floats.append(len(floats))
		primitive_float_counts.append(16)
		for i in (k3d.translate3(center) * k3d.scale3(1, 1, 1)).column_major_list():
			floats.append(i)

	primitives.append(k3d.primitive_type.segment)
	primitive_first_floats.append(len(floats))
	primitive_float_counts.append(23)
	floats.append(-1)
	floats.append(0)
	floats.append(0)
	floats.append(1)
	floats.append(0)
	floats.append(0)
	floats.append(1)
	for i in k3d.identity3().column_major_list():
		floats.append(i)

	Cs.assign([k3d.color(1, 0, 0), k3d.color(0, 1, 0), k3d.color(0, 0, 1), k3d.color(1, 1, 0), k3d.color(1, 1, 1)])

	operators.append(k3d.operator_type.add)
	operator_first_operands.append(len(operands))
	operator_operand_counts.append(6)
	operands.assign([5, 0, 1, 2, 3, 4])

	mesh_instance = doc.new_node("MeshInstance")
	mesh_instance.name = "Blobby Instance"
	mesh_instance.gl_painter = doc.get_node("GL Default Painter")
	mesh_instance.ri_painter = doc.get_node("RenderMan Default Painter")
	doc.set_dependency(mesh_instance.get_property("input_mesh"), frozen_mesh.get_property("output_mesh"))

	doc.finish_change_set("Create Test Blobby")

	k3d.ui().message("Note: not all RenderMan engines can render blobby geometry!")

except:
	doc.cancel_change_set()
	raise

