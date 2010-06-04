#python
import k3d
import glob
import os
import testing

factories = []
for factory in k3d.plugin.factory.lookup():
	factories.append(factory.name())

scalable_icons = []
for icon in glob.glob(str(k3d.share_path() / k3d.filesystem.generic_path("ngui/scalable/*.svg"))):
	scalable_icons.append(os.path.splitext(os.path.basename(icon))[0])

factories.sort()
scalable_icons.sort()

missing_icons = []
for factory in factories:
	if factory not in scalable_icons:
		missing_icons.append(factory)

if len(missing_icons):
	print "Factories without icons:\n"
	print str("\n").join(missing_icons)
	raise Exception("Found plugin factories without icons.")

