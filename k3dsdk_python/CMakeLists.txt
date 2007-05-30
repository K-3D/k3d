PROJECT(k3dsdk-python)

INCLUDE_DIRECTORIES(${k3d_SOURCE_DIR})
INCLUDE_DIRECTORIES(${k3d_SOURCE_DIR}/gil)
INCLUDE_DIRECTORIES(${K3D_BOOST_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${K3D_GLIBMM_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${K3D_SIGC_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})

LINK_DIRECTORIES(${K3D_BOOST_LIB_DIRS})
LINK_DIRECTORIES(${K3D_SIGC_LIB_DIRS})

ADD_LIBRARY(k3dsdk-python-types SHARED
	angle_axis_python.cpp
	angle_axis_python.h
	bitmap_python.cpp
	bitmap_python.h
	bounding_box3_python.cpp
	bounding_box3_python.h
	color_python.cpp
	color_python.h
	euler_angles_python.cpp
	euler_angles_python.h
	matrix4_python.cpp
	matrix4_python.h
	normal3_python.cpp
	normal3_python.h
	point3_python.cpp
	point3_python.h
	point4_python.cpp
	point4_python.h
	ri_render_state_python.cpp
	ri_render_state_python.h
	uuid_python.cpp
	uuid_python.h
	vector3_python.cpp
	vector3_python.h
	)

TARGET_LINK_LIBRARIES(k3dsdk-python-types k3dsdk)
TARGET_LINK_LIBRARIES(k3dsdk-python-types ${K3D_BOOST_PYTHON_LIBS})
TARGET_LINK_LIBRARIES(k3dsdk-python-types ${PYTHON_LIBRARIES})

IF(NOT WIN32)
	TARGET_LINK_LIBRARIES(k3dsdk-python-types pthread)
ENDIF(NOT WIN32)

ADD_LIBRARY(k3dsdk-python SHARED
	any_python.cpp
	any_python.h
	array_python.cpp
	array_python.h
	const_array_python.cpp
	const_array_python.h
	const_named_arrays_python.cpp
	const_named_arrays_python.h
	dynamic_cast_python.cpp
	dynamic_cast_python.h
	icommand_node_python.cpp
	icommand_node_python.h
	idocument_python.cpp
	idocument_python.h
	imaterial_python.cpp
	imaterial_python.h
	imesh_storage_python.cpp
	imesh_storage_python.h
	inode_python.cpp
	inode_python.h
	interface_wrapper_python.h
	iplugin_factory_python.cpp
	iplugin_factory_python.h
	iproperty_collection_python.cpp
	iproperty_collection_python.h
	iproperty_python.cpp
	iproperty_python.h
	iunknown_python.cpp
	iunknown_python.h
	iuser_interface_python.cpp
	iuser_interface_python.h
	mesh_python.cpp
	mesh_python.h
	mesh_selection_python.cpp
	mesh_selection_python.h
	named_arrays_python.cpp
	named_arrays_python.h
	node_python.cpp
	node_python.h
	object_model_python.cpp
	object_model_python.h
	)

TARGET_LINK_LIBRARIES(k3dsdk-python k3dsdk-python-types)

INSTALL(TARGETS k3dsdk-python-types
	RUNTIME DESTINATION bin 
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)

INSTALL(TARGETS k3dsdk-python
	RUNTIME DESTINATION bin 
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)

