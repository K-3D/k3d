#python

try:
  from OpenGL.GL import *

  glDisable(GL_LIGHTING)
  glColor3f(1, 1, 0)
  glBegin(GL_POLYGON)
  glVertex3f(-1, -1, 0)
  glVertex3f(1, -1, 0)
  glVertex3f(1, 1, 0)
  glVertex3f(-1, 1, 0)
  glEnd()

except:
  print "Couldn't load OpenGL module"

