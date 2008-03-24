SET(K3D_OPENCASCADE_FOUND 0)

FIND_PATH(K3D_OPENCASCADE_INCLUDE_DIR TDocStd_Document.hxx
	/opt/opencascade/ros/inc
	DOC "Directory where the .hxx OpenCascade header files are located"
	)
MARK_AS_ADVANCED(K3D_OPENCASCADE_INCLUDE_DIR)

SET(K3D_OPENCASCADE_LIB_DIR /opt/opencascade/ros/Linux/lib CACHE PATH "Directory where the OpenCascade libraries are located")
MARK_AS_ADVANCED(K3D_OPENCASCADE_LIB_DIR)

IF(K3D_OPENCASCADE_INCLUDE_DIR)
	SET(K3D_OPENCASCADE_FOUND 1)
ENDIF(K3D_OPENCASCADE_INCLUDE_DIR)

