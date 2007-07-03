PROJECT(k3dsdk-sgi-tesselator)

FILE(GLOB HEADERS *.h)
SET(SOURCES
	dict.c
	dict.h
	dict-list.h
	geom.c
	geom.h
	gluos.h
	memalloc.c
	memalloc.h
	mesh.c
	mesh.h
	normal.c
	normal.h
	priorityq.c
	priorityq-heap.h
	priorityq-sort.h
	render.c
	render.h
	sweep.c
	sweep.h
	tess.c
	tess.h
	tessmono.c
	tessmono.h
	)


ADD_LIBRARY(k3dsdk-sgi-tesselator SHARED ${SOURCES})
K3D_GENERATE_DEF_FILE(k3dsdk-sgi-tesselator)

INSTALL(TARGETS k3dsdk-sgi-tesselator
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)

