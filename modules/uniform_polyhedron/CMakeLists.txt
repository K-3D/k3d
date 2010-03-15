FILE(GLOB POLYHEDRA RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} data/*.xml)
LIST(SORT POLYHEDRA)
FOREACH(POLYHEDRON ${POLYHEDRA})
	K3D_COMPILE_RESOURCE(RESOURCES ${POLYHEDRON} "/module/uniform_polyhedron/${POLYHEDRON}")
ENDFOREACH()

K3D_COMPILE_RESOURCE(RESOURCES data/polyhedra.txt "/module/uniform_polyhedron/data/polyhedra.txt")

K3D_BUILD_MODULE(k3d-uniform-polyhedron EXTRA_SOURCES ${RESOURCES})
K3D_CREATE_MODULE_PROXY(k3d-uniform-polyhedron)
TARGET_LINK_LIBRARIES(k3d-uniform-polyhedron k3dsdk-resource)

