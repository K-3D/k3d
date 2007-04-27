// K-3D SDS creases
// Copyright (c) 2004-2006, Bart Janssens
//
// Contact: bart.janssens@lid.kviv.be
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
		\author Bart Janssens <bart.janssens@lid.kviv.be>
*/

#include "sds_crease.h"

#include <k3dsdk/basic_math.h>
#include <k3dsdk/types_ri.h>
#include <k3dsdk/result.h>

#include <boost/multi_array.hpp>

#include <cmath>
#include <map>

#define DEBUG_MESSAGE(Message)
//#define DEBUG_MESSAGE(Message) Message

namespace k3d
{

namespace sds
{

/// class link
class link
{
public:
	link(k3d::legacy::split_edge* Edge, bool dummy_edge = false) : edge(Edge), counter_clockwise(0), companion_clockwise(0), companion(0), m_Complete(false), m_Dummy_edge(dummy_edge)
	{
	}

	~link()
	{
		if (!is_complete() && m_Dummy_edge && edge)
		{
			delete edge;
		}
	}

/// the split_edge associated with this link
	k3d::legacy::split_edge* edge;
/// counter-clockwise link to this one
	link* counter_clockwise;
/// clockwise link to the companion of this one
	link* companion_clockwise;
/// link with the companion edge
	link* companion;
/// true if the link is completed
	bool is_complete()
	{
		return m_Complete;
	}
/// complete the link, i.e. set the actual companion and face_clockwise on the k3d::legacy::split_edge that is represented by the link
	void complete(bool recurse = true)
	{
		if (is_complete())
		{
			return;
		}
		m_Complete = true;
		return_if_fail(edge);
		return_if_fail(edge->vertex);
		return_if_fail(counter_clockwise);
		return_if_fail(companion_clockwise);
		return_if_fail(companion);
		if (recurse)
		{
			companion->complete(false);
		}
		return_if_fail(companion->edge);
		return_if_fail(companion_clockwise->edge);
		companion->edge->face_clockwise = companion_clockwise->edge;
		return_if_fail(counter_clockwise->edge);
		counter_clockwise->edge->face_clockwise = edge;
		if (!(edge->companion))
			k3d::legacy::join_edges(*edge, *(companion->edge));
	}
private:
	bool m_Complete;
	bool m_Dummy_edge;
};

class point
{
friend void splitter::make_creases();
public:
	point() : vertex(0), m_Reorder(false) {}
	point(k3d::legacy::point* Point) : vertex(Point), m_Reorder(false), m_Updated(false) {}
	virtual ~point()
	{
		for (unsigned long i = 0; i < links.size(); ++i)
		{
			delete links[i];
		}
	}
/// the point represented
	k3d::legacy::point* vertex;
/// update the relations between the links. Works correctly only if links are ordered clockwise around the vertex. Use reorder_on if this is not the case.
	virtual void update()
	{
		m_Updated = true;
		if (m_Reorder)
			reorder();
		for (unsigned long i = 0; i < links.size(); ++i)
		{
			links[i]->counter_clockwise = links[(i+1) % links.size()]->companion;
			links[i]->companion_clockwise = links[(links.size()+i-1) % links.size()];
		}
	}
	virtual link* join(point* other_point, bool recurse = true)
	{
		k3d::legacy::split_edge* new_edge = new k3d::legacy::split_edge(vertex);
		new_edge->set_selection_weight(1.0);
		link* new_link = new link(new_edge);
		links.push_back(new_link);
		if (recurse)
		{
			new_link->companion = other_point->join(this, false);
			new_link->companion->companion = new_link;
		}
		return new_link;
	}

/// complete all links
	virtual void complete()
	{
		return_if_fail(m_Updated);
		for (unsigned long i = 0; i < links.size(); ++i)
		{
			links[i]->complete();
		}
	}

/// Turn on automatic link reordering, arranging links clockwise around the face normal.
	void reorder_on(k3d::vector3& normal)
	{
		m_Reorder = true;
		m_Normal = normal / normal.length();
	}

private:
/// reorder the links around the vertex so they are arranged clockwise around the normal vector of the face.
	void reorder()
	{
		if (links.size() < 1)
			return;
		std::map<double, link*> angle_links;

		k3d::point3 v0 = links[0]->companion->edge->vertex->position - vertex->position;

		for (unsigned long i = 0; i < links.size(); ++i)
		{
			k3d::point3 v = links[i]->companion->edge->vertex->position - vertex->position;
			angle_links[angle(v0, v)] = links[i];
		}

		unsigned long i = 0;
		for (std::map<double, link*>::iterator it = angle_links.begin(); it != angle_links.end(); ++it)
		{
			links[i] = it->second;
			++i;
		}
	}
/// Calculate the angle between 2 vectors
	double angle(k3d::point3& v1, k3d::point3& v2)
	{
// cosine of the angle:
		double cos_th = (v1 * v2) / (k3d::to_vector(v1).length() * k3d::to_vector(v2).length());
//sometimes this value is greater than 1
		if (cos_th > 1.0)
			cos_th = 1.0;
		if (cos_th < -1.0)
			cos_th = -1.0;
		k3d::vector3 normal = k3d::normalize(k3d::to_vector(v1) ^ k3d::to_vector(v2));
		double tol = 0.00001;
// the angle
		double th = (normal + m_Normal).length() > tol ? acos(cos_th) : k3d::pi_times_2() - acos(cos_th);
//k3d::log() << debug << "cos_th: " << cos_th << ". angle is " << th * 180 / k3d::pi() << " degrees" << std::endl;
		return th;
	}
	k3d::vector3 m_Normal;
	bool m_Reorder;
	bool m_Updated;
protected:
/// a vector of all links from this point
	std::vector<link*> links;
};

// point at a t-junction. links[0] is the incomplete link (the leg of the T)
class t_point : public point
{
public:
	t_point(k3d::legacy::point* Point, k3d::legacy::split_edge* To, k3d::legacy::split_edge* From, bool dummy_edge = false)
	{
		this->vertex = Point;
		k3d::legacy::split_edge* new_edge = new k3d::legacy::split_edge(vertex);
		new_edge->set_selection_weight(1.0);
		link* leg = new link(new_edge, dummy_edge);
		leg->counter_clockwise = new link(To);
		leg->companion_clockwise = new link(From);
		links.push_back(leg);
		m_linked = false;
	}

	~t_point()
	{
		delete links[0]->counter_clockwise;
		delete links[0]->companion_clockwise;
	}

	void update()
	{
		return_if_fail(links.size() == 1);
		if (!m_linked)
			return;
		link* leg = links[0];
		return_if_fail(leg->edge);
		return_if_fail(leg->counter_clockwise);
		return_if_fail(leg->companion);
		return_if_fail(leg->companion_clockwise);
	}
	link* join(point* other_point, bool recurse = true)
	{
		m_linked = true;
		link* leg = links[0];
		if (recurse)
		{
			leg->companion = other_point->join(this, false);
			leg->companion->companion = leg;
		}
		return leg;
	}
	void complete()
	{
		if (!m_linked)
			return;
		links[0]->complete();
	}
private:
	bool m_linked;
};

/// class splitter
splitter::splitter(k3d::legacy::polyhedron& Polyhedron, k3d::legacy::mesh::points_t& Points, bool Use_selection) : m_Polyhedron(Polyhedron), m_Points(Points)
{
	for (unsigned long i = 0; i < Polyhedron.faces.size(); ++i)
	{
		m_all_faces[Polyhedron.faces[i]->first_edge] = Polyhedron.faces[i];
		k3d::legacy::split_edge* last_edge = Polyhedron.faces[i]->first_edge;
		k3d::legacy::split_edge* this_edge = last_edge->face_clockwise;

		bool has_selected_edge = false;
		has_selected_edge = get_sharpness(*last_edge) > 0.0 ? true : has_selected_edge;

		if (Use_selection && (last_edge->vertex->selection_weight || last_edge->selection_weight))
			has_selected_edge = true;

		k3d::legacy::point* centroid = new k3d::legacy::point(last_edge->vertex->position);

		unsigned long corners = 1;

		while(this_edge != Polyhedron.faces[i]->first_edge)
		{
			++corners;
			centroid->position += this_edge->vertex->position;
			has_selected_edge = get_sharpness(*this_edge) > 0.0 ? true : has_selected_edge;
			if (Use_selection && (this_edge->vertex->selection_weight || this_edge->selection_weight))
				has_selected_edge = true;
			last_edge = this_edge;
			this_edge = this_edge->face_clockwise;
		}
		if (has_selected_edge)
		{
			point_edge_info info;
			info.second[1] = last_edge;
			centroid->position /= corners;
			last_edge = Polyhedron.faces[i]->first_edge;
			this_edge = last_edge->face_clockwise;
			info.second[0] = this_edge;
			info.second[2] = last_edge->companion;
			info.first[1] = add_point(centroid);
			m_edges[last_edge] = info;

			while(this_edge != Polyhedron.faces[i]->first_edge)
			{
				info.second[0] = this_edge->face_clockwise;
				info.second[1] = last_edge;
				info.second[2] = this_edge->companion;
				info.first[0] = add_t_point(this_edge->vertex, last_edge, this_edge, true);
				info.first[1] = add_point(centroid);
				m_edges[this_edge] = info;
				last_edge = this_edge;
				this_edge = this_edge->face_clockwise;
			}
			m_edges[Polyhedron.faces[i]->first_edge].first[0] = add_t_point(Polyhedron.faces[i]->first_edge->vertex, last_edge, Polyhedron.faces[i]->first_edge, true);
			m_face_vector.push_back(Polyhedron.faces[i]);
			m_centroids.push_back(centroid);
		}
		else
		{
			delete centroid;
		}
	}
}

splitter::~splitter()
{
	for (unsigned long i = 0; i < m_centroids.size(); ++i)
	{
		delete m_centroids[i];
	}

	for (unsigned long i = 0; i < m_added_points.size(); ++i)
	{
		delete m_added_points[i];
	}
}

point* splitter::split_edge(k3d::legacy::split_edge& Edge, const double Factor, point* Start, point* End)
{
// first point of the edge:
	k3d::legacy::point* this_point = (!Start || (Start->vertex == 0)) ? Edge.vertex : Start->vertex;
// second point of the edge:
	k3d::legacy::point* other_point = (!End || (End->vertex == 0)) ? Edge.face_clockwise->vertex : End->vertex;
// new point position:
	k3d::point3 new_pos;
	new_pos = k3d::mix<k3d::point3>(this_point->position, other_point->position, Factor);
// new point:
	k3d::legacy::point* new_point = new k3d::legacy::point(new_pos);
// new edge:
	k3d::legacy::split_edge* new_edge = new k3d::legacy::split_edge(new_point, Edge.face_clockwise);
	new_edge->set_selection_weight(1.0);
	Edge.face_clockwise = new_edge;
// companion edge
	if(Edge.companion) {
		k3d::legacy::split_edge* new_comp_edge = new k3d::legacy::split_edge(new_point, Edge.companion->face_clockwise);
		new_comp_edge->set_selection_weight(1.0);
		Edge.companion->face_clockwise = new_comp_edge;
		k3d::legacy::join_edges(*Edge.companion, *new_edge);
		k3d::legacy::join_edges(Edge, *new_comp_edge);
	}
	return add_t_point(new_point, &Edge, Edge.face_clockwise);
}

void splitter::split_edge_add_point(k3d::legacy::split_edge& Edge, const double factor, point* Start, point* End)
{
	point* p = split_edge(Edge, factor, Start, End);
	m_Points.push_back(p->vertex);
}

void splitter::split_near(k3d::legacy::split_edge& edge, const double factor)
{
	point* far_v = far_point(*companion(edge));
	if (far_v)
	{
		k3d::point3 new_pos = k3d::mix(start(edge)->vertex->position, end(edge)->vertex->position, factor);
		far_v->vertex->position = k3d::mix(new_pos, far_v->vertex->position, 0.5);
		set_near(edge, *(add_t_point(far_v->vertex, 0, 0)));
	}
	else
	{
		point* new_point = split_edge(edge, factor, start(edge), end(edge));
		m_Points.push_back(new_point->vertex);
		set_near(edge, *new_point);
	}
}

void splitter::split_far(k3d::legacy::split_edge& edge, const double factor)
{
	point* near_v = near_point(*companion(edge));
	if (near_v)
	{
		k3d::point3 new_pos = k3d::mix(start(edge)->vertex->position, end(edge)->vertex->position, factor);
		near_v->vertex->position = k3d::mix(new_pos, near_v->vertex->position, 0.5);
		set_far(edge, *(add_t_point(near_v->vertex, 0, 0)));
	}
	else if (near_point(edge))
	{
		point* new_point = split_edge(*(edge.face_clockwise), factor, start(edge), end(edge));
		m_Points.push_back(new_point->vertex);
		set_far(edge, *new_point);
	}
	else
	{
		point* new_point = split_edge(edge, factor, start(edge), end(edge));
		m_Points.push_back(new_point->vertex);
		set_far(edge, *new_point);
	}
}

void splitter::make_creases()
{
// add the new points
	for (k3d::legacy::polyhedron::faces_t::iterator face = m_face_vector.begin(); face != m_face_vector.end(); ++face)
	{
		k3d::legacy::split_edge* first_edge = (*face)->first_edge;
		k3d::legacy::split_edge* this_edge = first_edge;
		bool start = true;
		while (this_edge != first_edge || start)
		{
			edges_t::iterator edge = find_info(*this_edge);
			double sharpness = get_sharpness(*(edge->first));
			double cl_sharpness = get_sharpness(*(edge->second.second[0]));
			if (sharpness > 0.0)
			{
				if (cl_sharpness > 0.0)
				{
				// Use the geometric mean of both factors to calculate the factor for lerping between the centroid and the corner
					double f = 2*sqrt(sharpness_to_factor(sharpness) * sharpness_to_factor(cl_sharpness));
					k3d::point3 face_pos(k3d::mix<k3d::point3>(edge->second.second[0]->vertex->position, edge->second.first[1]->vertex->position, f));
					point* face_vertex = add_point(new k3d::legacy::point(face_pos));
					m_Points.push_back(face_vertex->vertex);
					set_face_vertex(*(edge->second.second[0]), *face_vertex);
					if (!companion(*(edge->second.second[0])) || get_sharpness(*clockwise(*companion(*(edge->second.second[0])))) > 0.0)
					{
					// the clockwise edge
						split_near(*(edge->second.second[0]),sharpness_to_factor(sharpness));
					// the edge itself
						split_far(*(edge->first), 1-sharpness_to_factor(cl_sharpness));
					}
					else if (get_sharpness(*counter_clockwise(*companion(*(edge->first)))))
					{
					// the clockwise edge
						split_near(*(edge->second.second[0]),sharpness_to_factor(sharpness));
					// the edge itself
						split_far(*(edge->first), 1-sharpness_to_factor(cl_sharpness));
					}
				}
				else
				{
					split_near(*(edge->second.second[0]),sharpness_to_factor(sharpness));
				}
			}
			else if (cl_sharpness > 0.0)
			{
				split_far(*(edge->first), 1-sharpness_to_factor(cl_sharpness));
			}
			start = false;
			this_edge = clockwise(*this_edge);
		}
	}

	for (k3d::legacy::polyhedron::faces_t::iterator face = m_face_vector.begin(); face != m_face_vector.end(); ++face)
	{
		k3d::imaterial* current_material =  (*face)->material;
		std::vector<link*> new_links;
		k3d::legacy::split_edge* first_edge = (*face)->first_edge;
		k3d::legacy::split_edge* this_edge = first_edge;
		bool start = true;
	// correct clockwise and counter-clockwise edges
		while (this_edge != first_edge || start)
		{
			point* end_v = end(*this_edge);
			point* near_v = near_point(*this_edge);
			point* far_v = far_point(*this_edge);
			point* companion_far_v = far_point(*companion(*this_edge));
			point* companion_near_v = near_point(*companion(*this_edge));
			if (near_v || companion_far_v)
			{
				if (near_v)
				{
					near_v->links[0]->counter_clockwise->edge = this_edge;
					near_v->links[0]->companion_clockwise->edge = this_edge->face_clockwise;
				}
				if (far_v || companion_near_v)
				{
					if (far_v)
					{
						far_v->links[0]->counter_clockwise->edge = this_edge->face_clockwise;
						far_v->links[0]->companion_clockwise->edge = this_edge->face_clockwise->face_clockwise;
					}
					end_v->links[0]->counter_clockwise->edge = this_edge->face_clockwise->face_clockwise;
				}
				else
				{
					end_v->links[0]->counter_clockwise->edge = this_edge->face_clockwise;
				}
			}
			else if (far_v || companion_near_v)
			{
				if (far_v)
				{
					far_v->links[0]->counter_clockwise->edge = this_edge;
					far_v->links[0]->companion_clockwise->edge = this_edge->face_clockwise;
				}
				end_v->links[0]->counter_clockwise->edge = this_edge->face_clockwise;
			}
			start = false;
			this_edge = clockwise(*this_edge);
		}

		this_edge = first_edge;
		start = true;
	// connect the new points
		while (this_edge != first_edge || start)
		{
			point* face_v = face_vertex(*this_edge);
			point* near_v = near_point(*this_edge);
			point* far_v = far_point(*this_edge);
			point* start_v = this->start(*this_edge);
			point* next_face_v = face_vertex(*clockwise(*this_edge));
			point* next_near_v = 0;
			if (face_v && !next_face_v)
				next_near_v = near_point(*clockwise(*this_edge));
			if (face_v)
			{
				if (near_v)
				{
					new_links.push_back(near_v->join(face_v));
				}
				if (far_v)
				{
					if (next_face_v)
					{
						new_links.push_back(next_face_v->join(face_v));
						new_links.push_back(far_v->join(next_face_v));
					}
				}
			}
			if (face_v && !near_v && !far_point(*counter_clockwise(*this_edge)))
			{
				new_links.push_back(start_v->join(face_v));
				k3d::vector3 n = k3d::to_vector(k3d::legacy::normal(this_edge));
				face_v->reorder_on(n);
			}
			if (face_v && next_face_v && !far_v)
			{
				new_links.push_back(face_v->join(next_face_v));
			}
			if (!face_v && far_v && next_face_v)
			{
				new_links.push_back(far_v->join(next_face_v));
				k3d::vector3 n = k3d::to_vector(k3d::legacy::normal(this_edge));
				next_face_v->reorder_on(n);
			}
			if (!next_face_v && far_v)
			{
				point* next_v = face_vertex(*clockwise(*clockwise(*this_edge)));
				if (next_v)
				{
					new_links.push_back(next_v->join(far_v));
				}
			}
			point* last_far_v = far_point(*counter_clockwise(*this_edge));
			if (!next_face_v && !face_v && last_far_v)
			{
				new_links.push_back(last_far_v->join(near_point(*clockwise(*this_edge))));
			}

			if (next_near_v)
			{
				new_links.push_back(next_near_v->join(face_v));
			}
			start = false;
			this_edge = clockwise(*this_edge);
		} // end while (face loop)

		this_edge = first_edge;
		start = true;
	// update all links
		while (this_edge != first_edge || start)
		{
			point* face_v = face_vertex(*this_edge);
			point* near_v = near_point(*this_edge);
			point* far_v = far_point(*this_edge);
			if (face_v)
			{
				face_v->update();
			}
			if (near_v)
			{
				near_v->update();
			}
			if (far_v)
			{
				far_v->update();
			}
			start = false;
			this_edge = clockwise(*this_edge);
		} // end while (face loop)

		this_edge = first_edge;
		start = true;
	// complete the k3d::legacy::split_edges in the links
		while (this_edge != first_edge || start)
		{
			point* face_v = face_vertex(*this_edge);
			point* near_v = near_point(*this_edge);
			point* far_v = far_point(*this_edge);
			if (face_v)
			{
				face_v->complete();
			}
			if (near_v)
			{
				near_v->complete();
			}
			if (far_v)
			{
				far_v->complete();
			}
			start = false;
			this_edge = clockwise(*this_edge);
		} // end while (face loop)
	// add new edges and faces to the mesh and apply the appropriate material
		for (unsigned long i = 0; i < new_links.size(); ++i)
		{
			k3d::legacy::split_edge* edge1 = new_links[i]->edge;
			k3d::legacy::split_edge* edge2 = new_links[i]->companion->edge;
			add_face(*edge1, current_material);
			add_face(*edge2, current_material);
		}
	} // end for (all faces)
}

void splitter::link_points(point* A, point* B, k3d::imaterial* const Material)
{
	return_if_fail(A || B || A->vertex || B->vertex);
	link* l = A->join(B);
	A->update();
	B->update();
	A->complete();
	B->complete();
	add_face(*l->edge, Material);
	add_face(*l->companion->edge, Material);
}

void splitter::link_points()
{
	for (k3d::legacy::polyhedron::faces_t::iterator face = m_face_vector.begin(); face != m_face_vector.end(); ++face)
	{
		k3d::legacy::split_edge* first_edge = (*face)->first_edge;
		k3d::legacy::split_edge* this_edge = first_edge;
		bool start = true;
		std::vector<point*> points;
		while (this_edge != first_edge || start)
		{
			if (this_edge->vertex->selection_weight && (!this_edge->face_clockwise->vertex->selection_weight || this_edge->face_clockwise->face_clockwise->vertex->selection_weight))
				points.push_back(this->start(*this_edge));
			start = false;
			this_edge = clockwise(*this_edge);
		}
		if (points.size() == 2)
			link_points(points[0], points[1], (*face)->material);
	}
}

void splitter::split_face_parallel(k3d::legacy::split_edge& Edge, double Factor)
{
	return_if_fail(&Edge);
	return_if_fail(Edge.face_clockwise);
	k3d::legacy::face* the_face = find_face(Edge);
	k3d::imaterial* the_material = 0;
	if (the_face)
		the_material = the_face->material;
// get the counter-clockwise edge
	k3d::legacy::split_edge* ccl;
	for(ccl = &Edge; ccl && ccl->face_clockwise; ccl = ccl->face_clockwise)
	{
		if (ccl->face_clockwise == &Edge)
			break;
	}
	point* start = split_edge(*ccl, 1-Factor);
	m_Points.push_back(start->vertex);
	point* end = split_edge(*(Edge.face_clockwise), Factor);
	m_Points.push_back(end->vertex);
	link_points(start, end, the_material);
}

splitter::edges_t::iterator& splitter::find_info(k3d::legacy::split_edge& Edge)
{
	if (&Edge == m_cached_edge.first)
		return m_cached_edge.second;
	m_cached_edge.first = &Edge;
	m_cached_edge.second = m_edges.find(&Edge);
	return m_cached_edge.second;
}

k3d::legacy::split_edge* splitter::get_edge(k3d::legacy::split_edge& Edge, int Index)
{
	edges_t::iterator edge = find_info(Edge);
	if (edge != m_edges.end())
	{
		return edge->second.second[Index];
	}
	return 0;
}

k3d::legacy::split_edge* splitter::clockwise(k3d::legacy::split_edge& Edge)
{
	return get_edge(Edge, 0);
}

k3d::legacy::split_edge* splitter::counter_clockwise(k3d::legacy::split_edge& Edge)
{
	return get_edge(Edge, 1);
}

k3d::legacy::split_edge* splitter::companion(k3d::legacy::split_edge& Edge)
{
	return get_edge(Edge, 2);
}

point* splitter::get_point(k3d::legacy::split_edge& Edge, int Index)
{
	edges_t::iterator edge = find_info(Edge);
	if (edge != m_edges.end())
	{
		return edge->second.first[Index];
	}
	return 0;
}

point* splitter::start(k3d::legacy::split_edge& Edge)
{
	return get_point(Edge, 0);
}

point* splitter::end(k3d::legacy::split_edge& Edge)
{
	return start(*clockwise(Edge));
}

k3d::legacy::point* splitter::centroid(k3d::legacy::split_edge& Edge)
{
	return get_point(Edge, 1)->vertex;
}

point* splitter::near_point(k3d::legacy::split_edge& Edge)
{
	return get_point(Edge, 2);
}

point* splitter::far_point(k3d::legacy::split_edge& Edge)
{
	return get_point(Edge, 3);
}

point* splitter::face_vertex(k3d::legacy::split_edge& Edge)
{
	return get_point(Edge, 4);
}

void splitter::set_point(k3d::legacy::split_edge& Edge, point& Point, int Index)
{
	edges_t::iterator edge = find_info(Edge);
	if (edge != m_edges.end())
	{
		edge->second.first[Index] = &Point;
	}
}

void splitter::set_near(k3d::legacy::split_edge& Edge, point& Point)
{
	set_point(Edge, Point, 2);
}

void splitter::set_far(k3d::legacy::split_edge& Edge, point& Point)
{
	set_point(Edge, Point, 3);
}

void splitter::set_face_vertex(k3d::legacy::split_edge& Edge, point& Point)
{
	set_point(Edge, Point, 4);
}

k3d::legacy::face* splitter::find_face(k3d::legacy::split_edge& Edge)
{
	k3d::legacy::split_edge* last_edge = &Edge;
	k3d::legacy::split_edge* this_edge = Edge.face_clockwise;

	while(this_edge != &Edge)
	{
		faces_t::iterator face = m_all_faces.find(last_edge);
		if(face != m_all_faces.end())
		{
			return face->second;
		}

		last_edge = this_edge;
		this_edge = this_edge->face_clockwise;
	}
	faces_t::iterator face = m_all_faces.find(last_edge);
	if(face != m_all_faces.end())
		return face->second;
	return 0;
}

bool splitter::is_face(k3d::legacy::split_edge& Edge)
{
	if (find_face(Edge))
		return true;
	return false;
}

void splitter::add_face(k3d::legacy::split_edge& Edge, k3d::imaterial* const Material)
{
	if (!(is_face(Edge)))
	{
		k3d::legacy::face* new_face = new k3d::legacy::face(&Edge, Material);
		m_Polyhedron.faces.push_back(new_face);
		m_all_faces[&Edge] = new_face;
	}
}

point* splitter::add_point(k3d::legacy::point* Point)
{
	point* p = new point(Point);
	m_added_points.push_back(p);
	return p;
}

point* splitter::add_t_point(k3d::legacy::point* Point, k3d::legacy::split_edge* To, k3d::legacy::split_edge* From, bool dummy_edge)
{
	point* p = new t_point(Point, To, From, dummy_edge);
	m_added_points.push_back(p);
	return p;
}

double get_sharpness(const k3d::legacy::split_edge& Edge)
{
	for(k3d::legacy::parameters_t::const_iterator tag = Edge.tags.begin(); tag != Edge.tags.end(); ++tag)
	{
		if(tag->first == "crease" && (tag->second.type() == typeid(k3d::ri::real)))
		{
			return boost::any_cast<double>(tag->second);
		}
	}
	return 0.0;
}

void crease(k3d::legacy::mesh& Mesh, k3d::legacy::polyhedron& Polyhedron)
{
	return_if_fail(k3d::legacy::is_valid(Polyhedron));
	splitter Splitter(Polyhedron, Mesh.points);
	Splitter.make_creases();

// Remove crease info to avoid calculating creases twice
	for(k3d::legacy::polyhedron::faces_t::const_iterator face = Polyhedron.faces.begin(); face != Polyhedron.faces.end(); ++face)
	{
		k3d::legacy::split_edge* edge = (*face)->first_edge;
		do
		{
			edge->tags.erase("crease");

			edge = edge->face_clockwise;
		}
		while(edge != (*face)->first_edge);
	}

	return_if_fail(k3d::legacy::is_valid(Polyhedron));
}

void split_faces_parallel(const k3d::legacy::mesh& Input, k3d::legacy::mesh& Output, const double Factor)
{
	for(k3d::legacy::mesh::polyhedra_t::iterator it = Output.polyhedra.begin(); it != Output.polyhedra.end(); ++it)
	{
		k3d::legacy::polyhedron& Polyhedron = **it;
		return_if_fail(k3d::legacy::is_valid(Polyhedron));
		splitter Splitter(Polyhedron, Output.points, true);
		std::vector<k3d::legacy::split_edge*> selected_edges;
		for(k3d::legacy::polyhedron::faces_t::const_iterator face = Polyhedron.faces.begin(); face != Polyhedron.faces.end(); ++face)
		{
			k3d::legacy::split_edge* edge = (*face)->first_edge;
			do
			{
				if(edge->selection_weight)
					selected_edges.push_back(edge);

				edge = edge->face_clockwise;
			}
			while(edge != (*face)->first_edge);
		}

		for (std::vector<k3d::legacy::split_edge*>::iterator edge = selected_edges.begin(); edge != selected_edges.end(); ++edge)
			Splitter.split_face_parallel(*(*edge), Factor);

		return_if_fail(k3d::legacy::is_valid(Polyhedron));
	}
}

} // namespace sds

} // namespace k3d


