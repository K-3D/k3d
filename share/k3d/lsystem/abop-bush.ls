# --- L-System Parser/Mutator --- Lj Lapre ----------------------------------
7       # recursion depth
22.5    # angle
80      # thickness as % of length
#
c(12)A
#
A=[&FL!A]>>>>>[&FL!A]>>>>>>>[&FL!A]
F=S>>>>>F
S=FL
L=[c(8)^^{-f+f+f-|-f+f+f}]
@
