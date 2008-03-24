// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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

/** \file A STEP CAD file format reader, using OpenCascade
	\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include <deque>
#include <iostream>
#include <limits>

#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/imesh_storage.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/node.h>
#include <k3dsdk/path.h>
#include <k3dsdk/persistent.h>

#include <BRep_Tool.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dConvert.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <GeomConvert.hxx>
#include <GeomProjLib.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_PrimitiveTypes.hxx>
#include <STEPCAFControl_Reader.hxx>
#include <TDataStd_Name.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelSequence.hxx>
#include <TDocStd_Document.hxx>
#include <TopAbs.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>

#include "gprim_factory.h"

namespace module
{

namespace opencascade
{

namespace detail
{

/// Convert different OpenCascade components to a K-3D mesh
class occ_converter
{
public:
	occ_converter(k3d::mesh& Mesh) : m_factory(Mesh), m_mesh(Mesh)
	{
	}
	
	void process_shape(const TDF_Label& Shape)
	{
		Handle(TDataStd_Name) N;
		if (!Shape.FindAttribute(TDataStd_Name::GetID(),N))
		{
		  k3d::log() << debug << "Exploring shape with no name" << std::endl;
		}
		else
		{
			k3d::log() << debug << "Exploring shape " << N->Get() << std::endl;
		}
		
		TopoDS_Shape shape = XCAFDoc_ShapeTool::GetShape(Shape);
		if (!shape.IsNull())
		{
			switch(shape.ShapeType())
			{
			case TopAbs_COMPOUND:
				process_surface(shape);
				break;
			case TopAbs_COMPSOLID:
				process_surface(shape);
				break;
			case TopAbs_SOLID:
				process_surface(shape);
				break;
			case TopAbs_SHELL:
				process_surface(shape);
				break;
			case TopAbs_FACE:
				k3d::log() << debug << "Found a TopAbs_FACE" << std::endl;
				break;
			case TopAbs_WIRE:
				k3d::log() << debug << "Found a TopAbs_WIRE" << std::endl;
				break;
			case TopAbs_EDGE:
				k3d::log() << debug << "Found a TopAbs_EDGE" << std::endl;
				break;
			case TopAbs_VERTEX:
				k3d::log() << debug << "Found a TopAbs_VERTEX" << std::endl;
				break;
			case TopAbs_SHAPE:
				k3d::log() << debug << "Found a TopAbs_SHAPE" << std::endl;
				break;
			}
		}
		else
		{
			k3d::log() << debug << "Found node that is not a shape" << std::endl;
		}
		
		if (Shape.HasChild())
		{
			TDF_ChildIterator shapes(Shape);
			for (; shapes.More(); shapes.Next())
			{
				process_shape(shapes.Value());
			}
		}
	}
	
private:
	// process surface components
	void process_surface(TopoDS_Shape& shape)
	{
		TopExp_Explorer explorer;
		k3d::uint_t patch = 0;
		for (explorer.Init(shape,TopAbs_FACE); explorer.More(); explorer.Next())
		{
			const double maxsize = 1000; // Maximum size of the bbox in any direction
			Handle(Geom_Surface) surface = BRep_Tool::Surface(TopoDS::Face(explorer.Current())); // Get the surface representation 
			TopExp_Explorer wire_explorer;			
			double umin, umax, vmin, vmax;
			surface->Bounds(umin, umax, vmin, vmax);
			// Make sure we are within our predefined maximum size
			if (umin < -maxsize) umin = -maxsize;
			if (vmin < -maxsize) vmin = -maxsize;
			if (umax > maxsize) umax = maxsize;
			if (vmax > maxsize) vmax = maxsize;
			try
			{
				Handle(Geom_Surface) bounded_surface = new Geom_RectangularTrimmedSurface(surface, umin, umax, vmin, vmax);
				Handle(Geom_BSplineSurface) nurbs_surface = GeomConvert::SurfaceToBSplineSurface(bounded_surface);
				if (!on_bspline_surface(nurbs_surface))
					continue;
				// Visit trim curves in their connected order, and add them to the mesh
				k3d::uint_t loop = 0;
				for (wire_explorer.Init(explorer.Current(),TopAbs_WIRE); wire_explorer.More(); wire_explorer.Next()) // wires (trim curve loops)
				{
					TopAbs_Orientation wire_orientation = wire_explorer.Current().Orientation();
					if (wire_orientation != TopAbs_FORWARD && wire_orientation != TopAbs_REVERSED) // not a trim curve loop
						continue;
					BRepTools_WireExplorer trim_explorer;
					// Stores curves in the order they need to be processed, reversing curves if needed
					typedef std::deque<Handle(Geom2d_BSplineCurve)> curves_t;
					curves_t curves;
					bool connected = true;
//					if (!wire_explorer.Current().Closed())
//						continue;
					for (trim_explorer.Init(TopoDS::Wire(wire_explorer.Current()), TopoDS::Face(explorer.Current())); trim_explorer.More(); trim_explorer.Next())
					{
						TopAbs_Orientation edge_orientation = trim_explorer.Current().Orientation();
						if (edge_orientation != TopAbs_FORWARD && edge_orientation != TopAbs_REVERSED)
							k3d::log() << warning << "Invalid trim curve orientation" << std::endl;
						double first = 0.0;
						double last = 0.0;
						Handle(Geom2d_Curve) curve = BRep_Tool::CurveOnSurface(TopoDS::Edge(trim_explorer.Current()), TopoDS::Face(explorer.Current()), first, last);
						if (first < -maxsize) first = -maxsize;
						if (last > maxsize) last = maxsize;
						Handle(Geom2d_Curve) bounded_curve = new Geom2d_TrimmedCurve(curve, first, last);
						Handle(Geom2d_BSplineCurve) nurbs_trim = Geom2dConvert::CurveToBSplineCurve(bounded_curve);
						
						Handle(Geom_Curve) curve_3d = BRep_Tool::Curve(TopoDS::Edge(trim_explorer.Current()), first, last);
						Handle(Geom_Curve) bounded_curve_3d = new Geom_TrimmedCurve(curve_3d, first, last);
						Handle(Geom_BSplineCurve) nurbs_curve_3d = GeomConvert::CurveToBSplineCurve(bounded_curve_3d); 
						on_nurbs_curve(nurbs_curve_3d);
						
						if (edge_orientation != wire_orientation)
							nurbs_trim->Reverse();
						if (wire_orientation == TopAbs_FORWARD)
						{
							if (!curves.empty() && !equal(nurbs_trim->Pole(1), curves.back()->Pole(curves.back()->NbPoles())))
							{
								k3d::log() << warning << "Failed to connect trim curve" << std::endl;
								connected = false;
								break;
							}
							curves.push_back(nurbs_trim);
						}
						else
						{
							if (!curves.empty() && !equal(nurbs_trim->Pole(nurbs_trim->NbPoles()), curves.front()->Pole(1)))
							{
								k3d::log() << warning << "Failed to connect trim curve" << std::endl;
								connected = false;
								break;
							}
							curves.push_front(nurbs_trim);
						}
					}
					// Check if first and last points correspond
					if (!equal(curves.front()->Pole(1), curves.back()->Pole(curves.back()->NbPoles())))
					{
						k3d::log() << warning << "Trim curve is not closed. Skipping wire." << std::endl;
						continue;
					}
					for (curves_t::iterator curve = curves.begin(); curve != curves.end(); ++curve)
					{
						on_trim_curve(*curve, surface, nurbs_surface);
					}
					m_factory.close_trim_loop();
					++loop;
				}
			}
			catch (...)
			{
				k3d::log() << warning << "Error converting surface to NURBS." << std::endl;
				k3d::log() << debug << "Surface was bound to U1: " << umin << ", U2: " << umax << ", V1: " << vmin << ", V2: " << vmax << std::endl;
			}
			++patch;
		}
		k3d::log() << debug << "Found " << patch << " shapes" << std::endl;
	}
	
	bool on_bspline_surface(Handle(Geom_BSplineSurface) Patch)
	{
		const double min_knot_diff = 2e-5; // Minimal difference between two knots
		k3d::int32_t nup = Patch->NbUPoles();
		k3d::int32_t nvp = Patch->NbVPoles();
		k3d::mesh::indices_t control_points;
		k3d::mesh::weights_t weights;
		for (k3d::int32_t j = 1; j <= nvp; ++j)
		{
			for (k3d::int32_t i = 1; i <= nup; ++i)
			{
				gp_Pnt pole = Patch->Pole(i, j);
				m_factory.add_point(k3d::point3(pole.X(), pole.Y(), pole.Z()));
				control_points.push_back(m_mesh.points->size()-1);
				weights.push_back(Patch->Weight(i, j));
			}
		}
		
		k3d::uint_t u_order = Patch->UDegree()+1;
		k3d::uint_t v_order = Patch->VDegree()+1;
		
		k3d::mesh::knots_t uknots;
		k3d::int32_t nuk = Patch->NbUKnots();
		for (k3d::int32_t i = 1; i <= nuk; ++i)
		{
			for (k3d::int32_t j = 0; j < Patch->UMultiplicity(i); ++j)
				uknots.push_back(Patch->UKnot(i));
		}
		
		k3d::mesh::knots_t vknots;
		k3d::int32_t nvk = Patch->NbVKnots();
		for (k3d::int32_t i = 1; i <= nvk; ++i)
		{
			for (k3d::int32_t j = 0; j < Patch->VMultiplicity(i); ++j)
				vknots.push_back(Patch->VKnot(i));
		}
		
		return_val_if_fail(u_order == uknots.size() - nup, false);
		return_val_if_fail(v_order == vknots.size() - nvp, false);
		
		return m_factory.add_nurbs_patch(u_order, v_order, control_points, uknots, vknots, weights);
	}
	
	void on_nurbs_curve(Handle(Geom_BSplineCurve) Curve)
	{
		k3d::int32_t n_poles = Curve->NbPoles();
		k3d::mesh::indices_t points;
		k3d::mesh::weights_t weights;
		for (k3d::int32_t i = 1; i <= n_poles; ++i)
		{
			gp_Pnt pole = Curve->Pole(i);
			m_factory.add_point(k3d::point3(pole.X(), pole.Y(), pole.Z()));
			points.push_back(m_mesh.points->size()-1);
			weights.push_back(Curve->Weight(i));
		}
		k3d::mesh::knots_t knots;
		k3d::int32_t n_knots = Curve->NbKnots();
		for (k3d::int32_t i = 1; i <= n_knots; ++i)
		{
			for (k3d::int32_t j = 0; j < Curve->Multiplicity(i); ++j)
				knots.push_back(Curve->Knot(i));
		}
		m_factory.add_nurbs_curve(Curve->Degree() + 1, points, knots, weights);
	}
	
	void on_trim_curve(Handle(Geom2d_BSplineCurve) Curve, Handle(Geom_Surface) Surface, Handle(Geom_Surface) NurbsSurface)
	{
		k3d::int32_t n_poles = Curve->NbPoles();
		k3d::mesh::points_2d_t points;
		k3d::mesh::weights_t weights;
		
		double u1, u2, v1, v2;
		NurbsSurface->Bounds(u1, u2, v1, v2);
		
		k3d::mesh::indices_t points_3d;
		
		for (k3d::int32_t i = 1; i <= n_poles; ++i)
		{
			gp_Pnt2d pole_uv = Curve->Pole(i); // UV coordinates of the control point on the original surface
			gp_Pnt pole_3d = Surface->Value(pole_uv.X(), pole_uv.Y()); // World position for the control point on the original surface
			GeomAPI_ProjectPointOnSurf projection(pole_3d, NurbsSurface); // Project onto the NURBS conversion, which may have a different parameter space
			double dmin=10000;
			double u_min = 0;
			double v_min = 0;
			if (projection.NbPoints() == 0)
			{
				k3d::log() << warning << "Failed to project NURBS trim curve control point" << std::endl;
			}
			
			projection.LowerDistanceParameters(u_min, v_min);
			u_min = std::abs(pole_uv.X() - u_min) > 0.5 * std::abs(u2-u1) ? pole_uv.X() : u_min;
			v_min = std::abs(pole_uv.Y() - v_min) > 0.5 * std::abs(v2-v1) ? pole_uv.Y() : v_min;
			
			points.push_back(k3d::point2(u_min, v_min)); // Control point in UV space of the NURBS surface
			
			weights.push_back(Curve->Weight(i));
			
			// Add trim curve as a 3D NURBS curve
//			gp_Pnt pole_3d_nurbs = NurbsSurface->Value(u_min, v_min);
//			if (pole_3d.Distance(pole_3d_nurbs) > 1e-10)
//				k3d::log() << debug << "  Moving control point over " << pole_3d.Distance(pole_3d_nurbs) <<  " from " << pole_3d.X() << ", " << pole_3d.Y() << ", " << pole_3d.Z() << " to " << pole_3d_nurbs.X() << ", " << pole_3d_nurbs.Y() << ", " << pole_3d_nurbs.Z() << std::endl;
//			m_factory.add_point(k3d::point3(pole_3d_nurbs.X(), pole_3d_nurbs.Y(), pole_3d_nurbs.Z()));
//			points_3d.push_back(m_mesh.points->size()-1);
		}
		
		k3d::uint_t order = Curve->Degree() + 1;
		
		k3d::mesh::knots_t knots;
		k3d::int32_t n_knots = Curve->NbKnots();
		for (k3d::int32_t i = 1; i <= n_knots; ++i)
		{
			for (k3d::int32_t j = 0; j < Curve->Multiplicity(i); ++j)
				knots.push_back(Curve->Knot(i));
		}
		
		return_if_fail(order == knots.size() - n_poles);
		
		m_factory.add_trim_curve(order, points, knots, weights);
		//m_factory.add_nurbs_curve(Curve->Degree() + 1, points_3d, knots, weights);
	}
	
	/// True if both points are more or less equal
	bool equal(const gp_Pnt2d& A, const gp_Pnt2d& B)
	{
		const double tol = 1e-4;
		return std::abs(A.X()-B.X()) < tol && std::abs(A.Y()-B.Y()) < tol;
	}
	
	gprim_factory m_factory;
	k3d::mesh& m_mesh;
};

} // namespace detail

class step_mesh_reader_implementation :
	public k3d::mesh_source<k3d::persistent<k3d::node> >,
	public k3d::imesh_storage
{
	typedef k3d::mesh_source<k3d::persistent<k3d::node> > base;

public:
	step_mesh_reader_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_file(init_owner(*this) + init_name("file") + init_label(_("File")) + init_description(_("Input file")) + init_value(k3d::filesystem::path()) + init_path_mode(k3d::ipath_property::READ) + init_path_type("dxf_files"))
	{
		m_file.changed_signal().connect(make_topology_changed_slot());
	}
	
	void reset_mesh(k3d::mesh* const Mesh)
	{
		m_output_mesh.reset(Mesh);
	}
	
	void on_create_mesh_topology(k3d::mesh& Mesh)
	{
		const k3d::filesystem::path path = m_file.pipeline_value();
		if(path.empty())
			return;

		STEPCAFControl_Reader step_reader; // create a STEPCAF reader, which supports reading attributes like shape names, colors, ...
		step_reader.SetNameMode(true); // Make sure we read names
		// WHY is it so hard for a C++ lib to support std::string?
		IFSelect_ReturnStatus status = step_reader.ReadFile(const_cast<char*>(path.native_filesystem_string().c_str())); // Read the file
		if(status != IFSelect_RetDone)
		{
			k3d::log() << error << k3d_file_reference << ": error opening [" << path.native_console_string() << "]" << std::endl;
			return;
		}
		
		Handle(TDocStd_Document) xde_doc = new TDocStd_Document("STEP"); // initialise an empty document
		return_if_fail(step_reader.Transfer(xde_doc)); // attempt to transfer the STEP file contents to the document
		return_if_fail(XCAFDoc_DocumentTool::IsXCAFDocument(xde_doc)); // sanity check
		TDF_Label shaperoot = XCAFDoc_DocumentTool::ShapesLabel(xde_doc->Main()); // get the root node for the geometric shapes
		detail::occ_converter converter(Mesh);
		TDF_ChildIterator shapes(shaperoot);
		for (; shapes.More(); shapes.Next())
		{
			converter.process_shape(shapes.Value());
		}
	}
	
	void on_update_mesh_geometry(k3d::mesh& Mesh)
	{
	}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<step_mesh_reader_implementation,
	              k3d::interface_list<k3d::imesh_source,
	              k3d::interface_list<k3d::imesh_storage> > > factory(
			k3d::uuid(0x8e9c8376, 0x9f44e539, 0x90f9f7b7, 0x02ab8a1f),
			"STEPMeshReader",
			_("Mesh reader that loads external STEP (.stp, .step) files into the document by reference"),
			"MeshReader");
	
		return factory;
	}

private:
	k3d_data(k3d::filesystem::path, immutable_name, change_signal, with_undo, local_storage, no_constraint, path_property, path_serialization) m_file;
		
};

k3d::iplugin_factory& step_mesh_reader_factory()
{
	return step_mesh_reader_implementation::get_factory();
}

} // opencascade

} // module
