#python

import k3d
import testing
import benchmarking

setup = testing.setup_mesh_modifier_test("PolySphere", "CatmullClark")
setup.modifier.level = 5

setup.modifier.mesh_selection = k3d.geometry.selection.create(1)

profiler = k3d.plugin.create("PipelineProfiler", setup.document)

testing.require_valid_mesh(setup.document, setup.modifier.get_property("output_mesh"))
#note: no comparison to reference, as the file is 97 MB!

benchmarking.print_profiler_records(profiler.records)
print """<DartMeasurement name="Total Time" type="numeric/float">""" + str(benchmarking.total_profiler_time(profiler.records)) + """</DartMeasurement>"""
