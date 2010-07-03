# --- L-System Parser/Mutator --- Lj Lapre ----------------------------------
18					# recursion level
10					# basic angle
80					# starting thickness
%C					# axiom
#-------------------------------------- Creature
C=%LABHT
#-------------------------------------- Body
B=[???????zZZZ]
#-------------------------------------- Arms
A=[zf&&&&&&&&&"""[pj>>>>>>>>>S]|[qj<<<<<<<<<S]]
p=Ft+:'p				# left
q=Ft-:'q				# right
j=;"j
#-------------------------------------- Scorpers
S=['''!!r%^^r]
r=[Z?Z?Zu]				# jaw assembly
u=Z[co]!'^::u				# jaw and tooth
#-------------------------------------- Legs
L=[c">>>>[s]>>>>[s]>>>>[s]>>>[s]>>>>>>[s]>>>[s]>>>>[s]>>>>[s]]
s=+++li+++dccc??""FF			# down legs
l=Ft+:'l				# upper leg
d=FFF'd					# lower leg
i=;i
#-------------------------------------- Head
H=["">>>>>>>>>&&&&&&??FFFF!![???h][???k]%^^[??k]]
k=[Z?Z?Za]				# jaw assembly
a=Z[co]!'a				# jaw
o=&&&&&&&&&!!!!!F			# teeth
h=[^^gcccc"Z][^^ecccc"Z]		# eyes
g=z!+z!+z!+z				# left eye
e=z!-z!-z!-z				# right eye
#-------------------------------------- Tail
T=[f---------??""""""mccccF]
m=Ft+;'m				# curl
t=['::c!!!&&[f<<<n]^^^^f>>>n]		# spikes
n=[c{--z++z++z--|--z++z++z}]		# leaf
#-------------------------------------- End
@
