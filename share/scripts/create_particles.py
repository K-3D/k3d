#python

import k3d
from random import uniform

context.document.start_change_set()
try:
	# Create a FrozenMesh node to act as a mesh source ...
	frozen_mesh = context.document.new_node("FrozenMesh")
	frozen_mesh.name = "Particles"

	# Create a mesh ...
	mesh = frozen_mesh.create_mesh()

	particle_count = 100
	size = 10

	# Add geometric points to the mesh ...
	points = mesh.create_points()
	point_selection = mesh.create_point_selection()
	for i in range(particle_count):
		points.append(k3d.point3(uniform(-size, size), uniform(-size, size), uniform(-size, size)))
		point_selection.append(0.0)

	# Create a particle primitive ...
	particles = k3d.particle.create(mesh)

	# Create a custom attribute array to control the widths of particles ...
	constantwidth = particles.constant_attributes().create("constantwidth", "k3d::double_t")

	# Create a custom attribute array to store color values for each particle ...
	Cs = particles.vertex_attributes().create("Cs", "k3d::color")

	# Add particles to the primitive ...
	particles.material().append(None)
	for i in range(particle_count):
		particles.points().append(i)
	
	constantwidth.append(0.5)

	# Assign a random color to each particle ...
	for i in range(particle_count):
		Cs.append(k3d.color(uniform(0, 1), uniform(0, 1), uniform(0, 1)))

	# Connect the FrozenMesh to a MeshInstance to place it in the scene ...
	mesh_instance = context.document.new_node("MeshInstance")
	mesh_instance.name = "Particle Instance"
	mesh_instance.gl_painter = context.document.get_node("GL Default Painter")
	mesh_instance.ri_painter = context.document.get_node("RenderMan Default Painter")

	context.document.set_dependency(mesh_instance.get_property("input_mesh"), frozen_mesh.get_property("output_mesh"))

	# Make the MeshInstance visible to render engines ...
	k3d.node.show(context.document, mesh_instance)

	context.document.finish_change_set("Create Point Group")

except:
	context.document.cancel_change_set()
	raise

