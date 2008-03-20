#python

try:
  from OpenGL.GL import *

except:
  print "Couldn't load OpenGL module"

glDisable(GL_LIGHTING)

glMatrixMode(GL_MODELVIEW)
glPushMatrix()
glLoadIdentity()
  
glMatrixMode(GL_PROJECTION)
glPushMatrix()
glLoadIdentity()
glOrtho(-1, 1, -1, 1, 0, 10000)

glBegin(GL_POLYGON)
glColor3d(0.9, 0.9, 0.9)
glVertex3f(-1, 1, -9999)
glVertex3f(1, 1, -9999)
glColor3d(0.3, 0.3, 0.3)
glVertex3f(1, -1, -9999)
glVertex3f(-1, -1, -9999)
glEnd()

glPopMatrix()
glMatrixMode(GL_MODELVIEW)
glPopMatrix()
