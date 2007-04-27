# --- L-System Parser/Mutator --- Lj Lapre ----------------------------------
#
#---------------------------------------------------------------------------
#
# This file is to show the starting orientation of the turtle
# and the basic directions. The colors are as they will appear in
# the lviewer. This file will generate three blocks. Red is the current
# forward direction, yellow is the current left direction and green
# is the current up direction.
#
# By replacing rule 2 with the one of the following alternatives
# you can get a better grip on the turtle movement through space :
#
# T=^f			# pitch up and go forward
# T=&f			# pitch down and go forward
# T=+f			# turn left and go forward
# T=-f			# turn right and go forward
# T=>f			# roll right (= clockwise) and go forward
# T=<f			# roll left (= counter clockwise) and go forward
#
# Run these alternatives at a recursion level of 50.
#
# T=^+>f		# cylindrical motion
# T=+;'fff		# spiral motion
# T=+:f			# 'inverse' spiral motion
#
#---------------------------------------------------------------------------
#
5			# recursion level
20			# basic angle
5			# thickness
&(90)s			#0 axiom
s=aTs			#1 do axis, change orientation and recurse
T=f			#2 go forward (REPLACE THIS RULE)
a=[ABC]			#3 draw an axis
A=[c(2)Z]		#4 forward = red
B=[c(3)+(90)Z]		#5 left    = yellow
C=[c(4)^(90)Z]		#6 up      = green
#
#---------------------------------------------------------------------------
#
@			# end of ls

