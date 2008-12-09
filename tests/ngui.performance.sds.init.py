#python

# This test records the performance of the initialization of the new SDS method (emulated by a CatmullClark filter for now)

import k3d
import math
import testing
import benchmarking

document = k3d.documents()[0]

painter = document.new_node("VirtualOpenGLFacePainter")

mesh_instance = document.new_node("MeshInstance")
mesh_instance.name = "Test mesh instance"
mesh_instance.gl_painter = painter

source = document.new_node("PolyCube")
source.name = "Test source"
source.rows = 25
source.columns = 25
source.slices = 25
sds = document.new_node("CatmullClark")
sds.level = 2
sds.mesh_selection = k3d.mesh_selection.select_all()
k3d.ui().synchronize()

profiler = document.new_node("PipelineProfiler")
document.set_dependency(sds.get_property("input_mesh"), source.get_property("output_mesh"))
document.set_dependency(mesh_instance.get_property("input_mesh"), sds.get_property("output_mesh"))
k3d.ui().synchronize()

benchmarking.print_profiler_records(profiler.records)
print """<DartMeasurement name="SDS Time" type="numeric/float">""" + str(benchmarking.total_profiler_time(profiler.records)) + """</DartMeasurement>"""
