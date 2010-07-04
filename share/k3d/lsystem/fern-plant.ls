# --- L-System Parser/Mutator --- Lj Lapre ----------------------------------
#
20                                      # recursion
20                                      # angle
15                                      # thickness
#
#---------------------------------------- axioms
#
#c(12)&(60)C                            # use this as axiom to test a leave
#c(12)b                                 # use this as axiom to test a branch
c(12)b>(60)b>(60)b>(60)b>(60)b>(60)b    # the whole thing
#
# ---------------------------------------- rules
#
b=[&(30)A]
#
A=~(7)$t(.1)F[+(40)C][-(40)C]!(.95)~(7)t(.1)FA
#
C=~(10)$tF[+(60)L][-(60)L]C
#
L=[~(15)c(4){-f+f+f-|-f+f}]
#
F='(1.3)F'(.77)
f='(1.3)f'(.77)
#
#~=_                                    # uncomment to remove random efx
@
