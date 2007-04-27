#------Lparser/Mutator------C.J.van der Mark--------
#-- This file produces the 3D Sierpinski gasket.----
#-- Only uneven rec.levels will produce objects.----
7
30
1
#
#C  							# 3D Sierpinsky gasket
#
# --- plug in AXIOM lines for new forms --- Martin Higgs ---
#
#C|C                                               	# diamond
#C|>>cC|>>ccC|>>cccC|>>ccccC|>>cccccC              	# Rubric snake
#C>>cC>>ccC>>cccC>>ccccC>>cccccC                   	# hexagon
#C'>>|C'>>|C'>>|C'>>|C'>>|C                        	# snake 1
#C'|>>C'|>>C'|>>C|'>>C'|>>C                        	# snake 2
C|C'>>|cC'>>|C'>>|C'>>|C'>>|C'>>|C'>>|C'>>|C'>>|C 	# snake 3
#C'>>C'>>C'>>C'>>C''>>C                            	# ammonite 1
#Ce<<Ce<<Ce<<Ce<<Ce<<Ce<<Ce<<Ce<<Ce<<Ce<<C         	# ammonite 2
#e='(.885)                                         	#    "
#C<<C<<C<<C<<C<<C                                  	# star
#
A={[+f++++f++++f]}
B=[[^(19.5)A^(70)A]>>&(19.5)A^(19.5)---&&&&f&&&&<(19.5)-A]["(.5)C]+++f---
C=[BB+++^^^^f---B'(0.82)f'(1.22)---^'(0.574)f'(1.74)+++>>>>>B]+++ff---
#
{=_
}=_
#
@
