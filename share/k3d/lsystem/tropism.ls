# --- L-System Parser/Mutator --- Lj Lapre ----------------------------------
#
# This file is to show the effect of gravity on a form.
# By the use of the new tropism command. First try it
# with the current rules. Then try C=tFC for rule 3.
#                                    ^
#
36                      # recursion
10                      # basic angle
100                     # thickness
#
A                       # axiom
#
A=B>(10)A               # rule 1 : add a branch and roll 10 deg right
B=[&(45)C]              # rule 2 : pitch down 45 deg and start a branch
C=FC                    # rule 3 : expand a branch with a forward element
@                       # end of ls
