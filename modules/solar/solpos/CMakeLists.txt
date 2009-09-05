# Computes solar position using SOLPOS 2.0, developed by the National Renewable Energy Laboratory, http://www.nrel.gov
# See http://rredc.nrel.gov/solar/codes_algs/solpos/ for details.

PROJECT(solpos)

ADD_LIBRARY(k3d-solar-solpos SHARED solpos.cpp solpos.h)

INSTALL(TARGETS k3d-solar-solpos
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION ${K3D_LIBDIR}
	ARCHIVE DESTINATION ${K3D_LIBDIR}
	)

