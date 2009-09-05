# Computes solar position using SOLPOS 2.0, developed by the National Renewable Energy Laboratory, http://www.nrel.gov
# See http://rredc.nrel.gov/solar/codes_algs/solpos/ for details.

PROJECT(solpos)

ADD_LIBRARY(k3d-solar-solpos STATIC solpos.cpp solpos.h)

