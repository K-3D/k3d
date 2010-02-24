#python

import sys
import testing

# Floating-point measurement
print """<DartMeasurement name="Float" type="numeric/float">0.12</DartMeasurement>"""

# Integer measurement
print """<DartMeasurement name="Integer" type="numeric/integer">12</DartMeasurement>"""

# String measurement
print """<DartMeasurement name="String" type="text/string">A simple message</DartMeasurement>"""

# Longer text measurement
print """<DartMeasurement name="Text" type="text/text">A longer message</DartMeasurement>"""

# An HTML table measurement
print """<DartMeasurement name="HTML Table" type="text/html"><![CDATA[
<table>
<tr><th>Time</th><th>Value 1</th><th>Value 2</th></tr>
<tr><td>1</td><td>1.2</td><td>1.6</td></tr>
<tr><td>2</td><td>1.3</td><td>2.4</td></tr>
<tr><td>3</td><td>1.4</td><td>4.5</td></tr>
<tr><td>4</td><td>1.5</td><td>8.9</td></tr>
</table>
]]></DartMeasurement>"""

# An HTML Google Chart measurement
print """<DartMeasurement name="HTML Chart" type="text/html"><![CDATA[
<img src="http://chart.apis.google.com/chart?cht=p3&chd=t:60,40&chs=250x100&chl=Hello|World">
]]></DartMeasurement>"""

# An arbitrary XML measurement
print """<DartMeasurement name="XML" type="text/xml"><![CDATA[
<foo><bar a="b"><baz c="d"/></bar></foo>
]]></DartMeasurement>"""

# JPEG image measurement
print """<DartMeasurementFile name="JPEG Image" type="image/jpeg">""" + testing.source_path() + """/bitmaps/test_rgb_8.jpg</DartMeasurementFile>"""

# PNG image measurement
print """<DartMeasurementFile name="PNG Image" type="image/png">""" + testing.source_path() + """/bitmaps/test_rgb_8.png</DartMeasurementFile>"""

sys.stdout.flush()

