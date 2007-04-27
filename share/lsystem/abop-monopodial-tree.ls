# --- L-System Parser/Mutator --- Lj Lapre ----------------------------------
#
# Monopodial skeleton
#
# Here is how to read the formulas on page 56 into
# lparser syntax. The values can be input from page 57,
# the strings colapsed and used.
#
# A = F[ &( a0 ) '( r2 ) !( wr ) BL  ] >( c ) '( r1 ) !( wr ) A
# B = F[ -( a2 ) '( r2 ) !( wr ) $CL ]        '( r1 ) !( wr ) C
# C = F[ +( a2 ) '( r2 ) !( wr ) $BL ]        '( r1 ) !( wr ) B
#
10
45
15
#
c(12)FFAL
#
A=F[&'(.8)!BL]>(137)'!(.9)A
B=F[-'(.8)!(.9)$CL]'!(.9)C
C=F[+'(.8)!(.9)$BL]'!(.9)B
#
L=~c(8){+(30)f(20)-(120)f(20)-(120)f(20)}
@
