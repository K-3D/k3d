#ifndef K3DSDK_STREAM_RI_H
#define K3DSDK_STREAM_RI_H

// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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
	\author Tim Shead (tshead@k-3d.com)
*/

#include "istream_ri.h"
#include <set>

namespace k3d
{

namespace ri
{

/////////////////////////////////////////////////////////////////////////////
// stream

/// Provides a boilerplate implementation of k3d::ri::istream that writes RIB to a stream
class stream :
	public istream
{
public:
	stream(std::ostream& Stream);
	~stream();

	bool set_inline_types(const bool Inline);
	void use_shader(const path& Path);

	const light_handle RiAreaLightSourceV(const path& Path, const string& Name, const parameter_list& Parameters = parameter_list());
	void RiPointsV(const unsigned_integer VertexCount, const parameter_list& Parameters = parameter_list());
	void RiAtmosphereV(const path& Path, const string& Name, const parameter_list& Parameters = parameter_list());
	void RiAttributeBegin();
	void RiAttributeEnd();
	void RiAttributeV(const string& Name, const parameter_list& Parameters = parameter_list());
	void RiBasis(const matrix& UBasis, const unsigned_integer UStep, const matrix& VBasis, const unsigned_integer VStep);
	void RiBasis(const string& UBasis, const unsigned_integer UStep, const string& VBasis, const unsigned_integer VStep);
	void RiBlobbyV(const unsigned_integer NLeaf, const unsigned_integers& Codes, const reals& Floats, const strings& Strings, const parameter_list& Parameters = parameter_list());
	void RiBound(const boost::array<real, 6>& Bound);
	void RiClipping(const real Hither, const real Yon);
	void RiColor(const color& Color);
	void RiColorSamples(const unsigned_integer ParameterCount, const reals& nRGB, const reals& RGBn);
	void RiComment(const string& Comment);
	void RiConcatTransform(const matrix& Transform);
	void RiConeV(const real Height, const real Radius, const real ThetaMax, const parameter_list& Parameters = parameter_list());
	void RiCoordSysTransform(const string& Space);
	void RiCoordinateSystem(const string& Space);
	void RiCropWindow(const real XMin, const real XMax, const real YMin, const real YMax);
	void RiCurvesV(const string& Type, const unsigned_integers& VertexCounts, const string& Wrap, const parameter_list& Parameters = parameter_list());
	void RiCylinderV(const real Radius, const real ZMin, const real ZMax, const real ThetaMax, const parameter_list& Parameters = parameter_list());
	void RiDeclare(const string& Name, const string& Type);
	void RiDeformationV(const path& Path, const string& Name, const parameter_list& Parameters = parameter_list());
	void RiDepthOfField(const real FStop, const real FocalLength, const real FocalDistance);
	void RiDetail(const boost::array<real, 6>& Bound);
	void RiDetailRange(const real MinVis, const real LowTran, const real UpTran, const real MaxVis);
	void RiDiskV(real Height, real Radius, real ThetaMax, const parameter_list& Parameters = parameter_list());
	void RiDisplacementV(const path& Path, const string& Name, const parameter_list& Parameters = parameter_list());
	void RiDisplayV(const string& Name, const string& Type, const string& Mode, const parameter_list& Parameters = parameter_list());
	void RiErrorHandler(const string& Style);
	void RiExposure(const real Gain, const real Gamma);
	void RiExteriorV(const path& Path, const string& Name, const parameter_list& Parameters = parameter_list());
	void RiFormat(const unsigned_integer XResolution, const unsigned_integer YResolution, const real AspectRatio);
	void RiFrameAspectRatio(const real AspectRatio);
	void RiFrameBegin(const unsigned_integer FrameNumber);
	void RiFrameEnd();
	void RiGeneralPolygonV(const unsigned_integers& VertexCounts, const parameter_list& Parameters = parameter_list());
	void RiGeometricApproximation(const string& Type, const real Value);
	void RiGeometricRepresentation(const string& Type);
	void RiGeometryV(const string& Type, const parameter_list& Parameters = parameter_list());
	void RiHiderV(const string& Type, const parameter_list& Parameters = parameter_list());
	void RiHyperboloidV(const point& Point1, const point& Point2, const real ThetaMax, const parameter_list& Parameters = parameter_list());
	void RiIdentity();
	void RiIlluminate(const light_handle LightHandle, const bool OnOff);
	void RiImagerV(const path& Path, const string& Name, const parameter_list& Parameters = parameter_list());
	void RiInteriorV(const path& Path, const string& Name, const parameter_list& Parameters = parameter_list());
	const light_handle RiLightSourceV(const path& Path, const string& Name, const parameter_list& Parameters = parameter_list());
	void RiMakeCubeFaceEnvironmentV(const string& px, const string& nx, const string& py, const string& ny, const string& pz, const string& nz, const string& texturename, const real fov, const string& swrap, const string& twrap, const string& filterfunc, const real swidth, const real twidth, const parameter_list& Parameters = parameter_list());
	void RiMakeLatLongEnvironmentV(const string& picturename, const string& texturename, const string& filterfunc, const real swidth, const real twidth, const parameter_list& Parameters = parameter_list());
	void RiMakeShadowV(const string& picturename, const string& texturename, const parameter_list& Parameters = parameter_list());
	void RiMakeTextureV(const string& picturename, const string& texturename, const string& swrap, const string& twrap, const string& filterfunc, const real swidth, const real twidth, const parameter_list& Parameters = parameter_list());
	void RiMatte(const bool OnOff);
	void RiMotionBeginV(const sample_times_t& Times);
	void RiMotionEnd();
	void RiNewline();
	void RiNuPatchV(const unsigned_integer UCount, const unsigned_integer UOrder, const reals& UKnot, const real UMin, const real UMax, const unsigned_integer VCount, const unsigned_integer VOrder, const reals& VKnot, const real VMin, const real VMax, const parameter_list& Parameters = parameter_list());
	const object_handle RiObjectBegin();
	void RiObjectEnd();
	void RiObjectInstance(const object_handle Object);
	void RiOpacity(const color& Opacity);
	void RiOptionV(const string& Name, const parameter_list& Parameters = parameter_list());
	void RiOrientation(const string& Orientation);
	void RiParaboloidV(const real RMax, const real ZMin, const real ZMax, const real ThetaMax, const parameter_list& Parameters = parameter_list());
	void RiPatchMeshV(const string& Type, const unsigned_integer UCount, const string& UWrap, const unsigned_integer VCount, const string& VWrap, const parameter_list& Parameters = parameter_list());
	void RiPatchV(const string& Type, const parameter_list& Parameters = parameter_list());
	void RiPerspective(const real FieldOfView);
	void RiPixelFilter(const string& FilterName, const real XWidth, const real YWidth);
	void RiPixelSamples(const real XSamples, const real YSamples);
	void RiPixelVariance(const real Variation);
	void RiPointsGeneralPolygonsV(const unsigned_integers& LoopCounts, const unsigned_integers& VertexCounts, const unsigned_integers& VertexIDs, const parameter_list& Parameters = parameter_list());
	void RiPointsPolygonsV(const unsigned_integers& VertexCounts, const unsigned_integers& VertexIDs, const parameter_list& Parameters = parameter_list());
	void RiPolygonV(const unsigned_integer VertexCount, const parameter_list& Parameters = parameter_list());
	void RiProcDelayedReadArchive(const path& Archive, const bound& BoundingBox);
	void RiProjectionV(const string& Name, const parameter_list& Parameters = parameter_list());
	void RiQuantize(const string& Type, const integer One, const integer QMin, const integer QMax, const real Amplitude);
	void RiReadArchive(const path& Archive);
	void RiRelativeDetail(const real RelativeDetail);
	void RiReverseOrientation();
	void RiRotate(const real angle, const real DX, const real DY, const real DZ);
	void RiScale(const real DX, const real DY, const real DZ);
	void RiScreenWindow(const real Left, const real Right, const real Bottom, const real Top);
	void RiShadingInterpolation(const string& Type);
	void RiShadingRate(const real Size);
	void RiShutter(const real SMin, const real SMax);
	void RiSides(const unsigned_integer Sides);
	void RiSkew(const real Angle, const real DX1, const real DY1, const real DZ1, const real DX2, const real DY2, const real DZ2);
	void RiSolidBegin(const string& Type);
	void RiSolidEnd();
	void RiSphereV(const real Radius, const real ZMin, const real ZMax, const real ThetaMax, const parameter_list& Parameters = parameter_list());
	void RiStructure(const string& Structure);
	void RiSubdivisionMeshV(const string& Scheme, const unsigned_integers& VertexCounts, const unsigned_integers& VertexIDs, const strings& Tags, const unsigned_integers& ArgCounts, const integers& IntegerArgs, const reals& FloatArgs, const parameter_list& Parameters = parameter_list());
	void RiSurfaceV(const path& Path, const string& Name, const parameter_list& Parameters = parameter_list());
	void RiTextureCoordinates(const real S1, const real T1, const real S2, const real T2, const real S3, const real T3, const real S4, const real T4);
	void RiTorusV(const real MajorRadius, const real MinorRadius, const real PhiMin, const real PhiMax, const real ThetaMax, const parameter_list& Parameters = parameter_list());
	void RiTransform(const matrix& Transform);
	void RiTransformBegin();
	void RiTransformEnd();
	void RiTranslate(const real DX, const real DY, const real DZ);
	void RiTrimCurve(const unsigned_integers& CurveCounts, const unsigned_integers& Orders, const reals& Knots, const reals& Minimums, const reals& Maximums, const unsigned_integers& PointCounts, const reals& U, const reals& V, const reals& W);
	void RiWorldBegin();
	void RiWorldEnd();

	void RiShaderLayerV(const std::string& type, const path& Path, const std::string& name, const std::string& layername, const parameter_list& Parameters = parameter_list());
	void RiConnectShaderLayers(const std::string& type, const std::string& layer1, const std::string& variable1, const std::string& layer2, const std::string& variable2);

private:
	class implementation;
	implementation* const m_implementation;
};

} // namespace ri

} // namespace k3d

#endif // K3DSDK_STREAM_RI_H

