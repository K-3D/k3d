#python

from OpenGL.GL import *

points = Mesh.points()
if points:
	glDisable(GL_LIGHTING)

	glEnable(GL_POINT_SMOOTH)
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST)

	glEnable(GL_BLEND)
	glBlendFunc(GL_ONE, GL_ONE)

	glPointSize(3)
	glColor3d(1, 1, 1)

	glBegin(GL_POINTS)
	for point in points:
		glVertex3d(point[0], point[1], point[2])
	glEnd()

	glPointSize(7)
	glColor3d(0.05, 0.05, 1)

	glBegin(GL_POINTS)
	for point in points:
		glVertex3d(point[0], point[1], point[2])
	glEnd()
