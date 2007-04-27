#python

import k3d

doc = k3d.application.new_document()

# Create a simple cubic curve source ...
source = doc.new_node("MeshSourceScript")
source.script = """#python

import k3d
from random import uniform, seed

seed(1234)

curve_count = 2

points = Output.create_points()
point_selection = Output.create_point_selection()

for i in range(curve_count):
	for j in range(4):
		points.append(k3d.point3(uniform(-1, 1), uniform(-1, 1), uniform(-1, 1)))
		point_selection.append(0)

groups = Output.create_cubic_curve_groups()

first_curves = groups.create_first_curves()
first_curves.append(0)

curve_counts = groups.create_curve_counts()
curve_counts.append(curve_count)

periodic_curves = groups.create_periodic_curves()
periodic_curves.append(False)

materials = groups.create_materials()
materials.append(None)

curve_first_points = groups.create_curve_first_points()
for i in range(curve_count):
	curve_first_points.append(i * 4)

curve_point_counts = groups.create_curve_point_counts()
for i in range(curve_count):
	curve_point_counts.append(4)

curve_selection = groups.create_curve_selection()
for i in range(curve_count):
	curve_selection.append(0)

curve_points = groups.create_curve_points()
for i in range(curve_count):
	for j in range(4):
		curve_points.append(i * 4 + j)
"""

# Select some geometry ...
selection = doc.new_node("SelectCubicCurveByNumber")
selection.index = 0;
doc.set_dependency(selection.get_property("input_mesh"), source.get_property("output_mesh"))

# Run the geometry through a conversion from mesh to legacy-mesh and back ...
convert = doc.new_node("LegacyMeshConversion")
doc.set_dependency(convert.get_property("input_mesh"), selection.get_property("output_mesh"))

# Compare the original to the conversion ...
diff = doc.new_node("MeshDiff")
diff.add_user_property("k3d::dev::mesh*", "input_a", "InputA", "First input mesh")
diff.add_user_property("k3d::dev::mesh*", "input_b", "InputB", "Second input mesh")

doc.set_dependency(diff.get_property("input_a"), selection.get_property("output_mesh"))
doc.set_dependency(diff.get_property("input_b"), convert.get_property("output_mesh"))

if not diff.equal:
	print "source " + repr(selection.output_mesh)
	print "converted " + repr(convert.output_mesh)
	raise Exception("Converted mesh differs")

