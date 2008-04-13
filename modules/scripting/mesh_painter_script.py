#python
from OpenGL.GL import *

glPushAttrib(GL_ALL_ATTRIB_BITS)
glDisable(GL_LIGHTING)
glPointSize(5)
glColor3d(0, 0, 1)

glBegin(GL_POINTS)
for point in Mesh.points():
	glVertex3d(point[0], point[1], point[2])
glEnd()

glPopAttrib()

