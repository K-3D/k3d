# --- L-System Parser/Mutator --- Lj Lapre ----------------------------------
#
# The elongation rule from page 130 of ABOP :
#
# F(a) : * -> F(a*R)
#
# translates into :
#
# F='(R)F'(1/R)
#
15
45
20
#
&(90)+(90)a
#
a=F[+cx]Fb
b=F[-cy]Fa
#			apical delay
x=a
y=b
#
F='(1.36)F'(.735)	# internode elongation rate
@
