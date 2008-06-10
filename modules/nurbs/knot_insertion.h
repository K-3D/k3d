#ifndef _K3D_NURBS_KNOT_INSERTION
#define _K3D_NURBS_KNOT_INSERTION

#include <k3dsdk/log.h>
#include <k3dsdk/module.h>
#include <k3dsdk/node.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/nurbs.h>

namespace module
{
	namespace nurbs
	{
		void curve_knot_insertion(k3d::mesh& input, size_t curve, double u, size_t r)
		{
			k3d::mesh::nurbs_curve_groups_t& groups = *k3d::make_unique(input.nurbs_curve_groups);
			k3d::mesh::knots_t& curve_knots = *k3d::make_unique(groups.curve_knots);
			k3d::mesh::points_t& mesh_points = *k3d::make_unique(input.points);
			
			size_t order = (*groups.curve_orders)[curve];
			size_t nr_points = (*groups.curve_point_counts)[curve];
			
			size_t curve_points_begin = (*groups.curve_first_points)[curve];
			size_t curve_points_end = curve_points_begin + nr_points;
			
			size_t curve_knots_begin = (*groups.curve_first_knots)[curve];
			size_t curve_knots_end = curve_knots_begin + nr_points + order;
			
			int k=-1;
			size_t s=0;
			
			//look where to insert the new knot, check multiplicity(s) of the knot at u
			size_t i = curve_knots_begin;
			while( (i < curve_knots_end) && (curve_knots[i] < u) )
			{
				i++;
			}
			//we found the first knot greater than u or we're at the end so thats our k now
			k=i;
			if ( i < curve_knots_end)
			{
				//we go back to see how often we have this knot
				while( i >= curve_knots_begin && (curve_knots[k] == u) )
				{
					i--;
					s++;
				}
			}
			
			if( s + r > order - 1)
			{
				k3d::log() << error << "Cannot insert knot r=" << r << " times, multiplicity would be greater than degree" << std::endl;
				k3d::log() << debug << "Reducing r to the maximum possible value" << std::endl;
				r = order - 1 - s;
			}
			
			//*******************************************************************
			//Algorithm from "The NURBS book"
			//*******************************************************************
			
			size_t mp = nr_points + order;
			size_t np = nr_points + r;
			
			k3d::mesh::knots_t knots;
			
			for( size_t i = 0; i <= k; i++ )
				knots.push_back(curve_knots[i + curve_knots_begin]);
			
			for( size_t i = 1; i <= r; i++ )
				knots.push_back(u);
			
			for( size_t i = k + 1; i <= mp; i++ )
				knots.push_back(curve_knots[i + curve_knots_begin]);
				
			k3d::mesh::points_t points(mp, mesh_points[ ((*groups.curve_points)[ curve_points_begin ]) ] );
			
			for( size_t i = 0; i <= k + 1 - order; i++ )
				points[i] = mesh_points[ (*groups.curve_points)[ curve_points_begin + i] ];
			
			for( size_t i = k - s; i <= np; i++ )
				points[i + r] = mesh_points[ (*groups.curve_points)[ curve_points_begin + i] ];
			
			k3d::mesh::points_t tmp;
			
			for( size_t i = 0; i< order - 1 - s; i++ )
				tmp.push_back( mesh_points[ (*groups.curve_points)[ curve_points_begin + i + k - (order - 1)] ] );
				
			size_t L=0;
			for( size_t j = 1; j <= r; j++ )
			{
				L = k - (order - 1) + j;
				float alpha=0.0;
				for( size_t i = 0; i <= order - 1 - j - s; i++ )
				{
					alpha = (u - knots[L + i]) / (knots[i + k + 1] - knots[L + i]);
					tmp[i] = alpha * tmp[i + 1] + (1.0 - alpha) * tmp[i];
				}
				points[L] = tmp[0];
				points[k + r - j - s] = tmp[order - 1 - j - s];
			}
			
			for( size_t i = L + 1; i < k - s; i++ )
				points[i] = tmp[i - L];
				
			//*******************************************************************
			//Insert new points and knot vector into the mesh
			//*******************************************************************
			
			//insert knots
			k3d::mesh::knots_t::iterator knot = curve_knots.begin();
			knot += (*groups.curve_first_knots)[curve];
			//remove old knot vector
			for( size_t i = 0; i < nr_points + order; i++)
				curve_knots.erase(knot);
			//insert new one
			curve_knots.insert(knot, knots.begin(), knots.end());
			const size_t knot_offset = knots.size() - nr_points - order;
			
			k3d::mesh::indices_t curve_first_knots = *k3d::make_unique( groups.curve_first_knots );
			
			for( size_t curr_curve = curve; curr_curve < (*groups.curve_counts)[0]; curr_curve++ )
			{
				curve_first_knots[curr_curve] += knot_offset;
			}
			
			//new points
			/*
				TODO:
				->insert new ones into the mesh_points array(at the end!), leave others unchanged
				->index to the positions in mesh_points
				->offset all "first_points"
			*/
			k3d::mesh::indices_t curve_points(points.size(), 0);
			
			for( size_t i = 0; i < points.size(); i++ )
			{
				k3d::mesh::points_t::iterator index = find(mesh_points.begin(), mesh_points.end(), points[i]);
				if( index != mesh_points.end())
					curve_points[i] = distance(mesh_points.begin(),index);
			}
			
		}
	}//namespace nurbs
}//namespace module

#endif //_K3D_NURBS_KNOT_INSERTION
