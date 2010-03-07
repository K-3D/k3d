#python

import k3d
import testing
import benchmarking

document = k3d.new_document()

sphere = k3d.plugin.create("PolySphere", document)
torus = k3d.plugin.create("PolyTorus", document)

cgal_boolean = k3d.plugin.create("CGALBoolean", document)
cgal_boolean.create_property("k3d::mesh*", "input_1", "Input 1", "")
cgal_boolean.create_property("k3d::mesh*", "input_2", "Input 2", "")

document.set_dependency(cgal_boolean.get_property("input_1"), sphere.get_property("output_mesh"))
document.set_dependency(cgal_boolean.get_property("input_2"), torus.get_property("output_mesh"))

profiler = k3d.plugin.create("PipelineProfiler", document)

testing.require_valid_mesh(document, cgal_boolean.get_property("output_mesh"))
testing.require_similar_mesh(document, cgal_boolean.get_property("output_mesh"), "mesh.modifier.CGALBoolean.benchmark", 1)
benchmarking.print_profiler_records(profiler.records)
print """<DartMeasurement name="Total Boolean Time" type="numeric/float">""" + str(benchmarking.total_profiler_time(profiler.records)) + """</DartMeasurement>"""
