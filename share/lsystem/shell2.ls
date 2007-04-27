# --- L-System Parser/Mutator --- Lj Lapre ----------------------------------
19
10
10
#
>(20)PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP
#
P=S>(40)g'
S=[&(90)[cD][A]+(20)^(90)g'&(90)[B][cD]]
#
D=^FD                           # show stalks
#
A=[^gA{.].                      # put polygons on them
B=B^.g.}
@
