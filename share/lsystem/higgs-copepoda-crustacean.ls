# --- L-System Parser/Mutator --- Martin Higgs-------------------------
# --- This file produces a crustacean of the subclass Copepoda.
15
20
80
#
A
#
A=[T][B]
#
# --- tail --------------------------------------------------------------
#
T=c(1)+(90)^(5)[F--[n]++[m]++[m]++[o]]|^(10)>(180)[F--[q]++[p]++[p]++[r]]
m=!!!F'^(5)F--c(3){f++f++f--|--f++f++f}         # tail feather m
n=!!!F'^(5)F'^(5)F--''c(3){f++f++f--|--f++f++f} # tail feather n
o=!!!F-c(3){f+f+f-|-f+f+f}     			# tail feather o
p=!!!F'&(5)F--c(3){f++f++f--|--f++f++f}         # tail feather p
q=!!!F'&(5)F'&(5)F--''c(3){f++f++f--|--f++f++f} # tail feather q
r=!!!F-c(3){f+f+f-|-f+f+f}     			# tail feather r
#
# --- body ------------------------------------------------------------
#
B=ccc'''''''??bFbFbFFbFbF[[D][E]]F'F'''F[[D][E]]F'F''F[[D][E]]FF'[X][Y]
b=^(1)?(1.3)
#
# --- legs -------------------------------------------------------
#
D=^^!!!!c(6)-(85)FF'F[---'(.275)W]dddddd!!!c(4)F+'F+'F+(10)'F'F'+F
E=^^!!!!c(6)+(85)FF'F[+++'(.275)Q]eeeeee!!!c(4)F-'F-'F+-10)'F'F'-F
d=+(10)'F[++++!!c(4)'''F''F''F''F]
e=-(10)'F[----!!c(4)'''F''F''F''F]
#
# --- antenna --------------------------------------------------------
#
X=^-(90)fc(2)!(.2)"+++F[H]FF[-H]xxx[W]xxx['W]xxx[+'(.5)W]xxxllllllll"s
x=-(10)F!(.96)
l=+(10)F!(.96)
Y=^+(90)fc(2)!(.2)"---F[I]FF[+I]lll[Q]lll['Q]lll[-'(.5)Q]lllxxxxxxxx"v
H=c(7)"""++!!!!FFF--c(3){f+f+f-|-f+f+f}
I=c(7)"""--!!!!FFFc(3){f+f+f-|-f+f+f}
s=c(7)!!!FF+FF+F'^(5)F--c(3)""{f++f++f--|--f++f++f}
v=c(7)!!!FF-FF-F'^(5)F--c(3)""{f++f++f--|--f++f++f}
W=+c(7)!!aaaaaaaaaaaaa
a=FF'-(5)
Q=-c(7)!!jjjjjjjjjjjjj
j=FF'+(5)
#
@



