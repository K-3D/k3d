#python

import k3d

import glob
import sys

scripts = glob.glob(str(k3d.share_path()) + "/scripts/*.py")
scripts.sort()

failed_scripts = []

class dummyStream:
	def __init__(self): pass
	def write(self,data): pass
	def read(self,data): pass
	def flush(self): pass
	def close(self): pass

for script in scripts:
	k3d.log_info("Running " + script)

	old_streams = [sys.stdout, sys.stderr]
	sys.stdout,sys.stderr = dummyStream(),dummyStream()

	try:
		document = k3d.new_document()
		code = file(script).read()

		k3d.execute_script(code, {"Document": document})

	except:
		failed_scripts.append(script)

	sys.stdout,sys.stderr = old_streams

if len(failed_scripts):
	k3d.log_error("The following scripts FAILED:")
	for script in failed_scripts:
		k3d.log_error("        " + script + " (Failed)")

	sys.argv.append("K-3D Python Engine")
	raise ""

