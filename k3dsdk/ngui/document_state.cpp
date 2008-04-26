// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
//
// Contact: tshead@k-3d.com
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your argument) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/**		\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <gdkmm/screen.h>
#include <gtkmm/dialog.h>
#include <gtkmm/menu.h>
#include <gtkmm/window.h>

#include <gtk/gtkgl.h>
#include <gtk/gtkmain.h>

#include <k3d-i18n-config.h>
#include <k3d-platform-config.h>

#include "application_state.h"
#include "context_menu.h"
#include "document_state.h"
#include "move_tool.h"
#include "rotate_tool.h"
#include "safe_close_dialog.h"
#include "scale_tool.h"
#include "selection_tool.h"
#include "snap_tool.h"
#include "unsaved_document.h"
#include "utility.h"
#include "viewport.h"

#include <k3dsdk/application.h>
#include <k3dsdk/batch_mode.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/command_node.h>
#include <k3dsdk/command_tree.h>
#include <k3dsdk/plugins.h>
#include <k3dsdk/data.h>
#include <k3dsdk/iapplication.h>
#include <k3dsdk/icommand_tree.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/imaterial_sink.h>
#include <k3dsdk/imesh_selection_sink.h>
#include <k3dsdk/imesh_sink.h>
#include <k3dsdk/imesh_source.h>
#include <k3dsdk/imulti_mesh_sink.h>
#include <k3dsdk/inode_collection_sink.h>
#include <k3dsdk/ipersistent.h>
#include <k3dsdk/ipipeline.h>
#include <k3dsdk/iscripted_action.h>
#include <k3dsdk/iselectable.h>
#include <k3dsdk/itime_sink.h>
#include <k3dsdk/legacy_mesh.h>
#include <k3dsdk/mesh_selection.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/time_source.h>
#include <k3dsdk/transform.h>
#include <k3dsdk/user_properties.h>

#include <iterator>
#include <functional>
#include <set>

namespace libk3dngui
{

namespace detail
{

/// Provides human-readable labels for the selection_mode_t enumeration
const k3d::ienumeration_property::enumeration_values_t& selection_mode_values()
{
	static k3d::ienumeration_property::enumeration_values_t values;
	if(values.empty())
	{
		values.push_back(k3d::ienumeration_property::enumeration_value_t(_("Nodes"), "nodes", _("Select Nodes")));
		values.push_back(k3d::ienumeration_property::enumeration_value_t(_("Points"), "points", _("Select Points")));
		values.push_back(k3d::ienumeration_property::enumeration_value_t(_("Lines"), "lines", _("Select Lines")));
		values.push_back(k3d::ienumeration_property::enumeration_value_t(_("Faces"), "faces", _("Select Faces")));
	}

	return values;
}

const bool is_point_selected(const k3d::selection::record& Record)
{
	k3d::mesh* const mesh = k3d::selection::get_mesh(Record);
	if(!mesh)
		return false;

	if(mesh->point_selection)
	{
		const k3d::selection::id id = Record.get_id(k3d::selection::ABSOLUTE_POINT);
		if(id < mesh->point_selection->size())
			return (*mesh->point_selection)[id];
	}

	return false;
}

/** \todo Support nucurves */
const bool is_line_selected(const k3d::selection::record& Record)
{
	k3d::mesh* const mesh = k3d::selection::get_mesh(Record);
	if(!mesh)
		return false;

	if(mesh->polyhedra && mesh->polyhedra->edge_selection)
	{
		const k3d::selection::id id = Record.get_id(k3d::selection::ABSOLUTE_SPLIT_EDGE);
		if(id < mesh->polyhedra->edge_selection->size())
			return (*mesh->polyhedra->edge_selection)[id];
	}

	if(mesh->linear_curve_groups && mesh->linear_curve_groups->curve_selection)
	{
		const k3d::selection::id id = Record.get_id(k3d::selection::ABSOLUTE_LINEAR_CURVE);
		if(id < mesh->linear_curve_groups->curve_selection->size())
			return (*mesh->linear_curve_groups->curve_selection)[id];
	}

	if(mesh->cubic_curve_groups && mesh->cubic_curve_groups->curve_selection)
	{
		const k3d::selection::id id = Record.get_id(k3d::selection::ABSOLUTE_CUBIC_CURVE);
		if(id < mesh->cubic_curve_groups->curve_selection->size())
			return (*mesh->cubic_curve_groups->curve_selection)[id];
	}

	return false;
}

/** \todo Support patches */
const bool is_face_selected(const k3d::selection::record& Record)
{
	k3d::mesh* const mesh = k3d::selection::get_mesh(Record);
	if(!mesh)
		return false;

	if(mesh->polyhedra && mesh->polyhedra->face_selection)
	{
		const k3d::selection::id id = Record.get_id(k3d::selection::ABSOLUTE_FACE);
		if(id < mesh->polyhedra->face_selection->size())
			return (*mesh->polyhedra->face_selection)[id];
	}

	return false;
}

const bool is_node_selected(const k3d::selection::record& Record)
{
	return k3d::selection::is_selected(k3d::selection::get_node(Record));
}

/// Defines a mapping of nodes to selection records
typedef std::multimap<k3d::inode*, const k3d::selection::record*> node_selection_map_t;
/// Given a set of selection records, generates a mapping of the corresponding nodes to each record
const node_selection_map_t map_nodes(const k3d::selection::records& Selection)
{
	node_selection_map_t results;

	/** \todo Improve the performance of this loop by cacheing nodes */
	for(k3d::selection::records::const_iterator record = Selection.begin(); record != Selection.end(); ++record)
		results.insert(std::make_pair(k3d::selection::get_node(*record), &*record));

	if(results.count(0))
		k3d::log() << warning << "Selection contained records without nodes" << std::endl;
	results.erase(0);

	return results;
}

/// Policy class that updates a mesh_selection to select the given points
struct select_points
{
	select_points(const double Weight) :
		weight(Weight)
	{
	}

	void operator()(const k3d::selection::record& Record, k3d::mesh_selection& Selection) const
	{
		for(k3d::selection::record::tokens_t::const_iterator token = Record.tokens.begin(); token != Record.tokens.end(); ++token)
		{
			switch(token->type)
			{
				case k3d::selection::ABSOLUTE_POINT:
					Selection.points.push_back(k3d::mesh_selection::record(token->id, token->id+1, weight));
					return;
				default:
					break;
			}
		}
	}

	const double weight;
};

/// Policy class that updates a mesh_selection to select the given lines
struct select_lines
{
	select_lines(const double Weight) :
		weight(Weight)
	{
	}

	void operator()(const k3d::selection::record& Record, k3d::mesh_selection& Selection) const
	{
		for(k3d::selection::record::tokens_t::const_iterator token = Record.tokens.begin(); token != Record.tokens.end(); ++token)
		{
			switch(token->type)
			{
				case k3d::selection::ABSOLUTE_SPLIT_EDGE:
					Selection.edges.push_back(k3d::mesh_selection::record(token->id, token->id+1, weight));
					return;

				case k3d::selection::ABSOLUTE_LINEAR_CURVE:
					Selection.linear_curves.push_back(k3d::mesh_selection::record(token->id, token->id+1, weight));
					return;

				case k3d::selection::ABSOLUTE_CUBIC_CURVE:
					Selection.cubic_curves.push_back(k3d::mesh_selection::record(token->id, token->id+1, weight));
					return;

				case k3d::selection::ABSOLUTE_NURBS_CURVE:
					Selection.nurbs_curves.push_back(k3d::mesh_selection::record(token->id, token->id+1, weight));
					return;

				default:
					break;
			}
		}
	}

	const double weight;
};

/// Policy class that updates a mesh_selection to select the given faces
struct select_faces
{
	select_faces(const double Weight) :
		weight(Weight)
	{
	}

	void operator()(const k3d::selection::record& Record, k3d::mesh_selection& Selection) const
	{
		for(k3d::selection::record::tokens_t::const_iterator token = Record.tokens.begin(); token != Record.tokens.end(); ++token)
		{
			switch(token->type)
			{
				case k3d::selection::ABSOLUTE_FACE:
					Selection.faces.push_back(k3d::mesh_selection::record(token->id, token->id+1, weight));
					return;

				case k3d::selection::ABSOLUTE_BILINEAR_PATCH:
					Selection.bilinear_patches.push_back(k3d::mesh_selection::record(token->id, token->id+1, weight));
					return;

				case k3d::selection::ABSOLUTE_BICUBIC_PATCH:
					Selection.bicubic_patches.push_back(k3d::mesh_selection::record(token->id, token->id+1, weight));
					return;

				case k3d::selection::ABSOLUTE_NURBS_PATCH:
					Selection.nurbs_patches.push_back(k3d::mesh_selection::record(token->id, token->id+1, weight));
					return;

				default:
					break;
			}
		}
	}

	const double weight;
};

/// Uses an update policy to convert the supplied selection into updates to MeshInstance mesh selections
template<typename UpdatePolicyT>
void select_components(const k3d::selection::records& Selection, const UpdatePolicyT& UpdatePolicy)
{
	k3d::inode* node = 0;
	k3d::mesh_selection selection;
	k3d::imesh_selection_sink* sink = 0;

	const node_selection_map_t nodes = map_nodes(Selection);
	for(node_selection_map_t::const_iterator n = nodes.begin(); n != nodes.end(); ++n)
	{
		if(n->first != node)
		{
			if(sink && node)
			{
				k3d::property::set_internal_value(sink->mesh_selection_sink_input(), selection);
			}

			node = n->first;
			selection = k3d::mesh_selection::select_null();
			sink = dynamic_cast<k3d::imesh_selection_sink*>(node);
			if(sink)
				selection = boost::any_cast<k3d::mesh_selection>(sink->mesh_selection_sink_input().property_internal_value());
		}

		if(!sink)
			continue;

		UpdatePolicy(*n->second, selection);
	}

	if(sink && node)
	{
		k3d::property::set_internal_value(sink->mesh_selection_sink_input(), selection);
	}
}

/// Uses an update policy to alter the set of all selected MeshInstance mesh selections
template<typename UpdatePolicyT>
void update_component_selection(const k3d::nodes_t& Nodes, const UpdatePolicyT& UpdatePolicy, const bool VisibleSelection)
{
	for(k3d::nodes_t::const_iterator node = Nodes.begin(); node != Nodes.end(); ++node)
	{
		if(k3d::classes::MeshInstance() != (*node)->factory().factory_id())
			continue;

		k3d::imesh_selection_sink* const mesh_selection_sink = dynamic_cast<k3d::imesh_selection_sink*>(*node);
		if(!mesh_selection_sink)
			continue;

		k3d::imesh_source* const mesh_source = dynamic_cast<k3d::imesh_source*>(*node);
		if(!mesh_source)
            continue;

		const k3d::mesh* const mesh = boost::any_cast<k3d::mesh*>(mesh_source->mesh_source_output().property_internal_value());
		if(!mesh)
            continue;

		k3d::mesh_selection selection =
			boost::any_cast<k3d::mesh_selection>(mesh_selection_sink->mesh_selection_sink_input().property_internal_value());

		UpdatePolicy(*mesh, selection);

		k3d::property::set_internal_value(mesh_selection_sink->mesh_selection_sink_input(), selection);
		k3d::property::set_internal_value(**node, "show_component_selection", VisibleSelection);
	}
}

struct select_all_points
{
	void operator()(const k3d::mesh& Mesh, k3d::mesh_selection& Selection) const
	{
		Selection.points = k3d::mesh_selection::component_select_all();
		Selection.edges = k3d::mesh_selection::component_deselect_all();
		Selection.faces = k3d::mesh_selection::component_deselect_all();
		Selection.linear_curves = k3d::mesh_selection::component_deselect_all();
		Selection.cubic_curves = k3d::mesh_selection::component_deselect_all();
		Selection.nurbs_curves = k3d::mesh_selection::component_deselect_all();
		Selection.bilinear_patches = k3d::mesh_selection::component_deselect_all();
		Selection.bicubic_patches = k3d::mesh_selection::component_deselect_all();
		Selection.nurbs_patches = k3d::mesh_selection::component_deselect_all();
	}
};

struct select_all_lines
{
	void operator()(const k3d::mesh& Mesh, k3d::mesh_selection& Selection) const
	{
		Selection.points = k3d::mesh_selection::component_deselect_all();
		Selection.edges = k3d::mesh_selection::component_select_all();
		Selection.faces = k3d::mesh_selection::component_deselect_all();
		Selection.linear_curves = k3d::mesh_selection::component_select_all();
		Selection.cubic_curves = k3d::mesh_selection::component_select_all();
		Selection.nurbs_curves = k3d::mesh_selection::component_select_all();
		Selection.bilinear_patches = k3d::mesh_selection::component_deselect_all();
		Selection.bicubic_patches = k3d::mesh_selection::component_deselect_all();
		Selection.nurbs_patches = k3d::mesh_selection::component_deselect_all();
	}
};

struct select_all_faces
{
	void operator()(const k3d::mesh& Mesh, k3d::mesh_selection& Selection) const
	{
		Selection.points = k3d::mesh_selection::component_deselect_all();
		Selection.edges = k3d::mesh_selection::component_deselect_all();
		Selection.faces = k3d::mesh_selection::component_select_all();
		Selection.linear_curves = k3d::mesh_selection::component_deselect_all();
		Selection.cubic_curves = k3d::mesh_selection::component_deselect_all();
		Selection.nurbs_curves = k3d::mesh_selection::component_deselect_all();
		Selection.bilinear_patches = k3d::mesh_selection::component_select_all();
		Selection.bicubic_patches = k3d::mesh_selection::component_select_all();
		Selection.nurbs_patches = k3d::mesh_selection::component_select_all();
	}
};

struct deselect_all
{
	void operator()(const k3d::mesh& Mesh, k3d::mesh_selection& Selection) const
	{
		Selection = k3d::mesh_selection::deselect_all();
	}
};

struct select_null
{
	void operator()(const k3d::mesh& Mesh, k3d::mesh_selection& Selection) const
	{
		Selection = k3d::mesh_selection::select_null();
	}
};

void invert(k3d::mesh_selection::records_t& Records)
{
	for(k3d::mesh_selection::records_t::iterator record = Records.begin(); record != Records.end(); ++record)
		record->weight = record->weight ? 0.0 : 1.0;
}

struct invert_points
{
	void operator()(const k3d::mesh& Mesh, k3d::mesh_selection& Selection) const
	{
		invert(Selection.points);
	}
};

struct invert_lines
{
	void operator()(const k3d::mesh& Mesh, k3d::mesh_selection& Selection) const
	{
		invert(Selection.edges);
		invert(Selection.linear_curves);
		invert(Selection.cubic_curves);
		invert(Selection.nurbs_curves);
	}
};

struct invert_faces
{
	void operator()(const k3d::mesh& Mesh, k3d::mesh_selection& Selection) const
	{
		invert(Selection.faces);
		invert(Selection.bilinear_patches);
		invert(Selection.bicubic_patches);
		invert(Selection.nurbs_patches);
	}
};

void deselect_gaps(k3d::mesh_selection::records_t& Records)
{
    Records.insert(Records.begin(), k3d::mesh_selection::record(0, size_t(-1), 0.0));
}

void deselect_gaps(k3d::mesh_selection& Selection)
{
	deselect_gaps(Selection.points);
	deselect_gaps(Selection.edges);
	deselect_gaps(Selection.faces);
	deselect_gaps(Selection.linear_curves);
	deselect_gaps(Selection.cubic_curves);
	deselect_gaps(Selection.nurbs_curves);
	deselect_gaps(Selection.bilinear_patches);
	deselect_gaps(Selection.bicubic_patches);
	deselect_gaps(Selection.nurbs_patches);
}

/** \todo Handle adjacent edges */
struct convert_to_points
{
	convert_to_points(bool KeepSelection) : m_keep_selection(KeepSelection) {}
	void operator()(const k3d::mesh& Mesh, k3d::mesh_selection& Selection) const
	{
		if(!(Mesh.points && Mesh.point_selection))
			return;
		
		// Sync with existing point selections
		for (k3d::uint_t point = 0; point != Mesh.point_selection->size(); ++point)
		{
			if (Mesh.point_selection->at(point))
				Selection.points.push_back(k3d::mesh_selection::record(point, 1.0));
		}

		// Convert edge selections to point selections ...
		if(Mesh.polyhedra && Mesh.polyhedra->edge_points && Mesh.polyhedra->clockwise_edges)
		{
			const size_t edge_begin = 0;
			const size_t edge_end = edge_begin + Mesh.polyhedra->edge_points->size();
			for(size_t edge = edge_begin; edge != edge_end; ++edge)
			{
				if(!(*Mesh.polyhedra->edge_selection)[edge])
				continue;

				Selection.points.push_back(k3d::mesh_selection::record((*Mesh.polyhedra->edge_points)[edge], 1.0));
			}
		}

		// Convert face selections to point selections ...
		if(Mesh.polyhedra && Mesh.polyhedra->face_first_loops && Mesh.polyhedra->face_loop_counts && Mesh.polyhedra->face_selection && Mesh.polyhedra->loop_first_edges && Mesh.polyhedra->edge_points && Mesh.polyhedra->clockwise_edges)
		{
	    const size_t face_begin = 0;
	    const size_t face_end = face_begin + Mesh.polyhedra->face_first_loops->size();
	    for(size_t face = face_begin; face != face_end; ++face)
	    {
				if(!(*Mesh.polyhedra->face_selection)[face])
				    continue;
	
				const size_t face_loop_begin = (*Mesh.polyhedra->face_first_loops)[face];
				const size_t face_loop_end = face_loop_begin + (*Mesh.polyhedra->face_loop_counts)[face];
				for(size_t face_loop = face_loop_begin; face_loop != face_loop_end; ++face_loop)
				{
				    const size_t first_edge = (*Mesh.polyhedra->loop_first_edges)[face_loop];
				    for(size_t edge = first_edge; ; )
				    {
					Selection.points.push_back(k3d::mesh_selection::record((*Mesh.polyhedra->edge_points)[edge], 1.0));
	
					edge = (*Mesh.polyhedra->clockwise_edges)[edge];
					if(edge == first_edge)
					    break;
				    }
				}
	    } 
		}

		// Convert linear curve selections to point selections ...
		if(Mesh.linear_curve_groups && Mesh.linear_curve_groups->curve_first_points && Mesh.linear_curve_groups->curve_point_counts && Mesh.linear_curve_groups->curve_selection && Mesh.linear_curve_groups->curve_points)
		{
	    const size_t curve_begin = 0;
	    const size_t curve_end = curve_begin + Mesh.linear_curve_groups->curve_first_points->size();
	    for(size_t curve = curve_begin; curve != curve_end; ++curve)
	    {
				if(!(*Mesh.linear_curve_groups->curve_selection)[curve])
				    continue;
	
				const size_t curve_point_begin = (*Mesh.linear_curve_groups->curve_first_points)[curve];
				const size_t curve_point_end = curve_point_begin + (*Mesh.linear_curve_groups->curve_point_counts)[curve];
				for(size_t curve_point = curve_point_begin; curve_point != curve_point_end; ++curve_point)
				{
				    Selection.points.push_back(k3d::mesh_selection::record((*Mesh.linear_curve_groups->curve_points)[curve_point], 1.0));
				}
	    }
		}

		// Convert cubic curve selections to point selections ...
		if(Mesh.cubic_curve_groups && Mesh.cubic_curve_groups->curve_first_points && Mesh.cubic_curve_groups->curve_point_counts && Mesh.cubic_curve_groups->curve_selection && Mesh.cubic_curve_groups->curve_points)
		{
		    const size_t curve_begin = 0;
		    const size_t curve_end = curve_begin + Mesh.cubic_curve_groups->curve_first_points->size();
		    for(size_t curve = curve_begin; curve != curve_end; ++curve)
		    {
			if(!(*Mesh.cubic_curve_groups->curve_selection)[curve])
			    continue;

			const size_t curve_point_begin = (*Mesh.cubic_curve_groups->curve_first_points)[curve];
			const size_t curve_point_end = curve_point_begin + (*Mesh.cubic_curve_groups->curve_point_counts)[curve];
			for(size_t curve_point = curve_point_begin; curve_point != curve_point_end; ++curve_point)
			{
			    Selection.points.push_back(k3d::mesh_selection::record((*Mesh.cubic_curve_groups->curve_points)[curve_point], 1.0));
			}
		    }
		}

		// Convert nurbs curve selections to point selections ...
		if(Mesh.nurbs_curve_groups && Mesh.nurbs_curve_groups->curve_first_points && Mesh.nurbs_curve_groups->curve_point_counts && Mesh.nurbs_curve_groups->curve_selection && Mesh.nurbs_curve_groups->curve_points)
		{
		    const size_t curve_begin = 0;
		    const size_t curve_end = curve_begin + Mesh.nurbs_curve_groups->curve_first_points->size();
		    for(size_t curve = curve_begin; curve != curve_end; ++curve)
		    {
			if(!(*Mesh.nurbs_curve_groups->curve_selection)[curve])
			    continue;

			const size_t curve_point_begin = (*Mesh.nurbs_curve_groups->curve_first_points)[curve];
			const size_t curve_point_end = curve_point_begin + (*Mesh.nurbs_curve_groups->curve_point_counts)[curve];
			for(size_t curve_point = curve_point_begin; curve_point != curve_point_end; ++curve_point)
			{
			    Selection.points.push_back(k3d::mesh_selection::record((*Mesh.nurbs_curve_groups->curve_points)[curve_point], 1.0));
			}
		    }
		}

		// Convert bilinear patch selections to point selections ...
		if(Mesh.bilinear_patches && Mesh.bilinear_patches->patch_selection && Mesh.bilinear_patches->patch_points)
		{
			const size_t patch_begin = 0;
			const size_t patch_end = patch_begin + (Mesh.bilinear_patches->patch_points->size() / 4);
			for(size_t patch = patch_begin; patch != patch_end; ++patch)
			{
				if(!(*Mesh.bilinear_patches->patch_selection)[patch])
					continue;

				const size_t patch_point_begin = patch * 4;
				const size_t patch_point_end = patch_point_begin + 4;
				for(size_t patch_point = patch_point_begin; patch_point != patch_point_end; ++patch_point)
				{
					Selection.points.push_back(k3d::mesh_selection::record((*Mesh.bilinear_patches->patch_points)[patch_point], 1.0));
				}
			}
		}

		// Convert bicubic patch selections to point selections ...
		if(Mesh.bicubic_patches && Mesh.bicubic_patches->patch_selection && Mesh.bicubic_patches->patch_points)
		{
			const size_t patch_begin = 0;
			const size_t patch_end = patch_begin + (Mesh.bicubic_patches->patch_points->size() / 16);
			for(size_t patch = patch_begin; patch != patch_end; ++patch)
			{
				if(!(*Mesh.bicubic_patches->patch_selection)[patch])
					continue;

				const size_t patch_point_begin = patch * 16;
				const size_t patch_point_end = patch_point_begin + 16;
				for(size_t patch_point = patch_point_begin; patch_point != patch_point_end; ++patch_point)
				{
					Selection.points.push_back(k3d::mesh_selection::record((*Mesh.bicubic_patches->patch_points)[patch_point], 1.0));
				}
			}
		}

		// Convert nurbs patch selections to point selections ...
		if(Mesh.nurbs_patches && Mesh.nurbs_patches->patch_first_points && Mesh.nurbs_patches->patch_u_point_counts && Mesh.nurbs_patches->patch_v_point_counts && Mesh.nurbs_patches->patch_selection && Mesh.nurbs_patches->patch_points)
		{
			const size_t patch_begin = 0;
			const size_t patch_end = patch_begin + Mesh.nurbs_patches->patch_first_points->size();
			for(size_t patch = patch_begin; patch != patch_end; ++patch)
			{
				if(!(*Mesh.nurbs_patches->patch_selection)[patch])
					continue;

				const size_t patch_point_begin = (*Mesh.nurbs_patches->patch_first_points)[patch];
				const size_t patch_point_end = patch_point_begin + ((*Mesh.nurbs_patches->patch_u_point_counts)[patch] * (*Mesh.nurbs_patches->patch_v_point_counts)[patch]);
				for(size_t patch_point = patch_point_begin; patch_point != patch_point_end; ++patch_point)
				{
					Selection.points.push_back(k3d::mesh_selection::record((*Mesh.nurbs_patches->patch_points)[patch_point], 1.0));
				}
			}
		}

		if (!m_keep_selection)
		{
			Selection.edges.clear();
			Selection.faces.clear();
			Selection.linear_curves.clear();
			Selection.cubic_curves.clear();
			Selection.nurbs_curves.clear();
			Selection.bilinear_patches.clear();
			Selection.bicubic_patches.clear();
			Selection.nurbs_patches.clear();
		}

		// Ensure that anything not explicitly selected gets explicitly deselected ...
		deselect_gaps(Selection);
	}
	bool m_keep_selection;
};

/** \todo Select adjacent edges */
struct convert_to_lines
{
	convert_to_lines(bool KeepSelection) : m_keep_selection(KeepSelection) {}
	void operator()(const k3d::mesh& Mesh, k3d::mesh_selection& Selection) const
	{
		// Convert point selections to edge selections ...
		if(Mesh.polyhedra && Mesh.polyhedra->edge_points && Mesh.polyhedra->clockwise_edges && Mesh.polyhedra->edge_selection && Mesh.point_selection)
		{
			const k3d::mesh::indices_t& edge_points = *Mesh.polyhedra->edge_points;
			const k3d::mesh::indices_t& clockwise_edges = *Mesh.polyhedra->clockwise_edges;
			const k3d::mesh::selection_t edge_selection = *Mesh.polyhedra->edge_selection;
			const k3d::mesh::selection_t point_selection = *Mesh.point_selection;

			const size_t edge_begin = 0;
			const size_t edge_end = edge_begin + edge_points.size();
			for(size_t edge = edge_begin; edge != edge_end; ++edge)
			{
				if(edge_selection[edge])
				{
					Selection.edges.push_back(k3d::mesh_selection::record(edge, 1.0));
					continue;
				}

				if(point_selection[edge_points[edge]] || point_selection[edge_points[clockwise_edges[edge]]])
					Selection.edges.push_back(k3d::mesh_selection::record(edge, 1.0));
			}
		}

		// Convert point selections to linear curve selections ...
		if(Mesh.linear_curve_groups && Mesh.linear_curve_groups->curve_first_points && Mesh.linear_curve_groups->curve_point_counts && Mesh.linear_curve_groups->curve_selection && Mesh.linear_curve_groups->curve_points && Mesh.point_selection)
		{
			const k3d::mesh::indices_t& curve_first_points = *Mesh.linear_curve_groups->curve_first_points;
			const k3d::mesh::counts_t& curve_point_counts = *Mesh.linear_curve_groups->curve_point_counts;
			const k3d::mesh::selection_t& curve_selection = *Mesh.linear_curve_groups->curve_selection;
			const k3d::mesh::indices_t& curve_points = *Mesh.linear_curve_groups->curve_points;
			const k3d::mesh::selection_t& point_selection = *Mesh.point_selection;

			const size_t curve_begin = 0;
			const size_t curve_end = curve_begin + curve_first_points.size();
			for(size_t curve = curve_begin; curve != curve_end; ++curve)
			{
				if(curve_selection[curve])
				{
					Selection.linear_curves.push_back(k3d::mesh_selection::record(curve, 1.0));
					continue;
				}

				const size_t point_begin = curve_first_points[curve];
				const size_t point_end = point_begin + curve_point_counts[curve];
				for(size_t point = point_begin; point != point_end; ++point)
				{
					if(point_selection[curve_points[point]])
						Selection.linear_curves.push_back(k3d::mesh_selection::record(curve, 1.0));
				}
			}
		}
		
		// Convert point selections to cubic curve selections ...
		if(Mesh.cubic_curve_groups && Mesh.cubic_curve_groups->curve_first_points && Mesh.cubic_curve_groups->curve_point_counts && Mesh.cubic_curve_groups->curve_selection && Mesh.cubic_curve_groups->curve_points && Mesh.point_selection)
		{
			const k3d::mesh::indices_t& curve_first_points = *Mesh.cubic_curve_groups->curve_first_points;
			const k3d::mesh::counts_t& curve_point_counts = *Mesh.cubic_curve_groups->curve_point_counts;
			const k3d::mesh::selection_t& curve_selection = *Mesh.cubic_curve_groups->curve_selection;
			const k3d::mesh::indices_t& curve_points = *Mesh.cubic_curve_groups->curve_points;
			const k3d::mesh::selection_t& point_selection = *Mesh.point_selection;

			const size_t curve_begin = 0;
			const size_t curve_end = curve_begin + curve_first_points.size();
			for(size_t curve = curve_begin; curve != curve_end; ++curve)
			{
				if(curve_selection[curve])
				{
					Selection.cubic_curves.push_back(k3d::mesh_selection::record(curve, 1.0));
					continue;
				}

				const size_t point_begin = curve_first_points[curve];
				const size_t point_end = point_begin + curve_point_counts[curve];
				for(size_t point = point_begin; point != point_end; ++point)
				{
					if(point_selection[curve_points[point]])
						Selection.cubic_curves.push_back(k3d::mesh_selection::record(curve, 1.0));
				}
			}
		}
		
		// Convert point selections to nurbs curve selections ...
		if(Mesh.nurbs_curve_groups && Mesh.nurbs_curve_groups->curve_first_points && Mesh.nurbs_curve_groups->curve_point_counts && Mesh.nurbs_curve_groups->curve_selection && Mesh.nurbs_curve_groups->curve_points && Mesh.point_selection)
		{
			const k3d::mesh::indices_t& curve_first_points = *Mesh.nurbs_curve_groups->curve_first_points;
			const k3d::mesh::counts_t& curve_point_counts = *Mesh.nurbs_curve_groups->curve_point_counts;
			const k3d::mesh::selection_t& curve_selection = *Mesh.nurbs_curve_groups->curve_selection;
			const k3d::mesh::indices_t& curve_points = *Mesh.nurbs_curve_groups->curve_points;
			const k3d::mesh::selection_t& point_selection = *Mesh.point_selection;

			const size_t curve_begin = 0;
			const size_t curve_end = curve_begin + curve_first_points.size();
			for(size_t curve = curve_begin; curve != curve_end; ++curve)
			{
				if(curve_selection[curve])
				{
					Selection.nurbs_curves.push_back(k3d::mesh_selection::record(curve, 1.0));
					continue;
				}

				const size_t point_begin = curve_first_points[curve];
				const size_t point_end = point_begin + curve_point_counts[curve];
				for(size_t point = point_begin; point != point_end; ++point)
				{
					if(point_selection[curve_points[point]])
						Selection.nurbs_curves.push_back(k3d::mesh_selection::record(curve, 1.0));
				}
			}
		}

		// Convert face selections to edge selections ...
		if(Mesh.polyhedra && Mesh.polyhedra->face_first_loops && Mesh.polyhedra->face_loop_counts && Mesh.polyhedra->face_selection && Mesh.polyhedra->loop_first_edges && Mesh.polyhedra->clockwise_edges)
		{
			const k3d::mesh::indices_t& face_first_loops = *Mesh.polyhedra->face_first_loops;
			const k3d::mesh::counts_t& face_loop_counts = *Mesh.polyhedra->face_loop_counts;
			const k3d::mesh::selection_t& face_selection = *Mesh.polyhedra->face_selection;
			const k3d::mesh::indices_t& loop_first_edges = *Mesh.polyhedra->loop_first_edges;
			const k3d::mesh::indices_t& clockwise_edges = *Mesh.polyhedra->clockwise_edges;

			const size_t face_begin = 0;
			const size_t face_end = face_begin + face_first_loops.size();
			for(size_t face = face_begin; face != face_end; ++face)
			{
				if(!face_selection[face])
					continue;

				const size_t face_loop_begin = face_first_loops[face];
				const size_t face_loop_end = face_loop_begin + face_loop_counts[face];
				for(size_t face_loop = face_loop_begin; face_loop != face_loop_end; ++face_loop)
				{
					const size_t first_edge = loop_first_edges[face_loop];
					for(size_t edge = first_edge; ; )
					{
						Selection.edges.push_back(k3d::mesh_selection::record(edge, 1.0));

						edge = clockwise_edges[edge];
						if(edge == first_edge)
							break;
					}
				}
			}
		}

		if (!m_keep_selection)
		{
			Selection.points.clear();
			Selection.faces.clear();
			Selection.bilinear_patches.clear();
			Selection.bicubic_patches.clear();
			Selection.nurbs_patches.clear();
		}

		// Ensure that anything not explicitly selected gets explicitly deselected ...
		deselect_gaps(Selection);
	}
	bool m_keep_selection;
};

/** \todo Handle adjacent edge selections */
struct convert_to_faces
{
	convert_to_faces(bool KeepSelection) : m_keep_selection(KeepSelection) {}
	void operator()(const k3d::mesh& Mesh, k3d::mesh_selection& Selection) const
	{
		// Convert point and edge selections to face selections ...
		if(Mesh.point_selection && Mesh.polyhedra && Mesh.polyhedra->face_first_loops && Mesh.polyhedra->face_loop_counts && Mesh.polyhedra->face_selection && Mesh.polyhedra->loop_first_edges && Mesh.polyhedra->edge_points && Mesh.polyhedra->clockwise_edges && Mesh.polyhedra->edge_selection)
		{
			const size_t face_begin = 0;
			const size_t face_end = face_begin + Mesh.polyhedra->face_first_loops->size();
			for(size_t face = face_begin; face != face_end; ++face)
			{
				if((*Mesh.polyhedra->face_selection)[face])
				{
					Selection.faces.push_back(k3d::mesh_selection::record(face, 1.0));
					continue;
				}

				const size_t face_loop_begin = (*Mesh.polyhedra->face_first_loops)[face];
				const size_t face_loop_end = face_loop_begin + (*Mesh.polyhedra->face_loop_counts)[face];
				for(size_t face_loop = face_loop_begin; face_loop != face_loop_end; ++face_loop)
				{
					const size_t first_edge = (*Mesh.polyhedra->loop_first_edges)[face_loop];
					for(size_t edge = first_edge; ; )
					{
						if((*Mesh.polyhedra->edge_selection)[edge] || (*Mesh.point_selection)[(*Mesh.polyhedra->edge_points)[edge]])
						{
							Selection.faces.push_back(k3d::mesh_selection::record(face, 1.0));
							break;
						}

						edge = (*Mesh.polyhedra->clockwise_edges)[edge];
						if(edge == first_edge)
							break;
					}
				}
			}
		}
		
		// Convert point selections to bilinear patch selections ...
		if(Mesh.point_selection && Mesh.bilinear_patches && Mesh.bilinear_patches->patch_selection && Mesh.bilinear_patches->patch_points)
		{
			const size_t patch_begin = 0;
			const size_t patch_end = patch_begin + (Mesh.bilinear_patches->patch_points->size() / 4);
			for(size_t patch = patch_begin; patch != patch_end; ++patch)
			{
				if((*Mesh.bilinear_patches->patch_selection)[patch])
				{
					Selection.bilinear_patches.push_back(k3d::mesh_selection::record(patch, 1.0));
					continue;
				}

				const size_t patch_point_begin = patch * 4;
				const size_t patch_point_end = patch_point_begin + 4;
				for(size_t patch_point = patch_point_begin; patch_point != patch_point_end; ++patch_point)
				{
					if((*Mesh.point_selection)[(*Mesh.bilinear_patches->patch_points)[patch_point]])
					{
						Selection.bilinear_patches.push_back(k3d::mesh_selection::record(patch, 1.0));
						break;
					}
				}
			}
		}

		// Convert point selections to bicubic patch selections ...
		if(Mesh.point_selection && Mesh.bicubic_patches && Mesh.bicubic_patches->patch_selection && Mesh.bicubic_patches->patch_points)
		{
			const size_t patch_begin = 0;
			const size_t patch_end = patch_begin + (Mesh.bicubic_patches->patch_points->size() / 16);
			for(size_t patch = patch_begin; patch != patch_end; ++patch)
			{
				if((*Mesh.bicubic_patches->patch_selection)[patch])
				{
					Selection.bicubic_patches.push_back(k3d::mesh_selection::record(patch, 1.0));
					continue;
				}

				const size_t patch_point_begin = patch * 16;
				const size_t patch_point_end = patch_point_begin + 16;
				for(size_t patch_point = patch_point_begin; patch_point != patch_point_end; ++patch_point)
				{
					if((*Mesh.point_selection)[(*Mesh.bicubic_patches->patch_points)[patch_point]])
					{
						Selection.bicubic_patches.push_back(k3d::mesh_selection::record(patch, 1.0));
						break;
					}
				}
			}
		}

		// Convert point selections to nurbs patch selections ...
		if(Mesh.point_selection && Mesh.nurbs_patches && Mesh.nurbs_patches->patch_first_points && Mesh.nurbs_patches->patch_u_point_counts && Mesh.nurbs_patches->patch_v_point_counts && Mesh.nurbs_patches->patch_selection && Mesh.nurbs_patches->patch_points)
		{
			const size_t patch_begin = 0;
			const size_t patch_end = patch_begin + Mesh.nurbs_patches->patch_first_points->size();
			for(size_t patch = patch_begin; patch != patch_end; ++patch)
			{
				if((*Mesh.nurbs_patches->patch_selection)[patch])
				{
					Selection.nurbs_patches.push_back(k3d::mesh_selection::record(patch, 1.0));
					continue;
				}

				const size_t patch_point_begin = (*Mesh.nurbs_patches->patch_first_points)[patch];
				const size_t patch_point_end = patch_point_begin + ((*Mesh.nurbs_patches->patch_u_point_counts)[patch] * (*Mesh.nurbs_patches->patch_v_point_counts)[patch]);
				for(size_t patch_point = patch_point_begin; patch_point != patch_point_end; ++patch_point)
				{
					if((*Mesh.point_selection)[(*Mesh.nurbs_patches->patch_points)[patch_point]])
					{
						Selection.nurbs_patches.push_back(k3d::mesh_selection::record(patch, 1.0));
						break;
					}
				}
			}
		}

		if (!m_keep_selection)
		{
			Selection.points.clear();
			Selection.edges.clear();
			Selection.linear_curves.clear();
			Selection.cubic_curves.clear();
			Selection.nurbs_curves.clear();
		}

		// Ensure that anything not explicitly selected gets explicitly deselected ...
		deselect_gaps(Selection);
	}
	bool m_keep_selection;
};

/// Keeps the existing components explicitely selected
struct keep_selection
{
	void operator()(const k3d::mesh& Mesh, k3d::mesh_selection& Selection) const
	{
		if(Mesh.points && Mesh.point_selection)
		{
			for (k3d::uint_t point = 0; point != Mesh.point_selection->size(); ++point)
			{
				if (Mesh.point_selection->at(point))
					Selection.points.push_back(k3d::mesh_selection::record(point, 1.0));
			}
		}
		
		if (Mesh.polyhedra && Mesh.polyhedra->edge_selection)
		{
			for (k3d::uint_t edge = 0; edge != Mesh.polyhedra->edge_selection->size(); ++edge)
			{
				if (Mesh.polyhedra->edge_selection->at(edge))
					Selection.edges.push_back(k3d::mesh_selection::record(edge, 1.0));
			}
		}
		
		if (Mesh.polyhedra && Mesh.polyhedra->face_selection)
		{
			for (k3d::uint_t face = 0; face != Mesh.polyhedra->face_selection->size(); ++face)
			{
				if (Mesh.polyhedra->face_selection->at(face))
					Selection.faces.push_back(k3d::mesh_selection::record(face, 1.0));
			}
		}
		
		if (Mesh.linear_curve_groups && Mesh.linear_curve_groups->curve_selection)
		{
			for (k3d::uint_t curve = 0; curve != Mesh.linear_curve_groups->curve_selection->size(); ++curve)
			{
				if (Mesh.linear_curve_groups->curve_selection->at(curve))
					Selection.linear_curves.push_back(k3d::mesh_selection::record(curve, 1.0));
			}
		}
		
		if (Mesh.cubic_curve_groups && Mesh.cubic_curve_groups->curve_selection)
		{
			for (k3d::uint_t curve = 0; curve != Mesh.cubic_curve_groups->curve_selection->size(); ++curve)
			{
				if (Mesh.cubic_curve_groups->curve_selection->at(curve))
					Selection.cubic_curves.push_back(k3d::mesh_selection::record(curve, 1.0));
			}
		}
		
		if (Mesh.nurbs_curve_groups && Mesh.nurbs_curve_groups->curve_selection)
		{
			for (k3d::uint_t curve = 0; curve != Mesh.nurbs_curve_groups->curve_selection->size(); ++curve)
			{
				if (Mesh.nurbs_curve_groups->curve_selection->at(curve))
					Selection.nurbs_curves.push_back(k3d::mesh_selection::record(curve, 1.0));
			}
		}
		
		if (Mesh.bilinear_patches && Mesh.bilinear_patches->patch_selection)
		{
			for (k3d::uint_t patch = 0; patch != Mesh.bilinear_patches->patch_selection->size(); ++patch)
			{
				if (Mesh.bilinear_patches->patch_selection->at(patch))
					Selection.bilinear_patches.push_back(k3d::mesh_selection::record(patch, 1.0));
			}
		}
		
		if (Mesh.bicubic_patches && Mesh.bicubic_patches->patch_selection)
		{
			for (k3d::uint_t patch = 0; patch != Mesh.bicubic_patches->patch_selection->size(); ++patch)
			{
				if (Mesh.bicubic_patches->patch_selection->at(patch))
					Selection.bicubic_patches.push_back(k3d::mesh_selection::record(patch, 1.0));
			}
		}
		
		if (Mesh.nurbs_patches && Mesh.nurbs_patches->patch_selection)
		{
			for (k3d::uint_t patch = 0; patch != Mesh.nurbs_patches->patch_selection->size(); ++patch)
			{
				if (Mesh.nurbs_patches->patch_selection->at(patch))
					Selection.nurbs_patches.push_back(k3d::mesh_selection::record(patch, 1.0));
			}
		}
	}
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// document_state::implementation

class document_state::implementation :
	public sigc::trackable/*,
	public k3d::ipersistent*/
{
public:
	implementation(k3d::idocument& Document) :
		m_document(Document),
		m_gdkgl_share_list(0),
		m_selection_mode(init_owner(*this) + init_name("selection_mode") + init_label(_("Selection Type")) + init_description(_("Sets selection mode (nodes, faces, edges, points, etc)")) + init_value(SELECT_NODES) + init_values(detail::selection_mode_values())),
		m_last_selection_mode(SELECT_NODES),
		m_active_tool(0),
		m_selection_tool(0),
		m_move_tool(0),
		m_rotate_tool(0),
		m_scale_tool(0),
		m_context_menu_node("context_menu", dynamic_cast<k3d::icommand_node*>(&Document)),
		m_context_menu(0)
	{
		m_selection_mode.connect_explicit_change_signal(sigc::mem_fun(*this, &implementation::on_selection_mode_changed));

		// Process remove_nodes_signal
		m_document.nodes().remove_nodes_signal().connect(sigc::mem_fun(*this, &implementation::on_nodes_removed));
	}

	~implementation()
	{
		while(!m_tools.empty())
		{
			delete m_tools.begin()->second;
			m_tools.erase(m_tools.begin());
		}

		delete m_context_menu;
		delete m_scale_tool;
		delete m_rotate_tool;
		delete m_move_tool;
		delete m_selection_tool;
	}

	/// Returns the document
	k3d::idocument& document()
	{
		return m_document;
	}

	/** \todo Restore UI serialization */
/*
	void save(k3d::xml::element& Element, const k3d::ipersistent::save_context& Context)
	{
		save_command_node(dynamic_cast<k3d::icommand_node&>(m_document), Element.append(k3d::xml::element("nodes")), Context);
	}

	void save_command_node(k3d::icommand_node& Node, k3d::xml::element& Element, const k3d::ipersistent::save_context& Context)
	{
		if(k3d::ipersistent* const persistent = dynamic_cast<k3d::ipersistent*>(&Node))
			persistent->save(Element.append(k3d::xml::element("node", k3d::xml::attribute("path", k3d::command_node::path(Node)))), Context);

		const k3d::icommand_tree::nodes_t children = k3d::command_tree().children(&Node);
		for(k3d::icommand_tree::nodes_t::const_iterator child = children.begin(); child != children.end(); ++child)
			save_command_node(**child, Element, Context);
	}

	void load(k3d::xml::element& Element, const k3d::ipersistent::load_context& Context)
	{
		k3d::xml::element* const xml_nodes = k3d::xml::find_element(Element, "nodes");
		if(!xml_nodes)
			return;

		for(k3d::xml::element::elements_t::iterator xml_node = xml_nodes->children.begin(); xml_node != xml_nodes->children.end(); ++xml_node)
		{
			if(xml_node->name != "node")
				continue;

			const std::string path = k3d::xml::attribute_text(*xml_node, "path");
			k3d::icommand_node* const node = k3d::command_node::lookup(path);
			if(!node)
			{
				k3d::log() << error << "Could not find command node [" << path << "]" << std::endl;
				continue;
			}

			if(k3d::ipersistent* const persistent = dynamic_cast<k3d::ipersistent*>(node))
				persistent->load(*xml_node, Context);
		}
	}
*/

#ifndef K3D_API_WIN32

	/// Returns a "global" (to the document) gdkgl context that can be used to share display lists
	GdkGLContext* gdkgl_share_list()
	{
		if(!m_gdkgl_share_list)
		{
			GdkGLConfig* const config = gdk_gl_config_new_by_mode(
				static_cast<GdkGLConfigMode>(GDK_GL_MODE_RGBA | GDK_GL_MODE_DOUBLE | GDK_GL_MODE_DEPTH));
			return_val_if_fail(config, 0);

			GdkPixmap* const pixmap = gdk_pixmap_new(0, 8, 8, gdk_gl_config_get_depth(config));
			return_val_if_fail(pixmap, 0);

			GdkGLPixmap* const glpixmap = gdk_pixmap_set_gl_capability(pixmap, config, 0);
			return_val_if_fail(glpixmap, 0);

			GdkGLContext* const context = gdk_gl_context_new(
				gdk_pixmap_get_gl_drawable(pixmap), 0, true, GDK_GL_RGBA_TYPE);
			return_val_if_fail(context, 0);

			m_gdkgl_share_list = context;
		}

		return m_gdkgl_share_list;
	}

#else // !K3D_API_WIN32

	/// Returns a "global" (to the document) gdkgl context that can be used to share display lists
	GdkGLContext* gdkgl_share_list()
	{
		if(!m_gdkgl_share_list)
		{
			GdkGLConfig* const config = gdk_gl_config_new_by_mode(
				static_cast<GdkGLConfigMode>(GDK_GL_MODE_RGBA | GDK_GL_MODE_DOUBLE | GDK_GL_MODE_DEPTH));
			return_val_if_fail(config, 0);

			Gtk::Window* const window = new Gtk::Window();
			gtk_widget_set_gl_capability(GTK_WIDGET(window->gobj()), config, 0, true, GDK_GL_RGBA_TYPE);
			window->show();
			window->hide();

			GdkGLContext* const context = gtk_widget_get_gl_context(GTK_WIDGET(window->gobj()));
			return_val_if_fail(context, 0);

			m_gdkgl_share_list = context;
		}

		return m_gdkgl_share_list;
	}

#endif // K3D_API_WIN32

	/// Returns a signal that can be emitted to request display of the history for an node
	view_node_history_signal_t& view_node_history_signal()
	{
		return m_view_node_history_signal;
	}

	/// Returns a signal that can be emitted to request display of the properties for an node
	view_node_properties_signal_t& view_node_properties_signal()
	{
		return m_view_node_properties_signal;
	}

	/// Returns a signal that can be emitted to request display of the properties for a tool
	view_tool_properties_signal_t& view_tool_properties_signal()
	{
		return m_view_tool_properties_signal;
	}

	/// Returns a signal that can be emitted to acknowledge of a document selection change
	document_selection_change_signal_t& document_selection_change_signal()
	{
		return m_document_selection_change_signal;
	}

	/// Returns the active tool for the document
	tool& active_tool()
	{
		return *m_active_tool;
	}

	/// Sets the active tool for the document
	void set_active_tool(tool& ActiveTool)
	{
		m_active_tool->deactivate();
		m_active_tool = &ActiveTool;
		m_active_tool->activate();
		m_active_tool_changed_signal.emit(0);
	}

	active_tool_changed_signal_t& active_tool_changed_signal()
	{
		return m_active_tool_changed_signal;
	}

	/// Returns the current document-wide selection mode
	selection_mode_property_t& selection_mode()
	{
		return m_selection_mode;
	}

	void set_selection_mode(selection_mode_t Mode)
	{
		m_selection_mode.set_value(Mode);
	}

	/// Called by the signal system anytime nodes are removed from the document
	void on_nodes_removed(const k3d::inode_collection::nodes_t& Nodes)
	{
		selection_changed();
	}

	typedef std::set<k3d::legacy::mesh*> meshes_t;
	typedef std::set<k3d::inode*> node_selection_t;

	const bool is_selected(k3d::inode* Node)
	{
		if(k3d::iselectable* selectable = dynamic_cast<k3d::iselectable*>(Node))
		{
			if(selectable->get_selection_weight())
				return true;
		}

		return false;
	}

	const bool is_selected(const k3d::selection::record& Record)
	{
		switch(m_selection_mode.internal_value())
		{
			case SELECT_NODES:
				return detail::is_node_selected(Record);
			case SELECT_POINTS:
				return detail::is_point_selected(Record);
			case SELECT_LINES:
				return detail::is_line_selected(Record);
			case SELECT_FACES:
				return detail::is_face_selected(Record);
		}

		return false;
	}

	void selection_changed()
	{
		// Switch to node selection mode when there's no selected mesh in the document
		if(SELECT_NODES != m_selection_mode.internal_value())
		{
			unsigned long selected_mesh_count = 0;

			const k3d::nodes_t nodes = selected_nodes();
			for(k3d::nodes_t::const_iterator node = nodes.begin(); node != nodes.end(); ++node)
			{
/** \todo Restore this logic */
//				if(dynamic_cast<k3d::imesh_source*>(*node))
				{
					++selected_mesh_count;
					break;
				}
			}

			if(!selected_mesh_count)
				set_selection_mode(SELECT_NODES);
		}

		m_active_tool->document_selection_changed();
		m_document_selection_change_signal.emit();
	}

	void select_points(const k3d::selection::records& Selection)
	{
		detail::select_components(Selection, detail::select_points(1.0));
	}

	void select_lines(const k3d::selection::records& Selection)
	{
		detail::select_components(Selection, detail::select_lines(1.0));
	}

	void select_faces(const k3d::selection::records& Selection)
	{
		detail::select_components(Selection, detail::select_faces(1.0));
	}

	void select_nodes(const k3d::selection::records& Selection)
	{
		unsigned long selected_nodes = 0;
		k3d::inode* selected_node = 0;

		for(k3d::selection::records::const_iterator record = Selection.begin(); record != Selection.end(); ++record)
		{
			if(k3d::inode* const node = k3d::selection::get_node(*record))
			{
				k3d::selection::select(node);

				++selected_nodes;
				selected_node = node;
			}
		}

		// Iff one node was selected, send "view node" signals
		if(1 == selected_nodes && selected_node)
		{
			m_view_node_properties_signal.emit(selected_node);
			m_view_node_history_signal.emit(selected_node);

		}
	}

	void select(const k3d::selection::records& Selection)
	{
		switch(m_selection_mode.internal_value())
		{
			case SELECT_NODES:
				select_nodes(Selection);
				break;
			case SELECT_POINTS:
				select_points(Selection);
				break;
			case SELECT_LINES:
				select_lines(Selection);
				break;
			case SELECT_FACES:
				select_faces(Selection);
				break;
		}

		selection_changed();
	}

	void select_all_nodes()
	{
		const k3d::nodes_t& nodes = m_document.nodes().collection();
		for(k3d::nodes_t::const_iterator node = nodes.begin(); node != nodes.end(); ++node)
			k3d::selection::select(*node);
	}

	void select_all_points()
	{
		detail::update_component_selection(selected_nodes(), detail::select_all_points(), true);
	}

	void select_all_lines()
	{
		detail::update_component_selection(selected_nodes(), detail::select_all_lines(), true);
	}

	void select_all_faces()
	{
		detail::update_component_selection(selected_nodes(), detail::select_all_faces(), true);
	}

	void select_all()
	{
		switch(m_selection_mode.internal_value())
		{
			case SELECT_NODES:
				select_all_nodes();
				break;
			case SELECT_POINTS:
				select_all_points();
				break;
			case SELECT_LINES:
				select_all_lines();
				break;
			case SELECT_FACES:
				select_all_faces();
				break;
		}

		selection_changed();
	}

	void deselect_points(const k3d::selection::records& Selection)
	{
		detail::select_components(Selection, detail::select_points(0.0));
	}

	void deselect_lines(const k3d::selection::records& Selection)
	{
		detail::select_components(Selection, detail::select_lines(0.0));
	}

	void deselect_faces(const k3d::selection::records& Selection)
	{
		detail::select_components(Selection, detail::select_faces(0.0));
	}

	void deselect_nodes(const k3d::selection::records& Selection)
	{
		for(k3d::selection::records::const_iterator record = Selection.begin(); record != Selection.end(); ++record)
		{
			if(k3d::inode* const node = k3d::selection::get_node(*record))
				k3d::selection::deselect(node);
		}
	}

	void deselect(const k3d::selection::records& Selection)
	{
		switch(m_selection_mode.internal_value())
		{
			case SELECT_NODES:
				deselect_nodes(Selection);
				break;
			case SELECT_POINTS:
				deselect_points(Selection);
				break;
			case SELECT_LINES:
				deselect_lines(Selection);
				break;
			case SELECT_FACES:
				deselect_faces(Selection);
				break;
		}

		selection_changed();
	}

	void deselect_all_nodes()
	{
		for(k3d::inode_collection::nodes_t::const_iterator node = m_document.nodes().collection().begin(); node != m_document.nodes().collection().end(); ++node)
			k3d::selection::deselect(*node);
	}

	void deselect_all_components()
	{
		detail::update_component_selection(m_document.nodes().collection(), detail::deselect_all(), true);
	}

	void deselect_all()
	{
		switch(m_selection_mode.internal_value())
		{
			case SELECT_NODES:
				deselect_all_nodes();
				break;
			case SELECT_POINTS:
			case SELECT_LINES:
			case SELECT_FACES:
				deselect_all_components();
				break;
		}

		selection_changed();
	}

	void invert_node_selection()
	{
		for(k3d::inode_collection::nodes_t::const_iterator node = m_document.nodes().collection().begin(); node != m_document.nodes().collection().end(); ++node)
		{
			if(k3d::selection::is_selected(*node))
				k3d::selection::deselect(*node);
			else
				k3d::selection::select(*node);
		}
	}

	void invert_point_selection()
	{
		detail::update_component_selection(m_document.nodes().collection(), detail::invert_points(), true);
	}

	void invert_line_selection()
	{
		detail::update_component_selection(m_document.nodes().collection(), detail::invert_lines(), true);
	}

	void invert_face_selection()
	{
		detail::update_component_selection(m_document.nodes().collection(), detail::invert_faces(), true);
	}

	void invert_selection()
	{
		switch(m_selection_mode.internal_value())
		{
			case SELECT_NODES:
				invert_node_selection();
				break;
			case SELECT_POINTS:
				invert_point_selection();
				break;
			case SELECT_LINES:
				invert_line_selection();
				break;
			case SELECT_FACES:
				invert_face_selection();
				break;
		}

		selection_changed();
	}

	const k3d::nodes_t selected_nodes()
	{
		k3d::nodes_t results;

		for(k3d::inode_collection::nodes_t::const_iterator node = m_document.nodes().collection().begin(); node != m_document.nodes().collection().end(); ++node)
		{
			if(is_selected(*node))
				results.push_back(*node);
		}

		return results;
	}

	void hide_selection()
	{
		const k3d::nodes_t nodes = selected_nodes();
		for(k3d::nodes_t::const_iterator node = nodes.begin(); node != nodes.end(); ++node)
		{
			k3d::selection::select(*node);
			k3d::property::set_internal_value(**node, "viewport_visible", false);
			k3d::property::set_internal_value(**node, "render_final", false);
		}
	}

	void show_selection()
	{
		const k3d::nodes_t nodes = selected_nodes();
		for(k3d::nodes_t::const_iterator node = nodes.begin(); node != nodes.end(); ++node)
		{
			k3d::property::set_internal_value(**node, "viewport_visible", true);
			k3d::property::set_internal_value(**node, "render_final", true);
		}
	}

	void hide_unselected()
	{
		for(k3d::inode_collection::nodes_t::const_iterator node = m_document.nodes().collection().begin(); node != m_document.nodes().collection().end(); ++node)
		{
			if(!is_selected(*node))
			{
				k3d::property::set_internal_value(**node, "viewport_visible", false);
				k3d::property::set_internal_value(**node, "render_final", false);
			}
		}
	}

	void show_all_nodes()
	{
		for(k3d::inode_collection::nodes_t::const_iterator node = m_document.nodes().collection().begin(); node != m_document.nodes().collection().end(); ++node)
		{
			k3d::property::set_internal_value(**node, "viewport_visible", true);
			k3d::property::set_internal_value(**node, "render_final", true);
		}
	}

	document_state::set_cursor_signal_t& set_cursor_signal()
	{
		return m_set_cursor_signal;
	}

	document_state::clear_cursor_signal_t& clear_cursor_signal()
	{
		return m_clear_cursor_signal;
	}

	document_state::push_status_message_signal_t& push_status_message_signal()
	{
		return m_push_status_message_signal;
	}

	document_state::pop_status_message_signal_t& pop_status_message_signal()
	{
		return m_pop_status_message_signal;
	}

	k3d::inode* default_gl_painter()
	{
		const k3d::nodes_t nodes = k3d::find_nodes(m_document.nodes(), "GL Default Painter");
	        return (1 == nodes.size()) ? *nodes.begin() : 0;
	}

	k3d::inode* default_ri_painter()
	{
		const k3d::nodes_t nodes = k3d::find_nodes(m_document.nodes(), "RenderMan Default Painter");
	        return (1 == nodes.size()) ? *nodes.begin() : 0;
	}

	k3d::inode* create_node(k3d::iplugin_factory* Factory)
	{
		return_val_if_fail(Factory, 0);

		// Switch to node selection mode
		if(SELECT_NODES != m_selection_mode.internal_value())
			set_selection_mode(SELECT_NODES);

		// Create the requested node ...
		k3d::record_state_change_set changeset(m_document, k3d::string_cast(boost::format(_("Create %1%")) % Factory->name()), K3D_CHANGE_SET_CONTEXT);
		const std::string node_name = k3d::unique_name(m_document.nodes(), Factory->name());
		k3d::inode* const node = k3d::plugin::create<k3d::inode>(*Factory, m_document, node_name);
		return_val_if_fail(node, 0);

		// Keep track of the node to be selected ...
		k3d::inode* to_be_selected = node;

		// Keep track of every new node created ...
		std::vector<k3d::inode*> new_nodes;
		new_nodes.push_back(node);

		// Prepare to make connections to other nodes ...
		k3d::ipipeline::dependencies_t dependencies;

		// If the new node is a mesh source (but not a MeshInstance!), create a MeshInstance node and attach it so it's immediately visible ...
		k3d::imesh_source* const mesh_source = dynamic_cast<k3d::imesh_source*>(node);
		if(mesh_source && k3d::classes::MeshInstance() != Factory->factory_id())
		{
			// Create a mesh instance ...
			k3d::inode* const mesh_instance = k3d::plugin::create<k3d::inode>(k3d::classes::MeshInstance(), m_document, k3d::unique_name(m_document.nodes(), node_name + " Instance"));

			new_nodes.push_back(mesh_instance);

			// Assign a default painter ...
			k3d::property::set_internal_value(*mesh_instance, "gl_painter", default_gl_painter());
			k3d::property::set_internal_value(*mesh_instance, "ri_painter", default_ri_painter());

			// Connect the mesh instance to the source ...
			k3d::imesh_sink* const mesh_sink = dynamic_cast<k3d::imesh_sink*>(mesh_instance);
			if(mesh_sink)
				dependencies.insert(std::make_pair(&mesh_sink->mesh_sink_input(), &mesh_source->mesh_source_output()));

			// In this case, we want to select the mesh instance instead of the mesh source ...
			to_be_selected = mesh_instance;
		}

		// If the new node is a material sink, assign a default material ...
		if(k3d::imaterial_sink* const material_sink = dynamic_cast<k3d::imaterial_sink*>(node))
		{
			const k3d::nodes_t materials = k3d::find_nodes<k3d::imaterial>(m_document.nodes());
			if(materials.size())
				k3d::property::set_internal_value(material_sink->material_sink_input(), dynamic_cast<k3d::inode*>(*materials.rbegin()));
		}

		// If the new node is a time sink, connect it to the document time source ...
		if(k3d::itime_sink* const time_sink = dynamic_cast<k3d::itime_sink*>(node))
			dependencies.insert(std::make_pair(&time_sink->time_sink_input(), k3d::get_time(m_document)));

		if(!dependencies.empty())
			m_document.pipeline().set_dependencies(dependencies);

		// If the new node is a camera, orient it horizontally
		if(k3d::classes::Camera() == Factory->factory_id())
			k3d::set_matrix(*node, k3d::rotation3D(k3d::radians(90.0), k3d::vector3(1, 0, 0)));
		
		// If the new node is a multiple mesh sink, add two mesh inputs
		if(Factory->implements(typeid(k3d::imulti_mesh_sink)))
		{
			k3d::property::create<k3d::mesh*>(*node, "input1", "Input 1", "", static_cast<k3d::mesh*>(0));
			k3d::property::create<k3d::mesh*>(*node, "input2", "Input 2", "", static_cast<k3d::mesh*>(0));
		}

		// If the new node is a render-engine, default to making every node in the document visible ...
		if(k3d::inode_collection_sink* const node_collection_sink = dynamic_cast<k3d::inode_collection_sink*>(node))
		{
			const k3d::inode_collection_sink::properties_t properties = node_collection_sink->node_collection_properties();
			for(k3d::inode_collection_sink::properties_t::const_iterator property = properties.begin(); property != properties.end(); ++property)
				k3d::property::set_internal_value(**property, document().nodes().collection());
		}

		// By default, make the new node visible in any node collection sinks that already exist ...
		const k3d::inode_collection::nodes_t::const_iterator doc_node_end = document().nodes().collection().end();
		for(k3d::inode_collection::nodes_t::const_iterator doc_node = document().nodes().collection().begin(); doc_node != doc_node_end; ++doc_node)
		{
			if(k3d::inode_collection_sink* const node_collection_sink = dynamic_cast<k3d::inode_collection_sink*>(*doc_node))
			{
				const k3d::inode_collection_sink::properties_t properties = node_collection_sink->node_collection_properties();
				for(k3d::inode_collection_sink::properties_t::const_iterator property = properties.begin(); property != properties.end(); ++property)
				{
					if(k3d::inode_collection_property* const node_collection_property = dynamic_cast<k3d::inode_collection_property*>(*property))
					{
						k3d::inode_collection_property::nodes_t nodes = k3d::property::internal_value<k3d::inode_collection_property::nodes_t>(**property);
						for(k3d::uint_t i = 0; i != new_nodes.size(); ++i)
						{
							if(node_collection_property->property_allow(*new_nodes[i]))
								nodes.push_back(new_nodes[i]);
						}
						k3d::property::set_internal_value(**property, nodes);
					}
				}
			}
		}

		// Replace the current selection
		deselect_all();
		select(k3d::selection::make_records(to_be_selected));

		// Make the newly-created node properties visible ...
		view_node_properties_signal().emit(node);

		k3d::gl::redraw_all(m_document, k3d::gl::irender_viewport::ASYNCHRONOUS);

		return node;
	}

	void popup_context_menu(const bool UserAction)
	{
		if(UserAction)
			m_context_menu->popup(1, gtk_get_current_event_time());
		else
			m_context_menu->popup(0, GDK_CURRENT_TIME);
	}

	/// Sets the current selection when node selection mode is chosen
	void on_set_node_mode()
	{
		if (!m_selection_tool->keep_selection())
		{
			detail::update_component_selection(selected_nodes(), detail::deselect_all(), true);
		}
		else
		{
			detail::update_component_selection(selected_nodes(), detail::keep_selection(), true);
		}
	}

	/// Sets the current selection when point selection mode is chosen
	void on_set_point_mode()
	{
		if (m_selection_tool->convert_selection())
			detail::update_component_selection(selected_nodes(), detail::convert_to_points(m_selection_tool->keep_selection()), true);
		else
			detail::update_component_selection(selected_nodes(), detail::keep_selection(), true);
	}

	/// Sets the current selection when line selection mode is chosen
	void on_set_line_mode()
	{
		if (m_selection_tool->convert_selection())
			detail::update_component_selection(selected_nodes(), detail::convert_to_lines(m_selection_tool->keep_selection()), true);
		else
			detail::update_component_selection(selected_nodes(), detail::keep_selection(), true);
	}

	/// Sets the current selection when face selection mode is chosen
	void on_set_face_mode()
	{
		if (m_selection_tool->convert_selection())
			detail::update_component_selection(selected_nodes(), detail::convert_to_faces(m_selection_tool->keep_selection()), true);
		else
			detail::update_component_selection(selected_nodes(), detail::keep_selection(), true);
	}

	/// Called by the signal system when the selection mode changes
	void on_selection_mode_changed(k3d::iunknown*)
	{
		if (!m_selection_tool->keep_selection() && !m_selection_tool->convert_selection())
		{
			detail::update_component_selection(selected_nodes(), detail::deselect_all(), true);
		}
		switch(m_selection_mode.internal_value())
		{
			case SELECT_NODES:
				on_set_node_mode();
				break;
			case SELECT_POINTS:
				on_set_point_mode();
				break;
			case SELECT_LINES:
				on_set_line_mode();
				break;
			case SELECT_FACES:
				on_set_face_mode();
				break;
		}

		m_last_selection_mode = m_selection_mode.internal_value();

		selection_changed();
	}

	/// Stores a reference to the owning document
	k3d::idocument& m_document;
	sigc::signal<bool, k3d::signal::cancelable> m_safe_to_close_signal;
	/// Stores an unused off-screen pixmap so we can create a global gdkgl context for sharing display lists
	Glib::RefPtr<Gdk::Pixmap> m_gdkgl_share_pixmap;
	/// Stores a gdkgl context that can be used to share display lists for the entire document
	GdkGLContext* m_gdkgl_share_list;
	/// A signal that can be emitted to request display of the history for an node
	view_node_history_signal_t m_view_node_history_signal;
	/// A signal that can be emitted to request display of the properties for an node
	view_node_properties_signal_t m_view_node_properties_signal;
	/// A signal that can be emitted to request display of the properties for a tool
	view_tool_properties_signal_t m_view_tool_properties_signal;
	/// A signal that can be emitted to acknowledge of a document selection change
	document_selection_change_signal_t m_document_selection_change_signal;
	/// A signal that will be emitted whenever the active tool changes
	active_tool_changed_signal_t m_active_tool_changed_signal;
	/// Stores the current document-wide selection mode
	selection_mode_property_t m_selection_mode;
	/// Stores the last document-wide selection mode
	selection_mode_t m_last_selection_mode;

	document_state::set_cursor_signal_t m_set_cursor_signal;
	document_state::clear_cursor_signal_t m_clear_cursor_signal;
	document_state::push_status_message_signal_t m_push_status_message_signal;
	document_state::pop_status_message_signal_t m_pop_status_message_signal;

	/// Store a reference to the current active tool
	tool* m_active_tool;

	/// Store builtin tools (deprecated)
	libk3dngui::selection_tool* m_selection_tool;
	tool* m_move_tool;
	tool* m_rotate_tool;
	tool* m_scale_tool;
	tool* m_snap_tool;

	/// Store plugin tools
	std::map<k3d::string_t, tool*> m_tools;

	/// Placeholder that "groups" context menu command nodes together
	k3d::command_node::implementation m_context_menu_node;
	/// Document wide context menu
	Gtk::Menu* m_context_menu;

	/// Used to keep documents sorted in the "safe to close" dialog
	struct sort_by_title
	{
		bool operator()(const safe_close_dialog::entry& LHS, const safe_close_dialog::entry& RHS)
		{
			return LHS.document->unsaved_document_title() < RHS.document->unsaved_document_title();
		}
	};

	typedef sigc::signal<unsaved_document*> safe_close_signal_t;
	/// Signal that will be emitted prior to safe shutdown to "gather" unsaved documents
	safe_close_signal_t m_safe_close_signal;

	/// Stores auto-start plugins
	typedef std::vector<k3d::iunknown*> auto_start_plugins_t;
	/// Stores auto-start plugins
	auto_start_plugins_t m_auto_start_plugins;
};

/////////////////////////////////////////////////////////////////////////////
// document_state

document_state::document_state(k3d::idocument& Document) :
	m_implementation(new implementation(Document)),
	m_focus_viewport(0)
{
	// Create auto-start plugins for this document ...
	const k3d::iplugin_factory_collection::factories_t& factories = k3d::application().plugins();
	for(k3d::iplugin_factory_collection::factories_t::const_iterator factory = factories.begin(); factory != factories.end(); ++factory)
	{
		k3d::iplugin_factory::metadata_t metadata = (**factory).metadata();

		if(!metadata.count("ngui:document-start"))
			continue;

		k3d::log() << info << "Creating plugin [" << (**factory).name() << "] via ngui:document-start" << std::endl;

		k3d::iunknown* const plugin = k3d::plugin::create(**factory);
		if(!plugin)
		{
			k3d::log() << error << "Error creating plugin [" << (**factory).name() << "] via ngui:document-start" << std::endl;
			continue;
		}
		m_implementation->m_auto_start_plugins.push_back(plugin);

		if(k3d::iscripted_action* const scripted_action = dynamic_cast<k3d::iscripted_action*>(plugin))
		{
			k3d::iscript_engine::context_t context;
			context["Command"] = k3d::string_t("startup");
			context["Document"] = &Document;
			scripted_action->execute(context);
		}
	}

	Document.close_signal().connect(sigc::mem_fun(*this, &document_state::on_document_close));

	m_implementation->m_selection_tool = new libk3dngui::selection_tool(*this, "selection_tool");
	m_implementation->m_move_tool = new libk3dngui::move_tool(*this, "move_tool");
	m_implementation->m_rotate_tool = new libk3dngui::rotate_tool(*this, "rotate_tool");
	m_implementation->m_scale_tool = new libk3dngui::scale_tool(*this, "scale_tool");
	m_implementation->m_snap_tool = new libk3dngui::snap_tool(*this, "snap_tool");

	m_implementation->m_active_tool = m_implementation->m_selection_tool;
	m_implementation->m_active_tool->activate();

	m_implementation->m_context_menu =
		create_context_menu(*this, m_implementation->m_context_menu_node);
}

document_state::~document_state()
{
	// Shut-down any auto-start plugins associated with this document ...
	for(implementation::auto_start_plugins_t::iterator plugin = m_implementation->m_auto_start_plugins.begin(); plugin != m_implementation->m_auto_start_plugins.end(); ++plugin)
	{
		if(k3d::iscripted_action* const scripted_action = dynamic_cast<k3d::iscripted_action*>(*plugin))
		{
			k3d::iscript_engine::context_t context;
			context["Command"] = k3d::string_t("shutdown");
			context["Document"] = &document();
			scripted_action->execute(context);
		}
	}

	delete m_implementation;
}

void document_state::on_document_close()
{
	delete this;
}

k3d::idocument& document_state::document()
{
	return m_implementation->document();
}

sigc::connection document_state::connect_safe_close_signal(const sigc::slot<unsaved_document*>& Slot)
{
	return m_implementation->m_safe_close_signal.connect(Slot);
}

bool document_state::safe_close(Gtk::Window& Parent)
{
	if(k3d::batch_mode())
	{
		k3d::application().close_document(document());
		return true;
	}

	safe_close_dialog::entries_t entries;

	implementation::safe_close_signal_t::slot_list_type slots = m_implementation->m_safe_close_signal.slots();
	for(implementation::safe_close_signal_t::slot_list_type::iterator slot = slots.begin(); slot != slots.end(); ++slot)
	{
		unsaved_document* doc = (*slot)();
		if(doc && doc->unsaved_changes())
			entries.push_back(safe_close_dialog::entry(doc));
	}

	if(!entries.empty())
	{
		switch(safe_close_dialog::run(Parent, entries))
		{
			case Gtk::RESPONSE_NONE:
			case Gtk::RESPONSE_CANCEL:
			case Gtk::RESPONSE_DELETE_EVENT:
				return false;
			case Gtk::RESPONSE_CLOSE:
				k3d::application().close_document(document());
				return true;
			case Gtk::RESPONSE_OK:
				for(safe_close_dialog::entries_t::const_iterator entry = entries.begin(); entry != entries.end(); ++entry)
				{
					if(entry->save)
					{
						if(!entry->document->save_unsaved_changes())
							return false;
					}
				}
				k3d::application().close_document(document());
				return true;
		}
	}

	k3d::application().close_document(document());
	return true;
}

GdkGLContext* document_state::gdkgl_share_list()
{
	return m_implementation->gdkgl_share_list();
}

document_state::view_node_history_signal_t& document_state::view_node_history_signal()
{
	return m_implementation->view_node_history_signal();
}

document_state::view_node_properties_signal_t& document_state::view_node_properties_signal()
{
	return m_implementation->view_node_properties_signal();
}

document_state::view_tool_properties_signal_t& document_state::view_tool_properties_signal()
{
	return m_implementation->view_tool_properties_signal();
}

document_state::document_selection_change_signal_t& document_state::document_selection_change_signal()
{
	return m_implementation->document_selection_change_signal();
}

tool& document_state::active_tool()
{
	return m_implementation->active_tool();
}

void document_state::set_active_tool(tool& ActiveTool)
{
	m_implementation->set_active_tool(ActiveTool);
}

document_state::active_tool_changed_signal_t& document_state::active_tool_changed_signal()
{
	return m_implementation->active_tool_changed_signal();
}


tool* document_state::get_tool(const k3d::string_t& Name)
{
	if(!m_implementation->m_tools.count(Name))
	{
		tool* const new_tool = k3d::plugin::create<tool>(Name);
		return_val_if_fail(new_tool, new_tool);

		new_tool->initialize(*this);

		if(k3d::icommand_node* const command_node = dynamic_cast<k3d::icommand_node*>(new_tool))
			k3d::command_tree().add(*command_node, Name, dynamic_cast<k3d::icommand_node*>(&document()));

		m_implementation->m_tools.insert(std::make_pair(Name, new_tool));

		return new_tool;
	}

	return m_implementation->m_tools[Name];
}

tool& document_state::selection_tool()
{
	return *m_implementation->m_selection_tool;
}

tool& document_state::move_tool()
{
	return *m_implementation->m_move_tool;
}

tool& document_state::rotate_tool()
{
	return *m_implementation->m_rotate_tool;
}

tool& document_state::scale_tool()
{
	return *m_implementation->m_scale_tool;
}

tool& document_state::snap_tool()
{
	return *m_implementation->m_snap_tool;
}

document_state::selection_mode_property_t& document_state::selection_mode()
{
	return m_implementation->selection_mode();
}

void document_state::set_selection_mode(selection_mode_t Mode)
{
	m_implementation->set_selection_mode(Mode);
}

void document_state::select(const k3d::selection::record& Selection)
{
	m_implementation->select(k3d::selection::records(1, Selection));
}

void document_state::select(const k3d::selection::records& Selection)
{
	m_implementation->select(Selection);
}

const bool document_state::is_selected(k3d::inode* Node)
{
	return m_implementation->is_selected(Node);
}

const bool document_state::is_selected(const k3d::selection::record& Selection)
{
	return m_implementation->is_selected(Selection);
}

bool document_state::pick_backfacing()
{
	return m_implementation->m_selection_tool->pick_backfacing();
}

bool document_state::paint_backfacing()
{
	return m_implementation->m_selection_tool->paint_backfacing();
}

bool document_state::rubber_band_backfacing()
{
	return m_implementation->m_selection_tool->rubber_band_backfacing();
}

void document_state::select_all()
{
	m_implementation->select_all();
}

void document_state::deselect(const k3d::selection::record& Selection)
{
	m_implementation->deselect(k3d::selection::records(1, Selection));
}

void document_state::deselect(const k3d::selection::records& Selection)
{
	m_implementation->deselect(Selection);
}

void document_state::deselect_all()
{
	m_implementation->deselect_all();
}

void document_state::invert_selection()
{
	m_implementation->invert_selection();
}

const k3d::nodes_t document_state::selected_nodes()
{
	return m_implementation->selected_nodes();
}

void document_state::hide_selection()
{
	return m_implementation->hide_selection();
}

void document_state::show_selection()
{
	return m_implementation->show_selection();
}

void document_state::hide_unselected()
{
	return m_implementation->hide_unselected();
}

void document_state::show_all_nodes()
{
	return m_implementation->show_all_nodes();
}

document_state::set_cursor_signal_t& document_state::set_cursor_signal()
{
	return m_implementation->set_cursor_signal();
}

document_state::clear_cursor_signal_t& document_state::clear_cursor_signal()
{
	return m_implementation->clear_cursor_signal();
}

document_state::push_status_message_signal_t& document_state::push_status_message_signal()
{
	return m_implementation->push_status_message_signal();
}

document_state::pop_status_message_signal_t& document_state::pop_status_message_signal()
{
	return m_implementation->pop_status_message_signal();
}

k3d::inode* document_state::create_node(k3d::iplugin_factory* Factory)
{
	return m_implementation->create_node(Factory);
}

void document_state::popup_context_menu(const bool UserAction)
{
	m_implementation->popup_context_menu(UserAction);
}

} // namespace libk3dngui

