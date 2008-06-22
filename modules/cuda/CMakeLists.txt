INCLUDE ( FindCUDA.cmake )

WRAP_CUDA(CUDA_SOURCES cuda_entry_points.cu)

K3D_BUILD_CUDA_MODULE(k3d-cuda)
K3D_CREATE_MODULE_PROXY(k3d-cuda)

TARGET_LINK_LIBRARIES(k3d-cuda k3d-cuda-shared)

# disable RPATH since it prevents the emulation mode from working correctly
IF (CUDA_EMULATION)
    SET_TARGET_PROPERTIES ( k3d-cuda PROPERTIES SKIP_RPATH "ON" )
ENDIF (CUDA_EMULATION)


