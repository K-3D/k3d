#python

from OpenGL.GL import *

points = Mesh.points()
if points:
	glDisable(GL_LIGHTING)

	glPointSize(1)
	glColor3d(1, 1, 1)

	glBegin(GL_POINTS)
	for point in points:
		glVertex3d(point[0], point[1], point[2])
	glEnd()

	glPointSize(3)
	glColor3d(0, 1, 0)

	glBegin(GL_POINTS)
	for point in points:
		glVertex3d(point[0], point[1], point[2])
	glEnd()


	glPointSize(5)
	glColor3d(0, 0.5, 0)

	glBegin(GL_POINTS)
	for point in points:
		glVertex3d(point[0], point[1], point[2])
	glEnd()

