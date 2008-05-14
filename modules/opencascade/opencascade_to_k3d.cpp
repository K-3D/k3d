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

#include <deque>
#include <iostream>
#include <limits>
#include <sstream> // to convert OCC strings
#include <stack>

#include <k3dsdk/basic_math.h>
#include <k3dsdk/gprim_factory.h>
#include <k3dsdk/path.h>

#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <GeomAbs_Shape.hxx>
#include <GeomConvert.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <gp_Pnt2d.hxx>
#include <IGESCAFControl_Reader.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <ShapeConstruct_CompBezierCurves2dToBSplineCurve2d.hxx>
#include <ShapeCustom.hxx>
#include <ShapeCustom_RestrictionParameters.hxx>
#include <ShapeFix_Shape.hxx>
#include <ShapeUpgrade_ConvertCurve2dToBezier.hxx>
#include <ShapeUpgrade_ShapeDivideAngle.hxx>
#include <ShapeUpgrade_ShapeConvertToBezier.hxx>
#include <Standard_Failure.hxx>
#include <STEPCAFControl_Reader.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColGeom2d_HArray1OfCurve.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_Shape.hxx>
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

#include "opencascade_to_k3d.h"

namespace module
{

namespace opencascade
{

namespace detail
{

typedef std::stack<TDF_ChildIterator> shapes_t;

struct implementation
{
	Handle(TDocStd_Document) xde_doc;
	shapes_t shapes;
};

bool on_bspline_surface(const Handle(Geom_BSplineSurface)& Patch, k3d::gprim_factory& Factory)
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
			Factory.add_point(k3d::point3(pole.X(), pole.Y(), pole.Z()));
			control_points.push_back(Factory.target_mesh().points->size()-1);
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
	
	return Factory.add_nurbs_patch(u_order, v_order, control_points, uknots, vknots, weights);
}

void on_nurbs_curve(const Handle(Geom_BSplineCurve)& Curve, k3d::gprim_factory& Factory)
{
	k3d::int32_t n_poles = Curve->NbPoles();
	k3d::mesh::indices_t points;
	k3d::mesh::weights_t weights;
	for (k3d::int32_t i = 1; i <= n_poles; ++i)
	{
		gp_Pnt pole = Curve->Pole(i);
		Factory.add_point(k3d::point3(pole.X(), pole.Y(), pole.Z()));
		points.push_back(Factory.target_mesh().points->size()-1);
		weights.push_back(Curve->Weight(i));
	}
	k3d::mesh::knots_t knots;
	k3d::int32_t n_knots = Curve->NbKnots();
	for (k3d::int32_t i = 1; i <= n_knots; ++i)
	{
		for (k3d::int32_t j = 0; j < Curve->Multiplicity(i); ++j)
			knots.push_back(Curve->Knot(i));
	}
	Factory.add_nurbs_curve(Curve->Degree() + 1, points, knots, weights);
}

void on_trim_curve(const Handle(Geom2d_BSplineCurve)& Curve, const Handle(Geom_Surface)& Surface, const Handle(Geom_Surface)& NurbsSurface, const double Precision, k3d::gprim_factory& Factory)
{
	if (Curve.IsNull())
		return;
	k3d::int32_t n_poles = Curve->NbPoles();
	k3d::mesh::points_2d_t points;
	k3d::mesh::weights_t weights;
	
	double u1, u2, v1, v2;
	NurbsSurface->Bounds(u1, u2, v1, v2);
	
	k3d::mesh::indices_t points_3d;
	
	for (k3d::int32_t i = 1; i <= n_poles; ++i)
	{
		gp_Pnt2d pole_uv = Curve->Pole(i); // UV coordinates of the control point on the original surface
		double u_min = pole_uv.X();
		double v_min = pole_uv.Y();
		gp_Pnt pole_3d = Surface->Value(u_min, v_min); // World position for the control point on the original surface
		double original_gap = pole_3d.Distance(NurbsSurface->Value(u_min, v_min));
		if (original_gap > Precision) // This only happens when we converted to NURBS manually
		{
			ShapeAnalysis_Surface surface_analyser(NurbsSurface);
			// Project onto the NURBS conversion, which may have a different parameter space
			gp_Pnt2d pole_uv_nurbs = surface_analyser.ValueOfUV(pole_3d, Precision);
			if (surface_analyser.Gap() > original_gap)
			{
				k3d::log() << debug << "Gap for projection: " << surface_analyser.Gap() << " while correcting original gap of " << original_gap << ". Using original." << std::endl;
			}
			else
			{
				// Make sure there is no "jump" to the other side of the parameter space
				u_min = pole_uv_nurbs.X();
				v_min = pole_uv_nurbs.Y();
				u_min = std::abs(pole_uv.X() - u_min) > 0.5 * std::abs(u2-u1) ? pole_uv.X() : u_min;
				v_min = std::abs(pole_uv.Y() - v_min) > 0.5 * std::abs(v2-v1) ? pole_uv.Y() : v_min;
			}
		}
		
		points.push_back(k3d::point2(u_min, v_min)); // Control point in UV space of the NURBS surface
		
		weights.push_back(Curve->Weight(i));
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
	
	Factory.add_trim_curve(order, points, knots, weights);
}


// process a face
void process_face(const TopoDS_Face& Face, k3d::gprim_factory& Factory, const double Precision)
{
	const double maxsize = 1000; // Maximum size of the bbox in any direction 
	const TopoDS_Face& face = Face;
	Handle(Geom_Surface) surface = BRep_Tool::Surface(face);
	TopExp_Explorer wire_explorer;
	Handle(Geom_BSplineSurface) nurbs_surface = Handle(Geom_BSplineSurface)::DownCast(surface);
	if (nurbs_surface.IsNull()) // Entered when somehow the global NURBS conversion yielded a non-NURBS face
	{
		k3d::log() << debug << "Performing explicit conversion to NURBS" << std::endl; 
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
			nurbs_surface = GeomConvert::SurfaceToBSplineSurface(bounded_surface);
		}
		catch(Standard_Failure& Exception)
		{
			k3d::log() << warning << "Conversion to NURBS failed: " << Exception.GetMessageString() << ". Skipping face." << std::endl;
			return;
		}
	}
	if (!on_bspline_surface(nurbs_surface, Factory))
	{
		return;
	}
	// Visit trim curves in their connected order, and add them to the mesh
	for (wire_explorer.Init(face, TopAbs_WIRE); wire_explorer.More(); wire_explorer.Next()) // wires (trim curve loops)
	{
		TopAbs_Orientation wire_orientation = wire_explorer.Current().Orientation();
		if (wire_orientation != TopAbs_FORWARD && wire_orientation != TopAbs_REVERSED) // not a trim curve loop
			continue;
		
		BRepTools_WireExplorer trim_explorer;
		// Stores curves in the order they need to be processed, reversing curves if needed
		typedef std::deque<Handle(Geom2d_BSplineCurve)> curves_t;
		curves_t curves;
		for (trim_explorer.Init(TopoDS::Wire(wire_explorer.Current()), face); trim_explorer.More(); trim_explorer.Next())
		{
			TopAbs_Orientation edge_orientation = trim_explorer.Current().Orientation();
			if (edge_orientation != TopAbs_FORWARD && edge_orientation != TopAbs_REVERSED)
				k3d::log() << warning << "Invalid trim curve orientation" << std::endl;
			double first = 0.0;
			double last = 0.0;
			Handle(Geom2d_Curve) curve = BRep_Tool::CurveOnSurface(TopoDS::Edge(trim_explorer.Current()), face, first, last);
			if (curve.IsNull())
			{
				k3d::log() << warning << "Skipping null trim curve" << std::endl;
				continue;
			}
			if (first < -maxsize) first = -maxsize;
			if (last > maxsize) last = maxsize;
			Handle(Geom2d_BSplineCurve) nurbs_trim = Handle(Geom2d_BSplineCurve)::DownCast(curve);
			if (nurbs_trim.IsNull()) // Encountered a non-NURBS trim curve, do explicit conversion
			{
				// Trim the curve, so the converter knows the endpoints
				Handle(Geom2d_TrimmedCurve) trimmed_curve = new Geom2d_TrimmedCurve(curve, first, last);
				try
				{
					ShapeUpgrade_ConvertCurve2dToBezier trim_to_bezier;
					trim_to_bezier.Init(trimmed_curve);
					trim_to_bezier.Perform(true);
					const Handle(TColGeom2d_HArray1OfCurve)& bezier_curves = trim_to_bezier.GetCurves();
					ShapeConstruct_CompBezierCurves2dToBSplineCurve2d bezier_to_bspline;
					for (k3d::int32_t i = bezier_curves->Lower(); i <= bezier_curves->Upper(); ++i)
					{
						const Handle(Geom2d_Curve)& collected_curve = bezier_curves->Value(i);
						Handle(Geom2d_BezierCurve) bezier_curve = Handle(Geom2d_BezierCurve)::DownCast(collected_curve);
						return_if_fail(!bezier_curve.IsNull());
						TColgp_Array1OfPnt2d poles(1, bezier_curve->NbPoles());
						bezier_curve->Poles(poles);
						bezier_to_bspline.AddCurve(poles);
					}
					bezier_to_bspline.Perform();
					TColgp_Array1OfPnt2d poles(1, bezier_to_bspline.NbPoles());
					TColStd_Array1OfReal knots(1, bezier_to_bspline.NbKnots());
					TColStd_Array1OfInteger multiplicities(1, bezier_to_bspline.NbKnots());
					bezier_to_bspline.Poles(poles);
					bezier_to_bspline.KnotsAndMults(knots, multiplicities);
					nurbs_trim = new Geom2d_BSplineCurve(poles, knots, multiplicities, bezier_to_bspline.Degree());
					return_if_fail(!nurbs_trim.IsNull());
				}
				catch(...)
				{
					k3d::log() << warning << "Failed converting trim curve to NURBS" << std::endl;
					return;
				}
			}
			
			Handle(Geom_Curve) curve_3d = BRep_Tool::Curve(TopoDS::Edge(trim_explorer.Current()), first, last);
			if (!curve_3d.IsNull())
			{
				Handle(Geom_BSplineCurve) nurbs_curve_3d = Handle(Geom_BSplineCurve)::DownCast(curve_3d);
				if (!nurbs_curve_3d.IsNull()) // Ignore 3D curves that were skipped in the conversion to NURBS
					on_nurbs_curve(nurbs_curve_3d, Factory);
			}
			
			if (edge_orientation != wire_orientation)
				nurbs_trim->Reverse();
			if (wire_orientation == TopAbs_FORWARD)
			{
				curves.push_back(nurbs_trim);
			}
			else
			{
				curves.push_front(nurbs_trim);
			}
		}
		for (curves_t::iterator curve = curves.begin(); curve != curves.end(); ++curve)
		{
			on_trim_curve(*curve, surface, nurbs_surface, Precision, Factory);
		}
		if (!curves.empty())
			Factory.close_trim_loop();
	}
}

// process surface components (Shape should at least be a Shell)
void process_surface(const TopoDS_Shape& Shape, k3d::gprim_factory& Factory)
{
	// Attempt some global shape fixing, as suggested by Matthias Teich on the OpenCascade forum.
	ShapeFix_Shape fixer(Shape);
	fixer.Perform();
	BRepBuilderAPI_Sewing sew;
	sew.Add(fixer.Shape());
	sew.Perform();
	const TopoDS_Shape& sewed_shape = sew.SewedShape();
	return_if_fail(!sewed_shape.IsNull());
	// Global precision for the conversion to NURBS
	const double precision = 1e-4;
	// Split up conversion by shell, to conserve memory
	TopExp_Explorer shell_explorer;
	for (shell_explorer.Init(Shape,TopAbs_SHELL); shell_explorer.More(); shell_explorer.Next())
	{
		try
		{
			// Eliminate all elementary surfaces, which would be ignored by the BSpline converter
			TopoDS_Shape revolution_shape = ShapeCustom::ConvertToRevolution(shell_explorer.Current());
			
			// Split closed shapes
			ShapeUpgrade_ShapeDivideAngle angle_tool(k3d::pi_over_2(), revolution_shape);
			if (!angle_tool.Perform() && angle_tool.Status (ShapeExtend_FAIL)) {
			  k3d::log() << debug << "Splitting of angles failed" << std::endl;
			  return;
			}
			
			ShapeUpgrade_ShapeConvertToBezier bezier_converter(angle_tool.Result());
			bezier_converter.SetSurfaceConversion(true);
			bezier_converter.SetRevolutionMode(true);
			bezier_converter.SetSurfaceSegmentMode(true);
			bezier_converter.Set2dConversion(true);
			bezier_converter.Set3dConversion(false);
			bezier_converter.SetBSplineMode(false);
			bezier_converter.SetPlaneMode(false);
			bezier_converter.Set3dConicConversion(true);
			if (!bezier_converter.Perform() && bezier_converter.Status(ShapeExtend_FAIL))
			{
				k3d::log() << debug << "Bezier conversion failed" << std::endl;
				return;
			}
			
			// Convert everything to NURBS
			Handle(ShapeCustom_RestrictionParameters) nurbs_parameters = new ShapeCustom_RestrictionParameters();
			// Set conversion parameters
			nurbs_parameters->ConvertBezierSurf() = true;
			nurbs_parameters->ConvertCurve2d() = true;
			nurbs_parameters->ConvertCurve3d() = true;
			nurbs_parameters->ConvertExtrusionSurf() = true;
			nurbs_parameters->ConvertOffsetCurv2d() = true;
			nurbs_parameters->ConvertOffsetCurv3d() = true;
			nurbs_parameters->ConvertOffsetSurf() = true;
			nurbs_parameters->ConvertPlane() = true;
			nurbs_parameters->ConvertRevolutionSurf() = true;
			nurbs_parameters->SegmentSurfaceMode() = true;
			// Execute the conversion
			TopoDS_Shape nurbs_shape = ShapeCustom::BSplineRestriction(bezier_converter.Result(),
					precision,
					precision,
					4,
					4096,
					GeomAbs_C2,
					GeomAbs_C2,
					false,
					false,
					nurbs_parameters);
		
			
			TopExp_Explorer face_explorer;
			for (face_explorer.Init(nurbs_shape,TopAbs_FACE); face_explorer.More(); face_explorer.Next())
			{
				process_face(TopoDS::Face(face_explorer.Current()), Factory, precision);
			}
		}
		catch(Standard_Failure& Exception)
		{
			k3d::log() << warning << "Error converting shell: " << Exception.GetMessageString() << std::endl; 
		}
		catch(...)
		{
			k3d::log() << warning << "Unknown error converting shell" << std::endl;
		}
	}
	// Explore all faces not part of a shell
	
	for (TopExp_Explorer lost_face_explorer(Shape, TopAbs_FACE, TopAbs_SHELL); lost_face_explorer.More(); lost_face_explorer.Next())
	{
		try
		{
			process_face(TopoDS::Face(lost_face_explorer.Current()), Factory, precision);
		}
		catch(Standard_Failure& Exception)
		{
			k3d::log() << warning << "Error converting lost face: " << Exception.GetMessageString() << std::endl; 
		}
		catch(...)
		{
			k3d::log() << warning << "Unknown error converting a lost face" << std::endl;
		}
	}
}

template<typename ReaderT> void load_file(const k3d::filesystem::path& FilePath, const TCollection_ExtendedString& TypeName, implementation* Implementation)
{
	ReaderT reader; // create a CAF reader, which supports reading attributes like shape names, colors, ...
	reader.SetNameMode(true); // Make sure we read names
	IFSelect_ReturnStatus status = reader.ReadFile(const_cast<char*>(FilePath.native_filesystem_string().c_str())); // Read the file
	if(status != IFSelect_RetDone)
	{
		k3d::log() << error << k3d_file_reference << ": error opening [" << FilePath.native_console_string() << "]" << std::endl;
		throw std::exception();
	}
	
	Implementation->xde_doc = new TDocStd_Document(TypeName); // initialise an empty document
	if (!reader.Transfer(Implementation->xde_doc)) // attempt to transfer the STEP file contents to the document
	{
		k3d::log() << error << "Failed to transfer OpenCascade file" << std::endl;
		throw std::exception();
	}
	
	if(!XCAFDoc_DocumentTool::IsXCAFDocument(Implementation->xde_doc)) // sanity check
	{
		k3d::log() << error << "Invalid document" << std::endl;
		throw std::exception();
	}
	
	TDF_Label shaperoot = XCAFDoc_DocumentTool::ShapesLabel(Implementation->xde_doc->Main()); // get the root node for the geometric shapes
	Implementation->shapes.push(TDF_ChildIterator(shaperoot));
}

} // namespace detail

opencascade_document_processor::opencascade_document_processor(const k3d::filesystem::path& FilePath)
{
	m_implementation = new detail::implementation;
	std::string extension = k3d::filesystem::extension(FilePath).lowercase().raw();
	k3d::log() << debug << "Loading file with extension " << extension << std::endl;
	if (extension == ".stp" || extension == ".step")
	{
		detail::load_file<STEPCAFControl_Reader>(FilePath, "STEP", m_implementation);
	}
	else if (extension == ".igs" || extension == ".iges")
	{
		detail::load_file<IGESCAFControl_Reader>(FilePath, "IGES", m_implementation);
	}
	else if (extension == ".brep" || extension == ".rle")
	{
		m_implementation->xde_doc = new TDocStd_Document("BREP");
		TDF_Label shaperoot = m_implementation->xde_doc->Main().FindChild(1);
		TopoDS_Shape shape;
		BRep_Builder brep_builder;
		if(BRepTools::Read(shape, const_cast<char*>(FilePath.native_filesystem_string().c_str()), brep_builder))
			TDataStd_Shape::Set(shaperoot.NewChild(), shape);
		else
			k3d::log() << debug << "failed to find any shapes in BREP file" << std::endl;
		m_implementation->shapes.push(TDF_ChildIterator(shaperoot));
	}
	else
	{
		k3d::log() << error << "File format not supported by OpenCascade importer" << std::endl;
		throw std::exception();
	}
}

opencascade_document_processor::~opencascade_document_processor()
{
	delete m_implementation;
}

void opencascade_document_processor::process_current(k3d::gprim_factory& Factory, std::string& Name) const
{
	return_if_fail(!m_implementation->shapes.empty());
	const TDF_Label label = m_implementation->shapes.top().Value();
	return_if_fail(!label.IsNull());
	Handle(TDataStd_Name) N;
	std::stringstream string_stream;
	if (label.FindAttribute(TDataStd_Name::GetID(),N))
	{
	  string_stream << N->Get();
	}
  Name = string_stream.str();
  if (Name.empty())
  	Name = "OpenCascade shape";
	
	TopoDS_Shape shape = TDataStd_Shape::Get(label);
	if (!shape.IsNull())
	{
		try
		{
			switch(shape.ShapeType())
			{
			case TopAbs_COMPOUND:
				detail::process_surface(shape, Factory);
				break;
			case TopAbs_COMPSOLID:
				detail::process_surface(shape, Factory);
				break;
			case TopAbs_SOLID:
				detail::process_surface(shape, Factory);
				break;
			case TopAbs_SHELL:
				detail::process_surface(shape, Factory);
				break;
			case TopAbs_FACE:
				detail::process_face(TopoDS::Face(shape), Factory, 1e-4);
				break;
			case TopAbs_WIRE:  // Note: all shapes below here should only be part of the shapes above here
				break;
			case TopAbs_EDGE:
				break;
			case TopAbs_VERTEX:
				break;
			case TopAbs_SHAPE:
				break;
			}
		}
		catch(Standard_Failure& Exception)
		{
			k3d::log() << warning << "Error converting shape " << Name << ": " << Exception.GetMessageString() << std::endl; 
		}
		catch(...)
		{
			k3d::log() << warning << "Unknown error converting shape " << Name << std::endl;
		}
	}
	else
	{
		k3d::log() << debug << "Found node that is not a shape" << std::endl;
	}
}

bool opencascade_document_processor::has_children() const
{
	return_val_if_fail(!m_implementation->shapes.empty(), false);
	const TDF_Label label = m_implementation->shapes.top().Value();
	return_val_if_fail(!label.IsNull(), false);
	return label.HasChild();
}

void opencascade_document_processor::increase_level()
{
	return_if_fail(!m_implementation->shapes.empty());
	const TDF_Label label = m_implementation->shapes.top().Value();
	return_if_fail(!label.IsNull());
	return_if_fail(label.HasChild());
	m_implementation->shapes.push(TDF_ChildIterator(label));
}

void opencascade_document_processor::decrease_level()
{
	if (m_implementation->shapes.size() > 1)
		m_implementation->shapes.pop();
}

void opencascade_document_processor::next()
{
	return_if_fail(!m_implementation->shapes.empty());
	return_if_fail(m_implementation->shapes.top().More());
	m_implementation->shapes.top().Next();
}

bool opencascade_document_processor::more() const
{
	return_val_if_fail(!m_implementation->shapes.empty(), false);
	return m_implementation->shapes.top().More();
}

} // opencascade

} // module
