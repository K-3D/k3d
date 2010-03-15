#!/usr/bin/python

import re
import urllib

# For each polyhedron on polyhedra.org ...
index_file = open("polyhedra.txt", "w")
for index in range(142):
	print "Loading", index
	# Extract metadata from the per-polyhedron HTML page ...
	metadata = urllib.urlopen("http://polyhedra.org/poly/show/" + str(index)).read()
	match = re.search("<h2>(.*)</h2>", metadata);
	if not match:
		continue

	label = match.group(1)
	name = label.lower().replace(" ", "_")

	data = urllib.urlopen("http://polyhedra.org/poly/geometry/" + str(index) + ".xml")
	if data.getcode() != 200:
		continue

	open(name + ".xml", "w").write(data.read())
	index_file.write(name + " " + label + "\n")

