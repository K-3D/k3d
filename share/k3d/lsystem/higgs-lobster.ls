# --- L-System Parser/Mutator --- Martin Higgs -----------
# --- This file produces a crustation like form.
13  # recursion
20  # angle
30  # thickness
A
A=|[B][[C][P]][M]['''&M][''''&&M] # --- The complete crustation ---
# --- body
B=[+(80)[aaaaa]][-(80)[bbbbb]]&'[B]
a=>-"[F'''c(14)Fc(5)D[c(4)'''''''{-f+f+f-|-f+f+f}]]
b=<+"[F'''c(14)Fc(5)E[c(4)'''''''{-f+f+f-|-f+f+f}]]
D=!!!!!''''''dddd
E=!!!!!''''''eeee
d=$tZ++Z-     		# tropism added to tendrils
e=$tZ--Z+          	#    "      "   "     "
#
# --- complete claw
M=^[[NZ][OZ]]
N=++<<'''^^???????!!!c(11)-(85)FF'F[---'(.275)W]nnnnnn
O=-->>'''^^???????!!!c(11)+(85)FF'F[+++'(.275)Q]oooooo
#
# --- claw teeth
n=+(10)'F[++++!!!c(10)'''F''F''F''F]
o=-(10)'F[----!!!c(10)'''F''F''F''F]
#
# --- claw whiskers
W=+c(7)&(7)!wwwwwwwwwwwww
w=+(10)FF'
Q=-c(7)&(7)!qqqqqqqqqqqqq
q=-(10)FF'
#
# --- antenna
C=+(65)'''''''''c(9)F>>>>>>&&&wlwlwlwl
P=-(65)'''''''''c(9)F<<<<<<&&&qsqsqsqs
r=c(3)[{-f+f+f-|-f+f+f}]
s=[+r][w'r]
l=[-r][q'r]
@
