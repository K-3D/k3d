SET(K3D_GMM_FOUND 0)

FIND_PATH(K3D_GMM_INCLUDE_DIR gmm/gmm.h 
	/usr/include
	DOC "Directory where the gmm directory containing header files is located"
	)
MARK_AS_ADVANCED(K3D_GMM_INCLUDE_DIR)

IF(EXISTS ${K3D_GMM_INCLUDE_DIR}/gmm/gmm.h)
	SET(K3D_GMM_FOUND 1)
ENDIF(EXISTS ${K3D_GMM_INCLUDE_DIR}/gmm/gmm.h)

