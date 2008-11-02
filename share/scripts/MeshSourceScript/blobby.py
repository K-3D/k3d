#python

import k3d
k3d.check_node_environment(locals(), "MeshSourceScript")

blobby = k3d.blobby.create(Output)
Cs = blobby.varying_data().create("Cs", "k3d::color")

# Add four ellipsoids to the blobby ...
ellipsoids = [k3d.point3(-1, 0, 1), k3d.point3(1, 0, 1), k3d.point3(1, 0, -1), k3d.point3(-1, 0, -1)]

blobby.first_primitives().append(len(blobby.primitives()))
blobby.primitive_counts().append(len(ellipsoids) + 1)
blobby.first_operators().append(len(blobby.operators()))
blobby.operator_counts().append(1)
blobby.materials().append(k3d.dynamic_cast(Document.get_node("Material"), "imaterial"))

for center in ellipsoids:
	blobby.primitives().append(k3d.blobby.primitive_type.ELLIPSOID)
	blobby.primitive_first_floats().append(len(blobby.floats()))
	blobby.primitive_float_counts().append(16)
	for i in (k3d.translate3(center) * k3d.scale3(1, 1, 1)).column_major_list():
		blobby.floats().append(i)

# Add a segment to the blobby ...
blobby.primitives().append(k3d.blobby.primitive_type.SEGMENT)
blobby.primitive_first_floats().append(len(blobby.floats()))
blobby.primitive_float_counts().append(23)
blobby.floats().append(-1)
blobby.floats().append(0)
blobby.floats().append(0)
blobby.floats().append(1)
blobby.floats().append(0)
blobby.floats().append(0)
blobby.floats().append(1)
for i in k3d.identity3().column_major_list():
	blobby.floats().append(i)

# Assign colors to each ellipsoid and segment ...
Cs.assign([k3d.color(1, 0, 0), k3d.color(0, 1, 0), k3d.color(0, 0, 1), k3d.color(1, 1, 0), k3d.color(1, 1, 1)])

# Add the ellipsoids and segment together to produce a surface ...
blobby.operators().append(k3d.blobby.operator_type.ADD)
blobby.operator_first_operands().append(len(blobby.operands()))
blobby.operator_operand_counts().append(6)
blobby.operands().assign([5, 0, 1, 2, 3, 4])

