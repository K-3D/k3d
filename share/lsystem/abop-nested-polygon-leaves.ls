# --- L-System Parser/Mutator --- Lj Lapre ----------------------------------
20
7
50
#
&(10)c(4)Dc(6)[p>(72)p>(72)p>(72)p>(72)p]c(7)E	# 5 petal elements on a branch
#
p=[&(90)[A]+(72)[B]]				# one petal
#
A=[^gA{.].					# first 'curved line'
B=B^.g.}					# second 'curved line'
#
D=t(.1)Ft(.1)FD					# branch
E=FE						# stem
@
