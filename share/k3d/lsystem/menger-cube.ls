# --- L-System Parser/Mutator --- Lj Lapre ----------------------------------
#
# This ls produces menger cubes. Recursion level 0 will produce a
# 0th order cube, 2 a 1th, 4 a 3th, and 6 a 4th. Level 8 will
# produce a 5th order menger with a total off 960.000 polygons !
#
# Change F= ... to F=[~(5)"(.333333)[-f+&f^B]]f add randomness.
#
# One can replace B= ... with a number of alternatives:
#
# B=[fz+zF]^f&[FFF|z|+zFF|z|+zFF|z|+zF]^f&[fz+zF]
# B=[fz+zF]^f&[FFF|z|+zFF|z|+zFF|z|+zF|z|+zF]^f&[fz+zF]
# B=[FFF|z|+zFF|z|+zFF|z|+zF]^f&[FfF|z|+zfF|z|+zfF]^f&[FFF|z|+zFF|z|+zFF|z|+zF]
#
4
90
142.857
#
F
#
F=["(.333333)[-f+&f^B]]f
#
B=[FFF|z|+zFF|z|+zFF|z|+zF]^f&[FfF|z|+zfF|z|+zfF]^f&[FFF|z|+zFF|z|+zFF|z|+zF]
@
