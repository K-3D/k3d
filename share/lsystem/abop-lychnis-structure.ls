# --- L-System Parser/Mutator --- Lj Lapre ----------------------------------
31
18
100
#
# axiom
#
c(12)N
#
# rules
#
N=FII[&(60)Y]>(90)[&(45)'(0.8)A]>(90)[&(60)Y]>(90)[&(45)'(0.8)D]FIK
#
# forms substitutes, leaves from bop01.ls
#
Y=[c(4){++l.--l.--l.++|++l.--l.--l.}]
l=g(20)l # leaves grow in size with recursion level
#
# forms substitutes, flowers from bop01.ls
#
K=[!c(2)FF>w>(72)w>(72)w>(72)w>(72)w]
w=[c(2)^!F][c(5)&(72){-(54)f(300)+(54)f(300)|-(54)f(300)+(54)f(300)}]
f=_  # removes all flower leaves except those of the last recursion
#
# 7 step delay for A(i)
#
A=B
B=C
C=D
D=E
E=G
G=H
H=N
#
# 10 delay for I(i)
#
I=FO
O=FP
P=FQ
Q=FR
R=FS
S=FT
T=FU
U=FV
V=FW
W=FX
X=_
#
@
