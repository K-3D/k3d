# --- L-System Parser/Mutator --- Lj Lapre ----------------------------------
12
5
30
#
c(4)FFS                                         # the actual tree
#
S=FFR>(60)R>(60)R>(60)R>(60)R>(60)R>(30)S       # six branches per segment
R=[Ba]                                          # a single branch
#
a=$tF[Cx]Fb                                     # see bop08.ls
b=$tF[Dy]Fa
#
B=&B                                            # angle increments
C=+C
D=-D
#
x=a                                             # apical elay
y=b
#
F='(1.25)F'(.8)                                 # elongation rate
@
