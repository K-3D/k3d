#python

# k3d:plugin-class="document"
# k3d:plugin-type="NullOutputScript"
# k3d:plugin-name="FPSStatistics"
# k3d:plugin-description="Records statistics about render engine FPS"

from xml.dom.minidom import getDOMImplementation
import k3d

#Add the required user properties
if not Node.has_property("frame_time"):
	Node.add_user_property("k3d::double_t", "frame_time", "Frame Time", "Time for the last render")
if not Node.has_property("total_time"):
	Node.add_user_property("k3d::double_t", "total_time", "Total Time", "Sum of all render times")
if not Node.has_property("frame_count"):
	Node.add_user_property("k3d::double_t", "frame_count", "Frame Count", "Number of frames rendered")
if not Node.has_property("avg_fps"):
	Node.add_user_property("k3d::double_t", "avg_fps", "Average FPS", "Average frames per second")

if Node.total_time == 0:
	Node.frame_count = 0
	Node.avg_fps = 0

if Node.frame_time > 0:
	Node.total_time += Node.frame_time
	Node.frame_count += 1

if Node.total_time > 0:
	Node.avg_fps = Node.frame_count / Node.total_time
