# --- L-System Parser/Mutator --- Lj Lapre ----------------------------------
#
# Elaboration of example d on page 56
# with double the amount of branches
#
10
30
20
#
c(12)FAL
#
A=F[&'(.7)!BL]>(137)[&'(.6)!BL]>(137)'(.9)!(.9)A
B=F[-'(.7)!(.9)$CL]'(.9)!(.9)C
C=F[+'(.7)!(.9)$BL]'(.9)!(.9)B
#
L=~c(8){+(45)f(10)-(45)f(20)-(45)f(10)+(45)|+(45)f(10)-(45)f(20)-(45)f(10)}
@
