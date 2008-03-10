K3D_BUILD_MODULE(k3d-quadrilateralremeshing)
K3D_CREATE_MODULE_PROXY(k3d-quadrilateralremeshing)

TARGET_LINK_LIBRARIES(k3d-quadrilateralremeshing ${K3D_SUPERLU_LIBRARY} ${K3D_BLAS_LIBRARY})