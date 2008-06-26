/// Start fleshing out cuda mesh structure.

class cuda_mesh
{
    public:
        /// a pointer to a array of float4's allocated on the device
        float* points_and_selection;
        uint_t number_of_points;
    
    private:
        k3d::mesh& m_mesh_reference;
}        

class cuda_polyhedra;
{
    public:
        /// a pointer to a uint2 array allocated on the device
        uint_t* per_polygon_first_face_and_face_count;
        /// a pointer to a int array allocated on the device
        int32_t* per_polygon_types;
        
        /// the number of polyhedra - ie.  The lenghth of the above 2 arrays        
        uint_t number_of_polygons;
        
        /// a pointer to a uint2 array allocated on the device
        uint_t* per_face_first_loops_and_loop_count;
        /// a pointer to a float array on the device
        float* per_face_selection;
        
        /// the number of faces - ie.  The length of the above 2 arrays
        uint_t number_of_faces;
        
        /// a pointer to a uint array allocated on the device
        uint_t* per_loop_first_edge;
        /// the number of loops - ie.  The length of the above array
        uint_t number_of_loops;
        
        /// a pointer to a uint2 array allocated on the device
        uint_t* per_edge_point_and_clockwise_edge;
        /// a pointer to a float array on the device
        float* per_edge_selection;
        
        /// the number of edges - ie.  The length of the above 2 arrays
        uint_t number_of_edges;
        
}