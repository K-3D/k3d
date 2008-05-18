#python

import k3d

doc = k3d.new_document()

source = doc.new_node("PolyCube")
profiler = doc.new_node("PipelineProfiler")

# At this point the profiler should be empty ...
if len(profiler.records) != 0:
	raise Exception("expected zero profile records")

# Force execution of our source by reading its output ...
source.output_mesh

# At this point we expect the profiler to contain a single record ...
if len(profiler.records) != 1:
	raise Exception("expected one profile record")

# See what we've got ...
print profiler.records

