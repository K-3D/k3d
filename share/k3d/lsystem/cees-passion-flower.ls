# --- Passie.ls  C.J.van der Mark jr.--------------------------
#
10    		            # recursion
20                          # angle
20                          # thickness
#
#---------------------------------------- axioms
#o                                # the flower
#L                                # a single leaf
#&(60)C                           # the middle stem of the plant
#k                                # middle flower branch
#b                                # the side branches of the plant, with flower
k"b>(55)"b>(60)'b>(65)b>(55)''b>(80)b   # the whole passieflora like plant
#
#---------------------------------------- rules
b=['Ao]
k=[''''C^(90)o]
A=$t(.2)F''[+(55)$L][-(55)$L]t(-.1)FA
C=$F'[+(55)$L][-(55)$L]FC
#
#
#----------------------------------------Plant Leaf
L=F&(30)&(10)cc[D][E]
D=[+D{.].X.}
E=[^(5)-E{.].X.}
X=^(5)ggggX
#
#----------------------------------------Flower leaf
d=[+d{.].x.}
h=[-h{.].x.}
x=gggx
#
#----------------------------------------Whole flower
o=[&!ce>(90)w>(90)w>(90)w>(90)w>vy>vy>vy>vy>vy>vy>vy>vy>vy>yv>vy>vy>vy>vy>vy>vy>vy>vy]
#----------------------------------------Flower stem
e=Z
#----------------------------------------Spikes in the flower
v=[ZZc"""""&(90){'''F!!''F!!''F!!''F}]
#----------------------------------------Flower female stem and male stems
w=[[sc??c'!!Ft(-.1)F<<''''!!^^^s]&(100)'(0.5)ZZZ[d][h]]
#----------------------------------------Colored flower leafs
y=[[ZZcccc&(80)'''{-ff+ff|-ff+ff}][ZZccccc&(60)'''{-f+f|-f+f}][ZZcccccc&(35)''''{-f+f|-f+f}]]
#----------------------------------------Singel male/female stem
s=[c^(8)!Ft(-.2)Ft(-.2)Ft(-.2)Fu]
#----------------------------------------Pollen holder on male stem
u=[???F]
#
F='(1.3)F'(.77)
f='(1.3)f'(.77)
@
