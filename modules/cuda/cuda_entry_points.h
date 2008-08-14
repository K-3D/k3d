#ifndef CUDA_ENTRY_POINTS_H
#define CUDA_ENTRY_POINTS_H

// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
//
// Contact: tshead@k-3d.com
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/** \file
	\author Evan Lezar (evanlezar@gmail.com)
*/

#include <k3d-platform-config.h>

#if defined K3D_API_WIN32
	#include <k3dsdk/win32.h>
	#define K3D_CUDA_DECLSPEC __declspec(dllexport)
#else
	#include <sys/time.h>
	#define K3D_CUDA_DECLSPEC
#endif // !K3D_API_WIN32

// define the types of bitmap kernels supported
#define CUDA_BITMAP_ADD 0x00
#define CUDA_BITMAP_MULTIPLY 0x01
#define CUDA_BITMAP_SUBTRACT 0x02
#define CUDA_BITMAP_COLOR_MONOCHROME 0x03
#define CUDA_BITMAP_GAMMA 0x04
#define CUDA_BITMAP_INVERT 0x05
#define CUDA_BITMAP_MATTE_COLORDIFF 0x06
#define CUDA_BITMAP_MATTE_INVERT 0x07

// define the profiling strings
#define PROFILE_STRING_HOST_TO_DEVICE "Copy from Host to Device"
#define PROFILE_STRING_EXECUTE_KERNEL "Execute Kernel"
#define PROFILE_STRING_DEVICE_TO_HOST "Copy from Device to Host"

/**
 * a struct to pass timing info back from external C functions
 */
typedef struct
{
	int numEntries;
	double *timings;
	char **labels;
} timingInfo_t;

// forward declaration of the entry functions

extern "C" K3D_CUDA_DECLSPEC void checkLastCudaError ();
extern "C" K3D_CUDA_DECLSPEC void bitmap_arithmetic_kernel_entry(int operation, unsigned short* p_deviceImage, int width, int height, float value);
extern "C" K3D_CUDA_DECLSPEC void bitmap_color_monochrome_kernel_entry(unsigned short* p_deviceImage, int width, int height, float redWeight, float greenWeight, float blueWeight);
extern "C" K3D_CUDA_DECLSPEC void bitmap_threshold_kernel_entry(unsigned short* p_deviceImage, int width, int height, float redThreshold, float greenThreshold, float blueThreshold, float alphaThreshold);

extern "C" K3D_CUDA_DECLSPEC void apply_linear_transform_to_point_data ( float *device_points, float *device_matrix, int num_points );

extern "C" K3D_CUDA_DECLSPEC void allocate_device_memory ( void** device_pointer, int size_in_bytes );
extern "C" K3D_CUDA_DECLSPEC void set_device_memory ( void* device_pointer, int value, int size_in_bytes );
extern "C" K3D_CUDA_DECLSPEC void resize_device_memory_block ( void** new_device_pointer, void* current_device_pointer, int new_size_in_bytes, int old_size_in_bytes, char clear );
extern "C" K3D_CUDA_DECLSPEC void copy_from_host_to_device ( void* device_pointer, const void* host_pointer, int size_in_bytes );
extern "C" K3D_CUDA_DECLSPEC void copy_from_host_to_device_64_to_32_convert ( void* device_pointer, const void* host_pointer, int size_in_bytes );

extern "C" K3D_CUDA_DECLSPEC void copy_from_device_to_host ( void* host_pointer, const void* device_pointer, int size_in_bytes );
extern "C" K3D_CUDA_DECLSPEC void copy_from_device_to_host_32_to_64_convert ( void* host_pointer, const void* device_pointer, int size_in_bytes );

extern "C" K3D_CUDA_DECLSPEC void copy_from_device_to_device ( void* device_dest_pointer, const void* device_source_pointer, int size_in_bytes );

extern "C" K3D_CUDA_DECLSPEC void free_device_memory ( void* device_pointer );
extern "C" K3D_CUDA_DECLSPEC void allocate_pinned_host_memory ( void** pointer_on_host, int size_in_bytes );
extern "C" K3D_CUDA_DECLSPEC void free_pinned_host_memory ( void* pointer_on_host );


extern "C" K3D_CUDA_DECLSPEC void copy_and_bind_texture_to_array( void** cudaArrayPointer, float* arrayData, int width, int height );
extern "C" K3D_CUDA_DECLSPEC void free_CUDA_array ( void* cudaArrayPointer );

extern "C" K3D_CUDA_DECLSPEC void transform_points_synchronous ( double *InputPoints, double *PointSelection, double *OutputPoints, int num_points, timingInfo_t* tInfo );
extern "C" K3D_CUDA_DECLSPEC void transform_points_asynchronous ( double *InputPoints, double *PointSelection, double *OutputPoints, int num_points, timingInfo_t* tInfo );

extern "C" K3D_CUDA_DECLSPEC void subdivide_edges_split_point_calculator (
                                                        unsigned int* phost_edge_indices,
                                                        unsigned int num_edge_indices,
                                                        float* pdev_points_and_selection,
                                                        unsigned int num_input_points,
                                                        unsigned int* pdev_edge_point_indices,
                                                        unsigned int* pdev_clockwise_edge_indices,
                                                        int num_split_points
                                                            );

extern "C" K3D_CUDA_DECLSPEC void subdivide_edges_update_indices_entry (
                                                        unsigned int* pdev_input_edge_point_indices,
                                                        unsigned int* pdev_input_clockwise_edge_point_indices,
                                                        unsigned int num_host_edges,
                                                        unsigned int* pdev_output_edge_point_indices,
                                                        unsigned int* pdev_output_clockwise_edge_point_indices,
                                                        unsigned int* pdev_edge_index_map,
                                                        int num_edge_maps
                                                            );

extern "C" K3D_CUDA_DECLSPEC void subdivide_edges_update_loop_first_edges_entry (
                                                        unsigned int* pdev_ouput_loop_first_edges,
                                                        unsigned int num_loops,
                                                        unsigned int* pdev_edge_index_map,
                                                        int num_edge_maps
                                                            );
extern "C" K3D_CUDA_DECLSPEC void subdivide_edges_split_edges_entry (
                                                        unsigned int* pdev_output_edge_point_indices,
                                                        unsigned int* pdev_output_clockwise_edge_point_indices,
                                                        unsigned int* pdev_input_clockwise_edge_point_indices,
                                                        unsigned int* pdev_edge_index_map,
                                                        unsigned int* pdev_edge_indices,
                                                        unsigned int num_edge_indices,
                                                        int num_split_points,
                                                        unsigned int* pdev_first_midpoint,
                                                        unsigned int* pdev_companions,
                                                        unsigned char* pdev_boundary_edges
                                                        );

extern "C" K3D_CUDA_DECLSPEC void copy_2D_from_host_to_device_with_padding ( void* device_pointer, const void* host_pointer, int device_pitch, int host_pitch, int width_in_bytes, int rows );

extern "C" K3D_CUDA_DECLSPEC void synchronize_threads ();

extern "C" K3D_CUDA_DECLSPEC void set_selection_value_entry ( float* points_and_selection, float selection_value, int num_points );

extern "C" K3D_CUDA_DECLSPEC void find_companion_kernel_entry ( unsigned char* pdev_boundary_edges,
																unsigned int* pdev_adjacent_edge_indices,
															    const int num_edges,
															    const unsigned int* pdev_edge_point_indices,
															    const unsigned int* pdev_clockwise_edges_point_indices,
															    const unsigned int* pdev_first_edges,
															    const unsigned int* pdev_valences,
															    const unsigned int* pdev_point_edges
															    );

extern "C" K3D_CUDA_DECLSPEC int create_vertex_valence_lookup_kernel_entry (
																unsigned int* pdev_valence,
																const unsigned int* pdev_edge_point_indices,
																int num_edges
																);

extern "C" K3D_CUDA_DECLSPEC void calculate_first_edge_entry ( unsigned int* pdev_first_edge, const unsigned int* pdev_valences, int num_points );

extern "C" K3D_CUDA_DECLSPEC void calculate_point_edges_entry (
																unsigned int* pdev_point_edges,
																unsigned int* pdev_found_edges,
																const unsigned int* edge_point_indices,
																const unsigned int* pdev_first_edges,
																int num_edges,
																int num_points);

extern "C" K3D_CUDA_DECLSPEC unsigned int edge_index_calculator_entry (
															unsigned int* pdev_edge_list,
															unsigned int* phost_edge_list_size,
															unsigned int* pdev_first_midpoint,
															unsigned char* pdev_has_midpoint,
															unsigned int* pdev_index_map,
															const unsigned int* pdev_face_first_loops,
															const unsigned int* pdev_face_loop_counts,
															const unsigned int* pdev_loop_first_edges,
															const unsigned int* pdev_clockwise_edges,
															const float* pdev_edge_selection,
															const unsigned int* pdev_companions,
															const unsigned char* pdev_boundary_edges,
															int split_point_count,
															int num_faces,
															int first_new_point_index
																);

extern "C" K3D_CUDA_DECLSPEC void create_grid_structure_kernel_entry (
												unsigned int* pdev_face_first_loops,
												unsigned int* pdev_face_loop_count,
												unsigned int* pdev_loop_first_edge,
												unsigned int* pdev_edge_point,
												unsigned int* pdev_clockwise_edge,
												unsigned int rows,
												unsigned int columns);

extern "C" K3D_CUDA_DECLSPEC void calculate_grid_points_kernel_entry (
												float* pdev_point_and_selection,
												float* phost_x,
												float* phost_y,
												unsigned int rows,
												unsigned int columns
												);

#endif // !CUDA_ENTRY_POINTS_H
