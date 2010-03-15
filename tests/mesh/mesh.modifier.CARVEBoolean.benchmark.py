#python

import k3d
import testing
import benchmarking

document = k3d.new_document()

sphere = k3d.plugin.create("PolySphere", document)
torus = k3d.plugin.create("PolyTorus", document)

carve_boolean = k3d.plugin.create("CARVEBoolean", document)
k3d.property.create(carve_boolean, "k3d::mesh*", "input_1", "Input 1", "")
k3d.property.create(carve_boolean, "k3d::mesh*", "input_2", "Input 2", "")

k3d.property.connect(document, sphere.get_property("output_mesh"), carve_boolean.get_property("input_1"))
k3d.property.connect(document, torus.get_property("output_mesh"), carve_boolean.get_property("input_2"))

profiler = k3d.plugin.create("PipelineProfiler", document)

testing.require_valid_mesh(document, carve_boolean.get_property("output_mesh"))
testing.require_similar_mesh(document, carve_boolean.get_property("output_mesh"), "mesh.modifier.CARVEBoolean.benchmark", 1)
benchmarking.print_profiler_records(profiler.records)
print """<DartMeasurement name="Total Boolean Time" type="numeric/float">""" + str(benchmarking.total_profiler_time(profiler.records)) + """</DartMeasurement>"""
