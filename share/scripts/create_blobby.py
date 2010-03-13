#python

import k3d

context.document.start_change_set()
try:
	# Create a FrozenMesh node to act as a mesh source ...
	frozen_mesh = k3d.plugin.create("FrozenMesh", context.document)
	frozen_mesh.name = "Blobby"

	# Create a mesh ...
	mesh = frozen_mesh.create_mesh()

	# Create a blobby primitive ...
	blobby = k3d.blobby.create(mesh)

	# Create a custom attribute array to assign color values to each blobby primitive ...
	Cs = blobby.vertex_attributes().create("Cs", "k3d::color")

	# Add four ellipsoid primitives to the blobby ...
	blobby.materials().append(None)
	blobby.first_primitives().append(len(blobby.primitives()))
	blobby.primitive_counts().append(4)

	ellipsoids = [(-1, 0, 1), (1, 0, 1), (1, 0, -1), (-1, 0, -1)]
	for center in ellipsoids:
		blobby.primitives().append(k3d.blobby.primitive_type.ELLIPSOID)
		blobby.primitive_first_floats().append(len(blobby.floats()))
		blobby.primitive_float_counts().append(16)
		for i in (k3d.translate3(center) * k3d.scale3(1.7, 1, 1.7)).column_major_values():
			blobby.floats().append(i)

	# Assign a color to each primitive ...
	Cs.assign([k3d.color(1, 0, 0), k3d.color(0, 1, 0), k3d.color(0, 0, 1), k3d.color(1, 1, 1)])

	# Add a single operator to the blobby to add all four primitives together ...
	blobby.first_operators().append(len(blobby.operators()))
	blobby.operator_counts().append(1)
	blobby.operators().append(k3d.blobby.operator_type.ADD)
	blobby.operator_first_operands().append(len(blobby.operands()))
	blobby.operator_operand_counts().append(5)
	blobby.operands().assign([5, 0, 1, 2, 3]) # Operand count, followed by the index of each primitive to add

	# Connect the FrozenMesh to a MeshInstance to place it in the scene ...
	mesh_instance = k3d.plugin.create("MeshInstance", context.document)
	mesh_instance.name = "Blobby Instance"
	mesh_instance.gl_painter = k3d.node.lookup_one(context.document, "GL Default Painter")
	mesh_instance.ri_painter = k3d.node.lookup_one(context.document, "RenderMan Default Painter")
	k3d.property.connect(context.document, frozen_mesh.get_property("output_mesh"), mesh_instance.get_property("input_mesh"))

	# Make the MeshInstance visible to render engines ...
	k3d.node.show(context.document, mesh_instance)

	context.document.finish_change_set("Create Test Blobby")

	k3d.ui().message("Note: not all RenderMan engines can render blobby geometry!")

except:
	context.document.cancel_change_set()
	raise

