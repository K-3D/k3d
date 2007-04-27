# --- L-System Parser/Mutator --- Lj Lapre ----------------------------------
18					# recursion level
10					# basic angle
200					# starting thickness
C					# axiom
#-------------------------------------- Creature
C=LBW
#-------------------------------------- Body
B=[[''aH]|[g]]
a=Fs+;'a 				# upper part
g=Ft+;'g				# lower part
s=[::cc!!!!&&[FFcccZ]^^^^FFcccZ]	# upper spikes
t=[c!!!!&[FF]^^FF]			# lower spikes
#-------------------------------------- Lungs
L=O					# 8 recursions delay
O=P
P=Q
Q=R
R=U
U=X
X=Y
Y=V
V=[cc!!!&&&&&&&&&[Zp]|[Zp]]
p=h>>>>>>>>>>>>h>>>>>>>>>>>>h
h=[++++!F'''p]
#-------------------------------------- Head
H=[cccci[>>>>>dcFFF][<<<<<ecFFF]]
d=Z!&Z!&:'d				# left
e=Z!^Z!^:'e				# right
i=-:"i
#-------------------------------------- Wing
W=[%[!!cb][<<<!!cb][>>>!!cb]]
b=Fl!+Fl+;'b				# arc
l=[-cc{--z++z++z--|--z++z++z}]
#-------------------------------------- End
@
