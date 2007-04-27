#ifndef K3DSDK_CLASSES_H
#define K3DSDK_CLASSES_H

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
		\brief Provides public definitions of a subset of commonly-used class IDs
		\author Tim Shead (tshead@k-3d.com)
*/

#include "uuid.h"

namespace k3d
{

// Forward declarations
class iunknown;

namespace classes
{

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
////
//// DO NOT ADD TO THIS LIST!!!
////
//// You DO NOT have to add your objects' class IDs to this list to get them recognized
//// by K-3D!  This list only defines the class IDs for a subset of commonly-used objects
//// that need to be explicitly instantiated by other objects (like import plugins).
////
//// In general, you should never care about an object's class ID - only its capabilities,
//// as advertised by its interfaces!
////
//// Never, never change the values of the class IDs in this list - they get serialized in
//// documents, and are used to thaw out the correct object types!
////
//// Now go call your mother!
////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

/** \name Active class IDs */
//@{
inline const uuid Axes() { return uuid(0x00000001, 0x00000000, 0x00000000, 0x00000001); }
inline const uuid FileBitmap() { return uuid(0x00000001, 0x00000000, 0x00000000, 0x00000003); }
inline const uuid RenderManLight() { return uuid(0x00000001, 0x00000000, 0x00000000, 0x0000001d); }
inline const uuid RenderManMaterial() { return uuid(0x00000001, 0x00000000, 0x00000000, 0x00000024); }
inline const uuid Cone() { return uuid(0x00000001, 0x00000000, 0x00000000, 0x00000064); }
inline const uuid Cylinder() { return uuid(0x00000001, 0x00000000, 0x00000000, 0x00000065); }
inline const uuid Disk() { return uuid(0x00000001, 0x00000000, 0x00000000, 0x00000066); }
inline const uuid Hyperboloid() { return uuid(0x00000001, 0x00000000, 0x00000000, 0x00000067); }
inline const uuid Paraboloid() { return uuid(0x00000001, 0x00000000, 0x00000000, 0x00000068); }
inline const uuid Sphere() { return uuid(0x00000001, 0x00000000, 0x00000000, 0x00000069); }
inline const uuid Torus() { return uuid(0x00000001, 0x00000000, 0x00000000, 0x0000006a); }
inline const uuid PerlEngine() { return uuid(0x00000001, 0x00000000, 0x00000000, 0x00000075); }
inline const uuid PythonEngine() { return uuid(0x00000001, 0x00000000, 0x00000000, 0x00000076); }
inline const uuid ScalarBezierChannel() { return uuid(0x00000001, 0x00000000, 0x00000000, 0x0000007c); }
inline const uuid ColorBezierChannel() { return uuid(0x00000001, 0x00000000, 0x00000000, 0x0000007d); }
inline const uuid TIFFWriter() { return uuid(0x00000001, 0x00000000, 0x00000000, 0x00000085); }
inline const uuid FrozenMesh() { return uuid(0x305ceb88, 0x7cf34870, 0x8b5e673c, 0x636beab9); }
inline const uuid MeshInstance() { return uuid(0x2c1d60a6, 0x863541a1, 0xbd649bd1, 0x1ea8ae24); }
inline const uuid TimeSource() { return uuid(0x1978f1f7, 0x4a864c99, 0x8caf7cfc, 0xd2591baf); }
inline const uuid OpenGLEngine() { return uuid(0x0059ba47, 0x7bf740f3, 0x93fb523d, 0x5899f22c); }
inline const uuid RenderManEngine() { return uuid(0x0dc5e733, 0xfa684e05, 0x86a3865f, 0x42f7a99d); }
inline const uuid RenderManDisplacementShader() { return uuid(0x62827bb7, 0x4cb64427, 0xa9eadbc8, 0x62e1a3f2); }
inline const uuid RenderManImagerShader() { return uuid(0x91659033, 0xbd124ba0, 0x81eabb0a, 0xc5ecd094); }
inline const uuid RenderManLightShader() { return uuid(0x830e867b, 0x7bc94c6c, 0x8d5087ee, 0x8aa35db2); }
inline const uuid RenderManSurfaceShader() { return uuid(0xb553a3bd, 0xd1044c6e, 0xb3f21e07, 0x87132c31); }
inline const uuid RenderManVolumeShader() { return uuid(0xfd07a255, 0x9bb84841, 0x87632abe, 0x3ceb9fc7); }
inline const uuid Position() { return uuid(0xe2edd795, 0x08f9416a, 0x84139876, 0x9c25a60a); }
inline const uuid Orientation() { return uuid(0xcf562348, 0x02624035, 0xae649a84, 0xf42e0b69); }
inline const uuid Scale() { return uuid(0x542de0d6, 0xe1e94e4a, 0x883eb23c, 0x3612fb8a); }
inline const uuid K3DScriptEngine() { return uuid(0x6598ab84, 0xfa0f48f2, 0xbc690f11, 0xe686e11b); }
inline const uuid FrozenTransformation() { return uuid(0x6ea85710, 0x90da4d35, 0xacec30ee, 0xa6aacb9d); }
inline const uuid Camera() { return uuid(0x45ce1872, 0xaf184b6d, 0xb391e136, 0x0dcfe8b5); }
inline const uuid DocumentWriter() { return uuid(0x0cbc9131, 0x172541b2, 0xb424afab, 0x25143d55); }
inline const uuid DocumentReader() { return uuid(0xf02e8dac, 0xa57d4d76, 0xa6170c3a, 0x9bf09d3b); }
inline const uuid AqsisDisplacementShaderLayer() { return uuid(0x1802dce3, 0x71004329, 0x80cbdf0e, 0xbc7f142c); }
inline const uuid AqsisSurfaceShaderLayer() { return uuid(0x261e244e, 0xd82947ba, 0x9e56a9af, 0x03d4cc0f); }
inline const uuid Material() { return uuid(0x00000001, 0x00000002, 0x00000003, 0x00000004); }
inline const uuid OpenGLMaterial() { return uuid(0x00000001, 0x00000002, 0x00000003, 0x00000005); }
//@}

/** \name Inactive class IDs for plugins that have been removed */
//@{
inline const uuid JavaScriptEngine() { return uuid(0x00000001, 0x00000000, 0x00000000, 0x00000048); }
inline const uuid Transform() { return uuid(0x00000001, 0x00000000, 0x00000000, 0x00000081); }
//@}

} // namespace classes

} // namespace k3d

#endif // !K3DSDK_CLASSES_H

