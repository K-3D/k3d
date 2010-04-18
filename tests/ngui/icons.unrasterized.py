#python
import k3d
import glob
import os
import testing

scalable_icons = []
for icon in glob.glob(str(k3d.share_path() / k3d.filesystem.generic_path("ngui/scalable/*.svg"))):
	scalable_icons.append(os.path.splitext(os.path.basename(icon))[0])

rasterized_icons = []
for icon in glob.glob(str(k3d.share_path() / k3d.filesystem.generic_path("ngui/rasterized/*.png"))):
	rasterized_icons.append(os.path.splitext(os.path.basename(icon))[0])

scalable_icons.sort()
rasterized_icons.sort()

unrasterized = []
for icon in scalable_icons:
	if icon not in rasterized_icons:
		unrasterized.append(icon)

if len(unrasterized):
	print "\nScalable icons without corresponding rasterized icons:\n"
	print str("\n").join(unrasterized)
	raise Exception("Found scalable icons without corresponding rasterized icons.")

