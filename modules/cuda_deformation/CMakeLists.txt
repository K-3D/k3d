INCLUDE ( ../cuda_common/FindCUDA.cmake )
WRAP_CUDA(CUDA_SOURCES ../cuda_common/cuda_entry_points.cu)

GET_CUDA_INC_DIRS ( CUDA_INCLUDE_DIRECTORIES )

INCLUDE_DIRECTORIES(${CUDA_INCLUDE_DIRECTORIES})

SET(CUDA_LIBS ${CUDA_LIBRARIES} ${CUDA_UTIL_LIBRARY})

K3D_BUILD_CUDA_MODULE(k3d-cuda-deformation)
K3D_CREATE_MODULE_PROXY(k3d-cuda-deformation)

TARGET_LINK_LIBRARIES(k3d-cuda-deformation ${CUDA_LIBS})

# disable RPATH since it prevents the emulation mode from working correctly
IF (CUDA_EMULATION)
    SET_TARGET_PROPERTIES ( k3d-cuda-deformation PROPERTIES SKIP_RPATH "ON" )
ENDIF (CUDA_EMULATION)


