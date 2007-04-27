#ifndef K3DSDK_IRENDER_ENGINE_RI_H
#define K3DSDK_IRENDER_ENGINE_RI_H

// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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

#include "iunknown.h"
#include "types_ri.h"

namespace k3d
{

namespace ri
{

/// Abstract binding to the Pixar RenderMan interface
class irender_engine :
	public virtual iunknown
{
public:
	virtual bool set_inline_types(const bool Inline) = 0;

	virtual const light_handle RiAreaLightSourceV(const path& Path, const string& Name, const parameter_list& Parameters = parameter_list()) = 0;
	virtual void RiAtmosphereV(const path& Path, const string& Name, const parameter_list& Parameters = parameter_list()) = 0;
	virtual void RiAttributeBegin() = 0;
	virtual void RiAttributeEnd() = 0;
	virtual void RiAttributeV(const string& Name, const parameter_list& Parameters = parameter_list()) = 0;
	virtual void RiBasis(const matrix& UBasis, const unsigned_integer UStep, const matrix& VBasis, const unsigned_integer VStep) = 0;
	virtual void RiBasis(const string& UBasis, const unsigned_integer UStep, const string& VBasis, const unsigned_integer VStep) = 0;
	virtual void RiBlobbyV(const unsigned_integer NLeaf, const unsigned_integers& Codes, const reals& Floats, const strings& Strings, const parameter_list& Parameters = parameter_list()) = 0;
	virtual void RiBound(const boost::array<real, 6>& Bound) = 0;
	virtual void RiClipping(const real Hither, const real Yon) = 0;
	virtual void RiColor(const color& Color) = 0;
	virtual void RiColorSamples(const unsigned_integer ParameterCount, const reals& nRGB, const reals& RGBn) = 0;
	virtual void RiComment(const string& Comment) = 0;
	virtual void RiConcatTransform(const matrix& Transform) = 0;
	virtual void RiConeV(const real Height, const real Radius, const real ThetaMax, const parameter_list& Parameters = parameter_list()) = 0;
	virtual void RiCoordSysTransform(const string& Space) = 0;
	virtual void RiCoordinateSystem(const string& Space) = 0;
	virtual void RiCropWindow(const real XMin, const real XMax, const real YMin, const real YMax) = 0;
	virtual void RiCurvesV(const string& Type, const unsigned_integers& VertexCounts, const string& Wrap, const parameter_list& Parameters = parameter_list()) = 0;
	virtual void RiCylinderV(const real Radius, const real ZMin, const real ZMax, const real ThetaMax, const parameter_list& Parameters = parameter_list()) = 0;
	virtual void RiDeclare(const string& Name, const string& Type) = 0;
	virtual void RiDeformationV(const path& Path, const string& Name, const parameter_list& Parameters = parameter_list()) = 0;
	virtual void RiDepthOfField(const real FStop, const real FocalLength, const real FocalDistance) = 0;
	virtual void RiDetail(const boost::array<real, 6>& Bound) = 0;
	virtual void RiDetailRange(const real MinVis, const real LowTran, const real UpTran, const real MaxVis) = 0;
	virtual void RiDiskV(real Height, real Radius, real ThetaMax, const parameter_list& Parameters = parameter_list()) = 0;
	virtual void RiDisplacementV(const path& Path, const string& Name, const parameter_list& Parameters = parameter_list()) = 0;
	virtual void RiDisplayV(const string& Name, const string& Type, const string& Mode, const parameter_list& Parameters = parameter_list()) = 0;
	virtual void RiErrorHandler(const string& Style) = 0;
	virtual void RiExposure(const real Gain, const real Gamma) = 0;
	virtual void RiExteriorV(const path& Path, const string& Name, const parameter_list& Parameters = parameter_list()) = 0;
	virtual void RiFormat(const unsigned_integer XResolution, const unsigned_integer YResolution, const real AspectRatio) = 0;
	virtual void RiFrameAspectRatio(const real AspectRatio) = 0;
	virtual void RiFrameBegin(const unsigned_integer FrameNumber) = 0;
	virtual void RiFrameEnd() = 0;
	virtual void RiGeneralPolygonV(const unsigned_integers& VertexCounts, const parameter_list& Parameters = parameter_list()) = 0;
	virtual void RiGeometricApproximation(const string& Type, const real Value) = 0;
	virtual void RiGeometricRepresentation(const string& Type) = 0;
	virtual void RiGeometryV(const string& Type, const parameter_list& Parameters = parameter_list()) = 0;
	virtual void RiHiderV(const string& Type, const parameter_list& Parameters = parameter_list()) = 0;
	virtual void RiHyperboloidV(const point& Point1, const point& Point2, const real ThetaMax, const parameter_list& Parameters = parameter_list()) = 0;
	virtual void RiIdentity() = 0;
	virtual void RiIlluminate(const light_handle LightHandle, const bool OnOff) = 0;
	virtual void RiImagerV(const path& Path, const string& Name, const parameter_list& Parameters = parameter_list()) = 0;
	virtual void RiInteriorV(const path& Path, const string& Name, const parameter_list& Parameters = parameter_list()) = 0;
	virtual const light_handle RiLightSourceV(const path& Path, const string& Name, const parameter_list& Parameters = parameter_list()) = 0;
	virtual void RiMakeCubeFaceEnvironmentV(const string& px, const string& nx, const string& py, const string& ny, const string& pz, const string& nz, const string& texturename, const real fov, const string& swrap, const string& twrap, const string& filterfunc, const real swidth, const real twidth, const parameter_list& Parameters = parameter_list()) = 0;
	virtual void RiMakeLatLongEnvironmentV(const string& picturename, const string& texturename, const string& filterfunc, const real swidth, const real twidth, const parameter_list& Parameters = parameter_list()) = 0;
	virtual void RiMakeShadowV(const string& picturename, const string& texturename, const parameter_list& Parameters = parameter_list()) = 0;
	virtual void RiMakeTextureV(const string& picturename, const string& texturename, const string& swrap, const string& twrap, const string& filterfunc, const real swidth, const real twidth, const parameter_list& Parameters = parameter_list()) = 0;
	virtual void RiMatte(const bool OnOff) = 0;
	virtual void RiMotionBeginV(const sample_times_t& Times) = 0;
	virtual void RiMotionEnd() = 0;
	virtual void RiNewline() = 0;
	virtual void RiNuPatchV(const unsigned_integer UCount, const unsigned_integer UOrder, const reals& UKnot, const real UMin, const real UMax, const unsigned_integer VCount, const unsigned_integer VOrder, const reals& VKnot, const real VMin, const real VMax, const parameter_list& Parameters = parameter_list()) = 0;
	virtual const object_handle RiObjectBegin() = 0;
	virtual void RiObjectEnd() = 0;
	virtual void RiObjectInstance(const object_handle Object) = 0;
	virtual void RiOpacity(const color& Opacity) = 0;
	virtual void RiOptionV(const string& Name, const parameter_list& Parameters = parameter_list()) = 0;
	virtual void RiOrientation(const string& Orientation) = 0;
	virtual void RiParaboloidV(const real RMax, const real ZMin, const real ZMax, const real ThetaMax, const parameter_list& Parameters = parameter_list()) = 0;
	virtual void RiPatchMeshV(const string& Type, const unsigned_integer UCount, const string& UWrap, const unsigned_integer VCount, const string& VWrap, const parameter_list& Parameters = parameter_list()) = 0;
	virtual void RiPatchV(const string& Type, const parameter_list& Parameters = parameter_list()) = 0;
	virtual void RiPerspective(const real FieldOfView) = 0;
	virtual void RiPixelFilter(const string& FilterName, const real XWidth, const real YWidth) = 0;
	virtual void RiPixelSamples(const real XSamples, const real YSamples) = 0;
	virtual void RiPixelVariance(const real Variation) = 0;
	virtual void RiPointsGeneralPolygonsV(const unsigned_integers& LoopCounts, const unsigned_integers& VertexCounts, const unsigned_integers& VertexIDs, const parameter_list& Parameters = parameter_list()) = 0;
	virtual void RiPointsPolygonsV(const unsigned_integers& VertexCounts, const unsigned_integers& VertexIDs, const parameter_list& Parameters = parameter_list()) = 0;
	virtual	void RiPointsV(const unsigned_integer VertexCount, const parameter_list& Parameters = parameter_list()) = 0;
	virtual void RiPolygonV(const unsigned_integer VertexCount, const parameter_list& Parameters = parameter_list()) = 0;
	virtual void RiProcDelayedReadArchive(const path& Archive, const bound& BoundingBox) = 0;
	virtual void RiProjectionV(const string& Name, const parameter_list& Parameters = parameter_list()) = 0;
	virtual void RiQuantize(const string& Type, const integer One, const integer QMin, const integer QMax, const real Amplitude) = 0;
	virtual void RiReadArchive(const path& Archive) = 0;
	virtual void RiRelativeDetail(const real RelativeDetail) = 0;
	virtual void RiReverseOrientation() = 0;
	virtual void RiRotate(const real angle, const real DX, const real DY, const real DZ) = 0;
	virtual void RiScale(const real DX, const real DY, const real DZ) = 0;
	virtual void RiScreenWindow(const real Left, const real Right, const real Bottom, const real Top) = 0;
	virtual void RiShadingInterpolation(const string& Type) = 0;
	virtual void RiShadingRate(const real Size) = 0;
	virtual void RiShutter(const real SMin, const real SMax) = 0;
	virtual void RiSides(const unsigned_integer Sides) = 0;
	virtual void RiSkew(const real Angle, const real DX1, const real DY1, const real DZ1, const real DX2, const real DY2, const real DZ2) = 0;
	virtual void RiSolidBegin(const string& Type) = 0;
	virtual void RiSolidEnd() = 0;
	virtual void RiSphereV(const real Radius, const real ZMin, const real ZMax, const real ThetaMax, const parameter_list& Parameters = parameter_list()) = 0;
	virtual void RiStructure(const string& Structure) = 0;
	virtual void RiSubdivisionMeshV(const string& Scheme, const unsigned_integers& VertexCounts, const unsigned_integers& VertexIDs, const strings& Tags, const unsigned_integers& ArgCounts, const integers& IntegerArgs, const reals& FloatArgs, const parameter_list& Parameters = parameter_list()) = 0;
	virtual void RiSurfaceV(const path& Path, const string& Name, const parameter_list& Parameters = parameter_list()) = 0;
	virtual void RiTextureCoordinates(const real S1, const real T1, const real S2, const real T2, const real S3, const real T3, const real S4, const real T4) = 0;
	virtual void RiTorusV(const real MajorRadius, const real MinorRadius, const real PhiMin, const real PhiMax, const real ThetaMax, const parameter_list& Parameters = parameter_list()) = 0;
	virtual void RiTransform(const matrix& Transform) = 0;
	virtual void RiTransformBegin() = 0;
	virtual void RiTransformEnd() = 0;
	virtual void RiTranslate(const real DX, const real DY, const real DZ) = 0;
	virtual void RiTrimCurve(const unsigned_integer LoopCount, const unsigned_integers& CurveCounts, const unsigned_integers& Orders, const reals& Knots, const reals& Minimums, const reals& Maximums, const unsigned_integers& KnotCounts, const reals& U, const reals& V, const reals& W) = 0;
	virtual void RiWorldBegin() = 0;
	virtual void RiWorldEnd() = 0;

	// New Aqsis-specific calls introduced to support layered shaders
	//
	/** \todo Come up with a better way to handle non-standard calls */
	virtual void RiShaderLayerV(const std::string& type, const path& Path, const std::string& name, const std::string& layername, const parameter_list& Parameters = parameter_list()) = 0;
	/** \todo Come up with a better way to handle non-standard calls */
	virtual void RiConnectShaderLayers(const std::string& type, const std::string& layer1, const std::string& variable1, const std::string& layer2, const std::string& variable2) = 0;


protected:
	irender_engine() {}
	irender_engine(const irender_engine& ) {}
	irender_engine& operator=(const irender_engine& ) { return *this; }
	virtual ~irender_engine() {}
};

/// Provide definitions for the "standard" RenderMan tokens
inline const std::string RI_A() { return "a"; }
inline const std::string RI_ABORT() { return "abort"; }
inline const std::string RI_AMBIENTLIGHT() { return "ambientlight"; }
inline const std::string RI_AMPLITUDE() { return "amplitude"; }
inline const std::string RI_AZ() { return "az"; }
inline const std::string RI_BACKGROUND() { return "background"; }
inline const std::string RI_BEAMDISTRIBUTION() { return "beamdistribution"; }
inline const std::string RI_BICUBIC() { return "bicubic"; }
inline const std::string RI_BILINEAR() { return "bilinear"; }
inline const std::string RI_BLACK() { return "black"; }
inline const std::string RI_BOX() { return "box"; }
inline const std::string RI_BUMPY() { return "bumpy"; }
inline const std::string RI_CAMERA() { return "camera"; }
inline const std::string RI_CATMULL_ROM() { return "catmull-rom"; }
inline const std::string RI_CLAMP() { return "clamp"; }
inline const std::string RI_COMMENT() { return "comment"; }
inline const std::string RI_CONEANGLE() { return "coneangle"; }
inline const std::string RI_CONEDELTAANGLE() { return "conedeltaangle"; }
inline const std::string RI_CONSTANT() { return "constant"; }
inline const std::string RI_CONSTANTWIDTH() { return "constantwidth"; }
inline const std::string RI_CS() { return "Cs"; }
inline const std::string RI_CUBIC() { return "cubic"; }
inline const std::string RI_DEPTHCUE() { return "depthcue"; }
inline const std::string RI_DIFFERENCE() { return "difference"; }
inline const std::string RI_DISTANCE() { return "distance"; }
inline const std::string RI_DISTANTLIGHT() { return "distantlight"; }
inline const std::string RI_FILE() { return "file"; }
inline const std::string RI_FLATNESS() { return "flatness"; }
inline const std::string RI_FOG() { return "fog"; }
inline const std::string RI_FOV() { return "fov"; }
inline const std::string RI_FRAMEBUFFER() { return "framebuffer"; }
inline const std::string RI_FROM() { return "from"; }
inline const std::string RI_GAUSSIAN() { return "gaussian"; }
inline const std::string RI_HANDLER() { return "handler"; }
inline const std::string RI_HIDDEN() { return "hidden"; }
inline const std::string RI_IDENTIFIER() { return "identifier"; }
inline const std::string RI_IGNORE() { return "ignore"; }
inline const std::string RI_INSIDE() { return "inside"; }
inline const std::string RI_INTENSITY() { return "intensity"; }
inline const std::string RI_INTERSECTION() { return "intersection"; }
inline const std::string RI_KA() { return "Ka"; }
inline const std::string RI_KD() { return "Kd"; }
inline const std::string RI_KR() { return "Kr"; }
inline const std::string RI_KS() { return "Ks"; }
inline const std::string RI_LH() { return "lh"; }
inline const std::string RI_LIGHTCOLOR() { return "lightcolor"; }
inline const std::string RI_LINEAR() { return "linear"; }
inline const std::string RI_MATTE() { return "matte"; }
inline const std::string RI_MAXDISTANCE() { return "maxdistance"; }
inline const std::string RI_METAL() { return "metal"; }
inline const std::string RI_MINDISTANCE() { return "mindistance"; }
inline const std::string RI_N() { return "N"; }
inline const std::string RI_NAME() { return "name"; }
inline const std::string RI_NONPERIODIC() { return "nonperiodic"; }
inline const std::string RI_NP() { return "Np"; }
inline const std::string RI_OBJECT() { return "object"; }
inline const std::string RI_ORIGIN() { return "origin"; }
inline const std::string RI_ORTHOGRAPHIC() { return "orthographic"; }
inline const std::string RI_OS() { return "Os"; }
inline const std::string RI_OUTSIDE() { return "outside"; }
inline const std::string RI_P() { return "P"; }
inline const std::string RI_PAINT() { return "paint"; }
inline const std::string RI_PAINTEDPLASTIC() { return "paintedplastic"; }
inline const std::string RI_PERIODIC() { return "periodic"; }
inline const std::string RI_PERSPECTIVE() { return "perspective"; }
inline const std::string RI_PLASTIC() { return "plastic"; }
inline const std::string RI_POINTLIGHT() { return "pointlight"; }
inline const std::string RI_PRIMITIVE() { return "primitive"; }
inline const std::string RI_PRINT() { return "print"; }
inline const std::string RI_PW() { return "Pw"; }
inline const std::string RI_PZ() { return "Pz"; }
inline const std::string RI_RASTER() { return "raster"; }
inline const std::string RI_RGB() { return "rgb"; }
inline const std::string RI_RGBA() { return "rgba"; }
inline const std::string RI_RGBAZ() { return "rgbaz"; }
inline const std::string RI_RGBZ() { return "rgbz"; }
inline const std::string RI_RH() { return "rh"; }
inline const std::string RI_ROUGHNESS() { return "roughness"; }
inline const std::string RI_S() { return "s"; }
inline const std::string RI_SCREEN() { return "screen"; }
inline const std::string RI_SHINYMETAL() { return "shinymetal"; }
inline const std::string RI_SINC() { return "sinc"; }
inline const std::string RI_SMOOTH() { return "smooth"; }
inline const std::string RI_SPECULARCOLOR() { return "specularcolor"; }
inline const std::string RI_SPOTLIGHT() { return "spotlight"; }
inline const std::string RI_ST() { return "st"; }
inline const std::string RI_STRUCTURE() { return "structure"; }
inline const std::string RI_T() { return "t"; }
inline const std::string RI_TEXTURENAME() { return "texturename"; }
inline const std::string RI_TO() { return "to"; }
inline const std::string RI_TRIANGLE() { return "triangle"; }
inline const std::string RI_UNION() { return "union"; }
inline const std::string RI_WIDTH() { return "width"; }
inline const std::string RI_WORLD() { return "world"; }
inline const std::string RI_Z() { return "z"; }

} // namespace ri

} // namespace k3d

#endif // K3DSDK_IRENDER_ENGINE_RI_H

