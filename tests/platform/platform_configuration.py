#python

import platform
import sys

print """<DartMeasurement name="architecture" type="text/string">""" + str(platform.architecture()) + """</DartMeasurement>"""
print """<DartMeasurement name="machine" type="text/string">""" + str(platform.machine()) + """</DartMeasurement>"""
print """<DartMeasurement name="platform" type="text/string">""" + str(platform.platform()) + """</DartMeasurement>"""
print """<DartMeasurement name="processor" type="text/string">""" + str(platform.processor()) + """</DartMeasurement>"""
print """<DartMeasurement name="release" type="text/string">""" + str(platform.release()) + """</DartMeasurement>"""
print """<DartMeasurement name="system" type="text/string">""" + str(platform.system()) + """</DartMeasurement>"""
print """<DartMeasurement name="uname" type="text/string">""" + str(platform.uname()) + """</DartMeasurement>"""
print """<DartMeasurement name="version" type="text/string">""" + str(platform.version()) + """</DartMeasurement>"""

sys.stdout.flush()

