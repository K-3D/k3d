#python

import k3d
import sys
import testing

# Create a mesh source and our test modifier ...
setup = testing.setup_mesh_modifier_test("PolyGrid", "ScalePoints")
setup.source.rows = 100
setup.source.columns = 100

selection = k3d.deselect_all()
selection.points = k3d.component_select_all()
setup.modifier.mesh_selection = selection

profiler = setup.document.new_node("PipelineProfiler")

thread_count = 2

grain_sizes = []
timings = []

for grain_size in range(5000, 100000, 5000):
	k3d.parallel.set_grain_size(grain_size)
	k3d.parallel.set_thread_count(thread_count)
	setup.modifier.x = 1.0
	setup.modifier.x = 2.0

	mesh = setup.modifier.output_mesh

	for (node, timing) in profiler.records.items():
		if node.factory().name() == "ScalePoints":
			grain_sizes.append(grain_size)
			timings.append(timing["Update Mesh"])

print """<DartMeasurement name="Thread Count" type="numeric/integer">""", thread_count, """</DartMeasurement>"""

print """<DartMeasurement name="Timing" type="text/html"><![CDATA[
<table>
<tr><th>Grain Size</th><th>Time (Seconds)</th></tr>"""

for i in range(len(grain_sizes)):
	print """<tr><td>""", grain_sizes[i], """</td><td>""", timings[i], """</td></tr>"""

print """
</table>
]]></DartMeasurement>"""

#chart_string = "http://chart.apis.google.com/chart?chs=250x250&cht=lxy&chd=t:40,60,80|20,30,70&chds=0,80,0,70&chxt=x,y&chxr=0,0,80|1,0.70&chtt=Grain+Size+vs+Execution+Time"

chart_string = ""
chart_string += "http://chart.apis.google.com/chart"
chart_string += "?chs=250x250"
chart_string += "&amp;cht=lxy"

chart_string += "&amp;chd=t:"
for i in range(len(grain_sizes)):
	if i:
		chart_string += ","
	chart_string += str(grain_sizes[i])
chart_string += "|"
for i in range(len(timings)):
	if i:
		chart_string += ","
	chart_string += str(timings[i])

chart_string += "&amp;chds=0,"
chart_string += str(max(grain_sizes))
chart_string += ",0,"
chart_string += str(max(timings))

chart_string += "&amp;chxt=x,y"

chart_string += "&amp;chxr=0,0,"
chart_string += str(max(grain_sizes))
chart_string += "|1,0,"
chart_string += str(max(timings))

chart_string += "&amp;chtt=Grain+Size+vs+Execution+Time"

print """<DartMeasurement name="Timing Graph" type="text/html"><![CDATA[<img src=\"""" + chart_string + """"/>]]></DartMeasurement>"""

sys.stdout.flush()

