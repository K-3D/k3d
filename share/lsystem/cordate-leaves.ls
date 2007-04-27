# --- L-System Parser/Mutator --- Lj Lapre ----------------------------------
18	# recursion depth
10	# angle
10	# thickness as % of length
#
#P					# one leave
[|FFFFFFFFFFF]P>(90)'P>(90)'P>(90)'P	# plant
#
P=[&(10)G[ccA][ccB][a][b]]
G=tFtFtFtFtFtFtFtFtFtFtFtFtF
#
A=[+A{.].C.}
B=[-B{.].C.}
C=tfC
#
a=[+a]d
b=[-b]d
d=tFd
@
