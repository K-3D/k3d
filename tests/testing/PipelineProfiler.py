#python

import k3d

# Setup a simple pipeline ...
doc = k3d.new_document()
source = k3d.plugin.create("PolyCube", doc)
modifier = k3d.plugin.create("ScalePoints", doc)
doc.set_dependency(modifier.get_property("input_mesh"), source.get_property("output_mesh"))

# Create a profiler ...
profiler = k3d.plugin.create("PipelineProfiler", doc)

# At this point the profiler should be empty ...
if len(profiler.records) != 0:
	raise Exception("expected zero profile records")

# Force execution of the pipeline ...
modifier.output_mesh

# At this point we expect the profiler to contain two records (one for the source and one for the modifier) ...
if len(profiler.records) != 2:
	raise Exception("expected two profile records")

# See what we've got ...
print profiler.records

