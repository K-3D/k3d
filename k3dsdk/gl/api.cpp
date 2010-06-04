/////////////////////////////////////////////////////////////////////////////////
//
// Generated file, do not edit by hand!
//
/////////////////////////////////////////////////////////////////////////////////

#include <k3dsdk/log.h>
#include <k3dsdk/gl/api.h>

namespace k3d
{

namespace gl
{

static void fallback_glAccum(api::GLenum op, api::GLfloat value)
{
	log() << warning << "Accum not available." << std::endl;
}

static void fallback_glActiveProgramEXT(api::GLuint program)
{
	log() << warning << "ActiveProgramEXT not available." << std::endl;
}

static void fallback_glActiveStencilFaceEXT(api::GLenum face)
{
	log() << warning << "ActiveStencilFaceEXT not available." << std::endl;
}

static void fallback_glActiveTexture(api::GLenum texture)
{
	log() << warning << "ActiveTexture not available." << std::endl;
}

static void fallback_glActiveTextureARB(api::GLenum texture)
{
	log() << warning << "ActiveTextureARB not available." << std::endl;
}

static void fallback_glActiveVaryingNV(api::GLuint program, const api::GLchar* name)
{
	log() << warning << "ActiveVaryingNV not available." << std::endl;
}

static void fallback_glAlphaFragmentOp1ATI(api::GLenum op, api::GLuint dst, api::GLuint dstMod, api::GLuint arg1, api::GLuint arg1Rep, api::GLuint arg1Mod)
{
	log() << warning << "AlphaFragmentOp1ATI not available." << std::endl;
}

static void fallback_glAlphaFragmentOp2ATI(api::GLenum op, api::GLuint dst, api::GLuint dstMod, api::GLuint arg1, api::GLuint arg1Rep, api::GLuint arg1Mod, api::GLuint arg2, api::GLuint arg2Rep, api::GLuint arg2Mod)
{
	log() << warning << "AlphaFragmentOp2ATI not available." << std::endl;
}

static void fallback_glAlphaFragmentOp3ATI(api::GLenum op, api::GLuint dst, api::GLuint dstMod, api::GLuint arg1, api::GLuint arg1Rep, api::GLuint arg1Mod, api::GLuint arg2, api::GLuint arg2Rep, api::GLuint arg2Mod, api::GLuint arg3, api::GLuint arg3Rep, api::GLuint arg3Mod)
{
	log() << warning << "AlphaFragmentOp3ATI not available." << std::endl;
}

static void fallback_glAlphaFunc(api::GLenum func, api::GLclampf ref)
{
	log() << warning << "AlphaFunc not available." << std::endl;
}

static void fallback_glApplyTextureEXT(api::GLenum mode)
{
	log() << warning << "ApplyTextureEXT not available." << std::endl;
}

static api::GLboolean fallback_glAreProgramsResidentNV(api::GLsizei n, const api::GLuint* programs, api::GLboolean* residences)
{
	log() << warning << "AreProgramsResidentNV not available." << std::endl;
}

static api::GLboolean fallback_glAreTexturesResident(api::GLsizei n, const api::GLuint* textures, api::GLboolean* residences)
{
	log() << warning << "AreTexturesResident not available." << std::endl;
}

static api::GLboolean fallback_glAreTexturesResidentEXT(api::GLsizei n, const api::GLuint* textures, api::GLboolean* residences)
{
	log() << warning << "AreTexturesResidentEXT not available." << std::endl;
}

static void fallback_glArrayElement(api::GLint i)
{
	log() << warning << "ArrayElement not available." << std::endl;
}

static void fallback_glArrayElementEXT(api::GLint i)
{
	log() << warning << "ArrayElementEXT not available." << std::endl;
}

static void fallback_glArrayObjectATI(api::GLenum array, api::GLint size, api::GLenum type, api::GLsizei stride, api::GLuint buffer, api::GLuint offset)
{
	log() << warning << "ArrayObjectATI not available." << std::endl;
}

static void fallback_glAsyncMarkerSGIX(api::GLuint marker)
{
	log() << warning << "AsyncMarkerSGIX not available." << std::endl;
}

static void fallback_glAttachObjectARB(api::GLhandleARB containerObj, api::GLhandleARB obj)
{
	log() << warning << "AttachObjectARB not available." << std::endl;
}

static void fallback_glAttachShader(api::GLuint program, api::GLuint shader)
{
	log() << warning << "AttachShader not available." << std::endl;
}

static void fallback_glBegin(api::GLenum mode)
{
	log() << warning << "Begin not available." << std::endl;
}

static void fallback_glBeginConditionalRender(api::GLuint id, api::GLenum mode)
{
	log() << warning << "BeginConditionalRender not available." << std::endl;
}

static void fallback_glBeginConditionalRenderNV(api::GLuint id, api::GLenum mode)
{
	log() << warning << "BeginConditionalRenderNV not available." << std::endl;
}

static void fallback_glBeginFragmentShaderATI()
{
	log() << warning << "BeginFragmentShaderATI not available." << std::endl;
}

static void fallback_glBeginOcclusionQueryNV(api::GLuint id)
{
	log() << warning << "BeginOcclusionQueryNV not available." << std::endl;
}

static void fallback_glBeginPerfMonitorAMD(api::GLuint monitor)
{
	log() << warning << "BeginPerfMonitorAMD not available." << std::endl;
}

static void fallback_glBeginQuery(api::GLenum target, api::GLuint id)
{
	log() << warning << "BeginQuery not available." << std::endl;
}

static void fallback_glBeginQueryARB(api::GLenum target, api::GLuint id)
{
	log() << warning << "BeginQueryARB not available." << std::endl;
}

static void fallback_glBeginQueryIndexed(api::GLenum target, api::GLuint index, api::GLuint id)
{
	log() << warning << "BeginQueryIndexed not available." << std::endl;
}

static void fallback_glBeginTransformFeedback(api::GLenum primitiveMode)
{
	log() << warning << "BeginTransformFeedback not available." << std::endl;
}

static void fallback_glBeginTransformFeedbackEXT(api::GLenum primitiveMode)
{
	log() << warning << "BeginTransformFeedbackEXT not available." << std::endl;
}

static void fallback_glBeginTransformFeedbackNV(api::GLenum primitiveMode)
{
	log() << warning << "BeginTransformFeedbackNV not available." << std::endl;
}

static void fallback_glBeginVertexShaderEXT()
{
	log() << warning << "BeginVertexShaderEXT not available." << std::endl;
}

static void fallback_glBeginVideoCaptureNV(api::GLuint video_capture_slot)
{
	log() << warning << "BeginVideoCaptureNV not available." << std::endl;
}

static void fallback_glBindAttribLocation(api::GLuint program, api::GLuint index, const api::GLchar* name)
{
	log() << warning << "BindAttribLocation not available." << std::endl;
}

static void fallback_glBindAttribLocationARB(api::GLhandleARB programObj, api::GLuint index, const api::GLcharARB* name)
{
	log() << warning << "BindAttribLocationARB not available." << std::endl;
}

static void fallback_glBindBuffer(api::GLenum target, api::GLuint buffer)
{
	log() << warning << "BindBuffer not available." << std::endl;
}

static void fallback_glBindBufferARB(api::GLenum target, api::GLuint buffer)
{
	log() << warning << "BindBufferARB not available." << std::endl;
}

static void fallback_glBindBufferBase(api::GLenum target, api::GLuint index, api::GLuint buffer)
{
	log() << warning << "BindBufferBase not available." << std::endl;
}

static void fallback_glBindBufferBaseEXT(api::GLenum target, api::GLuint index, api::GLuint buffer)
{
	log() << warning << "BindBufferBaseEXT not available." << std::endl;
}

static void fallback_glBindBufferBaseNV(api::GLenum target, api::GLuint index, api::GLuint buffer)
{
	log() << warning << "BindBufferBaseNV not available." << std::endl;
}

static void fallback_glBindBufferOffsetEXT(api::GLenum target, api::GLuint index, api::GLuint buffer, api::GLintptr offset)
{
	log() << warning << "BindBufferOffsetEXT not available." << std::endl;
}

static void fallback_glBindBufferOffsetNV(api::GLenum target, api::GLuint index, api::GLuint buffer, api::GLintptr offset)
{
	log() << warning << "BindBufferOffsetNV not available." << std::endl;
}

static void fallback_glBindBufferRange(api::GLenum target, api::GLuint index, api::GLuint buffer, api::GLintptr offset, api::GLsizeiptr size)
{
	log() << warning << "BindBufferRange not available." << std::endl;
}

static void fallback_glBindBufferRangeEXT(api::GLenum target, api::GLuint index, api::GLuint buffer, api::GLintptr offset, api::GLsizeiptr size)
{
	log() << warning << "BindBufferRangeEXT not available." << std::endl;
}

static void fallback_glBindBufferRangeNV(api::GLenum target, api::GLuint index, api::GLuint buffer, api::GLintptr offset, api::GLsizeiptr size)
{
	log() << warning << "BindBufferRangeNV not available." << std::endl;
}

static void fallback_glBindFragDataLocation(api::GLuint program, api::GLuint color, const api::GLchar* name)
{
	log() << warning << "BindFragDataLocation not available." << std::endl;
}

static void fallback_glBindFragDataLocationEXT(api::GLuint program, api::GLuint color, const api::GLchar* name)
{
	log() << warning << "BindFragDataLocationEXT not available." << std::endl;
}

static void fallback_glBindFragDataLocationIndexed(api::GLuint program, api::GLuint colorNumber, api::GLuint index, const api::GLchar* name)
{
	log() << warning << "BindFragDataLocationIndexed not available." << std::endl;
}

static void fallback_glBindFragmentShaderATI(api::GLuint id)
{
	log() << warning << "BindFragmentShaderATI not available." << std::endl;
}

static void fallback_glBindFramebuffer(api::GLenum target, api::GLuint framebuffer)
{
	log() << warning << "BindFramebuffer not available." << std::endl;
}

static void fallback_glBindFramebufferEXT(api::GLenum target, api::GLuint framebuffer)
{
	log() << warning << "BindFramebufferEXT not available." << std::endl;
}

static api::GLuint fallback_glBindLightParameterEXT(api::GLenum light, api::GLenum value)
{
	log() << warning << "BindLightParameterEXT not available." << std::endl;
}

static api::GLuint fallback_glBindMaterialParameterEXT(api::GLenum face, api::GLenum value)
{
	log() << warning << "BindMaterialParameterEXT not available." << std::endl;
}

static void fallback_glBindMultiTextureEXT(api::GLenum texunit, api::GLenum target, api::GLuint texture)
{
	log() << warning << "BindMultiTextureEXT not available." << std::endl;
}

static api::GLuint fallback_glBindParameterEXT(api::GLenum value)
{
	log() << warning << "BindParameterEXT not available." << std::endl;
}

static void fallback_glBindProgramARB(api::GLenum target, api::GLuint program)
{
	log() << warning << "BindProgramARB not available." << std::endl;
}

static void fallback_glBindProgramNV(api::GLenum target, api::GLuint id)
{
	log() << warning << "BindProgramNV not available." << std::endl;
}

static void fallback_glBindRenderbuffer(api::GLenum target, api::GLuint renderbuffer)
{
	log() << warning << "BindRenderbuffer not available." << std::endl;
}

static void fallback_glBindRenderbufferEXT(api::GLenum target, api::GLuint renderbuffer)
{
	log() << warning << "BindRenderbufferEXT not available." << std::endl;
}

static void fallback_glBindSampler(api::GLenum unit, api::GLuint sampler)
{
	log() << warning << "BindSampler not available." << std::endl;
}

static api::GLuint fallback_glBindTexGenParameterEXT(api::GLenum unit, api::GLenum coord, api::GLenum value)
{
	log() << warning << "BindTexGenParameterEXT not available." << std::endl;
}

static void fallback_glBindTexture(api::GLenum target, api::GLuint texture)
{
	log() << warning << "BindTexture not available." << std::endl;
}

static void fallback_glBindTextureEXT(api::GLenum target, api::GLuint texture)
{
	log() << warning << "BindTextureEXT not available." << std::endl;
}

static api::GLuint fallback_glBindTextureUnitParameterEXT(api::GLenum unit, api::GLenum value)
{
	log() << warning << "BindTextureUnitParameterEXT not available." << std::endl;
}

static void fallback_glBindTransformFeedback(api::GLenum target, api::GLuint id)
{
	log() << warning << "BindTransformFeedback not available." << std::endl;
}

static void fallback_glBindTransformFeedbackNV(api::GLenum target, api::GLuint id)
{
	log() << warning << "BindTransformFeedbackNV not available." << std::endl;
}

static void fallback_glBindVertexArray(api::GLuint array)
{
	log() << warning << "BindVertexArray not available." << std::endl;
}

static void fallback_glBindVertexArrayAPPLE(api::GLuint array)
{
	log() << warning << "BindVertexArrayAPPLE not available." << std::endl;
}

static void fallback_glBindVertexShaderEXT(api::GLuint id)
{
	log() << warning << "BindVertexShaderEXT not available." << std::endl;
}

static void fallback_glBindVideoCaptureStreamBufferNV(api::GLuint video_capture_slot, api::GLuint stream, api::GLenum frame_region, api::GLintptrARB offset)
{
	log() << warning << "BindVideoCaptureStreamBufferNV not available." << std::endl;
}

static void fallback_glBindVideoCaptureStreamTextureNV(api::GLuint video_capture_slot, api::GLuint stream, api::GLenum frame_region, api::GLenum target, api::GLuint texture)
{
	log() << warning << "BindVideoCaptureStreamTextureNV not available." << std::endl;
}

static void fallback_glBinormal3bEXT(api::GLbyte bx, api::GLbyte by, api::GLbyte bz)
{
	log() << warning << "Binormal3bEXT not available." << std::endl;
}

static void fallback_glBinormal3bvEXT(const api::GLbyte* v)
{
	log() << warning << "Binormal3bvEXT not available." << std::endl;
}

static void fallback_glBinormal3dEXT(api::GLdouble bx, api::GLdouble by, api::GLdouble bz)
{
	log() << warning << "Binormal3dEXT not available." << std::endl;
}

static void fallback_glBinormal3dvEXT(const api::GLdouble* v)
{
	log() << warning << "Binormal3dvEXT not available." << std::endl;
}

static void fallback_glBinormal3fEXT(api::GLfloat bx, api::GLfloat by, api::GLfloat bz)
{
	log() << warning << "Binormal3fEXT not available." << std::endl;
}

static void fallback_glBinormal3fvEXT(const api::GLfloat* v)
{
	log() << warning << "Binormal3fvEXT not available." << std::endl;
}

static void fallback_glBinormal3iEXT(api::GLint bx, api::GLint by, api::GLint bz)
{
	log() << warning << "Binormal3iEXT not available." << std::endl;
}

static void fallback_glBinormal3ivEXT(const api::GLint* v)
{
	log() << warning << "Binormal3ivEXT not available." << std::endl;
}

static void fallback_glBinormal3sEXT(api::GLshort bx, api::GLshort by, api::GLshort bz)
{
	log() << warning << "Binormal3sEXT not available." << std::endl;
}

static void fallback_glBinormal3svEXT(const api::GLshort* v)
{
	log() << warning << "Binormal3svEXT not available." << std::endl;
}

static void fallback_glBinormalPointerEXT(api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "BinormalPointerEXT not available." << std::endl;
}

static void fallback_glBitmap(api::GLsizei width, api::GLsizei height, api::GLfloat xorig, api::GLfloat yorig, api::GLfloat xmove, api::GLfloat ymove, const api::GLubyte* bitmap)
{
	log() << warning << "Bitmap not available." << std::endl;
}

static void fallback_glBlendColor(api::GLclampf red, api::GLclampf green, api::GLclampf blue, api::GLclampf alpha)
{
	log() << warning << "BlendColor not available." << std::endl;
}

static void fallback_glBlendColorEXT(api::GLclampf red, api::GLclampf green, api::GLclampf blue, api::GLclampf alpha)
{
	log() << warning << "BlendColorEXT not available." << std::endl;
}

static void fallback_glBlendEquation(api::GLenum mode)
{
	log() << warning << "BlendEquation not available." << std::endl;
}

static void fallback_glBlendEquationEXT(api::GLenum mode)
{
	log() << warning << "BlendEquationEXT not available." << std::endl;
}

static void fallback_glBlendEquationIndexedAMD(api::GLuint buf, api::GLenum mode)
{
	log() << warning << "BlendEquationIndexedAMD not available." << std::endl;
}

static void fallback_glBlendEquationSeparate(api::GLenum modeRGB, api::GLenum modeAlpha)
{
	log() << warning << "BlendEquationSeparate not available." << std::endl;
}

static void fallback_glBlendEquationSeparateEXT(api::GLenum modeRGB, api::GLenum modeAlpha)
{
	log() << warning << "BlendEquationSeparateEXT not available." << std::endl;
}

static void fallback_glBlendEquationSeparateIndexedAMD(api::GLuint buf, api::GLenum modeRGB, api::GLenum modeAlpha)
{
	log() << warning << "BlendEquationSeparateIndexedAMD not available." << std::endl;
}

static void fallback_glBlendEquationSeparatei(api::GLuint buf, api::GLenum modeRGB, api::GLenum modeAlpha)
{
	log() << warning << "BlendEquationSeparatei not available." << std::endl;
}

static void fallback_glBlendEquationi(api::GLuint buf, api::GLenum mode)
{
	log() << warning << "BlendEquationi not available." << std::endl;
}

static void fallback_glBlendFunc(api::GLenum sfactor, api::GLenum dfactor)
{
	log() << warning << "BlendFunc not available." << std::endl;
}

static void fallback_glBlendFuncIndexedAMD(api::GLuint buf, api::GLenum src, api::GLenum dst)
{
	log() << warning << "BlendFuncIndexedAMD not available." << std::endl;
}

static void fallback_glBlendFuncSeparate(api::GLenum sfactorRGB, api::GLenum dfactorRGB, api::GLenum sfactorAlpha, api::GLenum dfactorAlpha)
{
	log() << warning << "BlendFuncSeparate not available." << std::endl;
}

static void fallback_glBlendFuncSeparateEXT(api::GLenum sfactorRGB, api::GLenum dfactorRGB, api::GLenum sfactorAlpha, api::GLenum dfactorAlpha)
{
	log() << warning << "BlendFuncSeparateEXT not available." << std::endl;
}

static void fallback_glBlendFuncSeparateINGR(api::GLenum sfactorRGB, api::GLenum dfactorRGB, api::GLenum sfactorAlpha, api::GLenum dfactorAlpha)
{
	log() << warning << "BlendFuncSeparateINGR not available." << std::endl;
}

static void fallback_glBlendFuncSeparateIndexedAMD(api::GLuint buf, api::GLenum srcRGB, api::GLenum dstRGB, api::GLenum srcAlpha, api::GLenum dstAlpha)
{
	log() << warning << "BlendFuncSeparateIndexedAMD not available." << std::endl;
}

static void fallback_glBlendFuncSeparatei(api::GLuint buf, api::GLenum srcRGB, api::GLenum dstRGB, api::GLenum srcAlpha, api::GLenum dstAlpha)
{
	log() << warning << "BlendFuncSeparatei not available." << std::endl;
}

static void fallback_glBlendFunci(api::GLuint buf, api::GLenum src, api::GLenum dst)
{
	log() << warning << "BlendFunci not available." << std::endl;
}

static void fallback_glBlitFramebuffer(api::GLint srcX0, api::GLint srcY0, api::GLint srcX1, api::GLint srcY1, api::GLint dstX0, api::GLint dstY0, api::GLint dstX1, api::GLint dstY1, api::GLbitfield mask, api::GLenum filter)
{
	log() << warning << "BlitFramebuffer not available." << std::endl;
}

static void fallback_glBlitFramebufferEXT(api::GLint srcX0, api::GLint srcY0, api::GLint srcX1, api::GLint srcY1, api::GLint dstX0, api::GLint dstY0, api::GLint dstX1, api::GLint dstY1, api::GLbitfield mask, api::GLenum filter)
{
	log() << warning << "BlitFramebufferEXT not available." << std::endl;
}

static void fallback_glBufferAddressRangeNV(api::GLenum pname, api::GLuint index, api::GLuint64EXT address, api::GLsizeiptr length)
{
	log() << warning << "BufferAddressRangeNV not available." << std::endl;
}

static void fallback_glBufferData(api::GLenum target, api::GLsizeiptr size, const api::GLvoid* data, api::GLenum usage)
{
	log() << warning << "BufferData not available." << std::endl;
}

static void fallback_glBufferDataARB(api::GLenum target, api::GLsizeiptrARB size, const api::GLvoid* data, api::GLenum usage)
{
	log() << warning << "BufferDataARB not available." << std::endl;
}

static void fallback_glBufferParameteriAPPLE(api::GLenum target, api::GLenum pname, api::GLint param)
{
	log() << warning << "BufferParameteriAPPLE not available." << std::endl;
}

static void fallback_glBufferSubData(api::GLenum target, api::GLintptr offset, api::GLsizeiptr size, const api::GLvoid* data)
{
	log() << warning << "BufferSubData not available." << std::endl;
}

static void fallback_glBufferSubDataARB(api::GLenum target, api::GLintptrARB offset, api::GLsizeiptrARB size, const api::GLvoid* data)
{
	log() << warning << "BufferSubDataARB not available." << std::endl;
}

static void fallback_glCallList(api::GLuint list)
{
	log() << warning << "CallList not available." << std::endl;
}

static void fallback_glCallLists(api::GLsizei n, api::GLenum type, const api::GLvoid* lists)
{
	log() << warning << "CallLists not available." << std::endl;
}

static api::GLenum fallback_glCheckFramebufferStatus(api::GLenum target)
{
	log() << warning << "CheckFramebufferStatus not available." << std::endl;
}

static api::GLenum fallback_glCheckFramebufferStatusEXT(api::GLenum target)
{
	log() << warning << "CheckFramebufferStatusEXT not available." << std::endl;
}

static api::GLenum fallback_glCheckNamedFramebufferStatusEXT(api::GLuint framebuffer, api::GLenum target)
{
	log() << warning << "CheckNamedFramebufferStatusEXT not available." << std::endl;
}

static void fallback_glClampColor(api::GLenum target, api::GLenum clamp)
{
	log() << warning << "ClampColor not available." << std::endl;
}

static void fallback_glClampColorARB(api::GLenum target, api::GLenum clamp)
{
	log() << warning << "ClampColorARB not available." << std::endl;
}

static void fallback_glClear(api::GLbitfield mask)
{
	log() << warning << "Clear not available." << std::endl;
}

static void fallback_glClearAccum(api::GLfloat red, api::GLfloat green, api::GLfloat blue, api::GLfloat alpha)
{
	log() << warning << "ClearAccum not available." << std::endl;
}

static void fallback_glClearBufferfi(api::GLenum buffer, api::GLint drawbuffer, api::GLfloat depth, api::GLint stencil)
{
	log() << warning << "ClearBufferfi not available." << std::endl;
}

static void fallback_glClearBufferfv(api::GLenum buffer, api::GLint drawbuffer, const api::GLfloat* value)
{
	log() << warning << "ClearBufferfv not available." << std::endl;
}

static void fallback_glClearBufferiv(api::GLenum buffer, api::GLint drawbuffer, const api::GLint* value)
{
	log() << warning << "ClearBufferiv not available." << std::endl;
}

static void fallback_glClearBufferuiv(api::GLenum buffer, api::GLint drawbuffer, const api::GLuint* value)
{
	log() << warning << "ClearBufferuiv not available." << std::endl;
}

static void fallback_glClearColor(api::GLclampf red, api::GLclampf green, api::GLclampf blue, api::GLclampf alpha)
{
	log() << warning << "ClearColor not available." << std::endl;
}

static void fallback_glClearColorIiEXT(api::GLint red, api::GLint green, api::GLint blue, api::GLint alpha)
{
	log() << warning << "ClearColorIiEXT not available." << std::endl;
}

static void fallback_glClearColorIuiEXT(api::GLuint red, api::GLuint green, api::GLuint blue, api::GLuint alpha)
{
	log() << warning << "ClearColorIuiEXT not available." << std::endl;
}

static void fallback_glClearDepth(api::GLclampd depth)
{
	log() << warning << "ClearDepth not available." << std::endl;
}

static void fallback_glClearDepthdNV(api::GLdouble depth)
{
	log() << warning << "ClearDepthdNV not available." << std::endl;
}

static void fallback_glClearIndex(api::GLfloat c)
{
	log() << warning << "ClearIndex not available." << std::endl;
}

static void fallback_glClearStencil(api::GLint s)
{
	log() << warning << "ClearStencil not available." << std::endl;
}

static void fallback_glClientActiveTexture(api::GLenum texture)
{
	log() << warning << "ClientActiveTexture not available." << std::endl;
}

static void fallback_glClientActiveTextureARB(api::GLenum texture)
{
	log() << warning << "ClientActiveTextureARB not available." << std::endl;
}

static void fallback_glClientActiveVertexStreamATI(api::GLenum stream)
{
	log() << warning << "ClientActiveVertexStreamATI not available." << std::endl;
}

static void fallback_glClientAttribDefaultEXT(api::GLbitfield mask)
{
	log() << warning << "ClientAttribDefaultEXT not available." << std::endl;
}

static api::GLenum fallback_glClientWaitSync(api::GLsync sync, api::GLbitfield flags, api::GLuint64 timeout)
{
	log() << warning << "ClientWaitSync not available." << std::endl;
}

static void fallback_glClipPlane(api::GLenum plane, const api::GLdouble* equation)
{
	log() << warning << "ClipPlane not available." << std::endl;
}

static void fallback_glColor3b(api::GLbyte red, api::GLbyte green, api::GLbyte blue)
{
	log() << warning << "Color3b not available." << std::endl;
}

static void fallback_glColor3bv(const api::GLbyte* v)
{
	log() << warning << "Color3bv not available." << std::endl;
}

static void fallback_glColor3d(api::GLdouble red, api::GLdouble green, api::GLdouble blue)
{
	log() << warning << "Color3d not available." << std::endl;
}

static void fallback_glColor3dv(const api::GLdouble* v)
{
	log() << warning << "Color3dv not available." << std::endl;
}

static void fallback_glColor3f(api::GLfloat red, api::GLfloat green, api::GLfloat blue)
{
	log() << warning << "Color3f not available." << std::endl;
}

static void fallback_glColor3fVertex3fSUN(api::GLfloat r, api::GLfloat g, api::GLfloat b, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "Color3fVertex3fSUN not available." << std::endl;
}

static void fallback_glColor3fVertex3fvSUN(const api::GLfloat* c, const api::GLfloat* v)
{
	log() << warning << "Color3fVertex3fvSUN not available." << std::endl;
}

static void fallback_glColor3fv(const api::GLfloat* v)
{
	log() << warning << "Color3fv not available." << std::endl;
}

static void fallback_glColor3hNV(api::GLhalfNV red, api::GLhalfNV green, api::GLhalfNV blue)
{
	log() << warning << "Color3hNV not available." << std::endl;
}

static void fallback_glColor3hvNV(const api::GLhalfNV* v)
{
	log() << warning << "Color3hvNV not available." << std::endl;
}

static void fallback_glColor3i(api::GLint red, api::GLint green, api::GLint blue)
{
	log() << warning << "Color3i not available." << std::endl;
}

static void fallback_glColor3iv(const api::GLint* v)
{
	log() << warning << "Color3iv not available." << std::endl;
}

static void fallback_glColor3s(api::GLshort red, api::GLshort green, api::GLshort blue)
{
	log() << warning << "Color3s not available." << std::endl;
}

static void fallback_glColor3sv(const api::GLshort* v)
{
	log() << warning << "Color3sv not available." << std::endl;
}

static void fallback_glColor3ub(api::GLubyte red, api::GLubyte green, api::GLubyte blue)
{
	log() << warning << "Color3ub not available." << std::endl;
}

static void fallback_glColor3ubv(const api::GLubyte* v)
{
	log() << warning << "Color3ubv not available." << std::endl;
}

static void fallback_glColor3ui(api::GLuint red, api::GLuint green, api::GLuint blue)
{
	log() << warning << "Color3ui not available." << std::endl;
}

static void fallback_glColor3uiv(const api::GLuint* v)
{
	log() << warning << "Color3uiv not available." << std::endl;
}

static void fallback_glColor3us(api::GLushort red, api::GLushort green, api::GLushort blue)
{
	log() << warning << "Color3us not available." << std::endl;
}

static void fallback_glColor3usv(const api::GLushort* v)
{
	log() << warning << "Color3usv not available." << std::endl;
}

static void fallback_glColor4b(api::GLbyte red, api::GLbyte green, api::GLbyte blue, api::GLbyte alpha)
{
	log() << warning << "Color4b not available." << std::endl;
}

static void fallback_glColor4bv(const api::GLbyte* v)
{
	log() << warning << "Color4bv not available." << std::endl;
}

static void fallback_glColor4d(api::GLdouble red, api::GLdouble green, api::GLdouble blue, api::GLdouble alpha)
{
	log() << warning << "Color4d not available." << std::endl;
}

static void fallback_glColor4dv(const api::GLdouble* v)
{
	log() << warning << "Color4dv not available." << std::endl;
}

static void fallback_glColor4f(api::GLfloat red, api::GLfloat green, api::GLfloat blue, api::GLfloat alpha)
{
	log() << warning << "Color4f not available." << std::endl;
}

static void fallback_glColor4fNormal3fVertex3fSUN(api::GLfloat r, api::GLfloat g, api::GLfloat b, api::GLfloat a, api::GLfloat nx, api::GLfloat ny, api::GLfloat nz, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "Color4fNormal3fVertex3fSUN not available." << std::endl;
}

static void fallback_glColor4fNormal3fVertex3fvSUN(const api::GLfloat* c, const api::GLfloat* n, const api::GLfloat* v)
{
	log() << warning << "Color4fNormal3fVertex3fvSUN not available." << std::endl;
}

static void fallback_glColor4fv(const api::GLfloat* v)
{
	log() << warning << "Color4fv not available." << std::endl;
}

static void fallback_glColor4hNV(api::GLhalfNV red, api::GLhalfNV green, api::GLhalfNV blue, api::GLhalfNV alpha)
{
	log() << warning << "Color4hNV not available." << std::endl;
}

static void fallback_glColor4hvNV(const api::GLhalfNV* v)
{
	log() << warning << "Color4hvNV not available." << std::endl;
}

static void fallback_glColor4i(api::GLint red, api::GLint green, api::GLint blue, api::GLint alpha)
{
	log() << warning << "Color4i not available." << std::endl;
}

static void fallback_glColor4iv(const api::GLint* v)
{
	log() << warning << "Color4iv not available." << std::endl;
}

static void fallback_glColor4s(api::GLshort red, api::GLshort green, api::GLshort blue, api::GLshort alpha)
{
	log() << warning << "Color4s not available." << std::endl;
}

static void fallback_glColor4sv(const api::GLshort* v)
{
	log() << warning << "Color4sv not available." << std::endl;
}

static void fallback_glColor4ub(api::GLubyte red, api::GLubyte green, api::GLubyte blue, api::GLubyte alpha)
{
	log() << warning << "Color4ub not available." << std::endl;
}

static void fallback_glColor4ubVertex2fSUN(api::GLubyte r, api::GLubyte g, api::GLubyte b, api::GLubyte a, api::GLfloat x, api::GLfloat y)
{
	log() << warning << "Color4ubVertex2fSUN not available." << std::endl;
}

static void fallback_glColor4ubVertex2fvSUN(const api::GLubyte* c, const api::GLfloat* v)
{
	log() << warning << "Color4ubVertex2fvSUN not available." << std::endl;
}

static void fallback_glColor4ubVertex3fSUN(api::GLubyte r, api::GLubyte g, api::GLubyte b, api::GLubyte a, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "Color4ubVertex3fSUN not available." << std::endl;
}

static void fallback_glColor4ubVertex3fvSUN(const api::GLubyte* c, const api::GLfloat* v)
{
	log() << warning << "Color4ubVertex3fvSUN not available." << std::endl;
}

static void fallback_glColor4ubv(const api::GLubyte* v)
{
	log() << warning << "Color4ubv not available." << std::endl;
}

static void fallback_glColor4ui(api::GLuint red, api::GLuint green, api::GLuint blue, api::GLuint alpha)
{
	log() << warning << "Color4ui not available." << std::endl;
}

static void fallback_glColor4uiv(const api::GLuint* v)
{
	log() << warning << "Color4uiv not available." << std::endl;
}

static void fallback_glColor4us(api::GLushort red, api::GLushort green, api::GLushort blue, api::GLushort alpha)
{
	log() << warning << "Color4us not available." << std::endl;
}

static void fallback_glColor4usv(const api::GLushort* v)
{
	log() << warning << "Color4usv not available." << std::endl;
}

static void fallback_glColorFormatNV(api::GLint size, api::GLenum type, api::GLsizei stride)
{
	log() << warning << "ColorFormatNV not available." << std::endl;
}

static void fallback_glColorFragmentOp1ATI(api::GLenum op, api::GLuint dst, api::GLuint dstMask, api::GLuint dstMod, api::GLuint arg1, api::GLuint arg1Rep, api::GLuint arg1Mod)
{
	log() << warning << "ColorFragmentOp1ATI not available." << std::endl;
}

static void fallback_glColorFragmentOp2ATI(api::GLenum op, api::GLuint dst, api::GLuint dstMask, api::GLuint dstMod, api::GLuint arg1, api::GLuint arg1Rep, api::GLuint arg1Mod, api::GLuint arg2, api::GLuint arg2Rep, api::GLuint arg2Mod)
{
	log() << warning << "ColorFragmentOp2ATI not available." << std::endl;
}

static void fallback_glColorFragmentOp3ATI(api::GLenum op, api::GLuint dst, api::GLuint dstMask, api::GLuint dstMod, api::GLuint arg1, api::GLuint arg1Rep, api::GLuint arg1Mod, api::GLuint arg2, api::GLuint arg2Rep, api::GLuint arg2Mod, api::GLuint arg3, api::GLuint arg3Rep, api::GLuint arg3Mod)
{
	log() << warning << "ColorFragmentOp3ATI not available." << std::endl;
}

static void fallback_glColorMask(api::GLboolean red, api::GLboolean green, api::GLboolean blue, api::GLboolean alpha)
{
	log() << warning << "ColorMask not available." << std::endl;
}

static void fallback_glColorMaskIndexedEXT(api::GLuint index, api::GLboolean r, api::GLboolean g, api::GLboolean b, api::GLboolean a)
{
	log() << warning << "ColorMaskIndexedEXT not available." << std::endl;
}

static void fallback_glColorMaski(api::GLuint index, api::GLboolean r, api::GLboolean g, api::GLboolean b, api::GLboolean a)
{
	log() << warning << "ColorMaski not available." << std::endl;
}

static void fallback_glColorMaterial(api::GLenum face, api::GLenum mode)
{
	log() << warning << "ColorMaterial not available." << std::endl;
}

static void fallback_glColorP3ui(api::GLenum type, api::GLuint color)
{
	log() << warning << "ColorP3ui not available." << std::endl;
}

static void fallback_glColorP3uiv(api::GLenum type, const api::GLuint* color)
{
	log() << warning << "ColorP3uiv not available." << std::endl;
}

static void fallback_glColorP4ui(api::GLenum type, api::GLuint color)
{
	log() << warning << "ColorP4ui not available." << std::endl;
}

static void fallback_glColorP4uiv(api::GLenum type, const api::GLuint* color)
{
	log() << warning << "ColorP4uiv not available." << std::endl;
}

static void fallback_glColorPointer(api::GLint size, api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "ColorPointer not available." << std::endl;
}

static void fallback_glColorPointerEXT(api::GLint size, api::GLenum type, api::GLsizei stride, api::GLsizei count, const api::GLvoid* pointer)
{
	log() << warning << "ColorPointerEXT not available." << std::endl;
}

static void fallback_glColorPointerListIBM(api::GLint size, api::GLenum type, api::GLint stride, const api::GLvoid** pointer, api::GLint ptrstride)
{
	log() << warning << "ColorPointerListIBM not available." << std::endl;
}

static void fallback_glColorPointervINTEL(api::GLint size, api::GLenum type, const api::GLvoid** pointer)
{
	log() << warning << "ColorPointervINTEL not available." << std::endl;
}

static void fallback_glColorSubTable(api::GLenum target, api::GLsizei start, api::GLsizei count, api::GLenum format, api::GLenum type, const api::GLvoid* data)
{
	log() << warning << "ColorSubTable not available." << std::endl;
}

static void fallback_glColorSubTableEXT(api::GLenum target, api::GLsizei start, api::GLsizei count, api::GLenum format, api::GLenum type, const api::GLvoid* data)
{
	log() << warning << "ColorSubTableEXT not available." << std::endl;
}

static void fallback_glColorTable(api::GLenum target, api::GLenum internalformat, api::GLsizei width, api::GLenum format, api::GLenum type, const api::GLvoid* table)
{
	log() << warning << "ColorTable not available." << std::endl;
}

static void fallback_glColorTableEXT(api::GLenum target, api::GLenum internalFormat, api::GLsizei width, api::GLenum format, api::GLenum type, const api::GLvoid* table)
{
	log() << warning << "ColorTableEXT not available." << std::endl;
}

static void fallback_glColorTableParameterfv(api::GLenum target, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "ColorTableParameterfv not available." << std::endl;
}

static void fallback_glColorTableParameterfvSGI(api::GLenum target, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "ColorTableParameterfvSGI not available." << std::endl;
}

static void fallback_glColorTableParameteriv(api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "ColorTableParameteriv not available." << std::endl;
}

static void fallback_glColorTableParameterivSGI(api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "ColorTableParameterivSGI not available." << std::endl;
}

static void fallback_glColorTableSGI(api::GLenum target, api::GLenum internalformat, api::GLsizei width, api::GLenum format, api::GLenum type, const api::GLvoid* table)
{
	log() << warning << "ColorTableSGI not available." << std::endl;
}

static void fallback_glCombinerInputNV(api::GLenum stage, api::GLenum portion, api::GLenum variable, api::GLenum input, api::GLenum mapping, api::GLenum componentUsage)
{
	log() << warning << "CombinerInputNV not available." << std::endl;
}

static void fallback_glCombinerOutputNV(api::GLenum stage, api::GLenum portion, api::GLenum abOutput, api::GLenum cdOutput, api::GLenum sumOutput, api::GLenum scale, api::GLenum bias, api::GLboolean abDotProduct, api::GLboolean cdDotProduct, api::GLboolean muxSum)
{
	log() << warning << "CombinerOutputNV not available." << std::endl;
}

static void fallback_glCombinerParameterfNV(api::GLenum pname, api::GLfloat param)
{
	log() << warning << "CombinerParameterfNV not available." << std::endl;
}

static void fallback_glCombinerParameterfvNV(api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "CombinerParameterfvNV not available." << std::endl;
}

static void fallback_glCombinerParameteriNV(api::GLenum pname, api::GLint param)
{
	log() << warning << "CombinerParameteriNV not available." << std::endl;
}

static void fallback_glCombinerParameterivNV(api::GLenum pname, const api::GLint* params)
{
	log() << warning << "CombinerParameterivNV not available." << std::endl;
}

static void fallback_glCombinerStageParameterfvNV(api::GLenum stage, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "CombinerStageParameterfvNV not available." << std::endl;
}

static void fallback_glCompileShader(api::GLuint shader)
{
	log() << warning << "CompileShader not available." << std::endl;
}

static void fallback_glCompileShaderARB(api::GLhandleARB shaderObj)
{
	log() << warning << "CompileShaderARB not available." << std::endl;
}

static void fallback_glCompileShaderIncludeARB(api::GLuint shader, api::GLsizei count, const api::GLchar** path, const api::GLint* length)
{
	log() << warning << "CompileShaderIncludeARB not available." << std::endl;
}

static void fallback_glCompressedMultiTexImage1DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLint border, api::GLsizei imageSize, const api::GLvoid* bits)
{
	log() << warning << "CompressedMultiTexImage1DEXT not available." << std::endl;
}

static void fallback_glCompressedMultiTexImage2DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLint border, api::GLsizei imageSize, const api::GLvoid* bits)
{
	log() << warning << "CompressedMultiTexImage2DEXT not available." << std::endl;
}

static void fallback_glCompressedMultiTexImage3DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLint border, api::GLsizei imageSize, const api::GLvoid* bits)
{
	log() << warning << "CompressedMultiTexImage3DEXT not available." << std::endl;
}

static void fallback_glCompressedMultiTexSubImage1DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLsizei width, api::GLenum format, api::GLsizei imageSize, const api::GLvoid* bits)
{
	log() << warning << "CompressedMultiTexSubImage1DEXT not available." << std::endl;
}

static void fallback_glCompressedMultiTexSubImage2DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLsizei width, api::GLsizei height, api::GLenum format, api::GLsizei imageSize, const api::GLvoid* bits)
{
	log() << warning << "CompressedMultiTexSubImage2DEXT not available." << std::endl;
}

static void fallback_glCompressedMultiTexSubImage3DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint zoffset, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLenum format, api::GLsizei imageSize, const api::GLvoid* bits)
{
	log() << warning << "CompressedMultiTexSubImage3DEXT not available." << std::endl;
}

static void fallback_glCompressedTexImage1D(api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLint border, api::GLsizei imageSize, const api::GLvoid* data)
{
	log() << warning << "CompressedTexImage1D not available." << std::endl;
}

static void fallback_glCompressedTexImage1DARB(api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLint border, api::GLsizei imageSize, const api::GLvoid* data)
{
	log() << warning << "CompressedTexImage1DARB not available." << std::endl;
}

static void fallback_glCompressedTexImage2D(api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLint border, api::GLsizei imageSize, const api::GLvoid* data)
{
	log() << warning << "CompressedTexImage2D not available." << std::endl;
}

static void fallback_glCompressedTexImage2DARB(api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLint border, api::GLsizei imageSize, const api::GLvoid* data)
{
	log() << warning << "CompressedTexImage2DARB not available." << std::endl;
}

static void fallback_glCompressedTexImage3D(api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLint border, api::GLsizei imageSize, const api::GLvoid* data)
{
	log() << warning << "CompressedTexImage3D not available." << std::endl;
}

static void fallback_glCompressedTexImage3DARB(api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLint border, api::GLsizei imageSize, const api::GLvoid* data)
{
	log() << warning << "CompressedTexImage3DARB not available." << std::endl;
}

static void fallback_glCompressedTexSubImage1D(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLsizei width, api::GLenum format, api::GLsizei imageSize, const api::GLvoid* data)
{
	log() << warning << "CompressedTexSubImage1D not available." << std::endl;
}

static void fallback_glCompressedTexSubImage1DARB(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLsizei width, api::GLenum format, api::GLsizei imageSize, const api::GLvoid* data)
{
	log() << warning << "CompressedTexSubImage1DARB not available." << std::endl;
}

static void fallback_glCompressedTexSubImage2D(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLsizei width, api::GLsizei height, api::GLenum format, api::GLsizei imageSize, const api::GLvoid* data)
{
	log() << warning << "CompressedTexSubImage2D not available." << std::endl;
}

static void fallback_glCompressedTexSubImage2DARB(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLsizei width, api::GLsizei height, api::GLenum format, api::GLsizei imageSize, const api::GLvoid* data)
{
	log() << warning << "CompressedTexSubImage2DARB not available." << std::endl;
}

static void fallback_glCompressedTexSubImage3D(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint zoffset, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLenum format, api::GLsizei imageSize, const api::GLvoid* data)
{
	log() << warning << "CompressedTexSubImage3D not available." << std::endl;
}

static void fallback_glCompressedTexSubImage3DARB(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint zoffset, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLenum format, api::GLsizei imageSize, const api::GLvoid* data)
{
	log() << warning << "CompressedTexSubImage3DARB not available." << std::endl;
}

static void fallback_glCompressedTextureImage1DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLint border, api::GLsizei imageSize, const api::GLvoid* bits)
{
	log() << warning << "CompressedTextureImage1DEXT not available." << std::endl;
}

static void fallback_glCompressedTextureImage2DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLint border, api::GLsizei imageSize, const api::GLvoid* bits)
{
	log() << warning << "CompressedTextureImage2DEXT not available." << std::endl;
}

static void fallback_glCompressedTextureImage3DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLint border, api::GLsizei imageSize, const api::GLvoid* bits)
{
	log() << warning << "CompressedTextureImage3DEXT not available." << std::endl;
}

static void fallback_glCompressedTextureSubImage1DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLsizei width, api::GLenum format, api::GLsizei imageSize, const api::GLvoid* bits)
{
	log() << warning << "CompressedTextureSubImage1DEXT not available." << std::endl;
}

static void fallback_glCompressedTextureSubImage2DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLsizei width, api::GLsizei height, api::GLenum format, api::GLsizei imageSize, const api::GLvoid* bits)
{
	log() << warning << "CompressedTextureSubImage2DEXT not available." << std::endl;
}

static void fallback_glCompressedTextureSubImage3DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint zoffset, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLenum format, api::GLsizei imageSize, const api::GLvoid* bits)
{
	log() << warning << "CompressedTextureSubImage3DEXT not available." << std::endl;
}

static void fallback_glConvolutionFilter1D(api::GLenum target, api::GLenum internalformat, api::GLsizei width, api::GLenum format, api::GLenum type, const api::GLvoid* image)
{
	log() << warning << "ConvolutionFilter1D not available." << std::endl;
}

static void fallback_glConvolutionFilter1DEXT(api::GLenum target, api::GLenum internalformat, api::GLsizei width, api::GLenum format, api::GLenum type, const api::GLvoid* image)
{
	log() << warning << "ConvolutionFilter1DEXT not available." << std::endl;
}

static void fallback_glConvolutionFilter2D(api::GLenum target, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLenum format, api::GLenum type, const api::GLvoid* image)
{
	log() << warning << "ConvolutionFilter2D not available." << std::endl;
}

static void fallback_glConvolutionFilter2DEXT(api::GLenum target, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLenum format, api::GLenum type, const api::GLvoid* image)
{
	log() << warning << "ConvolutionFilter2DEXT not available." << std::endl;
}

static void fallback_glConvolutionParameterf(api::GLenum target, api::GLenum pname, api::GLfloat params)
{
	log() << warning << "ConvolutionParameterf not available." << std::endl;
}

static void fallback_glConvolutionParameterfEXT(api::GLenum target, api::GLenum pname, api::GLfloat params)
{
	log() << warning << "ConvolutionParameterfEXT not available." << std::endl;
}

static void fallback_glConvolutionParameterfv(api::GLenum target, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "ConvolutionParameterfv not available." << std::endl;
}

static void fallback_glConvolutionParameterfvEXT(api::GLenum target, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "ConvolutionParameterfvEXT not available." << std::endl;
}

static void fallback_glConvolutionParameteri(api::GLenum target, api::GLenum pname, api::GLint params)
{
	log() << warning << "ConvolutionParameteri not available." << std::endl;
}

static void fallback_glConvolutionParameteriEXT(api::GLenum target, api::GLenum pname, api::GLint params)
{
	log() << warning << "ConvolutionParameteriEXT not available." << std::endl;
}

static void fallback_glConvolutionParameteriv(api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "ConvolutionParameteriv not available." << std::endl;
}

static void fallback_glConvolutionParameterivEXT(api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "ConvolutionParameterivEXT not available." << std::endl;
}

static void fallback_glCopyBufferSubData(api::GLenum readTarget, api::GLenum writeTarget, api::GLintptr readOffset, api::GLintptr writeOffset, api::GLsizeiptr size)
{
	log() << warning << "CopyBufferSubData not available." << std::endl;
}

static void fallback_glCopyColorSubTable(api::GLenum target, api::GLsizei start, api::GLint x, api::GLint y, api::GLsizei width)
{
	log() << warning << "CopyColorSubTable not available." << std::endl;
}

static void fallback_glCopyColorSubTableEXT(api::GLenum target, api::GLsizei start, api::GLint x, api::GLint y, api::GLsizei width)
{
	log() << warning << "CopyColorSubTableEXT not available." << std::endl;
}

static void fallback_glCopyColorTable(api::GLenum target, api::GLenum internalformat, api::GLint x, api::GLint y, api::GLsizei width)
{
	log() << warning << "CopyColorTable not available." << std::endl;
}

static void fallback_glCopyColorTableSGI(api::GLenum target, api::GLenum internalformat, api::GLint x, api::GLint y, api::GLsizei width)
{
	log() << warning << "CopyColorTableSGI not available." << std::endl;
}

static void fallback_glCopyConvolutionFilter1D(api::GLenum target, api::GLenum internalformat, api::GLint x, api::GLint y, api::GLsizei width)
{
	log() << warning << "CopyConvolutionFilter1D not available." << std::endl;
}

static void fallback_glCopyConvolutionFilter1DEXT(api::GLenum target, api::GLenum internalformat, api::GLint x, api::GLint y, api::GLsizei width)
{
	log() << warning << "CopyConvolutionFilter1DEXT not available." << std::endl;
}

static void fallback_glCopyConvolutionFilter2D(api::GLenum target, api::GLenum internalformat, api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "CopyConvolutionFilter2D not available." << std::endl;
}

static void fallback_glCopyConvolutionFilter2DEXT(api::GLenum target, api::GLenum internalformat, api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "CopyConvolutionFilter2DEXT not available." << std::endl;
}

static void fallback_glCopyImageSubDataNV(api::GLuint srcName, api::GLenum srcTarget, api::GLint srcLevel, api::GLint srcX, api::GLint srcY, api::GLint srcZ, api::GLuint dstName, api::GLenum dstTarget, api::GLint dstLevel, api::GLint dstX, api::GLint dstY, api::GLint dstZ, api::GLsizei width, api::GLsizei height, api::GLsizei depth)
{
	log() << warning << "CopyImageSubDataNV not available." << std::endl;
}

static void fallback_glCopyMultiTexImage1DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLint x, api::GLint y, api::GLsizei width, api::GLint border)
{
	log() << warning << "CopyMultiTexImage1DEXT not available." << std::endl;
}

static void fallback_glCopyMultiTexImage2DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height, api::GLint border)
{
	log() << warning << "CopyMultiTexImage2DEXT not available." << std::endl;
}

static void fallback_glCopyMultiTexSubImage1DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint x, api::GLint y, api::GLsizei width)
{
	log() << warning << "CopyMultiTexSubImage1DEXT not available." << std::endl;
}

static void fallback_glCopyMultiTexSubImage2DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "CopyMultiTexSubImage2DEXT not available." << std::endl;
}

static void fallback_glCopyMultiTexSubImage3DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint zoffset, api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "CopyMultiTexSubImage3DEXT not available." << std::endl;
}

static void fallback_glCopyPixels(api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height, api::GLenum type)
{
	log() << warning << "CopyPixels not available." << std::endl;
}

static void fallback_glCopyTexImage1D(api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLint x, api::GLint y, api::GLsizei width, api::GLint border)
{
	log() << warning << "CopyTexImage1D not available." << std::endl;
}

static void fallback_glCopyTexImage1DEXT(api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLint x, api::GLint y, api::GLsizei width, api::GLint border)
{
	log() << warning << "CopyTexImage1DEXT not available." << std::endl;
}

static void fallback_glCopyTexImage2D(api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height, api::GLint border)
{
	log() << warning << "CopyTexImage2D not available." << std::endl;
}

static void fallback_glCopyTexImage2DEXT(api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height, api::GLint border)
{
	log() << warning << "CopyTexImage2DEXT not available." << std::endl;
}

static void fallback_glCopyTexSubImage1D(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint x, api::GLint y, api::GLsizei width)
{
	log() << warning << "CopyTexSubImage1D not available." << std::endl;
}

static void fallback_glCopyTexSubImage1DEXT(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint x, api::GLint y, api::GLsizei width)
{
	log() << warning << "CopyTexSubImage1DEXT not available." << std::endl;
}

static void fallback_glCopyTexSubImage2D(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "CopyTexSubImage2D not available." << std::endl;
}

static void fallback_glCopyTexSubImage2DEXT(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "CopyTexSubImage2DEXT not available." << std::endl;
}

static void fallback_glCopyTexSubImage3D(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint zoffset, api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "CopyTexSubImage3D not available." << std::endl;
}

static void fallback_glCopyTexSubImage3DEXT(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint zoffset, api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "CopyTexSubImage3DEXT not available." << std::endl;
}

static void fallback_glCopyTextureImage1DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLint x, api::GLint y, api::GLsizei width, api::GLint border)
{
	log() << warning << "CopyTextureImage1DEXT not available." << std::endl;
}

static void fallback_glCopyTextureImage2DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height, api::GLint border)
{
	log() << warning << "CopyTextureImage2DEXT not available." << std::endl;
}

static void fallback_glCopyTextureSubImage1DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint x, api::GLint y, api::GLsizei width)
{
	log() << warning << "CopyTextureSubImage1DEXT not available." << std::endl;
}

static void fallback_glCopyTextureSubImage2DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "CopyTextureSubImage2DEXT not available." << std::endl;
}

static void fallback_glCopyTextureSubImage3DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint zoffset, api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "CopyTextureSubImage3DEXT not available." << std::endl;
}

static api::GLuint fallback_glCreateProgram()
{
	log() << warning << "CreateProgram not available." << std::endl;
}

static api::GLhandleARB fallback_glCreateProgramObjectARB()
{
	log() << warning << "CreateProgramObjectARB not available." << std::endl;
}

static api::GLuint fallback_glCreateShader(api::GLenum type)
{
	log() << warning << "CreateShader not available." << std::endl;
}

static api::GLhandleARB fallback_glCreateShaderObjectARB(api::GLenum shaderType)
{
	log() << warning << "CreateShaderObjectARB not available." << std::endl;
}

static api::GLuint fallback_glCreateShaderProgramEXT(api::GLenum type, const api::GLchar* string)
{
	log() << warning << "CreateShaderProgramEXT not available." << std::endl;
}

static void fallback_glCullFace(api::GLenum mode)
{
	log() << warning << "CullFace not available." << std::endl;
}

static void fallback_glCullParameterdvEXT(api::GLenum pname, api::GLdouble* params)
{
	log() << warning << "CullParameterdvEXT not available." << std::endl;
}

static void fallback_glCullParameterfvEXT(api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "CullParameterfvEXT not available." << std::endl;
}

static void fallback_glCurrentPaletteMatrixARB(api::GLint index)
{
	log() << warning << "CurrentPaletteMatrixARB not available." << std::endl;
}

static void fallback_glDeformSGIX(api::GLbitfield mask)
{
	log() << warning << "DeformSGIX not available." << std::endl;
}

static void fallback_glDeformationMap3dSGIX(api::GLenum target, api::GLdouble u1, api::GLdouble u2, api::GLint ustride, api::GLint uorder, api::GLdouble v1, api::GLdouble v2, api::GLint vstride, api::GLint vorder, api::GLdouble w1, api::GLdouble w2, api::GLint wstride, api::GLint worder, const api::GLdouble* points)
{
	log() << warning << "DeformationMap3dSGIX not available." << std::endl;
}

static void fallback_glDeformationMap3fSGIX(api::GLenum target, api::GLfloat u1, api::GLfloat u2, api::GLint ustride, api::GLint uorder, api::GLfloat v1, api::GLfloat v2, api::GLint vstride, api::GLint vorder, api::GLfloat w1, api::GLfloat w2, api::GLint wstride, api::GLint worder, const api::GLfloat* points)
{
	log() << warning << "DeformationMap3fSGIX not available." << std::endl;
}

static void fallback_glDeleteAsyncMarkersSGIX(api::GLuint marker, api::GLsizei range)
{
	log() << warning << "DeleteAsyncMarkersSGIX not available." << std::endl;
}

static void fallback_glDeleteBuffers(api::GLsizei n, const api::GLuint* buffers)
{
	log() << warning << "DeleteBuffers not available." << std::endl;
}

static void fallback_glDeleteBuffersARB(api::GLsizei n, const api::GLuint* buffers)
{
	log() << warning << "DeleteBuffersARB not available." << std::endl;
}

static void fallback_glDeleteFencesAPPLE(api::GLsizei n, const api::GLuint* fences)
{
	log() << warning << "DeleteFencesAPPLE not available." << std::endl;
}

static void fallback_glDeleteFencesNV(api::GLsizei n, const api::GLuint* fences)
{
	log() << warning << "DeleteFencesNV not available." << std::endl;
}

static void fallback_glDeleteFragmentShaderATI(api::GLuint id)
{
	log() << warning << "DeleteFragmentShaderATI not available." << std::endl;
}

static void fallback_glDeleteFramebuffers(api::GLsizei n, const api::GLuint* framebuffers)
{
	log() << warning << "DeleteFramebuffers not available." << std::endl;
}

static void fallback_glDeleteFramebuffersEXT(api::GLsizei n, const api::GLuint* framebuffers)
{
	log() << warning << "DeleteFramebuffersEXT not available." << std::endl;
}

static void fallback_glDeleteLists(api::GLuint list, api::GLsizei range)
{
	log() << warning << "DeleteLists not available." << std::endl;
}

static void fallback_glDeleteNamedStringARB(api::GLint namelen, const api::GLchar* name)
{
	log() << warning << "DeleteNamedStringARB not available." << std::endl;
}

static void fallback_glDeleteObjectARB(api::GLhandleARB obj)
{
	log() << warning << "DeleteObjectARB not available." << std::endl;
}

static void fallback_glDeleteOcclusionQueriesNV(api::GLsizei n, const api::GLuint* ids)
{
	log() << warning << "DeleteOcclusionQueriesNV not available." << std::endl;
}

static void fallback_glDeletePerfMonitorsAMD(api::GLsizei n, api::GLuint* monitors)
{
	log() << warning << "DeletePerfMonitorsAMD not available." << std::endl;
}

static void fallback_glDeleteProgram(api::GLuint program)
{
	log() << warning << "DeleteProgram not available." << std::endl;
}

static void fallback_glDeleteProgramsARB(api::GLsizei n, const api::GLuint* programs)
{
	log() << warning << "DeleteProgramsARB not available." << std::endl;
}

static void fallback_glDeleteProgramsNV(api::GLsizei n, const api::GLuint* programs)
{
	log() << warning << "DeleteProgramsNV not available." << std::endl;
}

static void fallback_glDeleteQueries(api::GLsizei n, const api::GLuint* ids)
{
	log() << warning << "DeleteQueries not available." << std::endl;
}

static void fallback_glDeleteQueriesARB(api::GLsizei n, const api::GLuint* ids)
{
	log() << warning << "DeleteQueriesARB not available." << std::endl;
}

static void fallback_glDeleteRenderbuffers(api::GLsizei n, const api::GLuint* renderbuffers)
{
	log() << warning << "DeleteRenderbuffers not available." << std::endl;
}

static void fallback_glDeleteRenderbuffersEXT(api::GLsizei n, const api::GLuint* renderbuffers)
{
	log() << warning << "DeleteRenderbuffersEXT not available." << std::endl;
}

static void fallback_glDeleteSamplers(api::GLsizei count, const api::GLuint* samplers)
{
	log() << warning << "DeleteSamplers not available." << std::endl;
}

static void fallback_glDeleteShader(api::GLuint shader)
{
	log() << warning << "DeleteShader not available." << std::endl;
}

static void fallback_glDeleteSync(api::GLsync sync)
{
	log() << warning << "DeleteSync not available." << std::endl;
}

static void fallback_glDeleteTextures(api::GLsizei n, const api::GLuint* textures)
{
	log() << warning << "DeleteTextures not available." << std::endl;
}

static void fallback_glDeleteTexturesEXT(api::GLsizei n, const api::GLuint* textures)
{
	log() << warning << "DeleteTexturesEXT not available." << std::endl;
}

static void fallback_glDeleteTransformFeedbacks(api::GLsizei n, const api::GLuint* ids)
{
	log() << warning << "DeleteTransformFeedbacks not available." << std::endl;
}

static void fallback_glDeleteTransformFeedbacksNV(api::GLsizei n, const api::GLuint* ids)
{
	log() << warning << "DeleteTransformFeedbacksNV not available." << std::endl;
}

static void fallback_glDeleteVertexArrays(api::GLsizei n, const api::GLuint* arrays)
{
	log() << warning << "DeleteVertexArrays not available." << std::endl;
}

static void fallback_glDeleteVertexArraysAPPLE(api::GLsizei n, const api::GLuint* arrays)
{
	log() << warning << "DeleteVertexArraysAPPLE not available." << std::endl;
}

static void fallback_glDeleteVertexShaderEXT(api::GLuint id)
{
	log() << warning << "DeleteVertexShaderEXT not available." << std::endl;
}

static void fallback_glDepthBoundsEXT(api::GLclampd zmin, api::GLclampd zmax)
{
	log() << warning << "DepthBoundsEXT not available." << std::endl;
}

static void fallback_glDepthBoundsdNV(api::GLdouble zmin, api::GLdouble zmax)
{
	log() << warning << "DepthBoundsdNV not available." << std::endl;
}

static void fallback_glDepthFunc(api::GLenum func)
{
	log() << warning << "DepthFunc not available." << std::endl;
}

static void fallback_glDepthMask(api::GLboolean flag)
{
	log() << warning << "DepthMask not available." << std::endl;
}

static void fallback_glDepthRange(api::GLclampd near, api::GLclampd far)
{
	log() << warning << "DepthRange not available." << std::endl;
}

static void fallback_glDepthRangedNV(api::GLdouble zNear, api::GLdouble zFar)
{
	log() << warning << "DepthRangedNV not available." << std::endl;
}

static void fallback_glDetachObjectARB(api::GLhandleARB containerObj, api::GLhandleARB attachedObj)
{
	log() << warning << "DetachObjectARB not available." << std::endl;
}

static void fallback_glDetachShader(api::GLuint program, api::GLuint shader)
{
	log() << warning << "DetachShader not available." << std::endl;
}

static void fallback_glDetailTexFuncSGIS(api::GLenum target, api::GLsizei n, const api::GLfloat* points)
{
	log() << warning << "DetailTexFuncSGIS not available." << std::endl;
}

static void fallback_glDisable(api::GLenum cap)
{
	log() << warning << "Disable not available." << std::endl;
}

static void fallback_glDisableClientState(api::GLenum array)
{
	log() << warning << "DisableClientState not available." << std::endl;
}

static void fallback_glDisableClientStateIndexedEXT(api::GLenum array, api::GLuint index)
{
	log() << warning << "DisableClientStateIndexedEXT not available." << std::endl;
}

static void fallback_glDisableIndexedEXT(api::GLenum target, api::GLuint index)
{
	log() << warning << "DisableIndexedEXT not available." << std::endl;
}

static void fallback_glDisableVariantClientStateEXT(api::GLuint id)
{
	log() << warning << "DisableVariantClientStateEXT not available." << std::endl;
}

static void fallback_glDisableVertexAttribAPPLE(api::GLuint index, api::GLenum pname)
{
	log() << warning << "DisableVertexAttribAPPLE not available." << std::endl;
}

static void fallback_glDisableVertexAttribArray(api::GLuint index)
{
	log() << warning << "DisableVertexAttribArray not available." << std::endl;
}

static void fallback_glDisableVertexAttribArrayARB(api::GLuint index)
{
	log() << warning << "DisableVertexAttribArrayARB not available." << std::endl;
}

static void fallback_glDisablei(api::GLenum target, api::GLuint index)
{
	log() << warning << "Disablei not available." << std::endl;
}

static void fallback_glDrawArrays(api::GLenum mode, api::GLint first, api::GLsizei count)
{
	log() << warning << "DrawArrays not available." << std::endl;
}

static void fallback_glDrawArraysEXT(api::GLenum mode, api::GLint first, api::GLsizei count)
{
	log() << warning << "DrawArraysEXT not available." << std::endl;
}

static void fallback_glDrawArraysIndirect(api::GLenum mode, const api::GLvoid* indirect)
{
	log() << warning << "DrawArraysIndirect not available." << std::endl;
}

static void fallback_glDrawArraysInstanced(api::GLenum mode, api::GLint first, api::GLsizei count, api::GLsizei primcount)
{
	log() << warning << "DrawArraysInstanced not available." << std::endl;
}

static void fallback_glDrawArraysInstancedARB(api::GLenum mode, api::GLint first, api::GLsizei count, api::GLsizei primcount)
{
	log() << warning << "DrawArraysInstancedARB not available." << std::endl;
}

static void fallback_glDrawArraysInstancedEXT(api::GLenum mode, api::GLint start, api::GLsizei count, api::GLsizei primcount)
{
	log() << warning << "DrawArraysInstancedEXT not available." << std::endl;
}

static void fallback_glDrawBuffer(api::GLenum mode)
{
	log() << warning << "DrawBuffer not available." << std::endl;
}

static void fallback_glDrawBuffers(api::GLsizei n, const api::GLenum* bufs)
{
	log() << warning << "DrawBuffers not available." << std::endl;
}

static void fallback_glDrawBuffersARB(api::GLsizei n, const api::GLenum* bufs)
{
	log() << warning << "DrawBuffersARB not available." << std::endl;
}

static void fallback_glDrawBuffersATI(api::GLsizei n, const api::GLenum* bufs)
{
	log() << warning << "DrawBuffersATI not available." << std::endl;
}

static void fallback_glDrawElementArrayAPPLE(api::GLenum mode, api::GLint first, api::GLsizei count)
{
	log() << warning << "DrawElementArrayAPPLE not available." << std::endl;
}

static void fallback_glDrawElementArrayATI(api::GLenum mode, api::GLsizei count)
{
	log() << warning << "DrawElementArrayATI not available." << std::endl;
}

static void fallback_glDrawElements(api::GLenum mode, api::GLsizei count, api::GLenum type, const api::GLvoid* indices)
{
	log() << warning << "DrawElements not available." << std::endl;
}

static void fallback_glDrawElementsBaseVertex(api::GLenum mode, api::GLsizei count, api::GLenum type, const api::GLvoid* indices, api::GLint basevertex)
{
	log() << warning << "DrawElementsBaseVertex not available." << std::endl;
}

static void fallback_glDrawElementsIndirect(api::GLenum mode, api::GLenum type, const api::GLvoid* indirect)
{
	log() << warning << "DrawElementsIndirect not available." << std::endl;
}

static void fallback_glDrawElementsInstanced(api::GLenum mode, api::GLsizei count, api::GLenum type, const api::GLvoid* indices, api::GLsizei primcount)
{
	log() << warning << "DrawElementsInstanced not available." << std::endl;
}

static void fallback_glDrawElementsInstancedARB(api::GLenum mode, api::GLsizei count, api::GLenum type, const api::GLvoid* indices, api::GLsizei primcount)
{
	log() << warning << "DrawElementsInstancedARB not available." << std::endl;
}

static void fallback_glDrawElementsInstancedBaseVertex(api::GLenum mode, api::GLsizei count, api::GLenum type, const api::GLvoid* indices, api::GLsizei primcount, api::GLint basevertex)
{
	log() << warning << "DrawElementsInstancedBaseVertex not available." << std::endl;
}

static void fallback_glDrawElementsInstancedEXT(api::GLenum mode, api::GLsizei count, api::GLenum type, const api::GLvoid* indices, api::GLsizei primcount)
{
	log() << warning << "DrawElementsInstancedEXT not available." << std::endl;
}

static void fallback_glDrawMeshArraysSUN(api::GLenum mode, api::GLint first, api::GLsizei count, api::GLsizei width)
{
	log() << warning << "DrawMeshArraysSUN not available." << std::endl;
}

static void fallback_glDrawPixels(api::GLsizei width, api::GLsizei height, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "DrawPixels not available." << std::endl;
}

static void fallback_glDrawRangeElementArrayAPPLE(api::GLenum mode, api::GLuint start, api::GLuint end, api::GLint first, api::GLsizei count)
{
	log() << warning << "DrawRangeElementArrayAPPLE not available." << std::endl;
}

static void fallback_glDrawRangeElementArrayATI(api::GLenum mode, api::GLuint start, api::GLuint end, api::GLsizei count)
{
	log() << warning << "DrawRangeElementArrayATI not available." << std::endl;
}

static void fallback_glDrawRangeElements(api::GLenum mode, api::GLuint start, api::GLuint end, api::GLsizei count, api::GLenum type, const api::GLvoid* indices)
{
	log() << warning << "DrawRangeElements not available." << std::endl;
}

static void fallback_glDrawRangeElementsBaseVertex(api::GLenum mode, api::GLuint start, api::GLuint end, api::GLsizei count, api::GLenum type, const api::GLvoid* indices, api::GLint basevertex)
{
	log() << warning << "DrawRangeElementsBaseVertex not available." << std::endl;
}

static void fallback_glDrawRangeElementsEXT(api::GLenum mode, api::GLuint start, api::GLuint end, api::GLsizei count, api::GLenum type, const api::GLvoid* indices)
{
	log() << warning << "DrawRangeElementsEXT not available." << std::endl;
}

static void fallback_glDrawTransformFeedback(api::GLenum mode, api::GLuint id)
{
	log() << warning << "DrawTransformFeedback not available." << std::endl;
}

static void fallback_glDrawTransformFeedbackNV(api::GLenum mode, api::GLuint id)
{
	log() << warning << "DrawTransformFeedbackNV not available." << std::endl;
}

static void fallback_glDrawTransformFeedbackStream(api::GLenum mode, api::GLuint id, api::GLuint stream)
{
	log() << warning << "DrawTransformFeedbackStream not available." << std::endl;
}

static void fallback_glEdgeFlag(api::GLboolean flag)
{
	log() << warning << "EdgeFlag not available." << std::endl;
}

static void fallback_glEdgeFlagFormatNV(api::GLsizei stride)
{
	log() << warning << "EdgeFlagFormatNV not available." << std::endl;
}

static void fallback_glEdgeFlagPointer(api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "EdgeFlagPointer not available." << std::endl;
}

static void fallback_glEdgeFlagPointerEXT(api::GLsizei stride, api::GLsizei count, const api::GLboolean* pointer)
{
	log() << warning << "EdgeFlagPointerEXT not available." << std::endl;
}

static void fallback_glEdgeFlagPointerListIBM(api::GLint stride, const api::GLboolean** pointer, api::GLint ptrstride)
{
	log() << warning << "EdgeFlagPointerListIBM not available." << std::endl;
}

static void fallback_glEdgeFlagv(const api::GLboolean* flag)
{
	log() << warning << "EdgeFlagv not available." << std::endl;
}

static void fallback_glElementPointerAPPLE(api::GLenum type, const api::GLvoid* pointer)
{
	log() << warning << "ElementPointerAPPLE not available." << std::endl;
}

static void fallback_glElementPointerATI(api::GLenum type, const api::GLvoid* pointer)
{
	log() << warning << "ElementPointerATI not available." << std::endl;
}

static void fallback_glEnable(api::GLenum cap)
{
	log() << warning << "Enable not available." << std::endl;
}

static void fallback_glEnableClientState(api::GLenum array)
{
	log() << warning << "EnableClientState not available." << std::endl;
}

static void fallback_glEnableClientStateIndexedEXT(api::GLenum array, api::GLuint index)
{
	log() << warning << "EnableClientStateIndexedEXT not available." << std::endl;
}

static void fallback_glEnableIndexedEXT(api::GLenum target, api::GLuint index)
{
	log() << warning << "EnableIndexedEXT not available." << std::endl;
}

static void fallback_glEnableVariantClientStateEXT(api::GLuint id)
{
	log() << warning << "EnableVariantClientStateEXT not available." << std::endl;
}

static void fallback_glEnableVertexAttribAPPLE(api::GLuint index, api::GLenum pname)
{
	log() << warning << "EnableVertexAttribAPPLE not available." << std::endl;
}

static void fallback_glEnableVertexAttribArray(api::GLuint index)
{
	log() << warning << "EnableVertexAttribArray not available." << std::endl;
}

static void fallback_glEnableVertexAttribArrayARB(api::GLuint index)
{
	log() << warning << "EnableVertexAttribArrayARB not available." << std::endl;
}

static void fallback_glEnablei(api::GLenum target, api::GLuint index)
{
	log() << warning << "Enablei not available." << std::endl;
}

static void fallback_glEnd()
{
	log() << warning << "End not available." << std::endl;
}

static void fallback_glEndConditionalRender()
{
	log() << warning << "EndConditionalRender not available." << std::endl;
}

static void fallback_glEndConditionalRenderNV()
{
	log() << warning << "EndConditionalRenderNV not available." << std::endl;
}

static void fallback_glEndFragmentShaderATI()
{
	log() << warning << "EndFragmentShaderATI not available." << std::endl;
}

static void fallback_glEndList()
{
	log() << warning << "EndList not available." << std::endl;
}

static void fallback_glEndOcclusionQueryNV()
{
	log() << warning << "EndOcclusionQueryNV not available." << std::endl;
}

static void fallback_glEndPerfMonitorAMD(api::GLuint monitor)
{
	log() << warning << "EndPerfMonitorAMD not available." << std::endl;
}

static void fallback_glEndQuery(api::GLenum target)
{
	log() << warning << "EndQuery not available." << std::endl;
}

static void fallback_glEndQueryARB(api::GLenum target)
{
	log() << warning << "EndQueryARB not available." << std::endl;
}

static void fallback_glEndQueryIndexed(api::GLenum target, api::GLuint index)
{
	log() << warning << "EndQueryIndexed not available." << std::endl;
}

static void fallback_glEndTransformFeedback()
{
	log() << warning << "EndTransformFeedback not available." << std::endl;
}

static void fallback_glEndTransformFeedbackEXT()
{
	log() << warning << "EndTransformFeedbackEXT not available." << std::endl;
}

static void fallback_glEndTransformFeedbackNV()
{
	log() << warning << "EndTransformFeedbackNV not available." << std::endl;
}

static void fallback_glEndVertexShaderEXT()
{
	log() << warning << "EndVertexShaderEXT not available." << std::endl;
}

static void fallback_glEndVideoCaptureNV(api::GLuint video_capture_slot)
{
	log() << warning << "EndVideoCaptureNV not available." << std::endl;
}

static void fallback_glEvalCoord1d(api::GLdouble u)
{
	log() << warning << "EvalCoord1d not available." << std::endl;
}

static void fallback_glEvalCoord1dv(const api::GLdouble* u)
{
	log() << warning << "EvalCoord1dv not available." << std::endl;
}

static void fallback_glEvalCoord1f(api::GLfloat u)
{
	log() << warning << "EvalCoord1f not available." << std::endl;
}

static void fallback_glEvalCoord1fv(const api::GLfloat* u)
{
	log() << warning << "EvalCoord1fv not available." << std::endl;
}

static void fallback_glEvalCoord2d(api::GLdouble u, api::GLdouble v)
{
	log() << warning << "EvalCoord2d not available." << std::endl;
}

static void fallback_glEvalCoord2dv(const api::GLdouble* u)
{
	log() << warning << "EvalCoord2dv not available." << std::endl;
}

static void fallback_glEvalCoord2f(api::GLfloat u, api::GLfloat v)
{
	log() << warning << "EvalCoord2f not available." << std::endl;
}

static void fallback_glEvalCoord2fv(const api::GLfloat* u)
{
	log() << warning << "EvalCoord2fv not available." << std::endl;
}

static void fallback_glEvalMapsNV(api::GLenum target, api::GLenum mode)
{
	log() << warning << "EvalMapsNV not available." << std::endl;
}

static void fallback_glEvalMesh1(api::GLenum mode, api::GLint i1, api::GLint i2)
{
	log() << warning << "EvalMesh1 not available." << std::endl;
}

static void fallback_glEvalMesh2(api::GLenum mode, api::GLint i1, api::GLint i2, api::GLint j1, api::GLint j2)
{
	log() << warning << "EvalMesh2 not available." << std::endl;
}

static void fallback_glEvalPoint1(api::GLint i)
{
	log() << warning << "EvalPoint1 not available." << std::endl;
}

static void fallback_glEvalPoint2(api::GLint i, api::GLint j)
{
	log() << warning << "EvalPoint2 not available." << std::endl;
}

static void fallback_glExecuteProgramNV(api::GLenum target, api::GLuint id, const api::GLfloat* params)
{
	log() << warning << "ExecuteProgramNV not available." << std::endl;
}

static void fallback_glExtractComponentEXT(api::GLuint res, api::GLuint src, api::GLuint num)
{
	log() << warning << "ExtractComponentEXT not available." << std::endl;
}

static void fallback_glFeedbackBuffer(api::GLsizei size, api::GLenum type, api::GLfloat* buffer)
{
	log() << warning << "FeedbackBuffer not available." << std::endl;
}

static api::GLsync fallback_glFenceSync(api::GLenum condition, api::GLbitfield flags)
{
	log() << warning << "FenceSync not available." << std::endl;
}

static void fallback_glFinalCombinerInputNV(api::GLenum variable, api::GLenum input, api::GLenum mapping, api::GLenum componentUsage)
{
	log() << warning << "FinalCombinerInputNV not available." << std::endl;
}

static void fallback_glFinish()
{
	log() << warning << "Finish not available." << std::endl;
}

static api::GLint fallback_glFinishAsyncSGIX(api::GLuint* markerp)
{
	log() << warning << "FinishAsyncSGIX not available." << std::endl;
}

static void fallback_glFinishFenceAPPLE(api::GLuint fence)
{
	log() << warning << "FinishFenceAPPLE not available." << std::endl;
}

static void fallback_glFinishFenceNV(api::GLuint fence)
{
	log() << warning << "FinishFenceNV not available." << std::endl;
}

static void fallback_glFinishObjectAPPLE(api::GLenum object, api::GLint name)
{
	log() << warning << "FinishObjectAPPLE not available." << std::endl;
}

static void fallback_glFinishTextureSUNX()
{
	log() << warning << "FinishTextureSUNX not available." << std::endl;
}

static void fallback_glFlush()
{
	log() << warning << "Flush not available." << std::endl;
}

static void fallback_glFlushMappedBufferRange(api::GLenum target, api::GLintptr offset, api::GLsizeiptr length)
{
	log() << warning << "FlushMappedBufferRange not available." << std::endl;
}

static void fallback_glFlushMappedBufferRangeAPPLE(api::GLenum target, api::GLintptr offset, api::GLsizeiptr size)
{
	log() << warning << "FlushMappedBufferRangeAPPLE not available." << std::endl;
}

static void fallback_glFlushPixelDataRangeNV(api::GLenum target)
{
	log() << warning << "FlushPixelDataRangeNV not available." << std::endl;
}

static void fallback_glFlushRasterSGIX()
{
	log() << warning << "FlushRasterSGIX not available." << std::endl;
}

static void fallback_glFlushVertexArrayRangeAPPLE(api::GLsizei length, api::GLvoid* pointer)
{
	log() << warning << "FlushVertexArrayRangeAPPLE not available." << std::endl;
}

static void fallback_glFlushVertexArrayRangeNV()
{
	log() << warning << "FlushVertexArrayRangeNV not available." << std::endl;
}

static void fallback_glFogCoordFormatNV(api::GLenum type, api::GLsizei stride)
{
	log() << warning << "FogCoordFormatNV not available." << std::endl;
}

static void fallback_glFogCoordPointer(api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "FogCoordPointer not available." << std::endl;
}

static void fallback_glFogCoordPointerEXT(api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "FogCoordPointerEXT not available." << std::endl;
}

static void fallback_glFogCoordPointerListIBM(api::GLenum type, api::GLint stride, const api::GLvoid** pointer, api::GLint ptrstride)
{
	log() << warning << "FogCoordPointerListIBM not available." << std::endl;
}

static void fallback_glFogCoordd(api::GLdouble coord)
{
	log() << warning << "FogCoordd not available." << std::endl;
}

static void fallback_glFogCoorddEXT(api::GLdouble coord)
{
	log() << warning << "FogCoorddEXT not available." << std::endl;
}

static void fallback_glFogCoorddv(const api::GLdouble* coord)
{
	log() << warning << "FogCoorddv not available." << std::endl;
}

static void fallback_glFogCoorddvEXT(const api::GLdouble* coord)
{
	log() << warning << "FogCoorddvEXT not available." << std::endl;
}

static void fallback_glFogCoordf(api::GLfloat coord)
{
	log() << warning << "FogCoordf not available." << std::endl;
}

static void fallback_glFogCoordfEXT(api::GLfloat coord)
{
	log() << warning << "FogCoordfEXT not available." << std::endl;
}

static void fallback_glFogCoordfv(const api::GLfloat* coord)
{
	log() << warning << "FogCoordfv not available." << std::endl;
}

static void fallback_glFogCoordfvEXT(const api::GLfloat* coord)
{
	log() << warning << "FogCoordfvEXT not available." << std::endl;
}

static void fallback_glFogCoordhNV(api::GLhalfNV fog)
{
	log() << warning << "FogCoordhNV not available." << std::endl;
}

static void fallback_glFogCoordhvNV(const api::GLhalfNV* fog)
{
	log() << warning << "FogCoordhvNV not available." << std::endl;
}

static void fallback_glFogFuncSGIS(api::GLsizei n, const api::GLfloat* points)
{
	log() << warning << "FogFuncSGIS not available." << std::endl;
}

static void fallback_glFogf(api::GLenum pname, api::GLfloat param)
{
	log() << warning << "Fogf not available." << std::endl;
}

static void fallback_glFogfv(api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "Fogfv not available." << std::endl;
}

static void fallback_glFogi(api::GLenum pname, api::GLint param)
{
	log() << warning << "Fogi not available." << std::endl;
}

static void fallback_glFogiv(api::GLenum pname, const api::GLint* params)
{
	log() << warning << "Fogiv not available." << std::endl;
}

static void fallback_glFragmentColorMaterialSGIX(api::GLenum face, api::GLenum mode)
{
	log() << warning << "FragmentColorMaterialSGIX not available." << std::endl;
}

static void fallback_glFragmentLightModelfSGIX(api::GLenum pname, api::GLfloat param)
{
	log() << warning << "FragmentLightModelfSGIX not available." << std::endl;
}

static void fallback_glFragmentLightModelfvSGIX(api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "FragmentLightModelfvSGIX not available." << std::endl;
}

static void fallback_glFragmentLightModeliSGIX(api::GLenum pname, api::GLint param)
{
	log() << warning << "FragmentLightModeliSGIX not available." << std::endl;
}

static void fallback_glFragmentLightModelivSGIX(api::GLenum pname, const api::GLint* params)
{
	log() << warning << "FragmentLightModelivSGIX not available." << std::endl;
}

static void fallback_glFragmentLightfSGIX(api::GLenum light, api::GLenum pname, api::GLfloat param)
{
	log() << warning << "FragmentLightfSGIX not available." << std::endl;
}

static void fallback_glFragmentLightfvSGIX(api::GLenum light, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "FragmentLightfvSGIX not available." << std::endl;
}

static void fallback_glFragmentLightiSGIX(api::GLenum light, api::GLenum pname, api::GLint param)
{
	log() << warning << "FragmentLightiSGIX not available." << std::endl;
}

static void fallback_glFragmentLightivSGIX(api::GLenum light, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "FragmentLightivSGIX not available." << std::endl;
}

static void fallback_glFragmentMaterialfSGIX(api::GLenum face, api::GLenum pname, api::GLfloat param)
{
	log() << warning << "FragmentMaterialfSGIX not available." << std::endl;
}

static void fallback_glFragmentMaterialfvSGIX(api::GLenum face, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "FragmentMaterialfvSGIX not available." << std::endl;
}

static void fallback_glFragmentMaterialiSGIX(api::GLenum face, api::GLenum pname, api::GLint param)
{
	log() << warning << "FragmentMaterialiSGIX not available." << std::endl;
}

static void fallback_glFragmentMaterialivSGIX(api::GLenum face, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "FragmentMaterialivSGIX not available." << std::endl;
}

static void fallback_glFrameTerminatorGREMEDY()
{
	log() << warning << "FrameTerminatorGREMEDY not available." << std::endl;
}

static void fallback_glFrameZoomSGIX(api::GLint factor)
{
	log() << warning << "FrameZoomSGIX not available." << std::endl;
}

static void fallback_glFramebufferDrawBufferEXT(api::GLuint framebuffer, api::GLenum mode)
{
	log() << warning << "FramebufferDrawBufferEXT not available." << std::endl;
}

static void fallback_glFramebufferDrawBuffersEXT(api::GLuint framebuffer, api::GLsizei n, const api::GLenum* bufs)
{
	log() << warning << "FramebufferDrawBuffersEXT not available." << std::endl;
}

static void fallback_glFramebufferReadBufferEXT(api::GLuint framebuffer, api::GLenum mode)
{
	log() << warning << "FramebufferReadBufferEXT not available." << std::endl;
}

static void fallback_glFramebufferRenderbuffer(api::GLenum target, api::GLenum attachment, api::GLenum renderbuffertarget, api::GLuint renderbuffer)
{
	log() << warning << "FramebufferRenderbuffer not available." << std::endl;
}

static void fallback_glFramebufferRenderbufferEXT(api::GLenum target, api::GLenum attachment, api::GLenum renderbuffertarget, api::GLuint renderbuffer)
{
	log() << warning << "FramebufferRenderbufferEXT not available." << std::endl;
}

static void fallback_glFramebufferTexture(api::GLenum target, api::GLenum attachment, api::GLuint texture, api::GLint level)
{
	log() << warning << "FramebufferTexture not available." << std::endl;
}

static void fallback_glFramebufferTexture1D(api::GLenum target, api::GLenum attachment, api::GLenum textarget, api::GLuint texture, api::GLint level)
{
	log() << warning << "FramebufferTexture1D not available." << std::endl;
}

static void fallback_glFramebufferTexture1DEXT(api::GLenum target, api::GLenum attachment, api::GLenum textarget, api::GLuint texture, api::GLint level)
{
	log() << warning << "FramebufferTexture1DEXT not available." << std::endl;
}

static void fallback_glFramebufferTexture2D(api::GLenum target, api::GLenum attachment, api::GLenum textarget, api::GLuint texture, api::GLint level)
{
	log() << warning << "FramebufferTexture2D not available." << std::endl;
}

static void fallback_glFramebufferTexture2DEXT(api::GLenum target, api::GLenum attachment, api::GLenum textarget, api::GLuint texture, api::GLint level)
{
	log() << warning << "FramebufferTexture2DEXT not available." << std::endl;
}

static void fallback_glFramebufferTexture3D(api::GLenum target, api::GLenum attachment, api::GLenum textarget, api::GLuint texture, api::GLint level, api::GLint zoffset)
{
	log() << warning << "FramebufferTexture3D not available." << std::endl;
}

static void fallback_glFramebufferTexture3DEXT(api::GLenum target, api::GLenum attachment, api::GLenum textarget, api::GLuint texture, api::GLint level, api::GLint zoffset)
{
	log() << warning << "FramebufferTexture3DEXT not available." << std::endl;
}

static void fallback_glFramebufferTextureARB(api::GLenum target, api::GLenum attachment, api::GLuint texture, api::GLint level)
{
	log() << warning << "FramebufferTextureARB not available." << std::endl;
}

static void fallback_glFramebufferTextureEXT(api::GLenum target, api::GLenum attachment, api::GLuint texture, api::GLint level)
{
	log() << warning << "FramebufferTextureEXT not available." << std::endl;
}

static void fallback_glFramebufferTextureFaceARB(api::GLenum target, api::GLenum attachment, api::GLuint texture, api::GLint level, api::GLenum face)
{
	log() << warning << "FramebufferTextureFaceARB not available." << std::endl;
}

static void fallback_glFramebufferTextureFaceEXT(api::GLenum target, api::GLenum attachment, api::GLuint texture, api::GLint level, api::GLenum face)
{
	log() << warning << "FramebufferTextureFaceEXT not available." << std::endl;
}

static void fallback_glFramebufferTextureLayer(api::GLenum target, api::GLenum attachment, api::GLuint texture, api::GLint level, api::GLint layer)
{
	log() << warning << "FramebufferTextureLayer not available." << std::endl;
}

static void fallback_glFramebufferTextureLayerARB(api::GLenum target, api::GLenum attachment, api::GLuint texture, api::GLint level, api::GLint layer)
{
	log() << warning << "FramebufferTextureLayerARB not available." << std::endl;
}

static void fallback_glFramebufferTextureLayerEXT(api::GLenum target, api::GLenum attachment, api::GLuint texture, api::GLint level, api::GLint layer)
{
	log() << warning << "FramebufferTextureLayerEXT not available." << std::endl;
}

static void fallback_glFreeObjectBufferATI(api::GLuint buffer)
{
	log() << warning << "FreeObjectBufferATI not available." << std::endl;
}

static void fallback_glFrontFace(api::GLenum mode)
{
	log() << warning << "FrontFace not available." << std::endl;
}

static void fallback_glFrustum(api::GLdouble left, api::GLdouble right, api::GLdouble bottom, api::GLdouble top, api::GLdouble zNear, api::GLdouble zFar)
{
	log() << warning << "Frustum not available." << std::endl;
}

static api::GLuint fallback_glGenAsyncMarkersSGIX(api::GLsizei range)
{
	log() << warning << "GenAsyncMarkersSGIX not available." << std::endl;
}

static void fallback_glGenBuffers(api::GLsizei n, api::GLuint* buffers)
{
	log() << warning << "GenBuffers not available." << std::endl;
}

static void fallback_glGenBuffersARB(api::GLsizei n, api::GLuint* buffers)
{
	log() << warning << "GenBuffersARB not available." << std::endl;
}

static void fallback_glGenFencesAPPLE(api::GLsizei n, api::GLuint* fences)
{
	log() << warning << "GenFencesAPPLE not available." << std::endl;
}

static void fallback_glGenFencesNV(api::GLsizei n, api::GLuint* fences)
{
	log() << warning << "GenFencesNV not available." << std::endl;
}

static api::GLuint fallback_glGenFragmentShadersATI(api::GLuint range)
{
	log() << warning << "GenFragmentShadersATI not available." << std::endl;
}

static void fallback_glGenFramebuffers(api::GLsizei n, api::GLuint* framebuffers)
{
	log() << warning << "GenFramebuffers not available." << std::endl;
}

static void fallback_glGenFramebuffersEXT(api::GLsizei n, api::GLuint* framebuffers)
{
	log() << warning << "GenFramebuffersEXT not available." << std::endl;
}

static api::GLuint fallback_glGenLists(api::GLsizei range)
{
	log() << warning << "GenLists not available." << std::endl;
}

static void fallback_glGenOcclusionQueriesNV(api::GLsizei n, api::GLuint* ids)
{
	log() << warning << "GenOcclusionQueriesNV not available." << std::endl;
}

static void fallback_glGenPerfMonitorsAMD(api::GLsizei n, api::GLuint* monitors)
{
	log() << warning << "GenPerfMonitorsAMD not available." << std::endl;
}

static void fallback_glGenProgramsARB(api::GLsizei n, api::GLuint* programs)
{
	log() << warning << "GenProgramsARB not available." << std::endl;
}

static void fallback_glGenProgramsNV(api::GLsizei n, api::GLuint* programs)
{
	log() << warning << "GenProgramsNV not available." << std::endl;
}

static void fallback_glGenQueries(api::GLsizei n, api::GLuint* ids)
{
	log() << warning << "GenQueries not available." << std::endl;
}

static void fallback_glGenQueriesARB(api::GLsizei n, api::GLuint* ids)
{
	log() << warning << "GenQueriesARB not available." << std::endl;
}

static void fallback_glGenRenderbuffers(api::GLsizei n, api::GLuint* renderbuffers)
{
	log() << warning << "GenRenderbuffers not available." << std::endl;
}

static void fallback_glGenRenderbuffersEXT(api::GLsizei n, api::GLuint* renderbuffers)
{
	log() << warning << "GenRenderbuffersEXT not available." << std::endl;
}

static void fallback_glGenSamplers(api::GLsizei count, api::GLuint* samplers)
{
	log() << warning << "GenSamplers not available." << std::endl;
}

static api::GLuint fallback_glGenSymbolsEXT(api::GLenum datatype, api::GLenum storagetype, api::GLenum range, api::GLuint components)
{
	log() << warning << "GenSymbolsEXT not available." << std::endl;
}

static void fallback_glGenTextures(api::GLsizei n, api::GLuint* textures)
{
	log() << warning << "GenTextures not available." << std::endl;
}

static void fallback_glGenTexturesEXT(api::GLsizei n, api::GLuint* textures)
{
	log() << warning << "GenTexturesEXT not available." << std::endl;
}

static void fallback_glGenTransformFeedbacks(api::GLsizei n, api::GLuint* ids)
{
	log() << warning << "GenTransformFeedbacks not available." << std::endl;
}

static void fallback_glGenTransformFeedbacksNV(api::GLsizei n, api::GLuint* ids)
{
	log() << warning << "GenTransformFeedbacksNV not available." << std::endl;
}

static void fallback_glGenVertexArrays(api::GLsizei n, api::GLuint* arrays)
{
	log() << warning << "GenVertexArrays not available." << std::endl;
}

static void fallback_glGenVertexArraysAPPLE(api::GLsizei n, api::GLuint* arrays)
{
	log() << warning << "GenVertexArraysAPPLE not available." << std::endl;
}

static api::GLuint fallback_glGenVertexShadersEXT(api::GLuint range)
{
	log() << warning << "GenVertexShadersEXT not available." << std::endl;
}

static void fallback_glGenerateMipmap(api::GLenum target)
{
	log() << warning << "GenerateMipmap not available." << std::endl;
}

static void fallback_glGenerateMipmapEXT(api::GLenum target)
{
	log() << warning << "GenerateMipmapEXT not available." << std::endl;
}

static void fallback_glGenerateMultiTexMipmapEXT(api::GLenum texunit, api::GLenum target)
{
	log() << warning << "GenerateMultiTexMipmapEXT not available." << std::endl;
}

static void fallback_glGenerateTextureMipmapEXT(api::GLuint texture, api::GLenum target)
{
	log() << warning << "GenerateTextureMipmapEXT not available." << std::endl;
}

static void fallback_glGetActiveAttrib(api::GLuint program, api::GLuint index, api::GLsizei bufSize, api::GLsizei* length, api::GLint* size, api::GLenum* type, api::GLchar* name)
{
	log() << warning << "GetActiveAttrib not available." << std::endl;
}

static void fallback_glGetActiveAttribARB(api::GLhandleARB programObj, api::GLuint index, api::GLsizei maxLength, api::GLsizei* length, api::GLint* size, api::GLenum* type, api::GLcharARB* name)
{
	log() << warning << "GetActiveAttribARB not available." << std::endl;
}

static void fallback_glGetActiveSubroutineName(api::GLuint program, api::GLenum shadertype, api::GLuint index, api::GLsizei bufsize, api::GLsizei* length, api::GLchar* name)
{
	log() << warning << "GetActiveSubroutineName not available." << std::endl;
}

static void fallback_glGetActiveSubroutineUniformName(api::GLuint program, api::GLenum shadertype, api::GLuint index, api::GLsizei bufsize, api::GLsizei* length, api::GLchar* name)
{
	log() << warning << "GetActiveSubroutineUniformName not available." << std::endl;
}

static void fallback_glGetActiveSubroutineUniformiv(api::GLuint program, api::GLenum shadertype, api::GLuint index, api::GLenum pname, api::GLint* values)
{
	log() << warning << "GetActiveSubroutineUniformiv not available." << std::endl;
}

static void fallback_glGetActiveUniform(api::GLuint program, api::GLuint index, api::GLsizei bufSize, api::GLsizei* length, api::GLint* size, api::GLenum* type, api::GLchar* name)
{
	log() << warning << "GetActiveUniform not available." << std::endl;
}

static void fallback_glGetActiveUniformARB(api::GLhandleARB programObj, api::GLuint index, api::GLsizei maxLength, api::GLsizei* length, api::GLint* size, api::GLenum* type, api::GLcharARB* name)
{
	log() << warning << "GetActiveUniformARB not available." << std::endl;
}

static void fallback_glGetActiveUniformBlockName(api::GLuint program, api::GLuint uniformBlockIndex, api::GLsizei bufSize, api::GLsizei* length, api::GLchar* uniformBlockName)
{
	log() << warning << "GetActiveUniformBlockName not available." << std::endl;
}

static void fallback_glGetActiveUniformBlockiv(api::GLuint program, api::GLuint uniformBlockIndex, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetActiveUniformBlockiv not available." << std::endl;
}

static void fallback_glGetActiveUniformName(api::GLuint program, api::GLuint uniformIndex, api::GLsizei bufSize, api::GLsizei* length, api::GLchar* uniformName)
{
	log() << warning << "GetActiveUniformName not available." << std::endl;
}

static void fallback_glGetActiveUniformsiv(api::GLuint program, api::GLsizei uniformCount, const api::GLuint* uniformIndices, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetActiveUniformsiv not available." << std::endl;
}

static void fallback_glGetActiveVaryingNV(api::GLuint program, api::GLuint index, api::GLsizei bufSize, api::GLsizei* length, api::GLsizei* size, api::GLenum* type, api::GLchar* name)
{
	log() << warning << "GetActiveVaryingNV not available." << std::endl;
}

static void fallback_glGetArrayObjectfvATI(api::GLenum array, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetArrayObjectfvATI not available." << std::endl;
}

static void fallback_glGetArrayObjectivATI(api::GLenum array, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetArrayObjectivATI not available." << std::endl;
}

static void fallback_glGetAttachedObjectsARB(api::GLhandleARB containerObj, api::GLsizei maxCount, api::GLsizei* count, api::GLhandleARB* obj)
{
	log() << warning << "GetAttachedObjectsARB not available." << std::endl;
}

static void fallback_glGetAttachedShaders(api::GLuint program, api::GLsizei maxCount, api::GLsizei* count, api::GLuint* obj)
{
	log() << warning << "GetAttachedShaders not available." << std::endl;
}

static api::GLint fallback_glGetAttribLocation(api::GLuint program, const api::GLchar* name)
{
	log() << warning << "GetAttribLocation not available." << std::endl;
}

static api::GLint fallback_glGetAttribLocationARB(api::GLhandleARB programObj, const api::GLcharARB* name)
{
	log() << warning << "GetAttribLocationARB not available." << std::endl;
}

static void fallback_glGetBooleanIndexedvEXT(api::GLenum target, api::GLuint index, api::GLboolean* data)
{
	log() << warning << "GetBooleanIndexedvEXT not available." << std::endl;
}

static void fallback_glGetBooleani_v(api::GLenum target, api::GLuint index, api::GLboolean* data)
{
	log() << warning << "GetBooleani_v not available." << std::endl;
}

static void fallback_glGetBooleanv(api::GLenum pname, api::GLboolean* params)
{
	log() << warning << "GetBooleanv not available." << std::endl;
}

static void fallback_glGetBufferParameteri64v(api::GLenum target, api::GLenum pname, api::GLint64* params)
{
	log() << warning << "GetBufferParameteri64v not available." << std::endl;
}

static void fallback_glGetBufferParameteriv(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetBufferParameteriv not available." << std::endl;
}

static void fallback_glGetBufferParameterivARB(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetBufferParameterivARB not available." << std::endl;
}

static void fallback_glGetBufferParameterui64vNV(api::GLenum target, api::GLenum pname, api::GLuint64EXT* params)
{
	log() << warning << "GetBufferParameterui64vNV not available." << std::endl;
}

static void fallback_glGetBufferPointerv(api::GLenum target, api::GLenum pname, api::GLvoid** params)
{
	log() << warning << "GetBufferPointerv not available." << std::endl;
}

static void fallback_glGetBufferPointervARB(api::GLenum target, api::GLenum pname, api::GLvoid** params)
{
	log() << warning << "GetBufferPointervARB not available." << std::endl;
}

static void fallback_glGetBufferSubData(api::GLenum target, api::GLintptr offset, api::GLsizeiptr size, api::GLvoid* data)
{
	log() << warning << "GetBufferSubData not available." << std::endl;
}

static void fallback_glGetBufferSubDataARB(api::GLenum target, api::GLintptrARB offset, api::GLsizeiptrARB size, api::GLvoid* data)
{
	log() << warning << "GetBufferSubDataARB not available." << std::endl;
}

static void fallback_glGetClipPlane(api::GLenum plane, api::GLdouble* equation)
{
	log() << warning << "GetClipPlane not available." << std::endl;
}

static void fallback_glGetColorTable(api::GLenum target, api::GLenum format, api::GLenum type, api::GLvoid* table)
{
	log() << warning << "GetColorTable not available." << std::endl;
}

static void fallback_glGetColorTableEXT(api::GLenum target, api::GLenum format, api::GLenum type, api::GLvoid* data)
{
	log() << warning << "GetColorTableEXT not available." << std::endl;
}

static void fallback_glGetColorTableParameterfv(api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetColorTableParameterfv not available." << std::endl;
}

static void fallback_glGetColorTableParameterfvEXT(api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetColorTableParameterfvEXT not available." << std::endl;
}

static void fallback_glGetColorTableParameterfvSGI(api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetColorTableParameterfvSGI not available." << std::endl;
}

static void fallback_glGetColorTableParameteriv(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetColorTableParameteriv not available." << std::endl;
}

static void fallback_glGetColorTableParameterivEXT(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetColorTableParameterivEXT not available." << std::endl;
}

static void fallback_glGetColorTableParameterivSGI(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetColorTableParameterivSGI not available." << std::endl;
}

static void fallback_glGetColorTableSGI(api::GLenum target, api::GLenum format, api::GLenum type, api::GLvoid* table)
{
	log() << warning << "GetColorTableSGI not available." << std::endl;
}

static void fallback_glGetCombinerInputParameterfvNV(api::GLenum stage, api::GLenum portion, api::GLenum variable, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetCombinerInputParameterfvNV not available." << std::endl;
}

static void fallback_glGetCombinerInputParameterivNV(api::GLenum stage, api::GLenum portion, api::GLenum variable, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetCombinerInputParameterivNV not available." << std::endl;
}

static void fallback_glGetCombinerOutputParameterfvNV(api::GLenum stage, api::GLenum portion, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetCombinerOutputParameterfvNV not available." << std::endl;
}

static void fallback_glGetCombinerOutputParameterivNV(api::GLenum stage, api::GLenum portion, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetCombinerOutputParameterivNV not available." << std::endl;
}

static void fallback_glGetCombinerStageParameterfvNV(api::GLenum stage, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetCombinerStageParameterfvNV not available." << std::endl;
}

static void fallback_glGetCompressedMultiTexImageEXT(api::GLenum texunit, api::GLenum target, api::GLint lod, api::GLvoid* img)
{
	log() << warning << "GetCompressedMultiTexImageEXT not available." << std::endl;
}

static void fallback_glGetCompressedTexImage(api::GLenum target, api::GLint level, api::GLvoid* img)
{
	log() << warning << "GetCompressedTexImage not available." << std::endl;
}

static void fallback_glGetCompressedTexImageARB(api::GLenum target, api::GLint level, api::GLvoid* img)
{
	log() << warning << "GetCompressedTexImageARB not available." << std::endl;
}

static void fallback_glGetCompressedTextureImageEXT(api::GLuint texture, api::GLenum target, api::GLint lod, api::GLvoid* img)
{
	log() << warning << "GetCompressedTextureImageEXT not available." << std::endl;
}

static void fallback_glGetConvolutionFilter(api::GLenum target, api::GLenum format, api::GLenum type, api::GLvoid* image)
{
	log() << warning << "GetConvolutionFilter not available." << std::endl;
}

static void fallback_glGetConvolutionFilterEXT(api::GLenum target, api::GLenum format, api::GLenum type, api::GLvoid* image)
{
	log() << warning << "GetConvolutionFilterEXT not available." << std::endl;
}

static void fallback_glGetConvolutionParameterfv(api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetConvolutionParameterfv not available." << std::endl;
}

static void fallback_glGetConvolutionParameterfvEXT(api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetConvolutionParameterfvEXT not available." << std::endl;
}

static void fallback_glGetConvolutionParameteriv(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetConvolutionParameteriv not available." << std::endl;
}

static void fallback_glGetConvolutionParameterivEXT(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetConvolutionParameterivEXT not available." << std::endl;
}

static void fallback_glGetDetailTexFuncSGIS(api::GLenum target, api::GLfloat* points)
{
	log() << warning << "GetDetailTexFuncSGIS not available." << std::endl;
}

static void fallback_glGetDoubleIndexedvEXT(api::GLenum target, api::GLuint index, api::GLdouble* data)
{
	log() << warning << "GetDoubleIndexedvEXT not available." << std::endl;
}

static void fallback_glGetDoublev(api::GLenum pname, api::GLdouble* params)
{
	log() << warning << "GetDoublev not available." << std::endl;
}

static api::GLenum fallback_glGetError()
{
	log() << warning << "GetError not available." << std::endl;
}

static void fallback_glGetFenceivNV(api::GLuint fence, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetFenceivNV not available." << std::endl;
}

static void fallback_glGetFinalCombinerInputParameterfvNV(api::GLenum variable, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetFinalCombinerInputParameterfvNV not available." << std::endl;
}

static void fallback_glGetFinalCombinerInputParameterivNV(api::GLenum variable, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetFinalCombinerInputParameterivNV not available." << std::endl;
}

static void fallback_glGetFloatIndexedvEXT(api::GLenum target, api::GLuint index, api::GLfloat* data)
{
	log() << warning << "GetFloatIndexedvEXT not available." << std::endl;
}

static void fallback_glGetFloatv(api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetFloatv not available." << std::endl;
}

static void fallback_glGetFogFuncSGIS(api::GLfloat* points)
{
	log() << warning << "GetFogFuncSGIS not available." << std::endl;
}

static api::GLint fallback_glGetFragDataIndex(api::GLuint program, const api::GLchar* name)
{
	log() << warning << "GetFragDataIndex not available." << std::endl;
}

static api::GLint fallback_glGetFragDataLocation(api::GLuint program, const api::GLchar* name)
{
	log() << warning << "GetFragDataLocation not available." << std::endl;
}

static api::GLint fallback_glGetFragDataLocationEXT(api::GLuint program, const api::GLchar* name)
{
	log() << warning << "GetFragDataLocationEXT not available." << std::endl;
}

static void fallback_glGetFragmentLightfvSGIX(api::GLenum light, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetFragmentLightfvSGIX not available." << std::endl;
}

static void fallback_glGetFragmentLightivSGIX(api::GLenum light, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetFragmentLightivSGIX not available." << std::endl;
}

static void fallback_glGetFragmentMaterialfvSGIX(api::GLenum face, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetFragmentMaterialfvSGIX not available." << std::endl;
}

static void fallback_glGetFragmentMaterialivSGIX(api::GLenum face, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetFragmentMaterialivSGIX not available." << std::endl;
}

static void fallback_glGetFramebufferAttachmentParameteriv(api::GLenum target, api::GLenum attachment, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetFramebufferAttachmentParameteriv not available." << std::endl;
}

static void fallback_glGetFramebufferAttachmentParameterivEXT(api::GLenum target, api::GLenum attachment, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetFramebufferAttachmentParameterivEXT not available." << std::endl;
}

static void fallback_glGetFramebufferParameterivEXT(api::GLuint framebuffer, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetFramebufferParameterivEXT not available." << std::endl;
}

static api::GLhandleARB fallback_glGetHandleARB(api::GLenum pname)
{
	log() << warning << "GetHandleARB not available." << std::endl;
}

static void fallback_glGetHistogram(api::GLenum target, api::GLboolean reset, api::GLenum format, api::GLenum type, api::GLvoid* values)
{
	log() << warning << "GetHistogram not available." << std::endl;
}

static void fallback_glGetHistogramEXT(api::GLenum target, api::GLboolean reset, api::GLenum format, api::GLenum type, api::GLvoid* values)
{
	log() << warning << "GetHistogramEXT not available." << std::endl;
}

static void fallback_glGetHistogramParameterfv(api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetHistogramParameterfv not available." << std::endl;
}

static void fallback_glGetHistogramParameterfvEXT(api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetHistogramParameterfvEXT not available." << std::endl;
}

static void fallback_glGetHistogramParameteriv(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetHistogramParameteriv not available." << std::endl;
}

static void fallback_glGetHistogramParameterivEXT(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetHistogramParameterivEXT not available." << std::endl;
}

static void fallback_glGetImageTransformParameterfvHP(api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetImageTransformParameterfvHP not available." << std::endl;
}

static void fallback_glGetImageTransformParameterivHP(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetImageTransformParameterivHP not available." << std::endl;
}

static void fallback_glGetInfoLogARB(api::GLhandleARB obj, api::GLsizei maxLength, api::GLsizei* length, api::GLcharARB* infoLog)
{
	log() << warning << "GetInfoLogARB not available." << std::endl;
}

static api::GLint fallback_glGetInstrumentsSGIX()
{
	log() << warning << "GetInstrumentsSGIX not available." << std::endl;
}

static void fallback_glGetInteger64i_v(api::GLenum target, api::GLuint index, api::GLint64* data)
{
	log() << warning << "GetInteger64i_v not available." << std::endl;
}

static void fallback_glGetInteger64v(api::GLenum pname, api::GLint64* params)
{
	log() << warning << "GetInteger64v not available." << std::endl;
}

static void fallback_glGetIntegerIndexedvEXT(api::GLenum target, api::GLuint index, api::GLint* data)
{
	log() << warning << "GetIntegerIndexedvEXT not available." << std::endl;
}

static void fallback_glGetIntegeri_v(api::GLenum target, api::GLuint index, api::GLint* data)
{
	log() << warning << "GetIntegeri_v not available." << std::endl;
}

static void fallback_glGetIntegerui64i_vNV(api::GLenum value, api::GLuint index, api::GLuint64EXT* result)
{
	log() << warning << "GetIntegerui64i_vNV not available." << std::endl;
}

static void fallback_glGetIntegerui64vNV(api::GLenum value, api::GLuint64EXT* result)
{
	log() << warning << "GetIntegerui64vNV not available." << std::endl;
}

static void fallback_glGetIntegerv(api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetIntegerv not available." << std::endl;
}

static void fallback_glGetInvariantBooleanvEXT(api::GLuint id, api::GLenum value, api::GLboolean* data)
{
	log() << warning << "GetInvariantBooleanvEXT not available." << std::endl;
}

static void fallback_glGetInvariantFloatvEXT(api::GLuint id, api::GLenum value, api::GLfloat* data)
{
	log() << warning << "GetInvariantFloatvEXT not available." << std::endl;
}

static void fallback_glGetInvariantIntegervEXT(api::GLuint id, api::GLenum value, api::GLint* data)
{
	log() << warning << "GetInvariantIntegervEXT not available." << std::endl;
}

static void fallback_glGetLightfv(api::GLenum light, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetLightfv not available." << std::endl;
}

static void fallback_glGetLightiv(api::GLenum light, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetLightiv not available." << std::endl;
}

static void fallback_glGetListParameterfvSGIX(api::GLuint list, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetListParameterfvSGIX not available." << std::endl;
}

static void fallback_glGetListParameterivSGIX(api::GLuint list, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetListParameterivSGIX not available." << std::endl;
}

static void fallback_glGetLocalConstantBooleanvEXT(api::GLuint id, api::GLenum value, api::GLboolean* data)
{
	log() << warning << "GetLocalConstantBooleanvEXT not available." << std::endl;
}

static void fallback_glGetLocalConstantFloatvEXT(api::GLuint id, api::GLenum value, api::GLfloat* data)
{
	log() << warning << "GetLocalConstantFloatvEXT not available." << std::endl;
}

static void fallback_glGetLocalConstantIntegervEXT(api::GLuint id, api::GLenum value, api::GLint* data)
{
	log() << warning << "GetLocalConstantIntegervEXT not available." << std::endl;
}

static void fallback_glGetMapAttribParameterfvNV(api::GLenum target, api::GLuint index, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetMapAttribParameterfvNV not available." << std::endl;
}

static void fallback_glGetMapAttribParameterivNV(api::GLenum target, api::GLuint index, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetMapAttribParameterivNV not available." << std::endl;
}

static void fallback_glGetMapControlPointsNV(api::GLenum target, api::GLuint index, api::GLenum type, api::GLsizei ustride, api::GLsizei vstride, api::GLboolean packed, api::GLvoid* points)
{
	log() << warning << "GetMapControlPointsNV not available." << std::endl;
}

static void fallback_glGetMapParameterfvNV(api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetMapParameterfvNV not available." << std::endl;
}

static void fallback_glGetMapParameterivNV(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetMapParameterivNV not available." << std::endl;
}

static void fallback_glGetMapdv(api::GLenum target, api::GLenum query, api::GLdouble* v)
{
	log() << warning << "GetMapdv not available." << std::endl;
}

static void fallback_glGetMapfv(api::GLenum target, api::GLenum query, api::GLfloat* v)
{
	log() << warning << "GetMapfv not available." << std::endl;
}

static void fallback_glGetMapiv(api::GLenum target, api::GLenum query, api::GLint* v)
{
	log() << warning << "GetMapiv not available." << std::endl;
}

static void fallback_glGetMaterialfv(api::GLenum face, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetMaterialfv not available." << std::endl;
}

static void fallback_glGetMaterialiv(api::GLenum face, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetMaterialiv not available." << std::endl;
}

static void fallback_glGetMinmax(api::GLenum target, api::GLboolean reset, api::GLenum format, api::GLenum type, api::GLvoid* values)
{
	log() << warning << "GetMinmax not available." << std::endl;
}

static void fallback_glGetMinmaxEXT(api::GLenum target, api::GLboolean reset, api::GLenum format, api::GLenum type, api::GLvoid* values)
{
	log() << warning << "GetMinmaxEXT not available." << std::endl;
}

static void fallback_glGetMinmaxParameterfv(api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetMinmaxParameterfv not available." << std::endl;
}

static void fallback_glGetMinmaxParameterfvEXT(api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetMinmaxParameterfvEXT not available." << std::endl;
}

static void fallback_glGetMinmaxParameteriv(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetMinmaxParameteriv not available." << std::endl;
}

static void fallback_glGetMinmaxParameterivEXT(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetMinmaxParameterivEXT not available." << std::endl;
}

static void fallback_glGetMultiTexEnvfvEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetMultiTexEnvfvEXT not available." << std::endl;
}

static void fallback_glGetMultiTexEnvivEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetMultiTexEnvivEXT not available." << std::endl;
}

static void fallback_glGetMultiTexGendvEXT(api::GLenum texunit, api::GLenum coord, api::GLenum pname, api::GLdouble* params)
{
	log() << warning << "GetMultiTexGendvEXT not available." << std::endl;
}

static void fallback_glGetMultiTexGenfvEXT(api::GLenum texunit, api::GLenum coord, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetMultiTexGenfvEXT not available." << std::endl;
}

static void fallback_glGetMultiTexGenivEXT(api::GLenum texunit, api::GLenum coord, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetMultiTexGenivEXT not available." << std::endl;
}

static void fallback_glGetMultiTexImageEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLenum format, api::GLenum type, api::GLvoid* pixels)
{
	log() << warning << "GetMultiTexImageEXT not available." << std::endl;
}

static void fallback_glGetMultiTexLevelParameterfvEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetMultiTexLevelParameterfvEXT not available." << std::endl;
}

static void fallback_glGetMultiTexLevelParameterivEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetMultiTexLevelParameterivEXT not available." << std::endl;
}

static void fallback_glGetMultiTexParameterIivEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetMultiTexParameterIivEXT not available." << std::endl;
}

static void fallback_glGetMultiTexParameterIuivEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, api::GLuint* params)
{
	log() << warning << "GetMultiTexParameterIuivEXT not available." << std::endl;
}

static void fallback_glGetMultiTexParameterfvEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetMultiTexParameterfvEXT not available." << std::endl;
}

static void fallback_glGetMultiTexParameterivEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetMultiTexParameterivEXT not available." << std::endl;
}

static void fallback_glGetMultisamplefv(api::GLenum pname, api::GLuint index, api::GLfloat* val)
{
	log() << warning << "GetMultisamplefv not available." << std::endl;
}

static void fallback_glGetMultisamplefvNV(api::GLenum pname, api::GLuint index, api::GLfloat* val)
{
	log() << warning << "GetMultisamplefvNV not available." << std::endl;
}

static void fallback_glGetNamedBufferParameterivEXT(api::GLuint buffer, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetNamedBufferParameterivEXT not available." << std::endl;
}

static void fallback_glGetNamedBufferParameterui64vNV(api::GLuint buffer, api::GLenum pname, api::GLuint64EXT* params)
{
	log() << warning << "GetNamedBufferParameterui64vNV not available." << std::endl;
}

static void fallback_glGetNamedBufferPointervEXT(api::GLuint buffer, api::GLenum pname, api::GLvoid** params)
{
	log() << warning << "GetNamedBufferPointervEXT not available." << std::endl;
}

static void fallback_glGetNamedBufferSubDataEXT(api::GLuint buffer, api::GLintptr offset, api::GLsizeiptr size, api::GLvoid* data)
{
	log() << warning << "GetNamedBufferSubDataEXT not available." << std::endl;
}

static void fallback_glGetNamedFramebufferAttachmentParameterivEXT(api::GLuint framebuffer, api::GLenum attachment, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetNamedFramebufferAttachmentParameterivEXT not available." << std::endl;
}

static void fallback_glGetNamedProgramLocalParameterIivEXT(api::GLuint program, api::GLenum target, api::GLuint index, api::GLint* params)
{
	log() << warning << "GetNamedProgramLocalParameterIivEXT not available." << std::endl;
}

static void fallback_glGetNamedProgramLocalParameterIuivEXT(api::GLuint program, api::GLenum target, api::GLuint index, api::GLuint* params)
{
	log() << warning << "GetNamedProgramLocalParameterIuivEXT not available." << std::endl;
}

static void fallback_glGetNamedProgramLocalParameterdvEXT(api::GLuint program, api::GLenum target, api::GLuint index, api::GLdouble* params)
{
	log() << warning << "GetNamedProgramLocalParameterdvEXT not available." << std::endl;
}

static void fallback_glGetNamedProgramLocalParameterfvEXT(api::GLuint program, api::GLenum target, api::GLuint index, api::GLfloat* params)
{
	log() << warning << "GetNamedProgramLocalParameterfvEXT not available." << std::endl;
}

static void fallback_glGetNamedProgramStringEXT(api::GLuint program, api::GLenum target, api::GLenum pname, api::GLvoid* string)
{
	log() << warning << "GetNamedProgramStringEXT not available." << std::endl;
}

static void fallback_glGetNamedProgramivEXT(api::GLuint program, api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetNamedProgramivEXT not available." << std::endl;
}

static void fallback_glGetNamedRenderbufferParameterivEXT(api::GLuint renderbuffer, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetNamedRenderbufferParameterivEXT not available." << std::endl;
}

static void fallback_glGetNamedStringARB(api::GLint namelen, const api::GLchar* name, api::GLsizei bufSize, api::GLint* stringlen, api::GLchar* string)
{
	log() << warning << "GetNamedStringARB not available." << std::endl;
}

static void fallback_glGetNamedStringivARB(api::GLint namelen, const api::GLchar* name, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetNamedStringivARB not available." << std::endl;
}

static void fallback_glGetObjectBufferfvATI(api::GLuint buffer, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetObjectBufferfvATI not available." << std::endl;
}

static void fallback_glGetObjectBufferivATI(api::GLuint buffer, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetObjectBufferivATI not available." << std::endl;
}

static void fallback_glGetObjectParameterfvARB(api::GLhandleARB obj, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetObjectParameterfvARB not available." << std::endl;
}

static void fallback_glGetObjectParameterivAPPLE(api::GLenum objectType, api::GLuint name, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetObjectParameterivAPPLE not available." << std::endl;
}

static void fallback_glGetObjectParameterivARB(api::GLhandleARB obj, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetObjectParameterivARB not available." << std::endl;
}

static void fallback_glGetOcclusionQueryivNV(api::GLuint id, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetOcclusionQueryivNV not available." << std::endl;
}

static void fallback_glGetOcclusionQueryuivNV(api::GLuint id, api::GLenum pname, api::GLuint* params)
{
	log() << warning << "GetOcclusionQueryuivNV not available." << std::endl;
}

static void fallback_glGetPerfMonitorCounterDataAMD(api::GLuint monitor, api::GLenum pname, api::GLsizei dataSize, api::GLuint* data, api::GLint* bytesWritten)
{
	log() << warning << "GetPerfMonitorCounterDataAMD not available." << std::endl;
}

static void fallback_glGetPerfMonitorCounterInfoAMD(api::GLuint group, api::GLuint counter, api::GLenum pname, void* data)
{
	log() << warning << "GetPerfMonitorCounterInfoAMD not available." << std::endl;
}

static void fallback_glGetPerfMonitorCounterStringAMD(api::GLuint group, api::GLuint counter, api::GLsizei bufSize, api::GLsizei* length, api::GLchar* counterString)
{
	log() << warning << "GetPerfMonitorCounterStringAMD not available." << std::endl;
}

static void fallback_glGetPerfMonitorCountersAMD(api::GLuint group, api::GLint* numCounters, api::GLint* maxActiveCounters, api::GLsizei counterSize, api::GLuint* counters)
{
	log() << warning << "GetPerfMonitorCountersAMD not available." << std::endl;
}

static void fallback_glGetPerfMonitorGroupStringAMD(api::GLuint group, api::GLsizei bufSize, api::GLsizei* length, api::GLchar* groupString)
{
	log() << warning << "GetPerfMonitorGroupStringAMD not available." << std::endl;
}

static void fallback_glGetPerfMonitorGroupsAMD(api::GLint* numGroups, api::GLsizei groupsSize, api::GLuint* groups)
{
	log() << warning << "GetPerfMonitorGroupsAMD not available." << std::endl;
}

static void fallback_glGetPixelMapfv(api::GLenum map, api::GLfloat* values)
{
	log() << warning << "GetPixelMapfv not available." << std::endl;
}

static void fallback_glGetPixelMapuiv(api::GLenum map, api::GLuint* values)
{
	log() << warning << "GetPixelMapuiv not available." << std::endl;
}

static void fallback_glGetPixelMapusv(api::GLenum map, api::GLushort* values)
{
	log() << warning << "GetPixelMapusv not available." << std::endl;
}

static void fallback_glGetPixelTexGenParameterfvSGIS(api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetPixelTexGenParameterfvSGIS not available." << std::endl;
}

static void fallback_glGetPixelTexGenParameterivSGIS(api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetPixelTexGenParameterivSGIS not available." << std::endl;
}

static void fallback_glGetPointerIndexedvEXT(api::GLenum target, api::GLuint index, api::GLvoid** data)
{
	log() << warning << "GetPointerIndexedvEXT not available." << std::endl;
}

static void fallback_glGetPointerv(api::GLenum pname, api::GLvoid** params)
{
	log() << warning << "GetPointerv not available." << std::endl;
}

static void fallback_glGetPointervEXT(api::GLenum pname, api::GLvoid** params)
{
	log() << warning << "GetPointervEXT not available." << std::endl;
}

static void fallback_glGetPolygonStipple(api::GLubyte* mask)
{
	log() << warning << "GetPolygonStipple not available." << std::endl;
}

static void fallback_glGetProgramEnvParameterIivNV(api::GLenum target, api::GLuint index, api::GLint* params)
{
	log() << warning << "GetProgramEnvParameterIivNV not available." << std::endl;
}

static void fallback_glGetProgramEnvParameterIuivNV(api::GLenum target, api::GLuint index, api::GLuint* params)
{
	log() << warning << "GetProgramEnvParameterIuivNV not available." << std::endl;
}

static void fallback_glGetProgramEnvParameterdvARB(api::GLenum target, api::GLuint index, api::GLdouble* params)
{
	log() << warning << "GetProgramEnvParameterdvARB not available." << std::endl;
}

static void fallback_glGetProgramEnvParameterfvARB(api::GLenum target, api::GLuint index, api::GLfloat* params)
{
	log() << warning << "GetProgramEnvParameterfvARB not available." << std::endl;
}

static void fallback_glGetProgramInfoLog(api::GLuint program, api::GLsizei bufSize, api::GLsizei* length, api::GLchar* infoLog)
{
	log() << warning << "GetProgramInfoLog not available." << std::endl;
}

static void fallback_glGetProgramLocalParameterIivNV(api::GLenum target, api::GLuint index, api::GLint* params)
{
	log() << warning << "GetProgramLocalParameterIivNV not available." << std::endl;
}

static void fallback_glGetProgramLocalParameterIuivNV(api::GLenum target, api::GLuint index, api::GLuint* params)
{
	log() << warning << "GetProgramLocalParameterIuivNV not available." << std::endl;
}

static void fallback_glGetProgramLocalParameterdvARB(api::GLenum target, api::GLuint index, api::GLdouble* params)
{
	log() << warning << "GetProgramLocalParameterdvARB not available." << std::endl;
}

static void fallback_glGetProgramLocalParameterfvARB(api::GLenum target, api::GLuint index, api::GLfloat* params)
{
	log() << warning << "GetProgramLocalParameterfvARB not available." << std::endl;
}

static void fallback_glGetProgramNamedParameterdvNV(api::GLuint id, api::GLsizei len, const api::GLubyte* name, api::GLdouble* params)
{
	log() << warning << "GetProgramNamedParameterdvNV not available." << std::endl;
}

static void fallback_glGetProgramNamedParameterfvNV(api::GLuint id, api::GLsizei len, const api::GLubyte* name, api::GLfloat* params)
{
	log() << warning << "GetProgramNamedParameterfvNV not available." << std::endl;
}

static void fallback_glGetProgramParameterdvNV(api::GLenum target, api::GLuint index, api::GLenum pname, api::GLdouble* params)
{
	log() << warning << "GetProgramParameterdvNV not available." << std::endl;
}

static void fallback_glGetProgramParameterfvNV(api::GLenum target, api::GLuint index, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetProgramParameterfvNV not available." << std::endl;
}

static void fallback_glGetProgramStageiv(api::GLuint program, api::GLenum shadertype, api::GLenum pname, api::GLint* values)
{
	log() << warning << "GetProgramStageiv not available." << std::endl;
}

static void fallback_glGetProgramStringARB(api::GLenum target, api::GLenum pname, api::GLvoid* string)
{
	log() << warning << "GetProgramStringARB not available." << std::endl;
}

static void fallback_glGetProgramStringNV(api::GLuint id, api::GLenum pname, api::GLubyte* program)
{
	log() << warning << "GetProgramStringNV not available." << std::endl;
}

static void fallback_glGetProgramiv(api::GLuint program, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetProgramiv not available." << std::endl;
}

static void fallback_glGetProgramivARB(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetProgramivARB not available." << std::endl;
}

static void fallback_glGetProgramivNV(api::GLuint id, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetProgramivNV not available." << std::endl;
}

static void fallback_glGetQueryIndexediv(api::GLenum target, api::GLuint index, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetQueryIndexediv not available." << std::endl;
}

static void fallback_glGetQueryObjecti64v(api::GLuint id, api::GLenum pname, api::GLint64* params)
{
	log() << warning << "GetQueryObjecti64v not available." << std::endl;
}

static void fallback_glGetQueryObjecti64vEXT(api::GLuint id, api::GLenum pname, api::GLint64EXT* params)
{
	log() << warning << "GetQueryObjecti64vEXT not available." << std::endl;
}

static void fallback_glGetQueryObjectiv(api::GLuint id, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetQueryObjectiv not available." << std::endl;
}

static void fallback_glGetQueryObjectivARB(api::GLuint id, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetQueryObjectivARB not available." << std::endl;
}

static void fallback_glGetQueryObjectui64v(api::GLuint id, api::GLenum pname, api::GLuint64* params)
{
	log() << warning << "GetQueryObjectui64v not available." << std::endl;
}

static void fallback_glGetQueryObjectui64vEXT(api::GLuint id, api::GLenum pname, api::GLuint64EXT* params)
{
	log() << warning << "GetQueryObjectui64vEXT not available." << std::endl;
}

static void fallback_glGetQueryObjectuiv(api::GLuint id, api::GLenum pname, api::GLuint* params)
{
	log() << warning << "GetQueryObjectuiv not available." << std::endl;
}

static void fallback_glGetQueryObjectuivARB(api::GLuint id, api::GLenum pname, api::GLuint* params)
{
	log() << warning << "GetQueryObjectuivARB not available." << std::endl;
}

static void fallback_glGetQueryiv(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetQueryiv not available." << std::endl;
}

static void fallback_glGetQueryivARB(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetQueryivARB not available." << std::endl;
}

static void fallback_glGetRenderbufferParameteriv(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetRenderbufferParameteriv not available." << std::endl;
}

static void fallback_glGetRenderbufferParameterivEXT(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetRenderbufferParameterivEXT not available." << std::endl;
}

static void fallback_glGetSamplerParameterIfv(api::GLuint sampler, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetSamplerParameterIfv not available." << std::endl;
}

static void fallback_glGetSamplerParameterIiv(api::GLuint sampler, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetSamplerParameterIiv not available." << std::endl;
}

static void fallback_glGetSamplerParameterfv(api::GLuint sampler, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetSamplerParameterfv not available." << std::endl;
}

static void fallback_glGetSamplerParameteriv(api::GLuint sampler, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetSamplerParameteriv not available." << std::endl;
}

static void fallback_glGetSeparableFilter(api::GLenum target, api::GLenum format, api::GLenum type, api::GLvoid* row, api::GLvoid* column, api::GLvoid* span)
{
	log() << warning << "GetSeparableFilter not available." << std::endl;
}

static void fallback_glGetSeparableFilterEXT(api::GLenum target, api::GLenum format, api::GLenum type, api::GLvoid* row, api::GLvoid* column, api::GLvoid* span)
{
	log() << warning << "GetSeparableFilterEXT not available." << std::endl;
}

static void fallback_glGetShaderInfoLog(api::GLuint shader, api::GLsizei bufSize, api::GLsizei* length, api::GLchar* infoLog)
{
	log() << warning << "GetShaderInfoLog not available." << std::endl;
}

static void fallback_glGetShaderSource(api::GLuint shader, api::GLsizei bufSize, api::GLsizei* length, api::GLchar* source)
{
	log() << warning << "GetShaderSource not available." << std::endl;
}

static void fallback_glGetShaderSourceARB(api::GLhandleARB obj, api::GLsizei maxLength, api::GLsizei* length, api::GLcharARB* source)
{
	log() << warning << "GetShaderSourceARB not available." << std::endl;
}

static void fallback_glGetShaderiv(api::GLuint shader, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetShaderiv not available." << std::endl;
}

static void fallback_glGetSharpenTexFuncSGIS(api::GLenum target, api::GLfloat* points)
{
	log() << warning << "GetSharpenTexFuncSGIS not available." << std::endl;
}

static const api::GLubyte * fallback_glGetString(api::GLenum name)
{
	log() << warning << "GetString not available." << std::endl;
}

static const api::GLubyte * fallback_glGetStringi(api::GLenum name, api::GLuint index)
{
	log() << warning << "GetStringi not available." << std::endl;
}

static api::GLuint fallback_glGetSubroutineIndex(api::GLuint program, api::GLenum shadertype, const api::GLchar* name)
{
	log() << warning << "GetSubroutineIndex not available." << std::endl;
}

static api::GLint fallback_glGetSubroutineUniformLocation(api::GLuint program, api::GLenum shadertype, const api::GLchar* name)
{
	log() << warning << "GetSubroutineUniformLocation not available." << std::endl;
}

static void fallback_glGetSynciv(api::GLsync sync, api::GLenum pname, api::GLsizei bufSize, api::GLsizei* length, api::GLint* values)
{
	log() << warning << "GetSynciv not available." << std::endl;
}

static void fallback_glGetTexBumpParameterfvATI(api::GLenum pname, api::GLfloat* param)
{
	log() << warning << "GetTexBumpParameterfvATI not available." << std::endl;
}

static void fallback_glGetTexBumpParameterivATI(api::GLenum pname, api::GLint* param)
{
	log() << warning << "GetTexBumpParameterivATI not available." << std::endl;
}

static void fallback_glGetTexEnvfv(api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetTexEnvfv not available." << std::endl;
}

static void fallback_glGetTexEnviv(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetTexEnviv not available." << std::endl;
}

static void fallback_glGetTexFilterFuncSGIS(api::GLenum target, api::GLenum filter, api::GLfloat* weights)
{
	log() << warning << "GetTexFilterFuncSGIS not available." << std::endl;
}

static void fallback_glGetTexGendv(api::GLenum coord, api::GLenum pname, api::GLdouble* params)
{
	log() << warning << "GetTexGendv not available." << std::endl;
}

static void fallback_glGetTexGenfv(api::GLenum coord, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetTexGenfv not available." << std::endl;
}

static void fallback_glGetTexGeniv(api::GLenum coord, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetTexGeniv not available." << std::endl;
}

static void fallback_glGetTexImage(api::GLenum target, api::GLint level, api::GLenum format, api::GLenum type, api::GLvoid* pixels)
{
	log() << warning << "GetTexImage not available." << std::endl;
}

static void fallback_glGetTexLevelParameterfv(api::GLenum target, api::GLint level, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetTexLevelParameterfv not available." << std::endl;
}

static void fallback_glGetTexLevelParameteriv(api::GLenum target, api::GLint level, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetTexLevelParameteriv not available." << std::endl;
}

static void fallback_glGetTexParameterIiv(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetTexParameterIiv not available." << std::endl;
}

static void fallback_glGetTexParameterIivEXT(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetTexParameterIivEXT not available." << std::endl;
}

static void fallback_glGetTexParameterIuiv(api::GLenum target, api::GLenum pname, api::GLuint* params)
{
	log() << warning << "GetTexParameterIuiv not available." << std::endl;
}

static void fallback_glGetTexParameterIuivEXT(api::GLenum target, api::GLenum pname, api::GLuint* params)
{
	log() << warning << "GetTexParameterIuivEXT not available." << std::endl;
}

static void fallback_glGetTexParameterPointervAPPLE(api::GLenum target, api::GLenum pname, api::GLvoid** params)
{
	log() << warning << "GetTexParameterPointervAPPLE not available." << std::endl;
}

static void fallback_glGetTexParameterfv(api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetTexParameterfv not available." << std::endl;
}

static void fallback_glGetTexParameteriv(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetTexParameteriv not available." << std::endl;
}

static void fallback_glGetTextureImageEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLenum format, api::GLenum type, api::GLvoid* pixels)
{
	log() << warning << "GetTextureImageEXT not available." << std::endl;
}

static void fallback_glGetTextureLevelParameterfvEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetTextureLevelParameterfvEXT not available." << std::endl;
}

static void fallback_glGetTextureLevelParameterivEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetTextureLevelParameterivEXT not available." << std::endl;
}

static void fallback_glGetTextureParameterIivEXT(api::GLuint texture, api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetTextureParameterIivEXT not available." << std::endl;
}

static void fallback_glGetTextureParameterIuivEXT(api::GLuint texture, api::GLenum target, api::GLenum pname, api::GLuint* params)
{
	log() << warning << "GetTextureParameterIuivEXT not available." << std::endl;
}

static void fallback_glGetTextureParameterfvEXT(api::GLuint texture, api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetTextureParameterfvEXT not available." << std::endl;
}

static void fallback_glGetTextureParameterivEXT(api::GLuint texture, api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetTextureParameterivEXT not available." << std::endl;
}

static void fallback_glGetTrackMatrixivNV(api::GLenum target, api::GLuint address, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetTrackMatrixivNV not available." << std::endl;
}

static void fallback_glGetTransformFeedbackVarying(api::GLuint program, api::GLuint index, api::GLsizei bufSize, api::GLsizei* length, api::GLsizei* size, api::GLenum* type, api::GLchar* name)
{
	log() << warning << "GetTransformFeedbackVarying not available." << std::endl;
}

static void fallback_glGetTransformFeedbackVaryingEXT(api::GLuint program, api::GLuint index, api::GLsizei bufSize, api::GLsizei* length, api::GLsizei* size, api::GLenum* type, api::GLchar* name)
{
	log() << warning << "GetTransformFeedbackVaryingEXT not available." << std::endl;
}

static void fallback_glGetTransformFeedbackVaryingNV(api::GLuint program, api::GLuint index, api::GLint* location)
{
	log() << warning << "GetTransformFeedbackVaryingNV not available." << std::endl;
}

static api::GLuint fallback_glGetUniformBlockIndex(api::GLuint program, const api::GLchar* uniformBlockName)
{
	log() << warning << "GetUniformBlockIndex not available." << std::endl;
}

static api::GLint fallback_glGetUniformBufferSizeEXT(api::GLuint program, api::GLint location)
{
	log() << warning << "GetUniformBufferSizeEXT not available." << std::endl;
}

static void fallback_glGetUniformIndices(api::GLuint program, api::GLsizei uniformCount, const api::GLchar** uniformNames, api::GLuint* uniformIndices)
{
	log() << warning << "GetUniformIndices not available." << std::endl;
}

static api::GLint fallback_glGetUniformLocation(api::GLuint program, const api::GLchar* name)
{
	log() << warning << "GetUniformLocation not available." << std::endl;
}

static api::GLint fallback_glGetUniformLocationARB(api::GLhandleARB programObj, const api::GLcharARB* name)
{
	log() << warning << "GetUniformLocationARB not available." << std::endl;
}

static api::GLintptr fallback_glGetUniformOffsetEXT(api::GLuint program, api::GLint location)
{
	log() << warning << "GetUniformOffsetEXT not available." << std::endl;
}

static void fallback_glGetUniformSubroutineuiv(api::GLenum shadertype, api::GLint location, api::GLuint* params)
{
	log() << warning << "GetUniformSubroutineuiv not available." << std::endl;
}

static void fallback_glGetUniformdv(api::GLuint program, api::GLint location, api::GLdouble* params)
{
	log() << warning << "GetUniformdv not available." << std::endl;
}

static void fallback_glGetUniformfv(api::GLuint program, api::GLint location, api::GLfloat* params)
{
	log() << warning << "GetUniformfv not available." << std::endl;
}

static void fallback_glGetUniformfvARB(api::GLhandleARB programObj, api::GLint location, api::GLfloat* params)
{
	log() << warning << "GetUniformfvARB not available." << std::endl;
}

static void fallback_glGetUniformiv(api::GLuint program, api::GLint location, api::GLint* params)
{
	log() << warning << "GetUniformiv not available." << std::endl;
}

static void fallback_glGetUniformivARB(api::GLhandleARB programObj, api::GLint location, api::GLint* params)
{
	log() << warning << "GetUniformivARB not available." << std::endl;
}

static void fallback_glGetUniformui64vNV(api::GLuint program, api::GLint location, api::GLuint64EXT* params)
{
	log() << warning << "GetUniformui64vNV not available." << std::endl;
}

static void fallback_glGetUniformuiv(api::GLuint program, api::GLint location, api::GLuint* params)
{
	log() << warning << "GetUniformuiv not available." << std::endl;
}

static void fallback_glGetUniformuivEXT(api::GLuint program, api::GLint location, api::GLuint* params)
{
	log() << warning << "GetUniformuivEXT not available." << std::endl;
}

static void fallback_glGetVariantArrayObjectfvATI(api::GLuint id, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetVariantArrayObjectfvATI not available." << std::endl;
}

static void fallback_glGetVariantArrayObjectivATI(api::GLuint id, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetVariantArrayObjectivATI not available." << std::endl;
}

static void fallback_glGetVariantBooleanvEXT(api::GLuint id, api::GLenum value, api::GLboolean* data)
{
	log() << warning << "GetVariantBooleanvEXT not available." << std::endl;
}

static void fallback_glGetVariantFloatvEXT(api::GLuint id, api::GLenum value, api::GLfloat* data)
{
	log() << warning << "GetVariantFloatvEXT not available." << std::endl;
}

static void fallback_glGetVariantIntegervEXT(api::GLuint id, api::GLenum value, api::GLint* data)
{
	log() << warning << "GetVariantIntegervEXT not available." << std::endl;
}

static void fallback_glGetVariantPointervEXT(api::GLuint id, api::GLenum value, api::GLvoid** data)
{
	log() << warning << "GetVariantPointervEXT not available." << std::endl;
}

static api::GLint fallback_glGetVaryingLocationNV(api::GLuint program, const api::GLchar* name)
{
	log() << warning << "GetVaryingLocationNV not available." << std::endl;
}

static void fallback_glGetVertexAttribArrayObjectfvATI(api::GLuint index, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetVertexAttribArrayObjectfvATI not available." << std::endl;
}

static void fallback_glGetVertexAttribArrayObjectivATI(api::GLuint index, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetVertexAttribArrayObjectivATI not available." << std::endl;
}

static void fallback_glGetVertexAttribIiv(api::GLuint index, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetVertexAttribIiv not available." << std::endl;
}

static void fallback_glGetVertexAttribIivEXT(api::GLuint index, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetVertexAttribIivEXT not available." << std::endl;
}

static void fallback_glGetVertexAttribIuiv(api::GLuint index, api::GLenum pname, api::GLuint* params)
{
	log() << warning << "GetVertexAttribIuiv not available." << std::endl;
}

static void fallback_glGetVertexAttribIuivEXT(api::GLuint index, api::GLenum pname, api::GLuint* params)
{
	log() << warning << "GetVertexAttribIuivEXT not available." << std::endl;
}

static void fallback_glGetVertexAttribPointerv(api::GLuint index, api::GLenum pname, api::GLvoid** pointer)
{
	log() << warning << "GetVertexAttribPointerv not available." << std::endl;
}

static void fallback_glGetVertexAttribPointervARB(api::GLuint index, api::GLenum pname, api::GLvoid** pointer)
{
	log() << warning << "GetVertexAttribPointervARB not available." << std::endl;
}

static void fallback_glGetVertexAttribPointervNV(api::GLuint index, api::GLenum pname, api::GLvoid** pointer)
{
	log() << warning << "GetVertexAttribPointervNV not available." << std::endl;
}

static void fallback_glGetVertexAttribdv(api::GLuint index, api::GLenum pname, api::GLdouble* params)
{
	log() << warning << "GetVertexAttribdv not available." << std::endl;
}

static void fallback_glGetVertexAttribdvARB(api::GLuint index, api::GLenum pname, api::GLdouble* params)
{
	log() << warning << "GetVertexAttribdvARB not available." << std::endl;
}

static void fallback_glGetVertexAttribdvNV(api::GLuint index, api::GLenum pname, api::GLdouble* params)
{
	log() << warning << "GetVertexAttribdvNV not available." << std::endl;
}

static void fallback_glGetVertexAttribfv(api::GLuint index, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetVertexAttribfv not available." << std::endl;
}

static void fallback_glGetVertexAttribfvARB(api::GLuint index, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetVertexAttribfvARB not available." << std::endl;
}

static void fallback_glGetVertexAttribfvNV(api::GLuint index, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetVertexAttribfvNV not available." << std::endl;
}

static void fallback_glGetVertexAttribiv(api::GLuint index, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetVertexAttribiv not available." << std::endl;
}

static void fallback_glGetVertexAttribivARB(api::GLuint index, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetVertexAttribivARB not available." << std::endl;
}

static void fallback_glGetVertexAttribivNV(api::GLuint index, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetVertexAttribivNV not available." << std::endl;
}

static void fallback_glGetVideoCaptureStreamdvNV(api::GLuint video_capture_slot, api::GLuint stream, api::GLenum pname, api::GLdouble* params)
{
	log() << warning << "GetVideoCaptureStreamdvNV not available." << std::endl;
}

static void fallback_glGetVideoCaptureStreamfvNV(api::GLuint video_capture_slot, api::GLuint stream, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetVideoCaptureStreamfvNV not available." << std::endl;
}

static void fallback_glGetVideoCaptureStreamivNV(api::GLuint video_capture_slot, api::GLuint stream, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetVideoCaptureStreamivNV not available." << std::endl;
}

static void fallback_glGetVideoCaptureivNV(api::GLuint video_capture_slot, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetVideoCaptureivNV not available." << std::endl;
}

static void fallback_glGetVideoi64vNV(api::GLuint video_slot, api::GLenum pname, api::GLint64EXT* params)
{
	log() << warning << "GetVideoi64vNV not available." << std::endl;
}

static void fallback_glGetVideoivNV(api::GLuint video_slot, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetVideoivNV not available." << std::endl;
}

static void fallback_glGetVideoui64vNV(api::GLuint video_slot, api::GLenum pname, api::GLuint64EXT* params)
{
	log() << warning << "GetVideoui64vNV not available." << std::endl;
}

static void fallback_glGetVideouivNV(api::GLuint video_slot, api::GLenum pname, api::GLuint* params)
{
	log() << warning << "GetVideouivNV not available." << std::endl;
}

static void fallback_glGlobalAlphaFactorbSUN(api::GLbyte factor)
{
	log() << warning << "GlobalAlphaFactorbSUN not available." << std::endl;
}

static void fallback_glGlobalAlphaFactordSUN(api::GLdouble factor)
{
	log() << warning << "GlobalAlphaFactordSUN not available." << std::endl;
}

static void fallback_glGlobalAlphaFactorfSUN(api::GLfloat factor)
{
	log() << warning << "GlobalAlphaFactorfSUN not available." << std::endl;
}

static void fallback_glGlobalAlphaFactoriSUN(api::GLint factor)
{
	log() << warning << "GlobalAlphaFactoriSUN not available." << std::endl;
}

static void fallback_glGlobalAlphaFactorsSUN(api::GLshort factor)
{
	log() << warning << "GlobalAlphaFactorsSUN not available." << std::endl;
}

static void fallback_glGlobalAlphaFactorubSUN(api::GLubyte factor)
{
	log() << warning << "GlobalAlphaFactorubSUN not available." << std::endl;
}

static void fallback_glGlobalAlphaFactoruiSUN(api::GLuint factor)
{
	log() << warning << "GlobalAlphaFactoruiSUN not available." << std::endl;
}

static void fallback_glGlobalAlphaFactorusSUN(api::GLushort factor)
{
	log() << warning << "GlobalAlphaFactorusSUN not available." << std::endl;
}

static void fallback_glHint(api::GLenum target, api::GLenum mode)
{
	log() << warning << "Hint not available." << std::endl;
}

static void fallback_glHintPGI(api::GLenum target, api::GLint mode)
{
	log() << warning << "HintPGI not available." << std::endl;
}

static void fallback_glHistogram(api::GLenum target, api::GLsizei width, api::GLenum internalformat, api::GLboolean sink)
{
	log() << warning << "Histogram not available." << std::endl;
}

static void fallback_glHistogramEXT(api::GLenum target, api::GLsizei width, api::GLenum internalformat, api::GLboolean sink)
{
	log() << warning << "HistogramEXT not available." << std::endl;
}

static void fallback_glIglooInterfaceSGIX(api::GLenum pname, const api::GLvoid* params)
{
	log() << warning << "IglooInterfaceSGIX not available." << std::endl;
}

static void fallback_glImageTransformParameterfHP(api::GLenum target, api::GLenum pname, api::GLfloat param)
{
	log() << warning << "ImageTransformParameterfHP not available." << std::endl;
}

static void fallback_glImageTransformParameterfvHP(api::GLenum target, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "ImageTransformParameterfvHP not available." << std::endl;
}

static void fallback_glImageTransformParameteriHP(api::GLenum target, api::GLenum pname, api::GLint param)
{
	log() << warning << "ImageTransformParameteriHP not available." << std::endl;
}

static void fallback_glImageTransformParameterivHP(api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "ImageTransformParameterivHP not available." << std::endl;
}

static void fallback_glIndexFormatNV(api::GLenum type, api::GLsizei stride)
{
	log() << warning << "IndexFormatNV not available." << std::endl;
}

static void fallback_glIndexFuncEXT(api::GLenum func, api::GLclampf ref)
{
	log() << warning << "IndexFuncEXT not available." << std::endl;
}

static void fallback_glIndexMask(api::GLuint mask)
{
	log() << warning << "IndexMask not available." << std::endl;
}

static void fallback_glIndexMaterialEXT(api::GLenum face, api::GLenum mode)
{
	log() << warning << "IndexMaterialEXT not available." << std::endl;
}

static void fallback_glIndexPointer(api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "IndexPointer not available." << std::endl;
}

static void fallback_glIndexPointerEXT(api::GLenum type, api::GLsizei stride, api::GLsizei count, const api::GLvoid* pointer)
{
	log() << warning << "IndexPointerEXT not available." << std::endl;
}

static void fallback_glIndexPointerListIBM(api::GLenum type, api::GLint stride, const api::GLvoid** pointer, api::GLint ptrstride)
{
	log() << warning << "IndexPointerListIBM not available." << std::endl;
}

static void fallback_glIndexd(api::GLdouble c)
{
	log() << warning << "Indexd not available." << std::endl;
}

static void fallback_glIndexdv(const api::GLdouble* c)
{
	log() << warning << "Indexdv not available." << std::endl;
}

static void fallback_glIndexf(api::GLfloat c)
{
	log() << warning << "Indexf not available." << std::endl;
}

static void fallback_glIndexfv(const api::GLfloat* c)
{
	log() << warning << "Indexfv not available." << std::endl;
}

static void fallback_glIndexi(api::GLint c)
{
	log() << warning << "Indexi not available." << std::endl;
}

static void fallback_glIndexiv(const api::GLint* c)
{
	log() << warning << "Indexiv not available." << std::endl;
}

static void fallback_glIndexs(api::GLshort c)
{
	log() << warning << "Indexs not available." << std::endl;
}

static void fallback_glIndexsv(const api::GLshort* c)
{
	log() << warning << "Indexsv not available." << std::endl;
}

static void fallback_glIndexub(api::GLubyte c)
{
	log() << warning << "Indexub not available." << std::endl;
}

static void fallback_glIndexubv(const api::GLubyte* c)
{
	log() << warning << "Indexubv not available." << std::endl;
}

static void fallback_glInitNames()
{
	log() << warning << "InitNames not available." << std::endl;
}

static void fallback_glInsertComponentEXT(api::GLuint res, api::GLuint src, api::GLuint num)
{
	log() << warning << "InsertComponentEXT not available." << std::endl;
}

static void fallback_glInstrumentsBufferSGIX(api::GLsizei size, api::GLint* buffer)
{
	log() << warning << "InstrumentsBufferSGIX not available." << std::endl;
}

static void fallback_glInterleavedArrays(api::GLenum format, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "InterleavedArrays not available." << std::endl;
}

static api::GLboolean fallback_glIsAsyncMarkerSGIX(api::GLuint marker)
{
	log() << warning << "IsAsyncMarkerSGIX not available." << std::endl;
}

static api::GLboolean fallback_glIsBuffer(api::GLuint buffer)
{
	log() << warning << "IsBuffer not available." << std::endl;
}

static api::GLboolean fallback_glIsBufferARB(api::GLuint buffer)
{
	log() << warning << "IsBufferARB not available." << std::endl;
}

static api::GLboolean fallback_glIsBufferResidentNV(api::GLenum target)
{
	log() << warning << "IsBufferResidentNV not available." << std::endl;
}

static api::GLboolean fallback_glIsEnabled(api::GLenum cap)
{
	log() << warning << "IsEnabled not available." << std::endl;
}

static api::GLboolean fallback_glIsEnabledIndexedEXT(api::GLenum target, api::GLuint index)
{
	log() << warning << "IsEnabledIndexedEXT not available." << std::endl;
}

static api::GLboolean fallback_glIsEnabledi(api::GLenum target, api::GLuint index)
{
	log() << warning << "IsEnabledi not available." << std::endl;
}

static api::GLboolean fallback_glIsFenceAPPLE(api::GLuint fence)
{
	log() << warning << "IsFenceAPPLE not available." << std::endl;
}

static api::GLboolean fallback_glIsFenceNV(api::GLuint fence)
{
	log() << warning << "IsFenceNV not available." << std::endl;
}

static api::GLboolean fallback_glIsFramebuffer(api::GLuint framebuffer)
{
	log() << warning << "IsFramebuffer not available." << std::endl;
}

static api::GLboolean fallback_glIsFramebufferEXT(api::GLuint framebuffer)
{
	log() << warning << "IsFramebufferEXT not available." << std::endl;
}

static api::GLboolean fallback_glIsList(api::GLuint list)
{
	log() << warning << "IsList not available." << std::endl;
}

static api::GLboolean fallback_glIsNamedBufferResidentNV(api::GLuint buffer)
{
	log() << warning << "IsNamedBufferResidentNV not available." << std::endl;
}

static api::GLboolean fallback_glIsNamedStringARB(api::GLint namelen, const api::GLchar* name)
{
	log() << warning << "IsNamedStringARB not available." << std::endl;
}

static api::GLboolean fallback_glIsObjectBufferATI(api::GLuint buffer)
{
	log() << warning << "IsObjectBufferATI not available." << std::endl;
}

static api::GLboolean fallback_glIsOcclusionQueryNV(api::GLuint id)
{
	log() << warning << "IsOcclusionQueryNV not available." << std::endl;
}

static api::GLboolean fallback_glIsProgram(api::GLuint program)
{
	log() << warning << "IsProgram not available." << std::endl;
}

static api::GLboolean fallback_glIsProgramARB(api::GLuint program)
{
	log() << warning << "IsProgramARB not available." << std::endl;
}

static api::GLboolean fallback_glIsProgramNV(api::GLuint id)
{
	log() << warning << "IsProgramNV not available." << std::endl;
}

static api::GLboolean fallback_glIsQuery(api::GLuint id)
{
	log() << warning << "IsQuery not available." << std::endl;
}

static api::GLboolean fallback_glIsQueryARB(api::GLuint id)
{
	log() << warning << "IsQueryARB not available." << std::endl;
}

static api::GLboolean fallback_glIsRenderbuffer(api::GLuint renderbuffer)
{
	log() << warning << "IsRenderbuffer not available." << std::endl;
}

static api::GLboolean fallback_glIsRenderbufferEXT(api::GLuint renderbuffer)
{
	log() << warning << "IsRenderbufferEXT not available." << std::endl;
}

static api::GLboolean fallback_glIsSampler(api::GLuint sampler)
{
	log() << warning << "IsSampler not available." << std::endl;
}

static api::GLboolean fallback_glIsShader(api::GLuint shader)
{
	log() << warning << "IsShader not available." << std::endl;
}

static api::GLboolean fallback_glIsSync(api::GLsync sync)
{
	log() << warning << "IsSync not available." << std::endl;
}

static api::GLboolean fallback_glIsTexture(api::GLuint texture)
{
	log() << warning << "IsTexture not available." << std::endl;
}

static api::GLboolean fallback_glIsTextureEXT(api::GLuint texture)
{
	log() << warning << "IsTextureEXT not available." << std::endl;
}

static api::GLboolean fallback_glIsTransformFeedback(api::GLuint id)
{
	log() << warning << "IsTransformFeedback not available." << std::endl;
}

static api::GLboolean fallback_glIsTransformFeedbackNV(api::GLuint id)
{
	log() << warning << "IsTransformFeedbackNV not available." << std::endl;
}

static api::GLboolean fallback_glIsVariantEnabledEXT(api::GLuint id, api::GLenum cap)
{
	log() << warning << "IsVariantEnabledEXT not available." << std::endl;
}

static api::GLboolean fallback_glIsVertexArray(api::GLuint array)
{
	log() << warning << "IsVertexArray not available." << std::endl;
}

static api::GLboolean fallback_glIsVertexArrayAPPLE(api::GLuint array)
{
	log() << warning << "IsVertexArrayAPPLE not available." << std::endl;
}

static api::GLboolean fallback_glIsVertexAttribEnabledAPPLE(api::GLuint index, api::GLenum pname)
{
	log() << warning << "IsVertexAttribEnabledAPPLE not available." << std::endl;
}

static void fallback_glLightEnviSGIX(api::GLenum pname, api::GLint param)
{
	log() << warning << "LightEnviSGIX not available." << std::endl;
}

static void fallback_glLightModelf(api::GLenum pname, api::GLfloat param)
{
	log() << warning << "LightModelf not available." << std::endl;
}

static void fallback_glLightModelfv(api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "LightModelfv not available." << std::endl;
}

static void fallback_glLightModeli(api::GLenum pname, api::GLint param)
{
	log() << warning << "LightModeli not available." << std::endl;
}

static void fallback_glLightModeliv(api::GLenum pname, const api::GLint* params)
{
	log() << warning << "LightModeliv not available." << std::endl;
}

static void fallback_glLightf(api::GLenum light, api::GLenum pname, api::GLfloat param)
{
	log() << warning << "Lightf not available." << std::endl;
}

static void fallback_glLightfv(api::GLenum light, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "Lightfv not available." << std::endl;
}

static void fallback_glLighti(api::GLenum light, api::GLenum pname, api::GLint param)
{
	log() << warning << "Lighti not available." << std::endl;
}

static void fallback_glLightiv(api::GLenum light, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "Lightiv not available." << std::endl;
}

static void fallback_glLineStipple(api::GLint factor, api::GLushort pattern)
{
	log() << warning << "LineStipple not available." << std::endl;
}

static void fallback_glLineWidth(api::GLfloat width)
{
	log() << warning << "LineWidth not available." << std::endl;
}

static void fallback_glLinkProgram(api::GLuint program)
{
	log() << warning << "LinkProgram not available." << std::endl;
}

static void fallback_glLinkProgramARB(api::GLhandleARB programObj)
{
	log() << warning << "LinkProgramARB not available." << std::endl;
}

static void fallback_glListBase(api::GLuint base)
{
	log() << warning << "ListBase not available." << std::endl;
}

static void fallback_glListParameterfSGIX(api::GLuint list, api::GLenum pname, api::GLfloat param)
{
	log() << warning << "ListParameterfSGIX not available." << std::endl;
}

static void fallback_glListParameterfvSGIX(api::GLuint list, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "ListParameterfvSGIX not available." << std::endl;
}

static void fallback_glListParameteriSGIX(api::GLuint list, api::GLenum pname, api::GLint param)
{
	log() << warning << "ListParameteriSGIX not available." << std::endl;
}

static void fallback_glListParameterivSGIX(api::GLuint list, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "ListParameterivSGIX not available." << std::endl;
}

static void fallback_glLoadIdentity()
{
	log() << warning << "LoadIdentity not available." << std::endl;
}

static void fallback_glLoadIdentityDeformationMapSGIX(api::GLbitfield mask)
{
	log() << warning << "LoadIdentityDeformationMapSGIX not available." << std::endl;
}

static void fallback_glLoadMatrixd(const api::GLdouble* m)
{
	log() << warning << "LoadMatrixd not available." << std::endl;
}

static void fallback_glLoadMatrixf(const api::GLfloat* m)
{
	log() << warning << "LoadMatrixf not available." << std::endl;
}

static void fallback_glLoadName(api::GLuint name)
{
	log() << warning << "LoadName not available." << std::endl;
}

static void fallback_glLoadProgramNV(api::GLenum target, api::GLuint id, api::GLsizei len, const api::GLubyte* program)
{
	log() << warning << "LoadProgramNV not available." << std::endl;
}

static void fallback_glLoadTransposeMatrixd(const api::GLdouble* m)
{
	log() << warning << "LoadTransposeMatrixd not available." << std::endl;
}

static void fallback_glLoadTransposeMatrixdARB(const api::GLdouble* m)
{
	log() << warning << "LoadTransposeMatrixdARB not available." << std::endl;
}

static void fallback_glLoadTransposeMatrixf(const api::GLfloat* m)
{
	log() << warning << "LoadTransposeMatrixf not available." << std::endl;
}

static void fallback_glLoadTransposeMatrixfARB(const api::GLfloat* m)
{
	log() << warning << "LoadTransposeMatrixfARB not available." << std::endl;
}

static void fallback_glLockArraysEXT(api::GLint first, api::GLsizei count)
{
	log() << warning << "LockArraysEXT not available." << std::endl;
}

static void fallback_glLogicOp(api::GLenum opcode)
{
	log() << warning << "LogicOp not available." << std::endl;
}

static void fallback_glMakeBufferNonResidentNV(api::GLenum target)
{
	log() << warning << "MakeBufferNonResidentNV not available." << std::endl;
}

static void fallback_glMakeBufferResidentNV(api::GLenum target, api::GLenum access)
{
	log() << warning << "MakeBufferResidentNV not available." << std::endl;
}

static void fallback_glMakeNamedBufferNonResidentNV(api::GLuint buffer)
{
	log() << warning << "MakeNamedBufferNonResidentNV not available." << std::endl;
}

static void fallback_glMakeNamedBufferResidentNV(api::GLuint buffer, api::GLenum access)
{
	log() << warning << "MakeNamedBufferResidentNV not available." << std::endl;
}

static void fallback_glMap1d(api::GLenum target, api::GLdouble u1, api::GLdouble u2, api::GLint stride, api::GLint order, const api::GLdouble* points)
{
	log() << warning << "Map1d not available." << std::endl;
}

static void fallback_glMap1f(api::GLenum target, api::GLfloat u1, api::GLfloat u2, api::GLint stride, api::GLint order, const api::GLfloat* points)
{
	log() << warning << "Map1f not available." << std::endl;
}

static void fallback_glMap2d(api::GLenum target, api::GLdouble u1, api::GLdouble u2, api::GLint ustride, api::GLint uorder, api::GLdouble v1, api::GLdouble v2, api::GLint vstride, api::GLint vorder, const api::GLdouble* points)
{
	log() << warning << "Map2d not available." << std::endl;
}

static void fallback_glMap2f(api::GLenum target, api::GLfloat u1, api::GLfloat u2, api::GLint ustride, api::GLint uorder, api::GLfloat v1, api::GLfloat v2, api::GLint vstride, api::GLint vorder, const api::GLfloat* points)
{
	log() << warning << "Map2f not available." << std::endl;
}

static api::GLvoid* fallback_glMapBuffer(api::GLenum target, api::GLenum access)
{
	log() << warning << "MapBuffer not available." << std::endl;
}

static api::GLvoid* fallback_glMapBufferARB(api::GLenum target, api::GLenum access)
{
	log() << warning << "MapBufferARB not available." << std::endl;
}

static api::GLvoid* fallback_glMapBufferRange(api::GLenum target, api::GLintptr offset, api::GLsizeiptr length, api::GLbitfield access)
{
	log() << warning << "MapBufferRange not available." << std::endl;
}

static void fallback_glMapControlPointsNV(api::GLenum target, api::GLuint index, api::GLenum type, api::GLsizei ustride, api::GLsizei vstride, api::GLint uorder, api::GLint vorder, api::GLboolean packed, const api::GLvoid* points)
{
	log() << warning << "MapControlPointsNV not available." << std::endl;
}

static void fallback_glMapGrid1d(api::GLint un, api::GLdouble u1, api::GLdouble u2)
{
	log() << warning << "MapGrid1d not available." << std::endl;
}

static void fallback_glMapGrid1f(api::GLint un, api::GLfloat u1, api::GLfloat u2)
{
	log() << warning << "MapGrid1f not available." << std::endl;
}

static void fallback_glMapGrid2d(api::GLint un, api::GLdouble u1, api::GLdouble u2, api::GLint vn, api::GLdouble v1, api::GLdouble v2)
{
	log() << warning << "MapGrid2d not available." << std::endl;
}

static void fallback_glMapGrid2f(api::GLint un, api::GLfloat u1, api::GLfloat u2, api::GLint vn, api::GLfloat v1, api::GLfloat v2)
{
	log() << warning << "MapGrid2f not available." << std::endl;
}

static api::GLvoid* fallback_glMapNamedBufferEXT(api::GLuint buffer, api::GLenum access)
{
	log() << warning << "MapNamedBufferEXT not available." << std::endl;
}

static api::GLvoid* fallback_glMapObjectBufferATI(api::GLuint buffer)
{
	log() << warning << "MapObjectBufferATI not available." << std::endl;
}

static void fallback_glMapParameterfvNV(api::GLenum target, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "MapParameterfvNV not available." << std::endl;
}

static void fallback_glMapParameterivNV(api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "MapParameterivNV not available." << std::endl;
}

static void fallback_glMapVertexAttrib1dAPPLE(api::GLuint index, api::GLuint size, api::GLdouble u1, api::GLdouble u2, api::GLint stride, api::GLint order, const api::GLdouble* points)
{
	log() << warning << "MapVertexAttrib1dAPPLE not available." << std::endl;
}

static void fallback_glMapVertexAttrib1fAPPLE(api::GLuint index, api::GLuint size, api::GLfloat u1, api::GLfloat u2, api::GLint stride, api::GLint order, const api::GLfloat* points)
{
	log() << warning << "MapVertexAttrib1fAPPLE not available." << std::endl;
}

static void fallback_glMapVertexAttrib2dAPPLE(api::GLuint index, api::GLuint size, api::GLdouble u1, api::GLdouble u2, api::GLint ustride, api::GLint uorder, api::GLdouble v1, api::GLdouble v2, api::GLint vstride, api::GLint vorder, const api::GLdouble* points)
{
	log() << warning << "MapVertexAttrib2dAPPLE not available." << std::endl;
}

static void fallback_glMapVertexAttrib2fAPPLE(api::GLuint index, api::GLuint size, api::GLfloat u1, api::GLfloat u2, api::GLint ustride, api::GLint uorder, api::GLfloat v1, api::GLfloat v2, api::GLint vstride, api::GLint vorder, const api::GLfloat* points)
{
	log() << warning << "MapVertexAttrib2fAPPLE not available." << std::endl;
}

static void fallback_glMaterialf(api::GLenum face, api::GLenum pname, api::GLfloat param)
{
	log() << warning << "Materialf not available." << std::endl;
}

static void fallback_glMaterialfv(api::GLenum face, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "Materialfv not available." << std::endl;
}

static void fallback_glMateriali(api::GLenum face, api::GLenum pname, api::GLint param)
{
	log() << warning << "Materiali not available." << std::endl;
}

static void fallback_glMaterialiv(api::GLenum face, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "Materialiv not available." << std::endl;
}

static void fallback_glMatrixFrustumEXT(api::GLenum mode, api::GLdouble left, api::GLdouble right, api::GLdouble bottom, api::GLdouble top, api::GLdouble zNear, api::GLdouble zFar)
{
	log() << warning << "MatrixFrustumEXT not available." << std::endl;
}

static void fallback_glMatrixIndexPointerARB(api::GLint size, api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "MatrixIndexPointerARB not available." << std::endl;
}

static void fallback_glMatrixIndexubvARB(api::GLint size, const api::GLubyte* indices)
{
	log() << warning << "MatrixIndexubvARB not available." << std::endl;
}

static void fallback_glMatrixIndexuivARB(api::GLint size, const api::GLuint* indices)
{
	log() << warning << "MatrixIndexuivARB not available." << std::endl;
}

static void fallback_glMatrixIndexusvARB(api::GLint size, const api::GLushort* indices)
{
	log() << warning << "MatrixIndexusvARB not available." << std::endl;
}

static void fallback_glMatrixLoadIdentityEXT(api::GLenum mode)
{
	log() << warning << "MatrixLoadIdentityEXT not available." << std::endl;
}

static void fallback_glMatrixLoadTransposedEXT(api::GLenum mode, const api::GLdouble* m)
{
	log() << warning << "MatrixLoadTransposedEXT not available." << std::endl;
}

static void fallback_glMatrixLoadTransposefEXT(api::GLenum mode, const api::GLfloat* m)
{
	log() << warning << "MatrixLoadTransposefEXT not available." << std::endl;
}

static void fallback_glMatrixLoaddEXT(api::GLenum mode, const api::GLdouble* m)
{
	log() << warning << "MatrixLoaddEXT not available." << std::endl;
}

static void fallback_glMatrixLoadfEXT(api::GLenum mode, const api::GLfloat* m)
{
	log() << warning << "MatrixLoadfEXT not available." << std::endl;
}

static void fallback_glMatrixMode(api::GLenum mode)
{
	log() << warning << "MatrixMode not available." << std::endl;
}

static void fallback_glMatrixMultTransposedEXT(api::GLenum mode, const api::GLdouble* m)
{
	log() << warning << "MatrixMultTransposedEXT not available." << std::endl;
}

static void fallback_glMatrixMultTransposefEXT(api::GLenum mode, const api::GLfloat* m)
{
	log() << warning << "MatrixMultTransposefEXT not available." << std::endl;
}

static void fallback_glMatrixMultdEXT(api::GLenum mode, const api::GLdouble* m)
{
	log() << warning << "MatrixMultdEXT not available." << std::endl;
}

static void fallback_glMatrixMultfEXT(api::GLenum mode, const api::GLfloat* m)
{
	log() << warning << "MatrixMultfEXT not available." << std::endl;
}

static void fallback_glMatrixOrthoEXT(api::GLenum mode, api::GLdouble left, api::GLdouble right, api::GLdouble bottom, api::GLdouble top, api::GLdouble zNear, api::GLdouble zFar)
{
	log() << warning << "MatrixOrthoEXT not available." << std::endl;
}

static void fallback_glMatrixPopEXT(api::GLenum mode)
{
	log() << warning << "MatrixPopEXT not available." << std::endl;
}

static void fallback_glMatrixPushEXT(api::GLenum mode)
{
	log() << warning << "MatrixPushEXT not available." << std::endl;
}

static void fallback_glMatrixRotatedEXT(api::GLenum mode, api::GLdouble angle, api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "MatrixRotatedEXT not available." << std::endl;
}

static void fallback_glMatrixRotatefEXT(api::GLenum mode, api::GLfloat angle, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "MatrixRotatefEXT not available." << std::endl;
}

static void fallback_glMatrixScaledEXT(api::GLenum mode, api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "MatrixScaledEXT not available." << std::endl;
}

static void fallback_glMatrixScalefEXT(api::GLenum mode, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "MatrixScalefEXT not available." << std::endl;
}

static void fallback_glMatrixTranslatedEXT(api::GLenum mode, api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "MatrixTranslatedEXT not available." << std::endl;
}

static void fallback_glMatrixTranslatefEXT(api::GLenum mode, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "MatrixTranslatefEXT not available." << std::endl;
}

static void fallback_glMinSampleShading(api::GLclampf value)
{
	log() << warning << "MinSampleShading not available." << std::endl;
}

static void fallback_glMinmax(api::GLenum target, api::GLenum internalformat, api::GLboolean sink)
{
	log() << warning << "Minmax not available." << std::endl;
}

static void fallback_glMinmaxEXT(api::GLenum target, api::GLenum internalformat, api::GLboolean sink)
{
	log() << warning << "MinmaxEXT not available." << std::endl;
}

static void fallback_glMultMatrixd(const api::GLdouble* m)
{
	log() << warning << "MultMatrixd not available." << std::endl;
}

static void fallback_glMultMatrixf(const api::GLfloat* m)
{
	log() << warning << "MultMatrixf not available." << std::endl;
}

static void fallback_glMultTransposeMatrixd(const api::GLdouble* m)
{
	log() << warning << "MultTransposeMatrixd not available." << std::endl;
}

static void fallback_glMultTransposeMatrixdARB(const api::GLdouble* m)
{
	log() << warning << "MultTransposeMatrixdARB not available." << std::endl;
}

static void fallback_glMultTransposeMatrixf(const api::GLfloat* m)
{
	log() << warning << "MultTransposeMatrixf not available." << std::endl;
}

static void fallback_glMultTransposeMatrixfARB(const api::GLfloat* m)
{
	log() << warning << "MultTransposeMatrixfARB not available." << std::endl;
}

static void fallback_glMultiDrawArrays(api::GLenum mode, api::GLint* first, api::GLsizei* count, api::GLsizei primcount)
{
	log() << warning << "MultiDrawArrays not available." << std::endl;
}

static void fallback_glMultiDrawArraysEXT(api::GLenum mode, api::GLint* first, api::GLsizei* count, api::GLsizei primcount)
{
	log() << warning << "MultiDrawArraysEXT not available." << std::endl;
}

static void fallback_glMultiDrawElementArrayAPPLE(api::GLenum mode, const api::GLint* first, const api::GLsizei* count, api::GLsizei primcount)
{
	log() << warning << "MultiDrawElementArrayAPPLE not available." << std::endl;
}

static void fallback_glMultiDrawElements(api::GLenum mode, const api::GLsizei* count, api::GLenum type, const api::GLvoid** indices, api::GLsizei primcount)
{
	log() << warning << "MultiDrawElements not available." << std::endl;
}

static void fallback_glMultiDrawElementsBaseVertex(api::GLenum mode, const api::GLsizei* count, api::GLenum type, const api::GLvoid** indices, api::GLsizei primcount, const api::GLint* basevertex)
{
	log() << warning << "MultiDrawElementsBaseVertex not available." << std::endl;
}

static void fallback_glMultiDrawElementsEXT(api::GLenum mode, const api::GLsizei* count, api::GLenum type, const api::GLvoid** indices, api::GLsizei primcount)
{
	log() << warning << "MultiDrawElementsEXT not available." << std::endl;
}

static void fallback_glMultiDrawRangeElementArrayAPPLE(api::GLenum mode, api::GLuint start, api::GLuint end, const api::GLint* first, const api::GLsizei* count, api::GLsizei primcount)
{
	log() << warning << "MultiDrawRangeElementArrayAPPLE not available." << std::endl;
}

static void fallback_glMultiModeDrawArraysIBM(const api::GLenum* mode, const api::GLint* first, const api::GLsizei* count, api::GLsizei primcount, api::GLint modestride)
{
	log() << warning << "MultiModeDrawArraysIBM not available." << std::endl;
}

static void fallback_glMultiModeDrawElementsIBM(const api::GLenum* mode, const api::GLsizei* count, api::GLenum type, const api::GLvoid* const* indices, api::GLsizei primcount, api::GLint modestride)
{
	log() << warning << "MultiModeDrawElementsIBM not available." << std::endl;
}

static void fallback_glMultiTexBufferEXT(api::GLenum texunit, api::GLenum target, api::GLenum internalformat, api::GLuint buffer)
{
	log() << warning << "MultiTexBufferEXT not available." << std::endl;
}

static void fallback_glMultiTexCoord1d(api::GLenum target, api::GLdouble s)
{
	log() << warning << "MultiTexCoord1d not available." << std::endl;
}

static void fallback_glMultiTexCoord1dARB(api::GLenum target, api::GLdouble s)
{
	log() << warning << "MultiTexCoord1dARB not available." << std::endl;
}

static void fallback_glMultiTexCoord1dv(api::GLenum target, const api::GLdouble* v)
{
	log() << warning << "MultiTexCoord1dv not available." << std::endl;
}

static void fallback_glMultiTexCoord1dvARB(api::GLenum target, const api::GLdouble* v)
{
	log() << warning << "MultiTexCoord1dvARB not available." << std::endl;
}

static void fallback_glMultiTexCoord1f(api::GLenum target, api::GLfloat s)
{
	log() << warning << "MultiTexCoord1f not available." << std::endl;
}

static void fallback_glMultiTexCoord1fARB(api::GLenum target, api::GLfloat s)
{
	log() << warning << "MultiTexCoord1fARB not available." << std::endl;
}

static void fallback_glMultiTexCoord1fv(api::GLenum target, const api::GLfloat* v)
{
	log() << warning << "MultiTexCoord1fv not available." << std::endl;
}

static void fallback_glMultiTexCoord1fvARB(api::GLenum target, const api::GLfloat* v)
{
	log() << warning << "MultiTexCoord1fvARB not available." << std::endl;
}

static void fallback_glMultiTexCoord1hNV(api::GLenum target, api::GLhalfNV s)
{
	log() << warning << "MultiTexCoord1hNV not available." << std::endl;
}

static void fallback_glMultiTexCoord1hvNV(api::GLenum target, const api::GLhalfNV* v)
{
	log() << warning << "MultiTexCoord1hvNV not available." << std::endl;
}

static void fallback_glMultiTexCoord1i(api::GLenum target, api::GLint s)
{
	log() << warning << "MultiTexCoord1i not available." << std::endl;
}

static void fallback_glMultiTexCoord1iARB(api::GLenum target, api::GLint s)
{
	log() << warning << "MultiTexCoord1iARB not available." << std::endl;
}

static void fallback_glMultiTexCoord1iv(api::GLenum target, const api::GLint* v)
{
	log() << warning << "MultiTexCoord1iv not available." << std::endl;
}

static void fallback_glMultiTexCoord1ivARB(api::GLenum target, const api::GLint* v)
{
	log() << warning << "MultiTexCoord1ivARB not available." << std::endl;
}

static void fallback_glMultiTexCoord1s(api::GLenum target, api::GLshort s)
{
	log() << warning << "MultiTexCoord1s not available." << std::endl;
}

static void fallback_glMultiTexCoord1sARB(api::GLenum target, api::GLshort s)
{
	log() << warning << "MultiTexCoord1sARB not available." << std::endl;
}

static void fallback_glMultiTexCoord1sv(api::GLenum target, const api::GLshort* v)
{
	log() << warning << "MultiTexCoord1sv not available." << std::endl;
}

static void fallback_glMultiTexCoord1svARB(api::GLenum target, const api::GLshort* v)
{
	log() << warning << "MultiTexCoord1svARB not available." << std::endl;
}

static void fallback_glMultiTexCoord2d(api::GLenum target, api::GLdouble s, api::GLdouble t)
{
	log() << warning << "MultiTexCoord2d not available." << std::endl;
}

static void fallback_glMultiTexCoord2dARB(api::GLenum target, api::GLdouble s, api::GLdouble t)
{
	log() << warning << "MultiTexCoord2dARB not available." << std::endl;
}

static void fallback_glMultiTexCoord2dv(api::GLenum target, const api::GLdouble* v)
{
	log() << warning << "MultiTexCoord2dv not available." << std::endl;
}

static void fallback_glMultiTexCoord2dvARB(api::GLenum target, const api::GLdouble* v)
{
	log() << warning << "MultiTexCoord2dvARB not available." << std::endl;
}

static void fallback_glMultiTexCoord2f(api::GLenum target, api::GLfloat s, api::GLfloat t)
{
	log() << warning << "MultiTexCoord2f not available." << std::endl;
}

static void fallback_glMultiTexCoord2fARB(api::GLenum target, api::GLfloat s, api::GLfloat t)
{
	log() << warning << "MultiTexCoord2fARB not available." << std::endl;
}

static void fallback_glMultiTexCoord2fv(api::GLenum target, const api::GLfloat* v)
{
	log() << warning << "MultiTexCoord2fv not available." << std::endl;
}

static void fallback_glMultiTexCoord2fvARB(api::GLenum target, const api::GLfloat* v)
{
	log() << warning << "MultiTexCoord2fvARB not available." << std::endl;
}

static void fallback_glMultiTexCoord2hNV(api::GLenum target, api::GLhalfNV s, api::GLhalfNV t)
{
	log() << warning << "MultiTexCoord2hNV not available." << std::endl;
}

static void fallback_glMultiTexCoord2hvNV(api::GLenum target, const api::GLhalfNV* v)
{
	log() << warning << "MultiTexCoord2hvNV not available." << std::endl;
}

static void fallback_glMultiTexCoord2i(api::GLenum target, api::GLint s, api::GLint t)
{
	log() << warning << "MultiTexCoord2i not available." << std::endl;
}

static void fallback_glMultiTexCoord2iARB(api::GLenum target, api::GLint s, api::GLint t)
{
	log() << warning << "MultiTexCoord2iARB not available." << std::endl;
}

static void fallback_glMultiTexCoord2iv(api::GLenum target, const api::GLint* v)
{
	log() << warning << "MultiTexCoord2iv not available." << std::endl;
}

static void fallback_glMultiTexCoord2ivARB(api::GLenum target, const api::GLint* v)
{
	log() << warning << "MultiTexCoord2ivARB not available." << std::endl;
}

static void fallback_glMultiTexCoord2s(api::GLenum target, api::GLshort s, api::GLshort t)
{
	log() << warning << "MultiTexCoord2s not available." << std::endl;
}

static void fallback_glMultiTexCoord2sARB(api::GLenum target, api::GLshort s, api::GLshort t)
{
	log() << warning << "MultiTexCoord2sARB not available." << std::endl;
}

static void fallback_glMultiTexCoord2sv(api::GLenum target, const api::GLshort* v)
{
	log() << warning << "MultiTexCoord2sv not available." << std::endl;
}

static void fallback_glMultiTexCoord2svARB(api::GLenum target, const api::GLshort* v)
{
	log() << warning << "MultiTexCoord2svARB not available." << std::endl;
}

static void fallback_glMultiTexCoord3d(api::GLenum target, api::GLdouble s, api::GLdouble t, api::GLdouble r)
{
	log() << warning << "MultiTexCoord3d not available." << std::endl;
}

static void fallback_glMultiTexCoord3dARB(api::GLenum target, api::GLdouble s, api::GLdouble t, api::GLdouble r)
{
	log() << warning << "MultiTexCoord3dARB not available." << std::endl;
}

static void fallback_glMultiTexCoord3dv(api::GLenum target, const api::GLdouble* v)
{
	log() << warning << "MultiTexCoord3dv not available." << std::endl;
}

static void fallback_glMultiTexCoord3dvARB(api::GLenum target, const api::GLdouble* v)
{
	log() << warning << "MultiTexCoord3dvARB not available." << std::endl;
}

static void fallback_glMultiTexCoord3f(api::GLenum target, api::GLfloat s, api::GLfloat t, api::GLfloat r)
{
	log() << warning << "MultiTexCoord3f not available." << std::endl;
}

static void fallback_glMultiTexCoord3fARB(api::GLenum target, api::GLfloat s, api::GLfloat t, api::GLfloat r)
{
	log() << warning << "MultiTexCoord3fARB not available." << std::endl;
}

static void fallback_glMultiTexCoord3fv(api::GLenum target, const api::GLfloat* v)
{
	log() << warning << "MultiTexCoord3fv not available." << std::endl;
}

static void fallback_glMultiTexCoord3fvARB(api::GLenum target, const api::GLfloat* v)
{
	log() << warning << "MultiTexCoord3fvARB not available." << std::endl;
}

static void fallback_glMultiTexCoord3hNV(api::GLenum target, api::GLhalfNV s, api::GLhalfNV t, api::GLhalfNV r)
{
	log() << warning << "MultiTexCoord3hNV not available." << std::endl;
}

static void fallback_glMultiTexCoord3hvNV(api::GLenum target, const api::GLhalfNV* v)
{
	log() << warning << "MultiTexCoord3hvNV not available." << std::endl;
}

static void fallback_glMultiTexCoord3i(api::GLenum target, api::GLint s, api::GLint t, api::GLint r)
{
	log() << warning << "MultiTexCoord3i not available." << std::endl;
}

static void fallback_glMultiTexCoord3iARB(api::GLenum target, api::GLint s, api::GLint t, api::GLint r)
{
	log() << warning << "MultiTexCoord3iARB not available." << std::endl;
}

static void fallback_glMultiTexCoord3iv(api::GLenum target, const api::GLint* v)
{
	log() << warning << "MultiTexCoord3iv not available." << std::endl;
}

static void fallback_glMultiTexCoord3ivARB(api::GLenum target, const api::GLint* v)
{
	log() << warning << "MultiTexCoord3ivARB not available." << std::endl;
}

static void fallback_glMultiTexCoord3s(api::GLenum target, api::GLshort s, api::GLshort t, api::GLshort r)
{
	log() << warning << "MultiTexCoord3s not available." << std::endl;
}

static void fallback_glMultiTexCoord3sARB(api::GLenum target, api::GLshort s, api::GLshort t, api::GLshort r)
{
	log() << warning << "MultiTexCoord3sARB not available." << std::endl;
}

static void fallback_glMultiTexCoord3sv(api::GLenum target, const api::GLshort* v)
{
	log() << warning << "MultiTexCoord3sv not available." << std::endl;
}

static void fallback_glMultiTexCoord3svARB(api::GLenum target, const api::GLshort* v)
{
	log() << warning << "MultiTexCoord3svARB not available." << std::endl;
}

static void fallback_glMultiTexCoord4d(api::GLenum target, api::GLdouble s, api::GLdouble t, api::GLdouble r, api::GLdouble q)
{
	log() << warning << "MultiTexCoord4d not available." << std::endl;
}

static void fallback_glMultiTexCoord4dARB(api::GLenum target, api::GLdouble s, api::GLdouble t, api::GLdouble r, api::GLdouble q)
{
	log() << warning << "MultiTexCoord4dARB not available." << std::endl;
}

static void fallback_glMultiTexCoord4dv(api::GLenum target, const api::GLdouble* v)
{
	log() << warning << "MultiTexCoord4dv not available." << std::endl;
}

static void fallback_glMultiTexCoord4dvARB(api::GLenum target, const api::GLdouble* v)
{
	log() << warning << "MultiTexCoord4dvARB not available." << std::endl;
}

static void fallback_glMultiTexCoord4f(api::GLenum target, api::GLfloat s, api::GLfloat t, api::GLfloat r, api::GLfloat q)
{
	log() << warning << "MultiTexCoord4f not available." << std::endl;
}

static void fallback_glMultiTexCoord4fARB(api::GLenum target, api::GLfloat s, api::GLfloat t, api::GLfloat r, api::GLfloat q)
{
	log() << warning << "MultiTexCoord4fARB not available." << std::endl;
}

static void fallback_glMultiTexCoord4fv(api::GLenum target, const api::GLfloat* v)
{
	log() << warning << "MultiTexCoord4fv not available." << std::endl;
}

static void fallback_glMultiTexCoord4fvARB(api::GLenum target, const api::GLfloat* v)
{
	log() << warning << "MultiTexCoord4fvARB not available." << std::endl;
}

static void fallback_glMultiTexCoord4hNV(api::GLenum target, api::GLhalfNV s, api::GLhalfNV t, api::GLhalfNV r, api::GLhalfNV q)
{
	log() << warning << "MultiTexCoord4hNV not available." << std::endl;
}

static void fallback_glMultiTexCoord4hvNV(api::GLenum target, const api::GLhalfNV* v)
{
	log() << warning << "MultiTexCoord4hvNV not available." << std::endl;
}

static void fallback_glMultiTexCoord4i(api::GLenum target, api::GLint s, api::GLint t, api::GLint r, api::GLint q)
{
	log() << warning << "MultiTexCoord4i not available." << std::endl;
}

static void fallback_glMultiTexCoord4iARB(api::GLenum target, api::GLint s, api::GLint t, api::GLint r, api::GLint q)
{
	log() << warning << "MultiTexCoord4iARB not available." << std::endl;
}

static void fallback_glMultiTexCoord4iv(api::GLenum target, const api::GLint* v)
{
	log() << warning << "MultiTexCoord4iv not available." << std::endl;
}

static void fallback_glMultiTexCoord4ivARB(api::GLenum target, const api::GLint* v)
{
	log() << warning << "MultiTexCoord4ivARB not available." << std::endl;
}

static void fallback_glMultiTexCoord4s(api::GLenum target, api::GLshort s, api::GLshort t, api::GLshort r, api::GLshort q)
{
	log() << warning << "MultiTexCoord4s not available." << std::endl;
}

static void fallback_glMultiTexCoord4sARB(api::GLenum target, api::GLshort s, api::GLshort t, api::GLshort r, api::GLshort q)
{
	log() << warning << "MultiTexCoord4sARB not available." << std::endl;
}

static void fallback_glMultiTexCoord4sv(api::GLenum target, const api::GLshort* v)
{
	log() << warning << "MultiTexCoord4sv not available." << std::endl;
}

static void fallback_glMultiTexCoord4svARB(api::GLenum target, const api::GLshort* v)
{
	log() << warning << "MultiTexCoord4svARB not available." << std::endl;
}

static void fallback_glMultiTexCoordP1ui(api::GLenum texture, api::GLenum type, api::GLuint coords)
{
	log() << warning << "MultiTexCoordP1ui not available." << std::endl;
}

static void fallback_glMultiTexCoordP1uiv(api::GLenum texture, api::GLenum type, const api::GLuint* coords)
{
	log() << warning << "MultiTexCoordP1uiv not available." << std::endl;
}

static void fallback_glMultiTexCoordP2ui(api::GLenum texture, api::GLenum type, api::GLuint coords)
{
	log() << warning << "MultiTexCoordP2ui not available." << std::endl;
}

static void fallback_glMultiTexCoordP2uiv(api::GLenum texture, api::GLenum type, const api::GLuint* coords)
{
	log() << warning << "MultiTexCoordP2uiv not available." << std::endl;
}

static void fallback_glMultiTexCoordP3ui(api::GLenum texture, api::GLenum type, api::GLuint coords)
{
	log() << warning << "MultiTexCoordP3ui not available." << std::endl;
}

static void fallback_glMultiTexCoordP3uiv(api::GLenum texture, api::GLenum type, const api::GLuint* coords)
{
	log() << warning << "MultiTexCoordP3uiv not available." << std::endl;
}

static void fallback_glMultiTexCoordP4ui(api::GLenum texture, api::GLenum type, api::GLuint coords)
{
	log() << warning << "MultiTexCoordP4ui not available." << std::endl;
}

static void fallback_glMultiTexCoordP4uiv(api::GLenum texture, api::GLenum type, const api::GLuint* coords)
{
	log() << warning << "MultiTexCoordP4uiv not available." << std::endl;
}

static void fallback_glMultiTexCoordPointerEXT(api::GLenum texunit, api::GLint size, api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "MultiTexCoordPointerEXT not available." << std::endl;
}

static void fallback_glMultiTexEnvfEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, api::GLfloat param)
{
	log() << warning << "MultiTexEnvfEXT not available." << std::endl;
}

static void fallback_glMultiTexEnvfvEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "MultiTexEnvfvEXT not available." << std::endl;
}

static void fallback_glMultiTexEnviEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, api::GLint param)
{
	log() << warning << "MultiTexEnviEXT not available." << std::endl;
}

static void fallback_glMultiTexEnvivEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "MultiTexEnvivEXT not available." << std::endl;
}

static void fallback_glMultiTexGendEXT(api::GLenum texunit, api::GLenum coord, api::GLenum pname, api::GLdouble param)
{
	log() << warning << "MultiTexGendEXT not available." << std::endl;
}

static void fallback_glMultiTexGendvEXT(api::GLenum texunit, api::GLenum coord, api::GLenum pname, const api::GLdouble* params)
{
	log() << warning << "MultiTexGendvEXT not available." << std::endl;
}

static void fallback_glMultiTexGenfEXT(api::GLenum texunit, api::GLenum coord, api::GLenum pname, api::GLfloat param)
{
	log() << warning << "MultiTexGenfEXT not available." << std::endl;
}

static void fallback_glMultiTexGenfvEXT(api::GLenum texunit, api::GLenum coord, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "MultiTexGenfvEXT not available." << std::endl;
}

static void fallback_glMultiTexGeniEXT(api::GLenum texunit, api::GLenum coord, api::GLenum pname, api::GLint param)
{
	log() << warning << "MultiTexGeniEXT not available." << std::endl;
}

static void fallback_glMultiTexGenivEXT(api::GLenum texunit, api::GLenum coord, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "MultiTexGenivEXT not available." << std::endl;
}

static void fallback_glMultiTexImage1DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLint border, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "MultiTexImage1DEXT not available." << std::endl;
}

static void fallback_glMultiTexImage2DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLint border, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "MultiTexImage2DEXT not available." << std::endl;
}

static void fallback_glMultiTexImage3DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLint border, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "MultiTexImage3DEXT not available." << std::endl;
}

static void fallback_glMultiTexParameterIivEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "MultiTexParameterIivEXT not available." << std::endl;
}

static void fallback_glMultiTexParameterIuivEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, const api::GLuint* params)
{
	log() << warning << "MultiTexParameterIuivEXT not available." << std::endl;
}

static void fallback_glMultiTexParameterfEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, api::GLfloat param)
{
	log() << warning << "MultiTexParameterfEXT not available." << std::endl;
}

static void fallback_glMultiTexParameterfvEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "MultiTexParameterfvEXT not available." << std::endl;
}

static void fallback_glMultiTexParameteriEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, api::GLint param)
{
	log() << warning << "MultiTexParameteriEXT not available." << std::endl;
}

static void fallback_glMultiTexParameterivEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "MultiTexParameterivEXT not available." << std::endl;
}

static void fallback_glMultiTexRenderbufferEXT(api::GLenum texunit, api::GLenum target, api::GLuint renderbuffer)
{
	log() << warning << "MultiTexRenderbufferEXT not available." << std::endl;
}

static void fallback_glMultiTexSubImage1DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLsizei width, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "MultiTexSubImage1DEXT not available." << std::endl;
}

static void fallback_glMultiTexSubImage2DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLsizei width, api::GLsizei height, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "MultiTexSubImage2DEXT not available." << std::endl;
}

static void fallback_glMultiTexSubImage3DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint zoffset, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "MultiTexSubImage3DEXT not available." << std::endl;
}

static void fallback_glNamedBufferDataEXT(api::GLuint buffer, api::GLsizeiptr size, const api::GLvoid* data, api::GLenum usage)
{
	log() << warning << "NamedBufferDataEXT not available." << std::endl;
}

static void fallback_glNamedBufferSubDataEXT(api::GLuint buffer, api::GLintptr offset, api::GLsizeiptr size, const api::GLvoid* data)
{
	log() << warning << "NamedBufferSubDataEXT not available." << std::endl;
}

static void fallback_glNamedFramebufferRenderbufferEXT(api::GLuint framebuffer, api::GLenum attachment, api::GLenum renderbuffertarget, api::GLuint renderbuffer)
{
	log() << warning << "NamedFramebufferRenderbufferEXT not available." << std::endl;
}

static void fallback_glNamedFramebufferTexture1DEXT(api::GLuint framebuffer, api::GLenum attachment, api::GLenum textarget, api::GLuint texture, api::GLint level)
{
	log() << warning << "NamedFramebufferTexture1DEXT not available." << std::endl;
}

static void fallback_glNamedFramebufferTexture2DEXT(api::GLuint framebuffer, api::GLenum attachment, api::GLenum textarget, api::GLuint texture, api::GLint level)
{
	log() << warning << "NamedFramebufferTexture2DEXT not available." << std::endl;
}

static void fallback_glNamedFramebufferTexture3DEXT(api::GLuint framebuffer, api::GLenum attachment, api::GLenum textarget, api::GLuint texture, api::GLint level, api::GLint zoffset)
{
	log() << warning << "NamedFramebufferTexture3DEXT not available." << std::endl;
}

static void fallback_glNamedFramebufferTextureEXT(api::GLuint framebuffer, api::GLenum attachment, api::GLuint texture, api::GLint level)
{
	log() << warning << "NamedFramebufferTextureEXT not available." << std::endl;
}

static void fallback_glNamedFramebufferTextureFaceEXT(api::GLuint framebuffer, api::GLenum attachment, api::GLuint texture, api::GLint level, api::GLenum face)
{
	log() << warning << "NamedFramebufferTextureFaceEXT not available." << std::endl;
}

static void fallback_glNamedFramebufferTextureLayerEXT(api::GLuint framebuffer, api::GLenum attachment, api::GLuint texture, api::GLint level, api::GLint layer)
{
	log() << warning << "NamedFramebufferTextureLayerEXT not available." << std::endl;
}

static void fallback_glNamedProgramLocalParameter4dEXT(api::GLuint program, api::GLenum target, api::GLuint index, api::GLdouble x, api::GLdouble y, api::GLdouble z, api::GLdouble w)
{
	log() << warning << "NamedProgramLocalParameter4dEXT not available." << std::endl;
}

static void fallback_glNamedProgramLocalParameter4dvEXT(api::GLuint program, api::GLenum target, api::GLuint index, const api::GLdouble* params)
{
	log() << warning << "NamedProgramLocalParameter4dvEXT not available." << std::endl;
}

static void fallback_glNamedProgramLocalParameter4fEXT(api::GLuint program, api::GLenum target, api::GLuint index, api::GLfloat x, api::GLfloat y, api::GLfloat z, api::GLfloat w)
{
	log() << warning << "NamedProgramLocalParameter4fEXT not available." << std::endl;
}

static void fallback_glNamedProgramLocalParameter4fvEXT(api::GLuint program, api::GLenum target, api::GLuint index, const api::GLfloat* params)
{
	log() << warning << "NamedProgramLocalParameter4fvEXT not available." << std::endl;
}

static void fallback_glNamedProgramLocalParameterI4iEXT(api::GLuint program, api::GLenum target, api::GLuint index, api::GLint x, api::GLint y, api::GLint z, api::GLint w)
{
	log() << warning << "NamedProgramLocalParameterI4iEXT not available." << std::endl;
}

static void fallback_glNamedProgramLocalParameterI4ivEXT(api::GLuint program, api::GLenum target, api::GLuint index, const api::GLint* params)
{
	log() << warning << "NamedProgramLocalParameterI4ivEXT not available." << std::endl;
}

static void fallback_glNamedProgramLocalParameterI4uiEXT(api::GLuint program, api::GLenum target, api::GLuint index, api::GLuint x, api::GLuint y, api::GLuint z, api::GLuint w)
{
	log() << warning << "NamedProgramLocalParameterI4uiEXT not available." << std::endl;
}

static void fallback_glNamedProgramLocalParameterI4uivEXT(api::GLuint program, api::GLenum target, api::GLuint index, const api::GLuint* params)
{
	log() << warning << "NamedProgramLocalParameterI4uivEXT not available." << std::endl;
}

static void fallback_glNamedProgramLocalParameters4fvEXT(api::GLuint program, api::GLenum target, api::GLuint index, api::GLsizei count, const api::GLfloat* params)
{
	log() << warning << "NamedProgramLocalParameters4fvEXT not available." << std::endl;
}

static void fallback_glNamedProgramLocalParametersI4ivEXT(api::GLuint program, api::GLenum target, api::GLuint index, api::GLsizei count, const api::GLint* params)
{
	log() << warning << "NamedProgramLocalParametersI4ivEXT not available." << std::endl;
}

static void fallback_glNamedProgramLocalParametersI4uivEXT(api::GLuint program, api::GLenum target, api::GLuint index, api::GLsizei count, const api::GLuint* params)
{
	log() << warning << "NamedProgramLocalParametersI4uivEXT not available." << std::endl;
}

static void fallback_glNamedProgramStringEXT(api::GLuint program, api::GLenum target, api::GLenum format, api::GLsizei len, const api::GLvoid* string)
{
	log() << warning << "NamedProgramStringEXT not available." << std::endl;
}

static void fallback_glNamedRenderbufferStorageEXT(api::GLuint renderbuffer, api::GLenum internalformat, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "NamedRenderbufferStorageEXT not available." << std::endl;
}

static void fallback_glNamedRenderbufferStorageMultisampleCoverageEXT(api::GLuint renderbuffer, api::GLsizei coverageSamples, api::GLsizei colorSamples, api::GLenum internalformat, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "NamedRenderbufferStorageMultisampleCoverageEXT not available." << std::endl;
}

static void fallback_glNamedRenderbufferStorageMultisampleEXT(api::GLuint renderbuffer, api::GLsizei samples, api::GLenum internalformat, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "NamedRenderbufferStorageMultisampleEXT not available." << std::endl;
}

static void fallback_glNamedStringARB(api::GLenum type, api::GLint namelen, const api::GLchar* name, api::GLint stringlen, const api::GLchar* string)
{
	log() << warning << "NamedStringARB not available." << std::endl;
}

static void fallback_glNewList(api::GLuint list, api::GLenum mode)
{
	log() << warning << "NewList not available." << std::endl;
}

static api::GLuint fallback_glNewObjectBufferATI(api::GLsizei size, const api::GLvoid* pointer, api::GLenum usage)
{
	log() << warning << "NewObjectBufferATI not available." << std::endl;
}

static void fallback_glNormal3b(api::GLbyte nx, api::GLbyte ny, api::GLbyte nz)
{
	log() << warning << "Normal3b not available." << std::endl;
}

static void fallback_glNormal3bv(const api::GLbyte* v)
{
	log() << warning << "Normal3bv not available." << std::endl;
}

static void fallback_glNormal3d(api::GLdouble nx, api::GLdouble ny, api::GLdouble nz)
{
	log() << warning << "Normal3d not available." << std::endl;
}

static void fallback_glNormal3dv(const api::GLdouble* v)
{
	log() << warning << "Normal3dv not available." << std::endl;
}

static void fallback_glNormal3f(api::GLfloat nx, api::GLfloat ny, api::GLfloat nz)
{
	log() << warning << "Normal3f not available." << std::endl;
}

static void fallback_glNormal3fVertex3fSUN(api::GLfloat nx, api::GLfloat ny, api::GLfloat nz, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "Normal3fVertex3fSUN not available." << std::endl;
}

static void fallback_glNormal3fVertex3fvSUN(const api::GLfloat* n, const api::GLfloat* v)
{
	log() << warning << "Normal3fVertex3fvSUN not available." << std::endl;
}

static void fallback_glNormal3fv(const api::GLfloat* v)
{
	log() << warning << "Normal3fv not available." << std::endl;
}

static void fallback_glNormal3hNV(api::GLhalfNV nx, api::GLhalfNV ny, api::GLhalfNV nz)
{
	log() << warning << "Normal3hNV not available." << std::endl;
}

static void fallback_glNormal3hvNV(const api::GLhalfNV* v)
{
	log() << warning << "Normal3hvNV not available." << std::endl;
}

static void fallback_glNormal3i(api::GLint nx, api::GLint ny, api::GLint nz)
{
	log() << warning << "Normal3i not available." << std::endl;
}

static void fallback_glNormal3iv(const api::GLint* v)
{
	log() << warning << "Normal3iv not available." << std::endl;
}

static void fallback_glNormal3s(api::GLshort nx, api::GLshort ny, api::GLshort nz)
{
	log() << warning << "Normal3s not available." << std::endl;
}

static void fallback_glNormal3sv(const api::GLshort* v)
{
	log() << warning << "Normal3sv not available." << std::endl;
}

static void fallback_glNormalFormatNV(api::GLenum type, api::GLsizei stride)
{
	log() << warning << "NormalFormatNV not available." << std::endl;
}

static void fallback_glNormalP3ui(api::GLenum type, api::GLuint coords)
{
	log() << warning << "NormalP3ui not available." << std::endl;
}

static void fallback_glNormalP3uiv(api::GLenum type, const api::GLuint* coords)
{
	log() << warning << "NormalP3uiv not available." << std::endl;
}

static void fallback_glNormalPointer(api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "NormalPointer not available." << std::endl;
}

static void fallback_glNormalPointerEXT(api::GLenum type, api::GLsizei stride, api::GLsizei count, const api::GLvoid* pointer)
{
	log() << warning << "NormalPointerEXT not available." << std::endl;
}

static void fallback_glNormalPointerListIBM(api::GLenum type, api::GLint stride, const api::GLvoid** pointer, api::GLint ptrstride)
{
	log() << warning << "NormalPointerListIBM not available." << std::endl;
}

static void fallback_glNormalPointervINTEL(api::GLenum type, const api::GLvoid** pointer)
{
	log() << warning << "NormalPointervINTEL not available." << std::endl;
}

static void fallback_glNormalStream3bATI(api::GLenum stream, api::GLbyte nx, api::GLbyte ny, api::GLbyte nz)
{
	log() << warning << "NormalStream3bATI not available." << std::endl;
}

static void fallback_glNormalStream3bvATI(api::GLenum stream, const api::GLbyte* coords)
{
	log() << warning << "NormalStream3bvATI not available." << std::endl;
}

static void fallback_glNormalStream3dATI(api::GLenum stream, api::GLdouble nx, api::GLdouble ny, api::GLdouble nz)
{
	log() << warning << "NormalStream3dATI not available." << std::endl;
}

static void fallback_glNormalStream3dvATI(api::GLenum stream, const api::GLdouble* coords)
{
	log() << warning << "NormalStream3dvATI not available." << std::endl;
}

static void fallback_glNormalStream3fATI(api::GLenum stream, api::GLfloat nx, api::GLfloat ny, api::GLfloat nz)
{
	log() << warning << "NormalStream3fATI not available." << std::endl;
}

static void fallback_glNormalStream3fvATI(api::GLenum stream, const api::GLfloat* coords)
{
	log() << warning << "NormalStream3fvATI not available." << std::endl;
}

static void fallback_glNormalStream3iATI(api::GLenum stream, api::GLint nx, api::GLint ny, api::GLint nz)
{
	log() << warning << "NormalStream3iATI not available." << std::endl;
}

static void fallback_glNormalStream3ivATI(api::GLenum stream, const api::GLint* coords)
{
	log() << warning << "NormalStream3ivATI not available." << std::endl;
}

static void fallback_glNormalStream3sATI(api::GLenum stream, api::GLshort nx, api::GLshort ny, api::GLshort nz)
{
	log() << warning << "NormalStream3sATI not available." << std::endl;
}

static void fallback_glNormalStream3svATI(api::GLenum stream, const api::GLshort* coords)
{
	log() << warning << "NormalStream3svATI not available." << std::endl;
}

static api::GLenum fallback_glObjectPurgeableAPPLE(api::GLenum objectType, api::GLuint name, api::GLenum option)
{
	log() << warning << "ObjectPurgeableAPPLE not available." << std::endl;
}

static api::GLenum fallback_glObjectUnpurgeableAPPLE(api::GLenum objectType, api::GLuint name, api::GLenum option)
{
	log() << warning << "ObjectUnpurgeableAPPLE not available." << std::endl;
}

static void fallback_glOrtho(api::GLdouble left, api::GLdouble right, api::GLdouble bottom, api::GLdouble top, api::GLdouble zNear, api::GLdouble zFar)
{
	log() << warning << "Ortho not available." << std::endl;
}

static void fallback_glPNTrianglesfATI(api::GLenum pname, api::GLfloat param)
{
	log() << warning << "PNTrianglesfATI not available." << std::endl;
}

static void fallback_glPNTrianglesiATI(api::GLenum pname, api::GLint param)
{
	log() << warning << "PNTrianglesiATI not available." << std::endl;
}

static void fallback_glPassTexCoordATI(api::GLuint dst, api::GLuint coord, api::GLenum swizzle)
{
	log() << warning << "PassTexCoordATI not available." << std::endl;
}

static void fallback_glPassThrough(api::GLfloat token)
{
	log() << warning << "PassThrough not available." << std::endl;
}

static void fallback_glPatchParameterfv(api::GLenum pname, const api::GLfloat* values)
{
	log() << warning << "PatchParameterfv not available." << std::endl;
}

static void fallback_glPatchParameteri(api::GLenum pname, api::GLint value)
{
	log() << warning << "PatchParameteri not available." << std::endl;
}

static void fallback_glPauseTransformFeedback()
{
	log() << warning << "PauseTransformFeedback not available." << std::endl;
}

static void fallback_glPauseTransformFeedbackNV()
{
	log() << warning << "PauseTransformFeedbackNV not available." << std::endl;
}

static void fallback_glPixelDataRangeNV(api::GLenum target, api::GLsizei length, api::GLvoid* pointer)
{
	log() << warning << "PixelDataRangeNV not available." << std::endl;
}

static void fallback_glPixelMapfv(api::GLenum map, api::GLint mapsize, const api::GLfloat* values)
{
	log() << warning << "PixelMapfv not available." << std::endl;
}

static void fallback_glPixelMapuiv(api::GLenum map, api::GLint mapsize, const api::GLuint* values)
{
	log() << warning << "PixelMapuiv not available." << std::endl;
}

static void fallback_glPixelMapusv(api::GLenum map, api::GLint mapsize, const api::GLushort* values)
{
	log() << warning << "PixelMapusv not available." << std::endl;
}

static void fallback_glPixelStoref(api::GLenum pname, api::GLfloat param)
{
	log() << warning << "PixelStoref not available." << std::endl;
}

static void fallback_glPixelStorei(api::GLenum pname, api::GLint param)
{
	log() << warning << "PixelStorei not available." << std::endl;
}

static void fallback_glPixelTexGenParameterfSGIS(api::GLenum pname, api::GLfloat param)
{
	log() << warning << "PixelTexGenParameterfSGIS not available." << std::endl;
}

static void fallback_glPixelTexGenParameterfvSGIS(api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "PixelTexGenParameterfvSGIS not available." << std::endl;
}

static void fallback_glPixelTexGenParameteriSGIS(api::GLenum pname, api::GLint param)
{
	log() << warning << "PixelTexGenParameteriSGIS not available." << std::endl;
}

static void fallback_glPixelTexGenParameterivSGIS(api::GLenum pname, const api::GLint* params)
{
	log() << warning << "PixelTexGenParameterivSGIS not available." << std::endl;
}

static void fallback_glPixelTexGenSGIX(api::GLenum mode)
{
	log() << warning << "PixelTexGenSGIX not available." << std::endl;
}

static void fallback_glPixelTransferf(api::GLenum pname, api::GLfloat param)
{
	log() << warning << "PixelTransferf not available." << std::endl;
}

static void fallback_glPixelTransferi(api::GLenum pname, api::GLint param)
{
	log() << warning << "PixelTransferi not available." << std::endl;
}

static void fallback_glPixelTransformParameterfEXT(api::GLenum target, api::GLenum pname, api::GLfloat param)
{
	log() << warning << "PixelTransformParameterfEXT not available." << std::endl;
}

static void fallback_glPixelTransformParameterfvEXT(api::GLenum target, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "PixelTransformParameterfvEXT not available." << std::endl;
}

static void fallback_glPixelTransformParameteriEXT(api::GLenum target, api::GLenum pname, api::GLint param)
{
	log() << warning << "PixelTransformParameteriEXT not available." << std::endl;
}

static void fallback_glPixelTransformParameterivEXT(api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "PixelTransformParameterivEXT not available." << std::endl;
}

static void fallback_glPixelZoom(api::GLfloat xfactor, api::GLfloat yfactor)
{
	log() << warning << "PixelZoom not available." << std::endl;
}

static void fallback_glPointParameterf(api::GLenum pname, api::GLfloat param)
{
	log() << warning << "PointParameterf not available." << std::endl;
}

static void fallback_glPointParameterfARB(api::GLenum pname, api::GLfloat param)
{
	log() << warning << "PointParameterfARB not available." << std::endl;
}

static void fallback_glPointParameterfEXT(api::GLenum pname, api::GLfloat param)
{
	log() << warning << "PointParameterfEXT not available." << std::endl;
}

static void fallback_glPointParameterfSGIS(api::GLenum pname, api::GLfloat param)
{
	log() << warning << "PointParameterfSGIS not available." << std::endl;
}

static void fallback_glPointParameterfv(api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "PointParameterfv not available." << std::endl;
}

static void fallback_glPointParameterfvARB(api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "PointParameterfvARB not available." << std::endl;
}

static void fallback_glPointParameterfvEXT(api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "PointParameterfvEXT not available." << std::endl;
}

static void fallback_glPointParameterfvSGIS(api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "PointParameterfvSGIS not available." << std::endl;
}

static void fallback_glPointParameteri(api::GLenum pname, api::GLint param)
{
	log() << warning << "PointParameteri not available." << std::endl;
}

static void fallback_glPointParameteriNV(api::GLenum pname, api::GLint param)
{
	log() << warning << "PointParameteriNV not available." << std::endl;
}

static void fallback_glPointParameteriv(api::GLenum pname, const api::GLint* params)
{
	log() << warning << "PointParameteriv not available." << std::endl;
}

static void fallback_glPointParameterivNV(api::GLenum pname, const api::GLint* params)
{
	log() << warning << "PointParameterivNV not available." << std::endl;
}

static void fallback_glPointSize(api::GLfloat size)
{
	log() << warning << "PointSize not available." << std::endl;
}

static api::GLint fallback_glPollAsyncSGIX(api::GLuint* markerp)
{
	log() << warning << "PollAsyncSGIX not available." << std::endl;
}

static api::GLint fallback_glPollInstrumentsSGIX(api::GLint* marker_p)
{
	log() << warning << "PollInstrumentsSGIX not available." << std::endl;
}

static void fallback_glPolygonMode(api::GLenum face, api::GLenum mode)
{
	log() << warning << "PolygonMode not available." << std::endl;
}

static void fallback_glPolygonOffset(api::GLfloat factor, api::GLfloat units)
{
	log() << warning << "PolygonOffset not available." << std::endl;
}

static void fallback_glPolygonOffsetEXT(api::GLfloat factor, api::GLfloat bias)
{
	log() << warning << "PolygonOffsetEXT not available." << std::endl;
}

static void fallback_glPolygonStipple(const api::GLubyte* mask)
{
	log() << warning << "PolygonStipple not available." << std::endl;
}

static void fallback_glPopAttrib()
{
	log() << warning << "PopAttrib not available." << std::endl;
}

static void fallback_glPopClientAttrib()
{
	log() << warning << "PopClientAttrib not available." << std::endl;
}

static void fallback_glPopMatrix()
{
	log() << warning << "PopMatrix not available." << std::endl;
}

static void fallback_glPopName()
{
	log() << warning << "PopName not available." << std::endl;
}

static void fallback_glPresentFrameDualFillNV(api::GLuint video_slot, api::GLuint64EXT minPresentTime, api::GLuint beginPresentTimeId, api::GLuint presentDurationId, api::GLenum type, api::GLenum target0, api::GLuint fill0, api::GLenum target1, api::GLuint fill1, api::GLenum target2, api::GLuint fill2, api::GLenum target3, api::GLuint fill3)
{
	log() << warning << "PresentFrameDualFillNV not available." << std::endl;
}

static void fallback_glPresentFrameKeyedNV(api::GLuint video_slot, api::GLuint64EXT minPresentTime, api::GLuint beginPresentTimeId, api::GLuint presentDurationId, api::GLenum type, api::GLenum target0, api::GLuint fill0, api::GLuint key0, api::GLenum target1, api::GLuint fill1, api::GLuint key1)
{
	log() << warning << "PresentFrameKeyedNV not available." << std::endl;
}

static void fallback_glPrimitiveRestartIndex(api::GLuint index)
{
	log() << warning << "PrimitiveRestartIndex not available." << std::endl;
}

static void fallback_glPrimitiveRestartIndexNV(api::GLuint index)
{
	log() << warning << "PrimitiveRestartIndexNV not available." << std::endl;
}

static void fallback_glPrimitiveRestartNV()
{
	log() << warning << "PrimitiveRestartNV not available." << std::endl;
}

static void fallback_glPrioritizeTextures(api::GLsizei n, const api::GLuint* textures, const api::GLclampf* priorities)
{
	log() << warning << "PrioritizeTextures not available." << std::endl;
}

static void fallback_glPrioritizeTexturesEXT(api::GLsizei n, const api::GLuint* textures, const api::GLclampf* priorities)
{
	log() << warning << "PrioritizeTexturesEXT not available." << std::endl;
}

static void fallback_glProgramBufferParametersIivNV(api::GLenum target, api::GLuint buffer, api::GLuint index, api::GLsizei count, const api::GLint* params)
{
	log() << warning << "ProgramBufferParametersIivNV not available." << std::endl;
}

static void fallback_glProgramBufferParametersIuivNV(api::GLenum target, api::GLuint buffer, api::GLuint index, api::GLsizei count, const api::GLuint* params)
{
	log() << warning << "ProgramBufferParametersIuivNV not available." << std::endl;
}

static void fallback_glProgramBufferParametersfvNV(api::GLenum target, api::GLuint buffer, api::GLuint index, api::GLsizei count, const api::GLfloat* params)
{
	log() << warning << "ProgramBufferParametersfvNV not available." << std::endl;
}

static void fallback_glProgramEnvParameter4dARB(api::GLenum target, api::GLuint index, api::GLdouble x, api::GLdouble y, api::GLdouble z, api::GLdouble w)
{
	log() << warning << "ProgramEnvParameter4dARB not available." << std::endl;
}

static void fallback_glProgramEnvParameter4dvARB(api::GLenum target, api::GLuint index, const api::GLdouble* params)
{
	log() << warning << "ProgramEnvParameter4dvARB not available." << std::endl;
}

static void fallback_glProgramEnvParameter4fARB(api::GLenum target, api::GLuint index, api::GLfloat x, api::GLfloat y, api::GLfloat z, api::GLfloat w)
{
	log() << warning << "ProgramEnvParameter4fARB not available." << std::endl;
}

static void fallback_glProgramEnvParameter4fvARB(api::GLenum target, api::GLuint index, const api::GLfloat* params)
{
	log() << warning << "ProgramEnvParameter4fvARB not available." << std::endl;
}

static void fallback_glProgramEnvParameterI4iNV(api::GLenum target, api::GLuint index, api::GLint x, api::GLint y, api::GLint z, api::GLint w)
{
	log() << warning << "ProgramEnvParameterI4iNV not available." << std::endl;
}

static void fallback_glProgramEnvParameterI4ivNV(api::GLenum target, api::GLuint index, const api::GLint* params)
{
	log() << warning << "ProgramEnvParameterI4ivNV not available." << std::endl;
}

static void fallback_glProgramEnvParameterI4uiNV(api::GLenum target, api::GLuint index, api::GLuint x, api::GLuint y, api::GLuint z, api::GLuint w)
{
	log() << warning << "ProgramEnvParameterI4uiNV not available." << std::endl;
}

static void fallback_glProgramEnvParameterI4uivNV(api::GLenum target, api::GLuint index, const api::GLuint* params)
{
	log() << warning << "ProgramEnvParameterI4uivNV not available." << std::endl;
}

static void fallback_glProgramEnvParameters4fvEXT(api::GLenum target, api::GLuint index, api::GLsizei count, const api::GLfloat* params)
{
	log() << warning << "ProgramEnvParameters4fvEXT not available." << std::endl;
}

static void fallback_glProgramEnvParametersI4ivNV(api::GLenum target, api::GLuint index, api::GLsizei count, const api::GLint* params)
{
	log() << warning << "ProgramEnvParametersI4ivNV not available." << std::endl;
}

static void fallback_glProgramEnvParametersI4uivNV(api::GLenum target, api::GLuint index, api::GLsizei count, const api::GLuint* params)
{
	log() << warning << "ProgramEnvParametersI4uivNV not available." << std::endl;
}

static void fallback_glProgramLocalParameter4dARB(api::GLenum target, api::GLuint index, api::GLdouble x, api::GLdouble y, api::GLdouble z, api::GLdouble w)
{
	log() << warning << "ProgramLocalParameter4dARB not available." << std::endl;
}

static void fallback_glProgramLocalParameter4dvARB(api::GLenum target, api::GLuint index, const api::GLdouble* params)
{
	log() << warning << "ProgramLocalParameter4dvARB not available." << std::endl;
}

static void fallback_glProgramLocalParameter4fARB(api::GLenum target, api::GLuint index, api::GLfloat x, api::GLfloat y, api::GLfloat z, api::GLfloat w)
{
	log() << warning << "ProgramLocalParameter4fARB not available." << std::endl;
}

static void fallback_glProgramLocalParameter4fvARB(api::GLenum target, api::GLuint index, const api::GLfloat* params)
{
	log() << warning << "ProgramLocalParameter4fvARB not available." << std::endl;
}

static void fallback_glProgramLocalParameterI4iNV(api::GLenum target, api::GLuint index, api::GLint x, api::GLint y, api::GLint z, api::GLint w)
{
	log() << warning << "ProgramLocalParameterI4iNV not available." << std::endl;
}

static void fallback_glProgramLocalParameterI4ivNV(api::GLenum target, api::GLuint index, const api::GLint* params)
{
	log() << warning << "ProgramLocalParameterI4ivNV not available." << std::endl;
}

static void fallback_glProgramLocalParameterI4uiNV(api::GLenum target, api::GLuint index, api::GLuint x, api::GLuint y, api::GLuint z, api::GLuint w)
{
	log() << warning << "ProgramLocalParameterI4uiNV not available." << std::endl;
}

static void fallback_glProgramLocalParameterI4uivNV(api::GLenum target, api::GLuint index, const api::GLuint* params)
{
	log() << warning << "ProgramLocalParameterI4uivNV not available." << std::endl;
}

static void fallback_glProgramLocalParameters4fvEXT(api::GLenum target, api::GLuint index, api::GLsizei count, const api::GLfloat* params)
{
	log() << warning << "ProgramLocalParameters4fvEXT not available." << std::endl;
}

static void fallback_glProgramLocalParametersI4ivNV(api::GLenum target, api::GLuint index, api::GLsizei count, const api::GLint* params)
{
	log() << warning << "ProgramLocalParametersI4ivNV not available." << std::endl;
}

static void fallback_glProgramLocalParametersI4uivNV(api::GLenum target, api::GLuint index, api::GLsizei count, const api::GLuint* params)
{
	log() << warning << "ProgramLocalParametersI4uivNV not available." << std::endl;
}

static void fallback_glProgramNamedParameter4dNV(api::GLuint id, api::GLsizei len, const api::GLubyte* name, api::GLdouble x, api::GLdouble y, api::GLdouble z, api::GLdouble w)
{
	log() << warning << "ProgramNamedParameter4dNV not available." << std::endl;
}

static void fallback_glProgramNamedParameter4dvNV(api::GLuint id, api::GLsizei len, const api::GLubyte* name, const api::GLdouble* v)
{
	log() << warning << "ProgramNamedParameter4dvNV not available." << std::endl;
}

static void fallback_glProgramNamedParameter4fNV(api::GLuint id, api::GLsizei len, const api::GLubyte* name, api::GLfloat x, api::GLfloat y, api::GLfloat z, api::GLfloat w)
{
	log() << warning << "ProgramNamedParameter4fNV not available." << std::endl;
}

static void fallback_glProgramNamedParameter4fvNV(api::GLuint id, api::GLsizei len, const api::GLubyte* name, const api::GLfloat* v)
{
	log() << warning << "ProgramNamedParameter4fvNV not available." << std::endl;
}

static void fallback_glProgramParameter4dNV(api::GLenum target, api::GLuint index, api::GLdouble x, api::GLdouble y, api::GLdouble z, api::GLdouble w)
{
	log() << warning << "ProgramParameter4dNV not available." << std::endl;
}

static void fallback_glProgramParameter4dvNV(api::GLenum target, api::GLuint index, const api::GLdouble* v)
{
	log() << warning << "ProgramParameter4dvNV not available." << std::endl;
}

static void fallback_glProgramParameter4fNV(api::GLenum target, api::GLuint index, api::GLfloat x, api::GLfloat y, api::GLfloat z, api::GLfloat w)
{
	log() << warning << "ProgramParameter4fNV not available." << std::endl;
}

static void fallback_glProgramParameter4fvNV(api::GLenum target, api::GLuint index, const api::GLfloat* v)
{
	log() << warning << "ProgramParameter4fvNV not available." << std::endl;
}

static void fallback_glProgramParameteri(api::GLuint program, api::GLenum pname, api::GLint value)
{
	log() << warning << "ProgramParameteri not available." << std::endl;
}

static void fallback_glProgramParameteriARB(api::GLuint program, api::GLenum pname, api::GLint value)
{
	log() << warning << "ProgramParameteriARB not available." << std::endl;
}

static void fallback_glProgramParameteriEXT(api::GLuint program, api::GLenum pname, api::GLint value)
{
	log() << warning << "ProgramParameteriEXT not available." << std::endl;
}

static void fallback_glProgramParameters4dvNV(api::GLenum target, api::GLuint index, api::GLuint count, const api::GLdouble* v)
{
	log() << warning << "ProgramParameters4dvNV not available." << std::endl;
}

static void fallback_glProgramParameters4fvNV(api::GLenum target, api::GLuint index, api::GLuint count, const api::GLfloat* v)
{
	log() << warning << "ProgramParameters4fvNV not available." << std::endl;
}

static void fallback_glProgramStringARB(api::GLenum target, api::GLenum format, api::GLsizei len, const api::GLvoid* string)
{
	log() << warning << "ProgramStringARB not available." << std::endl;
}

static void fallback_glProgramUniform1dEXT(api::GLuint program, api::GLint location, api::GLdouble x)
{
	log() << warning << "ProgramUniform1dEXT not available." << std::endl;
}

static void fallback_glProgramUniform1dvEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLdouble* value)
{
	log() << warning << "ProgramUniform1dvEXT not available." << std::endl;
}

static void fallback_glProgramUniform1fEXT(api::GLuint program, api::GLint location, api::GLfloat v0)
{
	log() << warning << "ProgramUniform1fEXT not available." << std::endl;
}

static void fallback_glProgramUniform1fvEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLfloat* value)
{
	log() << warning << "ProgramUniform1fvEXT not available." << std::endl;
}

static void fallback_glProgramUniform1iEXT(api::GLuint program, api::GLint location, api::GLint v0)
{
	log() << warning << "ProgramUniform1iEXT not available." << std::endl;
}

static void fallback_glProgramUniform1ivEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLint* value)
{
	log() << warning << "ProgramUniform1ivEXT not available." << std::endl;
}

static void fallback_glProgramUniform1uiEXT(api::GLuint program, api::GLint location, api::GLuint v0)
{
	log() << warning << "ProgramUniform1uiEXT not available." << std::endl;
}

static void fallback_glProgramUniform1uivEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLuint* value)
{
	log() << warning << "ProgramUniform1uivEXT not available." << std::endl;
}

static void fallback_glProgramUniform2dEXT(api::GLuint program, api::GLint location, api::GLdouble x, api::GLdouble y)
{
	log() << warning << "ProgramUniform2dEXT not available." << std::endl;
}

static void fallback_glProgramUniform2dvEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLdouble* value)
{
	log() << warning << "ProgramUniform2dvEXT not available." << std::endl;
}

static void fallback_glProgramUniform2fEXT(api::GLuint program, api::GLint location, api::GLfloat v0, api::GLfloat v1)
{
	log() << warning << "ProgramUniform2fEXT not available." << std::endl;
}

static void fallback_glProgramUniform2fvEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLfloat* value)
{
	log() << warning << "ProgramUniform2fvEXT not available." << std::endl;
}

static void fallback_glProgramUniform2iEXT(api::GLuint program, api::GLint location, api::GLint v0, api::GLint v1)
{
	log() << warning << "ProgramUniform2iEXT not available." << std::endl;
}

static void fallback_glProgramUniform2ivEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLint* value)
{
	log() << warning << "ProgramUniform2ivEXT not available." << std::endl;
}

static void fallback_glProgramUniform2uiEXT(api::GLuint program, api::GLint location, api::GLuint v0, api::GLuint v1)
{
	log() << warning << "ProgramUniform2uiEXT not available." << std::endl;
}

static void fallback_glProgramUniform2uivEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLuint* value)
{
	log() << warning << "ProgramUniform2uivEXT not available." << std::endl;
}

static void fallback_glProgramUniform3dEXT(api::GLuint program, api::GLint location, api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "ProgramUniform3dEXT not available." << std::endl;
}

static void fallback_glProgramUniform3dvEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLdouble* value)
{
	log() << warning << "ProgramUniform3dvEXT not available." << std::endl;
}

static void fallback_glProgramUniform3fEXT(api::GLuint program, api::GLint location, api::GLfloat v0, api::GLfloat v1, api::GLfloat v2)
{
	log() << warning << "ProgramUniform3fEXT not available." << std::endl;
}

static void fallback_glProgramUniform3fvEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLfloat* value)
{
	log() << warning << "ProgramUniform3fvEXT not available." << std::endl;
}

static void fallback_glProgramUniform3iEXT(api::GLuint program, api::GLint location, api::GLint v0, api::GLint v1, api::GLint v2)
{
	log() << warning << "ProgramUniform3iEXT not available." << std::endl;
}

static void fallback_glProgramUniform3ivEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLint* value)
{
	log() << warning << "ProgramUniform3ivEXT not available." << std::endl;
}

static void fallback_glProgramUniform3uiEXT(api::GLuint program, api::GLint location, api::GLuint v0, api::GLuint v1, api::GLuint v2)
{
	log() << warning << "ProgramUniform3uiEXT not available." << std::endl;
}

static void fallback_glProgramUniform3uivEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLuint* value)
{
	log() << warning << "ProgramUniform3uivEXT not available." << std::endl;
}

static void fallback_glProgramUniform4dEXT(api::GLuint program, api::GLint location, api::GLdouble x, api::GLdouble y, api::GLdouble z, api::GLdouble w)
{
	log() << warning << "ProgramUniform4dEXT not available." << std::endl;
}

static void fallback_glProgramUniform4dvEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLdouble* value)
{
	log() << warning << "ProgramUniform4dvEXT not available." << std::endl;
}

static void fallback_glProgramUniform4fEXT(api::GLuint program, api::GLint location, api::GLfloat v0, api::GLfloat v1, api::GLfloat v2, api::GLfloat v3)
{
	log() << warning << "ProgramUniform4fEXT not available." << std::endl;
}

static void fallback_glProgramUniform4fvEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLfloat* value)
{
	log() << warning << "ProgramUniform4fvEXT not available." << std::endl;
}

static void fallback_glProgramUniform4iEXT(api::GLuint program, api::GLint location, api::GLint v0, api::GLint v1, api::GLint v2, api::GLint v3)
{
	log() << warning << "ProgramUniform4iEXT not available." << std::endl;
}

static void fallback_glProgramUniform4ivEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLint* value)
{
	log() << warning << "ProgramUniform4ivEXT not available." << std::endl;
}

static void fallback_glProgramUniform4uiEXT(api::GLuint program, api::GLint location, api::GLuint v0, api::GLuint v1, api::GLuint v2, api::GLuint v3)
{
	log() << warning << "ProgramUniform4uiEXT not available." << std::endl;
}

static void fallback_glProgramUniform4uivEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLuint* value)
{
	log() << warning << "ProgramUniform4uivEXT not available." << std::endl;
}

static void fallback_glProgramUniformMatrix2dvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "ProgramUniformMatrix2dvEXT not available." << std::endl;
}

static void fallback_glProgramUniformMatrix2fvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "ProgramUniformMatrix2fvEXT not available." << std::endl;
}

static void fallback_glProgramUniformMatrix2x3dvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "ProgramUniformMatrix2x3dvEXT not available." << std::endl;
}

static void fallback_glProgramUniformMatrix2x3fvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "ProgramUniformMatrix2x3fvEXT not available." << std::endl;
}

static void fallback_glProgramUniformMatrix2x4dvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "ProgramUniformMatrix2x4dvEXT not available." << std::endl;
}

static void fallback_glProgramUniformMatrix2x4fvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "ProgramUniformMatrix2x4fvEXT not available." << std::endl;
}

static void fallback_glProgramUniformMatrix3dvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "ProgramUniformMatrix3dvEXT not available." << std::endl;
}

static void fallback_glProgramUniformMatrix3fvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "ProgramUniformMatrix3fvEXT not available." << std::endl;
}

static void fallback_glProgramUniformMatrix3x2dvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "ProgramUniformMatrix3x2dvEXT not available." << std::endl;
}

static void fallback_glProgramUniformMatrix3x2fvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "ProgramUniformMatrix3x2fvEXT not available." << std::endl;
}

static void fallback_glProgramUniformMatrix3x4dvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "ProgramUniformMatrix3x4dvEXT not available." << std::endl;
}

static void fallback_glProgramUniformMatrix3x4fvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "ProgramUniformMatrix3x4fvEXT not available." << std::endl;
}

static void fallback_glProgramUniformMatrix4dvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "ProgramUniformMatrix4dvEXT not available." << std::endl;
}

static void fallback_glProgramUniformMatrix4fvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "ProgramUniformMatrix4fvEXT not available." << std::endl;
}

static void fallback_glProgramUniformMatrix4x2dvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "ProgramUniformMatrix4x2dvEXT not available." << std::endl;
}

static void fallback_glProgramUniformMatrix4x2fvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "ProgramUniformMatrix4x2fvEXT not available." << std::endl;
}

static void fallback_glProgramUniformMatrix4x3dvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "ProgramUniformMatrix4x3dvEXT not available." << std::endl;
}

static void fallback_glProgramUniformMatrix4x3fvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "ProgramUniformMatrix4x3fvEXT not available." << std::endl;
}

static void fallback_glProgramUniformui64NV(api::GLuint program, api::GLint location, api::GLuint64EXT value)
{
	log() << warning << "ProgramUniformui64NV not available." << std::endl;
}

static void fallback_glProgramUniformui64vNV(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLuint64EXT* value)
{
	log() << warning << "ProgramUniformui64vNV not available." << std::endl;
}

static void fallback_glProgramVertexLimitNV(api::GLenum target, api::GLint limit)
{
	log() << warning << "ProgramVertexLimitNV not available." << std::endl;
}

static void fallback_glProvokingVertex(api::GLenum mode)
{
	log() << warning << "ProvokingVertex not available." << std::endl;
}

static void fallback_glProvokingVertexEXT(api::GLenum mode)
{
	log() << warning << "ProvokingVertexEXT not available." << std::endl;
}

static void fallback_glPushAttrib(api::GLbitfield mask)
{
	log() << warning << "PushAttrib not available." << std::endl;
}

static void fallback_glPushClientAttrib(api::GLbitfield mask)
{
	log() << warning << "PushClientAttrib not available." << std::endl;
}

static void fallback_glPushClientAttribDefaultEXT(api::GLbitfield mask)
{
	log() << warning << "PushClientAttribDefaultEXT not available." << std::endl;
}

static void fallback_glPushMatrix()
{
	log() << warning << "PushMatrix not available." << std::endl;
}

static void fallback_glPushName(api::GLuint name)
{
	log() << warning << "PushName not available." << std::endl;
}

static void fallback_glQueryCounter(api::GLuint id, api::GLenum target)
{
	log() << warning << "QueryCounter not available." << std::endl;
}

static void fallback_glRasterPos2d(api::GLdouble x, api::GLdouble y)
{
	log() << warning << "RasterPos2d not available." << std::endl;
}

static void fallback_glRasterPos2dv(const api::GLdouble* v)
{
	log() << warning << "RasterPos2dv not available." << std::endl;
}

static void fallback_glRasterPos2f(api::GLfloat x, api::GLfloat y)
{
	log() << warning << "RasterPos2f not available." << std::endl;
}

static void fallback_glRasterPos2fv(const api::GLfloat* v)
{
	log() << warning << "RasterPos2fv not available." << std::endl;
}

static void fallback_glRasterPos2i(api::GLint x, api::GLint y)
{
	log() << warning << "RasterPos2i not available." << std::endl;
}

static void fallback_glRasterPos2iv(const api::GLint* v)
{
	log() << warning << "RasterPos2iv not available." << std::endl;
}

static void fallback_glRasterPos2s(api::GLshort x, api::GLshort y)
{
	log() << warning << "RasterPos2s not available." << std::endl;
}

static void fallback_glRasterPos2sv(const api::GLshort* v)
{
	log() << warning << "RasterPos2sv not available." << std::endl;
}

static void fallback_glRasterPos3d(api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "RasterPos3d not available." << std::endl;
}

static void fallback_glRasterPos3dv(const api::GLdouble* v)
{
	log() << warning << "RasterPos3dv not available." << std::endl;
}

static void fallback_glRasterPos3f(api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "RasterPos3f not available." << std::endl;
}

static void fallback_glRasterPos3fv(const api::GLfloat* v)
{
	log() << warning << "RasterPos3fv not available." << std::endl;
}

static void fallback_glRasterPos3i(api::GLint x, api::GLint y, api::GLint z)
{
	log() << warning << "RasterPos3i not available." << std::endl;
}

static void fallback_glRasterPos3iv(const api::GLint* v)
{
	log() << warning << "RasterPos3iv not available." << std::endl;
}

static void fallback_glRasterPos3s(api::GLshort x, api::GLshort y, api::GLshort z)
{
	log() << warning << "RasterPos3s not available." << std::endl;
}

static void fallback_glRasterPos3sv(const api::GLshort* v)
{
	log() << warning << "RasterPos3sv not available." << std::endl;
}

static void fallback_glRasterPos4d(api::GLdouble x, api::GLdouble y, api::GLdouble z, api::GLdouble w)
{
	log() << warning << "RasterPos4d not available." << std::endl;
}

static void fallback_glRasterPos4dv(const api::GLdouble* v)
{
	log() << warning << "RasterPos4dv not available." << std::endl;
}

static void fallback_glRasterPos4f(api::GLfloat x, api::GLfloat y, api::GLfloat z, api::GLfloat w)
{
	log() << warning << "RasterPos4f not available." << std::endl;
}

static void fallback_glRasterPos4fv(const api::GLfloat* v)
{
	log() << warning << "RasterPos4fv not available." << std::endl;
}

static void fallback_glRasterPos4i(api::GLint x, api::GLint y, api::GLint z, api::GLint w)
{
	log() << warning << "RasterPos4i not available." << std::endl;
}

static void fallback_glRasterPos4iv(const api::GLint* v)
{
	log() << warning << "RasterPos4iv not available." << std::endl;
}

static void fallback_glRasterPos4s(api::GLshort x, api::GLshort y, api::GLshort z, api::GLshort w)
{
	log() << warning << "RasterPos4s not available." << std::endl;
}

static void fallback_glRasterPos4sv(const api::GLshort* v)
{
	log() << warning << "RasterPos4sv not available." << std::endl;
}

static void fallback_glReadBuffer(api::GLenum mode)
{
	log() << warning << "ReadBuffer not available." << std::endl;
}

static void fallback_glReadInstrumentsSGIX(api::GLint marker)
{
	log() << warning << "ReadInstrumentsSGIX not available." << std::endl;
}

static void fallback_glReadPixels(api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height, api::GLenum format, api::GLenum type, api::GLvoid* pixels)
{
	log() << warning << "ReadPixels not available." << std::endl;
}

static void fallback_glRectd(api::GLdouble x1, api::GLdouble y1, api::GLdouble x2, api::GLdouble y2)
{
	log() << warning << "Rectd not available." << std::endl;
}

static void fallback_glRectdv(const api::GLdouble* v1, const api::GLdouble* v2)
{
	log() << warning << "Rectdv not available." << std::endl;
}

static void fallback_glRectf(api::GLfloat x1, api::GLfloat y1, api::GLfloat x2, api::GLfloat y2)
{
	log() << warning << "Rectf not available." << std::endl;
}

static void fallback_glRectfv(const api::GLfloat* v1, const api::GLfloat* v2)
{
	log() << warning << "Rectfv not available." << std::endl;
}

static void fallback_glRecti(api::GLint x1, api::GLint y1, api::GLint x2, api::GLint y2)
{
	log() << warning << "Recti not available." << std::endl;
}

static void fallback_glRectiv(const api::GLint* v1, const api::GLint* v2)
{
	log() << warning << "Rectiv not available." << std::endl;
}

static void fallback_glRects(api::GLshort x1, api::GLshort y1, api::GLshort x2, api::GLshort y2)
{
	log() << warning << "Rects not available." << std::endl;
}

static void fallback_glRectsv(const api::GLshort* v1, const api::GLshort* v2)
{
	log() << warning << "Rectsv not available." << std::endl;
}

static void fallback_glReferencePlaneSGIX(const api::GLdouble* equation)
{
	log() << warning << "ReferencePlaneSGIX not available." << std::endl;
}

static api::GLint fallback_glRenderMode(api::GLenum mode)
{
	log() << warning << "RenderMode not available." << std::endl;
}

static void fallback_glRenderbufferStorage(api::GLenum target, api::GLenum internalformat, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "RenderbufferStorage not available." << std::endl;
}

static void fallback_glRenderbufferStorageEXT(api::GLenum target, api::GLenum internalformat, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "RenderbufferStorageEXT not available." << std::endl;
}

static void fallback_glRenderbufferStorageMultisample(api::GLenum target, api::GLsizei samples, api::GLenum internalformat, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "RenderbufferStorageMultisample not available." << std::endl;
}

static void fallback_glRenderbufferStorageMultisampleCoverageNV(api::GLenum target, api::GLsizei coverageSamples, api::GLsizei colorSamples, api::GLenum internalformat, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "RenderbufferStorageMultisampleCoverageNV not available." << std::endl;
}

static void fallback_glRenderbufferStorageMultisampleEXT(api::GLenum target, api::GLsizei samples, api::GLenum internalformat, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "RenderbufferStorageMultisampleEXT not available." << std::endl;
}

static void fallback_glReplacementCodePointerSUN(api::GLenum type, api::GLsizei stride, const api::GLvoid** pointer)
{
	log() << warning << "ReplacementCodePointerSUN not available." << std::endl;
}

static void fallback_glReplacementCodeubSUN(api::GLubyte code)
{
	log() << warning << "ReplacementCodeubSUN not available." << std::endl;
}

static void fallback_glReplacementCodeubvSUN(const api::GLubyte* code)
{
	log() << warning << "ReplacementCodeubvSUN not available." << std::endl;
}

static void fallback_glReplacementCodeuiColor3fVertex3fSUN(api::GLuint rc, api::GLfloat r, api::GLfloat g, api::GLfloat b, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "ReplacementCodeuiColor3fVertex3fSUN not available." << std::endl;
}

static void fallback_glReplacementCodeuiColor3fVertex3fvSUN(const api::GLuint* rc, const api::GLfloat* c, const api::GLfloat* v)
{
	log() << warning << "ReplacementCodeuiColor3fVertex3fvSUN not available." << std::endl;
}

static void fallback_glReplacementCodeuiColor4fNormal3fVertex3fSUN(api::GLuint rc, api::GLfloat r, api::GLfloat g, api::GLfloat b, api::GLfloat a, api::GLfloat nx, api::GLfloat ny, api::GLfloat nz, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "ReplacementCodeuiColor4fNormal3fVertex3fSUN not available." << std::endl;
}

static void fallback_glReplacementCodeuiColor4fNormal3fVertex3fvSUN(const api::GLuint* rc, const api::GLfloat* c, const api::GLfloat* n, const api::GLfloat* v)
{
	log() << warning << "ReplacementCodeuiColor4fNormal3fVertex3fvSUN not available." << std::endl;
}

static void fallback_glReplacementCodeuiColor4ubVertex3fSUN(api::GLuint rc, api::GLubyte r, api::GLubyte g, api::GLubyte b, api::GLubyte a, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "ReplacementCodeuiColor4ubVertex3fSUN not available." << std::endl;
}

static void fallback_glReplacementCodeuiColor4ubVertex3fvSUN(const api::GLuint* rc, const api::GLubyte* c, const api::GLfloat* v)
{
	log() << warning << "ReplacementCodeuiColor4ubVertex3fvSUN not available." << std::endl;
}

static void fallback_glReplacementCodeuiNormal3fVertex3fSUN(api::GLuint rc, api::GLfloat nx, api::GLfloat ny, api::GLfloat nz, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "ReplacementCodeuiNormal3fVertex3fSUN not available." << std::endl;
}

static void fallback_glReplacementCodeuiNormal3fVertex3fvSUN(const api::GLuint* rc, const api::GLfloat* n, const api::GLfloat* v)
{
	log() << warning << "ReplacementCodeuiNormal3fVertex3fvSUN not available." << std::endl;
}

static void fallback_glReplacementCodeuiSUN(api::GLuint code)
{
	log() << warning << "ReplacementCodeuiSUN not available." << std::endl;
}

static void fallback_glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN(api::GLuint rc, api::GLfloat s, api::GLfloat t, api::GLfloat r, api::GLfloat g, api::GLfloat b, api::GLfloat a, api::GLfloat nx, api::GLfloat ny, api::GLfloat nz, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "ReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN not available." << std::endl;
}

static void fallback_glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN(const api::GLuint* rc, const api::GLfloat* tc, const api::GLfloat* c, const api::GLfloat* n, const api::GLfloat* v)
{
	log() << warning << "ReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN not available." << std::endl;
}

static void fallback_glReplacementCodeuiTexCoord2fNormal3fVertex3fSUN(api::GLuint rc, api::GLfloat s, api::GLfloat t, api::GLfloat nx, api::GLfloat ny, api::GLfloat nz, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "ReplacementCodeuiTexCoord2fNormal3fVertex3fSUN not available." << std::endl;
}

static void fallback_glReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN(const api::GLuint* rc, const api::GLfloat* tc, const api::GLfloat* n, const api::GLfloat* v)
{
	log() << warning << "ReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN not available." << std::endl;
}

static void fallback_glReplacementCodeuiTexCoord2fVertex3fSUN(api::GLuint rc, api::GLfloat s, api::GLfloat t, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "ReplacementCodeuiTexCoord2fVertex3fSUN not available." << std::endl;
}

static void fallback_glReplacementCodeuiTexCoord2fVertex3fvSUN(const api::GLuint* rc, const api::GLfloat* tc, const api::GLfloat* v)
{
	log() << warning << "ReplacementCodeuiTexCoord2fVertex3fvSUN not available." << std::endl;
}

static void fallback_glReplacementCodeuiVertex3fSUN(api::GLuint rc, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "ReplacementCodeuiVertex3fSUN not available." << std::endl;
}

static void fallback_glReplacementCodeuiVertex3fvSUN(const api::GLuint* rc, const api::GLfloat* v)
{
	log() << warning << "ReplacementCodeuiVertex3fvSUN not available." << std::endl;
}

static void fallback_glReplacementCodeuivSUN(const api::GLuint* code)
{
	log() << warning << "ReplacementCodeuivSUN not available." << std::endl;
}

static void fallback_glReplacementCodeusSUN(api::GLushort code)
{
	log() << warning << "ReplacementCodeusSUN not available." << std::endl;
}

static void fallback_glReplacementCodeusvSUN(const api::GLushort* code)
{
	log() << warning << "ReplacementCodeusvSUN not available." << std::endl;
}

static void fallback_glRequestResidentProgramsNV(api::GLsizei n, const api::GLuint* programs)
{
	log() << warning << "RequestResidentProgramsNV not available." << std::endl;
}

static void fallback_glResetHistogram(api::GLenum target)
{
	log() << warning << "ResetHistogram not available." << std::endl;
}

static void fallback_glResetHistogramEXT(api::GLenum target)
{
	log() << warning << "ResetHistogramEXT not available." << std::endl;
}

static void fallback_glResetMinmax(api::GLenum target)
{
	log() << warning << "ResetMinmax not available." << std::endl;
}

static void fallback_glResetMinmaxEXT(api::GLenum target)
{
	log() << warning << "ResetMinmaxEXT not available." << std::endl;
}

static void fallback_glResizeBuffersMESA()
{
	log() << warning << "ResizeBuffersMESA not available." << std::endl;
}

static void fallback_glResumeTransformFeedback()
{
	log() << warning << "ResumeTransformFeedback not available." << std::endl;
}

static void fallback_glResumeTransformFeedbackNV()
{
	log() << warning << "ResumeTransformFeedbackNV not available." << std::endl;
}

static void fallback_glRotated(api::GLdouble angle, api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "Rotated not available." << std::endl;
}

static void fallback_glRotatef(api::GLfloat angle, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "Rotatef not available." << std::endl;
}

static void fallback_glSampleCoverage(api::GLclampf value, api::GLboolean invert)
{
	log() << warning << "SampleCoverage not available." << std::endl;
}

static void fallback_glSampleCoverageARB(api::GLclampf value, api::GLboolean invert)
{
	log() << warning << "SampleCoverageARB not available." << std::endl;
}

static void fallback_glSampleMapATI(api::GLuint dst, api::GLuint interp, api::GLenum swizzle)
{
	log() << warning << "SampleMapATI not available." << std::endl;
}

static void fallback_glSampleMaskEXT(api::GLclampf value, api::GLboolean invert)
{
	log() << warning << "SampleMaskEXT not available." << std::endl;
}

static void fallback_glSampleMaskIndexedNV(api::GLuint index, api::GLbitfield mask)
{
	log() << warning << "SampleMaskIndexedNV not available." << std::endl;
}

static void fallback_glSampleMaskSGIS(api::GLclampf value, api::GLboolean invert)
{
	log() << warning << "SampleMaskSGIS not available." << std::endl;
}

static void fallback_glSampleMaski(api::GLuint index, api::GLbitfield mask)
{
	log() << warning << "SampleMaski not available." << std::endl;
}

static void fallback_glSamplePatternEXT(api::GLenum pattern)
{
	log() << warning << "SamplePatternEXT not available." << std::endl;
}

static void fallback_glSamplePatternSGIS(api::GLenum pattern)
{
	log() << warning << "SamplePatternSGIS not available." << std::endl;
}

static void fallback_glSamplerParameterIiv(api::GLuint sampler, api::GLenum pname, const api::GLint* param)
{
	log() << warning << "SamplerParameterIiv not available." << std::endl;
}

static void fallback_glSamplerParameterIuiv(api::GLuint sampler, api::GLenum pname, const api::GLuint* param)
{
	log() << warning << "SamplerParameterIuiv not available." << std::endl;
}

static void fallback_glSamplerParameterf(api::GLuint sampler, api::GLenum pname, api::GLfloat param)
{
	log() << warning << "SamplerParameterf not available." << std::endl;
}

static void fallback_glSamplerParameterfv(api::GLuint sampler, api::GLenum pname, const api::GLfloat* param)
{
	log() << warning << "SamplerParameterfv not available." << std::endl;
}

static void fallback_glSamplerParameteri(api::GLuint sampler, api::GLenum pname, api::GLint param)
{
	log() << warning << "SamplerParameteri not available." << std::endl;
}

static void fallback_glSamplerParameteriv(api::GLuint sampler, api::GLenum pname, const api::GLint* param)
{
	log() << warning << "SamplerParameteriv not available." << std::endl;
}

static void fallback_glScaled(api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "Scaled not available." << std::endl;
}

static void fallback_glScalef(api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "Scalef not available." << std::endl;
}

static void fallback_glScissor(api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "Scissor not available." << std::endl;
}

static void fallback_glSecondaryColor3b(api::GLbyte red, api::GLbyte green, api::GLbyte blue)
{
	log() << warning << "SecondaryColor3b not available." << std::endl;
}

static void fallback_glSecondaryColor3bEXT(api::GLbyte red, api::GLbyte green, api::GLbyte blue)
{
	log() << warning << "SecondaryColor3bEXT not available." << std::endl;
}

static void fallback_glSecondaryColor3bv(const api::GLbyte* v)
{
	log() << warning << "SecondaryColor3bv not available." << std::endl;
}

static void fallback_glSecondaryColor3bvEXT(const api::GLbyte* v)
{
	log() << warning << "SecondaryColor3bvEXT not available." << std::endl;
}

static void fallback_glSecondaryColor3d(api::GLdouble red, api::GLdouble green, api::GLdouble blue)
{
	log() << warning << "SecondaryColor3d not available." << std::endl;
}

static void fallback_glSecondaryColor3dEXT(api::GLdouble red, api::GLdouble green, api::GLdouble blue)
{
	log() << warning << "SecondaryColor3dEXT not available." << std::endl;
}

static void fallback_glSecondaryColor3dv(const api::GLdouble* v)
{
	log() << warning << "SecondaryColor3dv not available." << std::endl;
}

static void fallback_glSecondaryColor3dvEXT(const api::GLdouble* v)
{
	log() << warning << "SecondaryColor3dvEXT not available." << std::endl;
}

static void fallback_glSecondaryColor3f(api::GLfloat red, api::GLfloat green, api::GLfloat blue)
{
	log() << warning << "SecondaryColor3f not available." << std::endl;
}

static void fallback_glSecondaryColor3fEXT(api::GLfloat red, api::GLfloat green, api::GLfloat blue)
{
	log() << warning << "SecondaryColor3fEXT not available." << std::endl;
}

static void fallback_glSecondaryColor3fv(const api::GLfloat* v)
{
	log() << warning << "SecondaryColor3fv not available." << std::endl;
}

static void fallback_glSecondaryColor3fvEXT(const api::GLfloat* v)
{
	log() << warning << "SecondaryColor3fvEXT not available." << std::endl;
}

static void fallback_glSecondaryColor3hNV(api::GLhalfNV red, api::GLhalfNV green, api::GLhalfNV blue)
{
	log() << warning << "SecondaryColor3hNV not available." << std::endl;
}

static void fallback_glSecondaryColor3hvNV(const api::GLhalfNV* v)
{
	log() << warning << "SecondaryColor3hvNV not available." << std::endl;
}

static void fallback_glSecondaryColor3i(api::GLint red, api::GLint green, api::GLint blue)
{
	log() << warning << "SecondaryColor3i not available." << std::endl;
}

static void fallback_glSecondaryColor3iEXT(api::GLint red, api::GLint green, api::GLint blue)
{
	log() << warning << "SecondaryColor3iEXT not available." << std::endl;
}

static void fallback_glSecondaryColor3iv(const api::GLint* v)
{
	log() << warning << "SecondaryColor3iv not available." << std::endl;
}

static void fallback_glSecondaryColor3ivEXT(const api::GLint* v)
{
	log() << warning << "SecondaryColor3ivEXT not available." << std::endl;
}

static void fallback_glSecondaryColor3s(api::GLshort red, api::GLshort green, api::GLshort blue)
{
	log() << warning << "SecondaryColor3s not available." << std::endl;
}

static void fallback_glSecondaryColor3sEXT(api::GLshort red, api::GLshort green, api::GLshort blue)
{
	log() << warning << "SecondaryColor3sEXT not available." << std::endl;
}

static void fallback_glSecondaryColor3sv(const api::GLshort* v)
{
	log() << warning << "SecondaryColor3sv not available." << std::endl;
}

static void fallback_glSecondaryColor3svEXT(const api::GLshort* v)
{
	log() << warning << "SecondaryColor3svEXT not available." << std::endl;
}

static void fallback_glSecondaryColor3ub(api::GLubyte red, api::GLubyte green, api::GLubyte blue)
{
	log() << warning << "SecondaryColor3ub not available." << std::endl;
}

static void fallback_glSecondaryColor3ubEXT(api::GLubyte red, api::GLubyte green, api::GLubyte blue)
{
	log() << warning << "SecondaryColor3ubEXT not available." << std::endl;
}

static void fallback_glSecondaryColor3ubv(const api::GLubyte* v)
{
	log() << warning << "SecondaryColor3ubv not available." << std::endl;
}

static void fallback_glSecondaryColor3ubvEXT(const api::GLubyte* v)
{
	log() << warning << "SecondaryColor3ubvEXT not available." << std::endl;
}

static void fallback_glSecondaryColor3ui(api::GLuint red, api::GLuint green, api::GLuint blue)
{
	log() << warning << "SecondaryColor3ui not available." << std::endl;
}

static void fallback_glSecondaryColor3uiEXT(api::GLuint red, api::GLuint green, api::GLuint blue)
{
	log() << warning << "SecondaryColor3uiEXT not available." << std::endl;
}

static void fallback_glSecondaryColor3uiv(const api::GLuint* v)
{
	log() << warning << "SecondaryColor3uiv not available." << std::endl;
}

static void fallback_glSecondaryColor3uivEXT(const api::GLuint* v)
{
	log() << warning << "SecondaryColor3uivEXT not available." << std::endl;
}

static void fallback_glSecondaryColor3us(api::GLushort red, api::GLushort green, api::GLushort blue)
{
	log() << warning << "SecondaryColor3us not available." << std::endl;
}

static void fallback_glSecondaryColor3usEXT(api::GLushort red, api::GLushort green, api::GLushort blue)
{
	log() << warning << "SecondaryColor3usEXT not available." << std::endl;
}

static void fallback_glSecondaryColor3usv(const api::GLushort* v)
{
	log() << warning << "SecondaryColor3usv not available." << std::endl;
}

static void fallback_glSecondaryColor3usvEXT(const api::GLushort* v)
{
	log() << warning << "SecondaryColor3usvEXT not available." << std::endl;
}

static void fallback_glSecondaryColorFormatNV(api::GLint size, api::GLenum type, api::GLsizei stride)
{
	log() << warning << "SecondaryColorFormatNV not available." << std::endl;
}

static void fallback_glSecondaryColorP3ui(api::GLenum type, api::GLuint color)
{
	log() << warning << "SecondaryColorP3ui not available." << std::endl;
}

static void fallback_glSecondaryColorP3uiv(api::GLenum type, const api::GLuint* color)
{
	log() << warning << "SecondaryColorP3uiv not available." << std::endl;
}

static void fallback_glSecondaryColorPointer(api::GLint size, api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "SecondaryColorPointer not available." << std::endl;
}

static void fallback_glSecondaryColorPointerEXT(api::GLint size, api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "SecondaryColorPointerEXT not available." << std::endl;
}

static void fallback_glSecondaryColorPointerListIBM(api::GLint size, api::GLenum type, api::GLint stride, const api::GLvoid** pointer, api::GLint ptrstride)
{
	log() << warning << "SecondaryColorPointerListIBM not available." << std::endl;
}

static void fallback_glSelectBuffer(api::GLsizei size, api::GLuint* buffer)
{
	log() << warning << "SelectBuffer not available." << std::endl;
}

static void fallback_glSelectPerfMonitorCountersAMD(api::GLuint monitor, api::GLboolean enable, api::GLuint group, api::GLint numCounters, api::GLuint* counterList)
{
	log() << warning << "SelectPerfMonitorCountersAMD not available." << std::endl;
}

static void fallback_glSeparableFilter2D(api::GLenum target, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLenum format, api::GLenum type, const api::GLvoid* row, const api::GLvoid* column)
{
	log() << warning << "SeparableFilter2D not available." << std::endl;
}

static void fallback_glSeparableFilter2DEXT(api::GLenum target, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLenum format, api::GLenum type, const api::GLvoid* row, const api::GLvoid* column)
{
	log() << warning << "SeparableFilter2DEXT not available." << std::endl;
}

static void fallback_glSetFenceAPPLE(api::GLuint fence)
{
	log() << warning << "SetFenceAPPLE not available." << std::endl;
}

static void fallback_glSetFenceNV(api::GLuint fence, api::GLenum condition)
{
	log() << warning << "SetFenceNV not available." << std::endl;
}

static void fallback_glSetFragmentShaderConstantATI(api::GLuint dst, const api::GLfloat* value)
{
	log() << warning << "SetFragmentShaderConstantATI not available." << std::endl;
}

static void fallback_glSetInvariantEXT(api::GLuint id, api::GLenum type, const api::GLvoid* addr)
{
	log() << warning << "SetInvariantEXT not available." << std::endl;
}

static void fallback_glSetLocalConstantEXT(api::GLuint id, api::GLenum type, const api::GLvoid* addr)
{
	log() << warning << "SetLocalConstantEXT not available." << std::endl;
}

static void fallback_glShadeModel(api::GLenum mode)
{
	log() << warning << "ShadeModel not available." << std::endl;
}

static void fallback_glShaderOp1EXT(api::GLenum op, api::GLuint res, api::GLuint arg1)
{
	log() << warning << "ShaderOp1EXT not available." << std::endl;
}

static void fallback_glShaderOp2EXT(api::GLenum op, api::GLuint res, api::GLuint arg1, api::GLuint arg2)
{
	log() << warning << "ShaderOp2EXT not available." << std::endl;
}

static void fallback_glShaderOp3EXT(api::GLenum op, api::GLuint res, api::GLuint arg1, api::GLuint arg2, api::GLuint arg3)
{
	log() << warning << "ShaderOp3EXT not available." << std::endl;
}

static void fallback_glShaderSource(api::GLuint shader, api::GLsizei count, const api::GLchar** string, const api::GLint* length)
{
	log() << warning << "ShaderSource not available." << std::endl;
}

static void fallback_glShaderSourceARB(api::GLhandleARB shaderObj, api::GLsizei count, const api::GLcharARB** string, const api::GLint* length)
{
	log() << warning << "ShaderSourceARB not available." << std::endl;
}

static void fallback_glSharpenTexFuncSGIS(api::GLenum target, api::GLsizei n, const api::GLfloat* points)
{
	log() << warning << "SharpenTexFuncSGIS not available." << std::endl;
}

static void fallback_glSpriteParameterfSGIX(api::GLenum pname, api::GLfloat param)
{
	log() << warning << "SpriteParameterfSGIX not available." << std::endl;
}

static void fallback_glSpriteParameterfvSGIX(api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "SpriteParameterfvSGIX not available." << std::endl;
}

static void fallback_glSpriteParameteriSGIX(api::GLenum pname, api::GLint param)
{
	log() << warning << "SpriteParameteriSGIX not available." << std::endl;
}

static void fallback_glSpriteParameterivSGIX(api::GLenum pname, const api::GLint* params)
{
	log() << warning << "SpriteParameterivSGIX not available." << std::endl;
}

static void fallback_glStartInstrumentsSGIX()
{
	log() << warning << "StartInstrumentsSGIX not available." << std::endl;
}

static void fallback_glStencilClearTagEXT(api::GLsizei stencilTagBits, api::GLuint stencilClearTag)
{
	log() << warning << "StencilClearTagEXT not available." << std::endl;
}

static void fallback_glStencilFunc(api::GLenum func, api::GLint ref, api::GLuint mask)
{
	log() << warning << "StencilFunc not available." << std::endl;
}

static void fallback_glStencilFuncSeparate(api::GLenum frontfunc, api::GLenum backfunc, api::GLint ref, api::GLuint mask)
{
	log() << warning << "StencilFuncSeparate not available." << std::endl;
}

static void fallback_glStencilFuncSeparateATI(api::GLenum frontfunc, api::GLenum backfunc, api::GLint ref, api::GLuint mask)
{
	log() << warning << "StencilFuncSeparateATI not available." << std::endl;
}

static void fallback_glStencilMask(api::GLuint mask)
{
	log() << warning << "StencilMask not available." << std::endl;
}

static void fallback_glStencilMaskSeparate(api::GLenum face, api::GLuint mask)
{
	log() << warning << "StencilMaskSeparate not available." << std::endl;
}

static void fallback_glStencilOp(api::GLenum fail, api::GLenum zfail, api::GLenum zpass)
{
	log() << warning << "StencilOp not available." << std::endl;
}

static void fallback_glStencilOpSeparate(api::GLenum face, api::GLenum sfail, api::GLenum dpfail, api::GLenum dppass)
{
	log() << warning << "StencilOpSeparate not available." << std::endl;
}

static void fallback_glStencilOpSeparateATI(api::GLenum face, api::GLenum sfail, api::GLenum dpfail, api::GLenum dppass)
{
	log() << warning << "StencilOpSeparateATI not available." << std::endl;
}

static void fallback_glStopInstrumentsSGIX(api::GLint marker)
{
	log() << warning << "StopInstrumentsSGIX not available." << std::endl;
}

static void fallback_glStringMarkerGREMEDY(api::GLsizei len, const api::GLvoid* string)
{
	log() << warning << "StringMarkerGREMEDY not available." << std::endl;
}

static void fallback_glSwizzleEXT(api::GLuint res, api::GLuint in, api::GLenum outX, api::GLenum outY, api::GLenum outZ, api::GLenum outW)
{
	log() << warning << "SwizzleEXT not available." << std::endl;
}

static void fallback_glTagSampleBufferSGIX()
{
	log() << warning << "TagSampleBufferSGIX not available." << std::endl;
}

static void fallback_glTangent3bEXT(api::GLbyte tx, api::GLbyte ty, api::GLbyte tz)
{
	log() << warning << "Tangent3bEXT not available." << std::endl;
}

static void fallback_glTangent3bvEXT(const api::GLbyte* v)
{
	log() << warning << "Tangent3bvEXT not available." << std::endl;
}

static void fallback_glTangent3dEXT(api::GLdouble tx, api::GLdouble ty, api::GLdouble tz)
{
	log() << warning << "Tangent3dEXT not available." << std::endl;
}

static void fallback_glTangent3dvEXT(const api::GLdouble* v)
{
	log() << warning << "Tangent3dvEXT not available." << std::endl;
}

static void fallback_glTangent3fEXT(api::GLfloat tx, api::GLfloat ty, api::GLfloat tz)
{
	log() << warning << "Tangent3fEXT not available." << std::endl;
}

static void fallback_glTangent3fvEXT(const api::GLfloat* v)
{
	log() << warning << "Tangent3fvEXT not available." << std::endl;
}

static void fallback_glTangent3iEXT(api::GLint tx, api::GLint ty, api::GLint tz)
{
	log() << warning << "Tangent3iEXT not available." << std::endl;
}

static void fallback_glTangent3ivEXT(const api::GLint* v)
{
	log() << warning << "Tangent3ivEXT not available." << std::endl;
}

static void fallback_glTangent3sEXT(api::GLshort tx, api::GLshort ty, api::GLshort tz)
{
	log() << warning << "Tangent3sEXT not available." << std::endl;
}

static void fallback_glTangent3svEXT(const api::GLshort* v)
{
	log() << warning << "Tangent3svEXT not available." << std::endl;
}

static void fallback_glTangentPointerEXT(api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "TangentPointerEXT not available." << std::endl;
}

static void fallback_glTbufferMask3DFX(api::GLuint mask)
{
	log() << warning << "TbufferMask3DFX not available." << std::endl;
}

static void fallback_glTessellationFactorAMD(api::GLfloat factor)
{
	log() << warning << "TessellationFactorAMD not available." << std::endl;
}

static void fallback_glTessellationModeAMD(api::GLenum mode)
{
	log() << warning << "TessellationModeAMD not available." << std::endl;
}

static api::GLboolean fallback_glTestFenceAPPLE(api::GLuint fence)
{
	log() << warning << "TestFenceAPPLE not available." << std::endl;
}

static api::GLboolean fallback_glTestFenceNV(api::GLuint fence)
{
	log() << warning << "TestFenceNV not available." << std::endl;
}

static api::GLboolean fallback_glTestObjectAPPLE(api::GLenum object, api::GLuint name)
{
	log() << warning << "TestObjectAPPLE not available." << std::endl;
}

static void fallback_glTexBuffer(api::GLenum target, api::GLenum internalformat, api::GLuint buffer)
{
	log() << warning << "TexBuffer not available." << std::endl;
}

static void fallback_glTexBufferARB(api::GLenum target, api::GLenum internalformat, api::GLuint buffer)
{
	log() << warning << "TexBufferARB not available." << std::endl;
}

static void fallback_glTexBufferEXT(api::GLenum target, api::GLenum internalformat, api::GLuint buffer)
{
	log() << warning << "TexBufferEXT not available." << std::endl;
}

static void fallback_glTexBumpParameterfvATI(api::GLenum pname, const api::GLfloat* param)
{
	log() << warning << "TexBumpParameterfvATI not available." << std::endl;
}

static void fallback_glTexBumpParameterivATI(api::GLenum pname, const api::GLint* param)
{
	log() << warning << "TexBumpParameterivATI not available." << std::endl;
}

static void fallback_glTexCoord1d(api::GLdouble s)
{
	log() << warning << "TexCoord1d not available." << std::endl;
}

static void fallback_glTexCoord1dv(const api::GLdouble* v)
{
	log() << warning << "TexCoord1dv not available." << std::endl;
}

static void fallback_glTexCoord1f(api::GLfloat s)
{
	log() << warning << "TexCoord1f not available." << std::endl;
}

static void fallback_glTexCoord1fv(const api::GLfloat* v)
{
	log() << warning << "TexCoord1fv not available." << std::endl;
}

static void fallback_glTexCoord1hNV(api::GLhalfNV s)
{
	log() << warning << "TexCoord1hNV not available." << std::endl;
}

static void fallback_glTexCoord1hvNV(const api::GLhalfNV* v)
{
	log() << warning << "TexCoord1hvNV not available." << std::endl;
}

static void fallback_glTexCoord1i(api::GLint s)
{
	log() << warning << "TexCoord1i not available." << std::endl;
}

static void fallback_glTexCoord1iv(const api::GLint* v)
{
	log() << warning << "TexCoord1iv not available." << std::endl;
}

static void fallback_glTexCoord1s(api::GLshort s)
{
	log() << warning << "TexCoord1s not available." << std::endl;
}

static void fallback_glTexCoord1sv(const api::GLshort* v)
{
	log() << warning << "TexCoord1sv not available." << std::endl;
}

static void fallback_glTexCoord2d(api::GLdouble s, api::GLdouble t)
{
	log() << warning << "TexCoord2d not available." << std::endl;
}

static void fallback_glTexCoord2dv(const api::GLdouble* v)
{
	log() << warning << "TexCoord2dv not available." << std::endl;
}

static void fallback_glTexCoord2f(api::GLfloat s, api::GLfloat t)
{
	log() << warning << "TexCoord2f not available." << std::endl;
}

static void fallback_glTexCoord2fColor3fVertex3fSUN(api::GLfloat s, api::GLfloat t, api::GLfloat r, api::GLfloat g, api::GLfloat b, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "TexCoord2fColor3fVertex3fSUN not available." << std::endl;
}

static void fallback_glTexCoord2fColor3fVertex3fvSUN(const api::GLfloat* tc, const api::GLfloat* c, const api::GLfloat* v)
{
	log() << warning << "TexCoord2fColor3fVertex3fvSUN not available." << std::endl;
}

static void fallback_glTexCoord2fColor4fNormal3fVertex3fSUN(api::GLfloat s, api::GLfloat t, api::GLfloat r, api::GLfloat g, api::GLfloat b, api::GLfloat a, api::GLfloat nx, api::GLfloat ny, api::GLfloat nz, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "TexCoord2fColor4fNormal3fVertex3fSUN not available." << std::endl;
}

static void fallback_glTexCoord2fColor4fNormal3fVertex3fvSUN(const api::GLfloat* tc, const api::GLfloat* c, const api::GLfloat* n, const api::GLfloat* v)
{
	log() << warning << "TexCoord2fColor4fNormal3fVertex3fvSUN not available." << std::endl;
}

static void fallback_glTexCoord2fColor4ubVertex3fSUN(api::GLfloat s, api::GLfloat t, api::GLubyte r, api::GLubyte g, api::GLubyte b, api::GLubyte a, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "TexCoord2fColor4ubVertex3fSUN not available." << std::endl;
}

static void fallback_glTexCoord2fColor4ubVertex3fvSUN(const api::GLfloat* tc, const api::GLubyte* c, const api::GLfloat* v)
{
	log() << warning << "TexCoord2fColor4ubVertex3fvSUN not available." << std::endl;
}

static void fallback_glTexCoord2fNormal3fVertex3fSUN(api::GLfloat s, api::GLfloat t, api::GLfloat nx, api::GLfloat ny, api::GLfloat nz, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "TexCoord2fNormal3fVertex3fSUN not available." << std::endl;
}

static void fallback_glTexCoord2fNormal3fVertex3fvSUN(const api::GLfloat* tc, const api::GLfloat* n, const api::GLfloat* v)
{
	log() << warning << "TexCoord2fNormal3fVertex3fvSUN not available." << std::endl;
}

static void fallback_glTexCoord2fVertex3fSUN(api::GLfloat s, api::GLfloat t, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "TexCoord2fVertex3fSUN not available." << std::endl;
}

static void fallback_glTexCoord2fVertex3fvSUN(const api::GLfloat* tc, const api::GLfloat* v)
{
	log() << warning << "TexCoord2fVertex3fvSUN not available." << std::endl;
}

static void fallback_glTexCoord2fv(const api::GLfloat* v)
{
	log() << warning << "TexCoord2fv not available." << std::endl;
}

static void fallback_glTexCoord2hNV(api::GLhalfNV s, api::GLhalfNV t)
{
	log() << warning << "TexCoord2hNV not available." << std::endl;
}

static void fallback_glTexCoord2hvNV(const api::GLhalfNV* v)
{
	log() << warning << "TexCoord2hvNV not available." << std::endl;
}

static void fallback_glTexCoord2i(api::GLint s, api::GLint t)
{
	log() << warning << "TexCoord2i not available." << std::endl;
}

static void fallback_glTexCoord2iv(const api::GLint* v)
{
	log() << warning << "TexCoord2iv not available." << std::endl;
}

static void fallback_glTexCoord2s(api::GLshort s, api::GLshort t)
{
	log() << warning << "TexCoord2s not available." << std::endl;
}

static void fallback_glTexCoord2sv(const api::GLshort* v)
{
	log() << warning << "TexCoord2sv not available." << std::endl;
}

static void fallback_glTexCoord3d(api::GLdouble s, api::GLdouble t, api::GLdouble r)
{
	log() << warning << "TexCoord3d not available." << std::endl;
}

static void fallback_glTexCoord3dv(const api::GLdouble* v)
{
	log() << warning << "TexCoord3dv not available." << std::endl;
}

static void fallback_glTexCoord3f(api::GLfloat s, api::GLfloat t, api::GLfloat r)
{
	log() << warning << "TexCoord3f not available." << std::endl;
}

static void fallback_glTexCoord3fv(const api::GLfloat* v)
{
	log() << warning << "TexCoord3fv not available." << std::endl;
}

static void fallback_glTexCoord3hNV(api::GLhalfNV s, api::GLhalfNV t, api::GLhalfNV r)
{
	log() << warning << "TexCoord3hNV not available." << std::endl;
}

static void fallback_glTexCoord3hvNV(const api::GLhalfNV* v)
{
	log() << warning << "TexCoord3hvNV not available." << std::endl;
}

static void fallback_glTexCoord3i(api::GLint s, api::GLint t, api::GLint r)
{
	log() << warning << "TexCoord3i not available." << std::endl;
}

static void fallback_glTexCoord3iv(const api::GLint* v)
{
	log() << warning << "TexCoord3iv not available." << std::endl;
}

static void fallback_glTexCoord3s(api::GLshort s, api::GLshort t, api::GLshort r)
{
	log() << warning << "TexCoord3s not available." << std::endl;
}

static void fallback_glTexCoord3sv(const api::GLshort* v)
{
	log() << warning << "TexCoord3sv not available." << std::endl;
}

static void fallback_glTexCoord4d(api::GLdouble s, api::GLdouble t, api::GLdouble r, api::GLdouble q)
{
	log() << warning << "TexCoord4d not available." << std::endl;
}

static void fallback_glTexCoord4dv(const api::GLdouble* v)
{
	log() << warning << "TexCoord4dv not available." << std::endl;
}

static void fallback_glTexCoord4f(api::GLfloat s, api::GLfloat t, api::GLfloat r, api::GLfloat q)
{
	log() << warning << "TexCoord4f not available." << std::endl;
}

static void fallback_glTexCoord4fColor4fNormal3fVertex4fSUN(api::GLfloat s, api::GLfloat t, api::GLfloat p, api::GLfloat q, api::GLfloat r, api::GLfloat g, api::GLfloat b, api::GLfloat a, api::GLfloat nx, api::GLfloat ny, api::GLfloat nz, api::GLfloat x, api::GLfloat y, api::GLfloat z, api::GLfloat w)
{
	log() << warning << "TexCoord4fColor4fNormal3fVertex4fSUN not available." << std::endl;
}

static void fallback_glTexCoord4fColor4fNormal3fVertex4fvSUN(const api::GLfloat* tc, const api::GLfloat* c, const api::GLfloat* n, const api::GLfloat* v)
{
	log() << warning << "TexCoord4fColor4fNormal3fVertex4fvSUN not available." << std::endl;
}

static void fallback_glTexCoord4fVertex4fSUN(api::GLfloat s, api::GLfloat t, api::GLfloat p, api::GLfloat q, api::GLfloat x, api::GLfloat y, api::GLfloat z, api::GLfloat w)
{
	log() << warning << "TexCoord4fVertex4fSUN not available." << std::endl;
}

static void fallback_glTexCoord4fVertex4fvSUN(const api::GLfloat* tc, const api::GLfloat* v)
{
	log() << warning << "TexCoord4fVertex4fvSUN not available." << std::endl;
}

static void fallback_glTexCoord4fv(const api::GLfloat* v)
{
	log() << warning << "TexCoord4fv not available." << std::endl;
}

static void fallback_glTexCoord4hNV(api::GLhalfNV s, api::GLhalfNV t, api::GLhalfNV r, api::GLhalfNV q)
{
	log() << warning << "TexCoord4hNV not available." << std::endl;
}

static void fallback_glTexCoord4hvNV(const api::GLhalfNV* v)
{
	log() << warning << "TexCoord4hvNV not available." << std::endl;
}

static void fallback_glTexCoord4i(api::GLint s, api::GLint t, api::GLint r, api::GLint q)
{
	log() << warning << "TexCoord4i not available." << std::endl;
}

static void fallback_glTexCoord4iv(const api::GLint* v)
{
	log() << warning << "TexCoord4iv not available." << std::endl;
}

static void fallback_glTexCoord4s(api::GLshort s, api::GLshort t, api::GLshort r, api::GLshort q)
{
	log() << warning << "TexCoord4s not available." << std::endl;
}

static void fallback_glTexCoord4sv(const api::GLshort* v)
{
	log() << warning << "TexCoord4sv not available." << std::endl;
}

static void fallback_glTexCoordFormatNV(api::GLint size, api::GLenum type, api::GLsizei stride)
{
	log() << warning << "TexCoordFormatNV not available." << std::endl;
}

static void fallback_glTexCoordP1ui(api::GLenum type, api::GLuint coords)
{
	log() << warning << "TexCoordP1ui not available." << std::endl;
}

static void fallback_glTexCoordP1uiv(api::GLenum type, const api::GLuint* coords)
{
	log() << warning << "TexCoordP1uiv not available." << std::endl;
}

static void fallback_glTexCoordP2ui(api::GLenum type, api::GLuint coords)
{
	log() << warning << "TexCoordP2ui not available." << std::endl;
}

static void fallback_glTexCoordP2uiv(api::GLenum type, const api::GLuint* coords)
{
	log() << warning << "TexCoordP2uiv not available." << std::endl;
}

static void fallback_glTexCoordP3ui(api::GLenum type, api::GLuint coords)
{
	log() << warning << "TexCoordP3ui not available." << std::endl;
}

static void fallback_glTexCoordP3uiv(api::GLenum type, const api::GLuint* coords)
{
	log() << warning << "TexCoordP3uiv not available." << std::endl;
}

static void fallback_glTexCoordP4ui(api::GLenum type, api::GLuint coords)
{
	log() << warning << "TexCoordP4ui not available." << std::endl;
}

static void fallback_glTexCoordP4uiv(api::GLenum type, const api::GLuint* coords)
{
	log() << warning << "TexCoordP4uiv not available." << std::endl;
}

static void fallback_glTexCoordPointer(api::GLint size, api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "TexCoordPointer not available." << std::endl;
}

static void fallback_glTexCoordPointerEXT(api::GLint size, api::GLenum type, api::GLsizei stride, api::GLsizei count, const api::GLvoid* pointer)
{
	log() << warning << "TexCoordPointerEXT not available." << std::endl;
}

static void fallback_glTexCoordPointerListIBM(api::GLint size, api::GLenum type, api::GLint stride, const api::GLvoid** pointer, api::GLint ptrstride)
{
	log() << warning << "TexCoordPointerListIBM not available." << std::endl;
}

static void fallback_glTexCoordPointervINTEL(api::GLint size, api::GLenum type, const api::GLvoid** pointer)
{
	log() << warning << "TexCoordPointervINTEL not available." << std::endl;
}

static void fallback_glTexEnvf(api::GLenum target, api::GLenum pname, api::GLfloat param)
{
	log() << warning << "TexEnvf not available." << std::endl;
}

static void fallback_glTexEnvfv(api::GLenum target, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "TexEnvfv not available." << std::endl;
}

static void fallback_glTexEnvi(api::GLenum target, api::GLenum pname, api::GLint param)
{
	log() << warning << "TexEnvi not available." << std::endl;
}

static void fallback_glTexEnviv(api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "TexEnviv not available." << std::endl;
}

static void fallback_glTexFilterFuncSGIS(api::GLenum target, api::GLenum filter, api::GLsizei n, const api::GLfloat* weights)
{
	log() << warning << "TexFilterFuncSGIS not available." << std::endl;
}

static void fallback_glTexGend(api::GLenum coord, api::GLenum pname, api::GLdouble param)
{
	log() << warning << "TexGend not available." << std::endl;
}

static void fallback_glTexGendv(api::GLenum coord, api::GLenum pname, const api::GLdouble* params)
{
	log() << warning << "TexGendv not available." << std::endl;
}

static void fallback_glTexGenf(api::GLenum coord, api::GLenum pname, api::GLfloat param)
{
	log() << warning << "TexGenf not available." << std::endl;
}

static void fallback_glTexGenfv(api::GLenum coord, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "TexGenfv not available." << std::endl;
}

static void fallback_glTexGeni(api::GLenum coord, api::GLenum pname, api::GLint param)
{
	log() << warning << "TexGeni not available." << std::endl;
}

static void fallback_glTexGeniv(api::GLenum coord, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "TexGeniv not available." << std::endl;
}

static void fallback_glTexImage1D(api::GLenum target, api::GLint level, api::GLint internalformat, api::GLsizei width, api::GLint border, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "TexImage1D not available." << std::endl;
}

static void fallback_glTexImage2D(api::GLenum target, api::GLint level, api::GLint internalformat, api::GLsizei width, api::GLsizei height, api::GLint border, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "TexImage2D not available." << std::endl;
}

static void fallback_glTexImage2DMultisample(api::GLenum target, api::GLsizei samples, api::GLint internalformat, api::GLsizei width, api::GLsizei height, api::GLboolean fixedsamplelocations)
{
	log() << warning << "TexImage2DMultisample not available." << std::endl;
}

static void fallback_glTexImage3D(api::GLenum target, api::GLint level, api::GLint internalformat, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLint border, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "TexImage3D not available." << std::endl;
}

static void fallback_glTexImage3DEXT(api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLint border, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "TexImage3DEXT not available." << std::endl;
}

static void fallback_glTexImage3DMultisample(api::GLenum target, api::GLsizei samples, api::GLint internalformat, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLboolean fixedsamplelocations)
{
	log() << warning << "TexImage3DMultisample not available." << std::endl;
}

static void fallback_glTexImage4DSGIS(api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLsizei size4d, api::GLint border, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "TexImage4DSGIS not available." << std::endl;
}

static void fallback_glTexParameterIiv(api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "TexParameterIiv not available." << std::endl;
}

static void fallback_glTexParameterIivEXT(api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "TexParameterIivEXT not available." << std::endl;
}

static void fallback_glTexParameterIuiv(api::GLenum target, api::GLenum pname, const api::GLuint* params)
{
	log() << warning << "TexParameterIuiv not available." << std::endl;
}

static void fallback_glTexParameterIuivEXT(api::GLenum target, api::GLenum pname, const api::GLuint* params)
{
	log() << warning << "TexParameterIuivEXT not available." << std::endl;
}

static void fallback_glTexParameterf(api::GLenum target, api::GLenum pname, api::GLfloat param)
{
	log() << warning << "TexParameterf not available." << std::endl;
}

static void fallback_glTexParameterfv(api::GLenum target, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "TexParameterfv not available." << std::endl;
}

static void fallback_glTexParameteri(api::GLenum target, api::GLenum pname, api::GLint param)
{
	log() << warning << "TexParameteri not available." << std::endl;
}

static void fallback_glTexParameteriv(api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "TexParameteriv not available." << std::endl;
}

static void fallback_glTexRenderbufferNV(api::GLenum target, api::GLuint renderbuffer)
{
	log() << warning << "TexRenderbufferNV not available." << std::endl;
}

static void fallback_glTexSubImage1D(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLsizei width, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "TexSubImage1D not available." << std::endl;
}

static void fallback_glTexSubImage1DEXT(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLsizei width, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "TexSubImage1DEXT not available." << std::endl;
}

static void fallback_glTexSubImage2D(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLsizei width, api::GLsizei height, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "TexSubImage2D not available." << std::endl;
}

static void fallback_glTexSubImage2DEXT(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLsizei width, api::GLsizei height, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "TexSubImage2DEXT not available." << std::endl;
}

static void fallback_glTexSubImage3D(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint zoffset, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "TexSubImage3D not available." << std::endl;
}

static void fallback_glTexSubImage3DEXT(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint zoffset, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "TexSubImage3DEXT not available." << std::endl;
}

static void fallback_glTexSubImage4DSGIS(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint zoffset, api::GLint woffset, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLsizei size4d, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "TexSubImage4DSGIS not available." << std::endl;
}

static void fallback_glTextureBarrierNV()
{
	log() << warning << "TextureBarrierNV not available." << std::endl;
}

static void fallback_glTextureBufferEXT(api::GLuint texture, api::GLenum target, api::GLenum internalformat, api::GLuint buffer)
{
	log() << warning << "TextureBufferEXT not available." << std::endl;
}

static void fallback_glTextureColorMaskSGIS(api::GLboolean red, api::GLboolean green, api::GLboolean blue, api::GLboolean alpha)
{
	log() << warning << "TextureColorMaskSGIS not available." << std::endl;
}

static void fallback_glTextureImage1DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLint border, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "TextureImage1DEXT not available." << std::endl;
}

static void fallback_glTextureImage2DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLint border, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "TextureImage2DEXT not available." << std::endl;
}

static void fallback_glTextureImage3DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLint border, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "TextureImage3DEXT not available." << std::endl;
}

static void fallback_glTextureLightEXT(api::GLenum pname)
{
	log() << warning << "TextureLightEXT not available." << std::endl;
}

static void fallback_glTextureMaterialEXT(api::GLenum face, api::GLenum mode)
{
	log() << warning << "TextureMaterialEXT not available." << std::endl;
}

static void fallback_glTextureNormalEXT(api::GLenum mode)
{
	log() << warning << "TextureNormalEXT not available." << std::endl;
}

static void fallback_glTextureParameterIivEXT(api::GLuint texture, api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "TextureParameterIivEXT not available." << std::endl;
}

static void fallback_glTextureParameterIuivEXT(api::GLuint texture, api::GLenum target, api::GLenum pname, const api::GLuint* params)
{
	log() << warning << "TextureParameterIuivEXT not available." << std::endl;
}

static void fallback_glTextureParameterfEXT(api::GLuint texture, api::GLenum target, api::GLenum pname, api::GLfloat param)
{
	log() << warning << "TextureParameterfEXT not available." << std::endl;
}

static void fallback_glTextureParameterfvEXT(api::GLuint texture, api::GLenum target, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "TextureParameterfvEXT not available." << std::endl;
}

static void fallback_glTextureParameteriEXT(api::GLuint texture, api::GLenum target, api::GLenum pname, api::GLint param)
{
	log() << warning << "TextureParameteriEXT not available." << std::endl;
}

static void fallback_glTextureParameterivEXT(api::GLuint texture, api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "TextureParameterivEXT not available." << std::endl;
}

static void fallback_glTextureRangeAPPLE(api::GLenum target, api::GLsizei length, const api::GLvoid* pointer)
{
	log() << warning << "TextureRangeAPPLE not available." << std::endl;
}

static void fallback_glTextureRenderbufferEXT(api::GLuint texture, api::GLenum target, api::GLuint renderbuffer)
{
	log() << warning << "TextureRenderbufferEXT not available." << std::endl;
}

static void fallback_glTextureSubImage1DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLsizei width, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "TextureSubImage1DEXT not available." << std::endl;
}

static void fallback_glTextureSubImage2DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLsizei width, api::GLsizei height, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "TextureSubImage2DEXT not available." << std::endl;
}

static void fallback_glTextureSubImage3DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint zoffset, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "TextureSubImage3DEXT not available." << std::endl;
}

static void fallback_glTrackMatrixNV(api::GLenum target, api::GLuint address, api::GLenum matrix, api::GLenum transform)
{
	log() << warning << "TrackMatrixNV not available." << std::endl;
}

static void fallback_glTransformFeedbackAttribsNV(api::GLuint count, const api::GLint* attribs, api::GLenum bufferMode)
{
	log() << warning << "TransformFeedbackAttribsNV not available." << std::endl;
}

static void fallback_glTransformFeedbackVaryings(api::GLuint program, api::GLsizei count, const api::GLchar** varyings, api::GLenum bufferMode)
{
	log() << warning << "TransformFeedbackVaryings not available." << std::endl;
}

static void fallback_glTransformFeedbackVaryingsEXT(api::GLuint program, api::GLsizei count, const api::GLchar** varyings, api::GLenum bufferMode)
{
	log() << warning << "TransformFeedbackVaryingsEXT not available." << std::endl;
}

static void fallback_glTransformFeedbackVaryingsNV(api::GLuint program, api::GLsizei count, const api::GLchar** varyings, api::GLenum bufferMode)
{
	log() << warning << "TransformFeedbackVaryingsNV not available." << std::endl;
}

static void fallback_glTranslated(api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "Translated not available." << std::endl;
}

static void fallback_glTranslatef(api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "Translatef not available." << std::endl;
}

static void fallback_glUniform1d(api::GLint location, api::GLdouble x)
{
	log() << warning << "Uniform1d not available." << std::endl;
}

static void fallback_glUniform1dv(api::GLint location, api::GLsizei count, const api::GLdouble* value)
{
	log() << warning << "Uniform1dv not available." << std::endl;
}

static void fallback_glUniform1f(api::GLint location, api::GLfloat v0)
{
	log() << warning << "Uniform1f not available." << std::endl;
}

static void fallback_glUniform1fARB(api::GLint location, api::GLfloat v0)
{
	log() << warning << "Uniform1fARB not available." << std::endl;
}

static void fallback_glUniform1fv(api::GLint location, api::GLsizei count, const api::GLfloat* value)
{
	log() << warning << "Uniform1fv not available." << std::endl;
}

static void fallback_glUniform1fvARB(api::GLint location, api::GLsizei count, const api::GLfloat* value)
{
	log() << warning << "Uniform1fvARB not available." << std::endl;
}

static void fallback_glUniform1i(api::GLint location, api::GLint v0)
{
	log() << warning << "Uniform1i not available." << std::endl;
}

static void fallback_glUniform1iARB(api::GLint location, api::GLint v0)
{
	log() << warning << "Uniform1iARB not available." << std::endl;
}

static void fallback_glUniform1iv(api::GLint location, api::GLsizei count, const api::GLint* value)
{
	log() << warning << "Uniform1iv not available." << std::endl;
}

static void fallback_glUniform1ivARB(api::GLint location, api::GLsizei count, const api::GLint* value)
{
	log() << warning << "Uniform1ivARB not available." << std::endl;
}

static void fallback_glUniform1ui(api::GLint location, api::GLuint v0)
{
	log() << warning << "Uniform1ui not available." << std::endl;
}

static void fallback_glUniform1uiEXT(api::GLint location, api::GLuint v0)
{
	log() << warning << "Uniform1uiEXT not available." << std::endl;
}

static void fallback_glUniform1uiv(api::GLint location, api::GLsizei count, const api::GLuint* value)
{
	log() << warning << "Uniform1uiv not available." << std::endl;
}

static void fallback_glUniform1uivEXT(api::GLint location, api::GLsizei count, const api::GLuint* value)
{
	log() << warning << "Uniform1uivEXT not available." << std::endl;
}

static void fallback_glUniform2d(api::GLint location, api::GLdouble x, api::GLdouble y)
{
	log() << warning << "Uniform2d not available." << std::endl;
}

static void fallback_glUniform2dv(api::GLint location, api::GLsizei count, const api::GLdouble* value)
{
	log() << warning << "Uniform2dv not available." << std::endl;
}

static void fallback_glUniform2f(api::GLint location, api::GLfloat v0, api::GLfloat v1)
{
	log() << warning << "Uniform2f not available." << std::endl;
}

static void fallback_glUniform2fARB(api::GLint location, api::GLfloat v0, api::GLfloat v1)
{
	log() << warning << "Uniform2fARB not available." << std::endl;
}

static void fallback_glUniform2fv(api::GLint location, api::GLsizei count, const api::GLfloat* value)
{
	log() << warning << "Uniform2fv not available." << std::endl;
}

static void fallback_glUniform2fvARB(api::GLint location, api::GLsizei count, const api::GLfloat* value)
{
	log() << warning << "Uniform2fvARB not available." << std::endl;
}

static void fallback_glUniform2i(api::GLint location, api::GLint v0, api::GLint v1)
{
	log() << warning << "Uniform2i not available." << std::endl;
}

static void fallback_glUniform2iARB(api::GLint location, api::GLint v0, api::GLint v1)
{
	log() << warning << "Uniform2iARB not available." << std::endl;
}

static void fallback_glUniform2iv(api::GLint location, api::GLsizei count, const api::GLint* value)
{
	log() << warning << "Uniform2iv not available." << std::endl;
}

static void fallback_glUniform2ivARB(api::GLint location, api::GLsizei count, const api::GLint* value)
{
	log() << warning << "Uniform2ivARB not available." << std::endl;
}

static void fallback_glUniform2ui(api::GLint location, api::GLuint v0, api::GLuint v1)
{
	log() << warning << "Uniform2ui not available." << std::endl;
}

static void fallback_glUniform2uiEXT(api::GLint location, api::GLuint v0, api::GLuint v1)
{
	log() << warning << "Uniform2uiEXT not available." << std::endl;
}

static void fallback_glUniform2uiv(api::GLint location, api::GLsizei count, const api::GLuint* value)
{
	log() << warning << "Uniform2uiv not available." << std::endl;
}

static void fallback_glUniform2uivEXT(api::GLint location, api::GLsizei count, const api::GLuint* value)
{
	log() << warning << "Uniform2uivEXT not available." << std::endl;
}

static void fallback_glUniform3d(api::GLint location, api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "Uniform3d not available." << std::endl;
}

static void fallback_glUniform3dv(api::GLint location, api::GLsizei count, const api::GLdouble* value)
{
	log() << warning << "Uniform3dv not available." << std::endl;
}

static void fallback_glUniform3f(api::GLint location, api::GLfloat v0, api::GLfloat v1, api::GLfloat v2)
{
	log() << warning << "Uniform3f not available." << std::endl;
}

static void fallback_glUniform3fARB(api::GLint location, api::GLfloat v0, api::GLfloat v1, api::GLfloat v2)
{
	log() << warning << "Uniform3fARB not available." << std::endl;
}

static void fallback_glUniform3fv(api::GLint location, api::GLsizei count, const api::GLfloat* value)
{
	log() << warning << "Uniform3fv not available." << std::endl;
}

static void fallback_glUniform3fvARB(api::GLint location, api::GLsizei count, const api::GLfloat* value)
{
	log() << warning << "Uniform3fvARB not available." << std::endl;
}

static void fallback_glUniform3i(api::GLint location, api::GLint v0, api::GLint v1, api::GLint v2)
{
	log() << warning << "Uniform3i not available." << std::endl;
}

static void fallback_glUniform3iARB(api::GLint location, api::GLint v0, api::GLint v1, api::GLint v2)
{
	log() << warning << "Uniform3iARB not available." << std::endl;
}

static void fallback_glUniform3iv(api::GLint location, api::GLsizei count, const api::GLint* value)
{
	log() << warning << "Uniform3iv not available." << std::endl;
}

static void fallback_glUniform3ivARB(api::GLint location, api::GLsizei count, const api::GLint* value)
{
	log() << warning << "Uniform3ivARB not available." << std::endl;
}

static void fallback_glUniform3ui(api::GLint location, api::GLuint v0, api::GLuint v1, api::GLuint v2)
{
	log() << warning << "Uniform3ui not available." << std::endl;
}

static void fallback_glUniform3uiEXT(api::GLint location, api::GLuint v0, api::GLuint v1, api::GLuint v2)
{
	log() << warning << "Uniform3uiEXT not available." << std::endl;
}

static void fallback_glUniform3uiv(api::GLint location, api::GLsizei count, const api::GLuint* value)
{
	log() << warning << "Uniform3uiv not available." << std::endl;
}

static void fallback_glUniform3uivEXT(api::GLint location, api::GLsizei count, const api::GLuint* value)
{
	log() << warning << "Uniform3uivEXT not available." << std::endl;
}

static void fallback_glUniform4d(api::GLint location, api::GLdouble x, api::GLdouble y, api::GLdouble z, api::GLdouble w)
{
	log() << warning << "Uniform4d not available." << std::endl;
}

static void fallback_glUniform4dv(api::GLint location, api::GLsizei count, const api::GLdouble* value)
{
	log() << warning << "Uniform4dv not available." << std::endl;
}

static void fallback_glUniform4f(api::GLint location, api::GLfloat v0, api::GLfloat v1, api::GLfloat v2, api::GLfloat v3)
{
	log() << warning << "Uniform4f not available." << std::endl;
}

static void fallback_glUniform4fARB(api::GLint location, api::GLfloat v0, api::GLfloat v1, api::GLfloat v2, api::GLfloat v3)
{
	log() << warning << "Uniform4fARB not available." << std::endl;
}

static void fallback_glUniform4fv(api::GLint location, api::GLsizei count, const api::GLfloat* value)
{
	log() << warning << "Uniform4fv not available." << std::endl;
}

static void fallback_glUniform4fvARB(api::GLint location, api::GLsizei count, const api::GLfloat* value)
{
	log() << warning << "Uniform4fvARB not available." << std::endl;
}

static void fallback_glUniform4i(api::GLint location, api::GLint v0, api::GLint v1, api::GLint v2, api::GLint v3)
{
	log() << warning << "Uniform4i not available." << std::endl;
}

static void fallback_glUniform4iARB(api::GLint location, api::GLint v0, api::GLint v1, api::GLint v2, api::GLint v3)
{
	log() << warning << "Uniform4iARB not available." << std::endl;
}

static void fallback_glUniform4iv(api::GLint location, api::GLsizei count, const api::GLint* value)
{
	log() << warning << "Uniform4iv not available." << std::endl;
}

static void fallback_glUniform4ivARB(api::GLint location, api::GLsizei count, const api::GLint* value)
{
	log() << warning << "Uniform4ivARB not available." << std::endl;
}

static void fallback_glUniform4ui(api::GLint location, api::GLuint v0, api::GLuint v1, api::GLuint v2, api::GLuint v3)
{
	log() << warning << "Uniform4ui not available." << std::endl;
}

static void fallback_glUniform4uiEXT(api::GLint location, api::GLuint v0, api::GLuint v1, api::GLuint v2, api::GLuint v3)
{
	log() << warning << "Uniform4uiEXT not available." << std::endl;
}

static void fallback_glUniform4uiv(api::GLint location, api::GLsizei count, const api::GLuint* value)
{
	log() << warning << "Uniform4uiv not available." << std::endl;
}

static void fallback_glUniform4uivEXT(api::GLint location, api::GLsizei count, const api::GLuint* value)
{
	log() << warning << "Uniform4uivEXT not available." << std::endl;
}

static void fallback_glUniformBlockBinding(api::GLuint program, api::GLuint uniformBlockIndex, api::GLuint uniformBlockBinding)
{
	log() << warning << "UniformBlockBinding not available." << std::endl;
}

static void fallback_glUniformBufferEXT(api::GLuint program, api::GLint location, api::GLuint buffer)
{
	log() << warning << "UniformBufferEXT not available." << std::endl;
}

static void fallback_glUniformMatrix2dv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "UniformMatrix2dv not available." << std::endl;
}

static void fallback_glUniformMatrix2fv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "UniformMatrix2fv not available." << std::endl;
}

static void fallback_glUniformMatrix2fvARB(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "UniformMatrix2fvARB not available." << std::endl;
}

static void fallback_glUniformMatrix2x3dv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "UniformMatrix2x3dv not available." << std::endl;
}

static void fallback_glUniformMatrix2x3fv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "UniformMatrix2x3fv not available." << std::endl;
}

static void fallback_glUniformMatrix2x4dv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "UniformMatrix2x4dv not available." << std::endl;
}

static void fallback_glUniformMatrix2x4fv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "UniformMatrix2x4fv not available." << std::endl;
}

static void fallback_glUniformMatrix3dv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "UniformMatrix3dv not available." << std::endl;
}

static void fallback_glUniformMatrix3fv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "UniformMatrix3fv not available." << std::endl;
}

static void fallback_glUniformMatrix3fvARB(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "UniformMatrix3fvARB not available." << std::endl;
}

static void fallback_glUniformMatrix3x2dv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "UniformMatrix3x2dv not available." << std::endl;
}

static void fallback_glUniformMatrix3x2fv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "UniformMatrix3x2fv not available." << std::endl;
}

static void fallback_glUniformMatrix3x4dv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "UniformMatrix3x4dv not available." << std::endl;
}

static void fallback_glUniformMatrix3x4fv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "UniformMatrix3x4fv not available." << std::endl;
}

static void fallback_glUniformMatrix4dv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "UniformMatrix4dv not available." << std::endl;
}

static void fallback_glUniformMatrix4fv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "UniformMatrix4fv not available." << std::endl;
}

static void fallback_glUniformMatrix4fvARB(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "UniformMatrix4fvARB not available." << std::endl;
}

static void fallback_glUniformMatrix4x2dv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "UniformMatrix4x2dv not available." << std::endl;
}

static void fallback_glUniformMatrix4x2fv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "UniformMatrix4x2fv not available." << std::endl;
}

static void fallback_glUniformMatrix4x3dv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "UniformMatrix4x3dv not available." << std::endl;
}

static void fallback_glUniformMatrix4x3fv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "UniformMatrix4x3fv not available." << std::endl;
}

static void fallback_glUniformSubroutinesuiv(api::GLenum shadertype, api::GLsizei count, const api::GLuint* indices)
{
	log() << warning << "UniformSubroutinesuiv not available." << std::endl;
}

static void fallback_glUniformui64NV(api::GLint location, api::GLuint64EXT value)
{
	log() << warning << "Uniformui64NV not available." << std::endl;
}

static void fallback_glUniformui64vNV(api::GLint location, api::GLsizei count, const api::GLuint64EXT* value)
{
	log() << warning << "Uniformui64vNV not available." << std::endl;
}

static void fallback_glUnlockArraysEXT()
{
	log() << warning << "UnlockArraysEXT not available." << std::endl;
}

static api::GLboolean fallback_glUnmapBuffer(api::GLenum target)
{
	log() << warning << "UnmapBuffer not available." << std::endl;
}

static api::GLboolean fallback_glUnmapBufferARB(api::GLenum target)
{
	log() << warning << "UnmapBufferARB not available." << std::endl;
}

static api::GLboolean fallback_glUnmapNamedBufferEXT(api::GLuint buffer)
{
	log() << warning << "UnmapNamedBufferEXT not available." << std::endl;
}

static void fallback_glUnmapObjectBufferATI(api::GLuint buffer)
{
	log() << warning << "UnmapObjectBufferATI not available." << std::endl;
}

static void fallback_glUpdateObjectBufferATI(api::GLuint buffer, api::GLuint offset, api::GLsizei size, const api::GLvoid* pointer, api::GLenum preserve)
{
	log() << warning << "UpdateObjectBufferATI not available." << std::endl;
}

static void fallback_glUseProgram(api::GLuint program)
{
	log() << warning << "UseProgram not available." << std::endl;
}

static void fallback_glUseProgramObjectARB(api::GLhandleARB programObj)
{
	log() << warning << "UseProgramObjectARB not available." << std::endl;
}

static void fallback_glUseShaderProgramEXT(api::GLenum type, api::GLuint program)
{
	log() << warning << "UseShaderProgramEXT not available." << std::endl;
}

static void fallback_glValidateProgram(api::GLuint program)
{
	log() << warning << "ValidateProgram not available." << std::endl;
}

static void fallback_glValidateProgramARB(api::GLhandleARB programObj)
{
	log() << warning << "ValidateProgramARB not available." << std::endl;
}

static void fallback_glVariantArrayObjectATI(api::GLuint id, api::GLenum type, api::GLsizei stride, api::GLuint buffer, api::GLuint offset)
{
	log() << warning << "VariantArrayObjectATI not available." << std::endl;
}

static void fallback_glVariantPointerEXT(api::GLuint id, api::GLenum type, api::GLuint stride, const api::GLvoid* addr)
{
	log() << warning << "VariantPointerEXT not available." << std::endl;
}

static void fallback_glVariantbvEXT(api::GLuint id, const api::GLbyte* addr)
{
	log() << warning << "VariantbvEXT not available." << std::endl;
}

static void fallback_glVariantdvEXT(api::GLuint id, const api::GLdouble* addr)
{
	log() << warning << "VariantdvEXT not available." << std::endl;
}

static void fallback_glVariantfvEXT(api::GLuint id, const api::GLfloat* addr)
{
	log() << warning << "VariantfvEXT not available." << std::endl;
}

static void fallback_glVariantivEXT(api::GLuint id, const api::GLint* addr)
{
	log() << warning << "VariantivEXT not available." << std::endl;
}

static void fallback_glVariantsvEXT(api::GLuint id, const api::GLshort* addr)
{
	log() << warning << "VariantsvEXT not available." << std::endl;
}

static void fallback_glVariantubvEXT(api::GLuint id, const api::GLubyte* addr)
{
	log() << warning << "VariantubvEXT not available." << std::endl;
}

static void fallback_glVariantuivEXT(api::GLuint id, const api::GLuint* addr)
{
	log() << warning << "VariantuivEXT not available." << std::endl;
}

static void fallback_glVariantusvEXT(api::GLuint id, const api::GLushort* addr)
{
	log() << warning << "VariantusvEXT not available." << std::endl;
}

static void fallback_glVertex2d(api::GLdouble x, api::GLdouble y)
{
	log() << warning << "Vertex2d not available." << std::endl;
}

static void fallback_glVertex2dv(const api::GLdouble* v)
{
	log() << warning << "Vertex2dv not available." << std::endl;
}

static void fallback_glVertex2f(api::GLfloat x, api::GLfloat y)
{
	log() << warning << "Vertex2f not available." << std::endl;
}

static void fallback_glVertex2fv(const api::GLfloat* v)
{
	log() << warning << "Vertex2fv not available." << std::endl;
}

static void fallback_glVertex2hNV(api::GLhalfNV x, api::GLhalfNV y)
{
	log() << warning << "Vertex2hNV not available." << std::endl;
}

static void fallback_glVertex2hvNV(const api::GLhalfNV* v)
{
	log() << warning << "Vertex2hvNV not available." << std::endl;
}

static void fallback_glVertex2i(api::GLint x, api::GLint y)
{
	log() << warning << "Vertex2i not available." << std::endl;
}

static void fallback_glVertex2iv(const api::GLint* v)
{
	log() << warning << "Vertex2iv not available." << std::endl;
}

static void fallback_glVertex2s(api::GLshort x, api::GLshort y)
{
	log() << warning << "Vertex2s not available." << std::endl;
}

static void fallback_glVertex2sv(const api::GLshort* v)
{
	log() << warning << "Vertex2sv not available." << std::endl;
}

static void fallback_glVertex3d(api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "Vertex3d not available." << std::endl;
}

static void fallback_glVertex3dv(const api::GLdouble* v)
{
	log() << warning << "Vertex3dv not available." << std::endl;
}

static void fallback_glVertex3f(api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "Vertex3f not available." << std::endl;
}

static void fallback_glVertex3fv(const api::GLfloat* v)
{
	log() << warning << "Vertex3fv not available." << std::endl;
}

static void fallback_glVertex3hNV(api::GLhalfNV x, api::GLhalfNV y, api::GLhalfNV z)
{
	log() << warning << "Vertex3hNV not available." << std::endl;
}

static void fallback_glVertex3hvNV(const api::GLhalfNV* v)
{
	log() << warning << "Vertex3hvNV not available." << std::endl;
}

static void fallback_glVertex3i(api::GLint x, api::GLint y, api::GLint z)
{
	log() << warning << "Vertex3i not available." << std::endl;
}

static void fallback_glVertex3iv(const api::GLint* v)
{
	log() << warning << "Vertex3iv not available." << std::endl;
}

static void fallback_glVertex3s(api::GLshort x, api::GLshort y, api::GLshort z)
{
	log() << warning << "Vertex3s not available." << std::endl;
}

static void fallback_glVertex3sv(const api::GLshort* v)
{
	log() << warning << "Vertex3sv not available." << std::endl;
}

static void fallback_glVertex4d(api::GLdouble x, api::GLdouble y, api::GLdouble z, api::GLdouble w)
{
	log() << warning << "Vertex4d not available." << std::endl;
}

static void fallback_glVertex4dv(const api::GLdouble* v)
{
	log() << warning << "Vertex4dv not available." << std::endl;
}

static void fallback_glVertex4f(api::GLfloat x, api::GLfloat y, api::GLfloat z, api::GLfloat w)
{
	log() << warning << "Vertex4f not available." << std::endl;
}

static void fallback_glVertex4fv(const api::GLfloat* v)
{
	log() << warning << "Vertex4fv not available." << std::endl;
}

static void fallback_glVertex4hNV(api::GLhalfNV x, api::GLhalfNV y, api::GLhalfNV z, api::GLhalfNV w)
{
	log() << warning << "Vertex4hNV not available." << std::endl;
}

static void fallback_glVertex4hvNV(const api::GLhalfNV* v)
{
	log() << warning << "Vertex4hvNV not available." << std::endl;
}

static void fallback_glVertex4i(api::GLint x, api::GLint y, api::GLint z, api::GLint w)
{
	log() << warning << "Vertex4i not available." << std::endl;
}

static void fallback_glVertex4iv(const api::GLint* v)
{
	log() << warning << "Vertex4iv not available." << std::endl;
}

static void fallback_glVertex4s(api::GLshort x, api::GLshort y, api::GLshort z, api::GLshort w)
{
	log() << warning << "Vertex4s not available." << std::endl;
}

static void fallback_glVertex4sv(const api::GLshort* v)
{
	log() << warning << "Vertex4sv not available." << std::endl;
}

static void fallback_glVertexArrayParameteriAPPLE(api::GLenum pname, api::GLint param)
{
	log() << warning << "VertexArrayParameteriAPPLE not available." << std::endl;
}

static void fallback_glVertexArrayRangeAPPLE(api::GLsizei length, api::GLvoid* pointer)
{
	log() << warning << "VertexArrayRangeAPPLE not available." << std::endl;
}

static void fallback_glVertexArrayRangeNV(api::GLsizei length, const api::GLvoid* pointer)
{
	log() << warning << "VertexArrayRangeNV not available." << std::endl;
}

static void fallback_glVertexAttrib1d(api::GLuint index, api::GLdouble x)
{
	log() << warning << "VertexAttrib1d not available." << std::endl;
}

static void fallback_glVertexAttrib1dARB(api::GLuint index, api::GLdouble x)
{
	log() << warning << "VertexAttrib1dARB not available." << std::endl;
}

static void fallback_glVertexAttrib1dNV(api::GLuint index, api::GLdouble x)
{
	log() << warning << "VertexAttrib1dNV not available." << std::endl;
}

static void fallback_glVertexAttrib1dv(api::GLuint index, const api::GLdouble* v)
{
	log() << warning << "VertexAttrib1dv not available." << std::endl;
}

static void fallback_glVertexAttrib1dvARB(api::GLuint index, const api::GLdouble* v)
{
	log() << warning << "VertexAttrib1dvARB not available." << std::endl;
}

static void fallback_glVertexAttrib1dvNV(api::GLuint index, const api::GLdouble* v)
{
	log() << warning << "VertexAttrib1dvNV not available." << std::endl;
}

static void fallback_glVertexAttrib1f(api::GLuint index, api::GLfloat x)
{
	log() << warning << "VertexAttrib1f not available." << std::endl;
}

static void fallback_glVertexAttrib1fARB(api::GLuint index, api::GLfloat x)
{
	log() << warning << "VertexAttrib1fARB not available." << std::endl;
}

static void fallback_glVertexAttrib1fNV(api::GLuint index, api::GLfloat x)
{
	log() << warning << "VertexAttrib1fNV not available." << std::endl;
}

static void fallback_glVertexAttrib1fv(api::GLuint index, const api::GLfloat* v)
{
	log() << warning << "VertexAttrib1fv not available." << std::endl;
}

static void fallback_glVertexAttrib1fvARB(api::GLuint index, const api::GLfloat* v)
{
	log() << warning << "VertexAttrib1fvARB not available." << std::endl;
}

static void fallback_glVertexAttrib1fvNV(api::GLuint index, const api::GLfloat* v)
{
	log() << warning << "VertexAttrib1fvNV not available." << std::endl;
}

static void fallback_glVertexAttrib1hNV(api::GLuint index, api::GLhalfNV x)
{
	log() << warning << "VertexAttrib1hNV not available." << std::endl;
}

static void fallback_glVertexAttrib1hvNV(api::GLuint index, const api::GLhalfNV* v)
{
	log() << warning << "VertexAttrib1hvNV not available." << std::endl;
}

static void fallback_glVertexAttrib1s(api::GLuint index, api::GLshort x)
{
	log() << warning << "VertexAttrib1s not available." << std::endl;
}

static void fallback_glVertexAttrib1sARB(api::GLuint index, api::GLshort x)
{
	log() << warning << "VertexAttrib1sARB not available." << std::endl;
}

static void fallback_glVertexAttrib1sNV(api::GLuint index, api::GLshort x)
{
	log() << warning << "VertexAttrib1sNV not available." << std::endl;
}

static void fallback_glVertexAttrib1sv(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "VertexAttrib1sv not available." << std::endl;
}

static void fallback_glVertexAttrib1svARB(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "VertexAttrib1svARB not available." << std::endl;
}

static void fallback_glVertexAttrib1svNV(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "VertexAttrib1svNV not available." << std::endl;
}

static void fallback_glVertexAttrib2d(api::GLuint index, api::GLdouble x, api::GLdouble y)
{
	log() << warning << "VertexAttrib2d not available." << std::endl;
}

static void fallback_glVertexAttrib2dARB(api::GLuint index, api::GLdouble x, api::GLdouble y)
{
	log() << warning << "VertexAttrib2dARB not available." << std::endl;
}

static void fallback_glVertexAttrib2dNV(api::GLuint index, api::GLdouble x, api::GLdouble y)
{
	log() << warning << "VertexAttrib2dNV not available." << std::endl;
}

static void fallback_glVertexAttrib2dv(api::GLuint index, const api::GLdouble* v)
{
	log() << warning << "VertexAttrib2dv not available." << std::endl;
}

static void fallback_glVertexAttrib2dvARB(api::GLuint index, const api::GLdouble* v)
{
	log() << warning << "VertexAttrib2dvARB not available." << std::endl;
}

static void fallback_glVertexAttrib2dvNV(api::GLuint index, const api::GLdouble* v)
{
	log() << warning << "VertexAttrib2dvNV not available." << std::endl;
}

static void fallback_glVertexAttrib2f(api::GLuint index, api::GLfloat x, api::GLfloat y)
{
	log() << warning << "VertexAttrib2f not available." << std::endl;
}

static void fallback_glVertexAttrib2fARB(api::GLuint index, api::GLfloat x, api::GLfloat y)
{
	log() << warning << "VertexAttrib2fARB not available." << std::endl;
}

static void fallback_glVertexAttrib2fNV(api::GLuint index, api::GLfloat x, api::GLfloat y)
{
	log() << warning << "VertexAttrib2fNV not available." << std::endl;
}

static void fallback_glVertexAttrib2fv(api::GLuint index, const api::GLfloat* v)
{
	log() << warning << "VertexAttrib2fv not available." << std::endl;
}

static void fallback_glVertexAttrib2fvARB(api::GLuint index, const api::GLfloat* v)
{
	log() << warning << "VertexAttrib2fvARB not available." << std::endl;
}

static void fallback_glVertexAttrib2fvNV(api::GLuint index, const api::GLfloat* v)
{
	log() << warning << "VertexAttrib2fvNV not available." << std::endl;
}

static void fallback_glVertexAttrib2hNV(api::GLuint index, api::GLhalfNV x, api::GLhalfNV y)
{
	log() << warning << "VertexAttrib2hNV not available." << std::endl;
}

static void fallback_glVertexAttrib2hvNV(api::GLuint index, const api::GLhalfNV* v)
{
	log() << warning << "VertexAttrib2hvNV not available." << std::endl;
}

static void fallback_glVertexAttrib2s(api::GLuint index, api::GLshort x, api::GLshort y)
{
	log() << warning << "VertexAttrib2s not available." << std::endl;
}

static void fallback_glVertexAttrib2sARB(api::GLuint index, api::GLshort x, api::GLshort y)
{
	log() << warning << "VertexAttrib2sARB not available." << std::endl;
}

static void fallback_glVertexAttrib2sNV(api::GLuint index, api::GLshort x, api::GLshort y)
{
	log() << warning << "VertexAttrib2sNV not available." << std::endl;
}

static void fallback_glVertexAttrib2sv(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "VertexAttrib2sv not available." << std::endl;
}

static void fallback_glVertexAttrib2svARB(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "VertexAttrib2svARB not available." << std::endl;
}

static void fallback_glVertexAttrib2svNV(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "VertexAttrib2svNV not available." << std::endl;
}

static void fallback_glVertexAttrib3d(api::GLuint index, api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "VertexAttrib3d not available." << std::endl;
}

static void fallback_glVertexAttrib3dARB(api::GLuint index, api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "VertexAttrib3dARB not available." << std::endl;
}

static void fallback_glVertexAttrib3dNV(api::GLuint index, api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "VertexAttrib3dNV not available." << std::endl;
}

static void fallback_glVertexAttrib3dv(api::GLuint index, const api::GLdouble* v)
{
	log() << warning << "VertexAttrib3dv not available." << std::endl;
}

static void fallback_glVertexAttrib3dvARB(api::GLuint index, const api::GLdouble* v)
{
	log() << warning << "VertexAttrib3dvARB not available." << std::endl;
}

static void fallback_glVertexAttrib3dvNV(api::GLuint index, const api::GLdouble* v)
{
	log() << warning << "VertexAttrib3dvNV not available." << std::endl;
}

static void fallback_glVertexAttrib3f(api::GLuint index, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "VertexAttrib3f not available." << std::endl;
}

static void fallback_glVertexAttrib3fARB(api::GLuint index, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "VertexAttrib3fARB not available." << std::endl;
}

static void fallback_glVertexAttrib3fNV(api::GLuint index, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "VertexAttrib3fNV not available." << std::endl;
}

static void fallback_glVertexAttrib3fv(api::GLuint index, const api::GLfloat* v)
{
	log() << warning << "VertexAttrib3fv not available." << std::endl;
}

static void fallback_glVertexAttrib3fvARB(api::GLuint index, const api::GLfloat* v)
{
	log() << warning << "VertexAttrib3fvARB not available." << std::endl;
}

static void fallback_glVertexAttrib3fvNV(api::GLuint index, const api::GLfloat* v)
{
	log() << warning << "VertexAttrib3fvNV not available." << std::endl;
}

static void fallback_glVertexAttrib3hNV(api::GLuint index, api::GLhalfNV x, api::GLhalfNV y, api::GLhalfNV z)
{
	log() << warning << "VertexAttrib3hNV not available." << std::endl;
}

static void fallback_glVertexAttrib3hvNV(api::GLuint index, const api::GLhalfNV* v)
{
	log() << warning << "VertexAttrib3hvNV not available." << std::endl;
}

static void fallback_glVertexAttrib3s(api::GLuint index, api::GLshort x, api::GLshort y, api::GLshort z)
{
	log() << warning << "VertexAttrib3s not available." << std::endl;
}

static void fallback_glVertexAttrib3sARB(api::GLuint index, api::GLshort x, api::GLshort y, api::GLshort z)
{
	log() << warning << "VertexAttrib3sARB not available." << std::endl;
}

static void fallback_glVertexAttrib3sNV(api::GLuint index, api::GLshort x, api::GLshort y, api::GLshort z)
{
	log() << warning << "VertexAttrib3sNV not available." << std::endl;
}

static void fallback_glVertexAttrib3sv(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "VertexAttrib3sv not available." << std::endl;
}

static void fallback_glVertexAttrib3svARB(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "VertexAttrib3svARB not available." << std::endl;
}

static void fallback_glVertexAttrib3svNV(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "VertexAttrib3svNV not available." << std::endl;
}

static void fallback_glVertexAttrib4Nbv(api::GLuint index, const api::GLbyte* v)
{
	log() << warning << "VertexAttrib4Nbv not available." << std::endl;
}

static void fallback_glVertexAttrib4NbvARB(api::GLuint index, const api::GLbyte* v)
{
	log() << warning << "VertexAttrib4NbvARB not available." << std::endl;
}

static void fallback_glVertexAttrib4Niv(api::GLuint index, const api::GLint* v)
{
	log() << warning << "VertexAttrib4Niv not available." << std::endl;
}

static void fallback_glVertexAttrib4NivARB(api::GLuint index, const api::GLint* v)
{
	log() << warning << "VertexAttrib4NivARB not available." << std::endl;
}

static void fallback_glVertexAttrib4Nsv(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "VertexAttrib4Nsv not available." << std::endl;
}

static void fallback_glVertexAttrib4NsvARB(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "VertexAttrib4NsvARB not available." << std::endl;
}

static void fallback_glVertexAttrib4Nub(api::GLuint index, api::GLubyte x, api::GLubyte y, api::GLubyte z, api::GLubyte w)
{
	log() << warning << "VertexAttrib4Nub not available." << std::endl;
}

static void fallback_glVertexAttrib4NubARB(api::GLuint index, api::GLubyte x, api::GLubyte y, api::GLubyte z, api::GLubyte w)
{
	log() << warning << "VertexAttrib4NubARB not available." << std::endl;
}

static void fallback_glVertexAttrib4Nubv(api::GLuint index, const api::GLubyte* v)
{
	log() << warning << "VertexAttrib4Nubv not available." << std::endl;
}

static void fallback_glVertexAttrib4NubvARB(api::GLuint index, const api::GLubyte* v)
{
	log() << warning << "VertexAttrib4NubvARB not available." << std::endl;
}

static void fallback_glVertexAttrib4Nuiv(api::GLuint index, const api::GLuint* v)
{
	log() << warning << "VertexAttrib4Nuiv not available." << std::endl;
}

static void fallback_glVertexAttrib4NuivARB(api::GLuint index, const api::GLuint* v)
{
	log() << warning << "VertexAttrib4NuivARB not available." << std::endl;
}

static void fallback_glVertexAttrib4Nusv(api::GLuint index, const api::GLushort* v)
{
	log() << warning << "VertexAttrib4Nusv not available." << std::endl;
}

static void fallback_glVertexAttrib4NusvARB(api::GLuint index, const api::GLushort* v)
{
	log() << warning << "VertexAttrib4NusvARB not available." << std::endl;
}

static void fallback_glVertexAttrib4bv(api::GLuint index, const api::GLbyte* v)
{
	log() << warning << "VertexAttrib4bv not available." << std::endl;
}

static void fallback_glVertexAttrib4bvARB(api::GLuint index, const api::GLbyte* v)
{
	log() << warning << "VertexAttrib4bvARB not available." << std::endl;
}

static void fallback_glVertexAttrib4d(api::GLuint index, api::GLdouble x, api::GLdouble y, api::GLdouble z, api::GLdouble w)
{
	log() << warning << "VertexAttrib4d not available." << std::endl;
}

static void fallback_glVertexAttrib4dARB(api::GLuint index, api::GLdouble x, api::GLdouble y, api::GLdouble z, api::GLdouble w)
{
	log() << warning << "VertexAttrib4dARB not available." << std::endl;
}

static void fallback_glVertexAttrib4dNV(api::GLuint index, api::GLdouble x, api::GLdouble y, api::GLdouble z, api::GLdouble w)
{
	log() << warning << "VertexAttrib4dNV not available." << std::endl;
}

static void fallback_glVertexAttrib4dv(api::GLuint index, const api::GLdouble* v)
{
	log() << warning << "VertexAttrib4dv not available." << std::endl;
}

static void fallback_glVertexAttrib4dvARB(api::GLuint index, const api::GLdouble* v)
{
	log() << warning << "VertexAttrib4dvARB not available." << std::endl;
}

static void fallback_glVertexAttrib4dvNV(api::GLuint index, const api::GLdouble* v)
{
	log() << warning << "VertexAttrib4dvNV not available." << std::endl;
}

static void fallback_glVertexAttrib4f(api::GLuint index, api::GLfloat x, api::GLfloat y, api::GLfloat z, api::GLfloat w)
{
	log() << warning << "VertexAttrib4f not available." << std::endl;
}

static void fallback_glVertexAttrib4fARB(api::GLuint index, api::GLfloat x, api::GLfloat y, api::GLfloat z, api::GLfloat w)
{
	log() << warning << "VertexAttrib4fARB not available." << std::endl;
}

static void fallback_glVertexAttrib4fNV(api::GLuint index, api::GLfloat x, api::GLfloat y, api::GLfloat z, api::GLfloat w)
{
	log() << warning << "VertexAttrib4fNV not available." << std::endl;
}

static void fallback_glVertexAttrib4fv(api::GLuint index, const api::GLfloat* v)
{
	log() << warning << "VertexAttrib4fv not available." << std::endl;
}

static void fallback_glVertexAttrib4fvARB(api::GLuint index, const api::GLfloat* v)
{
	log() << warning << "VertexAttrib4fvARB not available." << std::endl;
}

static void fallback_glVertexAttrib4fvNV(api::GLuint index, const api::GLfloat* v)
{
	log() << warning << "VertexAttrib4fvNV not available." << std::endl;
}

static void fallback_glVertexAttrib4hNV(api::GLuint index, api::GLhalfNV x, api::GLhalfNV y, api::GLhalfNV z, api::GLhalfNV w)
{
	log() << warning << "VertexAttrib4hNV not available." << std::endl;
}

static void fallback_glVertexAttrib4hvNV(api::GLuint index, const api::GLhalfNV* v)
{
	log() << warning << "VertexAttrib4hvNV not available." << std::endl;
}

static void fallback_glVertexAttrib4iv(api::GLuint index, const api::GLint* v)
{
	log() << warning << "VertexAttrib4iv not available." << std::endl;
}

static void fallback_glVertexAttrib4ivARB(api::GLuint index, const api::GLint* v)
{
	log() << warning << "VertexAttrib4ivARB not available." << std::endl;
}

static void fallback_glVertexAttrib4s(api::GLuint index, api::GLshort x, api::GLshort y, api::GLshort z, api::GLshort w)
{
	log() << warning << "VertexAttrib4s not available." << std::endl;
}

static void fallback_glVertexAttrib4sARB(api::GLuint index, api::GLshort x, api::GLshort y, api::GLshort z, api::GLshort w)
{
	log() << warning << "VertexAttrib4sARB not available." << std::endl;
}

static void fallback_glVertexAttrib4sNV(api::GLuint index, api::GLshort x, api::GLshort y, api::GLshort z, api::GLshort w)
{
	log() << warning << "VertexAttrib4sNV not available." << std::endl;
}

static void fallback_glVertexAttrib4sv(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "VertexAttrib4sv not available." << std::endl;
}

static void fallback_glVertexAttrib4svARB(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "VertexAttrib4svARB not available." << std::endl;
}

static void fallback_glVertexAttrib4svNV(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "VertexAttrib4svNV not available." << std::endl;
}

static void fallback_glVertexAttrib4ubNV(api::GLuint index, api::GLubyte x, api::GLubyte y, api::GLubyte z, api::GLubyte w)
{
	log() << warning << "VertexAttrib4ubNV not available." << std::endl;
}

static void fallback_glVertexAttrib4ubv(api::GLuint index, const api::GLubyte* v)
{
	log() << warning << "VertexAttrib4ubv not available." << std::endl;
}

static void fallback_glVertexAttrib4ubvARB(api::GLuint index, const api::GLubyte* v)
{
	log() << warning << "VertexAttrib4ubvARB not available." << std::endl;
}

static void fallback_glVertexAttrib4ubvNV(api::GLuint index, const api::GLubyte* v)
{
	log() << warning << "VertexAttrib4ubvNV not available." << std::endl;
}

static void fallback_glVertexAttrib4uiv(api::GLuint index, const api::GLuint* v)
{
	log() << warning << "VertexAttrib4uiv not available." << std::endl;
}

static void fallback_glVertexAttrib4uivARB(api::GLuint index, const api::GLuint* v)
{
	log() << warning << "VertexAttrib4uivARB not available." << std::endl;
}

static void fallback_glVertexAttrib4usv(api::GLuint index, const api::GLushort* v)
{
	log() << warning << "VertexAttrib4usv not available." << std::endl;
}

static void fallback_glVertexAttrib4usvARB(api::GLuint index, const api::GLushort* v)
{
	log() << warning << "VertexAttrib4usvARB not available." << std::endl;
}

static void fallback_glVertexAttribArrayObjectATI(api::GLuint index, api::GLint size, api::GLenum type, api::GLboolean normalized, api::GLsizei stride, api::GLuint buffer, api::GLuint offset)
{
	log() << warning << "VertexAttribArrayObjectATI not available." << std::endl;
}

static void fallback_glVertexAttribDivisorARB(api::GLuint index, api::GLuint divisor)
{
	log() << warning << "VertexAttribDivisorARB not available." << std::endl;
}

static void fallback_glVertexAttribFormatNV(api::GLuint index, api::GLint size, api::GLenum type, api::GLboolean normalized, api::GLsizei stride)
{
	log() << warning << "VertexAttribFormatNV not available." << std::endl;
}

static void fallback_glVertexAttribI1i(api::GLuint index, api::GLint x)
{
	log() << warning << "VertexAttribI1i not available." << std::endl;
}

static void fallback_glVertexAttribI1iEXT(api::GLuint index, api::GLint x)
{
	log() << warning << "VertexAttribI1iEXT not available." << std::endl;
}

static void fallback_glVertexAttribI1iv(api::GLuint index, const api::GLint* v)
{
	log() << warning << "VertexAttribI1iv not available." << std::endl;
}

static void fallback_glVertexAttribI1ivEXT(api::GLuint index, const api::GLint* v)
{
	log() << warning << "VertexAttribI1ivEXT not available." << std::endl;
}

static void fallback_glVertexAttribI1ui(api::GLuint index, api::GLuint x)
{
	log() << warning << "VertexAttribI1ui not available." << std::endl;
}

static void fallback_glVertexAttribI1uiEXT(api::GLuint index, api::GLuint x)
{
	log() << warning << "VertexAttribI1uiEXT not available." << std::endl;
}

static void fallback_glVertexAttribI1uiv(api::GLuint index, const api::GLuint* v)
{
	log() << warning << "VertexAttribI1uiv not available." << std::endl;
}

static void fallback_glVertexAttribI1uivEXT(api::GLuint index, const api::GLuint* v)
{
	log() << warning << "VertexAttribI1uivEXT not available." << std::endl;
}

static void fallback_glVertexAttribI2i(api::GLuint index, api::GLint x, api::GLint y)
{
	log() << warning << "VertexAttribI2i not available." << std::endl;
}

static void fallback_glVertexAttribI2iEXT(api::GLuint index, api::GLint x, api::GLint y)
{
	log() << warning << "VertexAttribI2iEXT not available." << std::endl;
}

static void fallback_glVertexAttribI2iv(api::GLuint index, const api::GLint* v)
{
	log() << warning << "VertexAttribI2iv not available." << std::endl;
}

static void fallback_glVertexAttribI2ivEXT(api::GLuint index, const api::GLint* v)
{
	log() << warning << "VertexAttribI2ivEXT not available." << std::endl;
}

static void fallback_glVertexAttribI2ui(api::GLuint index, api::GLuint x, api::GLuint y)
{
	log() << warning << "VertexAttribI2ui not available." << std::endl;
}

static void fallback_glVertexAttribI2uiEXT(api::GLuint index, api::GLuint x, api::GLuint y)
{
	log() << warning << "VertexAttribI2uiEXT not available." << std::endl;
}

static void fallback_glVertexAttribI2uiv(api::GLuint index, const api::GLuint* v)
{
	log() << warning << "VertexAttribI2uiv not available." << std::endl;
}

static void fallback_glVertexAttribI2uivEXT(api::GLuint index, const api::GLuint* v)
{
	log() << warning << "VertexAttribI2uivEXT not available." << std::endl;
}

static void fallback_glVertexAttribI3i(api::GLuint index, api::GLint x, api::GLint y, api::GLint z)
{
	log() << warning << "VertexAttribI3i not available." << std::endl;
}

static void fallback_glVertexAttribI3iEXT(api::GLuint index, api::GLint x, api::GLint y, api::GLint z)
{
	log() << warning << "VertexAttribI3iEXT not available." << std::endl;
}

static void fallback_glVertexAttribI3iv(api::GLuint index, const api::GLint* v)
{
	log() << warning << "VertexAttribI3iv not available." << std::endl;
}

static void fallback_glVertexAttribI3ivEXT(api::GLuint index, const api::GLint* v)
{
	log() << warning << "VertexAttribI3ivEXT not available." << std::endl;
}

static void fallback_glVertexAttribI3ui(api::GLuint index, api::GLuint x, api::GLuint y, api::GLuint z)
{
	log() << warning << "VertexAttribI3ui not available." << std::endl;
}

static void fallback_glVertexAttribI3uiEXT(api::GLuint index, api::GLuint x, api::GLuint y, api::GLuint z)
{
	log() << warning << "VertexAttribI3uiEXT not available." << std::endl;
}

static void fallback_glVertexAttribI3uiv(api::GLuint index, const api::GLuint* v)
{
	log() << warning << "VertexAttribI3uiv not available." << std::endl;
}

static void fallback_glVertexAttribI3uivEXT(api::GLuint index, const api::GLuint* v)
{
	log() << warning << "VertexAttribI3uivEXT not available." << std::endl;
}

static void fallback_glVertexAttribI4bv(api::GLuint index, const api::GLbyte* v)
{
	log() << warning << "VertexAttribI4bv not available." << std::endl;
}

static void fallback_glVertexAttribI4bvEXT(api::GLuint index, const api::GLbyte* v)
{
	log() << warning << "VertexAttribI4bvEXT not available." << std::endl;
}

static void fallback_glVertexAttribI4i(api::GLuint index, api::GLint x, api::GLint y, api::GLint z, api::GLint w)
{
	log() << warning << "VertexAttribI4i not available." << std::endl;
}

static void fallback_glVertexAttribI4iEXT(api::GLuint index, api::GLint x, api::GLint y, api::GLint z, api::GLint w)
{
	log() << warning << "VertexAttribI4iEXT not available." << std::endl;
}

static void fallback_glVertexAttribI4iv(api::GLuint index, const api::GLint* v)
{
	log() << warning << "VertexAttribI4iv not available." << std::endl;
}

static void fallback_glVertexAttribI4ivEXT(api::GLuint index, const api::GLint* v)
{
	log() << warning << "VertexAttribI4ivEXT not available." << std::endl;
}

static void fallback_glVertexAttribI4sv(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "VertexAttribI4sv not available." << std::endl;
}

static void fallback_glVertexAttribI4svEXT(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "VertexAttribI4svEXT not available." << std::endl;
}

static void fallback_glVertexAttribI4ubv(api::GLuint index, const api::GLubyte* v)
{
	log() << warning << "VertexAttribI4ubv not available." << std::endl;
}

static void fallback_glVertexAttribI4ubvEXT(api::GLuint index, const api::GLubyte* v)
{
	log() << warning << "VertexAttribI4ubvEXT not available." << std::endl;
}

static void fallback_glVertexAttribI4ui(api::GLuint index, api::GLuint x, api::GLuint y, api::GLuint z, api::GLuint w)
{
	log() << warning << "VertexAttribI4ui not available." << std::endl;
}

static void fallback_glVertexAttribI4uiEXT(api::GLuint index, api::GLuint x, api::GLuint y, api::GLuint z, api::GLuint w)
{
	log() << warning << "VertexAttribI4uiEXT not available." << std::endl;
}

static void fallback_glVertexAttribI4uiv(api::GLuint index, const api::GLuint* v)
{
	log() << warning << "VertexAttribI4uiv not available." << std::endl;
}

static void fallback_glVertexAttribI4uivEXT(api::GLuint index, const api::GLuint* v)
{
	log() << warning << "VertexAttribI4uivEXT not available." << std::endl;
}

static void fallback_glVertexAttribI4usv(api::GLuint index, const api::GLushort* v)
{
	log() << warning << "VertexAttribI4usv not available." << std::endl;
}

static void fallback_glVertexAttribI4usvEXT(api::GLuint index, const api::GLushort* v)
{
	log() << warning << "VertexAttribI4usvEXT not available." << std::endl;
}

static void fallback_glVertexAttribIFormatNV(api::GLuint index, api::GLint size, api::GLenum type, api::GLsizei stride)
{
	log() << warning << "VertexAttribIFormatNV not available." << std::endl;
}

static void fallback_glVertexAttribIPointer(api::GLuint index, api::GLint size, api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "VertexAttribIPointer not available." << std::endl;
}

static void fallback_glVertexAttribIPointerEXT(api::GLuint index, api::GLint size, api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "VertexAttribIPointerEXT not available." << std::endl;
}

static void fallback_glVertexAttribP1ui(api::GLuint index, api::GLenum type, api::GLboolean normalized, api::GLuint value)
{
	log() << warning << "VertexAttribP1ui not available." << std::endl;
}

static void fallback_glVertexAttribP1uiv(api::GLuint index, api::GLenum type, api::GLboolean normalized, const api::GLuint* value)
{
	log() << warning << "VertexAttribP1uiv not available." << std::endl;
}

static void fallback_glVertexAttribP2ui(api::GLuint index, api::GLenum type, api::GLboolean normalized, api::GLuint value)
{
	log() << warning << "VertexAttribP2ui not available." << std::endl;
}

static void fallback_glVertexAttribP2uiv(api::GLuint index, api::GLenum type, api::GLboolean normalized, const api::GLuint* value)
{
	log() << warning << "VertexAttribP2uiv not available." << std::endl;
}

static void fallback_glVertexAttribP3ui(api::GLuint index, api::GLenum type, api::GLboolean normalized, api::GLuint value)
{
	log() << warning << "VertexAttribP3ui not available." << std::endl;
}

static void fallback_glVertexAttribP3uiv(api::GLuint index, api::GLenum type, api::GLboolean normalized, const api::GLuint* value)
{
	log() << warning << "VertexAttribP3uiv not available." << std::endl;
}

static void fallback_glVertexAttribP4ui(api::GLuint index, api::GLenum type, api::GLboolean normalized, api::GLuint value)
{
	log() << warning << "VertexAttribP4ui not available." << std::endl;
}

static void fallback_glVertexAttribP4uiv(api::GLuint index, api::GLenum type, api::GLboolean normalized, const api::GLuint* value)
{
	log() << warning << "VertexAttribP4uiv not available." << std::endl;
}

static void fallback_glVertexAttribPointer(api::GLuint index, api::GLint size, api::GLenum type, api::GLboolean normalized, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "VertexAttribPointer not available." << std::endl;
}

static void fallback_glVertexAttribPointerARB(api::GLuint index, api::GLint size, api::GLenum type, api::GLboolean normalized, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "VertexAttribPointerARB not available." << std::endl;
}

static void fallback_glVertexAttribPointerNV(api::GLuint index, api::GLint fsize, api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "VertexAttribPointerNV not available." << std::endl;
}

static void fallback_glVertexAttribs1dvNV(api::GLuint index, api::GLsizei count, const api::GLdouble* v)
{
	log() << warning << "VertexAttribs1dvNV not available." << std::endl;
}

static void fallback_glVertexAttribs1fvNV(api::GLuint index, api::GLsizei count, const api::GLfloat* v)
{
	log() << warning << "VertexAttribs1fvNV not available." << std::endl;
}

static void fallback_glVertexAttribs1hvNV(api::GLuint index, api::GLsizei n, const api::GLhalfNV* v)
{
	log() << warning << "VertexAttribs1hvNV not available." << std::endl;
}

static void fallback_glVertexAttribs1svNV(api::GLuint index, api::GLsizei count, const api::GLshort* v)
{
	log() << warning << "VertexAttribs1svNV not available." << std::endl;
}

static void fallback_glVertexAttribs2dvNV(api::GLuint index, api::GLsizei count, const api::GLdouble* v)
{
	log() << warning << "VertexAttribs2dvNV not available." << std::endl;
}

static void fallback_glVertexAttribs2fvNV(api::GLuint index, api::GLsizei count, const api::GLfloat* v)
{
	log() << warning << "VertexAttribs2fvNV not available." << std::endl;
}

static void fallback_glVertexAttribs2hvNV(api::GLuint index, api::GLsizei n, const api::GLhalfNV* v)
{
	log() << warning << "VertexAttribs2hvNV not available." << std::endl;
}

static void fallback_glVertexAttribs2svNV(api::GLuint index, api::GLsizei count, const api::GLshort* v)
{
	log() << warning << "VertexAttribs2svNV not available." << std::endl;
}

static void fallback_glVertexAttribs3dvNV(api::GLuint index, api::GLsizei count, const api::GLdouble* v)
{
	log() << warning << "VertexAttribs3dvNV not available." << std::endl;
}

static void fallback_glVertexAttribs3fvNV(api::GLuint index, api::GLsizei count, const api::GLfloat* v)
{
	log() << warning << "VertexAttribs3fvNV not available." << std::endl;
}

static void fallback_glVertexAttribs3hvNV(api::GLuint index, api::GLsizei n, const api::GLhalfNV* v)
{
	log() << warning << "VertexAttribs3hvNV not available." << std::endl;
}

static void fallback_glVertexAttribs3svNV(api::GLuint index, api::GLsizei count, const api::GLshort* v)
{
	log() << warning << "VertexAttribs3svNV not available." << std::endl;
}

static void fallback_glVertexAttribs4dvNV(api::GLuint index, api::GLsizei count, const api::GLdouble* v)
{
	log() << warning << "VertexAttribs4dvNV not available." << std::endl;
}

static void fallback_glVertexAttribs4fvNV(api::GLuint index, api::GLsizei count, const api::GLfloat* v)
{
	log() << warning << "VertexAttribs4fvNV not available." << std::endl;
}

static void fallback_glVertexAttribs4hvNV(api::GLuint index, api::GLsizei n, const api::GLhalfNV* v)
{
	log() << warning << "VertexAttribs4hvNV not available." << std::endl;
}

static void fallback_glVertexAttribs4svNV(api::GLuint index, api::GLsizei count, const api::GLshort* v)
{
	log() << warning << "VertexAttribs4svNV not available." << std::endl;
}

static void fallback_glVertexAttribs4ubvNV(api::GLuint index, api::GLsizei count, const api::GLubyte* v)
{
	log() << warning << "VertexAttribs4ubvNV not available." << std::endl;
}

static void fallback_glVertexBlendARB(api::GLint count)
{
	log() << warning << "VertexBlendARB not available." << std::endl;
}

static void fallback_glVertexBlendEnvfATI(api::GLenum pname, api::GLfloat param)
{
	log() << warning << "VertexBlendEnvfATI not available." << std::endl;
}

static void fallback_glVertexBlendEnviATI(api::GLenum pname, api::GLint param)
{
	log() << warning << "VertexBlendEnviATI not available." << std::endl;
}

static void fallback_glVertexFormatNV(api::GLint size, api::GLenum type, api::GLsizei stride)
{
	log() << warning << "VertexFormatNV not available." << std::endl;
}

static void fallback_glVertexP2ui(api::GLenum type, api::GLuint value)
{
	log() << warning << "VertexP2ui not available." << std::endl;
}

static void fallback_glVertexP2uiv(api::GLenum type, const api::GLuint* value)
{
	log() << warning << "VertexP2uiv not available." << std::endl;
}

static void fallback_glVertexP3ui(api::GLenum type, api::GLuint value)
{
	log() << warning << "VertexP3ui not available." << std::endl;
}

static void fallback_glVertexP3uiv(api::GLenum type, const api::GLuint* value)
{
	log() << warning << "VertexP3uiv not available." << std::endl;
}

static void fallback_glVertexP4ui(api::GLenum type, api::GLuint value)
{
	log() << warning << "VertexP4ui not available." << std::endl;
}

static void fallback_glVertexP4uiv(api::GLenum type, const api::GLuint* value)
{
	log() << warning << "VertexP4uiv not available." << std::endl;
}

static void fallback_glVertexPointer(api::GLint size, api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "VertexPointer not available." << std::endl;
}

static void fallback_glVertexPointerEXT(api::GLint size, api::GLenum type, api::GLsizei stride, api::GLsizei count, const api::GLvoid* pointer)
{
	log() << warning << "VertexPointerEXT not available." << std::endl;
}

static void fallback_glVertexPointerListIBM(api::GLint size, api::GLenum type, api::GLint stride, const api::GLvoid** pointer, api::GLint ptrstride)
{
	log() << warning << "VertexPointerListIBM not available." << std::endl;
}

static void fallback_glVertexPointervINTEL(api::GLint size, api::GLenum type, const api::GLvoid** pointer)
{
	log() << warning << "VertexPointervINTEL not available." << std::endl;
}

static void fallback_glVertexStream1dATI(api::GLenum stream, api::GLdouble x)
{
	log() << warning << "VertexStream1dATI not available." << std::endl;
}

static void fallback_glVertexStream1dvATI(api::GLenum stream, const api::GLdouble* coords)
{
	log() << warning << "VertexStream1dvATI not available." << std::endl;
}

static void fallback_glVertexStream1fATI(api::GLenum stream, api::GLfloat x)
{
	log() << warning << "VertexStream1fATI not available." << std::endl;
}

static void fallback_glVertexStream1fvATI(api::GLenum stream, const api::GLfloat* coords)
{
	log() << warning << "VertexStream1fvATI not available." << std::endl;
}

static void fallback_glVertexStream1iATI(api::GLenum stream, api::GLint x)
{
	log() << warning << "VertexStream1iATI not available." << std::endl;
}

static void fallback_glVertexStream1ivATI(api::GLenum stream, const api::GLint* coords)
{
	log() << warning << "VertexStream1ivATI not available." << std::endl;
}

static void fallback_glVertexStream1sATI(api::GLenum stream, api::GLshort x)
{
	log() << warning << "VertexStream1sATI not available." << std::endl;
}

static void fallback_glVertexStream1svATI(api::GLenum stream, const api::GLshort* coords)
{
	log() << warning << "VertexStream1svATI not available." << std::endl;
}

static void fallback_glVertexStream2dATI(api::GLenum stream, api::GLdouble x, api::GLdouble y)
{
	log() << warning << "VertexStream2dATI not available." << std::endl;
}

static void fallback_glVertexStream2dvATI(api::GLenum stream, const api::GLdouble* coords)
{
	log() << warning << "VertexStream2dvATI not available." << std::endl;
}

static void fallback_glVertexStream2fATI(api::GLenum stream, api::GLfloat x, api::GLfloat y)
{
	log() << warning << "VertexStream2fATI not available." << std::endl;
}

static void fallback_glVertexStream2fvATI(api::GLenum stream, const api::GLfloat* coords)
{
	log() << warning << "VertexStream2fvATI not available." << std::endl;
}

static void fallback_glVertexStream2iATI(api::GLenum stream, api::GLint x, api::GLint y)
{
	log() << warning << "VertexStream2iATI not available." << std::endl;
}

static void fallback_glVertexStream2ivATI(api::GLenum stream, const api::GLint* coords)
{
	log() << warning << "VertexStream2ivATI not available." << std::endl;
}

static void fallback_glVertexStream2sATI(api::GLenum stream, api::GLshort x, api::GLshort y)
{
	log() << warning << "VertexStream2sATI not available." << std::endl;
}

static void fallback_glVertexStream2svATI(api::GLenum stream, const api::GLshort* coords)
{
	log() << warning << "VertexStream2svATI not available." << std::endl;
}

static void fallback_glVertexStream3dATI(api::GLenum stream, api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "VertexStream3dATI not available." << std::endl;
}

static void fallback_glVertexStream3dvATI(api::GLenum stream, const api::GLdouble* coords)
{
	log() << warning << "VertexStream3dvATI not available." << std::endl;
}

static void fallback_glVertexStream3fATI(api::GLenum stream, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "VertexStream3fATI not available." << std::endl;
}

static void fallback_glVertexStream3fvATI(api::GLenum stream, const api::GLfloat* coords)
{
	log() << warning << "VertexStream3fvATI not available." << std::endl;
}

static void fallback_glVertexStream3iATI(api::GLenum stream, api::GLint x, api::GLint y, api::GLint z)
{
	log() << warning << "VertexStream3iATI not available." << std::endl;
}

static void fallback_glVertexStream3ivATI(api::GLenum stream, const api::GLint* coords)
{
	log() << warning << "VertexStream3ivATI not available." << std::endl;
}

static void fallback_glVertexStream3sATI(api::GLenum stream, api::GLshort x, api::GLshort y, api::GLshort z)
{
	log() << warning << "VertexStream3sATI not available." << std::endl;
}

static void fallback_glVertexStream3svATI(api::GLenum stream, const api::GLshort* coords)
{
	log() << warning << "VertexStream3svATI not available." << std::endl;
}

static void fallback_glVertexStream4dATI(api::GLenum stream, api::GLdouble x, api::GLdouble y, api::GLdouble z, api::GLdouble w)
{
	log() << warning << "VertexStream4dATI not available." << std::endl;
}

static void fallback_glVertexStream4dvATI(api::GLenum stream, const api::GLdouble* coords)
{
	log() << warning << "VertexStream4dvATI not available." << std::endl;
}

static void fallback_glVertexStream4fATI(api::GLenum stream, api::GLfloat x, api::GLfloat y, api::GLfloat z, api::GLfloat w)
{
	log() << warning << "VertexStream4fATI not available." << std::endl;
}

static void fallback_glVertexStream4fvATI(api::GLenum stream, const api::GLfloat* coords)
{
	log() << warning << "VertexStream4fvATI not available." << std::endl;
}

static void fallback_glVertexStream4iATI(api::GLenum stream, api::GLint x, api::GLint y, api::GLint z, api::GLint w)
{
	log() << warning << "VertexStream4iATI not available." << std::endl;
}

static void fallback_glVertexStream4ivATI(api::GLenum stream, const api::GLint* coords)
{
	log() << warning << "VertexStream4ivATI not available." << std::endl;
}

static void fallback_glVertexStream4sATI(api::GLenum stream, api::GLshort x, api::GLshort y, api::GLshort z, api::GLshort w)
{
	log() << warning << "VertexStream4sATI not available." << std::endl;
}

static void fallback_glVertexStream4svATI(api::GLenum stream, const api::GLshort* coords)
{
	log() << warning << "VertexStream4svATI not available." << std::endl;
}

static void fallback_glVertexWeightPointerEXT(api::GLsizei size, api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "VertexWeightPointerEXT not available." << std::endl;
}

static void fallback_glVertexWeightfEXT(api::GLfloat weight)
{
	log() << warning << "VertexWeightfEXT not available." << std::endl;
}

static void fallback_glVertexWeightfvEXT(const api::GLfloat* weight)
{
	log() << warning << "VertexWeightfvEXT not available." << std::endl;
}

static void fallback_glVertexWeighthNV(api::GLhalfNV weight)
{
	log() << warning << "VertexWeighthNV not available." << std::endl;
}

static void fallback_glVertexWeighthvNV(const api::GLhalfNV* weight)
{
	log() << warning << "VertexWeighthvNV not available." << std::endl;
}

static api::GLenum fallback_glVideoCaptureNV(api::GLuint video_capture_slot)
{
	log() << warning << "VideoCaptureNV not available." << std::endl;
}

static void fallback_glVideoCaptureStreamParameterdvNV(api::GLuint video_capture_slot, api::GLuint stream, api::GLenum pname, const api::GLdouble* params)
{
	log() << warning << "VideoCaptureStreamParameterdvNV not available." << std::endl;
}

static void fallback_glVideoCaptureStreamParameterfvNV(api::GLuint video_capture_slot, api::GLuint stream, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "VideoCaptureStreamParameterfvNV not available." << std::endl;
}

static void fallback_glVideoCaptureStreamParameterivNV(api::GLuint video_capture_slot, api::GLuint stream, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "VideoCaptureStreamParameterivNV not available." << std::endl;
}

static void fallback_glViewport(api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "Viewport not available." << std::endl;
}

static void fallback_glWaitSync(api::GLsync sync, api::GLbitfield flags, api::GLuint64 timeout)
{
	log() << warning << "WaitSync not available." << std::endl;
}

static void fallback_glWeightPointerARB(api::GLint size, api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "WeightPointerARB not available." << std::endl;
}

static void fallback_glWeightbvARB(api::GLint size, const api::GLbyte* weights)
{
	log() << warning << "WeightbvARB not available." << std::endl;
}

static void fallback_glWeightdvARB(api::GLint size, const api::GLdouble* weights)
{
	log() << warning << "WeightdvARB not available." << std::endl;
}

static void fallback_glWeightfvARB(api::GLint size, const api::GLfloat* weights)
{
	log() << warning << "WeightfvARB not available." << std::endl;
}

static void fallback_glWeightivARB(api::GLint size, const api::GLint* weights)
{
	log() << warning << "WeightivARB not available." << std::endl;
}

static void fallback_glWeightsvARB(api::GLint size, const api::GLshort* weights)
{
	log() << warning << "WeightsvARB not available." << std::endl;
}

static void fallback_glWeightubvARB(api::GLint size, const api::GLubyte* weights)
{
	log() << warning << "WeightubvARB not available." << std::endl;
}

static void fallback_glWeightuivARB(api::GLint size, const api::GLuint* weights)
{
	log() << warning << "WeightuivARB not available." << std::endl;
}

static void fallback_glWeightusvARB(api::GLint size, const api::GLushort* weights)
{
	log() << warning << "WeightusvARB not available." << std::endl;
}

static void fallback_glWindowPos2d(api::GLdouble x, api::GLdouble y)
{
	log() << warning << "WindowPos2d not available." << std::endl;
}

static void fallback_glWindowPos2dARB(api::GLdouble x, api::GLdouble y)
{
	log() << warning << "WindowPos2dARB not available." << std::endl;
}

static void fallback_glWindowPos2dMESA(api::GLdouble x, api::GLdouble y)
{
	log() << warning << "WindowPos2dMESA not available." << std::endl;
}

static void fallback_glWindowPos2dv(const api::GLdouble* v)
{
	log() << warning << "WindowPos2dv not available." << std::endl;
}

static void fallback_glWindowPos2dvARB(const api::GLdouble* v)
{
	log() << warning << "WindowPos2dvARB not available." << std::endl;
}

static void fallback_glWindowPos2dvMESA(const api::GLdouble* v)
{
	log() << warning << "WindowPos2dvMESA not available." << std::endl;
}

static void fallback_glWindowPos2f(api::GLfloat x, api::GLfloat y)
{
	log() << warning << "WindowPos2f not available." << std::endl;
}

static void fallback_glWindowPos2fARB(api::GLfloat x, api::GLfloat y)
{
	log() << warning << "WindowPos2fARB not available." << std::endl;
}

static void fallback_glWindowPos2fMESA(api::GLfloat x, api::GLfloat y)
{
	log() << warning << "WindowPos2fMESA not available." << std::endl;
}

static void fallback_glWindowPos2fv(const api::GLfloat* v)
{
	log() << warning << "WindowPos2fv not available." << std::endl;
}

static void fallback_glWindowPos2fvARB(const api::GLfloat* v)
{
	log() << warning << "WindowPos2fvARB not available." << std::endl;
}

static void fallback_glWindowPos2fvMESA(const api::GLfloat* v)
{
	log() << warning << "WindowPos2fvMESA not available." << std::endl;
}

static void fallback_glWindowPos2i(api::GLint x, api::GLint y)
{
	log() << warning << "WindowPos2i not available." << std::endl;
}

static void fallback_glWindowPos2iARB(api::GLint x, api::GLint y)
{
	log() << warning << "WindowPos2iARB not available." << std::endl;
}

static void fallback_glWindowPos2iMESA(api::GLint x, api::GLint y)
{
	log() << warning << "WindowPos2iMESA not available." << std::endl;
}

static void fallback_glWindowPos2iv(const api::GLint* v)
{
	log() << warning << "WindowPos2iv not available." << std::endl;
}

static void fallback_glWindowPos2ivARB(const api::GLint* v)
{
	log() << warning << "WindowPos2ivARB not available." << std::endl;
}

static void fallback_glWindowPos2ivMESA(const api::GLint* v)
{
	log() << warning << "WindowPos2ivMESA not available." << std::endl;
}

static void fallback_glWindowPos2s(api::GLshort x, api::GLshort y)
{
	log() << warning << "WindowPos2s not available." << std::endl;
}

static void fallback_glWindowPos2sARB(api::GLshort x, api::GLshort y)
{
	log() << warning << "WindowPos2sARB not available." << std::endl;
}

static void fallback_glWindowPos2sMESA(api::GLshort x, api::GLshort y)
{
	log() << warning << "WindowPos2sMESA not available." << std::endl;
}

static void fallback_glWindowPos2sv(const api::GLshort* v)
{
	log() << warning << "WindowPos2sv not available." << std::endl;
}

static void fallback_glWindowPos2svARB(const api::GLshort* v)
{
	log() << warning << "WindowPos2svARB not available." << std::endl;
}

static void fallback_glWindowPos2svMESA(const api::GLshort* v)
{
	log() << warning << "WindowPos2svMESA not available." << std::endl;
}

static void fallback_glWindowPos3d(api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "WindowPos3d not available." << std::endl;
}

static void fallback_glWindowPos3dARB(api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "WindowPos3dARB not available." << std::endl;
}

static void fallback_glWindowPos3dMESA(api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "WindowPos3dMESA not available." << std::endl;
}

static void fallback_glWindowPos3dv(const api::GLdouble* v)
{
	log() << warning << "WindowPos3dv not available." << std::endl;
}

static void fallback_glWindowPos3dvARB(const api::GLdouble* v)
{
	log() << warning << "WindowPos3dvARB not available." << std::endl;
}

static void fallback_glWindowPos3dvMESA(const api::GLdouble* v)
{
	log() << warning << "WindowPos3dvMESA not available." << std::endl;
}

static void fallback_glWindowPos3f(api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "WindowPos3f not available." << std::endl;
}

static void fallback_glWindowPos3fARB(api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "WindowPos3fARB not available." << std::endl;
}

static void fallback_glWindowPos3fMESA(api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "WindowPos3fMESA not available." << std::endl;
}

static void fallback_glWindowPos3fv(const api::GLfloat* v)
{
	log() << warning << "WindowPos3fv not available." << std::endl;
}

static void fallback_glWindowPos3fvARB(const api::GLfloat* v)
{
	log() << warning << "WindowPos3fvARB not available." << std::endl;
}

static void fallback_glWindowPos3fvMESA(const api::GLfloat* v)
{
	log() << warning << "WindowPos3fvMESA not available." << std::endl;
}

static void fallback_glWindowPos3i(api::GLint x, api::GLint y, api::GLint z)
{
	log() << warning << "WindowPos3i not available." << std::endl;
}

static void fallback_glWindowPos3iARB(api::GLint x, api::GLint y, api::GLint z)
{
	log() << warning << "WindowPos3iARB not available." << std::endl;
}

static void fallback_glWindowPos3iMESA(api::GLint x, api::GLint y, api::GLint z)
{
	log() << warning << "WindowPos3iMESA not available." << std::endl;
}

static void fallback_glWindowPos3iv(const api::GLint* v)
{
	log() << warning << "WindowPos3iv not available." << std::endl;
}

static void fallback_glWindowPos3ivARB(const api::GLint* v)
{
	log() << warning << "WindowPos3ivARB not available." << std::endl;
}

static void fallback_glWindowPos3ivMESA(const api::GLint* v)
{
	log() << warning << "WindowPos3ivMESA not available." << std::endl;
}

static void fallback_glWindowPos3s(api::GLshort x, api::GLshort y, api::GLshort z)
{
	log() << warning << "WindowPos3s not available." << std::endl;
}

static void fallback_glWindowPos3sARB(api::GLshort x, api::GLshort y, api::GLshort z)
{
	log() << warning << "WindowPos3sARB not available." << std::endl;
}

static void fallback_glWindowPos3sMESA(api::GLshort x, api::GLshort y, api::GLshort z)
{
	log() << warning << "WindowPos3sMESA not available." << std::endl;
}

static void fallback_glWindowPos3sv(const api::GLshort* v)
{
	log() << warning << "WindowPos3sv not available." << std::endl;
}

static void fallback_glWindowPos3svARB(const api::GLshort* v)
{
	log() << warning << "WindowPos3svARB not available." << std::endl;
}

static void fallback_glWindowPos3svMESA(const api::GLshort* v)
{
	log() << warning << "WindowPos3svMESA not available." << std::endl;
}

static void fallback_glWindowPos4dMESA(api::GLdouble x, api::GLdouble y, api::GLdouble z, api::GLdouble w)
{
	log() << warning << "WindowPos4dMESA not available." << std::endl;
}

static void fallback_glWindowPos4dvMESA(const api::GLdouble* v)
{
	log() << warning << "WindowPos4dvMESA not available." << std::endl;
}

static void fallback_glWindowPos4fMESA(api::GLfloat x, api::GLfloat y, api::GLfloat z, api::GLfloat w)
{
	log() << warning << "WindowPos4fMESA not available." << std::endl;
}

static void fallback_glWindowPos4fvMESA(const api::GLfloat* v)
{
	log() << warning << "WindowPos4fvMESA not available." << std::endl;
}

static void fallback_glWindowPos4iMESA(api::GLint x, api::GLint y, api::GLint z, api::GLint w)
{
	log() << warning << "WindowPos4iMESA not available." << std::endl;
}

static void fallback_glWindowPos4ivMESA(const api::GLint* v)
{
	log() << warning << "WindowPos4ivMESA not available." << std::endl;
}

static void fallback_glWindowPos4sMESA(api::GLshort x, api::GLshort y, api::GLshort z, api::GLshort w)
{
	log() << warning << "WindowPos4sMESA not available." << std::endl;
}

static void fallback_glWindowPos4svMESA(const api::GLshort* v)
{
	log() << warning << "WindowPos4svMESA not available." << std::endl;
}

static void fallback_glWriteMaskEXT(api::GLuint res, api::GLuint in, api::GLenum outX, api::GLenum outY, api::GLenum outZ, api::GLenum outW)
{
	log() << warning << "WriteMaskEXT not available." << std::endl;
}

api::api() :
	glAccum(fallback_glAccum),
	glActiveProgramEXT(fallback_glActiveProgramEXT),
	glActiveStencilFaceEXT(fallback_glActiveStencilFaceEXT),
	glActiveTexture(fallback_glActiveTexture),
	glActiveTextureARB(fallback_glActiveTextureARB),
	glActiveVaryingNV(fallback_glActiveVaryingNV),
	glAlphaFragmentOp1ATI(fallback_glAlphaFragmentOp1ATI),
	glAlphaFragmentOp2ATI(fallback_glAlphaFragmentOp2ATI),
	glAlphaFragmentOp3ATI(fallback_glAlphaFragmentOp3ATI),
	glAlphaFunc(fallback_glAlphaFunc),
	glApplyTextureEXT(fallback_glApplyTextureEXT),
	glAreProgramsResidentNV(fallback_glAreProgramsResidentNV),
	glAreTexturesResident(fallback_glAreTexturesResident),
	glAreTexturesResidentEXT(fallback_glAreTexturesResidentEXT),
	glArrayElement(fallback_glArrayElement),
	glArrayElementEXT(fallback_glArrayElementEXT),
	glArrayObjectATI(fallback_glArrayObjectATI),
	glAsyncMarkerSGIX(fallback_glAsyncMarkerSGIX),
	glAttachObjectARB(fallback_glAttachObjectARB),
	glAttachShader(fallback_glAttachShader),
	glBegin(fallback_glBegin),
	glBeginConditionalRender(fallback_glBeginConditionalRender),
	glBeginConditionalRenderNV(fallback_glBeginConditionalRenderNV),
	glBeginFragmentShaderATI(fallback_glBeginFragmentShaderATI),
	glBeginOcclusionQueryNV(fallback_glBeginOcclusionQueryNV),
	glBeginPerfMonitorAMD(fallback_glBeginPerfMonitorAMD),
	glBeginQuery(fallback_glBeginQuery),
	glBeginQueryARB(fallback_glBeginQueryARB),
	glBeginQueryIndexed(fallback_glBeginQueryIndexed),
	glBeginTransformFeedback(fallback_glBeginTransformFeedback),
	glBeginTransformFeedbackEXT(fallback_glBeginTransformFeedbackEXT),
	glBeginTransformFeedbackNV(fallback_glBeginTransformFeedbackNV),
	glBeginVertexShaderEXT(fallback_glBeginVertexShaderEXT),
	glBeginVideoCaptureNV(fallback_glBeginVideoCaptureNV),
	glBindAttribLocation(fallback_glBindAttribLocation),
	glBindAttribLocationARB(fallback_glBindAttribLocationARB),
	glBindBuffer(fallback_glBindBuffer),
	glBindBufferARB(fallback_glBindBufferARB),
	glBindBufferBase(fallback_glBindBufferBase),
	glBindBufferBaseEXT(fallback_glBindBufferBaseEXT),
	glBindBufferBaseNV(fallback_glBindBufferBaseNV),
	glBindBufferOffsetEXT(fallback_glBindBufferOffsetEXT),
	glBindBufferOffsetNV(fallback_glBindBufferOffsetNV),
	glBindBufferRange(fallback_glBindBufferRange),
	glBindBufferRangeEXT(fallback_glBindBufferRangeEXT),
	glBindBufferRangeNV(fallback_glBindBufferRangeNV),
	glBindFragDataLocation(fallback_glBindFragDataLocation),
	glBindFragDataLocationEXT(fallback_glBindFragDataLocationEXT),
	glBindFragDataLocationIndexed(fallback_glBindFragDataLocationIndexed),
	glBindFragmentShaderATI(fallback_glBindFragmentShaderATI),
	glBindFramebuffer(fallback_glBindFramebuffer),
	glBindFramebufferEXT(fallback_glBindFramebufferEXT),
	glBindLightParameterEXT(fallback_glBindLightParameterEXT),
	glBindMaterialParameterEXT(fallback_glBindMaterialParameterEXT),
	glBindMultiTextureEXT(fallback_glBindMultiTextureEXT),
	glBindParameterEXT(fallback_glBindParameterEXT),
	glBindProgramARB(fallback_glBindProgramARB),
	glBindProgramNV(fallback_glBindProgramNV),
	glBindRenderbuffer(fallback_glBindRenderbuffer),
	glBindRenderbufferEXT(fallback_glBindRenderbufferEXT),
	glBindSampler(fallback_glBindSampler),
	glBindTexGenParameterEXT(fallback_glBindTexGenParameterEXT),
	glBindTexture(fallback_glBindTexture),
	glBindTextureEXT(fallback_glBindTextureEXT),
	glBindTextureUnitParameterEXT(fallback_glBindTextureUnitParameterEXT),
	glBindTransformFeedback(fallback_glBindTransformFeedback),
	glBindTransformFeedbackNV(fallback_glBindTransformFeedbackNV),
	glBindVertexArray(fallback_glBindVertexArray),
	glBindVertexArrayAPPLE(fallback_glBindVertexArrayAPPLE),
	glBindVertexShaderEXT(fallback_glBindVertexShaderEXT),
	glBindVideoCaptureStreamBufferNV(fallback_glBindVideoCaptureStreamBufferNV),
	glBindVideoCaptureStreamTextureNV(fallback_glBindVideoCaptureStreamTextureNV),
	glBinormal3bEXT(fallback_glBinormal3bEXT),
	glBinormal3bvEXT(fallback_glBinormal3bvEXT),
	glBinormal3dEXT(fallback_glBinormal3dEXT),
	glBinormal3dvEXT(fallback_glBinormal3dvEXT),
	glBinormal3fEXT(fallback_glBinormal3fEXT),
	glBinormal3fvEXT(fallback_glBinormal3fvEXT),
	glBinormal3iEXT(fallback_glBinormal3iEXT),
	glBinormal3ivEXT(fallback_glBinormal3ivEXT),
	glBinormal3sEXT(fallback_glBinormal3sEXT),
	glBinormal3svEXT(fallback_glBinormal3svEXT),
	glBinormalPointerEXT(fallback_glBinormalPointerEXT),
	glBitmap(fallback_glBitmap),
	glBlendColor(fallback_glBlendColor),
	glBlendColorEXT(fallback_glBlendColorEXT),
	glBlendEquation(fallback_glBlendEquation),
	glBlendEquationEXT(fallback_glBlendEquationEXT),
	glBlendEquationIndexedAMD(fallback_glBlendEquationIndexedAMD),
	glBlendEquationSeparate(fallback_glBlendEquationSeparate),
	glBlendEquationSeparateEXT(fallback_glBlendEquationSeparateEXT),
	glBlendEquationSeparateIndexedAMD(fallback_glBlendEquationSeparateIndexedAMD),
	glBlendEquationSeparatei(fallback_glBlendEquationSeparatei),
	glBlendEquationi(fallback_glBlendEquationi),
	glBlendFunc(fallback_glBlendFunc),
	glBlendFuncIndexedAMD(fallback_glBlendFuncIndexedAMD),
	glBlendFuncSeparate(fallback_glBlendFuncSeparate),
	glBlendFuncSeparateEXT(fallback_glBlendFuncSeparateEXT),
	glBlendFuncSeparateINGR(fallback_glBlendFuncSeparateINGR),
	glBlendFuncSeparateIndexedAMD(fallback_glBlendFuncSeparateIndexedAMD),
	glBlendFuncSeparatei(fallback_glBlendFuncSeparatei),
	glBlendFunci(fallback_glBlendFunci),
	glBlitFramebuffer(fallback_glBlitFramebuffer),
	glBlitFramebufferEXT(fallback_glBlitFramebufferEXT),
	glBufferAddressRangeNV(fallback_glBufferAddressRangeNV),
	glBufferData(fallback_glBufferData),
	glBufferDataARB(fallback_glBufferDataARB),
	glBufferParameteriAPPLE(fallback_glBufferParameteriAPPLE),
	glBufferSubData(fallback_glBufferSubData),
	glBufferSubDataARB(fallback_glBufferSubDataARB),
	glCallList(fallback_glCallList),
	glCallLists(fallback_glCallLists),
	glCheckFramebufferStatus(fallback_glCheckFramebufferStatus),
	glCheckFramebufferStatusEXT(fallback_glCheckFramebufferStatusEXT),
	glCheckNamedFramebufferStatusEXT(fallback_glCheckNamedFramebufferStatusEXT),
	glClampColor(fallback_glClampColor),
	glClampColorARB(fallback_glClampColorARB),
	glClear(fallback_glClear),
	glClearAccum(fallback_glClearAccum),
	glClearBufferfi(fallback_glClearBufferfi),
	glClearBufferfv(fallback_glClearBufferfv),
	glClearBufferiv(fallback_glClearBufferiv),
	glClearBufferuiv(fallback_glClearBufferuiv),
	glClearColor(fallback_glClearColor),
	glClearColorIiEXT(fallback_glClearColorIiEXT),
	glClearColorIuiEXT(fallback_glClearColorIuiEXT),
	glClearDepth(fallback_glClearDepth),
	glClearDepthdNV(fallback_glClearDepthdNV),
	glClearIndex(fallback_glClearIndex),
	glClearStencil(fallback_glClearStencil),
	glClientActiveTexture(fallback_glClientActiveTexture),
	glClientActiveTextureARB(fallback_glClientActiveTextureARB),
	glClientActiveVertexStreamATI(fallback_glClientActiveVertexStreamATI),
	glClientAttribDefaultEXT(fallback_glClientAttribDefaultEXT),
	glClientWaitSync(fallback_glClientWaitSync),
	glClipPlane(fallback_glClipPlane),
	glColor3b(fallback_glColor3b),
	glColor3bv(fallback_glColor3bv),
	glColor3d(fallback_glColor3d),
	glColor3dv(fallback_glColor3dv),
	glColor3f(fallback_glColor3f),
	glColor3fVertex3fSUN(fallback_glColor3fVertex3fSUN),
	glColor3fVertex3fvSUN(fallback_glColor3fVertex3fvSUN),
	glColor3fv(fallback_glColor3fv),
	glColor3hNV(fallback_glColor3hNV),
	glColor3hvNV(fallback_glColor3hvNV),
	glColor3i(fallback_glColor3i),
	glColor3iv(fallback_glColor3iv),
	glColor3s(fallback_glColor3s),
	glColor3sv(fallback_glColor3sv),
	glColor3ub(fallback_glColor3ub),
	glColor3ubv(fallback_glColor3ubv),
	glColor3ui(fallback_glColor3ui),
	glColor3uiv(fallback_glColor3uiv),
	glColor3us(fallback_glColor3us),
	glColor3usv(fallback_glColor3usv),
	glColor4b(fallback_glColor4b),
	glColor4bv(fallback_glColor4bv),
	glColor4d(fallback_glColor4d),
	glColor4dv(fallback_glColor4dv),
	glColor4f(fallback_glColor4f),
	glColor4fNormal3fVertex3fSUN(fallback_glColor4fNormal3fVertex3fSUN),
	glColor4fNormal3fVertex3fvSUN(fallback_glColor4fNormal3fVertex3fvSUN),
	glColor4fv(fallback_glColor4fv),
	glColor4hNV(fallback_glColor4hNV),
	glColor4hvNV(fallback_glColor4hvNV),
	glColor4i(fallback_glColor4i),
	glColor4iv(fallback_glColor4iv),
	glColor4s(fallback_glColor4s),
	glColor4sv(fallback_glColor4sv),
	glColor4ub(fallback_glColor4ub),
	glColor4ubVertex2fSUN(fallback_glColor4ubVertex2fSUN),
	glColor4ubVertex2fvSUN(fallback_glColor4ubVertex2fvSUN),
	glColor4ubVertex3fSUN(fallback_glColor4ubVertex3fSUN),
	glColor4ubVertex3fvSUN(fallback_glColor4ubVertex3fvSUN),
	glColor4ubv(fallback_glColor4ubv),
	glColor4ui(fallback_glColor4ui),
	glColor4uiv(fallback_glColor4uiv),
	glColor4us(fallback_glColor4us),
	glColor4usv(fallback_glColor4usv),
	glColorFormatNV(fallback_glColorFormatNV),
	glColorFragmentOp1ATI(fallback_glColorFragmentOp1ATI),
	glColorFragmentOp2ATI(fallback_glColorFragmentOp2ATI),
	glColorFragmentOp3ATI(fallback_glColorFragmentOp3ATI),
	glColorMask(fallback_glColorMask),
	glColorMaskIndexedEXT(fallback_glColorMaskIndexedEXT),
	glColorMaski(fallback_glColorMaski),
	glColorMaterial(fallback_glColorMaterial),
	glColorP3ui(fallback_glColorP3ui),
	glColorP3uiv(fallback_glColorP3uiv),
	glColorP4ui(fallback_glColorP4ui),
	glColorP4uiv(fallback_glColorP4uiv),
	glColorPointer(fallback_glColorPointer),
	glColorPointerEXT(fallback_glColorPointerEXT),
	glColorPointerListIBM(fallback_glColorPointerListIBM),
	glColorPointervINTEL(fallback_glColorPointervINTEL),
	glColorSubTable(fallback_glColorSubTable),
	glColorSubTableEXT(fallback_glColorSubTableEXT),
	glColorTable(fallback_glColorTable),
	glColorTableEXT(fallback_glColorTableEXT),
	glColorTableParameterfv(fallback_glColorTableParameterfv),
	glColorTableParameterfvSGI(fallback_glColorTableParameterfvSGI),
	glColorTableParameteriv(fallback_glColorTableParameteriv),
	glColorTableParameterivSGI(fallback_glColorTableParameterivSGI),
	glColorTableSGI(fallback_glColorTableSGI),
	glCombinerInputNV(fallback_glCombinerInputNV),
	glCombinerOutputNV(fallback_glCombinerOutputNV),
	glCombinerParameterfNV(fallback_glCombinerParameterfNV),
	glCombinerParameterfvNV(fallback_glCombinerParameterfvNV),
	glCombinerParameteriNV(fallback_glCombinerParameteriNV),
	glCombinerParameterivNV(fallback_glCombinerParameterivNV),
	glCombinerStageParameterfvNV(fallback_glCombinerStageParameterfvNV),
	glCompileShader(fallback_glCompileShader),
	glCompileShaderARB(fallback_glCompileShaderARB),
	glCompileShaderIncludeARB(fallback_glCompileShaderIncludeARB),
	glCompressedMultiTexImage1DEXT(fallback_glCompressedMultiTexImage1DEXT),
	glCompressedMultiTexImage2DEXT(fallback_glCompressedMultiTexImage2DEXT),
	glCompressedMultiTexImage3DEXT(fallback_glCompressedMultiTexImage3DEXT),
	glCompressedMultiTexSubImage1DEXT(fallback_glCompressedMultiTexSubImage1DEXT),
	glCompressedMultiTexSubImage2DEXT(fallback_glCompressedMultiTexSubImage2DEXT),
	glCompressedMultiTexSubImage3DEXT(fallback_glCompressedMultiTexSubImage3DEXT),
	glCompressedTexImage1D(fallback_glCompressedTexImage1D),
	glCompressedTexImage1DARB(fallback_glCompressedTexImage1DARB),
	glCompressedTexImage2D(fallback_glCompressedTexImage2D),
	glCompressedTexImage2DARB(fallback_glCompressedTexImage2DARB),
	glCompressedTexImage3D(fallback_glCompressedTexImage3D),
	glCompressedTexImage3DARB(fallback_glCompressedTexImage3DARB),
	glCompressedTexSubImage1D(fallback_glCompressedTexSubImage1D),
	glCompressedTexSubImage1DARB(fallback_glCompressedTexSubImage1DARB),
	glCompressedTexSubImage2D(fallback_glCompressedTexSubImage2D),
	glCompressedTexSubImage2DARB(fallback_glCompressedTexSubImage2DARB),
	glCompressedTexSubImage3D(fallback_glCompressedTexSubImage3D),
	glCompressedTexSubImage3DARB(fallback_glCompressedTexSubImage3DARB),
	glCompressedTextureImage1DEXT(fallback_glCompressedTextureImage1DEXT),
	glCompressedTextureImage2DEXT(fallback_glCompressedTextureImage2DEXT),
	glCompressedTextureImage3DEXT(fallback_glCompressedTextureImage3DEXT),
	glCompressedTextureSubImage1DEXT(fallback_glCompressedTextureSubImage1DEXT),
	glCompressedTextureSubImage2DEXT(fallback_glCompressedTextureSubImage2DEXT),
	glCompressedTextureSubImage3DEXT(fallback_glCompressedTextureSubImage3DEXT),
	glConvolutionFilter1D(fallback_glConvolutionFilter1D),
	glConvolutionFilter1DEXT(fallback_glConvolutionFilter1DEXT),
	glConvolutionFilter2D(fallback_glConvolutionFilter2D),
	glConvolutionFilter2DEXT(fallback_glConvolutionFilter2DEXT),
	glConvolutionParameterf(fallback_glConvolutionParameterf),
	glConvolutionParameterfEXT(fallback_glConvolutionParameterfEXT),
	glConvolutionParameterfv(fallback_glConvolutionParameterfv),
	glConvolutionParameterfvEXT(fallback_glConvolutionParameterfvEXT),
	glConvolutionParameteri(fallback_glConvolutionParameteri),
	glConvolutionParameteriEXT(fallback_glConvolutionParameteriEXT),
	glConvolutionParameteriv(fallback_glConvolutionParameteriv),
	glConvolutionParameterivEXT(fallback_glConvolutionParameterivEXT),
	glCopyBufferSubData(fallback_glCopyBufferSubData),
	glCopyColorSubTable(fallback_glCopyColorSubTable),
	glCopyColorSubTableEXT(fallback_glCopyColorSubTableEXT),
	glCopyColorTable(fallback_glCopyColorTable),
	glCopyColorTableSGI(fallback_glCopyColorTableSGI),
	glCopyConvolutionFilter1D(fallback_glCopyConvolutionFilter1D),
	glCopyConvolutionFilter1DEXT(fallback_glCopyConvolutionFilter1DEXT),
	glCopyConvolutionFilter2D(fallback_glCopyConvolutionFilter2D),
	glCopyConvolutionFilter2DEXT(fallback_glCopyConvolutionFilter2DEXT),
	glCopyImageSubDataNV(fallback_glCopyImageSubDataNV),
	glCopyMultiTexImage1DEXT(fallback_glCopyMultiTexImage1DEXT),
	glCopyMultiTexImage2DEXT(fallback_glCopyMultiTexImage2DEXT),
	glCopyMultiTexSubImage1DEXT(fallback_glCopyMultiTexSubImage1DEXT),
	glCopyMultiTexSubImage2DEXT(fallback_glCopyMultiTexSubImage2DEXT),
	glCopyMultiTexSubImage3DEXT(fallback_glCopyMultiTexSubImage3DEXT),
	glCopyPixels(fallback_glCopyPixels),
	glCopyTexImage1D(fallback_glCopyTexImage1D),
	glCopyTexImage1DEXT(fallback_glCopyTexImage1DEXT),
	glCopyTexImage2D(fallback_glCopyTexImage2D),
	glCopyTexImage2DEXT(fallback_glCopyTexImage2DEXT),
	glCopyTexSubImage1D(fallback_glCopyTexSubImage1D),
	glCopyTexSubImage1DEXT(fallback_glCopyTexSubImage1DEXT),
	glCopyTexSubImage2D(fallback_glCopyTexSubImage2D),
	glCopyTexSubImage2DEXT(fallback_glCopyTexSubImage2DEXT),
	glCopyTexSubImage3D(fallback_glCopyTexSubImage3D),
	glCopyTexSubImage3DEXT(fallback_glCopyTexSubImage3DEXT),
	glCopyTextureImage1DEXT(fallback_glCopyTextureImage1DEXT),
	glCopyTextureImage2DEXT(fallback_glCopyTextureImage2DEXT),
	glCopyTextureSubImage1DEXT(fallback_glCopyTextureSubImage1DEXT),
	glCopyTextureSubImage2DEXT(fallback_glCopyTextureSubImage2DEXT),
	glCopyTextureSubImage3DEXT(fallback_glCopyTextureSubImage3DEXT),
	glCreateProgram(fallback_glCreateProgram),
	glCreateProgramObjectARB(fallback_glCreateProgramObjectARB),
	glCreateShader(fallback_glCreateShader),
	glCreateShaderObjectARB(fallback_glCreateShaderObjectARB),
	glCreateShaderProgramEXT(fallback_glCreateShaderProgramEXT),
	glCullFace(fallback_glCullFace),
	glCullParameterdvEXT(fallback_glCullParameterdvEXT),
	glCullParameterfvEXT(fallback_glCullParameterfvEXT),
	glCurrentPaletteMatrixARB(fallback_glCurrentPaletteMatrixARB),
	glDeformSGIX(fallback_glDeformSGIX),
	glDeformationMap3dSGIX(fallback_glDeformationMap3dSGIX),
	glDeformationMap3fSGIX(fallback_glDeformationMap3fSGIX),
	glDeleteAsyncMarkersSGIX(fallback_glDeleteAsyncMarkersSGIX),
	glDeleteBuffers(fallback_glDeleteBuffers),
	glDeleteBuffersARB(fallback_glDeleteBuffersARB),
	glDeleteFencesAPPLE(fallback_glDeleteFencesAPPLE),
	glDeleteFencesNV(fallback_glDeleteFencesNV),
	glDeleteFragmentShaderATI(fallback_glDeleteFragmentShaderATI),
	glDeleteFramebuffers(fallback_glDeleteFramebuffers),
	glDeleteFramebuffersEXT(fallback_glDeleteFramebuffersEXT),
	glDeleteLists(fallback_glDeleteLists),
	glDeleteNamedStringARB(fallback_glDeleteNamedStringARB),
	glDeleteObjectARB(fallback_glDeleteObjectARB),
	glDeleteOcclusionQueriesNV(fallback_glDeleteOcclusionQueriesNV),
	glDeletePerfMonitorsAMD(fallback_glDeletePerfMonitorsAMD),
	glDeleteProgram(fallback_glDeleteProgram),
	glDeleteProgramsARB(fallback_glDeleteProgramsARB),
	glDeleteProgramsNV(fallback_glDeleteProgramsNV),
	glDeleteQueries(fallback_glDeleteQueries),
	glDeleteQueriesARB(fallback_glDeleteQueriesARB),
	glDeleteRenderbuffers(fallback_glDeleteRenderbuffers),
	glDeleteRenderbuffersEXT(fallback_glDeleteRenderbuffersEXT),
	glDeleteSamplers(fallback_glDeleteSamplers),
	glDeleteShader(fallback_glDeleteShader),
	glDeleteSync(fallback_glDeleteSync),
	glDeleteTextures(fallback_glDeleteTextures),
	glDeleteTexturesEXT(fallback_glDeleteTexturesEXT),
	glDeleteTransformFeedbacks(fallback_glDeleteTransformFeedbacks),
	glDeleteTransformFeedbacksNV(fallback_glDeleteTransformFeedbacksNV),
	glDeleteVertexArrays(fallback_glDeleteVertexArrays),
	glDeleteVertexArraysAPPLE(fallback_glDeleteVertexArraysAPPLE),
	glDeleteVertexShaderEXT(fallback_glDeleteVertexShaderEXT),
	glDepthBoundsEXT(fallback_glDepthBoundsEXT),
	glDepthBoundsdNV(fallback_glDepthBoundsdNV),
	glDepthFunc(fallback_glDepthFunc),
	glDepthMask(fallback_glDepthMask),
	glDepthRange(fallback_glDepthRange),
	glDepthRangedNV(fallback_glDepthRangedNV),
	glDetachObjectARB(fallback_glDetachObjectARB),
	glDetachShader(fallback_glDetachShader),
	glDetailTexFuncSGIS(fallback_glDetailTexFuncSGIS),
	glDisable(fallback_glDisable),
	glDisableClientState(fallback_glDisableClientState),
	glDisableClientStateIndexedEXT(fallback_glDisableClientStateIndexedEXT),
	glDisableIndexedEXT(fallback_glDisableIndexedEXT),
	glDisableVariantClientStateEXT(fallback_glDisableVariantClientStateEXT),
	glDisableVertexAttribAPPLE(fallback_glDisableVertexAttribAPPLE),
	glDisableVertexAttribArray(fallback_glDisableVertexAttribArray),
	glDisableVertexAttribArrayARB(fallback_glDisableVertexAttribArrayARB),
	glDisablei(fallback_glDisablei),
	glDrawArrays(fallback_glDrawArrays),
	glDrawArraysEXT(fallback_glDrawArraysEXT),
	glDrawArraysIndirect(fallback_glDrawArraysIndirect),
	glDrawArraysInstanced(fallback_glDrawArraysInstanced),
	glDrawArraysInstancedARB(fallback_glDrawArraysInstancedARB),
	glDrawArraysInstancedEXT(fallback_glDrawArraysInstancedEXT),
	glDrawBuffer(fallback_glDrawBuffer),
	glDrawBuffers(fallback_glDrawBuffers),
	glDrawBuffersARB(fallback_glDrawBuffersARB),
	glDrawBuffersATI(fallback_glDrawBuffersATI),
	glDrawElementArrayAPPLE(fallback_glDrawElementArrayAPPLE),
	glDrawElementArrayATI(fallback_glDrawElementArrayATI),
	glDrawElements(fallback_glDrawElements),
	glDrawElementsBaseVertex(fallback_glDrawElementsBaseVertex),
	glDrawElementsIndirect(fallback_glDrawElementsIndirect),
	glDrawElementsInstanced(fallback_glDrawElementsInstanced),
	glDrawElementsInstancedARB(fallback_glDrawElementsInstancedARB),
	glDrawElementsInstancedBaseVertex(fallback_glDrawElementsInstancedBaseVertex),
	glDrawElementsInstancedEXT(fallback_glDrawElementsInstancedEXT),
	glDrawMeshArraysSUN(fallback_glDrawMeshArraysSUN),
	glDrawPixels(fallback_glDrawPixels),
	glDrawRangeElementArrayAPPLE(fallback_glDrawRangeElementArrayAPPLE),
	glDrawRangeElementArrayATI(fallback_glDrawRangeElementArrayATI),
	glDrawRangeElements(fallback_glDrawRangeElements),
	glDrawRangeElementsBaseVertex(fallback_glDrawRangeElementsBaseVertex),
	glDrawRangeElementsEXT(fallback_glDrawRangeElementsEXT),
	glDrawTransformFeedback(fallback_glDrawTransformFeedback),
	glDrawTransformFeedbackNV(fallback_glDrawTransformFeedbackNV),
	glDrawTransformFeedbackStream(fallback_glDrawTransformFeedbackStream),
	glEdgeFlag(fallback_glEdgeFlag),
	glEdgeFlagFormatNV(fallback_glEdgeFlagFormatNV),
	glEdgeFlagPointer(fallback_glEdgeFlagPointer),
	glEdgeFlagPointerEXT(fallback_glEdgeFlagPointerEXT),
	glEdgeFlagPointerListIBM(fallback_glEdgeFlagPointerListIBM),
	glEdgeFlagv(fallback_glEdgeFlagv),
	glElementPointerAPPLE(fallback_glElementPointerAPPLE),
	glElementPointerATI(fallback_glElementPointerATI),
	glEnable(fallback_glEnable),
	glEnableClientState(fallback_glEnableClientState),
	glEnableClientStateIndexedEXT(fallback_glEnableClientStateIndexedEXT),
	glEnableIndexedEXT(fallback_glEnableIndexedEXT),
	glEnableVariantClientStateEXT(fallback_glEnableVariantClientStateEXT),
	glEnableVertexAttribAPPLE(fallback_glEnableVertexAttribAPPLE),
	glEnableVertexAttribArray(fallback_glEnableVertexAttribArray),
	glEnableVertexAttribArrayARB(fallback_glEnableVertexAttribArrayARB),
	glEnablei(fallback_glEnablei),
	glEnd(fallback_glEnd),
	glEndConditionalRender(fallback_glEndConditionalRender),
	glEndConditionalRenderNV(fallback_glEndConditionalRenderNV),
	glEndFragmentShaderATI(fallback_glEndFragmentShaderATI),
	glEndList(fallback_glEndList),
	glEndOcclusionQueryNV(fallback_glEndOcclusionQueryNV),
	glEndPerfMonitorAMD(fallback_glEndPerfMonitorAMD),
	glEndQuery(fallback_glEndQuery),
	glEndQueryARB(fallback_glEndQueryARB),
	glEndQueryIndexed(fallback_glEndQueryIndexed),
	glEndTransformFeedback(fallback_glEndTransformFeedback),
	glEndTransformFeedbackEXT(fallback_glEndTransformFeedbackEXT),
	glEndTransformFeedbackNV(fallback_glEndTransformFeedbackNV),
	glEndVertexShaderEXT(fallback_glEndVertexShaderEXT),
	glEndVideoCaptureNV(fallback_glEndVideoCaptureNV),
	glEvalCoord1d(fallback_glEvalCoord1d),
	glEvalCoord1dv(fallback_glEvalCoord1dv),
	glEvalCoord1f(fallback_glEvalCoord1f),
	glEvalCoord1fv(fallback_glEvalCoord1fv),
	glEvalCoord2d(fallback_glEvalCoord2d),
	glEvalCoord2dv(fallback_glEvalCoord2dv),
	glEvalCoord2f(fallback_glEvalCoord2f),
	glEvalCoord2fv(fallback_glEvalCoord2fv),
	glEvalMapsNV(fallback_glEvalMapsNV),
	glEvalMesh1(fallback_glEvalMesh1),
	glEvalMesh2(fallback_glEvalMesh2),
	glEvalPoint1(fallback_glEvalPoint1),
	glEvalPoint2(fallback_glEvalPoint2),
	glExecuteProgramNV(fallback_glExecuteProgramNV),
	glExtractComponentEXT(fallback_glExtractComponentEXT),
	glFeedbackBuffer(fallback_glFeedbackBuffer),
	glFenceSync(fallback_glFenceSync),
	glFinalCombinerInputNV(fallback_glFinalCombinerInputNV),
	glFinish(fallback_glFinish),
	glFinishAsyncSGIX(fallback_glFinishAsyncSGIX),
	glFinishFenceAPPLE(fallback_glFinishFenceAPPLE),
	glFinishFenceNV(fallback_glFinishFenceNV),
	glFinishObjectAPPLE(fallback_glFinishObjectAPPLE),
	glFinishTextureSUNX(fallback_glFinishTextureSUNX),
	glFlush(fallback_glFlush),
	glFlushMappedBufferRange(fallback_glFlushMappedBufferRange),
	glFlushMappedBufferRangeAPPLE(fallback_glFlushMappedBufferRangeAPPLE),
	glFlushPixelDataRangeNV(fallback_glFlushPixelDataRangeNV),
	glFlushRasterSGIX(fallback_glFlushRasterSGIX),
	glFlushVertexArrayRangeAPPLE(fallback_glFlushVertexArrayRangeAPPLE),
	glFlushVertexArrayRangeNV(fallback_glFlushVertexArrayRangeNV),
	glFogCoordFormatNV(fallback_glFogCoordFormatNV),
	glFogCoordPointer(fallback_glFogCoordPointer),
	glFogCoordPointerEXT(fallback_glFogCoordPointerEXT),
	glFogCoordPointerListIBM(fallback_glFogCoordPointerListIBM),
	glFogCoordd(fallback_glFogCoordd),
	glFogCoorddEXT(fallback_glFogCoorddEXT),
	glFogCoorddv(fallback_glFogCoorddv),
	glFogCoorddvEXT(fallback_glFogCoorddvEXT),
	glFogCoordf(fallback_glFogCoordf),
	glFogCoordfEXT(fallback_glFogCoordfEXT),
	glFogCoordfv(fallback_glFogCoordfv),
	glFogCoordfvEXT(fallback_glFogCoordfvEXT),
	glFogCoordhNV(fallback_glFogCoordhNV),
	glFogCoordhvNV(fallback_glFogCoordhvNV),
	glFogFuncSGIS(fallback_glFogFuncSGIS),
	glFogf(fallback_glFogf),
	glFogfv(fallback_glFogfv),
	glFogi(fallback_glFogi),
	glFogiv(fallback_glFogiv),
	glFragmentColorMaterialSGIX(fallback_glFragmentColorMaterialSGIX),
	glFragmentLightModelfSGIX(fallback_glFragmentLightModelfSGIX),
	glFragmentLightModelfvSGIX(fallback_glFragmentLightModelfvSGIX),
	glFragmentLightModeliSGIX(fallback_glFragmentLightModeliSGIX),
	glFragmentLightModelivSGIX(fallback_glFragmentLightModelivSGIX),
	glFragmentLightfSGIX(fallback_glFragmentLightfSGIX),
	glFragmentLightfvSGIX(fallback_glFragmentLightfvSGIX),
	glFragmentLightiSGIX(fallback_glFragmentLightiSGIX),
	glFragmentLightivSGIX(fallback_glFragmentLightivSGIX),
	glFragmentMaterialfSGIX(fallback_glFragmentMaterialfSGIX),
	glFragmentMaterialfvSGIX(fallback_glFragmentMaterialfvSGIX),
	glFragmentMaterialiSGIX(fallback_glFragmentMaterialiSGIX),
	glFragmentMaterialivSGIX(fallback_glFragmentMaterialivSGIX),
	glFrameTerminatorGREMEDY(fallback_glFrameTerminatorGREMEDY),
	glFrameZoomSGIX(fallback_glFrameZoomSGIX),
	glFramebufferDrawBufferEXT(fallback_glFramebufferDrawBufferEXT),
	glFramebufferDrawBuffersEXT(fallback_glFramebufferDrawBuffersEXT),
	glFramebufferReadBufferEXT(fallback_glFramebufferReadBufferEXT),
	glFramebufferRenderbuffer(fallback_glFramebufferRenderbuffer),
	glFramebufferRenderbufferEXT(fallback_glFramebufferRenderbufferEXT),
	glFramebufferTexture(fallback_glFramebufferTexture),
	glFramebufferTexture1D(fallback_glFramebufferTexture1D),
	glFramebufferTexture1DEXT(fallback_glFramebufferTexture1DEXT),
	glFramebufferTexture2D(fallback_glFramebufferTexture2D),
	glFramebufferTexture2DEXT(fallback_glFramebufferTexture2DEXT),
	glFramebufferTexture3D(fallback_glFramebufferTexture3D),
	glFramebufferTexture3DEXT(fallback_glFramebufferTexture3DEXT),
	glFramebufferTextureARB(fallback_glFramebufferTextureARB),
	glFramebufferTextureEXT(fallback_glFramebufferTextureEXT),
	glFramebufferTextureFaceARB(fallback_glFramebufferTextureFaceARB),
	glFramebufferTextureFaceEXT(fallback_glFramebufferTextureFaceEXT),
	glFramebufferTextureLayer(fallback_glFramebufferTextureLayer),
	glFramebufferTextureLayerARB(fallback_glFramebufferTextureLayerARB),
	glFramebufferTextureLayerEXT(fallback_glFramebufferTextureLayerEXT),
	glFreeObjectBufferATI(fallback_glFreeObjectBufferATI),
	glFrontFace(fallback_glFrontFace),
	glFrustum(fallback_glFrustum),
	glGenAsyncMarkersSGIX(fallback_glGenAsyncMarkersSGIX),
	glGenBuffers(fallback_glGenBuffers),
	glGenBuffersARB(fallback_glGenBuffersARB),
	glGenFencesAPPLE(fallback_glGenFencesAPPLE),
	glGenFencesNV(fallback_glGenFencesNV),
	glGenFragmentShadersATI(fallback_glGenFragmentShadersATI),
	glGenFramebuffers(fallback_glGenFramebuffers),
	glGenFramebuffersEXT(fallback_glGenFramebuffersEXT),
	glGenLists(fallback_glGenLists),
	glGenOcclusionQueriesNV(fallback_glGenOcclusionQueriesNV),
	glGenPerfMonitorsAMD(fallback_glGenPerfMonitorsAMD),
	glGenProgramsARB(fallback_glGenProgramsARB),
	glGenProgramsNV(fallback_glGenProgramsNV),
	glGenQueries(fallback_glGenQueries),
	glGenQueriesARB(fallback_glGenQueriesARB),
	glGenRenderbuffers(fallback_glGenRenderbuffers),
	glGenRenderbuffersEXT(fallback_glGenRenderbuffersEXT),
	glGenSamplers(fallback_glGenSamplers),
	glGenSymbolsEXT(fallback_glGenSymbolsEXT),
	glGenTextures(fallback_glGenTextures),
	glGenTexturesEXT(fallback_glGenTexturesEXT),
	glGenTransformFeedbacks(fallback_glGenTransformFeedbacks),
	glGenTransformFeedbacksNV(fallback_glGenTransformFeedbacksNV),
	glGenVertexArrays(fallback_glGenVertexArrays),
	glGenVertexArraysAPPLE(fallback_glGenVertexArraysAPPLE),
	glGenVertexShadersEXT(fallback_glGenVertexShadersEXT),
	glGenerateMipmap(fallback_glGenerateMipmap),
	glGenerateMipmapEXT(fallback_glGenerateMipmapEXT),
	glGenerateMultiTexMipmapEXT(fallback_glGenerateMultiTexMipmapEXT),
	glGenerateTextureMipmapEXT(fallback_glGenerateTextureMipmapEXT),
	glGetActiveAttrib(fallback_glGetActiveAttrib),
	glGetActiveAttribARB(fallback_glGetActiveAttribARB),
	glGetActiveSubroutineName(fallback_glGetActiveSubroutineName),
	glGetActiveSubroutineUniformName(fallback_glGetActiveSubroutineUniformName),
	glGetActiveSubroutineUniformiv(fallback_glGetActiveSubroutineUniformiv),
	glGetActiveUniform(fallback_glGetActiveUniform),
	glGetActiveUniformARB(fallback_glGetActiveUniformARB),
	glGetActiveUniformBlockName(fallback_glGetActiveUniformBlockName),
	glGetActiveUniformBlockiv(fallback_glGetActiveUniformBlockiv),
	glGetActiveUniformName(fallback_glGetActiveUniformName),
	glGetActiveUniformsiv(fallback_glGetActiveUniformsiv),
	glGetActiveVaryingNV(fallback_glGetActiveVaryingNV),
	glGetArrayObjectfvATI(fallback_glGetArrayObjectfvATI),
	glGetArrayObjectivATI(fallback_glGetArrayObjectivATI),
	glGetAttachedObjectsARB(fallback_glGetAttachedObjectsARB),
	glGetAttachedShaders(fallback_glGetAttachedShaders),
	glGetAttribLocation(fallback_glGetAttribLocation),
	glGetAttribLocationARB(fallback_glGetAttribLocationARB),
	glGetBooleanIndexedvEXT(fallback_glGetBooleanIndexedvEXT),
	glGetBooleani_v(fallback_glGetBooleani_v),
	glGetBooleanv(fallback_glGetBooleanv),
	glGetBufferParameteri64v(fallback_glGetBufferParameteri64v),
	glGetBufferParameteriv(fallback_glGetBufferParameteriv),
	glGetBufferParameterivARB(fallback_glGetBufferParameterivARB),
	glGetBufferParameterui64vNV(fallback_glGetBufferParameterui64vNV),
	glGetBufferPointerv(fallback_glGetBufferPointerv),
	glGetBufferPointervARB(fallback_glGetBufferPointervARB),
	glGetBufferSubData(fallback_glGetBufferSubData),
	glGetBufferSubDataARB(fallback_glGetBufferSubDataARB),
	glGetClipPlane(fallback_glGetClipPlane),
	glGetColorTable(fallback_glGetColorTable),
	glGetColorTableEXT(fallback_glGetColorTableEXT),
	glGetColorTableParameterfv(fallback_glGetColorTableParameterfv),
	glGetColorTableParameterfvEXT(fallback_glGetColorTableParameterfvEXT),
	glGetColorTableParameterfvSGI(fallback_glGetColorTableParameterfvSGI),
	glGetColorTableParameteriv(fallback_glGetColorTableParameteriv),
	glGetColorTableParameterivEXT(fallback_glGetColorTableParameterivEXT),
	glGetColorTableParameterivSGI(fallback_glGetColorTableParameterivSGI),
	glGetColorTableSGI(fallback_glGetColorTableSGI),
	glGetCombinerInputParameterfvNV(fallback_glGetCombinerInputParameterfvNV),
	glGetCombinerInputParameterivNV(fallback_glGetCombinerInputParameterivNV),
	glGetCombinerOutputParameterfvNV(fallback_glGetCombinerOutputParameterfvNV),
	glGetCombinerOutputParameterivNV(fallback_glGetCombinerOutputParameterivNV),
	glGetCombinerStageParameterfvNV(fallback_glGetCombinerStageParameterfvNV),
	glGetCompressedMultiTexImageEXT(fallback_glGetCompressedMultiTexImageEXT),
	glGetCompressedTexImage(fallback_glGetCompressedTexImage),
	glGetCompressedTexImageARB(fallback_glGetCompressedTexImageARB),
	glGetCompressedTextureImageEXT(fallback_glGetCompressedTextureImageEXT),
	glGetConvolutionFilter(fallback_glGetConvolutionFilter),
	glGetConvolutionFilterEXT(fallback_glGetConvolutionFilterEXT),
	glGetConvolutionParameterfv(fallback_glGetConvolutionParameterfv),
	glGetConvolutionParameterfvEXT(fallback_glGetConvolutionParameterfvEXT),
	glGetConvolutionParameteriv(fallback_glGetConvolutionParameteriv),
	glGetConvolutionParameterivEXT(fallback_glGetConvolutionParameterivEXT),
	glGetDetailTexFuncSGIS(fallback_glGetDetailTexFuncSGIS),
	glGetDoubleIndexedvEXT(fallback_glGetDoubleIndexedvEXT),
	glGetDoublev(fallback_glGetDoublev),
	glGetError(fallback_glGetError),
	glGetFenceivNV(fallback_glGetFenceivNV),
	glGetFinalCombinerInputParameterfvNV(fallback_glGetFinalCombinerInputParameterfvNV),
	glGetFinalCombinerInputParameterivNV(fallback_glGetFinalCombinerInputParameterivNV),
	glGetFloatIndexedvEXT(fallback_glGetFloatIndexedvEXT),
	glGetFloatv(fallback_glGetFloatv),
	glGetFogFuncSGIS(fallback_glGetFogFuncSGIS),
	glGetFragDataIndex(fallback_glGetFragDataIndex),
	glGetFragDataLocation(fallback_glGetFragDataLocation),
	glGetFragDataLocationEXT(fallback_glGetFragDataLocationEXT),
	glGetFragmentLightfvSGIX(fallback_glGetFragmentLightfvSGIX),
	glGetFragmentLightivSGIX(fallback_glGetFragmentLightivSGIX),
	glGetFragmentMaterialfvSGIX(fallback_glGetFragmentMaterialfvSGIX),
	glGetFragmentMaterialivSGIX(fallback_glGetFragmentMaterialivSGIX),
	glGetFramebufferAttachmentParameteriv(fallback_glGetFramebufferAttachmentParameteriv),
	glGetFramebufferAttachmentParameterivEXT(fallback_glGetFramebufferAttachmentParameterivEXT),
	glGetFramebufferParameterivEXT(fallback_glGetFramebufferParameterivEXT),
	glGetHandleARB(fallback_glGetHandleARB),
	glGetHistogram(fallback_glGetHistogram),
	glGetHistogramEXT(fallback_glGetHistogramEXT),
	glGetHistogramParameterfv(fallback_glGetHistogramParameterfv),
	glGetHistogramParameterfvEXT(fallback_glGetHistogramParameterfvEXT),
	glGetHistogramParameteriv(fallback_glGetHistogramParameteriv),
	glGetHistogramParameterivEXT(fallback_glGetHistogramParameterivEXT),
	glGetImageTransformParameterfvHP(fallback_glGetImageTransformParameterfvHP),
	glGetImageTransformParameterivHP(fallback_glGetImageTransformParameterivHP),
	glGetInfoLogARB(fallback_glGetInfoLogARB),
	glGetInstrumentsSGIX(fallback_glGetInstrumentsSGIX),
	glGetInteger64i_v(fallback_glGetInteger64i_v),
	glGetInteger64v(fallback_glGetInteger64v),
	glGetIntegerIndexedvEXT(fallback_glGetIntegerIndexedvEXT),
	glGetIntegeri_v(fallback_glGetIntegeri_v),
	glGetIntegerui64i_vNV(fallback_glGetIntegerui64i_vNV),
	glGetIntegerui64vNV(fallback_glGetIntegerui64vNV),
	glGetIntegerv(fallback_glGetIntegerv),
	glGetInvariantBooleanvEXT(fallback_glGetInvariantBooleanvEXT),
	glGetInvariantFloatvEXT(fallback_glGetInvariantFloatvEXT),
	glGetInvariantIntegervEXT(fallback_glGetInvariantIntegervEXT),
	glGetLightfv(fallback_glGetLightfv),
	glGetLightiv(fallback_glGetLightiv),
	glGetListParameterfvSGIX(fallback_glGetListParameterfvSGIX),
	glGetListParameterivSGIX(fallback_glGetListParameterivSGIX),
	glGetLocalConstantBooleanvEXT(fallback_glGetLocalConstantBooleanvEXT),
	glGetLocalConstantFloatvEXT(fallback_glGetLocalConstantFloatvEXT),
	glGetLocalConstantIntegervEXT(fallback_glGetLocalConstantIntegervEXT),
	glGetMapAttribParameterfvNV(fallback_glGetMapAttribParameterfvNV),
	glGetMapAttribParameterivNV(fallback_glGetMapAttribParameterivNV),
	glGetMapControlPointsNV(fallback_glGetMapControlPointsNV),
	glGetMapParameterfvNV(fallback_glGetMapParameterfvNV),
	glGetMapParameterivNV(fallback_glGetMapParameterivNV),
	glGetMapdv(fallback_glGetMapdv),
	glGetMapfv(fallback_glGetMapfv),
	glGetMapiv(fallback_glGetMapiv),
	glGetMaterialfv(fallback_glGetMaterialfv),
	glGetMaterialiv(fallback_glGetMaterialiv),
	glGetMinmax(fallback_glGetMinmax),
	glGetMinmaxEXT(fallback_glGetMinmaxEXT),
	glGetMinmaxParameterfv(fallback_glGetMinmaxParameterfv),
	glGetMinmaxParameterfvEXT(fallback_glGetMinmaxParameterfvEXT),
	glGetMinmaxParameteriv(fallback_glGetMinmaxParameteriv),
	glGetMinmaxParameterivEXT(fallback_glGetMinmaxParameterivEXT),
	glGetMultiTexEnvfvEXT(fallback_glGetMultiTexEnvfvEXT),
	glGetMultiTexEnvivEXT(fallback_glGetMultiTexEnvivEXT),
	glGetMultiTexGendvEXT(fallback_glGetMultiTexGendvEXT),
	glGetMultiTexGenfvEXT(fallback_glGetMultiTexGenfvEXT),
	glGetMultiTexGenivEXT(fallback_glGetMultiTexGenivEXT),
	glGetMultiTexImageEXT(fallback_glGetMultiTexImageEXT),
	glGetMultiTexLevelParameterfvEXT(fallback_glGetMultiTexLevelParameterfvEXT),
	glGetMultiTexLevelParameterivEXT(fallback_glGetMultiTexLevelParameterivEXT),
	glGetMultiTexParameterIivEXT(fallback_glGetMultiTexParameterIivEXT),
	glGetMultiTexParameterIuivEXT(fallback_glGetMultiTexParameterIuivEXT),
	glGetMultiTexParameterfvEXT(fallback_glGetMultiTexParameterfvEXT),
	glGetMultiTexParameterivEXT(fallback_glGetMultiTexParameterivEXT),
	glGetMultisamplefv(fallback_glGetMultisamplefv),
	glGetMultisamplefvNV(fallback_glGetMultisamplefvNV),
	glGetNamedBufferParameterivEXT(fallback_glGetNamedBufferParameterivEXT),
	glGetNamedBufferParameterui64vNV(fallback_glGetNamedBufferParameterui64vNV),
	glGetNamedBufferPointervEXT(fallback_glGetNamedBufferPointervEXT),
	glGetNamedBufferSubDataEXT(fallback_glGetNamedBufferSubDataEXT),
	glGetNamedFramebufferAttachmentParameterivEXT(fallback_glGetNamedFramebufferAttachmentParameterivEXT),
	glGetNamedProgramLocalParameterIivEXT(fallback_glGetNamedProgramLocalParameterIivEXT),
	glGetNamedProgramLocalParameterIuivEXT(fallback_glGetNamedProgramLocalParameterIuivEXT),
	glGetNamedProgramLocalParameterdvEXT(fallback_glGetNamedProgramLocalParameterdvEXT),
	glGetNamedProgramLocalParameterfvEXT(fallback_glGetNamedProgramLocalParameterfvEXT),
	glGetNamedProgramStringEXT(fallback_glGetNamedProgramStringEXT),
	glGetNamedProgramivEXT(fallback_glGetNamedProgramivEXT),
	glGetNamedRenderbufferParameterivEXT(fallback_glGetNamedRenderbufferParameterivEXT),
	glGetNamedStringARB(fallback_glGetNamedStringARB),
	glGetNamedStringivARB(fallback_glGetNamedStringivARB),
	glGetObjectBufferfvATI(fallback_glGetObjectBufferfvATI),
	glGetObjectBufferivATI(fallback_glGetObjectBufferivATI),
	glGetObjectParameterfvARB(fallback_glGetObjectParameterfvARB),
	glGetObjectParameterivAPPLE(fallback_glGetObjectParameterivAPPLE),
	glGetObjectParameterivARB(fallback_glGetObjectParameterivARB),
	glGetOcclusionQueryivNV(fallback_glGetOcclusionQueryivNV),
	glGetOcclusionQueryuivNV(fallback_glGetOcclusionQueryuivNV),
	glGetPerfMonitorCounterDataAMD(fallback_glGetPerfMonitorCounterDataAMD),
	glGetPerfMonitorCounterInfoAMD(fallback_glGetPerfMonitorCounterInfoAMD),
	glGetPerfMonitorCounterStringAMD(fallback_glGetPerfMonitorCounterStringAMD),
	glGetPerfMonitorCountersAMD(fallback_glGetPerfMonitorCountersAMD),
	glGetPerfMonitorGroupStringAMD(fallback_glGetPerfMonitorGroupStringAMD),
	glGetPerfMonitorGroupsAMD(fallback_glGetPerfMonitorGroupsAMD),
	glGetPixelMapfv(fallback_glGetPixelMapfv),
	glGetPixelMapuiv(fallback_glGetPixelMapuiv),
	glGetPixelMapusv(fallback_glGetPixelMapusv),
	glGetPixelTexGenParameterfvSGIS(fallback_glGetPixelTexGenParameterfvSGIS),
	glGetPixelTexGenParameterivSGIS(fallback_glGetPixelTexGenParameterivSGIS),
	glGetPointerIndexedvEXT(fallback_glGetPointerIndexedvEXT),
	glGetPointerv(fallback_glGetPointerv),
	glGetPointervEXT(fallback_glGetPointervEXT),
	glGetPolygonStipple(fallback_glGetPolygonStipple),
	glGetProgramEnvParameterIivNV(fallback_glGetProgramEnvParameterIivNV),
	glGetProgramEnvParameterIuivNV(fallback_glGetProgramEnvParameterIuivNV),
	glGetProgramEnvParameterdvARB(fallback_glGetProgramEnvParameterdvARB),
	glGetProgramEnvParameterfvARB(fallback_glGetProgramEnvParameterfvARB),
	glGetProgramInfoLog(fallback_glGetProgramInfoLog),
	glGetProgramLocalParameterIivNV(fallback_glGetProgramLocalParameterIivNV),
	glGetProgramLocalParameterIuivNV(fallback_glGetProgramLocalParameterIuivNV),
	glGetProgramLocalParameterdvARB(fallback_glGetProgramLocalParameterdvARB),
	glGetProgramLocalParameterfvARB(fallback_glGetProgramLocalParameterfvARB),
	glGetProgramNamedParameterdvNV(fallback_glGetProgramNamedParameterdvNV),
	glGetProgramNamedParameterfvNV(fallback_glGetProgramNamedParameterfvNV),
	glGetProgramParameterdvNV(fallback_glGetProgramParameterdvNV),
	glGetProgramParameterfvNV(fallback_glGetProgramParameterfvNV),
	glGetProgramStageiv(fallback_glGetProgramStageiv),
	glGetProgramStringARB(fallback_glGetProgramStringARB),
	glGetProgramStringNV(fallback_glGetProgramStringNV),
	glGetProgramiv(fallback_glGetProgramiv),
	glGetProgramivARB(fallback_glGetProgramivARB),
	glGetProgramivNV(fallback_glGetProgramivNV),
	glGetQueryIndexediv(fallback_glGetQueryIndexediv),
	glGetQueryObjecti64v(fallback_glGetQueryObjecti64v),
	glGetQueryObjecti64vEXT(fallback_glGetQueryObjecti64vEXT),
	glGetQueryObjectiv(fallback_glGetQueryObjectiv),
	glGetQueryObjectivARB(fallback_glGetQueryObjectivARB),
	glGetQueryObjectui64v(fallback_glGetQueryObjectui64v),
	glGetQueryObjectui64vEXT(fallback_glGetQueryObjectui64vEXT),
	glGetQueryObjectuiv(fallback_glGetQueryObjectuiv),
	glGetQueryObjectuivARB(fallback_glGetQueryObjectuivARB),
	glGetQueryiv(fallback_glGetQueryiv),
	glGetQueryivARB(fallback_glGetQueryivARB),
	glGetRenderbufferParameteriv(fallback_glGetRenderbufferParameteriv),
	glGetRenderbufferParameterivEXT(fallback_glGetRenderbufferParameterivEXT),
	glGetSamplerParameterIfv(fallback_glGetSamplerParameterIfv),
	glGetSamplerParameterIiv(fallback_glGetSamplerParameterIiv),
	glGetSamplerParameterfv(fallback_glGetSamplerParameterfv),
	glGetSamplerParameteriv(fallback_glGetSamplerParameteriv),
	glGetSeparableFilter(fallback_glGetSeparableFilter),
	glGetSeparableFilterEXT(fallback_glGetSeparableFilterEXT),
	glGetShaderInfoLog(fallback_glGetShaderInfoLog),
	glGetShaderSource(fallback_glGetShaderSource),
	glGetShaderSourceARB(fallback_glGetShaderSourceARB),
	glGetShaderiv(fallback_glGetShaderiv),
	glGetSharpenTexFuncSGIS(fallback_glGetSharpenTexFuncSGIS),
	glGetString(fallback_glGetString),
	glGetStringi(fallback_glGetStringi),
	glGetSubroutineIndex(fallback_glGetSubroutineIndex),
	glGetSubroutineUniformLocation(fallback_glGetSubroutineUniformLocation),
	glGetSynciv(fallback_glGetSynciv),
	glGetTexBumpParameterfvATI(fallback_glGetTexBumpParameterfvATI),
	glGetTexBumpParameterivATI(fallback_glGetTexBumpParameterivATI),
	glGetTexEnvfv(fallback_glGetTexEnvfv),
	glGetTexEnviv(fallback_glGetTexEnviv),
	glGetTexFilterFuncSGIS(fallback_glGetTexFilterFuncSGIS),
	glGetTexGendv(fallback_glGetTexGendv),
	glGetTexGenfv(fallback_glGetTexGenfv),
	glGetTexGeniv(fallback_glGetTexGeniv),
	glGetTexImage(fallback_glGetTexImage),
	glGetTexLevelParameterfv(fallback_glGetTexLevelParameterfv),
	glGetTexLevelParameteriv(fallback_glGetTexLevelParameteriv),
	glGetTexParameterIiv(fallback_glGetTexParameterIiv),
	glGetTexParameterIivEXT(fallback_glGetTexParameterIivEXT),
	glGetTexParameterIuiv(fallback_glGetTexParameterIuiv),
	glGetTexParameterIuivEXT(fallback_glGetTexParameterIuivEXT),
	glGetTexParameterPointervAPPLE(fallback_glGetTexParameterPointervAPPLE),
	glGetTexParameterfv(fallback_glGetTexParameterfv),
	glGetTexParameteriv(fallback_glGetTexParameteriv),
	glGetTextureImageEXT(fallback_glGetTextureImageEXT),
	glGetTextureLevelParameterfvEXT(fallback_glGetTextureLevelParameterfvEXT),
	glGetTextureLevelParameterivEXT(fallback_glGetTextureLevelParameterivEXT),
	glGetTextureParameterIivEXT(fallback_glGetTextureParameterIivEXT),
	glGetTextureParameterIuivEXT(fallback_glGetTextureParameterIuivEXT),
	glGetTextureParameterfvEXT(fallback_glGetTextureParameterfvEXT),
	glGetTextureParameterivEXT(fallback_glGetTextureParameterivEXT),
	glGetTrackMatrixivNV(fallback_glGetTrackMatrixivNV),
	glGetTransformFeedbackVarying(fallback_glGetTransformFeedbackVarying),
	glGetTransformFeedbackVaryingEXT(fallback_glGetTransformFeedbackVaryingEXT),
	glGetTransformFeedbackVaryingNV(fallback_glGetTransformFeedbackVaryingNV),
	glGetUniformBlockIndex(fallback_glGetUniformBlockIndex),
	glGetUniformBufferSizeEXT(fallback_glGetUniformBufferSizeEXT),
	glGetUniformIndices(fallback_glGetUniformIndices),
	glGetUniformLocation(fallback_glGetUniformLocation),
	glGetUniformLocationARB(fallback_glGetUniformLocationARB),
	glGetUniformOffsetEXT(fallback_glGetUniformOffsetEXT),
	glGetUniformSubroutineuiv(fallback_glGetUniformSubroutineuiv),
	glGetUniformdv(fallback_glGetUniformdv),
	glGetUniformfv(fallback_glGetUniformfv),
	glGetUniformfvARB(fallback_glGetUniformfvARB),
	glGetUniformiv(fallback_glGetUniformiv),
	glGetUniformivARB(fallback_glGetUniformivARB),
	glGetUniformui64vNV(fallback_glGetUniformui64vNV),
	glGetUniformuiv(fallback_glGetUniformuiv),
	glGetUniformuivEXT(fallback_glGetUniformuivEXT),
	glGetVariantArrayObjectfvATI(fallback_glGetVariantArrayObjectfvATI),
	glGetVariantArrayObjectivATI(fallback_glGetVariantArrayObjectivATI),
	glGetVariantBooleanvEXT(fallback_glGetVariantBooleanvEXT),
	glGetVariantFloatvEXT(fallback_glGetVariantFloatvEXT),
	glGetVariantIntegervEXT(fallback_glGetVariantIntegervEXT),
	glGetVariantPointervEXT(fallback_glGetVariantPointervEXT),
	glGetVaryingLocationNV(fallback_glGetVaryingLocationNV),
	glGetVertexAttribArrayObjectfvATI(fallback_glGetVertexAttribArrayObjectfvATI),
	glGetVertexAttribArrayObjectivATI(fallback_glGetVertexAttribArrayObjectivATI),
	glGetVertexAttribIiv(fallback_glGetVertexAttribIiv),
	glGetVertexAttribIivEXT(fallback_glGetVertexAttribIivEXT),
	glGetVertexAttribIuiv(fallback_glGetVertexAttribIuiv),
	glGetVertexAttribIuivEXT(fallback_glGetVertexAttribIuivEXT),
	glGetVertexAttribPointerv(fallback_glGetVertexAttribPointerv),
	glGetVertexAttribPointervARB(fallback_glGetVertexAttribPointervARB),
	glGetVertexAttribPointervNV(fallback_glGetVertexAttribPointervNV),
	glGetVertexAttribdv(fallback_glGetVertexAttribdv),
	glGetVertexAttribdvARB(fallback_glGetVertexAttribdvARB),
	glGetVertexAttribdvNV(fallback_glGetVertexAttribdvNV),
	glGetVertexAttribfv(fallback_glGetVertexAttribfv),
	glGetVertexAttribfvARB(fallback_glGetVertexAttribfvARB),
	glGetVertexAttribfvNV(fallback_glGetVertexAttribfvNV),
	glGetVertexAttribiv(fallback_glGetVertexAttribiv),
	glGetVertexAttribivARB(fallback_glGetVertexAttribivARB),
	glGetVertexAttribivNV(fallback_glGetVertexAttribivNV),
	glGetVideoCaptureStreamdvNV(fallback_glGetVideoCaptureStreamdvNV),
	glGetVideoCaptureStreamfvNV(fallback_glGetVideoCaptureStreamfvNV),
	glGetVideoCaptureStreamivNV(fallback_glGetVideoCaptureStreamivNV),
	glGetVideoCaptureivNV(fallback_glGetVideoCaptureivNV),
	glGetVideoi64vNV(fallback_glGetVideoi64vNV),
	glGetVideoivNV(fallback_glGetVideoivNV),
	glGetVideoui64vNV(fallback_glGetVideoui64vNV),
	glGetVideouivNV(fallback_glGetVideouivNV),
	glGlobalAlphaFactorbSUN(fallback_glGlobalAlphaFactorbSUN),
	glGlobalAlphaFactordSUN(fallback_glGlobalAlphaFactordSUN),
	glGlobalAlphaFactorfSUN(fallback_glGlobalAlphaFactorfSUN),
	glGlobalAlphaFactoriSUN(fallback_glGlobalAlphaFactoriSUN),
	glGlobalAlphaFactorsSUN(fallback_glGlobalAlphaFactorsSUN),
	glGlobalAlphaFactorubSUN(fallback_glGlobalAlphaFactorubSUN),
	glGlobalAlphaFactoruiSUN(fallback_glGlobalAlphaFactoruiSUN),
	glGlobalAlphaFactorusSUN(fallback_glGlobalAlphaFactorusSUN),
	glHint(fallback_glHint),
	glHintPGI(fallback_glHintPGI),
	glHistogram(fallback_glHistogram),
	glHistogramEXT(fallback_glHistogramEXT),
	glIglooInterfaceSGIX(fallback_glIglooInterfaceSGIX),
	glImageTransformParameterfHP(fallback_glImageTransformParameterfHP),
	glImageTransformParameterfvHP(fallback_glImageTransformParameterfvHP),
	glImageTransformParameteriHP(fallback_glImageTransformParameteriHP),
	glImageTransformParameterivHP(fallback_glImageTransformParameterivHP),
	glIndexFormatNV(fallback_glIndexFormatNV),
	glIndexFuncEXT(fallback_glIndexFuncEXT),
	glIndexMask(fallback_glIndexMask),
	glIndexMaterialEXT(fallback_glIndexMaterialEXT),
	glIndexPointer(fallback_glIndexPointer),
	glIndexPointerEXT(fallback_glIndexPointerEXT),
	glIndexPointerListIBM(fallback_glIndexPointerListIBM),
	glIndexd(fallback_glIndexd),
	glIndexdv(fallback_glIndexdv),
	glIndexf(fallback_glIndexf),
	glIndexfv(fallback_glIndexfv),
	glIndexi(fallback_glIndexi),
	glIndexiv(fallback_glIndexiv),
	glIndexs(fallback_glIndexs),
	glIndexsv(fallback_glIndexsv),
	glIndexub(fallback_glIndexub),
	glIndexubv(fallback_glIndexubv),
	glInitNames(fallback_glInitNames),
	glInsertComponentEXT(fallback_glInsertComponentEXT),
	glInstrumentsBufferSGIX(fallback_glInstrumentsBufferSGIX),
	glInterleavedArrays(fallback_glInterleavedArrays),
	glIsAsyncMarkerSGIX(fallback_glIsAsyncMarkerSGIX),
	glIsBuffer(fallback_glIsBuffer),
	glIsBufferARB(fallback_glIsBufferARB),
	glIsBufferResidentNV(fallback_glIsBufferResidentNV),
	glIsEnabled(fallback_glIsEnabled),
	glIsEnabledIndexedEXT(fallback_glIsEnabledIndexedEXT),
	glIsEnabledi(fallback_glIsEnabledi),
	glIsFenceAPPLE(fallback_glIsFenceAPPLE),
	glIsFenceNV(fallback_glIsFenceNV),
	glIsFramebuffer(fallback_glIsFramebuffer),
	glIsFramebufferEXT(fallback_glIsFramebufferEXT),
	glIsList(fallback_glIsList),
	glIsNamedBufferResidentNV(fallback_glIsNamedBufferResidentNV),
	glIsNamedStringARB(fallback_glIsNamedStringARB),
	glIsObjectBufferATI(fallback_glIsObjectBufferATI),
	glIsOcclusionQueryNV(fallback_glIsOcclusionQueryNV),
	glIsProgram(fallback_glIsProgram),
	glIsProgramARB(fallback_glIsProgramARB),
	glIsProgramNV(fallback_glIsProgramNV),
	glIsQuery(fallback_glIsQuery),
	glIsQueryARB(fallback_glIsQueryARB),
	glIsRenderbuffer(fallback_glIsRenderbuffer),
	glIsRenderbufferEXT(fallback_glIsRenderbufferEXT),
	glIsSampler(fallback_glIsSampler),
	glIsShader(fallback_glIsShader),
	glIsSync(fallback_glIsSync),
	glIsTexture(fallback_glIsTexture),
	glIsTextureEXT(fallback_glIsTextureEXT),
	glIsTransformFeedback(fallback_glIsTransformFeedback),
	glIsTransformFeedbackNV(fallback_glIsTransformFeedbackNV),
	glIsVariantEnabledEXT(fallback_glIsVariantEnabledEXT),
	glIsVertexArray(fallback_glIsVertexArray),
	glIsVertexArrayAPPLE(fallback_glIsVertexArrayAPPLE),
	glIsVertexAttribEnabledAPPLE(fallback_glIsVertexAttribEnabledAPPLE),
	glLightEnviSGIX(fallback_glLightEnviSGIX),
	glLightModelf(fallback_glLightModelf),
	glLightModelfv(fallback_glLightModelfv),
	glLightModeli(fallback_glLightModeli),
	glLightModeliv(fallback_glLightModeliv),
	glLightf(fallback_glLightf),
	glLightfv(fallback_glLightfv),
	glLighti(fallback_glLighti),
	glLightiv(fallback_glLightiv),
	glLineStipple(fallback_glLineStipple),
	glLineWidth(fallback_glLineWidth),
	glLinkProgram(fallback_glLinkProgram),
	glLinkProgramARB(fallback_glLinkProgramARB),
	glListBase(fallback_glListBase),
	glListParameterfSGIX(fallback_glListParameterfSGIX),
	glListParameterfvSGIX(fallback_glListParameterfvSGIX),
	glListParameteriSGIX(fallback_glListParameteriSGIX),
	glListParameterivSGIX(fallback_glListParameterivSGIX),
	glLoadIdentity(fallback_glLoadIdentity),
	glLoadIdentityDeformationMapSGIX(fallback_glLoadIdentityDeformationMapSGIX),
	glLoadMatrixd(fallback_glLoadMatrixd),
	glLoadMatrixf(fallback_glLoadMatrixf),
	glLoadName(fallback_glLoadName),
	glLoadProgramNV(fallback_glLoadProgramNV),
	glLoadTransposeMatrixd(fallback_glLoadTransposeMatrixd),
	glLoadTransposeMatrixdARB(fallback_glLoadTransposeMatrixdARB),
	glLoadTransposeMatrixf(fallback_glLoadTransposeMatrixf),
	glLoadTransposeMatrixfARB(fallback_glLoadTransposeMatrixfARB),
	glLockArraysEXT(fallback_glLockArraysEXT),
	glLogicOp(fallback_glLogicOp),
	glMakeBufferNonResidentNV(fallback_glMakeBufferNonResidentNV),
	glMakeBufferResidentNV(fallback_glMakeBufferResidentNV),
	glMakeNamedBufferNonResidentNV(fallback_glMakeNamedBufferNonResidentNV),
	glMakeNamedBufferResidentNV(fallback_glMakeNamedBufferResidentNV),
	glMap1d(fallback_glMap1d),
	glMap1f(fallback_glMap1f),
	glMap2d(fallback_glMap2d),
	glMap2f(fallback_glMap2f),
	glMapBuffer(fallback_glMapBuffer),
	glMapBufferARB(fallback_glMapBufferARB),
	glMapBufferRange(fallback_glMapBufferRange),
	glMapControlPointsNV(fallback_glMapControlPointsNV),
	glMapGrid1d(fallback_glMapGrid1d),
	glMapGrid1f(fallback_glMapGrid1f),
	glMapGrid2d(fallback_glMapGrid2d),
	glMapGrid2f(fallback_glMapGrid2f),
	glMapNamedBufferEXT(fallback_glMapNamedBufferEXT),
	glMapObjectBufferATI(fallback_glMapObjectBufferATI),
	glMapParameterfvNV(fallback_glMapParameterfvNV),
	glMapParameterivNV(fallback_glMapParameterivNV),
	glMapVertexAttrib1dAPPLE(fallback_glMapVertexAttrib1dAPPLE),
	glMapVertexAttrib1fAPPLE(fallback_glMapVertexAttrib1fAPPLE),
	glMapVertexAttrib2dAPPLE(fallback_glMapVertexAttrib2dAPPLE),
	glMapVertexAttrib2fAPPLE(fallback_glMapVertexAttrib2fAPPLE),
	glMaterialf(fallback_glMaterialf),
	glMaterialfv(fallback_glMaterialfv),
	glMateriali(fallback_glMateriali),
	glMaterialiv(fallback_glMaterialiv),
	glMatrixFrustumEXT(fallback_glMatrixFrustumEXT),
	glMatrixIndexPointerARB(fallback_glMatrixIndexPointerARB),
	glMatrixIndexubvARB(fallback_glMatrixIndexubvARB),
	glMatrixIndexuivARB(fallback_glMatrixIndexuivARB),
	glMatrixIndexusvARB(fallback_glMatrixIndexusvARB),
	glMatrixLoadIdentityEXT(fallback_glMatrixLoadIdentityEXT),
	glMatrixLoadTransposedEXT(fallback_glMatrixLoadTransposedEXT),
	glMatrixLoadTransposefEXT(fallback_glMatrixLoadTransposefEXT),
	glMatrixLoaddEXT(fallback_glMatrixLoaddEXT),
	glMatrixLoadfEXT(fallback_glMatrixLoadfEXT),
	glMatrixMode(fallback_glMatrixMode),
	glMatrixMultTransposedEXT(fallback_glMatrixMultTransposedEXT),
	glMatrixMultTransposefEXT(fallback_glMatrixMultTransposefEXT),
	glMatrixMultdEXT(fallback_glMatrixMultdEXT),
	glMatrixMultfEXT(fallback_glMatrixMultfEXT),
	glMatrixOrthoEXT(fallback_glMatrixOrthoEXT),
	glMatrixPopEXT(fallback_glMatrixPopEXT),
	glMatrixPushEXT(fallback_glMatrixPushEXT),
	glMatrixRotatedEXT(fallback_glMatrixRotatedEXT),
	glMatrixRotatefEXT(fallback_glMatrixRotatefEXT),
	glMatrixScaledEXT(fallback_glMatrixScaledEXT),
	glMatrixScalefEXT(fallback_glMatrixScalefEXT),
	glMatrixTranslatedEXT(fallback_glMatrixTranslatedEXT),
	glMatrixTranslatefEXT(fallback_glMatrixTranslatefEXT),
	glMinSampleShading(fallback_glMinSampleShading),
	glMinmax(fallback_glMinmax),
	glMinmaxEXT(fallback_glMinmaxEXT),
	glMultMatrixd(fallback_glMultMatrixd),
	glMultMatrixf(fallback_glMultMatrixf),
	glMultTransposeMatrixd(fallback_glMultTransposeMatrixd),
	glMultTransposeMatrixdARB(fallback_glMultTransposeMatrixdARB),
	glMultTransposeMatrixf(fallback_glMultTransposeMatrixf),
	glMultTransposeMatrixfARB(fallback_glMultTransposeMatrixfARB),
	glMultiDrawArrays(fallback_glMultiDrawArrays),
	glMultiDrawArraysEXT(fallback_glMultiDrawArraysEXT),
	glMultiDrawElementArrayAPPLE(fallback_glMultiDrawElementArrayAPPLE),
	glMultiDrawElements(fallback_glMultiDrawElements),
	glMultiDrawElementsBaseVertex(fallback_glMultiDrawElementsBaseVertex),
	glMultiDrawElementsEXT(fallback_glMultiDrawElementsEXT),
	glMultiDrawRangeElementArrayAPPLE(fallback_glMultiDrawRangeElementArrayAPPLE),
	glMultiModeDrawArraysIBM(fallback_glMultiModeDrawArraysIBM),
	glMultiModeDrawElementsIBM(fallback_glMultiModeDrawElementsIBM),
	glMultiTexBufferEXT(fallback_glMultiTexBufferEXT),
	glMultiTexCoord1d(fallback_glMultiTexCoord1d),
	glMultiTexCoord1dARB(fallback_glMultiTexCoord1dARB),
	glMultiTexCoord1dv(fallback_glMultiTexCoord1dv),
	glMultiTexCoord1dvARB(fallback_glMultiTexCoord1dvARB),
	glMultiTexCoord1f(fallback_glMultiTexCoord1f),
	glMultiTexCoord1fARB(fallback_glMultiTexCoord1fARB),
	glMultiTexCoord1fv(fallback_glMultiTexCoord1fv),
	glMultiTexCoord1fvARB(fallback_glMultiTexCoord1fvARB),
	glMultiTexCoord1hNV(fallback_glMultiTexCoord1hNV),
	glMultiTexCoord1hvNV(fallback_glMultiTexCoord1hvNV),
	glMultiTexCoord1i(fallback_glMultiTexCoord1i),
	glMultiTexCoord1iARB(fallback_glMultiTexCoord1iARB),
	glMultiTexCoord1iv(fallback_glMultiTexCoord1iv),
	glMultiTexCoord1ivARB(fallback_glMultiTexCoord1ivARB),
	glMultiTexCoord1s(fallback_glMultiTexCoord1s),
	glMultiTexCoord1sARB(fallback_glMultiTexCoord1sARB),
	glMultiTexCoord1sv(fallback_glMultiTexCoord1sv),
	glMultiTexCoord1svARB(fallback_glMultiTexCoord1svARB),
	glMultiTexCoord2d(fallback_glMultiTexCoord2d),
	glMultiTexCoord2dARB(fallback_glMultiTexCoord2dARB),
	glMultiTexCoord2dv(fallback_glMultiTexCoord2dv),
	glMultiTexCoord2dvARB(fallback_glMultiTexCoord2dvARB),
	glMultiTexCoord2f(fallback_glMultiTexCoord2f),
	glMultiTexCoord2fARB(fallback_glMultiTexCoord2fARB),
	glMultiTexCoord2fv(fallback_glMultiTexCoord2fv),
	glMultiTexCoord2fvARB(fallback_glMultiTexCoord2fvARB),
	glMultiTexCoord2hNV(fallback_glMultiTexCoord2hNV),
	glMultiTexCoord2hvNV(fallback_glMultiTexCoord2hvNV),
	glMultiTexCoord2i(fallback_glMultiTexCoord2i),
	glMultiTexCoord2iARB(fallback_glMultiTexCoord2iARB),
	glMultiTexCoord2iv(fallback_glMultiTexCoord2iv),
	glMultiTexCoord2ivARB(fallback_glMultiTexCoord2ivARB),
	glMultiTexCoord2s(fallback_glMultiTexCoord2s),
	glMultiTexCoord2sARB(fallback_glMultiTexCoord2sARB),
	glMultiTexCoord2sv(fallback_glMultiTexCoord2sv),
	glMultiTexCoord2svARB(fallback_glMultiTexCoord2svARB),
	glMultiTexCoord3d(fallback_glMultiTexCoord3d),
	glMultiTexCoord3dARB(fallback_glMultiTexCoord3dARB),
	glMultiTexCoord3dv(fallback_glMultiTexCoord3dv),
	glMultiTexCoord3dvARB(fallback_glMultiTexCoord3dvARB),
	glMultiTexCoord3f(fallback_glMultiTexCoord3f),
	glMultiTexCoord3fARB(fallback_glMultiTexCoord3fARB),
	glMultiTexCoord3fv(fallback_glMultiTexCoord3fv),
	glMultiTexCoord3fvARB(fallback_glMultiTexCoord3fvARB),
	glMultiTexCoord3hNV(fallback_glMultiTexCoord3hNV),
	glMultiTexCoord3hvNV(fallback_glMultiTexCoord3hvNV),
	glMultiTexCoord3i(fallback_glMultiTexCoord3i),
	glMultiTexCoord3iARB(fallback_glMultiTexCoord3iARB),
	glMultiTexCoord3iv(fallback_glMultiTexCoord3iv),
	glMultiTexCoord3ivARB(fallback_glMultiTexCoord3ivARB),
	glMultiTexCoord3s(fallback_glMultiTexCoord3s),
	glMultiTexCoord3sARB(fallback_glMultiTexCoord3sARB),
	glMultiTexCoord3sv(fallback_glMultiTexCoord3sv),
	glMultiTexCoord3svARB(fallback_glMultiTexCoord3svARB),
	glMultiTexCoord4d(fallback_glMultiTexCoord4d),
	glMultiTexCoord4dARB(fallback_glMultiTexCoord4dARB),
	glMultiTexCoord4dv(fallback_glMultiTexCoord4dv),
	glMultiTexCoord4dvARB(fallback_glMultiTexCoord4dvARB),
	glMultiTexCoord4f(fallback_glMultiTexCoord4f),
	glMultiTexCoord4fARB(fallback_glMultiTexCoord4fARB),
	glMultiTexCoord4fv(fallback_glMultiTexCoord4fv),
	glMultiTexCoord4fvARB(fallback_glMultiTexCoord4fvARB),
	glMultiTexCoord4hNV(fallback_glMultiTexCoord4hNV),
	glMultiTexCoord4hvNV(fallback_glMultiTexCoord4hvNV),
	glMultiTexCoord4i(fallback_glMultiTexCoord4i),
	glMultiTexCoord4iARB(fallback_glMultiTexCoord4iARB),
	glMultiTexCoord4iv(fallback_glMultiTexCoord4iv),
	glMultiTexCoord4ivARB(fallback_glMultiTexCoord4ivARB),
	glMultiTexCoord4s(fallback_glMultiTexCoord4s),
	glMultiTexCoord4sARB(fallback_glMultiTexCoord4sARB),
	glMultiTexCoord4sv(fallback_glMultiTexCoord4sv),
	glMultiTexCoord4svARB(fallback_glMultiTexCoord4svARB),
	glMultiTexCoordP1ui(fallback_glMultiTexCoordP1ui),
	glMultiTexCoordP1uiv(fallback_glMultiTexCoordP1uiv),
	glMultiTexCoordP2ui(fallback_glMultiTexCoordP2ui),
	glMultiTexCoordP2uiv(fallback_glMultiTexCoordP2uiv),
	glMultiTexCoordP3ui(fallback_glMultiTexCoordP3ui),
	glMultiTexCoordP3uiv(fallback_glMultiTexCoordP3uiv),
	glMultiTexCoordP4ui(fallback_glMultiTexCoordP4ui),
	glMultiTexCoordP4uiv(fallback_glMultiTexCoordP4uiv),
	glMultiTexCoordPointerEXT(fallback_glMultiTexCoordPointerEXT),
	glMultiTexEnvfEXT(fallback_glMultiTexEnvfEXT),
	glMultiTexEnvfvEXT(fallback_glMultiTexEnvfvEXT),
	glMultiTexEnviEXT(fallback_glMultiTexEnviEXT),
	glMultiTexEnvivEXT(fallback_glMultiTexEnvivEXT),
	glMultiTexGendEXT(fallback_glMultiTexGendEXT),
	glMultiTexGendvEXT(fallback_glMultiTexGendvEXT),
	glMultiTexGenfEXT(fallback_glMultiTexGenfEXT),
	glMultiTexGenfvEXT(fallback_glMultiTexGenfvEXT),
	glMultiTexGeniEXT(fallback_glMultiTexGeniEXT),
	glMultiTexGenivEXT(fallback_glMultiTexGenivEXT),
	glMultiTexImage1DEXT(fallback_glMultiTexImage1DEXT),
	glMultiTexImage2DEXT(fallback_glMultiTexImage2DEXT),
	glMultiTexImage3DEXT(fallback_glMultiTexImage3DEXT),
	glMultiTexParameterIivEXT(fallback_glMultiTexParameterIivEXT),
	glMultiTexParameterIuivEXT(fallback_glMultiTexParameterIuivEXT),
	glMultiTexParameterfEXT(fallback_glMultiTexParameterfEXT),
	glMultiTexParameterfvEXT(fallback_glMultiTexParameterfvEXT),
	glMultiTexParameteriEXT(fallback_glMultiTexParameteriEXT),
	glMultiTexParameterivEXT(fallback_glMultiTexParameterivEXT),
	glMultiTexRenderbufferEXT(fallback_glMultiTexRenderbufferEXT),
	glMultiTexSubImage1DEXT(fallback_glMultiTexSubImage1DEXT),
	glMultiTexSubImage2DEXT(fallback_glMultiTexSubImage2DEXT),
	glMultiTexSubImage3DEXT(fallback_glMultiTexSubImage3DEXT),
	glNamedBufferDataEXT(fallback_glNamedBufferDataEXT),
	glNamedBufferSubDataEXT(fallback_glNamedBufferSubDataEXT),
	glNamedFramebufferRenderbufferEXT(fallback_glNamedFramebufferRenderbufferEXT),
	glNamedFramebufferTexture1DEXT(fallback_glNamedFramebufferTexture1DEXT),
	glNamedFramebufferTexture2DEXT(fallback_glNamedFramebufferTexture2DEXT),
	glNamedFramebufferTexture3DEXT(fallback_glNamedFramebufferTexture3DEXT),
	glNamedFramebufferTextureEXT(fallback_glNamedFramebufferTextureEXT),
	glNamedFramebufferTextureFaceEXT(fallback_glNamedFramebufferTextureFaceEXT),
	glNamedFramebufferTextureLayerEXT(fallback_glNamedFramebufferTextureLayerEXT),
	glNamedProgramLocalParameter4dEXT(fallback_glNamedProgramLocalParameter4dEXT),
	glNamedProgramLocalParameter4dvEXT(fallback_glNamedProgramLocalParameter4dvEXT),
	glNamedProgramLocalParameter4fEXT(fallback_glNamedProgramLocalParameter4fEXT),
	glNamedProgramLocalParameter4fvEXT(fallback_glNamedProgramLocalParameter4fvEXT),
	glNamedProgramLocalParameterI4iEXT(fallback_glNamedProgramLocalParameterI4iEXT),
	glNamedProgramLocalParameterI4ivEXT(fallback_glNamedProgramLocalParameterI4ivEXT),
	glNamedProgramLocalParameterI4uiEXT(fallback_glNamedProgramLocalParameterI4uiEXT),
	glNamedProgramLocalParameterI4uivEXT(fallback_glNamedProgramLocalParameterI4uivEXT),
	glNamedProgramLocalParameters4fvEXT(fallback_glNamedProgramLocalParameters4fvEXT),
	glNamedProgramLocalParametersI4ivEXT(fallback_glNamedProgramLocalParametersI4ivEXT),
	glNamedProgramLocalParametersI4uivEXT(fallback_glNamedProgramLocalParametersI4uivEXT),
	glNamedProgramStringEXT(fallback_glNamedProgramStringEXT),
	glNamedRenderbufferStorageEXT(fallback_glNamedRenderbufferStorageEXT),
	glNamedRenderbufferStorageMultisampleCoverageEXT(fallback_glNamedRenderbufferStorageMultisampleCoverageEXT),
	glNamedRenderbufferStorageMultisampleEXT(fallback_glNamedRenderbufferStorageMultisampleEXT),
	glNamedStringARB(fallback_glNamedStringARB),
	glNewList(fallback_glNewList),
	glNewObjectBufferATI(fallback_glNewObjectBufferATI),
	glNormal3b(fallback_glNormal3b),
	glNormal3bv(fallback_glNormal3bv),
	glNormal3d(fallback_glNormal3d),
	glNormal3dv(fallback_glNormal3dv),
	glNormal3f(fallback_glNormal3f),
	glNormal3fVertex3fSUN(fallback_glNormal3fVertex3fSUN),
	glNormal3fVertex3fvSUN(fallback_glNormal3fVertex3fvSUN),
	glNormal3fv(fallback_glNormal3fv),
	glNormal3hNV(fallback_glNormal3hNV),
	glNormal3hvNV(fallback_glNormal3hvNV),
	glNormal3i(fallback_glNormal3i),
	glNormal3iv(fallback_glNormal3iv),
	glNormal3s(fallback_glNormal3s),
	glNormal3sv(fallback_glNormal3sv),
	glNormalFormatNV(fallback_glNormalFormatNV),
	glNormalP3ui(fallback_glNormalP3ui),
	glNormalP3uiv(fallback_glNormalP3uiv),
	glNormalPointer(fallback_glNormalPointer),
	glNormalPointerEXT(fallback_glNormalPointerEXT),
	glNormalPointerListIBM(fallback_glNormalPointerListIBM),
	glNormalPointervINTEL(fallback_glNormalPointervINTEL),
	glNormalStream3bATI(fallback_glNormalStream3bATI),
	glNormalStream3bvATI(fallback_glNormalStream3bvATI),
	glNormalStream3dATI(fallback_glNormalStream3dATI),
	glNormalStream3dvATI(fallback_glNormalStream3dvATI),
	glNormalStream3fATI(fallback_glNormalStream3fATI),
	glNormalStream3fvATI(fallback_glNormalStream3fvATI),
	glNormalStream3iATI(fallback_glNormalStream3iATI),
	glNormalStream3ivATI(fallback_glNormalStream3ivATI),
	glNormalStream3sATI(fallback_glNormalStream3sATI),
	glNormalStream3svATI(fallback_glNormalStream3svATI),
	glObjectPurgeableAPPLE(fallback_glObjectPurgeableAPPLE),
	glObjectUnpurgeableAPPLE(fallback_glObjectUnpurgeableAPPLE),
	glOrtho(fallback_glOrtho),
	glPNTrianglesfATI(fallback_glPNTrianglesfATI),
	glPNTrianglesiATI(fallback_glPNTrianglesiATI),
	glPassTexCoordATI(fallback_glPassTexCoordATI),
	glPassThrough(fallback_glPassThrough),
	glPatchParameterfv(fallback_glPatchParameterfv),
	glPatchParameteri(fallback_glPatchParameteri),
	glPauseTransformFeedback(fallback_glPauseTransformFeedback),
	glPauseTransformFeedbackNV(fallback_glPauseTransformFeedbackNV),
	glPixelDataRangeNV(fallback_glPixelDataRangeNV),
	glPixelMapfv(fallback_glPixelMapfv),
	glPixelMapuiv(fallback_glPixelMapuiv),
	glPixelMapusv(fallback_glPixelMapusv),
	glPixelStoref(fallback_glPixelStoref),
	glPixelStorei(fallback_glPixelStorei),
	glPixelTexGenParameterfSGIS(fallback_glPixelTexGenParameterfSGIS),
	glPixelTexGenParameterfvSGIS(fallback_glPixelTexGenParameterfvSGIS),
	glPixelTexGenParameteriSGIS(fallback_glPixelTexGenParameteriSGIS),
	glPixelTexGenParameterivSGIS(fallback_glPixelTexGenParameterivSGIS),
	glPixelTexGenSGIX(fallback_glPixelTexGenSGIX),
	glPixelTransferf(fallback_glPixelTransferf),
	glPixelTransferi(fallback_glPixelTransferi),
	glPixelTransformParameterfEXT(fallback_glPixelTransformParameterfEXT),
	glPixelTransformParameterfvEXT(fallback_glPixelTransformParameterfvEXT),
	glPixelTransformParameteriEXT(fallback_glPixelTransformParameteriEXT),
	glPixelTransformParameterivEXT(fallback_glPixelTransformParameterivEXT),
	glPixelZoom(fallback_glPixelZoom),
	glPointParameterf(fallback_glPointParameterf),
	glPointParameterfARB(fallback_glPointParameterfARB),
	glPointParameterfEXT(fallback_glPointParameterfEXT),
	glPointParameterfSGIS(fallback_glPointParameterfSGIS),
	glPointParameterfv(fallback_glPointParameterfv),
	glPointParameterfvARB(fallback_glPointParameterfvARB),
	glPointParameterfvEXT(fallback_glPointParameterfvEXT),
	glPointParameterfvSGIS(fallback_glPointParameterfvSGIS),
	glPointParameteri(fallback_glPointParameteri),
	glPointParameteriNV(fallback_glPointParameteriNV),
	glPointParameteriv(fallback_glPointParameteriv),
	glPointParameterivNV(fallback_glPointParameterivNV),
	glPointSize(fallback_glPointSize),
	glPollAsyncSGIX(fallback_glPollAsyncSGIX),
	glPollInstrumentsSGIX(fallback_glPollInstrumentsSGIX),
	glPolygonMode(fallback_glPolygonMode),
	glPolygonOffset(fallback_glPolygonOffset),
	glPolygonOffsetEXT(fallback_glPolygonOffsetEXT),
	glPolygonStipple(fallback_glPolygonStipple),
	glPopAttrib(fallback_glPopAttrib),
	glPopClientAttrib(fallback_glPopClientAttrib),
	glPopMatrix(fallback_glPopMatrix),
	glPopName(fallback_glPopName),
	glPresentFrameDualFillNV(fallback_glPresentFrameDualFillNV),
	glPresentFrameKeyedNV(fallback_glPresentFrameKeyedNV),
	glPrimitiveRestartIndex(fallback_glPrimitiveRestartIndex),
	glPrimitiveRestartIndexNV(fallback_glPrimitiveRestartIndexNV),
	glPrimitiveRestartNV(fallback_glPrimitiveRestartNV),
	glPrioritizeTextures(fallback_glPrioritizeTextures),
	glPrioritizeTexturesEXT(fallback_glPrioritizeTexturesEXT),
	glProgramBufferParametersIivNV(fallback_glProgramBufferParametersIivNV),
	glProgramBufferParametersIuivNV(fallback_glProgramBufferParametersIuivNV),
	glProgramBufferParametersfvNV(fallback_glProgramBufferParametersfvNV),
	glProgramEnvParameter4dARB(fallback_glProgramEnvParameter4dARB),
	glProgramEnvParameter4dvARB(fallback_glProgramEnvParameter4dvARB),
	glProgramEnvParameter4fARB(fallback_glProgramEnvParameter4fARB),
	glProgramEnvParameter4fvARB(fallback_glProgramEnvParameter4fvARB),
	glProgramEnvParameterI4iNV(fallback_glProgramEnvParameterI4iNV),
	glProgramEnvParameterI4ivNV(fallback_glProgramEnvParameterI4ivNV),
	glProgramEnvParameterI4uiNV(fallback_glProgramEnvParameterI4uiNV),
	glProgramEnvParameterI4uivNV(fallback_glProgramEnvParameterI4uivNV),
	glProgramEnvParameters4fvEXT(fallback_glProgramEnvParameters4fvEXT),
	glProgramEnvParametersI4ivNV(fallback_glProgramEnvParametersI4ivNV),
	glProgramEnvParametersI4uivNV(fallback_glProgramEnvParametersI4uivNV),
	glProgramLocalParameter4dARB(fallback_glProgramLocalParameter4dARB),
	glProgramLocalParameter4dvARB(fallback_glProgramLocalParameter4dvARB),
	glProgramLocalParameter4fARB(fallback_glProgramLocalParameter4fARB),
	glProgramLocalParameter4fvARB(fallback_glProgramLocalParameter4fvARB),
	glProgramLocalParameterI4iNV(fallback_glProgramLocalParameterI4iNV),
	glProgramLocalParameterI4ivNV(fallback_glProgramLocalParameterI4ivNV),
	glProgramLocalParameterI4uiNV(fallback_glProgramLocalParameterI4uiNV),
	glProgramLocalParameterI4uivNV(fallback_glProgramLocalParameterI4uivNV),
	glProgramLocalParameters4fvEXT(fallback_glProgramLocalParameters4fvEXT),
	glProgramLocalParametersI4ivNV(fallback_glProgramLocalParametersI4ivNV),
	glProgramLocalParametersI4uivNV(fallback_glProgramLocalParametersI4uivNV),
	glProgramNamedParameter4dNV(fallback_glProgramNamedParameter4dNV),
	glProgramNamedParameter4dvNV(fallback_glProgramNamedParameter4dvNV),
	glProgramNamedParameter4fNV(fallback_glProgramNamedParameter4fNV),
	glProgramNamedParameter4fvNV(fallback_glProgramNamedParameter4fvNV),
	glProgramParameter4dNV(fallback_glProgramParameter4dNV),
	glProgramParameter4dvNV(fallback_glProgramParameter4dvNV),
	glProgramParameter4fNV(fallback_glProgramParameter4fNV),
	glProgramParameter4fvNV(fallback_glProgramParameter4fvNV),
	glProgramParameteri(fallback_glProgramParameteri),
	glProgramParameteriARB(fallback_glProgramParameteriARB),
	glProgramParameteriEXT(fallback_glProgramParameteriEXT),
	glProgramParameters4dvNV(fallback_glProgramParameters4dvNV),
	glProgramParameters4fvNV(fallback_glProgramParameters4fvNV),
	glProgramStringARB(fallback_glProgramStringARB),
	glProgramUniform1dEXT(fallback_glProgramUniform1dEXT),
	glProgramUniform1dvEXT(fallback_glProgramUniform1dvEXT),
	glProgramUniform1fEXT(fallback_glProgramUniform1fEXT),
	glProgramUniform1fvEXT(fallback_glProgramUniform1fvEXT),
	glProgramUniform1iEXT(fallback_glProgramUniform1iEXT),
	glProgramUniform1ivEXT(fallback_glProgramUniform1ivEXT),
	glProgramUniform1uiEXT(fallback_glProgramUniform1uiEXT),
	glProgramUniform1uivEXT(fallback_glProgramUniform1uivEXT),
	glProgramUniform2dEXT(fallback_glProgramUniform2dEXT),
	glProgramUniform2dvEXT(fallback_glProgramUniform2dvEXT),
	glProgramUniform2fEXT(fallback_glProgramUniform2fEXT),
	glProgramUniform2fvEXT(fallback_glProgramUniform2fvEXT),
	glProgramUniform2iEXT(fallback_glProgramUniform2iEXT),
	glProgramUniform2ivEXT(fallback_glProgramUniform2ivEXT),
	glProgramUniform2uiEXT(fallback_glProgramUniform2uiEXT),
	glProgramUniform2uivEXT(fallback_glProgramUniform2uivEXT),
	glProgramUniform3dEXT(fallback_glProgramUniform3dEXT),
	glProgramUniform3dvEXT(fallback_glProgramUniform3dvEXT),
	glProgramUniform3fEXT(fallback_glProgramUniform3fEXT),
	glProgramUniform3fvEXT(fallback_glProgramUniform3fvEXT),
	glProgramUniform3iEXT(fallback_glProgramUniform3iEXT),
	glProgramUniform3ivEXT(fallback_glProgramUniform3ivEXT),
	glProgramUniform3uiEXT(fallback_glProgramUniform3uiEXT),
	glProgramUniform3uivEXT(fallback_glProgramUniform3uivEXT),
	glProgramUniform4dEXT(fallback_glProgramUniform4dEXT),
	glProgramUniform4dvEXT(fallback_glProgramUniform4dvEXT),
	glProgramUniform4fEXT(fallback_glProgramUniform4fEXT),
	glProgramUniform4fvEXT(fallback_glProgramUniform4fvEXT),
	glProgramUniform4iEXT(fallback_glProgramUniform4iEXT),
	glProgramUniform4ivEXT(fallback_glProgramUniform4ivEXT),
	glProgramUniform4uiEXT(fallback_glProgramUniform4uiEXT),
	glProgramUniform4uivEXT(fallback_glProgramUniform4uivEXT),
	glProgramUniformMatrix2dvEXT(fallback_glProgramUniformMatrix2dvEXT),
	glProgramUniformMatrix2fvEXT(fallback_glProgramUniformMatrix2fvEXT),
	glProgramUniformMatrix2x3dvEXT(fallback_glProgramUniformMatrix2x3dvEXT),
	glProgramUniformMatrix2x3fvEXT(fallback_glProgramUniformMatrix2x3fvEXT),
	glProgramUniformMatrix2x4dvEXT(fallback_glProgramUniformMatrix2x4dvEXT),
	glProgramUniformMatrix2x4fvEXT(fallback_glProgramUniformMatrix2x4fvEXT),
	glProgramUniformMatrix3dvEXT(fallback_glProgramUniformMatrix3dvEXT),
	glProgramUniformMatrix3fvEXT(fallback_glProgramUniformMatrix3fvEXT),
	glProgramUniformMatrix3x2dvEXT(fallback_glProgramUniformMatrix3x2dvEXT),
	glProgramUniformMatrix3x2fvEXT(fallback_glProgramUniformMatrix3x2fvEXT),
	glProgramUniformMatrix3x4dvEXT(fallback_glProgramUniformMatrix3x4dvEXT),
	glProgramUniformMatrix3x4fvEXT(fallback_glProgramUniformMatrix3x4fvEXT),
	glProgramUniformMatrix4dvEXT(fallback_glProgramUniformMatrix4dvEXT),
	glProgramUniformMatrix4fvEXT(fallback_glProgramUniformMatrix4fvEXT),
	glProgramUniformMatrix4x2dvEXT(fallback_glProgramUniformMatrix4x2dvEXT),
	glProgramUniformMatrix4x2fvEXT(fallback_glProgramUniformMatrix4x2fvEXT),
	glProgramUniformMatrix4x3dvEXT(fallback_glProgramUniformMatrix4x3dvEXT),
	glProgramUniformMatrix4x3fvEXT(fallback_glProgramUniformMatrix4x3fvEXT),
	glProgramUniformui64NV(fallback_glProgramUniformui64NV),
	glProgramUniformui64vNV(fallback_glProgramUniformui64vNV),
	glProgramVertexLimitNV(fallback_glProgramVertexLimitNV),
	glProvokingVertex(fallback_glProvokingVertex),
	glProvokingVertexEXT(fallback_glProvokingVertexEXT),
	glPushAttrib(fallback_glPushAttrib),
	glPushClientAttrib(fallback_glPushClientAttrib),
	glPushClientAttribDefaultEXT(fallback_glPushClientAttribDefaultEXT),
	glPushMatrix(fallback_glPushMatrix),
	glPushName(fallback_glPushName),
	glQueryCounter(fallback_glQueryCounter),
	glRasterPos2d(fallback_glRasterPos2d),
	glRasterPos2dv(fallback_glRasterPos2dv),
	glRasterPos2f(fallback_glRasterPos2f),
	glRasterPos2fv(fallback_glRasterPos2fv),
	glRasterPos2i(fallback_glRasterPos2i),
	glRasterPos2iv(fallback_glRasterPos2iv),
	glRasterPos2s(fallback_glRasterPos2s),
	glRasterPos2sv(fallback_glRasterPos2sv),
	glRasterPos3d(fallback_glRasterPos3d),
	glRasterPos3dv(fallback_glRasterPos3dv),
	glRasterPos3f(fallback_glRasterPos3f),
	glRasterPos3fv(fallback_glRasterPos3fv),
	glRasterPos3i(fallback_glRasterPos3i),
	glRasterPos3iv(fallback_glRasterPos3iv),
	glRasterPos3s(fallback_glRasterPos3s),
	glRasterPos3sv(fallback_glRasterPos3sv),
	glRasterPos4d(fallback_glRasterPos4d),
	glRasterPos4dv(fallback_glRasterPos4dv),
	glRasterPos4f(fallback_glRasterPos4f),
	glRasterPos4fv(fallback_glRasterPos4fv),
	glRasterPos4i(fallback_glRasterPos4i),
	glRasterPos4iv(fallback_glRasterPos4iv),
	glRasterPos4s(fallback_glRasterPos4s),
	glRasterPos4sv(fallback_glRasterPos4sv),
	glReadBuffer(fallback_glReadBuffer),
	glReadInstrumentsSGIX(fallback_glReadInstrumentsSGIX),
	glReadPixels(fallback_glReadPixels),
	glRectd(fallback_glRectd),
	glRectdv(fallback_glRectdv),
	glRectf(fallback_glRectf),
	glRectfv(fallback_glRectfv),
	glRecti(fallback_glRecti),
	glRectiv(fallback_glRectiv),
	glRects(fallback_glRects),
	glRectsv(fallback_glRectsv),
	glReferencePlaneSGIX(fallback_glReferencePlaneSGIX),
	glRenderMode(fallback_glRenderMode),
	glRenderbufferStorage(fallback_glRenderbufferStorage),
	glRenderbufferStorageEXT(fallback_glRenderbufferStorageEXT),
	glRenderbufferStorageMultisample(fallback_glRenderbufferStorageMultisample),
	glRenderbufferStorageMultisampleCoverageNV(fallback_glRenderbufferStorageMultisampleCoverageNV),
	glRenderbufferStorageMultisampleEXT(fallback_glRenderbufferStorageMultisampleEXT),
	glReplacementCodePointerSUN(fallback_glReplacementCodePointerSUN),
	glReplacementCodeubSUN(fallback_glReplacementCodeubSUN),
	glReplacementCodeubvSUN(fallback_glReplacementCodeubvSUN),
	glReplacementCodeuiColor3fVertex3fSUN(fallback_glReplacementCodeuiColor3fVertex3fSUN),
	glReplacementCodeuiColor3fVertex3fvSUN(fallback_glReplacementCodeuiColor3fVertex3fvSUN),
	glReplacementCodeuiColor4fNormal3fVertex3fSUN(fallback_glReplacementCodeuiColor4fNormal3fVertex3fSUN),
	glReplacementCodeuiColor4fNormal3fVertex3fvSUN(fallback_glReplacementCodeuiColor4fNormal3fVertex3fvSUN),
	glReplacementCodeuiColor4ubVertex3fSUN(fallback_glReplacementCodeuiColor4ubVertex3fSUN),
	glReplacementCodeuiColor4ubVertex3fvSUN(fallback_glReplacementCodeuiColor4ubVertex3fvSUN),
	glReplacementCodeuiNormal3fVertex3fSUN(fallback_glReplacementCodeuiNormal3fVertex3fSUN),
	glReplacementCodeuiNormal3fVertex3fvSUN(fallback_glReplacementCodeuiNormal3fVertex3fvSUN),
	glReplacementCodeuiSUN(fallback_glReplacementCodeuiSUN),
	glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN(fallback_glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN),
	glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN(fallback_glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN),
	glReplacementCodeuiTexCoord2fNormal3fVertex3fSUN(fallback_glReplacementCodeuiTexCoord2fNormal3fVertex3fSUN),
	glReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN(fallback_glReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN),
	glReplacementCodeuiTexCoord2fVertex3fSUN(fallback_glReplacementCodeuiTexCoord2fVertex3fSUN),
	glReplacementCodeuiTexCoord2fVertex3fvSUN(fallback_glReplacementCodeuiTexCoord2fVertex3fvSUN),
	glReplacementCodeuiVertex3fSUN(fallback_glReplacementCodeuiVertex3fSUN),
	glReplacementCodeuiVertex3fvSUN(fallback_glReplacementCodeuiVertex3fvSUN),
	glReplacementCodeuivSUN(fallback_glReplacementCodeuivSUN),
	glReplacementCodeusSUN(fallback_glReplacementCodeusSUN),
	glReplacementCodeusvSUN(fallback_glReplacementCodeusvSUN),
	glRequestResidentProgramsNV(fallback_glRequestResidentProgramsNV),
	glResetHistogram(fallback_glResetHistogram),
	glResetHistogramEXT(fallback_glResetHistogramEXT),
	glResetMinmax(fallback_glResetMinmax),
	glResetMinmaxEXT(fallback_glResetMinmaxEXT),
	glResizeBuffersMESA(fallback_glResizeBuffersMESA),
	glResumeTransformFeedback(fallback_glResumeTransformFeedback),
	glResumeTransformFeedbackNV(fallback_glResumeTransformFeedbackNV),
	glRotated(fallback_glRotated),
	glRotatef(fallback_glRotatef),
	glSampleCoverage(fallback_glSampleCoverage),
	glSampleCoverageARB(fallback_glSampleCoverageARB),
	glSampleMapATI(fallback_glSampleMapATI),
	glSampleMaskEXT(fallback_glSampleMaskEXT),
	glSampleMaskIndexedNV(fallback_glSampleMaskIndexedNV),
	glSampleMaskSGIS(fallback_glSampleMaskSGIS),
	glSampleMaski(fallback_glSampleMaski),
	glSamplePatternEXT(fallback_glSamplePatternEXT),
	glSamplePatternSGIS(fallback_glSamplePatternSGIS),
	glSamplerParameterIiv(fallback_glSamplerParameterIiv),
	glSamplerParameterIuiv(fallback_glSamplerParameterIuiv),
	glSamplerParameterf(fallback_glSamplerParameterf),
	glSamplerParameterfv(fallback_glSamplerParameterfv),
	glSamplerParameteri(fallback_glSamplerParameteri),
	glSamplerParameteriv(fallback_glSamplerParameteriv),
	glScaled(fallback_glScaled),
	glScalef(fallback_glScalef),
	glScissor(fallback_glScissor),
	glSecondaryColor3b(fallback_glSecondaryColor3b),
	glSecondaryColor3bEXT(fallback_glSecondaryColor3bEXT),
	glSecondaryColor3bv(fallback_glSecondaryColor3bv),
	glSecondaryColor3bvEXT(fallback_glSecondaryColor3bvEXT),
	glSecondaryColor3d(fallback_glSecondaryColor3d),
	glSecondaryColor3dEXT(fallback_glSecondaryColor3dEXT),
	glSecondaryColor3dv(fallback_glSecondaryColor3dv),
	glSecondaryColor3dvEXT(fallback_glSecondaryColor3dvEXT),
	glSecondaryColor3f(fallback_glSecondaryColor3f),
	glSecondaryColor3fEXT(fallback_glSecondaryColor3fEXT),
	glSecondaryColor3fv(fallback_glSecondaryColor3fv),
	glSecondaryColor3fvEXT(fallback_glSecondaryColor3fvEXT),
	glSecondaryColor3hNV(fallback_glSecondaryColor3hNV),
	glSecondaryColor3hvNV(fallback_glSecondaryColor3hvNV),
	glSecondaryColor3i(fallback_glSecondaryColor3i),
	glSecondaryColor3iEXT(fallback_glSecondaryColor3iEXT),
	glSecondaryColor3iv(fallback_glSecondaryColor3iv),
	glSecondaryColor3ivEXT(fallback_glSecondaryColor3ivEXT),
	glSecondaryColor3s(fallback_glSecondaryColor3s),
	glSecondaryColor3sEXT(fallback_glSecondaryColor3sEXT),
	glSecondaryColor3sv(fallback_glSecondaryColor3sv),
	glSecondaryColor3svEXT(fallback_glSecondaryColor3svEXT),
	glSecondaryColor3ub(fallback_glSecondaryColor3ub),
	glSecondaryColor3ubEXT(fallback_glSecondaryColor3ubEXT),
	glSecondaryColor3ubv(fallback_glSecondaryColor3ubv),
	glSecondaryColor3ubvEXT(fallback_glSecondaryColor3ubvEXT),
	glSecondaryColor3ui(fallback_glSecondaryColor3ui),
	glSecondaryColor3uiEXT(fallback_glSecondaryColor3uiEXT),
	glSecondaryColor3uiv(fallback_glSecondaryColor3uiv),
	glSecondaryColor3uivEXT(fallback_glSecondaryColor3uivEXT),
	glSecondaryColor3us(fallback_glSecondaryColor3us),
	glSecondaryColor3usEXT(fallback_glSecondaryColor3usEXT),
	glSecondaryColor3usv(fallback_glSecondaryColor3usv),
	glSecondaryColor3usvEXT(fallback_glSecondaryColor3usvEXT),
	glSecondaryColorFormatNV(fallback_glSecondaryColorFormatNV),
	glSecondaryColorP3ui(fallback_glSecondaryColorP3ui),
	glSecondaryColorP3uiv(fallback_glSecondaryColorP3uiv),
	glSecondaryColorPointer(fallback_glSecondaryColorPointer),
	glSecondaryColorPointerEXT(fallback_glSecondaryColorPointerEXT),
	glSecondaryColorPointerListIBM(fallback_glSecondaryColorPointerListIBM),
	glSelectBuffer(fallback_glSelectBuffer),
	glSelectPerfMonitorCountersAMD(fallback_glSelectPerfMonitorCountersAMD),
	glSeparableFilter2D(fallback_glSeparableFilter2D),
	glSeparableFilter2DEXT(fallback_glSeparableFilter2DEXT),
	glSetFenceAPPLE(fallback_glSetFenceAPPLE),
	glSetFenceNV(fallback_glSetFenceNV),
	glSetFragmentShaderConstantATI(fallback_glSetFragmentShaderConstantATI),
	glSetInvariantEXT(fallback_glSetInvariantEXT),
	glSetLocalConstantEXT(fallback_glSetLocalConstantEXT),
	glShadeModel(fallback_glShadeModel),
	glShaderOp1EXT(fallback_glShaderOp1EXT),
	glShaderOp2EXT(fallback_glShaderOp2EXT),
	glShaderOp3EXT(fallback_glShaderOp3EXT),
	glShaderSource(fallback_glShaderSource),
	glShaderSourceARB(fallback_glShaderSourceARB),
	glSharpenTexFuncSGIS(fallback_glSharpenTexFuncSGIS),
	glSpriteParameterfSGIX(fallback_glSpriteParameterfSGIX),
	glSpriteParameterfvSGIX(fallback_glSpriteParameterfvSGIX),
	glSpriteParameteriSGIX(fallback_glSpriteParameteriSGIX),
	glSpriteParameterivSGIX(fallback_glSpriteParameterivSGIX),
	glStartInstrumentsSGIX(fallback_glStartInstrumentsSGIX),
	glStencilClearTagEXT(fallback_glStencilClearTagEXT),
	glStencilFunc(fallback_glStencilFunc),
	glStencilFuncSeparate(fallback_glStencilFuncSeparate),
	glStencilFuncSeparateATI(fallback_glStencilFuncSeparateATI),
	glStencilMask(fallback_glStencilMask),
	glStencilMaskSeparate(fallback_glStencilMaskSeparate),
	glStencilOp(fallback_glStencilOp),
	glStencilOpSeparate(fallback_glStencilOpSeparate),
	glStencilOpSeparateATI(fallback_glStencilOpSeparateATI),
	glStopInstrumentsSGIX(fallback_glStopInstrumentsSGIX),
	glStringMarkerGREMEDY(fallback_glStringMarkerGREMEDY),
	glSwizzleEXT(fallback_glSwizzleEXT),
	glTagSampleBufferSGIX(fallback_glTagSampleBufferSGIX),
	glTangent3bEXT(fallback_glTangent3bEXT),
	glTangent3bvEXT(fallback_glTangent3bvEXT),
	glTangent3dEXT(fallback_glTangent3dEXT),
	glTangent3dvEXT(fallback_glTangent3dvEXT),
	glTangent3fEXT(fallback_glTangent3fEXT),
	glTangent3fvEXT(fallback_glTangent3fvEXT),
	glTangent3iEXT(fallback_glTangent3iEXT),
	glTangent3ivEXT(fallback_glTangent3ivEXT),
	glTangent3sEXT(fallback_glTangent3sEXT),
	glTangent3svEXT(fallback_glTangent3svEXT),
	glTangentPointerEXT(fallback_glTangentPointerEXT),
	glTbufferMask3DFX(fallback_glTbufferMask3DFX),
	glTessellationFactorAMD(fallback_glTessellationFactorAMD),
	glTessellationModeAMD(fallback_glTessellationModeAMD),
	glTestFenceAPPLE(fallback_glTestFenceAPPLE),
	glTestFenceNV(fallback_glTestFenceNV),
	glTestObjectAPPLE(fallback_glTestObjectAPPLE),
	glTexBuffer(fallback_glTexBuffer),
	glTexBufferARB(fallback_glTexBufferARB),
	glTexBufferEXT(fallback_glTexBufferEXT),
	glTexBumpParameterfvATI(fallback_glTexBumpParameterfvATI),
	glTexBumpParameterivATI(fallback_glTexBumpParameterivATI),
	glTexCoord1d(fallback_glTexCoord1d),
	glTexCoord1dv(fallback_glTexCoord1dv),
	glTexCoord1f(fallback_glTexCoord1f),
	glTexCoord1fv(fallback_glTexCoord1fv),
	glTexCoord1hNV(fallback_glTexCoord1hNV),
	glTexCoord1hvNV(fallback_glTexCoord1hvNV),
	glTexCoord1i(fallback_glTexCoord1i),
	glTexCoord1iv(fallback_glTexCoord1iv),
	glTexCoord1s(fallback_glTexCoord1s),
	glTexCoord1sv(fallback_glTexCoord1sv),
	glTexCoord2d(fallback_glTexCoord2d),
	glTexCoord2dv(fallback_glTexCoord2dv),
	glTexCoord2f(fallback_glTexCoord2f),
	glTexCoord2fColor3fVertex3fSUN(fallback_glTexCoord2fColor3fVertex3fSUN),
	glTexCoord2fColor3fVertex3fvSUN(fallback_glTexCoord2fColor3fVertex3fvSUN),
	glTexCoord2fColor4fNormal3fVertex3fSUN(fallback_glTexCoord2fColor4fNormal3fVertex3fSUN),
	glTexCoord2fColor4fNormal3fVertex3fvSUN(fallback_glTexCoord2fColor4fNormal3fVertex3fvSUN),
	glTexCoord2fColor4ubVertex3fSUN(fallback_glTexCoord2fColor4ubVertex3fSUN),
	glTexCoord2fColor4ubVertex3fvSUN(fallback_glTexCoord2fColor4ubVertex3fvSUN),
	glTexCoord2fNormal3fVertex3fSUN(fallback_glTexCoord2fNormal3fVertex3fSUN),
	glTexCoord2fNormal3fVertex3fvSUN(fallback_glTexCoord2fNormal3fVertex3fvSUN),
	glTexCoord2fVertex3fSUN(fallback_glTexCoord2fVertex3fSUN),
	glTexCoord2fVertex3fvSUN(fallback_glTexCoord2fVertex3fvSUN),
	glTexCoord2fv(fallback_glTexCoord2fv),
	glTexCoord2hNV(fallback_glTexCoord2hNV),
	glTexCoord2hvNV(fallback_glTexCoord2hvNV),
	glTexCoord2i(fallback_glTexCoord2i),
	glTexCoord2iv(fallback_glTexCoord2iv),
	glTexCoord2s(fallback_glTexCoord2s),
	glTexCoord2sv(fallback_glTexCoord2sv),
	glTexCoord3d(fallback_glTexCoord3d),
	glTexCoord3dv(fallback_glTexCoord3dv),
	glTexCoord3f(fallback_glTexCoord3f),
	glTexCoord3fv(fallback_glTexCoord3fv),
	glTexCoord3hNV(fallback_glTexCoord3hNV),
	glTexCoord3hvNV(fallback_glTexCoord3hvNV),
	glTexCoord3i(fallback_glTexCoord3i),
	glTexCoord3iv(fallback_glTexCoord3iv),
	glTexCoord3s(fallback_glTexCoord3s),
	glTexCoord3sv(fallback_glTexCoord3sv),
	glTexCoord4d(fallback_glTexCoord4d),
	glTexCoord4dv(fallback_glTexCoord4dv),
	glTexCoord4f(fallback_glTexCoord4f),
	glTexCoord4fColor4fNormal3fVertex4fSUN(fallback_glTexCoord4fColor4fNormal3fVertex4fSUN),
	glTexCoord4fColor4fNormal3fVertex4fvSUN(fallback_glTexCoord4fColor4fNormal3fVertex4fvSUN),
	glTexCoord4fVertex4fSUN(fallback_glTexCoord4fVertex4fSUN),
	glTexCoord4fVertex4fvSUN(fallback_glTexCoord4fVertex4fvSUN),
	glTexCoord4fv(fallback_glTexCoord4fv),
	glTexCoord4hNV(fallback_glTexCoord4hNV),
	glTexCoord4hvNV(fallback_glTexCoord4hvNV),
	glTexCoord4i(fallback_glTexCoord4i),
	glTexCoord4iv(fallback_glTexCoord4iv),
	glTexCoord4s(fallback_glTexCoord4s),
	glTexCoord4sv(fallback_glTexCoord4sv),
	glTexCoordFormatNV(fallback_glTexCoordFormatNV),
	glTexCoordP1ui(fallback_glTexCoordP1ui),
	glTexCoordP1uiv(fallback_glTexCoordP1uiv),
	glTexCoordP2ui(fallback_glTexCoordP2ui),
	glTexCoordP2uiv(fallback_glTexCoordP2uiv),
	glTexCoordP3ui(fallback_glTexCoordP3ui),
	glTexCoordP3uiv(fallback_glTexCoordP3uiv),
	glTexCoordP4ui(fallback_glTexCoordP4ui),
	glTexCoordP4uiv(fallback_glTexCoordP4uiv),
	glTexCoordPointer(fallback_glTexCoordPointer),
	glTexCoordPointerEXT(fallback_glTexCoordPointerEXT),
	glTexCoordPointerListIBM(fallback_glTexCoordPointerListIBM),
	glTexCoordPointervINTEL(fallback_glTexCoordPointervINTEL),
	glTexEnvf(fallback_glTexEnvf),
	glTexEnvfv(fallback_glTexEnvfv),
	glTexEnvi(fallback_glTexEnvi),
	glTexEnviv(fallback_glTexEnviv),
	glTexFilterFuncSGIS(fallback_glTexFilterFuncSGIS),
	glTexGend(fallback_glTexGend),
	glTexGendv(fallback_glTexGendv),
	glTexGenf(fallback_glTexGenf),
	glTexGenfv(fallback_glTexGenfv),
	glTexGeni(fallback_glTexGeni),
	glTexGeniv(fallback_glTexGeniv),
	glTexImage1D(fallback_glTexImage1D),
	glTexImage2D(fallback_glTexImage2D),
	glTexImage2DMultisample(fallback_glTexImage2DMultisample),
	glTexImage3D(fallback_glTexImage3D),
	glTexImage3DEXT(fallback_glTexImage3DEXT),
	glTexImage3DMultisample(fallback_glTexImage3DMultisample),
	glTexImage4DSGIS(fallback_glTexImage4DSGIS),
	glTexParameterIiv(fallback_glTexParameterIiv),
	glTexParameterIivEXT(fallback_glTexParameterIivEXT),
	glTexParameterIuiv(fallback_glTexParameterIuiv),
	glTexParameterIuivEXT(fallback_glTexParameterIuivEXT),
	glTexParameterf(fallback_glTexParameterf),
	glTexParameterfv(fallback_glTexParameterfv),
	glTexParameteri(fallback_glTexParameteri),
	glTexParameteriv(fallback_glTexParameteriv),
	glTexRenderbufferNV(fallback_glTexRenderbufferNV),
	glTexSubImage1D(fallback_glTexSubImage1D),
	glTexSubImage1DEXT(fallback_glTexSubImage1DEXT),
	glTexSubImage2D(fallback_glTexSubImage2D),
	glTexSubImage2DEXT(fallback_glTexSubImage2DEXT),
	glTexSubImage3D(fallback_glTexSubImage3D),
	glTexSubImage3DEXT(fallback_glTexSubImage3DEXT),
	glTexSubImage4DSGIS(fallback_glTexSubImage4DSGIS),
	glTextureBarrierNV(fallback_glTextureBarrierNV),
	glTextureBufferEXT(fallback_glTextureBufferEXT),
	glTextureColorMaskSGIS(fallback_glTextureColorMaskSGIS),
	glTextureImage1DEXT(fallback_glTextureImage1DEXT),
	glTextureImage2DEXT(fallback_glTextureImage2DEXT),
	glTextureImage3DEXT(fallback_glTextureImage3DEXT),
	glTextureLightEXT(fallback_glTextureLightEXT),
	glTextureMaterialEXT(fallback_glTextureMaterialEXT),
	glTextureNormalEXT(fallback_glTextureNormalEXT),
	glTextureParameterIivEXT(fallback_glTextureParameterIivEXT),
	glTextureParameterIuivEXT(fallback_glTextureParameterIuivEXT),
	glTextureParameterfEXT(fallback_glTextureParameterfEXT),
	glTextureParameterfvEXT(fallback_glTextureParameterfvEXT),
	glTextureParameteriEXT(fallback_glTextureParameteriEXT),
	glTextureParameterivEXT(fallback_glTextureParameterivEXT),
	glTextureRangeAPPLE(fallback_glTextureRangeAPPLE),
	glTextureRenderbufferEXT(fallback_glTextureRenderbufferEXT),
	glTextureSubImage1DEXT(fallback_glTextureSubImage1DEXT),
	glTextureSubImage2DEXT(fallback_glTextureSubImage2DEXT),
	glTextureSubImage3DEXT(fallback_glTextureSubImage3DEXT),
	glTrackMatrixNV(fallback_glTrackMatrixNV),
	glTransformFeedbackAttribsNV(fallback_glTransformFeedbackAttribsNV),
	glTransformFeedbackVaryings(fallback_glTransformFeedbackVaryings),
	glTransformFeedbackVaryingsEXT(fallback_glTransformFeedbackVaryingsEXT),
	glTransformFeedbackVaryingsNV(fallback_glTransformFeedbackVaryingsNV),
	glTranslated(fallback_glTranslated),
	glTranslatef(fallback_glTranslatef),
	glUniform1d(fallback_glUniform1d),
	glUniform1dv(fallback_glUniform1dv),
	glUniform1f(fallback_glUniform1f),
	glUniform1fARB(fallback_glUniform1fARB),
	glUniform1fv(fallback_glUniform1fv),
	glUniform1fvARB(fallback_glUniform1fvARB),
	glUniform1i(fallback_glUniform1i),
	glUniform1iARB(fallback_glUniform1iARB),
	glUniform1iv(fallback_glUniform1iv),
	glUniform1ivARB(fallback_glUniform1ivARB),
	glUniform1ui(fallback_glUniform1ui),
	glUniform1uiEXT(fallback_glUniform1uiEXT),
	glUniform1uiv(fallback_glUniform1uiv),
	glUniform1uivEXT(fallback_glUniform1uivEXT),
	glUniform2d(fallback_glUniform2d),
	glUniform2dv(fallback_glUniform2dv),
	glUniform2f(fallback_glUniform2f),
	glUniform2fARB(fallback_glUniform2fARB),
	glUniform2fv(fallback_glUniform2fv),
	glUniform2fvARB(fallback_glUniform2fvARB),
	glUniform2i(fallback_glUniform2i),
	glUniform2iARB(fallback_glUniform2iARB),
	glUniform2iv(fallback_glUniform2iv),
	glUniform2ivARB(fallback_glUniform2ivARB),
	glUniform2ui(fallback_glUniform2ui),
	glUniform2uiEXT(fallback_glUniform2uiEXT),
	glUniform2uiv(fallback_glUniform2uiv),
	glUniform2uivEXT(fallback_glUniform2uivEXT),
	glUniform3d(fallback_glUniform3d),
	glUniform3dv(fallback_glUniform3dv),
	glUniform3f(fallback_glUniform3f),
	glUniform3fARB(fallback_glUniform3fARB),
	glUniform3fv(fallback_glUniform3fv),
	glUniform3fvARB(fallback_glUniform3fvARB),
	glUniform3i(fallback_glUniform3i),
	glUniform3iARB(fallback_glUniform3iARB),
	glUniform3iv(fallback_glUniform3iv),
	glUniform3ivARB(fallback_glUniform3ivARB),
	glUniform3ui(fallback_glUniform3ui),
	glUniform3uiEXT(fallback_glUniform3uiEXT),
	glUniform3uiv(fallback_glUniform3uiv),
	glUniform3uivEXT(fallback_glUniform3uivEXT),
	glUniform4d(fallback_glUniform4d),
	glUniform4dv(fallback_glUniform4dv),
	glUniform4f(fallback_glUniform4f),
	glUniform4fARB(fallback_glUniform4fARB),
	glUniform4fv(fallback_glUniform4fv),
	glUniform4fvARB(fallback_glUniform4fvARB),
	glUniform4i(fallback_glUniform4i),
	glUniform4iARB(fallback_glUniform4iARB),
	glUniform4iv(fallback_glUniform4iv),
	glUniform4ivARB(fallback_glUniform4ivARB),
	glUniform4ui(fallback_glUniform4ui),
	glUniform4uiEXT(fallback_glUniform4uiEXT),
	glUniform4uiv(fallback_glUniform4uiv),
	glUniform4uivEXT(fallback_glUniform4uivEXT),
	glUniformBlockBinding(fallback_glUniformBlockBinding),
	glUniformBufferEXT(fallback_glUniformBufferEXT),
	glUniformMatrix2dv(fallback_glUniformMatrix2dv),
	glUniformMatrix2fv(fallback_glUniformMatrix2fv),
	glUniformMatrix2fvARB(fallback_glUniformMatrix2fvARB),
	glUniformMatrix2x3dv(fallback_glUniformMatrix2x3dv),
	glUniformMatrix2x3fv(fallback_glUniformMatrix2x3fv),
	glUniformMatrix2x4dv(fallback_glUniformMatrix2x4dv),
	glUniformMatrix2x4fv(fallback_glUniformMatrix2x4fv),
	glUniformMatrix3dv(fallback_glUniformMatrix3dv),
	glUniformMatrix3fv(fallback_glUniformMatrix3fv),
	glUniformMatrix3fvARB(fallback_glUniformMatrix3fvARB),
	glUniformMatrix3x2dv(fallback_glUniformMatrix3x2dv),
	glUniformMatrix3x2fv(fallback_glUniformMatrix3x2fv),
	glUniformMatrix3x4dv(fallback_glUniformMatrix3x4dv),
	glUniformMatrix3x4fv(fallback_glUniformMatrix3x4fv),
	glUniformMatrix4dv(fallback_glUniformMatrix4dv),
	glUniformMatrix4fv(fallback_glUniformMatrix4fv),
	glUniformMatrix4fvARB(fallback_glUniformMatrix4fvARB),
	glUniformMatrix4x2dv(fallback_glUniformMatrix4x2dv),
	glUniformMatrix4x2fv(fallback_glUniformMatrix4x2fv),
	glUniformMatrix4x3dv(fallback_glUniformMatrix4x3dv),
	glUniformMatrix4x3fv(fallback_glUniformMatrix4x3fv),
	glUniformSubroutinesuiv(fallback_glUniformSubroutinesuiv),
	glUniformui64NV(fallback_glUniformui64NV),
	glUniformui64vNV(fallback_glUniformui64vNV),
	glUnlockArraysEXT(fallback_glUnlockArraysEXT),
	glUnmapBuffer(fallback_glUnmapBuffer),
	glUnmapBufferARB(fallback_glUnmapBufferARB),
	glUnmapNamedBufferEXT(fallback_glUnmapNamedBufferEXT),
	glUnmapObjectBufferATI(fallback_glUnmapObjectBufferATI),
	glUpdateObjectBufferATI(fallback_glUpdateObjectBufferATI),
	glUseProgram(fallback_glUseProgram),
	glUseProgramObjectARB(fallback_glUseProgramObjectARB),
	glUseShaderProgramEXT(fallback_glUseShaderProgramEXT),
	glValidateProgram(fallback_glValidateProgram),
	glValidateProgramARB(fallback_glValidateProgramARB),
	glVariantArrayObjectATI(fallback_glVariantArrayObjectATI),
	glVariantPointerEXT(fallback_glVariantPointerEXT),
	glVariantbvEXT(fallback_glVariantbvEXT),
	glVariantdvEXT(fallback_glVariantdvEXT),
	glVariantfvEXT(fallback_glVariantfvEXT),
	glVariantivEXT(fallback_glVariantivEXT),
	glVariantsvEXT(fallback_glVariantsvEXT),
	glVariantubvEXT(fallback_glVariantubvEXT),
	glVariantuivEXT(fallback_glVariantuivEXT),
	glVariantusvEXT(fallback_glVariantusvEXT),
	glVertex2d(fallback_glVertex2d),
	glVertex2dv(fallback_glVertex2dv),
	glVertex2f(fallback_glVertex2f),
	glVertex2fv(fallback_glVertex2fv),
	glVertex2hNV(fallback_glVertex2hNV),
	glVertex2hvNV(fallback_glVertex2hvNV),
	glVertex2i(fallback_glVertex2i),
	glVertex2iv(fallback_glVertex2iv),
	glVertex2s(fallback_glVertex2s),
	glVertex2sv(fallback_glVertex2sv),
	glVertex3d(fallback_glVertex3d),
	glVertex3dv(fallback_glVertex3dv),
	glVertex3f(fallback_glVertex3f),
	glVertex3fv(fallback_glVertex3fv),
	glVertex3hNV(fallback_glVertex3hNV),
	glVertex3hvNV(fallback_glVertex3hvNV),
	glVertex3i(fallback_glVertex3i),
	glVertex3iv(fallback_glVertex3iv),
	glVertex3s(fallback_glVertex3s),
	glVertex3sv(fallback_glVertex3sv),
	glVertex4d(fallback_glVertex4d),
	glVertex4dv(fallback_glVertex4dv),
	glVertex4f(fallback_glVertex4f),
	glVertex4fv(fallback_glVertex4fv),
	glVertex4hNV(fallback_glVertex4hNV),
	glVertex4hvNV(fallback_glVertex4hvNV),
	glVertex4i(fallback_glVertex4i),
	glVertex4iv(fallback_glVertex4iv),
	glVertex4s(fallback_glVertex4s),
	glVertex4sv(fallback_glVertex4sv),
	glVertexArrayParameteriAPPLE(fallback_glVertexArrayParameteriAPPLE),
	glVertexArrayRangeAPPLE(fallback_glVertexArrayRangeAPPLE),
	glVertexArrayRangeNV(fallback_glVertexArrayRangeNV),
	glVertexAttrib1d(fallback_glVertexAttrib1d),
	glVertexAttrib1dARB(fallback_glVertexAttrib1dARB),
	glVertexAttrib1dNV(fallback_glVertexAttrib1dNV),
	glVertexAttrib1dv(fallback_glVertexAttrib1dv),
	glVertexAttrib1dvARB(fallback_glVertexAttrib1dvARB),
	glVertexAttrib1dvNV(fallback_glVertexAttrib1dvNV),
	glVertexAttrib1f(fallback_glVertexAttrib1f),
	glVertexAttrib1fARB(fallback_glVertexAttrib1fARB),
	glVertexAttrib1fNV(fallback_glVertexAttrib1fNV),
	glVertexAttrib1fv(fallback_glVertexAttrib1fv),
	glVertexAttrib1fvARB(fallback_glVertexAttrib1fvARB),
	glVertexAttrib1fvNV(fallback_glVertexAttrib1fvNV),
	glVertexAttrib1hNV(fallback_glVertexAttrib1hNV),
	glVertexAttrib1hvNV(fallback_glVertexAttrib1hvNV),
	glVertexAttrib1s(fallback_glVertexAttrib1s),
	glVertexAttrib1sARB(fallback_glVertexAttrib1sARB),
	glVertexAttrib1sNV(fallback_glVertexAttrib1sNV),
	glVertexAttrib1sv(fallback_glVertexAttrib1sv),
	glVertexAttrib1svARB(fallback_glVertexAttrib1svARB),
	glVertexAttrib1svNV(fallback_glVertexAttrib1svNV),
	glVertexAttrib2d(fallback_glVertexAttrib2d),
	glVertexAttrib2dARB(fallback_glVertexAttrib2dARB),
	glVertexAttrib2dNV(fallback_glVertexAttrib2dNV),
	glVertexAttrib2dv(fallback_glVertexAttrib2dv),
	glVertexAttrib2dvARB(fallback_glVertexAttrib2dvARB),
	glVertexAttrib2dvNV(fallback_glVertexAttrib2dvNV),
	glVertexAttrib2f(fallback_glVertexAttrib2f),
	glVertexAttrib2fARB(fallback_glVertexAttrib2fARB),
	glVertexAttrib2fNV(fallback_glVertexAttrib2fNV),
	glVertexAttrib2fv(fallback_glVertexAttrib2fv),
	glVertexAttrib2fvARB(fallback_glVertexAttrib2fvARB),
	glVertexAttrib2fvNV(fallback_glVertexAttrib2fvNV),
	glVertexAttrib2hNV(fallback_glVertexAttrib2hNV),
	glVertexAttrib2hvNV(fallback_glVertexAttrib2hvNV),
	glVertexAttrib2s(fallback_glVertexAttrib2s),
	glVertexAttrib2sARB(fallback_glVertexAttrib2sARB),
	glVertexAttrib2sNV(fallback_glVertexAttrib2sNV),
	glVertexAttrib2sv(fallback_glVertexAttrib2sv),
	glVertexAttrib2svARB(fallback_glVertexAttrib2svARB),
	glVertexAttrib2svNV(fallback_glVertexAttrib2svNV),
	glVertexAttrib3d(fallback_glVertexAttrib3d),
	glVertexAttrib3dARB(fallback_glVertexAttrib3dARB),
	glVertexAttrib3dNV(fallback_glVertexAttrib3dNV),
	glVertexAttrib3dv(fallback_glVertexAttrib3dv),
	glVertexAttrib3dvARB(fallback_glVertexAttrib3dvARB),
	glVertexAttrib3dvNV(fallback_glVertexAttrib3dvNV),
	glVertexAttrib3f(fallback_glVertexAttrib3f),
	glVertexAttrib3fARB(fallback_glVertexAttrib3fARB),
	glVertexAttrib3fNV(fallback_glVertexAttrib3fNV),
	glVertexAttrib3fv(fallback_glVertexAttrib3fv),
	glVertexAttrib3fvARB(fallback_glVertexAttrib3fvARB),
	glVertexAttrib3fvNV(fallback_glVertexAttrib3fvNV),
	glVertexAttrib3hNV(fallback_glVertexAttrib3hNV),
	glVertexAttrib3hvNV(fallback_glVertexAttrib3hvNV),
	glVertexAttrib3s(fallback_glVertexAttrib3s),
	glVertexAttrib3sARB(fallback_glVertexAttrib3sARB),
	glVertexAttrib3sNV(fallback_glVertexAttrib3sNV),
	glVertexAttrib3sv(fallback_glVertexAttrib3sv),
	glVertexAttrib3svARB(fallback_glVertexAttrib3svARB),
	glVertexAttrib3svNV(fallback_glVertexAttrib3svNV),
	glVertexAttrib4Nbv(fallback_glVertexAttrib4Nbv),
	glVertexAttrib4NbvARB(fallback_glVertexAttrib4NbvARB),
	glVertexAttrib4Niv(fallback_glVertexAttrib4Niv),
	glVertexAttrib4NivARB(fallback_glVertexAttrib4NivARB),
	glVertexAttrib4Nsv(fallback_glVertexAttrib4Nsv),
	glVertexAttrib4NsvARB(fallback_glVertexAttrib4NsvARB),
	glVertexAttrib4Nub(fallback_glVertexAttrib4Nub),
	glVertexAttrib4NubARB(fallback_glVertexAttrib4NubARB),
	glVertexAttrib4Nubv(fallback_glVertexAttrib4Nubv),
	glVertexAttrib4NubvARB(fallback_glVertexAttrib4NubvARB),
	glVertexAttrib4Nuiv(fallback_glVertexAttrib4Nuiv),
	glVertexAttrib4NuivARB(fallback_glVertexAttrib4NuivARB),
	glVertexAttrib4Nusv(fallback_glVertexAttrib4Nusv),
	glVertexAttrib4NusvARB(fallback_glVertexAttrib4NusvARB),
	glVertexAttrib4bv(fallback_glVertexAttrib4bv),
	glVertexAttrib4bvARB(fallback_glVertexAttrib4bvARB),
	glVertexAttrib4d(fallback_glVertexAttrib4d),
	glVertexAttrib4dARB(fallback_glVertexAttrib4dARB),
	glVertexAttrib4dNV(fallback_glVertexAttrib4dNV),
	glVertexAttrib4dv(fallback_glVertexAttrib4dv),
	glVertexAttrib4dvARB(fallback_glVertexAttrib4dvARB),
	glVertexAttrib4dvNV(fallback_glVertexAttrib4dvNV),
	glVertexAttrib4f(fallback_glVertexAttrib4f),
	glVertexAttrib4fARB(fallback_glVertexAttrib4fARB),
	glVertexAttrib4fNV(fallback_glVertexAttrib4fNV),
	glVertexAttrib4fv(fallback_glVertexAttrib4fv),
	glVertexAttrib4fvARB(fallback_glVertexAttrib4fvARB),
	glVertexAttrib4fvNV(fallback_glVertexAttrib4fvNV),
	glVertexAttrib4hNV(fallback_glVertexAttrib4hNV),
	glVertexAttrib4hvNV(fallback_glVertexAttrib4hvNV),
	glVertexAttrib4iv(fallback_glVertexAttrib4iv),
	glVertexAttrib4ivARB(fallback_glVertexAttrib4ivARB),
	glVertexAttrib4s(fallback_glVertexAttrib4s),
	glVertexAttrib4sARB(fallback_glVertexAttrib4sARB),
	glVertexAttrib4sNV(fallback_glVertexAttrib4sNV),
	glVertexAttrib4sv(fallback_glVertexAttrib4sv),
	glVertexAttrib4svARB(fallback_glVertexAttrib4svARB),
	glVertexAttrib4svNV(fallback_glVertexAttrib4svNV),
	glVertexAttrib4ubNV(fallback_glVertexAttrib4ubNV),
	glVertexAttrib4ubv(fallback_glVertexAttrib4ubv),
	glVertexAttrib4ubvARB(fallback_glVertexAttrib4ubvARB),
	glVertexAttrib4ubvNV(fallback_glVertexAttrib4ubvNV),
	glVertexAttrib4uiv(fallback_glVertexAttrib4uiv),
	glVertexAttrib4uivARB(fallback_glVertexAttrib4uivARB),
	glVertexAttrib4usv(fallback_glVertexAttrib4usv),
	glVertexAttrib4usvARB(fallback_glVertexAttrib4usvARB),
	glVertexAttribArrayObjectATI(fallback_glVertexAttribArrayObjectATI),
	glVertexAttribDivisorARB(fallback_glVertexAttribDivisorARB),
	glVertexAttribFormatNV(fallback_glVertexAttribFormatNV),
	glVertexAttribI1i(fallback_glVertexAttribI1i),
	glVertexAttribI1iEXT(fallback_glVertexAttribI1iEXT),
	glVertexAttribI1iv(fallback_glVertexAttribI1iv),
	glVertexAttribI1ivEXT(fallback_glVertexAttribI1ivEXT),
	glVertexAttribI1ui(fallback_glVertexAttribI1ui),
	glVertexAttribI1uiEXT(fallback_glVertexAttribI1uiEXT),
	glVertexAttribI1uiv(fallback_glVertexAttribI1uiv),
	glVertexAttribI1uivEXT(fallback_glVertexAttribI1uivEXT),
	glVertexAttribI2i(fallback_glVertexAttribI2i),
	glVertexAttribI2iEXT(fallback_glVertexAttribI2iEXT),
	glVertexAttribI2iv(fallback_glVertexAttribI2iv),
	glVertexAttribI2ivEXT(fallback_glVertexAttribI2ivEXT),
	glVertexAttribI2ui(fallback_glVertexAttribI2ui),
	glVertexAttribI2uiEXT(fallback_glVertexAttribI2uiEXT),
	glVertexAttribI2uiv(fallback_glVertexAttribI2uiv),
	glVertexAttribI2uivEXT(fallback_glVertexAttribI2uivEXT),
	glVertexAttribI3i(fallback_glVertexAttribI3i),
	glVertexAttribI3iEXT(fallback_glVertexAttribI3iEXT),
	glVertexAttribI3iv(fallback_glVertexAttribI3iv),
	glVertexAttribI3ivEXT(fallback_glVertexAttribI3ivEXT),
	glVertexAttribI3ui(fallback_glVertexAttribI3ui),
	glVertexAttribI3uiEXT(fallback_glVertexAttribI3uiEXT),
	glVertexAttribI3uiv(fallback_glVertexAttribI3uiv),
	glVertexAttribI3uivEXT(fallback_glVertexAttribI3uivEXT),
	glVertexAttribI4bv(fallback_glVertexAttribI4bv),
	glVertexAttribI4bvEXT(fallback_glVertexAttribI4bvEXT),
	glVertexAttribI4i(fallback_glVertexAttribI4i),
	glVertexAttribI4iEXT(fallback_glVertexAttribI4iEXT),
	glVertexAttribI4iv(fallback_glVertexAttribI4iv),
	glVertexAttribI4ivEXT(fallback_glVertexAttribI4ivEXT),
	glVertexAttribI4sv(fallback_glVertexAttribI4sv),
	glVertexAttribI4svEXT(fallback_glVertexAttribI4svEXT),
	glVertexAttribI4ubv(fallback_glVertexAttribI4ubv),
	glVertexAttribI4ubvEXT(fallback_glVertexAttribI4ubvEXT),
	glVertexAttribI4ui(fallback_glVertexAttribI4ui),
	glVertexAttribI4uiEXT(fallback_glVertexAttribI4uiEXT),
	glVertexAttribI4uiv(fallback_glVertexAttribI4uiv),
	glVertexAttribI4uivEXT(fallback_glVertexAttribI4uivEXT),
	glVertexAttribI4usv(fallback_glVertexAttribI4usv),
	glVertexAttribI4usvEXT(fallback_glVertexAttribI4usvEXT),
	glVertexAttribIFormatNV(fallback_glVertexAttribIFormatNV),
	glVertexAttribIPointer(fallback_glVertexAttribIPointer),
	glVertexAttribIPointerEXT(fallback_glVertexAttribIPointerEXT),
	glVertexAttribP1ui(fallback_glVertexAttribP1ui),
	glVertexAttribP1uiv(fallback_glVertexAttribP1uiv),
	glVertexAttribP2ui(fallback_glVertexAttribP2ui),
	glVertexAttribP2uiv(fallback_glVertexAttribP2uiv),
	glVertexAttribP3ui(fallback_glVertexAttribP3ui),
	glVertexAttribP3uiv(fallback_glVertexAttribP3uiv),
	glVertexAttribP4ui(fallback_glVertexAttribP4ui),
	glVertexAttribP4uiv(fallback_glVertexAttribP4uiv),
	glVertexAttribPointer(fallback_glVertexAttribPointer),
	glVertexAttribPointerARB(fallback_glVertexAttribPointerARB),
	glVertexAttribPointerNV(fallback_glVertexAttribPointerNV),
	glVertexAttribs1dvNV(fallback_glVertexAttribs1dvNV),
	glVertexAttribs1fvNV(fallback_glVertexAttribs1fvNV),
	glVertexAttribs1hvNV(fallback_glVertexAttribs1hvNV),
	glVertexAttribs1svNV(fallback_glVertexAttribs1svNV),
	glVertexAttribs2dvNV(fallback_glVertexAttribs2dvNV),
	glVertexAttribs2fvNV(fallback_glVertexAttribs2fvNV),
	glVertexAttribs2hvNV(fallback_glVertexAttribs2hvNV),
	glVertexAttribs2svNV(fallback_glVertexAttribs2svNV),
	glVertexAttribs3dvNV(fallback_glVertexAttribs3dvNV),
	glVertexAttribs3fvNV(fallback_glVertexAttribs3fvNV),
	glVertexAttribs3hvNV(fallback_glVertexAttribs3hvNV),
	glVertexAttribs3svNV(fallback_glVertexAttribs3svNV),
	glVertexAttribs4dvNV(fallback_glVertexAttribs4dvNV),
	glVertexAttribs4fvNV(fallback_glVertexAttribs4fvNV),
	glVertexAttribs4hvNV(fallback_glVertexAttribs4hvNV),
	glVertexAttribs4svNV(fallback_glVertexAttribs4svNV),
	glVertexAttribs4ubvNV(fallback_glVertexAttribs4ubvNV),
	glVertexBlendARB(fallback_glVertexBlendARB),
	glVertexBlendEnvfATI(fallback_glVertexBlendEnvfATI),
	glVertexBlendEnviATI(fallback_glVertexBlendEnviATI),
	glVertexFormatNV(fallback_glVertexFormatNV),
	glVertexP2ui(fallback_glVertexP2ui),
	glVertexP2uiv(fallback_glVertexP2uiv),
	glVertexP3ui(fallback_glVertexP3ui),
	glVertexP3uiv(fallback_glVertexP3uiv),
	glVertexP4ui(fallback_glVertexP4ui),
	glVertexP4uiv(fallback_glVertexP4uiv),
	glVertexPointer(fallback_glVertexPointer),
	glVertexPointerEXT(fallback_glVertexPointerEXT),
	glVertexPointerListIBM(fallback_glVertexPointerListIBM),
	glVertexPointervINTEL(fallback_glVertexPointervINTEL),
	glVertexStream1dATI(fallback_glVertexStream1dATI),
	glVertexStream1dvATI(fallback_glVertexStream1dvATI),
	glVertexStream1fATI(fallback_glVertexStream1fATI),
	glVertexStream1fvATI(fallback_glVertexStream1fvATI),
	glVertexStream1iATI(fallback_glVertexStream1iATI),
	glVertexStream1ivATI(fallback_glVertexStream1ivATI),
	glVertexStream1sATI(fallback_glVertexStream1sATI),
	glVertexStream1svATI(fallback_glVertexStream1svATI),
	glVertexStream2dATI(fallback_glVertexStream2dATI),
	glVertexStream2dvATI(fallback_glVertexStream2dvATI),
	glVertexStream2fATI(fallback_glVertexStream2fATI),
	glVertexStream2fvATI(fallback_glVertexStream2fvATI),
	glVertexStream2iATI(fallback_glVertexStream2iATI),
	glVertexStream2ivATI(fallback_glVertexStream2ivATI),
	glVertexStream2sATI(fallback_glVertexStream2sATI),
	glVertexStream2svATI(fallback_glVertexStream2svATI),
	glVertexStream3dATI(fallback_glVertexStream3dATI),
	glVertexStream3dvATI(fallback_glVertexStream3dvATI),
	glVertexStream3fATI(fallback_glVertexStream3fATI),
	glVertexStream3fvATI(fallback_glVertexStream3fvATI),
	glVertexStream3iATI(fallback_glVertexStream3iATI),
	glVertexStream3ivATI(fallback_glVertexStream3ivATI),
	glVertexStream3sATI(fallback_glVertexStream3sATI),
	glVertexStream3svATI(fallback_glVertexStream3svATI),
	glVertexStream4dATI(fallback_glVertexStream4dATI),
	glVertexStream4dvATI(fallback_glVertexStream4dvATI),
	glVertexStream4fATI(fallback_glVertexStream4fATI),
	glVertexStream4fvATI(fallback_glVertexStream4fvATI),
	glVertexStream4iATI(fallback_glVertexStream4iATI),
	glVertexStream4ivATI(fallback_glVertexStream4ivATI),
	glVertexStream4sATI(fallback_glVertexStream4sATI),
	glVertexStream4svATI(fallback_glVertexStream4svATI),
	glVertexWeightPointerEXT(fallback_glVertexWeightPointerEXT),
	glVertexWeightfEXT(fallback_glVertexWeightfEXT),
	glVertexWeightfvEXT(fallback_glVertexWeightfvEXT),
	glVertexWeighthNV(fallback_glVertexWeighthNV),
	glVertexWeighthvNV(fallback_glVertexWeighthvNV),
	glVideoCaptureNV(fallback_glVideoCaptureNV),
	glVideoCaptureStreamParameterdvNV(fallback_glVideoCaptureStreamParameterdvNV),
	glVideoCaptureStreamParameterfvNV(fallback_glVideoCaptureStreamParameterfvNV),
	glVideoCaptureStreamParameterivNV(fallback_glVideoCaptureStreamParameterivNV),
	glViewport(fallback_glViewport),
	glWaitSync(fallback_glWaitSync),
	glWeightPointerARB(fallback_glWeightPointerARB),
	glWeightbvARB(fallback_glWeightbvARB),
	glWeightdvARB(fallback_glWeightdvARB),
	glWeightfvARB(fallback_glWeightfvARB),
	glWeightivARB(fallback_glWeightivARB),
	glWeightsvARB(fallback_glWeightsvARB),
	glWeightubvARB(fallback_glWeightubvARB),
	glWeightuivARB(fallback_glWeightuivARB),
	glWeightusvARB(fallback_glWeightusvARB),
	glWindowPos2d(fallback_glWindowPos2d),
	glWindowPos2dARB(fallback_glWindowPos2dARB),
	glWindowPos2dMESA(fallback_glWindowPos2dMESA),
	glWindowPos2dv(fallback_glWindowPos2dv),
	glWindowPos2dvARB(fallback_glWindowPos2dvARB),
	glWindowPos2dvMESA(fallback_glWindowPos2dvMESA),
	glWindowPos2f(fallback_glWindowPos2f),
	glWindowPos2fARB(fallback_glWindowPos2fARB),
	glWindowPos2fMESA(fallback_glWindowPos2fMESA),
	glWindowPos2fv(fallback_glWindowPos2fv),
	glWindowPos2fvARB(fallback_glWindowPos2fvARB),
	glWindowPos2fvMESA(fallback_glWindowPos2fvMESA),
	glWindowPos2i(fallback_glWindowPos2i),
	glWindowPos2iARB(fallback_glWindowPos2iARB),
	glWindowPos2iMESA(fallback_glWindowPos2iMESA),
	glWindowPos2iv(fallback_glWindowPos2iv),
	glWindowPos2ivARB(fallback_glWindowPos2ivARB),
	glWindowPos2ivMESA(fallback_glWindowPos2ivMESA),
	glWindowPos2s(fallback_glWindowPos2s),
	glWindowPos2sARB(fallback_glWindowPos2sARB),
	glWindowPos2sMESA(fallback_glWindowPos2sMESA),
	glWindowPos2sv(fallback_glWindowPos2sv),
	glWindowPos2svARB(fallback_glWindowPos2svARB),
	glWindowPos2svMESA(fallback_glWindowPos2svMESA),
	glWindowPos3d(fallback_glWindowPos3d),
	glWindowPos3dARB(fallback_glWindowPos3dARB),
	glWindowPos3dMESA(fallback_glWindowPos3dMESA),
	glWindowPos3dv(fallback_glWindowPos3dv),
	glWindowPos3dvARB(fallback_glWindowPos3dvARB),
	glWindowPos3dvMESA(fallback_glWindowPos3dvMESA),
	glWindowPos3f(fallback_glWindowPos3f),
	glWindowPos3fARB(fallback_glWindowPos3fARB),
	glWindowPos3fMESA(fallback_glWindowPos3fMESA),
	glWindowPos3fv(fallback_glWindowPos3fv),
	glWindowPos3fvARB(fallback_glWindowPos3fvARB),
	glWindowPos3fvMESA(fallback_glWindowPos3fvMESA),
	glWindowPos3i(fallback_glWindowPos3i),
	glWindowPos3iARB(fallback_glWindowPos3iARB),
	glWindowPos3iMESA(fallback_glWindowPos3iMESA),
	glWindowPos3iv(fallback_glWindowPos3iv),
	glWindowPos3ivARB(fallback_glWindowPos3ivARB),
	glWindowPos3ivMESA(fallback_glWindowPos3ivMESA),
	glWindowPos3s(fallback_glWindowPos3s),
	glWindowPos3sARB(fallback_glWindowPos3sARB),
	glWindowPos3sMESA(fallback_glWindowPos3sMESA),
	glWindowPos3sv(fallback_glWindowPos3sv),
	glWindowPos3svARB(fallback_glWindowPos3svARB),
	glWindowPos3svMESA(fallback_glWindowPos3svMESA),
	glWindowPos4dMESA(fallback_glWindowPos4dMESA),
	glWindowPos4dvMESA(fallback_glWindowPos4dvMESA),
	glWindowPos4fMESA(fallback_glWindowPos4fMESA),
	glWindowPos4fvMESA(fallback_glWindowPos4fvMESA),
	glWindowPos4iMESA(fallback_glWindowPos4iMESA),
	glWindowPos4ivMESA(fallback_glWindowPos4ivMESA),
	glWindowPos4sMESA(fallback_glWindowPos4sMESA),
	glWindowPos4svMESA(fallback_glWindowPos4svMESA),
	glWriteMaskEXT(fallback_glWriteMaskEXT)
{
}

} // namespace gl

} // namespace k3d
