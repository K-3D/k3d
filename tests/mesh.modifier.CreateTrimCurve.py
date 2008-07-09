#python

import testing

setup = testing.setup_mesh_source_test("CreateTrimCurve")
grid = setup.document.new_node("NurbsGrid")
setup.document.set_dependency(setup.source.get_property("input_mesh"), grid.get_property("output_mesh"))

testing.mesh_comparison_to_reference(setup.document, setup.source.get_property("output_mesh"), "mesh.modifier.CreateTrimCurve", 1)

