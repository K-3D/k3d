# --- L-System Parser/Mutator --- Lj Lapre ----------------------------------
#
# 'Trail off Trees'. This is a setup to get all development
#                    stages of a tree together in one image.
#
14
5
30
#
&(90)H
#
H=f(1500)TcH                                    # move forward and plant a tree
#
T=[^(90)FFS]                                    # the actual tree
S=FFR>(120)R>(120)R>(60)S                       # three branches per segment
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
