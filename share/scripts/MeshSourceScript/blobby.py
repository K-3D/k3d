#python

import k3d
k3d.check_node_environment(locals(), "MeshSourceScript")

ellipsoids = [k3d.point3(-1, 0, 1), k3d.point3(1, 0, 1), k3d.point3(1, 0, -1), k3d.point3(-1, 0, -1)]

blobbies = Output.create_blobbies()

first_primitives = blobbies.create_first_primitives()
first_primitives.append(0)

primitive_counts = blobbies.create_primitive_counts()
primitive_counts.append(len(ellipsoids) + 1)

first_operators = blobbies.create_first_operators()
first_operators.append(0)

operator_counts = blobbies.create_operator_counts()
operator_counts.append(1)

materials = blobbies.create_materials()
materials.append(k3d.dynamic_cast(Document.get_node("Material"), "imaterial"))

primitives = blobbies.create_primitives()
primitive_first_floats = blobbies.create_primitive_first_floats()
primitive_float_counts = blobbies.create_primitive_float_counts()
floats = blobbies.create_floats()

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

Cs = blobbies.writable_varying_data().create("Cs", "k3d::color")
Cs.assign([k3d.color(1, 0, 0), k3d.color(0, 1, 0), k3d.color(0, 0, 1), k3d.color(1, 1, 0), k3d.color(1, 1, 1)])


operators = blobbies.create_operators()
operator_first_operands = blobbies.create_operator_first_operands()
operator_operand_counts = blobbies.create_operator_operand_counts()
operands = blobbies.create_operands()

operators.append(k3d.operator_type.add)
operator_first_operands.append(len(operands))
operator_operand_counts.append(6)
operands.assign([5, 0, 1, 2, 3, 4])

