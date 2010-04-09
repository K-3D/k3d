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

static void fallbackAccum(api::GLenum op, api::GLfloat value)
{
	log() << warning << "Accum not available." << std::endl;
}

static void fallbackActiveProgramEXT(api::GLuint program)
{
	log() << warning << "ActiveProgramEXT not available." << std::endl;
}

static void fallbackActiveStencilFaceEXT(api::GLenum face)
{
	log() << warning << "ActiveStencilFaceEXT not available." << std::endl;
}

static void fallbackActiveTexture(api::GLenum texture)
{
	log() << warning << "ActiveTexture not available." << std::endl;
}

static void fallbackActiveTextureARB(api::GLenum texture)
{
	log() << warning << "ActiveTextureARB not available." << std::endl;
}

static void fallbackActiveVaryingNV(api::GLuint program, const api::GLchar* name)
{
	log() << warning << "ActiveVaryingNV not available." << std::endl;
}

static void fallbackAlphaFragmentOp1ATI(api::GLenum op, api::GLuint dst, api::GLuint dstMod, api::GLuint arg1, api::GLuint arg1Rep, api::GLuint arg1Mod)
{
	log() << warning << "AlphaFragmentOp1ATI not available." << std::endl;
}

static void fallbackAlphaFragmentOp2ATI(api::GLenum op, api::GLuint dst, api::GLuint dstMod, api::GLuint arg1, api::GLuint arg1Rep, api::GLuint arg1Mod, api::GLuint arg2, api::GLuint arg2Rep, api::GLuint arg2Mod)
{
	log() << warning << "AlphaFragmentOp2ATI not available." << std::endl;
}

static void fallbackAlphaFragmentOp3ATI(api::GLenum op, api::GLuint dst, api::GLuint dstMod, api::GLuint arg1, api::GLuint arg1Rep, api::GLuint arg1Mod, api::GLuint arg2, api::GLuint arg2Rep, api::GLuint arg2Mod, api::GLuint arg3, api::GLuint arg3Rep, api::GLuint arg3Mod)
{
	log() << warning << "AlphaFragmentOp3ATI not available." << std::endl;
}

static void fallbackAlphaFunc(api::GLenum func, api::GLclampf ref)
{
	log() << warning << "AlphaFunc not available." << std::endl;
}

static void fallbackApplyTextureEXT(api::GLenum mode)
{
	log() << warning << "ApplyTextureEXT not available." << std::endl;
}

static api::GLboolean fallbackAreProgramsResidentNV(api::GLsizei n, const api::GLuint* programs, api::GLboolean* residences)
{
	log() << warning << "AreProgramsResidentNV not available." << std::endl;
}

static api::GLboolean fallbackAreTexturesResident(api::GLsizei n, const api::GLuint* textures, api::GLboolean* residences)
{
	log() << warning << "AreTexturesResident not available." << std::endl;
}

static api::GLboolean fallbackAreTexturesResidentEXT(api::GLsizei n, const api::GLuint* textures, api::GLboolean* residences)
{
	log() << warning << "AreTexturesResidentEXT not available." << std::endl;
}

static void fallbackArrayElement(api::GLint i)
{
	log() << warning << "ArrayElement not available." << std::endl;
}

static void fallbackArrayElementEXT(api::GLint i)
{
	log() << warning << "ArrayElementEXT not available." << std::endl;
}

static void fallbackArrayObjectATI(api::GLenum array, api::GLint size, api::GLenum type, api::GLsizei stride, api::GLuint buffer, api::GLuint offset)
{
	log() << warning << "ArrayObjectATI not available." << std::endl;
}

static void fallbackAsyncMarkerSGIX(api::GLuint marker)
{
	log() << warning << "AsyncMarkerSGIX not available." << std::endl;
}

static void fallbackAttachObjectARB(api::GLhandleARB containerObj, api::GLhandleARB obj)
{
	log() << warning << "AttachObjectARB not available." << std::endl;
}

static void fallbackAttachShader(api::GLuint program, api::GLuint shader)
{
	log() << warning << "AttachShader not available." << std::endl;
}

static void fallbackBegin(api::GLenum mode)
{
	log() << warning << "Begin not available." << std::endl;
}

static void fallbackBeginConditionalRender(api::GLuint id, api::GLenum mode)
{
	log() << warning << "BeginConditionalRender not available." << std::endl;
}

static void fallbackBeginConditionalRenderNV(api::GLuint id, api::GLenum mode)
{
	log() << warning << "BeginConditionalRenderNV not available." << std::endl;
}

static void fallbackBeginFragmentShaderATI()
{
	log() << warning << "BeginFragmentShaderATI not available." << std::endl;
}

static void fallbackBeginOcclusionQueryNV(api::GLuint id)
{
	log() << warning << "BeginOcclusionQueryNV not available." << std::endl;
}

static void fallbackBeginPerfMonitorAMD(api::GLuint monitor)
{
	log() << warning << "BeginPerfMonitorAMD not available." << std::endl;
}

static void fallbackBeginQuery(api::GLenum target, api::GLuint id)
{
	log() << warning << "BeginQuery not available." << std::endl;
}

static void fallbackBeginQueryARB(api::GLenum target, api::GLuint id)
{
	log() << warning << "BeginQueryARB not available." << std::endl;
}

static void fallbackBeginQueryIndexed(api::GLenum target, api::GLuint index, api::GLuint id)
{
	log() << warning << "BeginQueryIndexed not available." << std::endl;
}

static void fallbackBeginTransformFeedback(api::GLenum primitiveMode)
{
	log() << warning << "BeginTransformFeedback not available." << std::endl;
}

static void fallbackBeginTransformFeedbackEXT(api::GLenum primitiveMode)
{
	log() << warning << "BeginTransformFeedbackEXT not available." << std::endl;
}

static void fallbackBeginTransformFeedbackNV(api::GLenum primitiveMode)
{
	log() << warning << "BeginTransformFeedbackNV not available." << std::endl;
}

static void fallbackBeginVertexShaderEXT()
{
	log() << warning << "BeginVertexShaderEXT not available." << std::endl;
}

static void fallbackBeginVideoCaptureNV(api::GLuint video_capture_slot)
{
	log() << warning << "BeginVideoCaptureNV not available." << std::endl;
}

static void fallbackBindAttribLocation(api::GLuint program, api::GLuint index, const api::GLchar* name)
{
	log() << warning << "BindAttribLocation not available." << std::endl;
}

static void fallbackBindAttribLocationARB(api::GLhandleARB programObj, api::GLuint index, const api::GLcharARB* name)
{
	log() << warning << "BindAttribLocationARB not available." << std::endl;
}

static void fallbackBindBuffer(api::GLenum target, api::GLuint buffer)
{
	log() << warning << "BindBuffer not available." << std::endl;
}

static void fallbackBindBufferARB(api::GLenum target, api::GLuint buffer)
{
	log() << warning << "BindBufferARB not available." << std::endl;
}

static void fallbackBindBufferBase(api::GLenum target, api::GLuint index, api::GLuint buffer)
{
	log() << warning << "BindBufferBase not available." << std::endl;
}

static void fallbackBindBufferBaseEXT(api::GLenum target, api::GLuint index, api::GLuint buffer)
{
	log() << warning << "BindBufferBaseEXT not available." << std::endl;
}

static void fallbackBindBufferBaseNV(api::GLenum target, api::GLuint index, api::GLuint buffer)
{
	log() << warning << "BindBufferBaseNV not available." << std::endl;
}

static void fallbackBindBufferOffsetEXT(api::GLenum target, api::GLuint index, api::GLuint buffer, api::GLintptr offset)
{
	log() << warning << "BindBufferOffsetEXT not available." << std::endl;
}

static void fallbackBindBufferOffsetNV(api::GLenum target, api::GLuint index, api::GLuint buffer, api::GLintptr offset)
{
	log() << warning << "BindBufferOffsetNV not available." << std::endl;
}

static void fallbackBindBufferRange(api::GLenum target, api::GLuint index, api::GLuint buffer, api::GLintptr offset, api::GLsizeiptr size)
{
	log() << warning << "BindBufferRange not available." << std::endl;
}

static void fallbackBindBufferRangeEXT(api::GLenum target, api::GLuint index, api::GLuint buffer, api::GLintptr offset, api::GLsizeiptr size)
{
	log() << warning << "BindBufferRangeEXT not available." << std::endl;
}

static void fallbackBindBufferRangeNV(api::GLenum target, api::GLuint index, api::GLuint buffer, api::GLintptr offset, api::GLsizeiptr size)
{
	log() << warning << "BindBufferRangeNV not available." << std::endl;
}

static void fallbackBindFragDataLocation(api::GLuint program, api::GLuint color, const api::GLchar* name)
{
	log() << warning << "BindFragDataLocation not available." << std::endl;
}

static void fallbackBindFragDataLocationEXT(api::GLuint program, api::GLuint color, const api::GLchar* name)
{
	log() << warning << "BindFragDataLocationEXT not available." << std::endl;
}

static void fallbackBindFragDataLocationIndexed(api::GLuint program, api::GLuint colorNumber, api::GLuint index, const api::GLchar* name)
{
	log() << warning << "BindFragDataLocationIndexed not available." << std::endl;
}

static void fallbackBindFragmentShaderATI(api::GLuint id)
{
	log() << warning << "BindFragmentShaderATI not available." << std::endl;
}

static void fallbackBindFramebuffer(api::GLenum target, api::GLuint framebuffer)
{
	log() << warning << "BindFramebuffer not available." << std::endl;
}

static void fallbackBindFramebufferEXT(api::GLenum target, api::GLuint framebuffer)
{
	log() << warning << "BindFramebufferEXT not available." << std::endl;
}

static api::GLuint fallbackBindLightParameterEXT(api::GLenum light, api::GLenum value)
{
	log() << warning << "BindLightParameterEXT not available." << std::endl;
}

static api::GLuint fallbackBindMaterialParameterEXT(api::GLenum face, api::GLenum value)
{
	log() << warning << "BindMaterialParameterEXT not available." << std::endl;
}

static void fallbackBindMultiTextureEXT(api::GLenum texunit, api::GLenum target, api::GLuint texture)
{
	log() << warning << "BindMultiTextureEXT not available." << std::endl;
}

static api::GLuint fallbackBindParameterEXT(api::GLenum value)
{
	log() << warning << "BindParameterEXT not available." << std::endl;
}

static void fallbackBindProgramARB(api::GLenum target, api::GLuint program)
{
	log() << warning << "BindProgramARB not available." << std::endl;
}

static void fallbackBindProgramNV(api::GLenum target, api::GLuint id)
{
	log() << warning << "BindProgramNV not available." << std::endl;
}

static void fallbackBindRenderbuffer(api::GLenum target, api::GLuint renderbuffer)
{
	log() << warning << "BindRenderbuffer not available." << std::endl;
}

static void fallbackBindRenderbufferEXT(api::GLenum target, api::GLuint renderbuffer)
{
	log() << warning << "BindRenderbufferEXT not available." << std::endl;
}

static void fallbackBindSampler(api::GLenum unit, api::GLuint sampler)
{
	log() << warning << "BindSampler not available." << std::endl;
}

static api::GLuint fallbackBindTexGenParameterEXT(api::GLenum unit, api::GLenum coord, api::GLenum value)
{
	log() << warning << "BindTexGenParameterEXT not available." << std::endl;
}

static void fallbackBindTexture(api::GLenum target, api::GLuint texture)
{
	log() << warning << "BindTexture not available." << std::endl;
}

static void fallbackBindTextureEXT(api::GLenum target, api::GLuint texture)
{
	log() << warning << "BindTextureEXT not available." << std::endl;
}

static api::GLuint fallbackBindTextureUnitParameterEXT(api::GLenum unit, api::GLenum value)
{
	log() << warning << "BindTextureUnitParameterEXT not available." << std::endl;
}

static void fallbackBindTransformFeedback(api::GLenum target, api::GLuint id)
{
	log() << warning << "BindTransformFeedback not available." << std::endl;
}

static void fallbackBindTransformFeedbackNV(api::GLenum target, api::GLuint id)
{
	log() << warning << "BindTransformFeedbackNV not available." << std::endl;
}

static void fallbackBindVertexArray(api::GLuint array)
{
	log() << warning << "BindVertexArray not available." << std::endl;
}

static void fallbackBindVertexArrayAPPLE(api::GLuint array)
{
	log() << warning << "BindVertexArrayAPPLE not available." << std::endl;
}

static void fallbackBindVertexShaderEXT(api::GLuint id)
{
	log() << warning << "BindVertexShaderEXT not available." << std::endl;
}

static void fallbackBindVideoCaptureStreamBufferNV(api::GLuint video_capture_slot, api::GLuint stream, api::GLenum frame_region, api::GLintptrARB offset)
{
	log() << warning << "BindVideoCaptureStreamBufferNV not available." << std::endl;
}

static void fallbackBindVideoCaptureStreamTextureNV(api::GLuint video_capture_slot, api::GLuint stream, api::GLenum frame_region, api::GLenum target, api::GLuint texture)
{
	log() << warning << "BindVideoCaptureStreamTextureNV not available." << std::endl;
}

static void fallbackBinormal3bEXT(api::GLbyte bx, api::GLbyte by, api::GLbyte bz)
{
	log() << warning << "Binormal3bEXT not available." << std::endl;
}

static void fallbackBinormal3bvEXT(const api::GLbyte* v)
{
	log() << warning << "Binormal3bvEXT not available." << std::endl;
}

static void fallbackBinormal3dEXT(api::GLdouble bx, api::GLdouble by, api::GLdouble bz)
{
	log() << warning << "Binormal3dEXT not available." << std::endl;
}

static void fallbackBinormal3dvEXT(const api::GLdouble* v)
{
	log() << warning << "Binormal3dvEXT not available." << std::endl;
}

static void fallbackBinormal3fEXT(api::GLfloat bx, api::GLfloat by, api::GLfloat bz)
{
	log() << warning << "Binormal3fEXT not available." << std::endl;
}

static void fallbackBinormal3fvEXT(const api::GLfloat* v)
{
	log() << warning << "Binormal3fvEXT not available." << std::endl;
}

static void fallbackBinormal3iEXT(api::GLint bx, api::GLint by, api::GLint bz)
{
	log() << warning << "Binormal3iEXT not available." << std::endl;
}

static void fallbackBinormal3ivEXT(const api::GLint* v)
{
	log() << warning << "Binormal3ivEXT not available." << std::endl;
}

static void fallbackBinormal3sEXT(api::GLshort bx, api::GLshort by, api::GLshort bz)
{
	log() << warning << "Binormal3sEXT not available." << std::endl;
}

static void fallbackBinormal3svEXT(const api::GLshort* v)
{
	log() << warning << "Binormal3svEXT not available." << std::endl;
}

static void fallbackBinormalPointerEXT(api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "BinormalPointerEXT not available." << std::endl;
}

static void fallbackBitmap(api::GLsizei width, api::GLsizei height, api::GLfloat xorig, api::GLfloat yorig, api::GLfloat xmove, api::GLfloat ymove, const api::GLubyte* bitmap)
{
	log() << warning << "Bitmap not available." << std::endl;
}

static void fallbackBlendColor(api::GLclampf red, api::GLclampf green, api::GLclampf blue, api::GLclampf alpha)
{
	log() << warning << "BlendColor not available." << std::endl;
}

static void fallbackBlendColorEXT(api::GLclampf red, api::GLclampf green, api::GLclampf blue, api::GLclampf alpha)
{
	log() << warning << "BlendColorEXT not available." << std::endl;
}

static void fallbackBlendEquation(api::GLenum mode)
{
	log() << warning << "BlendEquation not available." << std::endl;
}

static void fallbackBlendEquationEXT(api::GLenum mode)
{
	log() << warning << "BlendEquationEXT not available." << std::endl;
}

static void fallbackBlendEquationIndexedAMD(api::GLuint buf, api::GLenum mode)
{
	log() << warning << "BlendEquationIndexedAMD not available." << std::endl;
}

static void fallbackBlendEquationSeparate(api::GLenum modeRGB, api::GLenum modeAlpha)
{
	log() << warning << "BlendEquationSeparate not available." << std::endl;
}

static void fallbackBlendEquationSeparateEXT(api::GLenum modeRGB, api::GLenum modeAlpha)
{
	log() << warning << "BlendEquationSeparateEXT not available." << std::endl;
}

static void fallbackBlendEquationSeparateIndexedAMD(api::GLuint buf, api::GLenum modeRGB, api::GLenum modeAlpha)
{
	log() << warning << "BlendEquationSeparateIndexedAMD not available." << std::endl;
}

static void fallbackBlendEquationSeparatei(api::GLuint buf, api::GLenum modeRGB, api::GLenum modeAlpha)
{
	log() << warning << "BlendEquationSeparatei not available." << std::endl;
}

static void fallbackBlendEquationi(api::GLuint buf, api::GLenum mode)
{
	log() << warning << "BlendEquationi not available." << std::endl;
}

static void fallbackBlendFunc(api::GLenum sfactor, api::GLenum dfactor)
{
	log() << warning << "BlendFunc not available." << std::endl;
}

static void fallbackBlendFuncIndexedAMD(api::GLuint buf, api::GLenum src, api::GLenum dst)
{
	log() << warning << "BlendFuncIndexedAMD not available." << std::endl;
}

static void fallbackBlendFuncSeparate(api::GLenum sfactorRGB, api::GLenum dfactorRGB, api::GLenum sfactorAlpha, api::GLenum dfactorAlpha)
{
	log() << warning << "BlendFuncSeparate not available." << std::endl;
}

static void fallbackBlendFuncSeparateEXT(api::GLenum sfactorRGB, api::GLenum dfactorRGB, api::GLenum sfactorAlpha, api::GLenum dfactorAlpha)
{
	log() << warning << "BlendFuncSeparateEXT not available." << std::endl;
}

static void fallbackBlendFuncSeparateINGR(api::GLenum sfactorRGB, api::GLenum dfactorRGB, api::GLenum sfactorAlpha, api::GLenum dfactorAlpha)
{
	log() << warning << "BlendFuncSeparateINGR not available." << std::endl;
}

static void fallbackBlendFuncSeparateIndexedAMD(api::GLuint buf, api::GLenum srcRGB, api::GLenum dstRGB, api::GLenum srcAlpha, api::GLenum dstAlpha)
{
	log() << warning << "BlendFuncSeparateIndexedAMD not available." << std::endl;
}

static void fallbackBlendFuncSeparatei(api::GLuint buf, api::GLenum srcRGB, api::GLenum dstRGB, api::GLenum srcAlpha, api::GLenum dstAlpha)
{
	log() << warning << "BlendFuncSeparatei not available." << std::endl;
}

static void fallbackBlendFunci(api::GLuint buf, api::GLenum src, api::GLenum dst)
{
	log() << warning << "BlendFunci not available." << std::endl;
}

static void fallbackBlitFramebuffer(api::GLint srcX0, api::GLint srcY0, api::GLint srcX1, api::GLint srcY1, api::GLint dstX0, api::GLint dstY0, api::GLint dstX1, api::GLint dstY1, api::GLbitfield mask, api::GLenum filter)
{
	log() << warning << "BlitFramebuffer not available." << std::endl;
}

static void fallbackBlitFramebufferEXT(api::GLint srcX0, api::GLint srcY0, api::GLint srcX1, api::GLint srcY1, api::GLint dstX0, api::GLint dstY0, api::GLint dstX1, api::GLint dstY1, api::GLbitfield mask, api::GLenum filter)
{
	log() << warning << "BlitFramebufferEXT not available." << std::endl;
}

static void fallbackBufferAddressRangeNV(api::GLenum pname, api::GLuint index, api::GLuint64EXT address, api::GLsizeiptr length)
{
	log() << warning << "BufferAddressRangeNV not available." << std::endl;
}

static void fallbackBufferData(api::GLenum target, api::GLsizeiptr size, const api::GLvoid* data, api::GLenum usage)
{
	log() << warning << "BufferData not available." << std::endl;
}

static void fallbackBufferDataARB(api::GLenum target, api::GLsizeiptrARB size, const api::GLvoid* data, api::GLenum usage)
{
	log() << warning << "BufferDataARB not available." << std::endl;
}

static void fallbackBufferParameteriAPPLE(api::GLenum target, api::GLenum pname, api::GLint param)
{
	log() << warning << "BufferParameteriAPPLE not available." << std::endl;
}

static void fallbackBufferSubData(api::GLenum target, api::GLintptr offset, api::GLsizeiptr size, const api::GLvoid* data)
{
	log() << warning << "BufferSubData not available." << std::endl;
}

static void fallbackBufferSubDataARB(api::GLenum target, api::GLintptrARB offset, api::GLsizeiptrARB size, const api::GLvoid* data)
{
	log() << warning << "BufferSubDataARB not available." << std::endl;
}

static void fallbackCallList(api::GLuint list)
{
	log() << warning << "CallList not available." << std::endl;
}

static void fallbackCallLists(api::GLsizei n, api::GLenum type, const api::GLvoid* lists)
{
	log() << warning << "CallLists not available." << std::endl;
}

static api::GLenum fallbackCheckFramebufferStatus(api::GLenum target)
{
	log() << warning << "CheckFramebufferStatus not available." << std::endl;
}

static api::GLenum fallbackCheckFramebufferStatusEXT(api::GLenum target)
{
	log() << warning << "CheckFramebufferStatusEXT not available." << std::endl;
}

static api::GLenum fallbackCheckNamedFramebufferStatusEXT(api::GLuint framebuffer, api::GLenum target)
{
	log() << warning << "CheckNamedFramebufferStatusEXT not available." << std::endl;
}

static void fallbackClampColor(api::GLenum target, api::GLenum clamp)
{
	log() << warning << "ClampColor not available." << std::endl;
}

static void fallbackClampColorARB(api::GLenum target, api::GLenum clamp)
{
	log() << warning << "ClampColorARB not available." << std::endl;
}

static void fallbackClear(api::GLbitfield mask)
{
	log() << warning << "Clear not available." << std::endl;
}

static void fallbackClearAccum(api::GLfloat red, api::GLfloat green, api::GLfloat blue, api::GLfloat alpha)
{
	log() << warning << "ClearAccum not available." << std::endl;
}

static void fallbackClearBufferfi(api::GLenum buffer, api::GLint drawbuffer, api::GLfloat depth, api::GLint stencil)
{
	log() << warning << "ClearBufferfi not available." << std::endl;
}

static void fallbackClearBufferfv(api::GLenum buffer, api::GLint drawbuffer, const api::GLfloat* value)
{
	log() << warning << "ClearBufferfv not available." << std::endl;
}

static void fallbackClearBufferiv(api::GLenum buffer, api::GLint drawbuffer, const api::GLint* value)
{
	log() << warning << "ClearBufferiv not available." << std::endl;
}

static void fallbackClearBufferuiv(api::GLenum buffer, api::GLint drawbuffer, const api::GLuint* value)
{
	log() << warning << "ClearBufferuiv not available." << std::endl;
}

static void fallbackClearColor(api::GLclampf red, api::GLclampf green, api::GLclampf blue, api::GLclampf alpha)
{
	log() << warning << "ClearColor not available." << std::endl;
}

static void fallbackClearColorIiEXT(api::GLint red, api::GLint green, api::GLint blue, api::GLint alpha)
{
	log() << warning << "ClearColorIiEXT not available." << std::endl;
}

static void fallbackClearColorIuiEXT(api::GLuint red, api::GLuint green, api::GLuint blue, api::GLuint alpha)
{
	log() << warning << "ClearColorIuiEXT not available." << std::endl;
}

static void fallbackClearDepth(api::GLclampd depth)
{
	log() << warning << "ClearDepth not available." << std::endl;
}

static void fallbackClearDepthdNV(api::GLdouble depth)
{
	log() << warning << "ClearDepthdNV not available." << std::endl;
}

static void fallbackClearIndex(api::GLfloat c)
{
	log() << warning << "ClearIndex not available." << std::endl;
}

static void fallbackClearStencil(api::GLint s)
{
	log() << warning << "ClearStencil not available." << std::endl;
}

static void fallbackClientActiveTexture(api::GLenum texture)
{
	log() << warning << "ClientActiveTexture not available." << std::endl;
}

static void fallbackClientActiveTextureARB(api::GLenum texture)
{
	log() << warning << "ClientActiveTextureARB not available." << std::endl;
}

static void fallbackClientActiveVertexStreamATI(api::GLenum stream)
{
	log() << warning << "ClientActiveVertexStreamATI not available." << std::endl;
}

static void fallbackClientAttribDefaultEXT(api::GLbitfield mask)
{
	log() << warning << "ClientAttribDefaultEXT not available." << std::endl;
}

static api::GLenum fallbackClientWaitSync(api::GLsync sync, api::GLbitfield flags, api::GLuint64 timeout)
{
	log() << warning << "ClientWaitSync not available." << std::endl;
}

static void fallbackClipPlane(api::GLenum plane, const api::GLdouble* equation)
{
	log() << warning << "ClipPlane not available." << std::endl;
}

static void fallbackColor3b(api::GLbyte red, api::GLbyte green, api::GLbyte blue)
{
	log() << warning << "Color3b not available." << std::endl;
}

static void fallbackColor3bv(const api::GLbyte* v)
{
	log() << warning << "Color3bv not available." << std::endl;
}

static void fallbackColor3d(api::GLdouble red, api::GLdouble green, api::GLdouble blue)
{
	log() << warning << "Color3d not available." << std::endl;
}

static void fallbackColor3dv(const api::GLdouble* v)
{
	log() << warning << "Color3dv not available." << std::endl;
}

static void fallbackColor3f(api::GLfloat red, api::GLfloat green, api::GLfloat blue)
{
	log() << warning << "Color3f not available." << std::endl;
}

static void fallbackColor3fVertex3fSUN(api::GLfloat r, api::GLfloat g, api::GLfloat b, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "Color3fVertex3fSUN not available." << std::endl;
}

static void fallbackColor3fVertex3fvSUN(const api::GLfloat* c, const api::GLfloat* v)
{
	log() << warning << "Color3fVertex3fvSUN not available." << std::endl;
}

static void fallbackColor3fv(const api::GLfloat* v)
{
	log() << warning << "Color3fv not available." << std::endl;
}

static void fallbackColor3hNV(api::GLhalfNV red, api::GLhalfNV green, api::GLhalfNV blue)
{
	log() << warning << "Color3hNV not available." << std::endl;
}

static void fallbackColor3hvNV(const api::GLhalfNV* v)
{
	log() << warning << "Color3hvNV not available." << std::endl;
}

static void fallbackColor3i(api::GLint red, api::GLint green, api::GLint blue)
{
	log() << warning << "Color3i not available." << std::endl;
}

static void fallbackColor3iv(const api::GLint* v)
{
	log() << warning << "Color3iv not available." << std::endl;
}

static void fallbackColor3s(api::GLshort red, api::GLshort green, api::GLshort blue)
{
	log() << warning << "Color3s not available." << std::endl;
}

static void fallbackColor3sv(const api::GLshort* v)
{
	log() << warning << "Color3sv not available." << std::endl;
}

static void fallbackColor3ub(api::GLubyte red, api::GLubyte green, api::GLubyte blue)
{
	log() << warning << "Color3ub not available." << std::endl;
}

static void fallbackColor3ubv(const api::GLubyte* v)
{
	log() << warning << "Color3ubv not available." << std::endl;
}

static void fallbackColor3ui(api::GLuint red, api::GLuint green, api::GLuint blue)
{
	log() << warning << "Color3ui not available." << std::endl;
}

static void fallbackColor3uiv(const api::GLuint* v)
{
	log() << warning << "Color3uiv not available." << std::endl;
}

static void fallbackColor3us(api::GLushort red, api::GLushort green, api::GLushort blue)
{
	log() << warning << "Color3us not available." << std::endl;
}

static void fallbackColor3usv(const api::GLushort* v)
{
	log() << warning << "Color3usv not available." << std::endl;
}

static void fallbackColor4b(api::GLbyte red, api::GLbyte green, api::GLbyte blue, api::GLbyte alpha)
{
	log() << warning << "Color4b not available." << std::endl;
}

static void fallbackColor4bv(const api::GLbyte* v)
{
	log() << warning << "Color4bv not available." << std::endl;
}

static void fallbackColor4d(api::GLdouble red, api::GLdouble green, api::GLdouble blue, api::GLdouble alpha)
{
	log() << warning << "Color4d not available." << std::endl;
}

static void fallbackColor4dv(const api::GLdouble* v)
{
	log() << warning << "Color4dv not available." << std::endl;
}

static void fallbackColor4f(api::GLfloat red, api::GLfloat green, api::GLfloat blue, api::GLfloat alpha)
{
	log() << warning << "Color4f not available." << std::endl;
}

static void fallbackColor4fNormal3fVertex3fSUN(api::GLfloat r, api::GLfloat g, api::GLfloat b, api::GLfloat a, api::GLfloat nx, api::GLfloat ny, api::GLfloat nz, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "Color4fNormal3fVertex3fSUN not available." << std::endl;
}

static void fallbackColor4fNormal3fVertex3fvSUN(const api::GLfloat* c, const api::GLfloat* n, const api::GLfloat* v)
{
	log() << warning << "Color4fNormal3fVertex3fvSUN not available." << std::endl;
}

static void fallbackColor4fv(const api::GLfloat* v)
{
	log() << warning << "Color4fv not available." << std::endl;
}

static void fallbackColor4hNV(api::GLhalfNV red, api::GLhalfNV green, api::GLhalfNV blue, api::GLhalfNV alpha)
{
	log() << warning << "Color4hNV not available." << std::endl;
}

static void fallbackColor4hvNV(const api::GLhalfNV* v)
{
	log() << warning << "Color4hvNV not available." << std::endl;
}

static void fallbackColor4i(api::GLint red, api::GLint green, api::GLint blue, api::GLint alpha)
{
	log() << warning << "Color4i not available." << std::endl;
}

static void fallbackColor4iv(const api::GLint* v)
{
	log() << warning << "Color4iv not available." << std::endl;
}

static void fallbackColor4s(api::GLshort red, api::GLshort green, api::GLshort blue, api::GLshort alpha)
{
	log() << warning << "Color4s not available." << std::endl;
}

static void fallbackColor4sv(const api::GLshort* v)
{
	log() << warning << "Color4sv not available." << std::endl;
}

static void fallbackColor4ub(api::GLubyte red, api::GLubyte green, api::GLubyte blue, api::GLubyte alpha)
{
	log() << warning << "Color4ub not available." << std::endl;
}

static void fallbackColor4ubVertex2fSUN(api::GLubyte r, api::GLubyte g, api::GLubyte b, api::GLubyte a, api::GLfloat x, api::GLfloat y)
{
	log() << warning << "Color4ubVertex2fSUN not available." << std::endl;
}

static void fallbackColor4ubVertex2fvSUN(const api::GLubyte* c, const api::GLfloat* v)
{
	log() << warning << "Color4ubVertex2fvSUN not available." << std::endl;
}

static void fallbackColor4ubVertex3fSUN(api::GLubyte r, api::GLubyte g, api::GLubyte b, api::GLubyte a, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "Color4ubVertex3fSUN not available." << std::endl;
}

static void fallbackColor4ubVertex3fvSUN(const api::GLubyte* c, const api::GLfloat* v)
{
	log() << warning << "Color4ubVertex3fvSUN not available." << std::endl;
}

static void fallbackColor4ubv(const api::GLubyte* v)
{
	log() << warning << "Color4ubv not available." << std::endl;
}

static void fallbackColor4ui(api::GLuint red, api::GLuint green, api::GLuint blue, api::GLuint alpha)
{
	log() << warning << "Color4ui not available." << std::endl;
}

static void fallbackColor4uiv(const api::GLuint* v)
{
	log() << warning << "Color4uiv not available." << std::endl;
}

static void fallbackColor4us(api::GLushort red, api::GLushort green, api::GLushort blue, api::GLushort alpha)
{
	log() << warning << "Color4us not available." << std::endl;
}

static void fallbackColor4usv(const api::GLushort* v)
{
	log() << warning << "Color4usv not available." << std::endl;
}

static void fallbackColorFormatNV(api::GLint size, api::GLenum type, api::GLsizei stride)
{
	log() << warning << "ColorFormatNV not available." << std::endl;
}

static void fallbackColorFragmentOp1ATI(api::GLenum op, api::GLuint dst, api::GLuint dstMask, api::GLuint dstMod, api::GLuint arg1, api::GLuint arg1Rep, api::GLuint arg1Mod)
{
	log() << warning << "ColorFragmentOp1ATI not available." << std::endl;
}

static void fallbackColorFragmentOp2ATI(api::GLenum op, api::GLuint dst, api::GLuint dstMask, api::GLuint dstMod, api::GLuint arg1, api::GLuint arg1Rep, api::GLuint arg1Mod, api::GLuint arg2, api::GLuint arg2Rep, api::GLuint arg2Mod)
{
	log() << warning << "ColorFragmentOp2ATI not available." << std::endl;
}

static void fallbackColorFragmentOp3ATI(api::GLenum op, api::GLuint dst, api::GLuint dstMask, api::GLuint dstMod, api::GLuint arg1, api::GLuint arg1Rep, api::GLuint arg1Mod, api::GLuint arg2, api::GLuint arg2Rep, api::GLuint arg2Mod, api::GLuint arg3, api::GLuint arg3Rep, api::GLuint arg3Mod)
{
	log() << warning << "ColorFragmentOp3ATI not available." << std::endl;
}

static void fallbackColorMask(api::GLboolean red, api::GLboolean green, api::GLboolean blue, api::GLboolean alpha)
{
	log() << warning << "ColorMask not available." << std::endl;
}

static void fallbackColorMaskIndexedEXT(api::GLuint index, api::GLboolean r, api::GLboolean g, api::GLboolean b, api::GLboolean a)
{
	log() << warning << "ColorMaskIndexedEXT not available." << std::endl;
}

static void fallbackColorMaski(api::GLuint index, api::GLboolean r, api::GLboolean g, api::GLboolean b, api::GLboolean a)
{
	log() << warning << "ColorMaski not available." << std::endl;
}

static void fallbackColorMaterial(api::GLenum face, api::GLenum mode)
{
	log() << warning << "ColorMaterial not available." << std::endl;
}

static void fallbackColorP3ui(api::GLenum type, api::GLuint color)
{
	log() << warning << "ColorP3ui not available." << std::endl;
}

static void fallbackColorP3uiv(api::GLenum type, const api::GLuint* color)
{
	log() << warning << "ColorP3uiv not available." << std::endl;
}

static void fallbackColorP4ui(api::GLenum type, api::GLuint color)
{
	log() << warning << "ColorP4ui not available." << std::endl;
}

static void fallbackColorP4uiv(api::GLenum type, const api::GLuint* color)
{
	log() << warning << "ColorP4uiv not available." << std::endl;
}

static void fallbackColorPointer(api::GLint size, api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "ColorPointer not available." << std::endl;
}

static void fallbackColorPointerEXT(api::GLint size, api::GLenum type, api::GLsizei stride, api::GLsizei count, const api::GLvoid* pointer)
{
	log() << warning << "ColorPointerEXT not available." << std::endl;
}

static void fallbackColorPointerListIBM(api::GLint size, api::GLenum type, api::GLint stride, const api::GLvoid** pointer, api::GLint ptrstride)
{
	log() << warning << "ColorPointerListIBM not available." << std::endl;
}

static void fallbackColorPointervINTEL(api::GLint size, api::GLenum type, const api::GLvoid** pointer)
{
	log() << warning << "ColorPointervINTEL not available." << std::endl;
}

static void fallbackColorSubTable(api::GLenum target, api::GLsizei start, api::GLsizei count, api::GLenum format, api::GLenum type, const api::GLvoid* data)
{
	log() << warning << "ColorSubTable not available." << std::endl;
}

static void fallbackColorSubTableEXT(api::GLenum target, api::GLsizei start, api::GLsizei count, api::GLenum format, api::GLenum type, const api::GLvoid* data)
{
	log() << warning << "ColorSubTableEXT not available." << std::endl;
}

static void fallbackColorTable(api::GLenum target, api::GLenum internalformat, api::GLsizei width, api::GLenum format, api::GLenum type, const api::GLvoid* table)
{
	log() << warning << "ColorTable not available." << std::endl;
}

static void fallbackColorTableEXT(api::GLenum target, api::GLenum internalFormat, api::GLsizei width, api::GLenum format, api::GLenum type, const api::GLvoid* table)
{
	log() << warning << "ColorTableEXT not available." << std::endl;
}

static void fallbackColorTableParameterfv(api::GLenum target, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "ColorTableParameterfv not available." << std::endl;
}

static void fallbackColorTableParameterfvSGI(api::GLenum target, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "ColorTableParameterfvSGI not available." << std::endl;
}

static void fallbackColorTableParameteriv(api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "ColorTableParameteriv not available." << std::endl;
}

static void fallbackColorTableParameterivSGI(api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "ColorTableParameterivSGI not available." << std::endl;
}

static void fallbackColorTableSGI(api::GLenum target, api::GLenum internalformat, api::GLsizei width, api::GLenum format, api::GLenum type, const api::GLvoid* table)
{
	log() << warning << "ColorTableSGI not available." << std::endl;
}

static void fallbackCombinerInputNV(api::GLenum stage, api::GLenum portion, api::GLenum variable, api::GLenum input, api::GLenum mapping, api::GLenum componentUsage)
{
	log() << warning << "CombinerInputNV not available." << std::endl;
}

static void fallbackCombinerOutputNV(api::GLenum stage, api::GLenum portion, api::GLenum abOutput, api::GLenum cdOutput, api::GLenum sumOutput, api::GLenum scale, api::GLenum bias, api::GLboolean abDotProduct, api::GLboolean cdDotProduct, api::GLboolean muxSum)
{
	log() << warning << "CombinerOutputNV not available." << std::endl;
}

static void fallbackCombinerParameterfNV(api::GLenum pname, api::GLfloat param)
{
	log() << warning << "CombinerParameterfNV not available." << std::endl;
}

static void fallbackCombinerParameterfvNV(api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "CombinerParameterfvNV not available." << std::endl;
}

static void fallbackCombinerParameteriNV(api::GLenum pname, api::GLint param)
{
	log() << warning << "CombinerParameteriNV not available." << std::endl;
}

static void fallbackCombinerParameterivNV(api::GLenum pname, const api::GLint* params)
{
	log() << warning << "CombinerParameterivNV not available." << std::endl;
}

static void fallbackCombinerStageParameterfvNV(api::GLenum stage, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "CombinerStageParameterfvNV not available." << std::endl;
}

static void fallbackCompileShader(api::GLuint shader)
{
	log() << warning << "CompileShader not available." << std::endl;
}

static void fallbackCompileShaderARB(api::GLhandleARB shaderObj)
{
	log() << warning << "CompileShaderARB not available." << std::endl;
}

static void fallbackCompileShaderIncludeARB(api::GLuint shader, api::GLsizei count, const api::GLchar** path, const api::GLint* length)
{
	log() << warning << "CompileShaderIncludeARB not available." << std::endl;
}

static void fallbackCompressedMultiTexImage1DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLint border, api::GLsizei imageSize, const api::GLvoid* bits)
{
	log() << warning << "CompressedMultiTexImage1DEXT not available." << std::endl;
}

static void fallbackCompressedMultiTexImage2DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLint border, api::GLsizei imageSize, const api::GLvoid* bits)
{
	log() << warning << "CompressedMultiTexImage2DEXT not available." << std::endl;
}

static void fallbackCompressedMultiTexImage3DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLint border, api::GLsizei imageSize, const api::GLvoid* bits)
{
	log() << warning << "CompressedMultiTexImage3DEXT not available." << std::endl;
}

static void fallbackCompressedMultiTexSubImage1DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLsizei width, api::GLenum format, api::GLsizei imageSize, const api::GLvoid* bits)
{
	log() << warning << "CompressedMultiTexSubImage1DEXT not available." << std::endl;
}

static void fallbackCompressedMultiTexSubImage2DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLsizei width, api::GLsizei height, api::GLenum format, api::GLsizei imageSize, const api::GLvoid* bits)
{
	log() << warning << "CompressedMultiTexSubImage2DEXT not available." << std::endl;
}

static void fallbackCompressedMultiTexSubImage3DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint zoffset, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLenum format, api::GLsizei imageSize, const api::GLvoid* bits)
{
	log() << warning << "CompressedMultiTexSubImage3DEXT not available." << std::endl;
}

static void fallbackCompressedTexImage1D(api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLint border, api::GLsizei imageSize, const api::GLvoid* data)
{
	log() << warning << "CompressedTexImage1D not available." << std::endl;
}

static void fallbackCompressedTexImage1DARB(api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLint border, api::GLsizei imageSize, const api::GLvoid* data)
{
	log() << warning << "CompressedTexImage1DARB not available." << std::endl;
}

static void fallbackCompressedTexImage2D(api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLint border, api::GLsizei imageSize, const api::GLvoid* data)
{
	log() << warning << "CompressedTexImage2D not available." << std::endl;
}

static void fallbackCompressedTexImage2DARB(api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLint border, api::GLsizei imageSize, const api::GLvoid* data)
{
	log() << warning << "CompressedTexImage2DARB not available." << std::endl;
}

static void fallbackCompressedTexImage3D(api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLint border, api::GLsizei imageSize, const api::GLvoid* data)
{
	log() << warning << "CompressedTexImage3D not available." << std::endl;
}

static void fallbackCompressedTexImage3DARB(api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLint border, api::GLsizei imageSize, const api::GLvoid* data)
{
	log() << warning << "CompressedTexImage3DARB not available." << std::endl;
}

static void fallbackCompressedTexSubImage1D(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLsizei width, api::GLenum format, api::GLsizei imageSize, const api::GLvoid* data)
{
	log() << warning << "CompressedTexSubImage1D not available." << std::endl;
}

static void fallbackCompressedTexSubImage1DARB(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLsizei width, api::GLenum format, api::GLsizei imageSize, const api::GLvoid* data)
{
	log() << warning << "CompressedTexSubImage1DARB not available." << std::endl;
}

static void fallbackCompressedTexSubImage2D(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLsizei width, api::GLsizei height, api::GLenum format, api::GLsizei imageSize, const api::GLvoid* data)
{
	log() << warning << "CompressedTexSubImage2D not available." << std::endl;
}

static void fallbackCompressedTexSubImage2DARB(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLsizei width, api::GLsizei height, api::GLenum format, api::GLsizei imageSize, const api::GLvoid* data)
{
	log() << warning << "CompressedTexSubImage2DARB not available." << std::endl;
}

static void fallbackCompressedTexSubImage3D(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint zoffset, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLenum format, api::GLsizei imageSize, const api::GLvoid* data)
{
	log() << warning << "CompressedTexSubImage3D not available." << std::endl;
}

static void fallbackCompressedTexSubImage3DARB(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint zoffset, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLenum format, api::GLsizei imageSize, const api::GLvoid* data)
{
	log() << warning << "CompressedTexSubImage3DARB not available." << std::endl;
}

static void fallbackCompressedTextureImage1DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLint border, api::GLsizei imageSize, const api::GLvoid* bits)
{
	log() << warning << "CompressedTextureImage1DEXT not available." << std::endl;
}

static void fallbackCompressedTextureImage2DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLint border, api::GLsizei imageSize, const api::GLvoid* bits)
{
	log() << warning << "CompressedTextureImage2DEXT not available." << std::endl;
}

static void fallbackCompressedTextureImage3DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLint border, api::GLsizei imageSize, const api::GLvoid* bits)
{
	log() << warning << "CompressedTextureImage3DEXT not available." << std::endl;
}

static void fallbackCompressedTextureSubImage1DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLsizei width, api::GLenum format, api::GLsizei imageSize, const api::GLvoid* bits)
{
	log() << warning << "CompressedTextureSubImage1DEXT not available." << std::endl;
}

static void fallbackCompressedTextureSubImage2DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLsizei width, api::GLsizei height, api::GLenum format, api::GLsizei imageSize, const api::GLvoid* bits)
{
	log() << warning << "CompressedTextureSubImage2DEXT not available." << std::endl;
}

static void fallbackCompressedTextureSubImage3DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint zoffset, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLenum format, api::GLsizei imageSize, const api::GLvoid* bits)
{
	log() << warning << "CompressedTextureSubImage3DEXT not available." << std::endl;
}

static void fallbackConvolutionFilter1D(api::GLenum target, api::GLenum internalformat, api::GLsizei width, api::GLenum format, api::GLenum type, const api::GLvoid* image)
{
	log() << warning << "ConvolutionFilter1D not available." << std::endl;
}

static void fallbackConvolutionFilter1DEXT(api::GLenum target, api::GLenum internalformat, api::GLsizei width, api::GLenum format, api::GLenum type, const api::GLvoid* image)
{
	log() << warning << "ConvolutionFilter1DEXT not available." << std::endl;
}

static void fallbackConvolutionFilter2D(api::GLenum target, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLenum format, api::GLenum type, const api::GLvoid* image)
{
	log() << warning << "ConvolutionFilter2D not available." << std::endl;
}

static void fallbackConvolutionFilter2DEXT(api::GLenum target, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLenum format, api::GLenum type, const api::GLvoid* image)
{
	log() << warning << "ConvolutionFilter2DEXT not available." << std::endl;
}

static void fallbackConvolutionParameterf(api::GLenum target, api::GLenum pname, api::GLfloat params)
{
	log() << warning << "ConvolutionParameterf not available." << std::endl;
}

static void fallbackConvolutionParameterfEXT(api::GLenum target, api::GLenum pname, api::GLfloat params)
{
	log() << warning << "ConvolutionParameterfEXT not available." << std::endl;
}

static void fallbackConvolutionParameterfv(api::GLenum target, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "ConvolutionParameterfv not available." << std::endl;
}

static void fallbackConvolutionParameterfvEXT(api::GLenum target, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "ConvolutionParameterfvEXT not available." << std::endl;
}

static void fallbackConvolutionParameteri(api::GLenum target, api::GLenum pname, api::GLint params)
{
	log() << warning << "ConvolutionParameteri not available." << std::endl;
}

static void fallbackConvolutionParameteriEXT(api::GLenum target, api::GLenum pname, api::GLint params)
{
	log() << warning << "ConvolutionParameteriEXT not available." << std::endl;
}

static void fallbackConvolutionParameteriv(api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "ConvolutionParameteriv not available." << std::endl;
}

static void fallbackConvolutionParameterivEXT(api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "ConvolutionParameterivEXT not available." << std::endl;
}

static void fallbackCopyBufferSubData(api::GLenum readTarget, api::GLenum writeTarget, api::GLintptr readOffset, api::GLintptr writeOffset, api::GLsizeiptr size)
{
	log() << warning << "CopyBufferSubData not available." << std::endl;
}

static void fallbackCopyColorSubTable(api::GLenum target, api::GLsizei start, api::GLint x, api::GLint y, api::GLsizei width)
{
	log() << warning << "CopyColorSubTable not available." << std::endl;
}

static void fallbackCopyColorSubTableEXT(api::GLenum target, api::GLsizei start, api::GLint x, api::GLint y, api::GLsizei width)
{
	log() << warning << "CopyColorSubTableEXT not available." << std::endl;
}

static void fallbackCopyColorTable(api::GLenum target, api::GLenum internalformat, api::GLint x, api::GLint y, api::GLsizei width)
{
	log() << warning << "CopyColorTable not available." << std::endl;
}

static void fallbackCopyColorTableSGI(api::GLenum target, api::GLenum internalformat, api::GLint x, api::GLint y, api::GLsizei width)
{
	log() << warning << "CopyColorTableSGI not available." << std::endl;
}

static void fallbackCopyConvolutionFilter1D(api::GLenum target, api::GLenum internalformat, api::GLint x, api::GLint y, api::GLsizei width)
{
	log() << warning << "CopyConvolutionFilter1D not available." << std::endl;
}

static void fallbackCopyConvolutionFilter1DEXT(api::GLenum target, api::GLenum internalformat, api::GLint x, api::GLint y, api::GLsizei width)
{
	log() << warning << "CopyConvolutionFilter1DEXT not available." << std::endl;
}

static void fallbackCopyConvolutionFilter2D(api::GLenum target, api::GLenum internalformat, api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "CopyConvolutionFilter2D not available." << std::endl;
}

static void fallbackCopyConvolutionFilter2DEXT(api::GLenum target, api::GLenum internalformat, api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "CopyConvolutionFilter2DEXT not available." << std::endl;
}

static void fallbackCopyImageSubDataNV(api::GLuint srcName, api::GLenum srcTarget, api::GLint srcLevel, api::GLint srcX, api::GLint srcY, api::GLint srcZ, api::GLuint dstName, api::GLenum dstTarget, api::GLint dstLevel, api::GLint dstX, api::GLint dstY, api::GLint dstZ, api::GLsizei width, api::GLsizei height, api::GLsizei depth)
{
	log() << warning << "CopyImageSubDataNV not available." << std::endl;
}

static void fallbackCopyMultiTexImage1DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLint x, api::GLint y, api::GLsizei width, api::GLint border)
{
	log() << warning << "CopyMultiTexImage1DEXT not available." << std::endl;
}

static void fallbackCopyMultiTexImage2DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height, api::GLint border)
{
	log() << warning << "CopyMultiTexImage2DEXT not available." << std::endl;
}

static void fallbackCopyMultiTexSubImage1DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint x, api::GLint y, api::GLsizei width)
{
	log() << warning << "CopyMultiTexSubImage1DEXT not available." << std::endl;
}

static void fallbackCopyMultiTexSubImage2DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "CopyMultiTexSubImage2DEXT not available." << std::endl;
}

static void fallbackCopyMultiTexSubImage3DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint zoffset, api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "CopyMultiTexSubImage3DEXT not available." << std::endl;
}

static void fallbackCopyPixels(api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height, api::GLenum type)
{
	log() << warning << "CopyPixels not available." << std::endl;
}

static void fallbackCopyTexImage1D(api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLint x, api::GLint y, api::GLsizei width, api::GLint border)
{
	log() << warning << "CopyTexImage1D not available." << std::endl;
}

static void fallbackCopyTexImage1DEXT(api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLint x, api::GLint y, api::GLsizei width, api::GLint border)
{
	log() << warning << "CopyTexImage1DEXT not available." << std::endl;
}

static void fallbackCopyTexImage2D(api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height, api::GLint border)
{
	log() << warning << "CopyTexImage2D not available." << std::endl;
}

static void fallbackCopyTexImage2DEXT(api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height, api::GLint border)
{
	log() << warning << "CopyTexImage2DEXT not available." << std::endl;
}

static void fallbackCopyTexSubImage1D(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint x, api::GLint y, api::GLsizei width)
{
	log() << warning << "CopyTexSubImage1D not available." << std::endl;
}

static void fallbackCopyTexSubImage1DEXT(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint x, api::GLint y, api::GLsizei width)
{
	log() << warning << "CopyTexSubImage1DEXT not available." << std::endl;
}

static void fallbackCopyTexSubImage2D(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "CopyTexSubImage2D not available." << std::endl;
}

static void fallbackCopyTexSubImage2DEXT(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "CopyTexSubImage2DEXT not available." << std::endl;
}

static void fallbackCopyTexSubImage3D(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint zoffset, api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "CopyTexSubImage3D not available." << std::endl;
}

static void fallbackCopyTexSubImage3DEXT(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint zoffset, api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "CopyTexSubImage3DEXT not available." << std::endl;
}

static void fallbackCopyTextureImage1DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLint x, api::GLint y, api::GLsizei width, api::GLint border)
{
	log() << warning << "CopyTextureImage1DEXT not available." << std::endl;
}

static void fallbackCopyTextureImage2DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height, api::GLint border)
{
	log() << warning << "CopyTextureImage2DEXT not available." << std::endl;
}

static void fallbackCopyTextureSubImage1DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint x, api::GLint y, api::GLsizei width)
{
	log() << warning << "CopyTextureSubImage1DEXT not available." << std::endl;
}

static void fallbackCopyTextureSubImage2DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "CopyTextureSubImage2DEXT not available." << std::endl;
}

static void fallbackCopyTextureSubImage3DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint zoffset, api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "CopyTextureSubImage3DEXT not available." << std::endl;
}

static api::GLuint fallbackCreateProgram()
{
	log() << warning << "CreateProgram not available." << std::endl;
}

static api::GLhandleARB fallbackCreateProgramObjectARB()
{
	log() << warning << "CreateProgramObjectARB not available." << std::endl;
}

static api::GLuint fallbackCreateShader(api::GLenum type)
{
	log() << warning << "CreateShader not available." << std::endl;
}

static api::GLhandleARB fallbackCreateShaderObjectARB(api::GLenum shaderType)
{
	log() << warning << "CreateShaderObjectARB not available." << std::endl;
}

static api::GLuint fallbackCreateShaderProgramEXT(api::GLenum type, const api::GLchar* string)
{
	log() << warning << "CreateShaderProgramEXT not available." << std::endl;
}

static void fallbackCullFace(api::GLenum mode)
{
	log() << warning << "CullFace not available." << std::endl;
}

static void fallbackCullParameterdvEXT(api::GLenum pname, api::GLdouble* params)
{
	log() << warning << "CullParameterdvEXT not available." << std::endl;
}

static void fallbackCullParameterfvEXT(api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "CullParameterfvEXT not available." << std::endl;
}

static void fallbackCurrentPaletteMatrixARB(api::GLint index)
{
	log() << warning << "CurrentPaletteMatrixARB not available." << std::endl;
}

static void fallbackDeformSGIX(api::GLbitfield mask)
{
	log() << warning << "DeformSGIX not available." << std::endl;
}

static void fallbackDeformationMap3dSGIX(api::GLenum target, api::GLdouble u1, api::GLdouble u2, api::GLint ustride, api::GLint uorder, api::GLdouble v1, api::GLdouble v2, api::GLint vstride, api::GLint vorder, api::GLdouble w1, api::GLdouble w2, api::GLint wstride, api::GLint worder, const api::GLdouble* points)
{
	log() << warning << "DeformationMap3dSGIX not available." << std::endl;
}

static void fallbackDeformationMap3fSGIX(api::GLenum target, api::GLfloat u1, api::GLfloat u2, api::GLint ustride, api::GLint uorder, api::GLfloat v1, api::GLfloat v2, api::GLint vstride, api::GLint vorder, api::GLfloat w1, api::GLfloat w2, api::GLint wstride, api::GLint worder, const api::GLfloat* points)
{
	log() << warning << "DeformationMap3fSGIX not available." << std::endl;
}

static void fallbackDeleteAsyncMarkersSGIX(api::GLuint marker, api::GLsizei range)
{
	log() << warning << "DeleteAsyncMarkersSGIX not available." << std::endl;
}

static void fallbackDeleteBuffers(api::GLsizei n, const api::GLuint* buffers)
{
	log() << warning << "DeleteBuffers not available." << std::endl;
}

static void fallbackDeleteBuffersARB(api::GLsizei n, const api::GLuint* buffers)
{
	log() << warning << "DeleteBuffersARB not available." << std::endl;
}

static void fallbackDeleteFencesAPPLE(api::GLsizei n, const api::GLuint* fences)
{
	log() << warning << "DeleteFencesAPPLE not available." << std::endl;
}

static void fallbackDeleteFencesNV(api::GLsizei n, const api::GLuint* fences)
{
	log() << warning << "DeleteFencesNV not available." << std::endl;
}

static void fallbackDeleteFragmentShaderATI(api::GLuint id)
{
	log() << warning << "DeleteFragmentShaderATI not available." << std::endl;
}

static void fallbackDeleteFramebuffers(api::GLsizei n, const api::GLuint* framebuffers)
{
	log() << warning << "DeleteFramebuffers not available." << std::endl;
}

static void fallbackDeleteFramebuffersEXT(api::GLsizei n, const api::GLuint* framebuffers)
{
	log() << warning << "DeleteFramebuffersEXT not available." << std::endl;
}

static void fallbackDeleteLists(api::GLuint list, api::GLsizei range)
{
	log() << warning << "DeleteLists not available." << std::endl;
}

static void fallbackDeleteNamedStringARB(api::GLint namelen, const api::GLchar* name)
{
	log() << warning << "DeleteNamedStringARB not available." << std::endl;
}

static void fallbackDeleteObjectARB(api::GLhandleARB obj)
{
	log() << warning << "DeleteObjectARB not available." << std::endl;
}

static void fallbackDeleteOcclusionQueriesNV(api::GLsizei n, const api::GLuint* ids)
{
	log() << warning << "DeleteOcclusionQueriesNV not available." << std::endl;
}

static void fallbackDeletePerfMonitorsAMD(api::GLsizei n, api::GLuint* monitors)
{
	log() << warning << "DeletePerfMonitorsAMD not available." << std::endl;
}

static void fallbackDeleteProgram(api::GLuint program)
{
	log() << warning << "DeleteProgram not available." << std::endl;
}

static void fallbackDeleteProgramsARB(api::GLsizei n, const api::GLuint* programs)
{
	log() << warning << "DeleteProgramsARB not available." << std::endl;
}

static void fallbackDeleteProgramsNV(api::GLsizei n, const api::GLuint* programs)
{
	log() << warning << "DeleteProgramsNV not available." << std::endl;
}

static void fallbackDeleteQueries(api::GLsizei n, const api::GLuint* ids)
{
	log() << warning << "DeleteQueries not available." << std::endl;
}

static void fallbackDeleteQueriesARB(api::GLsizei n, const api::GLuint* ids)
{
	log() << warning << "DeleteQueriesARB not available." << std::endl;
}

static void fallbackDeleteRenderbuffers(api::GLsizei n, const api::GLuint* renderbuffers)
{
	log() << warning << "DeleteRenderbuffers not available." << std::endl;
}

static void fallbackDeleteRenderbuffersEXT(api::GLsizei n, const api::GLuint* renderbuffers)
{
	log() << warning << "DeleteRenderbuffersEXT not available." << std::endl;
}

static void fallbackDeleteSamplers(api::GLsizei count, const api::GLuint* samplers)
{
	log() << warning << "DeleteSamplers not available." << std::endl;
}

static void fallbackDeleteShader(api::GLuint shader)
{
	log() << warning << "DeleteShader not available." << std::endl;
}

static void fallbackDeleteSync(api::GLsync sync)
{
	log() << warning << "DeleteSync not available." << std::endl;
}

static void fallbackDeleteTextures(api::GLsizei n, const api::GLuint* textures)
{
	log() << warning << "DeleteTextures not available." << std::endl;
}

static void fallbackDeleteTexturesEXT(api::GLsizei n, const api::GLuint* textures)
{
	log() << warning << "DeleteTexturesEXT not available." << std::endl;
}

static void fallbackDeleteTransformFeedbacks(api::GLsizei n, const api::GLuint* ids)
{
	log() << warning << "DeleteTransformFeedbacks not available." << std::endl;
}

static void fallbackDeleteTransformFeedbacksNV(api::GLsizei n, const api::GLuint* ids)
{
	log() << warning << "DeleteTransformFeedbacksNV not available." << std::endl;
}

static void fallbackDeleteVertexArrays(api::GLsizei n, const api::GLuint* arrays)
{
	log() << warning << "DeleteVertexArrays not available." << std::endl;
}

static void fallbackDeleteVertexArraysAPPLE(api::GLsizei n, const api::GLuint* arrays)
{
	log() << warning << "DeleteVertexArraysAPPLE not available." << std::endl;
}

static void fallbackDeleteVertexShaderEXT(api::GLuint id)
{
	log() << warning << "DeleteVertexShaderEXT not available." << std::endl;
}

static void fallbackDepthBoundsEXT(api::GLclampd zmin, api::GLclampd zmax)
{
	log() << warning << "DepthBoundsEXT not available." << std::endl;
}

static void fallbackDepthBoundsdNV(api::GLdouble zmin, api::GLdouble zmax)
{
	log() << warning << "DepthBoundsdNV not available." << std::endl;
}

static void fallbackDepthFunc(api::GLenum func)
{
	log() << warning << "DepthFunc not available." << std::endl;
}

static void fallbackDepthMask(api::GLboolean flag)
{
	log() << warning << "DepthMask not available." << std::endl;
}

static void fallbackDepthRange(api::GLclampd near, api::GLclampd far)
{
	log() << warning << "DepthRange not available." << std::endl;
}

static void fallbackDepthRangedNV(api::GLdouble zNear, api::GLdouble zFar)
{
	log() << warning << "DepthRangedNV not available." << std::endl;
}

static void fallbackDetachObjectARB(api::GLhandleARB containerObj, api::GLhandleARB attachedObj)
{
	log() << warning << "DetachObjectARB not available." << std::endl;
}

static void fallbackDetachShader(api::GLuint program, api::GLuint shader)
{
	log() << warning << "DetachShader not available." << std::endl;
}

static void fallbackDetailTexFuncSGIS(api::GLenum target, api::GLsizei n, const api::GLfloat* points)
{
	log() << warning << "DetailTexFuncSGIS not available." << std::endl;
}

static void fallbackDisable(api::GLenum cap)
{
	log() << warning << "Disable not available." << std::endl;
}

static void fallbackDisableClientState(api::GLenum array)
{
	log() << warning << "DisableClientState not available." << std::endl;
}

static void fallbackDisableClientStateIndexedEXT(api::GLenum array, api::GLuint index)
{
	log() << warning << "DisableClientStateIndexedEXT not available." << std::endl;
}

static void fallbackDisableIndexedEXT(api::GLenum target, api::GLuint index)
{
	log() << warning << "DisableIndexedEXT not available." << std::endl;
}

static void fallbackDisableVariantClientStateEXT(api::GLuint id)
{
	log() << warning << "DisableVariantClientStateEXT not available." << std::endl;
}

static void fallbackDisableVertexAttribAPPLE(api::GLuint index, api::GLenum pname)
{
	log() << warning << "DisableVertexAttribAPPLE not available." << std::endl;
}

static void fallbackDisableVertexAttribArray(api::GLuint index)
{
	log() << warning << "DisableVertexAttribArray not available." << std::endl;
}

static void fallbackDisableVertexAttribArrayARB(api::GLuint index)
{
	log() << warning << "DisableVertexAttribArrayARB not available." << std::endl;
}

static void fallbackDisablei(api::GLenum target, api::GLuint index)
{
	log() << warning << "Disablei not available." << std::endl;
}

static void fallbackDrawArrays(api::GLenum mode, api::GLint first, api::GLsizei count)
{
	log() << warning << "DrawArrays not available." << std::endl;
}

static void fallbackDrawArraysEXT(api::GLenum mode, api::GLint first, api::GLsizei count)
{
	log() << warning << "DrawArraysEXT not available." << std::endl;
}

static void fallbackDrawArraysIndirect(api::GLenum mode, const api::GLvoid* indirect)
{
	log() << warning << "DrawArraysIndirect not available." << std::endl;
}

static void fallbackDrawArraysInstanced(api::GLenum mode, api::GLint first, api::GLsizei count, api::GLsizei primcount)
{
	log() << warning << "DrawArraysInstanced not available." << std::endl;
}

static void fallbackDrawArraysInstancedARB(api::GLenum mode, api::GLint first, api::GLsizei count, api::GLsizei primcount)
{
	log() << warning << "DrawArraysInstancedARB not available." << std::endl;
}

static void fallbackDrawArraysInstancedEXT(api::GLenum mode, api::GLint start, api::GLsizei count, api::GLsizei primcount)
{
	log() << warning << "DrawArraysInstancedEXT not available." << std::endl;
}

static void fallbackDrawBuffer(api::GLenum mode)
{
	log() << warning << "DrawBuffer not available." << std::endl;
}

static void fallbackDrawBuffers(api::GLsizei n, const api::GLenum* bufs)
{
	log() << warning << "DrawBuffers not available." << std::endl;
}

static void fallbackDrawBuffersARB(api::GLsizei n, const api::GLenum* bufs)
{
	log() << warning << "DrawBuffersARB not available." << std::endl;
}

static void fallbackDrawBuffersATI(api::GLsizei n, const api::GLenum* bufs)
{
	log() << warning << "DrawBuffersATI not available." << std::endl;
}

static void fallbackDrawElementArrayAPPLE(api::GLenum mode, api::GLint first, api::GLsizei count)
{
	log() << warning << "DrawElementArrayAPPLE not available." << std::endl;
}

static void fallbackDrawElementArrayATI(api::GLenum mode, api::GLsizei count)
{
	log() << warning << "DrawElementArrayATI not available." << std::endl;
}

static void fallbackDrawElements(api::GLenum mode, api::GLsizei count, api::GLenum type, const api::GLvoid* indices)
{
	log() << warning << "DrawElements not available." << std::endl;
}

static void fallbackDrawElementsBaseVertex(api::GLenum mode, api::GLsizei count, api::GLenum type, const api::GLvoid* indices, api::GLint basevertex)
{
	log() << warning << "DrawElementsBaseVertex not available." << std::endl;
}

static void fallbackDrawElementsIndirect(api::GLenum mode, api::GLenum type, const api::GLvoid* indirect)
{
	log() << warning << "DrawElementsIndirect not available." << std::endl;
}

static void fallbackDrawElementsInstanced(api::GLenum mode, api::GLsizei count, api::GLenum type, const api::GLvoid* indices, api::GLsizei primcount)
{
	log() << warning << "DrawElementsInstanced not available." << std::endl;
}

static void fallbackDrawElementsInstancedARB(api::GLenum mode, api::GLsizei count, api::GLenum type, const api::GLvoid* indices, api::GLsizei primcount)
{
	log() << warning << "DrawElementsInstancedARB not available." << std::endl;
}

static void fallbackDrawElementsInstancedBaseVertex(api::GLenum mode, api::GLsizei count, api::GLenum type, const api::GLvoid* indices, api::GLsizei primcount, api::GLint basevertex)
{
	log() << warning << "DrawElementsInstancedBaseVertex not available." << std::endl;
}

static void fallbackDrawElementsInstancedEXT(api::GLenum mode, api::GLsizei count, api::GLenum type, const api::GLvoid* indices, api::GLsizei primcount)
{
	log() << warning << "DrawElementsInstancedEXT not available." << std::endl;
}

static void fallbackDrawMeshArraysSUN(api::GLenum mode, api::GLint first, api::GLsizei count, api::GLsizei width)
{
	log() << warning << "DrawMeshArraysSUN not available." << std::endl;
}

static void fallbackDrawPixels(api::GLsizei width, api::GLsizei height, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "DrawPixels not available." << std::endl;
}

static void fallbackDrawRangeElementArrayAPPLE(api::GLenum mode, api::GLuint start, api::GLuint end, api::GLint first, api::GLsizei count)
{
	log() << warning << "DrawRangeElementArrayAPPLE not available." << std::endl;
}

static void fallbackDrawRangeElementArrayATI(api::GLenum mode, api::GLuint start, api::GLuint end, api::GLsizei count)
{
	log() << warning << "DrawRangeElementArrayATI not available." << std::endl;
}

static void fallbackDrawRangeElements(api::GLenum mode, api::GLuint start, api::GLuint end, api::GLsizei count, api::GLenum type, const api::GLvoid* indices)
{
	log() << warning << "DrawRangeElements not available." << std::endl;
}

static void fallbackDrawRangeElementsBaseVertex(api::GLenum mode, api::GLuint start, api::GLuint end, api::GLsizei count, api::GLenum type, const api::GLvoid* indices, api::GLint basevertex)
{
	log() << warning << "DrawRangeElementsBaseVertex not available." << std::endl;
}

static void fallbackDrawRangeElementsEXT(api::GLenum mode, api::GLuint start, api::GLuint end, api::GLsizei count, api::GLenum type, const api::GLvoid* indices)
{
	log() << warning << "DrawRangeElementsEXT not available." << std::endl;
}

static void fallbackDrawTransformFeedback(api::GLenum mode, api::GLuint id)
{
	log() << warning << "DrawTransformFeedback not available." << std::endl;
}

static void fallbackDrawTransformFeedbackNV(api::GLenum mode, api::GLuint id)
{
	log() << warning << "DrawTransformFeedbackNV not available." << std::endl;
}

static void fallbackDrawTransformFeedbackStream(api::GLenum mode, api::GLuint id, api::GLuint stream)
{
	log() << warning << "DrawTransformFeedbackStream not available." << std::endl;
}

static void fallbackEdgeFlag(api::GLboolean flag)
{
	log() << warning << "EdgeFlag not available." << std::endl;
}

static void fallbackEdgeFlagFormatNV(api::GLsizei stride)
{
	log() << warning << "EdgeFlagFormatNV not available." << std::endl;
}

static void fallbackEdgeFlagPointer(api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "EdgeFlagPointer not available." << std::endl;
}

static void fallbackEdgeFlagPointerEXT(api::GLsizei stride, api::GLsizei count, const api::GLboolean* pointer)
{
	log() << warning << "EdgeFlagPointerEXT not available." << std::endl;
}

static void fallbackEdgeFlagPointerListIBM(api::GLint stride, const api::GLboolean** pointer, api::GLint ptrstride)
{
	log() << warning << "EdgeFlagPointerListIBM not available." << std::endl;
}

static void fallbackEdgeFlagv(const api::GLboolean* flag)
{
	log() << warning << "EdgeFlagv not available." << std::endl;
}

static void fallbackElementPointerAPPLE(api::GLenum type, const api::GLvoid* pointer)
{
	log() << warning << "ElementPointerAPPLE not available." << std::endl;
}

static void fallbackElementPointerATI(api::GLenum type, const api::GLvoid* pointer)
{
	log() << warning << "ElementPointerATI not available." << std::endl;
}

static void fallbackEnable(api::GLenum cap)
{
	log() << warning << "Enable not available." << std::endl;
}

static void fallbackEnableClientState(api::GLenum array)
{
	log() << warning << "EnableClientState not available." << std::endl;
}

static void fallbackEnableClientStateIndexedEXT(api::GLenum array, api::GLuint index)
{
	log() << warning << "EnableClientStateIndexedEXT not available." << std::endl;
}

static void fallbackEnableIndexedEXT(api::GLenum target, api::GLuint index)
{
	log() << warning << "EnableIndexedEXT not available." << std::endl;
}

static void fallbackEnableVariantClientStateEXT(api::GLuint id)
{
	log() << warning << "EnableVariantClientStateEXT not available." << std::endl;
}

static void fallbackEnableVertexAttribAPPLE(api::GLuint index, api::GLenum pname)
{
	log() << warning << "EnableVertexAttribAPPLE not available." << std::endl;
}

static void fallbackEnableVertexAttribArray(api::GLuint index)
{
	log() << warning << "EnableVertexAttribArray not available." << std::endl;
}

static void fallbackEnableVertexAttribArrayARB(api::GLuint index)
{
	log() << warning << "EnableVertexAttribArrayARB not available." << std::endl;
}

static void fallbackEnablei(api::GLenum target, api::GLuint index)
{
	log() << warning << "Enablei not available." << std::endl;
}

static void fallbackEnd()
{
	log() << warning << "End not available." << std::endl;
}

static void fallbackEndConditionalRender()
{
	log() << warning << "EndConditionalRender not available." << std::endl;
}

static void fallbackEndConditionalRenderNV()
{
	log() << warning << "EndConditionalRenderNV not available." << std::endl;
}

static void fallbackEndFragmentShaderATI()
{
	log() << warning << "EndFragmentShaderATI not available." << std::endl;
}

static void fallbackEndList()
{
	log() << warning << "EndList not available." << std::endl;
}

static void fallbackEndOcclusionQueryNV()
{
	log() << warning << "EndOcclusionQueryNV not available." << std::endl;
}

static void fallbackEndPerfMonitorAMD(api::GLuint monitor)
{
	log() << warning << "EndPerfMonitorAMD not available." << std::endl;
}

static void fallbackEndQuery(api::GLenum target)
{
	log() << warning << "EndQuery not available." << std::endl;
}

static void fallbackEndQueryARB(api::GLenum target)
{
	log() << warning << "EndQueryARB not available." << std::endl;
}

static void fallbackEndQueryIndexed(api::GLenum target, api::GLuint index)
{
	log() << warning << "EndQueryIndexed not available." << std::endl;
}

static void fallbackEndTransformFeedback()
{
	log() << warning << "EndTransformFeedback not available." << std::endl;
}

static void fallbackEndTransformFeedbackEXT()
{
	log() << warning << "EndTransformFeedbackEXT not available." << std::endl;
}

static void fallbackEndTransformFeedbackNV()
{
	log() << warning << "EndTransformFeedbackNV not available." << std::endl;
}

static void fallbackEndVertexShaderEXT()
{
	log() << warning << "EndVertexShaderEXT not available." << std::endl;
}

static void fallbackEndVideoCaptureNV(api::GLuint video_capture_slot)
{
	log() << warning << "EndVideoCaptureNV not available." << std::endl;
}

static void fallbackEvalCoord1d(api::GLdouble u)
{
	log() << warning << "EvalCoord1d not available." << std::endl;
}

static void fallbackEvalCoord1dv(const api::GLdouble* u)
{
	log() << warning << "EvalCoord1dv not available." << std::endl;
}

static void fallbackEvalCoord1f(api::GLfloat u)
{
	log() << warning << "EvalCoord1f not available." << std::endl;
}

static void fallbackEvalCoord1fv(const api::GLfloat* u)
{
	log() << warning << "EvalCoord1fv not available." << std::endl;
}

static void fallbackEvalCoord2d(api::GLdouble u, api::GLdouble v)
{
	log() << warning << "EvalCoord2d not available." << std::endl;
}

static void fallbackEvalCoord2dv(const api::GLdouble* u)
{
	log() << warning << "EvalCoord2dv not available." << std::endl;
}

static void fallbackEvalCoord2f(api::GLfloat u, api::GLfloat v)
{
	log() << warning << "EvalCoord2f not available." << std::endl;
}

static void fallbackEvalCoord2fv(const api::GLfloat* u)
{
	log() << warning << "EvalCoord2fv not available." << std::endl;
}

static void fallbackEvalMapsNV(api::GLenum target, api::GLenum mode)
{
	log() << warning << "EvalMapsNV not available." << std::endl;
}

static void fallbackEvalMesh1(api::GLenum mode, api::GLint i1, api::GLint i2)
{
	log() << warning << "EvalMesh1 not available." << std::endl;
}

static void fallbackEvalMesh2(api::GLenum mode, api::GLint i1, api::GLint i2, api::GLint j1, api::GLint j2)
{
	log() << warning << "EvalMesh2 not available." << std::endl;
}

static void fallbackEvalPoint1(api::GLint i)
{
	log() << warning << "EvalPoint1 not available." << std::endl;
}

static void fallbackEvalPoint2(api::GLint i, api::GLint j)
{
	log() << warning << "EvalPoint2 not available." << std::endl;
}

static void fallbackExecuteProgramNV(api::GLenum target, api::GLuint id, const api::GLfloat* params)
{
	log() << warning << "ExecuteProgramNV not available." << std::endl;
}

static void fallbackExtractComponentEXT(api::GLuint res, api::GLuint src, api::GLuint num)
{
	log() << warning << "ExtractComponentEXT not available." << std::endl;
}

static void fallbackFeedbackBuffer(api::GLsizei size, api::GLenum type, api::GLfloat* buffer)
{
	log() << warning << "FeedbackBuffer not available." << std::endl;
}

static api::GLsync fallbackFenceSync(api::GLenum condition, api::GLbitfield flags)
{
	log() << warning << "FenceSync not available." << std::endl;
}

static void fallbackFinalCombinerInputNV(api::GLenum variable, api::GLenum input, api::GLenum mapping, api::GLenum componentUsage)
{
	log() << warning << "FinalCombinerInputNV not available." << std::endl;
}

static void fallbackFinish()
{
	log() << warning << "Finish not available." << std::endl;
}

static api::GLint fallbackFinishAsyncSGIX(api::GLuint* markerp)
{
	log() << warning << "FinishAsyncSGIX not available." << std::endl;
}

static void fallbackFinishFenceAPPLE(api::GLuint fence)
{
	log() << warning << "FinishFenceAPPLE not available." << std::endl;
}

static void fallbackFinishFenceNV(api::GLuint fence)
{
	log() << warning << "FinishFenceNV not available." << std::endl;
}

static void fallbackFinishObjectAPPLE(api::GLenum object, api::GLint name)
{
	log() << warning << "FinishObjectAPPLE not available." << std::endl;
}

static void fallbackFinishTextureSUNX()
{
	log() << warning << "FinishTextureSUNX not available." << std::endl;
}

static void fallbackFlush()
{
	log() << warning << "Flush not available." << std::endl;
}

static void fallbackFlushMappedBufferRange(api::GLenum target, api::GLintptr offset, api::GLsizeiptr length)
{
	log() << warning << "FlushMappedBufferRange not available." << std::endl;
}

static void fallbackFlushMappedBufferRangeAPPLE(api::GLenum target, api::GLintptr offset, api::GLsizeiptr size)
{
	log() << warning << "FlushMappedBufferRangeAPPLE not available." << std::endl;
}

static void fallbackFlushPixelDataRangeNV(api::GLenum target)
{
	log() << warning << "FlushPixelDataRangeNV not available." << std::endl;
}

static void fallbackFlushRasterSGIX()
{
	log() << warning << "FlushRasterSGIX not available." << std::endl;
}

static void fallbackFlushVertexArrayRangeAPPLE(api::GLsizei length, api::GLvoid* pointer)
{
	log() << warning << "FlushVertexArrayRangeAPPLE not available." << std::endl;
}

static void fallbackFlushVertexArrayRangeNV()
{
	log() << warning << "FlushVertexArrayRangeNV not available." << std::endl;
}

static void fallbackFogCoordFormatNV(api::GLenum type, api::GLsizei stride)
{
	log() << warning << "FogCoordFormatNV not available." << std::endl;
}

static void fallbackFogCoordPointer(api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "FogCoordPointer not available." << std::endl;
}

static void fallbackFogCoordPointerEXT(api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "FogCoordPointerEXT not available." << std::endl;
}

static void fallbackFogCoordPointerListIBM(api::GLenum type, api::GLint stride, const api::GLvoid** pointer, api::GLint ptrstride)
{
	log() << warning << "FogCoordPointerListIBM not available." << std::endl;
}

static void fallbackFogCoordd(api::GLdouble coord)
{
	log() << warning << "FogCoordd not available." << std::endl;
}

static void fallbackFogCoorddEXT(api::GLdouble coord)
{
	log() << warning << "FogCoorddEXT not available." << std::endl;
}

static void fallbackFogCoorddv(const api::GLdouble* coord)
{
	log() << warning << "FogCoorddv not available." << std::endl;
}

static void fallbackFogCoorddvEXT(const api::GLdouble* coord)
{
	log() << warning << "FogCoorddvEXT not available." << std::endl;
}

static void fallbackFogCoordf(api::GLfloat coord)
{
	log() << warning << "FogCoordf not available." << std::endl;
}

static void fallbackFogCoordfEXT(api::GLfloat coord)
{
	log() << warning << "FogCoordfEXT not available." << std::endl;
}

static void fallbackFogCoordfv(const api::GLfloat* coord)
{
	log() << warning << "FogCoordfv not available." << std::endl;
}

static void fallbackFogCoordfvEXT(const api::GLfloat* coord)
{
	log() << warning << "FogCoordfvEXT not available." << std::endl;
}

static void fallbackFogCoordhNV(api::GLhalfNV fog)
{
	log() << warning << "FogCoordhNV not available." << std::endl;
}

static void fallbackFogCoordhvNV(const api::GLhalfNV* fog)
{
	log() << warning << "FogCoordhvNV not available." << std::endl;
}

static void fallbackFogFuncSGIS(api::GLsizei n, const api::GLfloat* points)
{
	log() << warning << "FogFuncSGIS not available." << std::endl;
}

static void fallbackFogf(api::GLenum pname, api::GLfloat param)
{
	log() << warning << "Fogf not available." << std::endl;
}

static void fallbackFogfv(api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "Fogfv not available." << std::endl;
}

static void fallbackFogi(api::GLenum pname, api::GLint param)
{
	log() << warning << "Fogi not available." << std::endl;
}

static void fallbackFogiv(api::GLenum pname, const api::GLint* params)
{
	log() << warning << "Fogiv not available." << std::endl;
}

static void fallbackFragmentColorMaterialSGIX(api::GLenum face, api::GLenum mode)
{
	log() << warning << "FragmentColorMaterialSGIX not available." << std::endl;
}

static void fallbackFragmentLightModelfSGIX(api::GLenum pname, api::GLfloat param)
{
	log() << warning << "FragmentLightModelfSGIX not available." << std::endl;
}

static void fallbackFragmentLightModelfvSGIX(api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "FragmentLightModelfvSGIX not available." << std::endl;
}

static void fallbackFragmentLightModeliSGIX(api::GLenum pname, api::GLint param)
{
	log() << warning << "FragmentLightModeliSGIX not available." << std::endl;
}

static void fallbackFragmentLightModelivSGIX(api::GLenum pname, const api::GLint* params)
{
	log() << warning << "FragmentLightModelivSGIX not available." << std::endl;
}

static void fallbackFragmentLightfSGIX(api::GLenum light, api::GLenum pname, api::GLfloat param)
{
	log() << warning << "FragmentLightfSGIX not available." << std::endl;
}

static void fallbackFragmentLightfvSGIX(api::GLenum light, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "FragmentLightfvSGIX not available." << std::endl;
}

static void fallbackFragmentLightiSGIX(api::GLenum light, api::GLenum pname, api::GLint param)
{
	log() << warning << "FragmentLightiSGIX not available." << std::endl;
}

static void fallbackFragmentLightivSGIX(api::GLenum light, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "FragmentLightivSGIX not available." << std::endl;
}

static void fallbackFragmentMaterialfSGIX(api::GLenum face, api::GLenum pname, api::GLfloat param)
{
	log() << warning << "FragmentMaterialfSGIX not available." << std::endl;
}

static void fallbackFragmentMaterialfvSGIX(api::GLenum face, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "FragmentMaterialfvSGIX not available." << std::endl;
}

static void fallbackFragmentMaterialiSGIX(api::GLenum face, api::GLenum pname, api::GLint param)
{
	log() << warning << "FragmentMaterialiSGIX not available." << std::endl;
}

static void fallbackFragmentMaterialivSGIX(api::GLenum face, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "FragmentMaterialivSGIX not available." << std::endl;
}

static void fallbackFrameTerminatorGREMEDY()
{
	log() << warning << "FrameTerminatorGREMEDY not available." << std::endl;
}

static void fallbackFrameZoomSGIX(api::GLint factor)
{
	log() << warning << "FrameZoomSGIX not available." << std::endl;
}

static void fallbackFramebufferDrawBufferEXT(api::GLuint framebuffer, api::GLenum mode)
{
	log() << warning << "FramebufferDrawBufferEXT not available." << std::endl;
}

static void fallbackFramebufferDrawBuffersEXT(api::GLuint framebuffer, api::GLsizei n, const api::GLenum* bufs)
{
	log() << warning << "FramebufferDrawBuffersEXT not available." << std::endl;
}

static void fallbackFramebufferReadBufferEXT(api::GLuint framebuffer, api::GLenum mode)
{
	log() << warning << "FramebufferReadBufferEXT not available." << std::endl;
}

static void fallbackFramebufferRenderbuffer(api::GLenum target, api::GLenum attachment, api::GLenum renderbuffertarget, api::GLuint renderbuffer)
{
	log() << warning << "FramebufferRenderbuffer not available." << std::endl;
}

static void fallbackFramebufferRenderbufferEXT(api::GLenum target, api::GLenum attachment, api::GLenum renderbuffertarget, api::GLuint renderbuffer)
{
	log() << warning << "FramebufferRenderbufferEXT not available." << std::endl;
}

static void fallbackFramebufferTexture(api::GLenum target, api::GLenum attachment, api::GLuint texture, api::GLint level)
{
	log() << warning << "FramebufferTexture not available." << std::endl;
}

static void fallbackFramebufferTexture1D(api::GLenum target, api::GLenum attachment, api::GLenum textarget, api::GLuint texture, api::GLint level)
{
	log() << warning << "FramebufferTexture1D not available." << std::endl;
}

static void fallbackFramebufferTexture1DEXT(api::GLenum target, api::GLenum attachment, api::GLenum textarget, api::GLuint texture, api::GLint level)
{
	log() << warning << "FramebufferTexture1DEXT not available." << std::endl;
}

static void fallbackFramebufferTexture2D(api::GLenum target, api::GLenum attachment, api::GLenum textarget, api::GLuint texture, api::GLint level)
{
	log() << warning << "FramebufferTexture2D not available." << std::endl;
}

static void fallbackFramebufferTexture2DEXT(api::GLenum target, api::GLenum attachment, api::GLenum textarget, api::GLuint texture, api::GLint level)
{
	log() << warning << "FramebufferTexture2DEXT not available." << std::endl;
}

static void fallbackFramebufferTexture3D(api::GLenum target, api::GLenum attachment, api::GLenum textarget, api::GLuint texture, api::GLint level, api::GLint zoffset)
{
	log() << warning << "FramebufferTexture3D not available." << std::endl;
}

static void fallbackFramebufferTexture3DEXT(api::GLenum target, api::GLenum attachment, api::GLenum textarget, api::GLuint texture, api::GLint level, api::GLint zoffset)
{
	log() << warning << "FramebufferTexture3DEXT not available." << std::endl;
}

static void fallbackFramebufferTextureARB(api::GLenum target, api::GLenum attachment, api::GLuint texture, api::GLint level)
{
	log() << warning << "FramebufferTextureARB not available." << std::endl;
}

static void fallbackFramebufferTextureEXT(api::GLenum target, api::GLenum attachment, api::GLuint texture, api::GLint level)
{
	log() << warning << "FramebufferTextureEXT not available." << std::endl;
}

static void fallbackFramebufferTextureFaceARB(api::GLenum target, api::GLenum attachment, api::GLuint texture, api::GLint level, api::GLenum face)
{
	log() << warning << "FramebufferTextureFaceARB not available." << std::endl;
}

static void fallbackFramebufferTextureFaceEXT(api::GLenum target, api::GLenum attachment, api::GLuint texture, api::GLint level, api::GLenum face)
{
	log() << warning << "FramebufferTextureFaceEXT not available." << std::endl;
}

static void fallbackFramebufferTextureLayer(api::GLenum target, api::GLenum attachment, api::GLuint texture, api::GLint level, api::GLint layer)
{
	log() << warning << "FramebufferTextureLayer not available." << std::endl;
}

static void fallbackFramebufferTextureLayerARB(api::GLenum target, api::GLenum attachment, api::GLuint texture, api::GLint level, api::GLint layer)
{
	log() << warning << "FramebufferTextureLayerARB not available." << std::endl;
}

static void fallbackFramebufferTextureLayerEXT(api::GLenum target, api::GLenum attachment, api::GLuint texture, api::GLint level, api::GLint layer)
{
	log() << warning << "FramebufferTextureLayerEXT not available." << std::endl;
}

static void fallbackFreeObjectBufferATI(api::GLuint buffer)
{
	log() << warning << "FreeObjectBufferATI not available." << std::endl;
}

static void fallbackFrontFace(api::GLenum mode)
{
	log() << warning << "FrontFace not available." << std::endl;
}

static void fallbackFrustum(api::GLdouble left, api::GLdouble right, api::GLdouble bottom, api::GLdouble top, api::GLdouble zNear, api::GLdouble zFar)
{
	log() << warning << "Frustum not available." << std::endl;
}

static api::GLuint fallbackGenAsyncMarkersSGIX(api::GLsizei range)
{
	log() << warning << "GenAsyncMarkersSGIX not available." << std::endl;
}

static void fallbackGenBuffers(api::GLsizei n, api::GLuint* buffers)
{
	log() << warning << "GenBuffers not available." << std::endl;
}

static void fallbackGenBuffersARB(api::GLsizei n, api::GLuint* buffers)
{
	log() << warning << "GenBuffersARB not available." << std::endl;
}

static void fallbackGenFencesAPPLE(api::GLsizei n, api::GLuint* fences)
{
	log() << warning << "GenFencesAPPLE not available." << std::endl;
}

static void fallbackGenFencesNV(api::GLsizei n, api::GLuint* fences)
{
	log() << warning << "GenFencesNV not available." << std::endl;
}

static api::GLuint fallbackGenFragmentShadersATI(api::GLuint range)
{
	log() << warning << "GenFragmentShadersATI not available." << std::endl;
}

static void fallbackGenFramebuffers(api::GLsizei n, api::GLuint* framebuffers)
{
	log() << warning << "GenFramebuffers not available." << std::endl;
}

static void fallbackGenFramebuffersEXT(api::GLsizei n, api::GLuint* framebuffers)
{
	log() << warning << "GenFramebuffersEXT not available." << std::endl;
}

static api::GLuint fallbackGenLists(api::GLsizei range)
{
	log() << warning << "GenLists not available." << std::endl;
}

static void fallbackGenOcclusionQueriesNV(api::GLsizei n, api::GLuint* ids)
{
	log() << warning << "GenOcclusionQueriesNV not available." << std::endl;
}

static void fallbackGenPerfMonitorsAMD(api::GLsizei n, api::GLuint* monitors)
{
	log() << warning << "GenPerfMonitorsAMD not available." << std::endl;
}

static void fallbackGenProgramsARB(api::GLsizei n, api::GLuint* programs)
{
	log() << warning << "GenProgramsARB not available." << std::endl;
}

static void fallbackGenProgramsNV(api::GLsizei n, api::GLuint* programs)
{
	log() << warning << "GenProgramsNV not available." << std::endl;
}

static void fallbackGenQueries(api::GLsizei n, api::GLuint* ids)
{
	log() << warning << "GenQueries not available." << std::endl;
}

static void fallbackGenQueriesARB(api::GLsizei n, api::GLuint* ids)
{
	log() << warning << "GenQueriesARB not available." << std::endl;
}

static void fallbackGenRenderbuffers(api::GLsizei n, api::GLuint* renderbuffers)
{
	log() << warning << "GenRenderbuffers not available." << std::endl;
}

static void fallbackGenRenderbuffersEXT(api::GLsizei n, api::GLuint* renderbuffers)
{
	log() << warning << "GenRenderbuffersEXT not available." << std::endl;
}

static void fallbackGenSamplers(api::GLsizei count, api::GLuint* samplers)
{
	log() << warning << "GenSamplers not available." << std::endl;
}

static api::GLuint fallbackGenSymbolsEXT(api::GLenum datatype, api::GLenum storagetype, api::GLenum range, api::GLuint components)
{
	log() << warning << "GenSymbolsEXT not available." << std::endl;
}

static void fallbackGenTextures(api::GLsizei n, api::GLuint* textures)
{
	log() << warning << "GenTextures not available." << std::endl;
}

static void fallbackGenTexturesEXT(api::GLsizei n, api::GLuint* textures)
{
	log() << warning << "GenTexturesEXT not available." << std::endl;
}

static void fallbackGenTransformFeedbacks(api::GLsizei n, api::GLuint* ids)
{
	log() << warning << "GenTransformFeedbacks not available." << std::endl;
}

static void fallbackGenTransformFeedbacksNV(api::GLsizei n, api::GLuint* ids)
{
	log() << warning << "GenTransformFeedbacksNV not available." << std::endl;
}

static void fallbackGenVertexArrays(api::GLsizei n, api::GLuint* arrays)
{
	log() << warning << "GenVertexArrays not available." << std::endl;
}

static void fallbackGenVertexArraysAPPLE(api::GLsizei n, api::GLuint* arrays)
{
	log() << warning << "GenVertexArraysAPPLE not available." << std::endl;
}

static api::GLuint fallbackGenVertexShadersEXT(api::GLuint range)
{
	log() << warning << "GenVertexShadersEXT not available." << std::endl;
}

static void fallbackGenerateMipmap(api::GLenum target)
{
	log() << warning << "GenerateMipmap not available." << std::endl;
}

static void fallbackGenerateMipmapEXT(api::GLenum target)
{
	log() << warning << "GenerateMipmapEXT not available." << std::endl;
}

static void fallbackGenerateMultiTexMipmapEXT(api::GLenum texunit, api::GLenum target)
{
	log() << warning << "GenerateMultiTexMipmapEXT not available." << std::endl;
}

static void fallbackGenerateTextureMipmapEXT(api::GLuint texture, api::GLenum target)
{
	log() << warning << "GenerateTextureMipmapEXT not available." << std::endl;
}

static void fallbackGetActiveAttrib(api::GLuint program, api::GLuint index, api::GLsizei bufSize, api::GLsizei* length, api::GLint* size, api::GLenum* type, api::GLchar* name)
{
	log() << warning << "GetActiveAttrib not available." << std::endl;
}

static void fallbackGetActiveAttribARB(api::GLhandleARB programObj, api::GLuint index, api::GLsizei maxLength, api::GLsizei* length, api::GLint* size, api::GLenum* type, api::GLcharARB* name)
{
	log() << warning << "GetActiveAttribARB not available." << std::endl;
}

static void fallbackGetActiveSubroutineName(api::GLuint program, api::GLenum shadertype, api::GLuint index, api::GLsizei bufsize, api::GLsizei* length, api::GLchar* name)
{
	log() << warning << "GetActiveSubroutineName not available." << std::endl;
}

static void fallbackGetActiveSubroutineUniformName(api::GLuint program, api::GLenum shadertype, api::GLuint index, api::GLsizei bufsize, api::GLsizei* length, api::GLchar* name)
{
	log() << warning << "GetActiveSubroutineUniformName not available." << std::endl;
}

static void fallbackGetActiveSubroutineUniformiv(api::GLuint program, api::GLenum shadertype, api::GLuint index, api::GLenum pname, api::GLint* values)
{
	log() << warning << "GetActiveSubroutineUniformiv not available." << std::endl;
}

static void fallbackGetActiveUniform(api::GLuint program, api::GLuint index, api::GLsizei bufSize, api::GLsizei* length, api::GLint* size, api::GLenum* type, api::GLchar* name)
{
	log() << warning << "GetActiveUniform not available." << std::endl;
}

static void fallbackGetActiveUniformARB(api::GLhandleARB programObj, api::GLuint index, api::GLsizei maxLength, api::GLsizei* length, api::GLint* size, api::GLenum* type, api::GLcharARB* name)
{
	log() << warning << "GetActiveUniformARB not available." << std::endl;
}

static void fallbackGetActiveUniformBlockName(api::GLuint program, api::GLuint uniformBlockIndex, api::GLsizei bufSize, api::GLsizei* length, api::GLchar* uniformBlockName)
{
	log() << warning << "GetActiveUniformBlockName not available." << std::endl;
}

static void fallbackGetActiveUniformBlockiv(api::GLuint program, api::GLuint uniformBlockIndex, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetActiveUniformBlockiv not available." << std::endl;
}

static void fallbackGetActiveUniformName(api::GLuint program, api::GLuint uniformIndex, api::GLsizei bufSize, api::GLsizei* length, api::GLchar* uniformName)
{
	log() << warning << "GetActiveUniformName not available." << std::endl;
}

static void fallbackGetActiveUniformsiv(api::GLuint program, api::GLsizei uniformCount, const api::GLuint* uniformIndices, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetActiveUniformsiv not available." << std::endl;
}

static void fallbackGetActiveVaryingNV(api::GLuint program, api::GLuint index, api::GLsizei bufSize, api::GLsizei* length, api::GLsizei* size, api::GLenum* type, api::GLchar* name)
{
	log() << warning << "GetActiveVaryingNV not available." << std::endl;
}

static void fallbackGetArrayObjectfvATI(api::GLenum array, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetArrayObjectfvATI not available." << std::endl;
}

static void fallbackGetArrayObjectivATI(api::GLenum array, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetArrayObjectivATI not available." << std::endl;
}

static void fallbackGetAttachedObjectsARB(api::GLhandleARB containerObj, api::GLsizei maxCount, api::GLsizei* count, api::GLhandleARB* obj)
{
	log() << warning << "GetAttachedObjectsARB not available." << std::endl;
}

static void fallbackGetAttachedShaders(api::GLuint program, api::GLsizei maxCount, api::GLsizei* count, api::GLuint* obj)
{
	log() << warning << "GetAttachedShaders not available." << std::endl;
}

static api::GLint fallbackGetAttribLocation(api::GLuint program, const api::GLchar* name)
{
	log() << warning << "GetAttribLocation not available." << std::endl;
}

static api::GLint fallbackGetAttribLocationARB(api::GLhandleARB programObj, const api::GLcharARB* name)
{
	log() << warning << "GetAttribLocationARB not available." << std::endl;
}

static void fallbackGetBooleanIndexedvEXT(api::GLenum target, api::GLuint index, api::GLboolean* data)
{
	log() << warning << "GetBooleanIndexedvEXT not available." << std::endl;
}

static void fallbackGetBooleani_v(api::GLenum target, api::GLuint index, api::GLboolean* data)
{
	log() << warning << "GetBooleani_v not available." << std::endl;
}

static void fallbackGetBooleanv(api::GLenum pname, api::GLboolean* params)
{
	log() << warning << "GetBooleanv not available." << std::endl;
}

static void fallbackGetBufferParameteri64v(api::GLenum target, api::GLenum pname, api::GLint64* params)
{
	log() << warning << "GetBufferParameteri64v not available." << std::endl;
}

static void fallbackGetBufferParameteriv(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetBufferParameteriv not available." << std::endl;
}

static void fallbackGetBufferParameterivARB(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetBufferParameterivARB not available." << std::endl;
}

static void fallbackGetBufferParameterui64vNV(api::GLenum target, api::GLenum pname, api::GLuint64EXT* params)
{
	log() << warning << "GetBufferParameterui64vNV not available." << std::endl;
}

static void fallbackGetBufferPointerv(api::GLenum target, api::GLenum pname, api::GLvoid** params)
{
	log() << warning << "GetBufferPointerv not available." << std::endl;
}

static void fallbackGetBufferPointervARB(api::GLenum target, api::GLenum pname, api::GLvoid** params)
{
	log() << warning << "GetBufferPointervARB not available." << std::endl;
}

static void fallbackGetBufferSubData(api::GLenum target, api::GLintptr offset, api::GLsizeiptr size, api::GLvoid* data)
{
	log() << warning << "GetBufferSubData not available." << std::endl;
}

static void fallbackGetBufferSubDataARB(api::GLenum target, api::GLintptrARB offset, api::GLsizeiptrARB size, api::GLvoid* data)
{
	log() << warning << "GetBufferSubDataARB not available." << std::endl;
}

static void fallbackGetClipPlane(api::GLenum plane, api::GLdouble* equation)
{
	log() << warning << "GetClipPlane not available." << std::endl;
}

static void fallbackGetColorTable(api::GLenum target, api::GLenum format, api::GLenum type, api::GLvoid* table)
{
	log() << warning << "GetColorTable not available." << std::endl;
}

static void fallbackGetColorTableEXT(api::GLenum target, api::GLenum format, api::GLenum type, api::GLvoid* data)
{
	log() << warning << "GetColorTableEXT not available." << std::endl;
}

static void fallbackGetColorTableParameterfv(api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetColorTableParameterfv not available." << std::endl;
}

static void fallbackGetColorTableParameterfvEXT(api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetColorTableParameterfvEXT not available." << std::endl;
}

static void fallbackGetColorTableParameterfvSGI(api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetColorTableParameterfvSGI not available." << std::endl;
}

static void fallbackGetColorTableParameteriv(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetColorTableParameteriv not available." << std::endl;
}

static void fallbackGetColorTableParameterivEXT(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetColorTableParameterivEXT not available." << std::endl;
}

static void fallbackGetColorTableParameterivSGI(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetColorTableParameterivSGI not available." << std::endl;
}

static void fallbackGetColorTableSGI(api::GLenum target, api::GLenum format, api::GLenum type, api::GLvoid* table)
{
	log() << warning << "GetColorTableSGI not available." << std::endl;
}

static void fallbackGetCombinerInputParameterfvNV(api::GLenum stage, api::GLenum portion, api::GLenum variable, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetCombinerInputParameterfvNV not available." << std::endl;
}

static void fallbackGetCombinerInputParameterivNV(api::GLenum stage, api::GLenum portion, api::GLenum variable, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetCombinerInputParameterivNV not available." << std::endl;
}

static void fallbackGetCombinerOutputParameterfvNV(api::GLenum stage, api::GLenum portion, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetCombinerOutputParameterfvNV not available." << std::endl;
}

static void fallbackGetCombinerOutputParameterivNV(api::GLenum stage, api::GLenum portion, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetCombinerOutputParameterivNV not available." << std::endl;
}

static void fallbackGetCombinerStageParameterfvNV(api::GLenum stage, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetCombinerStageParameterfvNV not available." << std::endl;
}

static void fallbackGetCompressedMultiTexImageEXT(api::GLenum texunit, api::GLenum target, api::GLint lod, api::GLvoid* img)
{
	log() << warning << "GetCompressedMultiTexImageEXT not available." << std::endl;
}

static void fallbackGetCompressedTexImage(api::GLenum target, api::GLint level, api::GLvoid* img)
{
	log() << warning << "GetCompressedTexImage not available." << std::endl;
}

static void fallbackGetCompressedTexImageARB(api::GLenum target, api::GLint level, api::GLvoid* img)
{
	log() << warning << "GetCompressedTexImageARB not available." << std::endl;
}

static void fallbackGetCompressedTextureImageEXT(api::GLuint texture, api::GLenum target, api::GLint lod, api::GLvoid* img)
{
	log() << warning << "GetCompressedTextureImageEXT not available." << std::endl;
}

static void fallbackGetConvolutionFilter(api::GLenum target, api::GLenum format, api::GLenum type, api::GLvoid* image)
{
	log() << warning << "GetConvolutionFilter not available." << std::endl;
}

static void fallbackGetConvolutionFilterEXT(api::GLenum target, api::GLenum format, api::GLenum type, api::GLvoid* image)
{
	log() << warning << "GetConvolutionFilterEXT not available." << std::endl;
}

static void fallbackGetConvolutionParameterfv(api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetConvolutionParameterfv not available." << std::endl;
}

static void fallbackGetConvolutionParameterfvEXT(api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetConvolutionParameterfvEXT not available." << std::endl;
}

static void fallbackGetConvolutionParameteriv(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetConvolutionParameteriv not available." << std::endl;
}

static void fallbackGetConvolutionParameterivEXT(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetConvolutionParameterivEXT not available." << std::endl;
}

static void fallbackGetDetailTexFuncSGIS(api::GLenum target, api::GLfloat* points)
{
	log() << warning << "GetDetailTexFuncSGIS not available." << std::endl;
}

static void fallbackGetDoubleIndexedvEXT(api::GLenum target, api::GLuint index, api::GLdouble* data)
{
	log() << warning << "GetDoubleIndexedvEXT not available." << std::endl;
}

static void fallbackGetDoublev(api::GLenum pname, api::GLdouble* params)
{
	log() << warning << "GetDoublev not available." << std::endl;
}

static api::GLenum fallbackGetError()
{
	log() << warning << "GetError not available." << std::endl;
}

static void fallbackGetFenceivNV(api::GLuint fence, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetFenceivNV not available." << std::endl;
}

static void fallbackGetFinalCombinerInputParameterfvNV(api::GLenum variable, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetFinalCombinerInputParameterfvNV not available." << std::endl;
}

static void fallbackGetFinalCombinerInputParameterivNV(api::GLenum variable, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetFinalCombinerInputParameterivNV not available." << std::endl;
}

static void fallbackGetFloatIndexedvEXT(api::GLenum target, api::GLuint index, api::GLfloat* data)
{
	log() << warning << "GetFloatIndexedvEXT not available." << std::endl;
}

static void fallbackGetFloatv(api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetFloatv not available." << std::endl;
}

static void fallbackGetFogFuncSGIS(api::GLfloat* points)
{
	log() << warning << "GetFogFuncSGIS not available." << std::endl;
}

static api::GLint fallbackGetFragDataIndex(api::GLuint program, const api::GLchar* name)
{
	log() << warning << "GetFragDataIndex not available." << std::endl;
}

static api::GLint fallbackGetFragDataLocation(api::GLuint program, const api::GLchar* name)
{
	log() << warning << "GetFragDataLocation not available." << std::endl;
}

static api::GLint fallbackGetFragDataLocationEXT(api::GLuint program, const api::GLchar* name)
{
	log() << warning << "GetFragDataLocationEXT not available." << std::endl;
}

static void fallbackGetFragmentLightfvSGIX(api::GLenum light, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetFragmentLightfvSGIX not available." << std::endl;
}

static void fallbackGetFragmentLightivSGIX(api::GLenum light, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetFragmentLightivSGIX not available." << std::endl;
}

static void fallbackGetFragmentMaterialfvSGIX(api::GLenum face, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetFragmentMaterialfvSGIX not available." << std::endl;
}

static void fallbackGetFragmentMaterialivSGIX(api::GLenum face, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetFragmentMaterialivSGIX not available." << std::endl;
}

static void fallbackGetFramebufferAttachmentParameteriv(api::GLenum target, api::GLenum attachment, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetFramebufferAttachmentParameteriv not available." << std::endl;
}

static void fallbackGetFramebufferAttachmentParameterivEXT(api::GLenum target, api::GLenum attachment, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetFramebufferAttachmentParameterivEXT not available." << std::endl;
}

static void fallbackGetFramebufferParameterivEXT(api::GLuint framebuffer, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetFramebufferParameterivEXT not available." << std::endl;
}

static api::GLhandleARB fallbackGetHandleARB(api::GLenum pname)
{
	log() << warning << "GetHandleARB not available." << std::endl;
}

static void fallbackGetHistogram(api::GLenum target, api::GLboolean reset, api::GLenum format, api::GLenum type, api::GLvoid* values)
{
	log() << warning << "GetHistogram not available." << std::endl;
}

static void fallbackGetHistogramEXT(api::GLenum target, api::GLboolean reset, api::GLenum format, api::GLenum type, api::GLvoid* values)
{
	log() << warning << "GetHistogramEXT not available." << std::endl;
}

static void fallbackGetHistogramParameterfv(api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetHistogramParameterfv not available." << std::endl;
}

static void fallbackGetHistogramParameterfvEXT(api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetHistogramParameterfvEXT not available." << std::endl;
}

static void fallbackGetHistogramParameteriv(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetHistogramParameteriv not available." << std::endl;
}

static void fallbackGetHistogramParameterivEXT(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetHistogramParameterivEXT not available." << std::endl;
}

static void fallbackGetImageTransformParameterfvHP(api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetImageTransformParameterfvHP not available." << std::endl;
}

static void fallbackGetImageTransformParameterivHP(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetImageTransformParameterivHP not available." << std::endl;
}

static void fallbackGetInfoLogARB(api::GLhandleARB obj, api::GLsizei maxLength, api::GLsizei* length, api::GLcharARB* infoLog)
{
	log() << warning << "GetInfoLogARB not available." << std::endl;
}

static api::GLint fallbackGetInstrumentsSGIX()
{
	log() << warning << "GetInstrumentsSGIX not available." << std::endl;
}

static void fallbackGetInteger64i_v(api::GLenum target, api::GLuint index, api::GLint64* data)
{
	log() << warning << "GetInteger64i_v not available." << std::endl;
}

static void fallbackGetInteger64v(api::GLenum pname, api::GLint64* params)
{
	log() << warning << "GetInteger64v not available." << std::endl;
}

static void fallbackGetIntegerIndexedvEXT(api::GLenum target, api::GLuint index, api::GLint* data)
{
	log() << warning << "GetIntegerIndexedvEXT not available." << std::endl;
}

static void fallbackGetIntegeri_v(api::GLenum target, api::GLuint index, api::GLint* data)
{
	log() << warning << "GetIntegeri_v not available." << std::endl;
}

static void fallbackGetIntegerui64i_vNV(api::GLenum value, api::GLuint index, api::GLuint64EXT* result)
{
	log() << warning << "GetIntegerui64i_vNV not available." << std::endl;
}

static void fallbackGetIntegerui64vNV(api::GLenum value, api::GLuint64EXT* result)
{
	log() << warning << "GetIntegerui64vNV not available." << std::endl;
}

static void fallbackGetIntegerv(api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetIntegerv not available." << std::endl;
}

static void fallbackGetInvariantBooleanvEXT(api::GLuint id, api::GLenum value, api::GLboolean* data)
{
	log() << warning << "GetInvariantBooleanvEXT not available." << std::endl;
}

static void fallbackGetInvariantFloatvEXT(api::GLuint id, api::GLenum value, api::GLfloat* data)
{
	log() << warning << "GetInvariantFloatvEXT not available." << std::endl;
}

static void fallbackGetInvariantIntegervEXT(api::GLuint id, api::GLenum value, api::GLint* data)
{
	log() << warning << "GetInvariantIntegervEXT not available." << std::endl;
}

static void fallbackGetLightfv(api::GLenum light, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetLightfv not available." << std::endl;
}

static void fallbackGetLightiv(api::GLenum light, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetLightiv not available." << std::endl;
}

static void fallbackGetListParameterfvSGIX(api::GLuint list, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetListParameterfvSGIX not available." << std::endl;
}

static void fallbackGetListParameterivSGIX(api::GLuint list, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetListParameterivSGIX not available." << std::endl;
}

static void fallbackGetLocalConstantBooleanvEXT(api::GLuint id, api::GLenum value, api::GLboolean* data)
{
	log() << warning << "GetLocalConstantBooleanvEXT not available." << std::endl;
}

static void fallbackGetLocalConstantFloatvEXT(api::GLuint id, api::GLenum value, api::GLfloat* data)
{
	log() << warning << "GetLocalConstantFloatvEXT not available." << std::endl;
}

static void fallbackGetLocalConstantIntegervEXT(api::GLuint id, api::GLenum value, api::GLint* data)
{
	log() << warning << "GetLocalConstantIntegervEXT not available." << std::endl;
}

static void fallbackGetMapAttribParameterfvNV(api::GLenum target, api::GLuint index, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetMapAttribParameterfvNV not available." << std::endl;
}

static void fallbackGetMapAttribParameterivNV(api::GLenum target, api::GLuint index, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetMapAttribParameterivNV not available." << std::endl;
}

static void fallbackGetMapControlPointsNV(api::GLenum target, api::GLuint index, api::GLenum type, api::GLsizei ustride, api::GLsizei vstride, api::GLboolean packed, api::GLvoid* points)
{
	log() << warning << "GetMapControlPointsNV not available." << std::endl;
}

static void fallbackGetMapParameterfvNV(api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetMapParameterfvNV not available." << std::endl;
}

static void fallbackGetMapParameterivNV(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetMapParameterivNV not available." << std::endl;
}

static void fallbackGetMapdv(api::GLenum target, api::GLenum query, api::GLdouble* v)
{
	log() << warning << "GetMapdv not available." << std::endl;
}

static void fallbackGetMapfv(api::GLenum target, api::GLenum query, api::GLfloat* v)
{
	log() << warning << "GetMapfv not available." << std::endl;
}

static void fallbackGetMapiv(api::GLenum target, api::GLenum query, api::GLint* v)
{
	log() << warning << "GetMapiv not available." << std::endl;
}

static void fallbackGetMaterialfv(api::GLenum face, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetMaterialfv not available." << std::endl;
}

static void fallbackGetMaterialiv(api::GLenum face, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetMaterialiv not available." << std::endl;
}

static void fallbackGetMinmax(api::GLenum target, api::GLboolean reset, api::GLenum format, api::GLenum type, api::GLvoid* values)
{
	log() << warning << "GetMinmax not available." << std::endl;
}

static void fallbackGetMinmaxEXT(api::GLenum target, api::GLboolean reset, api::GLenum format, api::GLenum type, api::GLvoid* values)
{
	log() << warning << "GetMinmaxEXT not available." << std::endl;
}

static void fallbackGetMinmaxParameterfv(api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetMinmaxParameterfv not available." << std::endl;
}

static void fallbackGetMinmaxParameterfvEXT(api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetMinmaxParameterfvEXT not available." << std::endl;
}

static void fallbackGetMinmaxParameteriv(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetMinmaxParameteriv not available." << std::endl;
}

static void fallbackGetMinmaxParameterivEXT(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetMinmaxParameterivEXT not available." << std::endl;
}

static void fallbackGetMultiTexEnvfvEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetMultiTexEnvfvEXT not available." << std::endl;
}

static void fallbackGetMultiTexEnvivEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetMultiTexEnvivEXT not available." << std::endl;
}

static void fallbackGetMultiTexGendvEXT(api::GLenum texunit, api::GLenum coord, api::GLenum pname, api::GLdouble* params)
{
	log() << warning << "GetMultiTexGendvEXT not available." << std::endl;
}

static void fallbackGetMultiTexGenfvEXT(api::GLenum texunit, api::GLenum coord, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetMultiTexGenfvEXT not available." << std::endl;
}

static void fallbackGetMultiTexGenivEXT(api::GLenum texunit, api::GLenum coord, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetMultiTexGenivEXT not available." << std::endl;
}

static void fallbackGetMultiTexImageEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLenum format, api::GLenum type, api::GLvoid* pixels)
{
	log() << warning << "GetMultiTexImageEXT not available." << std::endl;
}

static void fallbackGetMultiTexLevelParameterfvEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetMultiTexLevelParameterfvEXT not available." << std::endl;
}

static void fallbackGetMultiTexLevelParameterivEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetMultiTexLevelParameterivEXT not available." << std::endl;
}

static void fallbackGetMultiTexParameterIivEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetMultiTexParameterIivEXT not available." << std::endl;
}

static void fallbackGetMultiTexParameterIuivEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, api::GLuint* params)
{
	log() << warning << "GetMultiTexParameterIuivEXT not available." << std::endl;
}

static void fallbackGetMultiTexParameterfvEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetMultiTexParameterfvEXT not available." << std::endl;
}

static void fallbackGetMultiTexParameterivEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetMultiTexParameterivEXT not available." << std::endl;
}

static void fallbackGetMultisamplefv(api::GLenum pname, api::GLuint index, api::GLfloat* val)
{
	log() << warning << "GetMultisamplefv not available." << std::endl;
}

static void fallbackGetMultisamplefvNV(api::GLenum pname, api::GLuint index, api::GLfloat* val)
{
	log() << warning << "GetMultisamplefvNV not available." << std::endl;
}

static void fallbackGetNamedBufferParameterivEXT(api::GLuint buffer, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetNamedBufferParameterivEXT not available." << std::endl;
}

static void fallbackGetNamedBufferParameterui64vNV(api::GLuint buffer, api::GLenum pname, api::GLuint64EXT* params)
{
	log() << warning << "GetNamedBufferParameterui64vNV not available." << std::endl;
}

static void fallbackGetNamedBufferPointervEXT(api::GLuint buffer, api::GLenum pname, api::GLvoid** params)
{
	log() << warning << "GetNamedBufferPointervEXT not available." << std::endl;
}

static void fallbackGetNamedBufferSubDataEXT(api::GLuint buffer, api::GLintptr offset, api::GLsizeiptr size, api::GLvoid* data)
{
	log() << warning << "GetNamedBufferSubDataEXT not available." << std::endl;
}

static void fallbackGetNamedFramebufferAttachmentParameterivEXT(api::GLuint framebuffer, api::GLenum attachment, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetNamedFramebufferAttachmentParameterivEXT not available." << std::endl;
}

static void fallbackGetNamedProgramLocalParameterIivEXT(api::GLuint program, api::GLenum target, api::GLuint index, api::GLint* params)
{
	log() << warning << "GetNamedProgramLocalParameterIivEXT not available." << std::endl;
}

static void fallbackGetNamedProgramLocalParameterIuivEXT(api::GLuint program, api::GLenum target, api::GLuint index, api::GLuint* params)
{
	log() << warning << "GetNamedProgramLocalParameterIuivEXT not available." << std::endl;
}

static void fallbackGetNamedProgramLocalParameterdvEXT(api::GLuint program, api::GLenum target, api::GLuint index, api::GLdouble* params)
{
	log() << warning << "GetNamedProgramLocalParameterdvEXT not available." << std::endl;
}

static void fallbackGetNamedProgramLocalParameterfvEXT(api::GLuint program, api::GLenum target, api::GLuint index, api::GLfloat* params)
{
	log() << warning << "GetNamedProgramLocalParameterfvEXT not available." << std::endl;
}

static void fallbackGetNamedProgramStringEXT(api::GLuint program, api::GLenum target, api::GLenum pname, api::GLvoid* string)
{
	log() << warning << "GetNamedProgramStringEXT not available." << std::endl;
}

static void fallbackGetNamedProgramivEXT(api::GLuint program, api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetNamedProgramivEXT not available." << std::endl;
}

static void fallbackGetNamedRenderbufferParameterivEXT(api::GLuint renderbuffer, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetNamedRenderbufferParameterivEXT not available." << std::endl;
}

static void fallbackGetNamedStringARB(api::GLint namelen, const api::GLchar* name, api::GLsizei bufSize, api::GLint* stringlen, api::GLchar* string)
{
	log() << warning << "GetNamedStringARB not available." << std::endl;
}

static void fallbackGetNamedStringivARB(api::GLint namelen, const api::GLchar* name, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetNamedStringivARB not available." << std::endl;
}

static void fallbackGetObjectBufferfvATI(api::GLuint buffer, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetObjectBufferfvATI not available." << std::endl;
}

static void fallbackGetObjectBufferivATI(api::GLuint buffer, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetObjectBufferivATI not available." << std::endl;
}

static void fallbackGetObjectParameterfvARB(api::GLhandleARB obj, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetObjectParameterfvARB not available." << std::endl;
}

static void fallbackGetObjectParameterivAPPLE(api::GLenum objectType, api::GLuint name, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetObjectParameterivAPPLE not available." << std::endl;
}

static void fallbackGetObjectParameterivARB(api::GLhandleARB obj, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetObjectParameterivARB not available." << std::endl;
}

static void fallbackGetOcclusionQueryivNV(api::GLuint id, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetOcclusionQueryivNV not available." << std::endl;
}

static void fallbackGetOcclusionQueryuivNV(api::GLuint id, api::GLenum pname, api::GLuint* params)
{
	log() << warning << "GetOcclusionQueryuivNV not available." << std::endl;
}

static void fallbackGetPerfMonitorCounterDataAMD(api::GLuint monitor, api::GLenum pname, api::GLsizei dataSize, api::GLuint* data, api::GLint* bytesWritten)
{
	log() << warning << "GetPerfMonitorCounterDataAMD not available." << std::endl;
}

static void fallbackGetPerfMonitorCounterInfoAMD(api::GLuint group, api::GLuint counter, api::GLenum pname, void* data)
{
	log() << warning << "GetPerfMonitorCounterInfoAMD not available." << std::endl;
}

static void fallbackGetPerfMonitorCounterStringAMD(api::GLuint group, api::GLuint counter, api::GLsizei bufSize, api::GLsizei* length, api::GLchar* counterString)
{
	log() << warning << "GetPerfMonitorCounterStringAMD not available." << std::endl;
}

static void fallbackGetPerfMonitorCountersAMD(api::GLuint group, api::GLint* numCounters, api::GLint* maxActiveCounters, api::GLsizei counterSize, api::GLuint* counters)
{
	log() << warning << "GetPerfMonitorCountersAMD not available." << std::endl;
}

static void fallbackGetPerfMonitorGroupStringAMD(api::GLuint group, api::GLsizei bufSize, api::GLsizei* length, api::GLchar* groupString)
{
	log() << warning << "GetPerfMonitorGroupStringAMD not available." << std::endl;
}

static void fallbackGetPerfMonitorGroupsAMD(api::GLint* numGroups, api::GLsizei groupsSize, api::GLuint* groups)
{
	log() << warning << "GetPerfMonitorGroupsAMD not available." << std::endl;
}

static void fallbackGetPixelMapfv(api::GLenum map, api::GLfloat* values)
{
	log() << warning << "GetPixelMapfv not available." << std::endl;
}

static void fallbackGetPixelMapuiv(api::GLenum map, api::GLuint* values)
{
	log() << warning << "GetPixelMapuiv not available." << std::endl;
}

static void fallbackGetPixelMapusv(api::GLenum map, api::GLushort* values)
{
	log() << warning << "GetPixelMapusv not available." << std::endl;
}

static void fallbackGetPixelTexGenParameterfvSGIS(api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetPixelTexGenParameterfvSGIS not available." << std::endl;
}

static void fallbackGetPixelTexGenParameterivSGIS(api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetPixelTexGenParameterivSGIS not available." << std::endl;
}

static void fallbackGetPointerIndexedvEXT(api::GLenum target, api::GLuint index, api::GLvoid** data)
{
	log() << warning << "GetPointerIndexedvEXT not available." << std::endl;
}

static void fallbackGetPointerv(api::GLenum pname, api::GLvoid** params)
{
	log() << warning << "GetPointerv not available." << std::endl;
}

static void fallbackGetPointervEXT(api::GLenum pname, api::GLvoid** params)
{
	log() << warning << "GetPointervEXT not available." << std::endl;
}

static void fallbackGetPolygonStipple(api::GLubyte* mask)
{
	log() << warning << "GetPolygonStipple not available." << std::endl;
}

static void fallbackGetProgramEnvParameterIivNV(api::GLenum target, api::GLuint index, api::GLint* params)
{
	log() << warning << "GetProgramEnvParameterIivNV not available." << std::endl;
}

static void fallbackGetProgramEnvParameterIuivNV(api::GLenum target, api::GLuint index, api::GLuint* params)
{
	log() << warning << "GetProgramEnvParameterIuivNV not available." << std::endl;
}

static void fallbackGetProgramEnvParameterdvARB(api::GLenum target, api::GLuint index, api::GLdouble* params)
{
	log() << warning << "GetProgramEnvParameterdvARB not available." << std::endl;
}

static void fallbackGetProgramEnvParameterfvARB(api::GLenum target, api::GLuint index, api::GLfloat* params)
{
	log() << warning << "GetProgramEnvParameterfvARB not available." << std::endl;
}

static void fallbackGetProgramInfoLog(api::GLuint program, api::GLsizei bufSize, api::GLsizei* length, api::GLchar* infoLog)
{
	log() << warning << "GetProgramInfoLog not available." << std::endl;
}

static void fallbackGetProgramLocalParameterIivNV(api::GLenum target, api::GLuint index, api::GLint* params)
{
	log() << warning << "GetProgramLocalParameterIivNV not available." << std::endl;
}

static void fallbackGetProgramLocalParameterIuivNV(api::GLenum target, api::GLuint index, api::GLuint* params)
{
	log() << warning << "GetProgramLocalParameterIuivNV not available." << std::endl;
}

static void fallbackGetProgramLocalParameterdvARB(api::GLenum target, api::GLuint index, api::GLdouble* params)
{
	log() << warning << "GetProgramLocalParameterdvARB not available." << std::endl;
}

static void fallbackGetProgramLocalParameterfvARB(api::GLenum target, api::GLuint index, api::GLfloat* params)
{
	log() << warning << "GetProgramLocalParameterfvARB not available." << std::endl;
}

static void fallbackGetProgramNamedParameterdvNV(api::GLuint id, api::GLsizei len, const api::GLubyte* name, api::GLdouble* params)
{
	log() << warning << "GetProgramNamedParameterdvNV not available." << std::endl;
}

static void fallbackGetProgramNamedParameterfvNV(api::GLuint id, api::GLsizei len, const api::GLubyte* name, api::GLfloat* params)
{
	log() << warning << "GetProgramNamedParameterfvNV not available." << std::endl;
}

static void fallbackGetProgramParameterdvNV(api::GLenum target, api::GLuint index, api::GLenum pname, api::GLdouble* params)
{
	log() << warning << "GetProgramParameterdvNV not available." << std::endl;
}

static void fallbackGetProgramParameterfvNV(api::GLenum target, api::GLuint index, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetProgramParameterfvNV not available." << std::endl;
}

static void fallbackGetProgramStageiv(api::GLuint program, api::GLenum shadertype, api::GLenum pname, api::GLint* values)
{
	log() << warning << "GetProgramStageiv not available." << std::endl;
}

static void fallbackGetProgramStringARB(api::GLenum target, api::GLenum pname, api::GLvoid* string)
{
	log() << warning << "GetProgramStringARB not available." << std::endl;
}

static void fallbackGetProgramStringNV(api::GLuint id, api::GLenum pname, api::GLubyte* program)
{
	log() << warning << "GetProgramStringNV not available." << std::endl;
}

static void fallbackGetProgramiv(api::GLuint program, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetProgramiv not available." << std::endl;
}

static void fallbackGetProgramivARB(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetProgramivARB not available." << std::endl;
}

static void fallbackGetProgramivNV(api::GLuint id, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetProgramivNV not available." << std::endl;
}

static void fallbackGetQueryIndexediv(api::GLenum target, api::GLuint index, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetQueryIndexediv not available." << std::endl;
}

static void fallbackGetQueryObjecti64v(api::GLuint id, api::GLenum pname, api::GLint64* params)
{
	log() << warning << "GetQueryObjecti64v not available." << std::endl;
}

static void fallbackGetQueryObjecti64vEXT(api::GLuint id, api::GLenum pname, api::GLint64EXT* params)
{
	log() << warning << "GetQueryObjecti64vEXT not available." << std::endl;
}

static void fallbackGetQueryObjectiv(api::GLuint id, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetQueryObjectiv not available." << std::endl;
}

static void fallbackGetQueryObjectivARB(api::GLuint id, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetQueryObjectivARB not available." << std::endl;
}

static void fallbackGetQueryObjectui64v(api::GLuint id, api::GLenum pname, api::GLuint64* params)
{
	log() << warning << "GetQueryObjectui64v not available." << std::endl;
}

static void fallbackGetQueryObjectui64vEXT(api::GLuint id, api::GLenum pname, api::GLuint64EXT* params)
{
	log() << warning << "GetQueryObjectui64vEXT not available." << std::endl;
}

static void fallbackGetQueryObjectuiv(api::GLuint id, api::GLenum pname, api::GLuint* params)
{
	log() << warning << "GetQueryObjectuiv not available." << std::endl;
}

static void fallbackGetQueryObjectuivARB(api::GLuint id, api::GLenum pname, api::GLuint* params)
{
	log() << warning << "GetQueryObjectuivARB not available." << std::endl;
}

static void fallbackGetQueryiv(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetQueryiv not available." << std::endl;
}

static void fallbackGetQueryivARB(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetQueryivARB not available." << std::endl;
}

static void fallbackGetRenderbufferParameteriv(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetRenderbufferParameteriv not available." << std::endl;
}

static void fallbackGetRenderbufferParameterivEXT(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetRenderbufferParameterivEXT not available." << std::endl;
}

static void fallbackGetSamplerParameterIfv(api::GLuint sampler, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetSamplerParameterIfv not available." << std::endl;
}

static void fallbackGetSamplerParameterIiv(api::GLuint sampler, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetSamplerParameterIiv not available." << std::endl;
}

static void fallbackGetSamplerParameterfv(api::GLuint sampler, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetSamplerParameterfv not available." << std::endl;
}

static void fallbackGetSamplerParameteriv(api::GLuint sampler, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetSamplerParameteriv not available." << std::endl;
}

static void fallbackGetSeparableFilter(api::GLenum target, api::GLenum format, api::GLenum type, api::GLvoid* row, api::GLvoid* column, api::GLvoid* span)
{
	log() << warning << "GetSeparableFilter not available." << std::endl;
}

static void fallbackGetSeparableFilterEXT(api::GLenum target, api::GLenum format, api::GLenum type, api::GLvoid* row, api::GLvoid* column, api::GLvoid* span)
{
	log() << warning << "GetSeparableFilterEXT not available." << std::endl;
}

static void fallbackGetShaderInfoLog(api::GLuint shader, api::GLsizei bufSize, api::GLsizei* length, api::GLchar* infoLog)
{
	log() << warning << "GetShaderInfoLog not available." << std::endl;
}

static void fallbackGetShaderSource(api::GLuint shader, api::GLsizei bufSize, api::GLsizei* length, api::GLchar* source)
{
	log() << warning << "GetShaderSource not available." << std::endl;
}

static void fallbackGetShaderSourceARB(api::GLhandleARB obj, api::GLsizei maxLength, api::GLsizei* length, api::GLcharARB* source)
{
	log() << warning << "GetShaderSourceARB not available." << std::endl;
}

static void fallbackGetShaderiv(api::GLuint shader, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetShaderiv not available." << std::endl;
}

static void fallbackGetSharpenTexFuncSGIS(api::GLenum target, api::GLfloat* points)
{
	log() << warning << "GetSharpenTexFuncSGIS not available." << std::endl;
}

static const api::GLubyte * fallbackGetString(api::GLenum name)
{
	log() << warning << "GetString not available." << std::endl;
}

static const api::GLubyte * fallbackGetStringi(api::GLenum name, api::GLuint index)
{
	log() << warning << "GetStringi not available." << std::endl;
}

static api::GLuint fallbackGetSubroutineIndex(api::GLuint program, api::GLenum shadertype, const api::GLchar* name)
{
	log() << warning << "GetSubroutineIndex not available." << std::endl;
}

static api::GLint fallbackGetSubroutineUniformLocation(api::GLuint program, api::GLenum shadertype, const api::GLchar* name)
{
	log() << warning << "GetSubroutineUniformLocation not available." << std::endl;
}

static void fallbackGetSynciv(api::GLsync sync, api::GLenum pname, api::GLsizei bufSize, api::GLsizei* length, api::GLint* values)
{
	log() << warning << "GetSynciv not available." << std::endl;
}

static void fallbackGetTexBumpParameterfvATI(api::GLenum pname, api::GLfloat* param)
{
	log() << warning << "GetTexBumpParameterfvATI not available." << std::endl;
}

static void fallbackGetTexBumpParameterivATI(api::GLenum pname, api::GLint* param)
{
	log() << warning << "GetTexBumpParameterivATI not available." << std::endl;
}

static void fallbackGetTexEnvfv(api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetTexEnvfv not available." << std::endl;
}

static void fallbackGetTexEnviv(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetTexEnviv not available." << std::endl;
}

static void fallbackGetTexFilterFuncSGIS(api::GLenum target, api::GLenum filter, api::GLfloat* weights)
{
	log() << warning << "GetTexFilterFuncSGIS not available." << std::endl;
}

static void fallbackGetTexGendv(api::GLenum coord, api::GLenum pname, api::GLdouble* params)
{
	log() << warning << "GetTexGendv not available." << std::endl;
}

static void fallbackGetTexGenfv(api::GLenum coord, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetTexGenfv not available." << std::endl;
}

static void fallbackGetTexGeniv(api::GLenum coord, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetTexGeniv not available." << std::endl;
}

static void fallbackGetTexImage(api::GLenum target, api::GLint level, api::GLenum format, api::GLenum type, api::GLvoid* pixels)
{
	log() << warning << "GetTexImage not available." << std::endl;
}

static void fallbackGetTexLevelParameterfv(api::GLenum target, api::GLint level, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetTexLevelParameterfv not available." << std::endl;
}

static void fallbackGetTexLevelParameteriv(api::GLenum target, api::GLint level, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetTexLevelParameteriv not available." << std::endl;
}

static void fallbackGetTexParameterIiv(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetTexParameterIiv not available." << std::endl;
}

static void fallbackGetTexParameterIivEXT(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetTexParameterIivEXT not available." << std::endl;
}

static void fallbackGetTexParameterIuiv(api::GLenum target, api::GLenum pname, api::GLuint* params)
{
	log() << warning << "GetTexParameterIuiv not available." << std::endl;
}

static void fallbackGetTexParameterIuivEXT(api::GLenum target, api::GLenum pname, api::GLuint* params)
{
	log() << warning << "GetTexParameterIuivEXT not available." << std::endl;
}

static void fallbackGetTexParameterPointervAPPLE(api::GLenum target, api::GLenum pname, api::GLvoid** params)
{
	log() << warning << "GetTexParameterPointervAPPLE not available." << std::endl;
}

static void fallbackGetTexParameterfv(api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetTexParameterfv not available." << std::endl;
}

static void fallbackGetTexParameteriv(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetTexParameteriv not available." << std::endl;
}

static void fallbackGetTextureImageEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLenum format, api::GLenum type, api::GLvoid* pixels)
{
	log() << warning << "GetTextureImageEXT not available." << std::endl;
}

static void fallbackGetTextureLevelParameterfvEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetTextureLevelParameterfvEXT not available." << std::endl;
}

static void fallbackGetTextureLevelParameterivEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetTextureLevelParameterivEXT not available." << std::endl;
}

static void fallbackGetTextureParameterIivEXT(api::GLuint texture, api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetTextureParameterIivEXT not available." << std::endl;
}

static void fallbackGetTextureParameterIuivEXT(api::GLuint texture, api::GLenum target, api::GLenum pname, api::GLuint* params)
{
	log() << warning << "GetTextureParameterIuivEXT not available." << std::endl;
}

static void fallbackGetTextureParameterfvEXT(api::GLuint texture, api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetTextureParameterfvEXT not available." << std::endl;
}

static void fallbackGetTextureParameterivEXT(api::GLuint texture, api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetTextureParameterivEXT not available." << std::endl;
}

static void fallbackGetTrackMatrixivNV(api::GLenum target, api::GLuint address, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetTrackMatrixivNV not available." << std::endl;
}

static void fallbackGetTransformFeedbackVarying(api::GLuint program, api::GLuint index, api::GLsizei bufSize, api::GLsizei* length, api::GLsizei* size, api::GLenum* type, api::GLchar* name)
{
	log() << warning << "GetTransformFeedbackVarying not available." << std::endl;
}

static void fallbackGetTransformFeedbackVaryingEXT(api::GLuint program, api::GLuint index, api::GLsizei bufSize, api::GLsizei* length, api::GLsizei* size, api::GLenum* type, api::GLchar* name)
{
	log() << warning << "GetTransformFeedbackVaryingEXT not available." << std::endl;
}

static void fallbackGetTransformFeedbackVaryingNV(api::GLuint program, api::GLuint index, api::GLint* location)
{
	log() << warning << "GetTransformFeedbackVaryingNV not available." << std::endl;
}

static api::GLuint fallbackGetUniformBlockIndex(api::GLuint program, const api::GLchar* uniformBlockName)
{
	log() << warning << "GetUniformBlockIndex not available." << std::endl;
}

static api::GLint fallbackGetUniformBufferSizeEXT(api::GLuint program, api::GLint location)
{
	log() << warning << "GetUniformBufferSizeEXT not available." << std::endl;
}

static void fallbackGetUniformIndices(api::GLuint program, api::GLsizei uniformCount, const api::GLchar** uniformNames, api::GLuint* uniformIndices)
{
	log() << warning << "GetUniformIndices not available." << std::endl;
}

static api::GLint fallbackGetUniformLocation(api::GLuint program, const api::GLchar* name)
{
	log() << warning << "GetUniformLocation not available." << std::endl;
}

static api::GLint fallbackGetUniformLocationARB(api::GLhandleARB programObj, const api::GLcharARB* name)
{
	log() << warning << "GetUniformLocationARB not available." << std::endl;
}

static api::GLintptr fallbackGetUniformOffsetEXT(api::GLuint program, api::GLint location)
{
	log() << warning << "GetUniformOffsetEXT not available." << std::endl;
}

static void fallbackGetUniformSubroutineuiv(api::GLenum shadertype, api::GLint location, api::GLuint* params)
{
	log() << warning << "GetUniformSubroutineuiv not available." << std::endl;
}

static void fallbackGetUniformdv(api::GLuint program, api::GLint location, api::GLdouble* params)
{
	log() << warning << "GetUniformdv not available." << std::endl;
}

static void fallbackGetUniformfv(api::GLuint program, api::GLint location, api::GLfloat* params)
{
	log() << warning << "GetUniformfv not available." << std::endl;
}

static void fallbackGetUniformfvARB(api::GLhandleARB programObj, api::GLint location, api::GLfloat* params)
{
	log() << warning << "GetUniformfvARB not available." << std::endl;
}

static void fallbackGetUniformiv(api::GLuint program, api::GLint location, api::GLint* params)
{
	log() << warning << "GetUniformiv not available." << std::endl;
}

static void fallbackGetUniformivARB(api::GLhandleARB programObj, api::GLint location, api::GLint* params)
{
	log() << warning << "GetUniformivARB not available." << std::endl;
}

static void fallbackGetUniformui64vNV(api::GLuint program, api::GLint location, api::GLuint64EXT* params)
{
	log() << warning << "GetUniformui64vNV not available." << std::endl;
}

static void fallbackGetUniformuiv(api::GLuint program, api::GLint location, api::GLuint* params)
{
	log() << warning << "GetUniformuiv not available." << std::endl;
}

static void fallbackGetUniformuivEXT(api::GLuint program, api::GLint location, api::GLuint* params)
{
	log() << warning << "GetUniformuivEXT not available." << std::endl;
}

static void fallbackGetVariantArrayObjectfvATI(api::GLuint id, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetVariantArrayObjectfvATI not available." << std::endl;
}

static void fallbackGetVariantArrayObjectivATI(api::GLuint id, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetVariantArrayObjectivATI not available." << std::endl;
}

static void fallbackGetVariantBooleanvEXT(api::GLuint id, api::GLenum value, api::GLboolean* data)
{
	log() << warning << "GetVariantBooleanvEXT not available." << std::endl;
}

static void fallbackGetVariantFloatvEXT(api::GLuint id, api::GLenum value, api::GLfloat* data)
{
	log() << warning << "GetVariantFloatvEXT not available." << std::endl;
}

static void fallbackGetVariantIntegervEXT(api::GLuint id, api::GLenum value, api::GLint* data)
{
	log() << warning << "GetVariantIntegervEXT not available." << std::endl;
}

static void fallbackGetVariantPointervEXT(api::GLuint id, api::GLenum value, api::GLvoid** data)
{
	log() << warning << "GetVariantPointervEXT not available." << std::endl;
}

static api::GLint fallbackGetVaryingLocationNV(api::GLuint program, const api::GLchar* name)
{
	log() << warning << "GetVaryingLocationNV not available." << std::endl;
}

static void fallbackGetVertexAttribArrayObjectfvATI(api::GLuint index, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetVertexAttribArrayObjectfvATI not available." << std::endl;
}

static void fallbackGetVertexAttribArrayObjectivATI(api::GLuint index, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetVertexAttribArrayObjectivATI not available." << std::endl;
}

static void fallbackGetVertexAttribIiv(api::GLuint index, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetVertexAttribIiv not available." << std::endl;
}

static void fallbackGetVertexAttribIivEXT(api::GLuint index, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetVertexAttribIivEXT not available." << std::endl;
}

static void fallbackGetVertexAttribIuiv(api::GLuint index, api::GLenum pname, api::GLuint* params)
{
	log() << warning << "GetVertexAttribIuiv not available." << std::endl;
}

static void fallbackGetVertexAttribIuivEXT(api::GLuint index, api::GLenum pname, api::GLuint* params)
{
	log() << warning << "GetVertexAttribIuivEXT not available." << std::endl;
}

static void fallbackGetVertexAttribPointerv(api::GLuint index, api::GLenum pname, api::GLvoid** pointer)
{
	log() << warning << "GetVertexAttribPointerv not available." << std::endl;
}

static void fallbackGetVertexAttribPointervARB(api::GLuint index, api::GLenum pname, api::GLvoid** pointer)
{
	log() << warning << "GetVertexAttribPointervARB not available." << std::endl;
}

static void fallbackGetVertexAttribPointervNV(api::GLuint index, api::GLenum pname, api::GLvoid** pointer)
{
	log() << warning << "GetVertexAttribPointervNV not available." << std::endl;
}

static void fallbackGetVertexAttribdv(api::GLuint index, api::GLenum pname, api::GLdouble* params)
{
	log() << warning << "GetVertexAttribdv not available." << std::endl;
}

static void fallbackGetVertexAttribdvARB(api::GLuint index, api::GLenum pname, api::GLdouble* params)
{
	log() << warning << "GetVertexAttribdvARB not available." << std::endl;
}

static void fallbackGetVertexAttribdvNV(api::GLuint index, api::GLenum pname, api::GLdouble* params)
{
	log() << warning << "GetVertexAttribdvNV not available." << std::endl;
}

static void fallbackGetVertexAttribfv(api::GLuint index, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetVertexAttribfv not available." << std::endl;
}

static void fallbackGetVertexAttribfvARB(api::GLuint index, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetVertexAttribfvARB not available." << std::endl;
}

static void fallbackGetVertexAttribfvNV(api::GLuint index, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetVertexAttribfvNV not available." << std::endl;
}

static void fallbackGetVertexAttribiv(api::GLuint index, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetVertexAttribiv not available." << std::endl;
}

static void fallbackGetVertexAttribivARB(api::GLuint index, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetVertexAttribivARB not available." << std::endl;
}

static void fallbackGetVertexAttribivNV(api::GLuint index, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetVertexAttribivNV not available." << std::endl;
}

static void fallbackGetVideoCaptureStreamdvNV(api::GLuint video_capture_slot, api::GLuint stream, api::GLenum pname, api::GLdouble* params)
{
	log() << warning << "GetVideoCaptureStreamdvNV not available." << std::endl;
}

static void fallbackGetVideoCaptureStreamfvNV(api::GLuint video_capture_slot, api::GLuint stream, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "GetVideoCaptureStreamfvNV not available." << std::endl;
}

static void fallbackGetVideoCaptureStreamivNV(api::GLuint video_capture_slot, api::GLuint stream, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetVideoCaptureStreamivNV not available." << std::endl;
}

static void fallbackGetVideoCaptureivNV(api::GLuint video_capture_slot, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetVideoCaptureivNV not available." << std::endl;
}

static void fallbackGetVideoi64vNV(api::GLuint video_slot, api::GLenum pname, api::GLint64EXT* params)
{
	log() << warning << "GetVideoi64vNV not available." << std::endl;
}

static void fallbackGetVideoivNV(api::GLuint video_slot, api::GLenum pname, api::GLint* params)
{
	log() << warning << "GetVideoivNV not available." << std::endl;
}

static void fallbackGetVideoui64vNV(api::GLuint video_slot, api::GLenum pname, api::GLuint64EXT* params)
{
	log() << warning << "GetVideoui64vNV not available." << std::endl;
}

static void fallbackGetVideouivNV(api::GLuint video_slot, api::GLenum pname, api::GLuint* params)
{
	log() << warning << "GetVideouivNV not available." << std::endl;
}

static void fallbackGlobalAlphaFactorbSUN(api::GLbyte factor)
{
	log() << warning << "GlobalAlphaFactorbSUN not available." << std::endl;
}

static void fallbackGlobalAlphaFactordSUN(api::GLdouble factor)
{
	log() << warning << "GlobalAlphaFactordSUN not available." << std::endl;
}

static void fallbackGlobalAlphaFactorfSUN(api::GLfloat factor)
{
	log() << warning << "GlobalAlphaFactorfSUN not available." << std::endl;
}

static void fallbackGlobalAlphaFactoriSUN(api::GLint factor)
{
	log() << warning << "GlobalAlphaFactoriSUN not available." << std::endl;
}

static void fallbackGlobalAlphaFactorsSUN(api::GLshort factor)
{
	log() << warning << "GlobalAlphaFactorsSUN not available." << std::endl;
}

static void fallbackGlobalAlphaFactorubSUN(api::GLubyte factor)
{
	log() << warning << "GlobalAlphaFactorubSUN not available." << std::endl;
}

static void fallbackGlobalAlphaFactoruiSUN(api::GLuint factor)
{
	log() << warning << "GlobalAlphaFactoruiSUN not available." << std::endl;
}

static void fallbackGlobalAlphaFactorusSUN(api::GLushort factor)
{
	log() << warning << "GlobalAlphaFactorusSUN not available." << std::endl;
}

static void fallbackHint(api::GLenum target, api::GLenum mode)
{
	log() << warning << "Hint not available." << std::endl;
}

static void fallbackHintPGI(api::GLenum target, api::GLint mode)
{
	log() << warning << "HintPGI not available." << std::endl;
}

static void fallbackHistogram(api::GLenum target, api::GLsizei width, api::GLenum internalformat, api::GLboolean sink)
{
	log() << warning << "Histogram not available." << std::endl;
}

static void fallbackHistogramEXT(api::GLenum target, api::GLsizei width, api::GLenum internalformat, api::GLboolean sink)
{
	log() << warning << "HistogramEXT not available." << std::endl;
}

static void fallbackIglooInterfaceSGIX(api::GLenum pname, const api::GLvoid* params)
{
	log() << warning << "IglooInterfaceSGIX not available." << std::endl;
}

static void fallbackImageTransformParameterfHP(api::GLenum target, api::GLenum pname, api::GLfloat param)
{
	log() << warning << "ImageTransformParameterfHP not available." << std::endl;
}

static void fallbackImageTransformParameterfvHP(api::GLenum target, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "ImageTransformParameterfvHP not available." << std::endl;
}

static void fallbackImageTransformParameteriHP(api::GLenum target, api::GLenum pname, api::GLint param)
{
	log() << warning << "ImageTransformParameteriHP not available." << std::endl;
}

static void fallbackImageTransformParameterivHP(api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "ImageTransformParameterivHP not available." << std::endl;
}

static void fallbackIndexFormatNV(api::GLenum type, api::GLsizei stride)
{
	log() << warning << "IndexFormatNV not available." << std::endl;
}

static void fallbackIndexFuncEXT(api::GLenum func, api::GLclampf ref)
{
	log() << warning << "IndexFuncEXT not available." << std::endl;
}

static void fallbackIndexMask(api::GLuint mask)
{
	log() << warning << "IndexMask not available." << std::endl;
}

static void fallbackIndexMaterialEXT(api::GLenum face, api::GLenum mode)
{
	log() << warning << "IndexMaterialEXT not available." << std::endl;
}

static void fallbackIndexPointer(api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "IndexPointer not available." << std::endl;
}

static void fallbackIndexPointerEXT(api::GLenum type, api::GLsizei stride, api::GLsizei count, const api::GLvoid* pointer)
{
	log() << warning << "IndexPointerEXT not available." << std::endl;
}

static void fallbackIndexPointerListIBM(api::GLenum type, api::GLint stride, const api::GLvoid** pointer, api::GLint ptrstride)
{
	log() << warning << "IndexPointerListIBM not available." << std::endl;
}

static void fallbackIndexd(api::GLdouble c)
{
	log() << warning << "Indexd not available." << std::endl;
}

static void fallbackIndexdv(const api::GLdouble* c)
{
	log() << warning << "Indexdv not available." << std::endl;
}

static void fallbackIndexf(api::GLfloat c)
{
	log() << warning << "Indexf not available." << std::endl;
}

static void fallbackIndexfv(const api::GLfloat* c)
{
	log() << warning << "Indexfv not available." << std::endl;
}

static void fallbackIndexi(api::GLint c)
{
	log() << warning << "Indexi not available." << std::endl;
}

static void fallbackIndexiv(const api::GLint* c)
{
	log() << warning << "Indexiv not available." << std::endl;
}

static void fallbackIndexs(api::GLshort c)
{
	log() << warning << "Indexs not available." << std::endl;
}

static void fallbackIndexsv(const api::GLshort* c)
{
	log() << warning << "Indexsv not available." << std::endl;
}

static void fallbackIndexub(api::GLubyte c)
{
	log() << warning << "Indexub not available." << std::endl;
}

static void fallbackIndexubv(const api::GLubyte* c)
{
	log() << warning << "Indexubv not available." << std::endl;
}

static void fallbackInitNames()
{
	log() << warning << "InitNames not available." << std::endl;
}

static void fallbackInsertComponentEXT(api::GLuint res, api::GLuint src, api::GLuint num)
{
	log() << warning << "InsertComponentEXT not available." << std::endl;
}

static void fallbackInstrumentsBufferSGIX(api::GLsizei size, api::GLint* buffer)
{
	log() << warning << "InstrumentsBufferSGIX not available." << std::endl;
}

static void fallbackInterleavedArrays(api::GLenum format, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "InterleavedArrays not available." << std::endl;
}

static api::GLboolean fallbackIsAsyncMarkerSGIX(api::GLuint marker)
{
	log() << warning << "IsAsyncMarkerSGIX not available." << std::endl;
}

static api::GLboolean fallbackIsBuffer(api::GLuint buffer)
{
	log() << warning << "IsBuffer not available." << std::endl;
}

static api::GLboolean fallbackIsBufferARB(api::GLuint buffer)
{
	log() << warning << "IsBufferARB not available." << std::endl;
}

static api::GLboolean fallbackIsBufferResidentNV(api::GLenum target)
{
	log() << warning << "IsBufferResidentNV not available." << std::endl;
}

static api::GLboolean fallbackIsEnabled(api::GLenum cap)
{
	log() << warning << "IsEnabled not available." << std::endl;
}

static api::GLboolean fallbackIsEnabledIndexedEXT(api::GLenum target, api::GLuint index)
{
	log() << warning << "IsEnabledIndexedEXT not available." << std::endl;
}

static api::GLboolean fallbackIsEnabledi(api::GLenum target, api::GLuint index)
{
	log() << warning << "IsEnabledi not available." << std::endl;
}

static api::GLboolean fallbackIsFenceAPPLE(api::GLuint fence)
{
	log() << warning << "IsFenceAPPLE not available." << std::endl;
}

static api::GLboolean fallbackIsFenceNV(api::GLuint fence)
{
	log() << warning << "IsFenceNV not available." << std::endl;
}

static api::GLboolean fallbackIsFramebuffer(api::GLuint framebuffer)
{
	log() << warning << "IsFramebuffer not available." << std::endl;
}

static api::GLboolean fallbackIsFramebufferEXT(api::GLuint framebuffer)
{
	log() << warning << "IsFramebufferEXT not available." << std::endl;
}

static api::GLboolean fallbackIsList(api::GLuint list)
{
	log() << warning << "IsList not available." << std::endl;
}

static api::GLboolean fallbackIsNamedBufferResidentNV(api::GLuint buffer)
{
	log() << warning << "IsNamedBufferResidentNV not available." << std::endl;
}

static api::GLboolean fallbackIsNamedStringARB(api::GLint namelen, const api::GLchar* name)
{
	log() << warning << "IsNamedStringARB not available." << std::endl;
}

static api::GLboolean fallbackIsObjectBufferATI(api::GLuint buffer)
{
	log() << warning << "IsObjectBufferATI not available." << std::endl;
}

static api::GLboolean fallbackIsOcclusionQueryNV(api::GLuint id)
{
	log() << warning << "IsOcclusionQueryNV not available." << std::endl;
}

static api::GLboolean fallbackIsProgram(api::GLuint program)
{
	log() << warning << "IsProgram not available." << std::endl;
}

static api::GLboolean fallbackIsProgramARB(api::GLuint program)
{
	log() << warning << "IsProgramARB not available." << std::endl;
}

static api::GLboolean fallbackIsProgramNV(api::GLuint id)
{
	log() << warning << "IsProgramNV not available." << std::endl;
}

static api::GLboolean fallbackIsQuery(api::GLuint id)
{
	log() << warning << "IsQuery not available." << std::endl;
}

static api::GLboolean fallbackIsQueryARB(api::GLuint id)
{
	log() << warning << "IsQueryARB not available." << std::endl;
}

static api::GLboolean fallbackIsRenderbuffer(api::GLuint renderbuffer)
{
	log() << warning << "IsRenderbuffer not available." << std::endl;
}

static api::GLboolean fallbackIsRenderbufferEXT(api::GLuint renderbuffer)
{
	log() << warning << "IsRenderbufferEXT not available." << std::endl;
}

static api::GLboolean fallbackIsSampler(api::GLuint sampler)
{
	log() << warning << "IsSampler not available." << std::endl;
}

static api::GLboolean fallbackIsShader(api::GLuint shader)
{
	log() << warning << "IsShader not available." << std::endl;
}

static api::GLboolean fallbackIsSync(api::GLsync sync)
{
	log() << warning << "IsSync not available." << std::endl;
}

static api::GLboolean fallbackIsTexture(api::GLuint texture)
{
	log() << warning << "IsTexture not available." << std::endl;
}

static api::GLboolean fallbackIsTextureEXT(api::GLuint texture)
{
	log() << warning << "IsTextureEXT not available." << std::endl;
}

static api::GLboolean fallbackIsTransformFeedback(api::GLuint id)
{
	log() << warning << "IsTransformFeedback not available." << std::endl;
}

static api::GLboolean fallbackIsTransformFeedbackNV(api::GLuint id)
{
	log() << warning << "IsTransformFeedbackNV not available." << std::endl;
}

static api::GLboolean fallbackIsVariantEnabledEXT(api::GLuint id, api::GLenum cap)
{
	log() << warning << "IsVariantEnabledEXT not available." << std::endl;
}

static api::GLboolean fallbackIsVertexArray(api::GLuint array)
{
	log() << warning << "IsVertexArray not available." << std::endl;
}

static api::GLboolean fallbackIsVertexArrayAPPLE(api::GLuint array)
{
	log() << warning << "IsVertexArrayAPPLE not available." << std::endl;
}

static api::GLboolean fallbackIsVertexAttribEnabledAPPLE(api::GLuint index, api::GLenum pname)
{
	log() << warning << "IsVertexAttribEnabledAPPLE not available." << std::endl;
}

static void fallbackLightEnviSGIX(api::GLenum pname, api::GLint param)
{
	log() << warning << "LightEnviSGIX not available." << std::endl;
}

static void fallbackLightModelf(api::GLenum pname, api::GLfloat param)
{
	log() << warning << "LightModelf not available." << std::endl;
}

static void fallbackLightModelfv(api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "LightModelfv not available." << std::endl;
}

static void fallbackLightModeli(api::GLenum pname, api::GLint param)
{
	log() << warning << "LightModeli not available." << std::endl;
}

static void fallbackLightModeliv(api::GLenum pname, const api::GLint* params)
{
	log() << warning << "LightModeliv not available." << std::endl;
}

static void fallbackLightf(api::GLenum light, api::GLenum pname, api::GLfloat param)
{
	log() << warning << "Lightf not available." << std::endl;
}

static void fallbackLightfv(api::GLenum light, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "Lightfv not available." << std::endl;
}

static void fallbackLighti(api::GLenum light, api::GLenum pname, api::GLint param)
{
	log() << warning << "Lighti not available." << std::endl;
}

static void fallbackLightiv(api::GLenum light, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "Lightiv not available." << std::endl;
}

static void fallbackLineStipple(api::GLint factor, api::GLushort pattern)
{
	log() << warning << "LineStipple not available." << std::endl;
}

static void fallbackLineWidth(api::GLfloat width)
{
	log() << warning << "LineWidth not available." << std::endl;
}

static void fallbackLinkProgram(api::GLuint program)
{
	log() << warning << "LinkProgram not available." << std::endl;
}

static void fallbackLinkProgramARB(api::GLhandleARB programObj)
{
	log() << warning << "LinkProgramARB not available." << std::endl;
}

static void fallbackListBase(api::GLuint base)
{
	log() << warning << "ListBase not available." << std::endl;
}

static void fallbackListParameterfSGIX(api::GLuint list, api::GLenum pname, api::GLfloat param)
{
	log() << warning << "ListParameterfSGIX not available." << std::endl;
}

static void fallbackListParameterfvSGIX(api::GLuint list, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "ListParameterfvSGIX not available." << std::endl;
}

static void fallbackListParameteriSGIX(api::GLuint list, api::GLenum pname, api::GLint param)
{
	log() << warning << "ListParameteriSGIX not available." << std::endl;
}

static void fallbackListParameterivSGIX(api::GLuint list, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "ListParameterivSGIX not available." << std::endl;
}

static void fallbackLoadIdentity()
{
	log() << warning << "LoadIdentity not available." << std::endl;
}

static void fallbackLoadIdentityDeformationMapSGIX(api::GLbitfield mask)
{
	log() << warning << "LoadIdentityDeformationMapSGIX not available." << std::endl;
}

static void fallbackLoadMatrixd(const api::GLdouble* m)
{
	log() << warning << "LoadMatrixd not available." << std::endl;
}

static void fallbackLoadMatrixf(const api::GLfloat* m)
{
	log() << warning << "LoadMatrixf not available." << std::endl;
}

static void fallbackLoadName(api::GLuint name)
{
	log() << warning << "LoadName not available." << std::endl;
}

static void fallbackLoadProgramNV(api::GLenum target, api::GLuint id, api::GLsizei len, const api::GLubyte* program)
{
	log() << warning << "LoadProgramNV not available." << std::endl;
}

static void fallbackLoadTransposeMatrixd(const api::GLdouble* m)
{
	log() << warning << "LoadTransposeMatrixd not available." << std::endl;
}

static void fallbackLoadTransposeMatrixdARB(const api::GLdouble* m)
{
	log() << warning << "LoadTransposeMatrixdARB not available." << std::endl;
}

static void fallbackLoadTransposeMatrixf(const api::GLfloat* m)
{
	log() << warning << "LoadTransposeMatrixf not available." << std::endl;
}

static void fallbackLoadTransposeMatrixfARB(const api::GLfloat* m)
{
	log() << warning << "LoadTransposeMatrixfARB not available." << std::endl;
}

static void fallbackLockArraysEXT(api::GLint first, api::GLsizei count)
{
	log() << warning << "LockArraysEXT not available." << std::endl;
}

static void fallbackLogicOp(api::GLenum opcode)
{
	log() << warning << "LogicOp not available." << std::endl;
}

static void fallbackMakeBufferNonResidentNV(api::GLenum target)
{
	log() << warning << "MakeBufferNonResidentNV not available." << std::endl;
}

static void fallbackMakeBufferResidentNV(api::GLenum target, api::GLenum access)
{
	log() << warning << "MakeBufferResidentNV not available." << std::endl;
}

static void fallbackMakeNamedBufferNonResidentNV(api::GLuint buffer)
{
	log() << warning << "MakeNamedBufferNonResidentNV not available." << std::endl;
}

static void fallbackMakeNamedBufferResidentNV(api::GLuint buffer, api::GLenum access)
{
	log() << warning << "MakeNamedBufferResidentNV not available." << std::endl;
}

static void fallbackMap1d(api::GLenum target, api::GLdouble u1, api::GLdouble u2, api::GLint stride, api::GLint order, const api::GLdouble* points)
{
	log() << warning << "Map1d not available." << std::endl;
}

static void fallbackMap1f(api::GLenum target, api::GLfloat u1, api::GLfloat u2, api::GLint stride, api::GLint order, const api::GLfloat* points)
{
	log() << warning << "Map1f not available." << std::endl;
}

static void fallbackMap2d(api::GLenum target, api::GLdouble u1, api::GLdouble u2, api::GLint ustride, api::GLint uorder, api::GLdouble v1, api::GLdouble v2, api::GLint vstride, api::GLint vorder, const api::GLdouble* points)
{
	log() << warning << "Map2d not available." << std::endl;
}

static void fallbackMap2f(api::GLenum target, api::GLfloat u1, api::GLfloat u2, api::GLint ustride, api::GLint uorder, api::GLfloat v1, api::GLfloat v2, api::GLint vstride, api::GLint vorder, const api::GLfloat* points)
{
	log() << warning << "Map2f not available." << std::endl;
}

static api::GLvoid* fallbackMapBuffer(api::GLenum target, api::GLenum access)
{
	log() << warning << "MapBuffer not available." << std::endl;
}

static api::GLvoid* fallbackMapBufferARB(api::GLenum target, api::GLenum access)
{
	log() << warning << "MapBufferARB not available." << std::endl;
}

static api::GLvoid* fallbackMapBufferRange(api::GLenum target, api::GLintptr offset, api::GLsizeiptr length, api::GLbitfield access)
{
	log() << warning << "MapBufferRange not available." << std::endl;
}

static void fallbackMapControlPointsNV(api::GLenum target, api::GLuint index, api::GLenum type, api::GLsizei ustride, api::GLsizei vstride, api::GLint uorder, api::GLint vorder, api::GLboolean packed, const api::GLvoid* points)
{
	log() << warning << "MapControlPointsNV not available." << std::endl;
}

static void fallbackMapGrid1d(api::GLint un, api::GLdouble u1, api::GLdouble u2)
{
	log() << warning << "MapGrid1d not available." << std::endl;
}

static void fallbackMapGrid1f(api::GLint un, api::GLfloat u1, api::GLfloat u2)
{
	log() << warning << "MapGrid1f not available." << std::endl;
}

static void fallbackMapGrid2d(api::GLint un, api::GLdouble u1, api::GLdouble u2, api::GLint vn, api::GLdouble v1, api::GLdouble v2)
{
	log() << warning << "MapGrid2d not available." << std::endl;
}

static void fallbackMapGrid2f(api::GLint un, api::GLfloat u1, api::GLfloat u2, api::GLint vn, api::GLfloat v1, api::GLfloat v2)
{
	log() << warning << "MapGrid2f not available." << std::endl;
}

static api::GLvoid* fallbackMapNamedBufferEXT(api::GLuint buffer, api::GLenum access)
{
	log() << warning << "MapNamedBufferEXT not available." << std::endl;
}

static api::GLvoid* fallbackMapObjectBufferATI(api::GLuint buffer)
{
	log() << warning << "MapObjectBufferATI not available." << std::endl;
}

static void fallbackMapParameterfvNV(api::GLenum target, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "MapParameterfvNV not available." << std::endl;
}

static void fallbackMapParameterivNV(api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "MapParameterivNV not available." << std::endl;
}

static void fallbackMapVertexAttrib1dAPPLE(api::GLuint index, api::GLuint size, api::GLdouble u1, api::GLdouble u2, api::GLint stride, api::GLint order, const api::GLdouble* points)
{
	log() << warning << "MapVertexAttrib1dAPPLE not available." << std::endl;
}

static void fallbackMapVertexAttrib1fAPPLE(api::GLuint index, api::GLuint size, api::GLfloat u1, api::GLfloat u2, api::GLint stride, api::GLint order, const api::GLfloat* points)
{
	log() << warning << "MapVertexAttrib1fAPPLE not available." << std::endl;
}

static void fallbackMapVertexAttrib2dAPPLE(api::GLuint index, api::GLuint size, api::GLdouble u1, api::GLdouble u2, api::GLint ustride, api::GLint uorder, api::GLdouble v1, api::GLdouble v2, api::GLint vstride, api::GLint vorder, const api::GLdouble* points)
{
	log() << warning << "MapVertexAttrib2dAPPLE not available." << std::endl;
}

static void fallbackMapVertexAttrib2fAPPLE(api::GLuint index, api::GLuint size, api::GLfloat u1, api::GLfloat u2, api::GLint ustride, api::GLint uorder, api::GLfloat v1, api::GLfloat v2, api::GLint vstride, api::GLint vorder, const api::GLfloat* points)
{
	log() << warning << "MapVertexAttrib2fAPPLE not available." << std::endl;
}

static void fallbackMaterialf(api::GLenum face, api::GLenum pname, api::GLfloat param)
{
	log() << warning << "Materialf not available." << std::endl;
}

static void fallbackMaterialfv(api::GLenum face, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "Materialfv not available." << std::endl;
}

static void fallbackMateriali(api::GLenum face, api::GLenum pname, api::GLint param)
{
	log() << warning << "Materiali not available." << std::endl;
}

static void fallbackMaterialiv(api::GLenum face, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "Materialiv not available." << std::endl;
}

static void fallbackMatrixFrustumEXT(api::GLenum mode, api::GLdouble left, api::GLdouble right, api::GLdouble bottom, api::GLdouble top, api::GLdouble zNear, api::GLdouble zFar)
{
	log() << warning << "MatrixFrustumEXT not available." << std::endl;
}

static void fallbackMatrixIndexPointerARB(api::GLint size, api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "MatrixIndexPointerARB not available." << std::endl;
}

static void fallbackMatrixIndexubvARB(api::GLint size, const api::GLubyte* indices)
{
	log() << warning << "MatrixIndexubvARB not available." << std::endl;
}

static void fallbackMatrixIndexuivARB(api::GLint size, const api::GLuint* indices)
{
	log() << warning << "MatrixIndexuivARB not available." << std::endl;
}

static void fallbackMatrixIndexusvARB(api::GLint size, const api::GLushort* indices)
{
	log() << warning << "MatrixIndexusvARB not available." << std::endl;
}

static void fallbackMatrixLoadIdentityEXT(api::GLenum mode)
{
	log() << warning << "MatrixLoadIdentityEXT not available." << std::endl;
}

static void fallbackMatrixLoadTransposedEXT(api::GLenum mode, const api::GLdouble* m)
{
	log() << warning << "MatrixLoadTransposedEXT not available." << std::endl;
}

static void fallbackMatrixLoadTransposefEXT(api::GLenum mode, const api::GLfloat* m)
{
	log() << warning << "MatrixLoadTransposefEXT not available." << std::endl;
}

static void fallbackMatrixLoaddEXT(api::GLenum mode, const api::GLdouble* m)
{
	log() << warning << "MatrixLoaddEXT not available." << std::endl;
}

static void fallbackMatrixLoadfEXT(api::GLenum mode, const api::GLfloat* m)
{
	log() << warning << "MatrixLoadfEXT not available." << std::endl;
}

static void fallbackMatrixMode(api::GLenum mode)
{
	log() << warning << "MatrixMode not available." << std::endl;
}

static void fallbackMatrixMultTransposedEXT(api::GLenum mode, const api::GLdouble* m)
{
	log() << warning << "MatrixMultTransposedEXT not available." << std::endl;
}

static void fallbackMatrixMultTransposefEXT(api::GLenum mode, const api::GLfloat* m)
{
	log() << warning << "MatrixMultTransposefEXT not available." << std::endl;
}

static void fallbackMatrixMultdEXT(api::GLenum mode, const api::GLdouble* m)
{
	log() << warning << "MatrixMultdEXT not available." << std::endl;
}

static void fallbackMatrixMultfEXT(api::GLenum mode, const api::GLfloat* m)
{
	log() << warning << "MatrixMultfEXT not available." << std::endl;
}

static void fallbackMatrixOrthoEXT(api::GLenum mode, api::GLdouble left, api::GLdouble right, api::GLdouble bottom, api::GLdouble top, api::GLdouble zNear, api::GLdouble zFar)
{
	log() << warning << "MatrixOrthoEXT not available." << std::endl;
}

static void fallbackMatrixPopEXT(api::GLenum mode)
{
	log() << warning << "MatrixPopEXT not available." << std::endl;
}

static void fallbackMatrixPushEXT(api::GLenum mode)
{
	log() << warning << "MatrixPushEXT not available." << std::endl;
}

static void fallbackMatrixRotatedEXT(api::GLenum mode, api::GLdouble angle, api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "MatrixRotatedEXT not available." << std::endl;
}

static void fallbackMatrixRotatefEXT(api::GLenum mode, api::GLfloat angle, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "MatrixRotatefEXT not available." << std::endl;
}

static void fallbackMatrixScaledEXT(api::GLenum mode, api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "MatrixScaledEXT not available." << std::endl;
}

static void fallbackMatrixScalefEXT(api::GLenum mode, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "MatrixScalefEXT not available." << std::endl;
}

static void fallbackMatrixTranslatedEXT(api::GLenum mode, api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "MatrixTranslatedEXT not available." << std::endl;
}

static void fallbackMatrixTranslatefEXT(api::GLenum mode, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "MatrixTranslatefEXT not available." << std::endl;
}

static void fallbackMinSampleShading(api::GLclampf value)
{
	log() << warning << "MinSampleShading not available." << std::endl;
}

static void fallbackMinmax(api::GLenum target, api::GLenum internalformat, api::GLboolean sink)
{
	log() << warning << "Minmax not available." << std::endl;
}

static void fallbackMinmaxEXT(api::GLenum target, api::GLenum internalformat, api::GLboolean sink)
{
	log() << warning << "MinmaxEXT not available." << std::endl;
}

static void fallbackMultMatrixd(const api::GLdouble* m)
{
	log() << warning << "MultMatrixd not available." << std::endl;
}

static void fallbackMultMatrixf(const api::GLfloat* m)
{
	log() << warning << "MultMatrixf not available." << std::endl;
}

static void fallbackMultTransposeMatrixd(const api::GLdouble* m)
{
	log() << warning << "MultTransposeMatrixd not available." << std::endl;
}

static void fallbackMultTransposeMatrixdARB(const api::GLdouble* m)
{
	log() << warning << "MultTransposeMatrixdARB not available." << std::endl;
}

static void fallbackMultTransposeMatrixf(const api::GLfloat* m)
{
	log() << warning << "MultTransposeMatrixf not available." << std::endl;
}

static void fallbackMultTransposeMatrixfARB(const api::GLfloat* m)
{
	log() << warning << "MultTransposeMatrixfARB not available." << std::endl;
}

static void fallbackMultiDrawArrays(api::GLenum mode, api::GLint* first, api::GLsizei* count, api::GLsizei primcount)
{
	log() << warning << "MultiDrawArrays not available." << std::endl;
}

static void fallbackMultiDrawArraysEXT(api::GLenum mode, api::GLint* first, api::GLsizei* count, api::GLsizei primcount)
{
	log() << warning << "MultiDrawArraysEXT not available." << std::endl;
}

static void fallbackMultiDrawElementArrayAPPLE(api::GLenum mode, const api::GLint* first, const api::GLsizei* count, api::GLsizei primcount)
{
	log() << warning << "MultiDrawElementArrayAPPLE not available." << std::endl;
}

static void fallbackMultiDrawElements(api::GLenum mode, const api::GLsizei* count, api::GLenum type, const api::GLvoid** indices, api::GLsizei primcount)
{
	log() << warning << "MultiDrawElements not available." << std::endl;
}

static void fallbackMultiDrawElementsBaseVertex(api::GLenum mode, const api::GLsizei* count, api::GLenum type, const api::GLvoid** indices, api::GLsizei primcount, const api::GLint* basevertex)
{
	log() << warning << "MultiDrawElementsBaseVertex not available." << std::endl;
}

static void fallbackMultiDrawElementsEXT(api::GLenum mode, const api::GLsizei* count, api::GLenum type, const api::GLvoid** indices, api::GLsizei primcount)
{
	log() << warning << "MultiDrawElementsEXT not available." << std::endl;
}

static void fallbackMultiDrawRangeElementArrayAPPLE(api::GLenum mode, api::GLuint start, api::GLuint end, const api::GLint* first, const api::GLsizei* count, api::GLsizei primcount)
{
	log() << warning << "MultiDrawRangeElementArrayAPPLE not available." << std::endl;
}

static void fallbackMultiModeDrawArraysIBM(const api::GLenum* mode, const api::GLint* first, const api::GLsizei* count, api::GLsizei primcount, api::GLint modestride)
{
	log() << warning << "MultiModeDrawArraysIBM not available." << std::endl;
}

static void fallbackMultiModeDrawElementsIBM(const api::GLenum* mode, const api::GLsizei* count, api::GLenum type, const api::GLvoid* const* indices, api::GLsizei primcount, api::GLint modestride)
{
	log() << warning << "MultiModeDrawElementsIBM not available." << std::endl;
}

static void fallbackMultiTexBufferEXT(api::GLenum texunit, api::GLenum target, api::GLenum internalformat, api::GLuint buffer)
{
	log() << warning << "MultiTexBufferEXT not available." << std::endl;
}

static void fallbackMultiTexCoord1d(api::GLenum target, api::GLdouble s)
{
	log() << warning << "MultiTexCoord1d not available." << std::endl;
}

static void fallbackMultiTexCoord1dARB(api::GLenum target, api::GLdouble s)
{
	log() << warning << "MultiTexCoord1dARB not available." << std::endl;
}

static void fallbackMultiTexCoord1dv(api::GLenum target, const api::GLdouble* v)
{
	log() << warning << "MultiTexCoord1dv not available." << std::endl;
}

static void fallbackMultiTexCoord1dvARB(api::GLenum target, const api::GLdouble* v)
{
	log() << warning << "MultiTexCoord1dvARB not available." << std::endl;
}

static void fallbackMultiTexCoord1f(api::GLenum target, api::GLfloat s)
{
	log() << warning << "MultiTexCoord1f not available." << std::endl;
}

static void fallbackMultiTexCoord1fARB(api::GLenum target, api::GLfloat s)
{
	log() << warning << "MultiTexCoord1fARB not available." << std::endl;
}

static void fallbackMultiTexCoord1fv(api::GLenum target, const api::GLfloat* v)
{
	log() << warning << "MultiTexCoord1fv not available." << std::endl;
}

static void fallbackMultiTexCoord1fvARB(api::GLenum target, const api::GLfloat* v)
{
	log() << warning << "MultiTexCoord1fvARB not available." << std::endl;
}

static void fallbackMultiTexCoord1hNV(api::GLenum target, api::GLhalfNV s)
{
	log() << warning << "MultiTexCoord1hNV not available." << std::endl;
}

static void fallbackMultiTexCoord1hvNV(api::GLenum target, const api::GLhalfNV* v)
{
	log() << warning << "MultiTexCoord1hvNV not available." << std::endl;
}

static void fallbackMultiTexCoord1i(api::GLenum target, api::GLint s)
{
	log() << warning << "MultiTexCoord1i not available." << std::endl;
}

static void fallbackMultiTexCoord1iARB(api::GLenum target, api::GLint s)
{
	log() << warning << "MultiTexCoord1iARB not available." << std::endl;
}

static void fallbackMultiTexCoord1iv(api::GLenum target, const api::GLint* v)
{
	log() << warning << "MultiTexCoord1iv not available." << std::endl;
}

static void fallbackMultiTexCoord1ivARB(api::GLenum target, const api::GLint* v)
{
	log() << warning << "MultiTexCoord1ivARB not available." << std::endl;
}

static void fallbackMultiTexCoord1s(api::GLenum target, api::GLshort s)
{
	log() << warning << "MultiTexCoord1s not available." << std::endl;
}

static void fallbackMultiTexCoord1sARB(api::GLenum target, api::GLshort s)
{
	log() << warning << "MultiTexCoord1sARB not available." << std::endl;
}

static void fallbackMultiTexCoord1sv(api::GLenum target, const api::GLshort* v)
{
	log() << warning << "MultiTexCoord1sv not available." << std::endl;
}

static void fallbackMultiTexCoord1svARB(api::GLenum target, const api::GLshort* v)
{
	log() << warning << "MultiTexCoord1svARB not available." << std::endl;
}

static void fallbackMultiTexCoord2d(api::GLenum target, api::GLdouble s, api::GLdouble t)
{
	log() << warning << "MultiTexCoord2d not available." << std::endl;
}

static void fallbackMultiTexCoord2dARB(api::GLenum target, api::GLdouble s, api::GLdouble t)
{
	log() << warning << "MultiTexCoord2dARB not available." << std::endl;
}

static void fallbackMultiTexCoord2dv(api::GLenum target, const api::GLdouble* v)
{
	log() << warning << "MultiTexCoord2dv not available." << std::endl;
}

static void fallbackMultiTexCoord2dvARB(api::GLenum target, const api::GLdouble* v)
{
	log() << warning << "MultiTexCoord2dvARB not available." << std::endl;
}

static void fallbackMultiTexCoord2f(api::GLenum target, api::GLfloat s, api::GLfloat t)
{
	log() << warning << "MultiTexCoord2f not available." << std::endl;
}

static void fallbackMultiTexCoord2fARB(api::GLenum target, api::GLfloat s, api::GLfloat t)
{
	log() << warning << "MultiTexCoord2fARB not available." << std::endl;
}

static void fallbackMultiTexCoord2fv(api::GLenum target, const api::GLfloat* v)
{
	log() << warning << "MultiTexCoord2fv not available." << std::endl;
}

static void fallbackMultiTexCoord2fvARB(api::GLenum target, const api::GLfloat* v)
{
	log() << warning << "MultiTexCoord2fvARB not available." << std::endl;
}

static void fallbackMultiTexCoord2hNV(api::GLenum target, api::GLhalfNV s, api::GLhalfNV t)
{
	log() << warning << "MultiTexCoord2hNV not available." << std::endl;
}

static void fallbackMultiTexCoord2hvNV(api::GLenum target, const api::GLhalfNV* v)
{
	log() << warning << "MultiTexCoord2hvNV not available." << std::endl;
}

static void fallbackMultiTexCoord2i(api::GLenum target, api::GLint s, api::GLint t)
{
	log() << warning << "MultiTexCoord2i not available." << std::endl;
}

static void fallbackMultiTexCoord2iARB(api::GLenum target, api::GLint s, api::GLint t)
{
	log() << warning << "MultiTexCoord2iARB not available." << std::endl;
}

static void fallbackMultiTexCoord2iv(api::GLenum target, const api::GLint* v)
{
	log() << warning << "MultiTexCoord2iv not available." << std::endl;
}

static void fallbackMultiTexCoord2ivARB(api::GLenum target, const api::GLint* v)
{
	log() << warning << "MultiTexCoord2ivARB not available." << std::endl;
}

static void fallbackMultiTexCoord2s(api::GLenum target, api::GLshort s, api::GLshort t)
{
	log() << warning << "MultiTexCoord2s not available." << std::endl;
}

static void fallbackMultiTexCoord2sARB(api::GLenum target, api::GLshort s, api::GLshort t)
{
	log() << warning << "MultiTexCoord2sARB not available." << std::endl;
}

static void fallbackMultiTexCoord2sv(api::GLenum target, const api::GLshort* v)
{
	log() << warning << "MultiTexCoord2sv not available." << std::endl;
}

static void fallbackMultiTexCoord2svARB(api::GLenum target, const api::GLshort* v)
{
	log() << warning << "MultiTexCoord2svARB not available." << std::endl;
}

static void fallbackMultiTexCoord3d(api::GLenum target, api::GLdouble s, api::GLdouble t, api::GLdouble r)
{
	log() << warning << "MultiTexCoord3d not available." << std::endl;
}

static void fallbackMultiTexCoord3dARB(api::GLenum target, api::GLdouble s, api::GLdouble t, api::GLdouble r)
{
	log() << warning << "MultiTexCoord3dARB not available." << std::endl;
}

static void fallbackMultiTexCoord3dv(api::GLenum target, const api::GLdouble* v)
{
	log() << warning << "MultiTexCoord3dv not available." << std::endl;
}

static void fallbackMultiTexCoord3dvARB(api::GLenum target, const api::GLdouble* v)
{
	log() << warning << "MultiTexCoord3dvARB not available." << std::endl;
}

static void fallbackMultiTexCoord3f(api::GLenum target, api::GLfloat s, api::GLfloat t, api::GLfloat r)
{
	log() << warning << "MultiTexCoord3f not available." << std::endl;
}

static void fallbackMultiTexCoord3fARB(api::GLenum target, api::GLfloat s, api::GLfloat t, api::GLfloat r)
{
	log() << warning << "MultiTexCoord3fARB not available." << std::endl;
}

static void fallbackMultiTexCoord3fv(api::GLenum target, const api::GLfloat* v)
{
	log() << warning << "MultiTexCoord3fv not available." << std::endl;
}

static void fallbackMultiTexCoord3fvARB(api::GLenum target, const api::GLfloat* v)
{
	log() << warning << "MultiTexCoord3fvARB not available." << std::endl;
}

static void fallbackMultiTexCoord3hNV(api::GLenum target, api::GLhalfNV s, api::GLhalfNV t, api::GLhalfNV r)
{
	log() << warning << "MultiTexCoord3hNV not available." << std::endl;
}

static void fallbackMultiTexCoord3hvNV(api::GLenum target, const api::GLhalfNV* v)
{
	log() << warning << "MultiTexCoord3hvNV not available." << std::endl;
}

static void fallbackMultiTexCoord3i(api::GLenum target, api::GLint s, api::GLint t, api::GLint r)
{
	log() << warning << "MultiTexCoord3i not available." << std::endl;
}

static void fallbackMultiTexCoord3iARB(api::GLenum target, api::GLint s, api::GLint t, api::GLint r)
{
	log() << warning << "MultiTexCoord3iARB not available." << std::endl;
}

static void fallbackMultiTexCoord3iv(api::GLenum target, const api::GLint* v)
{
	log() << warning << "MultiTexCoord3iv not available." << std::endl;
}

static void fallbackMultiTexCoord3ivARB(api::GLenum target, const api::GLint* v)
{
	log() << warning << "MultiTexCoord3ivARB not available." << std::endl;
}

static void fallbackMultiTexCoord3s(api::GLenum target, api::GLshort s, api::GLshort t, api::GLshort r)
{
	log() << warning << "MultiTexCoord3s not available." << std::endl;
}

static void fallbackMultiTexCoord3sARB(api::GLenum target, api::GLshort s, api::GLshort t, api::GLshort r)
{
	log() << warning << "MultiTexCoord3sARB not available." << std::endl;
}

static void fallbackMultiTexCoord3sv(api::GLenum target, const api::GLshort* v)
{
	log() << warning << "MultiTexCoord3sv not available." << std::endl;
}

static void fallbackMultiTexCoord3svARB(api::GLenum target, const api::GLshort* v)
{
	log() << warning << "MultiTexCoord3svARB not available." << std::endl;
}

static void fallbackMultiTexCoord4d(api::GLenum target, api::GLdouble s, api::GLdouble t, api::GLdouble r, api::GLdouble q)
{
	log() << warning << "MultiTexCoord4d not available." << std::endl;
}

static void fallbackMultiTexCoord4dARB(api::GLenum target, api::GLdouble s, api::GLdouble t, api::GLdouble r, api::GLdouble q)
{
	log() << warning << "MultiTexCoord4dARB not available." << std::endl;
}

static void fallbackMultiTexCoord4dv(api::GLenum target, const api::GLdouble* v)
{
	log() << warning << "MultiTexCoord4dv not available." << std::endl;
}

static void fallbackMultiTexCoord4dvARB(api::GLenum target, const api::GLdouble* v)
{
	log() << warning << "MultiTexCoord4dvARB not available." << std::endl;
}

static void fallbackMultiTexCoord4f(api::GLenum target, api::GLfloat s, api::GLfloat t, api::GLfloat r, api::GLfloat q)
{
	log() << warning << "MultiTexCoord4f not available." << std::endl;
}

static void fallbackMultiTexCoord4fARB(api::GLenum target, api::GLfloat s, api::GLfloat t, api::GLfloat r, api::GLfloat q)
{
	log() << warning << "MultiTexCoord4fARB not available." << std::endl;
}

static void fallbackMultiTexCoord4fv(api::GLenum target, const api::GLfloat* v)
{
	log() << warning << "MultiTexCoord4fv not available." << std::endl;
}

static void fallbackMultiTexCoord4fvARB(api::GLenum target, const api::GLfloat* v)
{
	log() << warning << "MultiTexCoord4fvARB not available." << std::endl;
}

static void fallbackMultiTexCoord4hNV(api::GLenum target, api::GLhalfNV s, api::GLhalfNV t, api::GLhalfNV r, api::GLhalfNV q)
{
	log() << warning << "MultiTexCoord4hNV not available." << std::endl;
}

static void fallbackMultiTexCoord4hvNV(api::GLenum target, const api::GLhalfNV* v)
{
	log() << warning << "MultiTexCoord4hvNV not available." << std::endl;
}

static void fallbackMultiTexCoord4i(api::GLenum target, api::GLint s, api::GLint t, api::GLint r, api::GLint q)
{
	log() << warning << "MultiTexCoord4i not available." << std::endl;
}

static void fallbackMultiTexCoord4iARB(api::GLenum target, api::GLint s, api::GLint t, api::GLint r, api::GLint q)
{
	log() << warning << "MultiTexCoord4iARB not available." << std::endl;
}

static void fallbackMultiTexCoord4iv(api::GLenum target, const api::GLint* v)
{
	log() << warning << "MultiTexCoord4iv not available." << std::endl;
}

static void fallbackMultiTexCoord4ivARB(api::GLenum target, const api::GLint* v)
{
	log() << warning << "MultiTexCoord4ivARB not available." << std::endl;
}

static void fallbackMultiTexCoord4s(api::GLenum target, api::GLshort s, api::GLshort t, api::GLshort r, api::GLshort q)
{
	log() << warning << "MultiTexCoord4s not available." << std::endl;
}

static void fallbackMultiTexCoord4sARB(api::GLenum target, api::GLshort s, api::GLshort t, api::GLshort r, api::GLshort q)
{
	log() << warning << "MultiTexCoord4sARB not available." << std::endl;
}

static void fallbackMultiTexCoord4sv(api::GLenum target, const api::GLshort* v)
{
	log() << warning << "MultiTexCoord4sv not available." << std::endl;
}

static void fallbackMultiTexCoord4svARB(api::GLenum target, const api::GLshort* v)
{
	log() << warning << "MultiTexCoord4svARB not available." << std::endl;
}

static void fallbackMultiTexCoordP1ui(api::GLenum texture, api::GLenum type, api::GLuint coords)
{
	log() << warning << "MultiTexCoordP1ui not available." << std::endl;
}

static void fallbackMultiTexCoordP1uiv(api::GLenum texture, api::GLenum type, const api::GLuint* coords)
{
	log() << warning << "MultiTexCoordP1uiv not available." << std::endl;
}

static void fallbackMultiTexCoordP2ui(api::GLenum texture, api::GLenum type, api::GLuint coords)
{
	log() << warning << "MultiTexCoordP2ui not available." << std::endl;
}

static void fallbackMultiTexCoordP2uiv(api::GLenum texture, api::GLenum type, const api::GLuint* coords)
{
	log() << warning << "MultiTexCoordP2uiv not available." << std::endl;
}

static void fallbackMultiTexCoordP3ui(api::GLenum texture, api::GLenum type, api::GLuint coords)
{
	log() << warning << "MultiTexCoordP3ui not available." << std::endl;
}

static void fallbackMultiTexCoordP3uiv(api::GLenum texture, api::GLenum type, const api::GLuint* coords)
{
	log() << warning << "MultiTexCoordP3uiv not available." << std::endl;
}

static void fallbackMultiTexCoordP4ui(api::GLenum texture, api::GLenum type, api::GLuint coords)
{
	log() << warning << "MultiTexCoordP4ui not available." << std::endl;
}

static void fallbackMultiTexCoordP4uiv(api::GLenum texture, api::GLenum type, const api::GLuint* coords)
{
	log() << warning << "MultiTexCoordP4uiv not available." << std::endl;
}

static void fallbackMultiTexCoordPointerEXT(api::GLenum texunit, api::GLint size, api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "MultiTexCoordPointerEXT not available." << std::endl;
}

static void fallbackMultiTexEnvfEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, api::GLfloat param)
{
	log() << warning << "MultiTexEnvfEXT not available." << std::endl;
}

static void fallbackMultiTexEnvfvEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "MultiTexEnvfvEXT not available." << std::endl;
}

static void fallbackMultiTexEnviEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, api::GLint param)
{
	log() << warning << "MultiTexEnviEXT not available." << std::endl;
}

static void fallbackMultiTexEnvivEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "MultiTexEnvivEXT not available." << std::endl;
}

static void fallbackMultiTexGendEXT(api::GLenum texunit, api::GLenum coord, api::GLenum pname, api::GLdouble param)
{
	log() << warning << "MultiTexGendEXT not available." << std::endl;
}

static void fallbackMultiTexGendvEXT(api::GLenum texunit, api::GLenum coord, api::GLenum pname, const api::GLdouble* params)
{
	log() << warning << "MultiTexGendvEXT not available." << std::endl;
}

static void fallbackMultiTexGenfEXT(api::GLenum texunit, api::GLenum coord, api::GLenum pname, api::GLfloat param)
{
	log() << warning << "MultiTexGenfEXT not available." << std::endl;
}

static void fallbackMultiTexGenfvEXT(api::GLenum texunit, api::GLenum coord, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "MultiTexGenfvEXT not available." << std::endl;
}

static void fallbackMultiTexGeniEXT(api::GLenum texunit, api::GLenum coord, api::GLenum pname, api::GLint param)
{
	log() << warning << "MultiTexGeniEXT not available." << std::endl;
}

static void fallbackMultiTexGenivEXT(api::GLenum texunit, api::GLenum coord, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "MultiTexGenivEXT not available." << std::endl;
}

static void fallbackMultiTexImage1DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLint border, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "MultiTexImage1DEXT not available." << std::endl;
}

static void fallbackMultiTexImage2DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLint border, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "MultiTexImage2DEXT not available." << std::endl;
}

static void fallbackMultiTexImage3DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLint border, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "MultiTexImage3DEXT not available." << std::endl;
}

static void fallbackMultiTexParameterIivEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "MultiTexParameterIivEXT not available." << std::endl;
}

static void fallbackMultiTexParameterIuivEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, const api::GLuint* params)
{
	log() << warning << "MultiTexParameterIuivEXT not available." << std::endl;
}

static void fallbackMultiTexParameterfEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, api::GLfloat param)
{
	log() << warning << "MultiTexParameterfEXT not available." << std::endl;
}

static void fallbackMultiTexParameterfvEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "MultiTexParameterfvEXT not available." << std::endl;
}

static void fallbackMultiTexParameteriEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, api::GLint param)
{
	log() << warning << "MultiTexParameteriEXT not available." << std::endl;
}

static void fallbackMultiTexParameterivEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "MultiTexParameterivEXT not available." << std::endl;
}

static void fallbackMultiTexRenderbufferEXT(api::GLenum texunit, api::GLenum target, api::GLuint renderbuffer)
{
	log() << warning << "MultiTexRenderbufferEXT not available." << std::endl;
}

static void fallbackMultiTexSubImage1DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLsizei width, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "MultiTexSubImage1DEXT not available." << std::endl;
}

static void fallbackMultiTexSubImage2DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLsizei width, api::GLsizei height, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "MultiTexSubImage2DEXT not available." << std::endl;
}

static void fallbackMultiTexSubImage3DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint zoffset, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "MultiTexSubImage3DEXT not available." << std::endl;
}

static void fallbackNamedBufferDataEXT(api::GLuint buffer, api::GLsizeiptr size, const api::GLvoid* data, api::GLenum usage)
{
	log() << warning << "NamedBufferDataEXT not available." << std::endl;
}

static void fallbackNamedBufferSubDataEXT(api::GLuint buffer, api::GLintptr offset, api::GLsizeiptr size, const api::GLvoid* data)
{
	log() << warning << "NamedBufferSubDataEXT not available." << std::endl;
}

static void fallbackNamedFramebufferRenderbufferEXT(api::GLuint framebuffer, api::GLenum attachment, api::GLenum renderbuffertarget, api::GLuint renderbuffer)
{
	log() << warning << "NamedFramebufferRenderbufferEXT not available." << std::endl;
}

static void fallbackNamedFramebufferTexture1DEXT(api::GLuint framebuffer, api::GLenum attachment, api::GLenum textarget, api::GLuint texture, api::GLint level)
{
	log() << warning << "NamedFramebufferTexture1DEXT not available." << std::endl;
}

static void fallbackNamedFramebufferTexture2DEXT(api::GLuint framebuffer, api::GLenum attachment, api::GLenum textarget, api::GLuint texture, api::GLint level)
{
	log() << warning << "NamedFramebufferTexture2DEXT not available." << std::endl;
}

static void fallbackNamedFramebufferTexture3DEXT(api::GLuint framebuffer, api::GLenum attachment, api::GLenum textarget, api::GLuint texture, api::GLint level, api::GLint zoffset)
{
	log() << warning << "NamedFramebufferTexture3DEXT not available." << std::endl;
}

static void fallbackNamedFramebufferTextureEXT(api::GLuint framebuffer, api::GLenum attachment, api::GLuint texture, api::GLint level)
{
	log() << warning << "NamedFramebufferTextureEXT not available." << std::endl;
}

static void fallbackNamedFramebufferTextureFaceEXT(api::GLuint framebuffer, api::GLenum attachment, api::GLuint texture, api::GLint level, api::GLenum face)
{
	log() << warning << "NamedFramebufferTextureFaceEXT not available." << std::endl;
}

static void fallbackNamedFramebufferTextureLayerEXT(api::GLuint framebuffer, api::GLenum attachment, api::GLuint texture, api::GLint level, api::GLint layer)
{
	log() << warning << "NamedFramebufferTextureLayerEXT not available." << std::endl;
}

static void fallbackNamedProgramLocalParameter4dEXT(api::GLuint program, api::GLenum target, api::GLuint index, api::GLdouble x, api::GLdouble y, api::GLdouble z, api::GLdouble w)
{
	log() << warning << "NamedProgramLocalParameter4dEXT not available." << std::endl;
}

static void fallbackNamedProgramLocalParameter4dvEXT(api::GLuint program, api::GLenum target, api::GLuint index, const api::GLdouble* params)
{
	log() << warning << "NamedProgramLocalParameter4dvEXT not available." << std::endl;
}

static void fallbackNamedProgramLocalParameter4fEXT(api::GLuint program, api::GLenum target, api::GLuint index, api::GLfloat x, api::GLfloat y, api::GLfloat z, api::GLfloat w)
{
	log() << warning << "NamedProgramLocalParameter4fEXT not available." << std::endl;
}

static void fallbackNamedProgramLocalParameter4fvEXT(api::GLuint program, api::GLenum target, api::GLuint index, const api::GLfloat* params)
{
	log() << warning << "NamedProgramLocalParameter4fvEXT not available." << std::endl;
}

static void fallbackNamedProgramLocalParameterI4iEXT(api::GLuint program, api::GLenum target, api::GLuint index, api::GLint x, api::GLint y, api::GLint z, api::GLint w)
{
	log() << warning << "NamedProgramLocalParameterI4iEXT not available." << std::endl;
}

static void fallbackNamedProgramLocalParameterI4ivEXT(api::GLuint program, api::GLenum target, api::GLuint index, const api::GLint* params)
{
	log() << warning << "NamedProgramLocalParameterI4ivEXT not available." << std::endl;
}

static void fallbackNamedProgramLocalParameterI4uiEXT(api::GLuint program, api::GLenum target, api::GLuint index, api::GLuint x, api::GLuint y, api::GLuint z, api::GLuint w)
{
	log() << warning << "NamedProgramLocalParameterI4uiEXT not available." << std::endl;
}

static void fallbackNamedProgramLocalParameterI4uivEXT(api::GLuint program, api::GLenum target, api::GLuint index, const api::GLuint* params)
{
	log() << warning << "NamedProgramLocalParameterI4uivEXT not available." << std::endl;
}

static void fallbackNamedProgramLocalParameters4fvEXT(api::GLuint program, api::GLenum target, api::GLuint index, api::GLsizei count, const api::GLfloat* params)
{
	log() << warning << "NamedProgramLocalParameters4fvEXT not available." << std::endl;
}

static void fallbackNamedProgramLocalParametersI4ivEXT(api::GLuint program, api::GLenum target, api::GLuint index, api::GLsizei count, const api::GLint* params)
{
	log() << warning << "NamedProgramLocalParametersI4ivEXT not available." << std::endl;
}

static void fallbackNamedProgramLocalParametersI4uivEXT(api::GLuint program, api::GLenum target, api::GLuint index, api::GLsizei count, const api::GLuint* params)
{
	log() << warning << "NamedProgramLocalParametersI4uivEXT not available." << std::endl;
}

static void fallbackNamedProgramStringEXT(api::GLuint program, api::GLenum target, api::GLenum format, api::GLsizei len, const api::GLvoid* string)
{
	log() << warning << "NamedProgramStringEXT not available." << std::endl;
}

static void fallbackNamedRenderbufferStorageEXT(api::GLuint renderbuffer, api::GLenum internalformat, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "NamedRenderbufferStorageEXT not available." << std::endl;
}

static void fallbackNamedRenderbufferStorageMultisampleCoverageEXT(api::GLuint renderbuffer, api::GLsizei coverageSamples, api::GLsizei colorSamples, api::GLenum internalformat, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "NamedRenderbufferStorageMultisampleCoverageEXT not available." << std::endl;
}

static void fallbackNamedRenderbufferStorageMultisampleEXT(api::GLuint renderbuffer, api::GLsizei samples, api::GLenum internalformat, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "NamedRenderbufferStorageMultisampleEXT not available." << std::endl;
}

static void fallbackNamedStringARB(api::GLenum type, api::GLint namelen, const api::GLchar* name, api::GLint stringlen, const api::GLchar* string)
{
	log() << warning << "NamedStringARB not available." << std::endl;
}

static void fallbackNewList(api::GLuint list, api::GLenum mode)
{
	log() << warning << "NewList not available." << std::endl;
}

static api::GLuint fallbackNewObjectBufferATI(api::GLsizei size, const api::GLvoid* pointer, api::GLenum usage)
{
	log() << warning << "NewObjectBufferATI not available." << std::endl;
}

static void fallbackNormal3b(api::GLbyte nx, api::GLbyte ny, api::GLbyte nz)
{
	log() << warning << "Normal3b not available." << std::endl;
}

static void fallbackNormal3bv(const api::GLbyte* v)
{
	log() << warning << "Normal3bv not available." << std::endl;
}

static void fallbackNormal3d(api::GLdouble nx, api::GLdouble ny, api::GLdouble nz)
{
	log() << warning << "Normal3d not available." << std::endl;
}

static void fallbackNormal3dv(const api::GLdouble* v)
{
	log() << warning << "Normal3dv not available." << std::endl;
}

static void fallbackNormal3f(api::GLfloat nx, api::GLfloat ny, api::GLfloat nz)
{
	log() << warning << "Normal3f not available." << std::endl;
}

static void fallbackNormal3fVertex3fSUN(api::GLfloat nx, api::GLfloat ny, api::GLfloat nz, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "Normal3fVertex3fSUN not available." << std::endl;
}

static void fallbackNormal3fVertex3fvSUN(const api::GLfloat* n, const api::GLfloat* v)
{
	log() << warning << "Normal3fVertex3fvSUN not available." << std::endl;
}

static void fallbackNormal3fv(const api::GLfloat* v)
{
	log() << warning << "Normal3fv not available." << std::endl;
}

static void fallbackNormal3hNV(api::GLhalfNV nx, api::GLhalfNV ny, api::GLhalfNV nz)
{
	log() << warning << "Normal3hNV not available." << std::endl;
}

static void fallbackNormal3hvNV(const api::GLhalfNV* v)
{
	log() << warning << "Normal3hvNV not available." << std::endl;
}

static void fallbackNormal3i(api::GLint nx, api::GLint ny, api::GLint nz)
{
	log() << warning << "Normal3i not available." << std::endl;
}

static void fallbackNormal3iv(const api::GLint* v)
{
	log() << warning << "Normal3iv not available." << std::endl;
}

static void fallbackNormal3s(api::GLshort nx, api::GLshort ny, api::GLshort nz)
{
	log() << warning << "Normal3s not available." << std::endl;
}

static void fallbackNormal3sv(const api::GLshort* v)
{
	log() << warning << "Normal3sv not available." << std::endl;
}

static void fallbackNormalFormatNV(api::GLenum type, api::GLsizei stride)
{
	log() << warning << "NormalFormatNV not available." << std::endl;
}

static void fallbackNormalP3ui(api::GLenum type, api::GLuint coords)
{
	log() << warning << "NormalP3ui not available." << std::endl;
}

static void fallbackNormalP3uiv(api::GLenum type, const api::GLuint* coords)
{
	log() << warning << "NormalP3uiv not available." << std::endl;
}

static void fallbackNormalPointer(api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "NormalPointer not available." << std::endl;
}

static void fallbackNormalPointerEXT(api::GLenum type, api::GLsizei stride, api::GLsizei count, const api::GLvoid* pointer)
{
	log() << warning << "NormalPointerEXT not available." << std::endl;
}

static void fallbackNormalPointerListIBM(api::GLenum type, api::GLint stride, const api::GLvoid** pointer, api::GLint ptrstride)
{
	log() << warning << "NormalPointerListIBM not available." << std::endl;
}

static void fallbackNormalPointervINTEL(api::GLenum type, const api::GLvoid** pointer)
{
	log() << warning << "NormalPointervINTEL not available." << std::endl;
}

static void fallbackNormalStream3bATI(api::GLenum stream, api::GLbyte nx, api::GLbyte ny, api::GLbyte nz)
{
	log() << warning << "NormalStream3bATI not available." << std::endl;
}

static void fallbackNormalStream3bvATI(api::GLenum stream, const api::GLbyte* coords)
{
	log() << warning << "NormalStream3bvATI not available." << std::endl;
}

static void fallbackNormalStream3dATI(api::GLenum stream, api::GLdouble nx, api::GLdouble ny, api::GLdouble nz)
{
	log() << warning << "NormalStream3dATI not available." << std::endl;
}

static void fallbackNormalStream3dvATI(api::GLenum stream, const api::GLdouble* coords)
{
	log() << warning << "NormalStream3dvATI not available." << std::endl;
}

static void fallbackNormalStream3fATI(api::GLenum stream, api::GLfloat nx, api::GLfloat ny, api::GLfloat nz)
{
	log() << warning << "NormalStream3fATI not available." << std::endl;
}

static void fallbackNormalStream3fvATI(api::GLenum stream, const api::GLfloat* coords)
{
	log() << warning << "NormalStream3fvATI not available." << std::endl;
}

static void fallbackNormalStream3iATI(api::GLenum stream, api::GLint nx, api::GLint ny, api::GLint nz)
{
	log() << warning << "NormalStream3iATI not available." << std::endl;
}

static void fallbackNormalStream3ivATI(api::GLenum stream, const api::GLint* coords)
{
	log() << warning << "NormalStream3ivATI not available." << std::endl;
}

static void fallbackNormalStream3sATI(api::GLenum stream, api::GLshort nx, api::GLshort ny, api::GLshort nz)
{
	log() << warning << "NormalStream3sATI not available." << std::endl;
}

static void fallbackNormalStream3svATI(api::GLenum stream, const api::GLshort* coords)
{
	log() << warning << "NormalStream3svATI not available." << std::endl;
}

static api::GLenum fallbackObjectPurgeableAPPLE(api::GLenum objectType, api::GLuint name, api::GLenum option)
{
	log() << warning << "ObjectPurgeableAPPLE not available." << std::endl;
}

static api::GLenum fallbackObjectUnpurgeableAPPLE(api::GLenum objectType, api::GLuint name, api::GLenum option)
{
	log() << warning << "ObjectUnpurgeableAPPLE not available." << std::endl;
}

static void fallbackOrtho(api::GLdouble left, api::GLdouble right, api::GLdouble bottom, api::GLdouble top, api::GLdouble zNear, api::GLdouble zFar)
{
	log() << warning << "Ortho not available." << std::endl;
}

static void fallbackPNTrianglesfATI(api::GLenum pname, api::GLfloat param)
{
	log() << warning << "PNTrianglesfATI not available." << std::endl;
}

static void fallbackPNTrianglesiATI(api::GLenum pname, api::GLint param)
{
	log() << warning << "PNTrianglesiATI not available." << std::endl;
}

static void fallbackPassTexCoordATI(api::GLuint dst, api::GLuint coord, api::GLenum swizzle)
{
	log() << warning << "PassTexCoordATI not available." << std::endl;
}

static void fallbackPassThrough(api::GLfloat token)
{
	log() << warning << "PassThrough not available." << std::endl;
}

static void fallbackPatchParameterfv(api::GLenum pname, const api::GLfloat* values)
{
	log() << warning << "PatchParameterfv not available." << std::endl;
}

static void fallbackPatchParameteri(api::GLenum pname, api::GLint value)
{
	log() << warning << "PatchParameteri not available." << std::endl;
}

static void fallbackPauseTransformFeedback()
{
	log() << warning << "PauseTransformFeedback not available." << std::endl;
}

static void fallbackPauseTransformFeedbackNV()
{
	log() << warning << "PauseTransformFeedbackNV not available." << std::endl;
}

static void fallbackPixelDataRangeNV(api::GLenum target, api::GLsizei length, api::GLvoid* pointer)
{
	log() << warning << "PixelDataRangeNV not available." << std::endl;
}

static void fallbackPixelMapfv(api::GLenum map, api::GLint mapsize, const api::GLfloat* values)
{
	log() << warning << "PixelMapfv not available." << std::endl;
}

static void fallbackPixelMapuiv(api::GLenum map, api::GLint mapsize, const api::GLuint* values)
{
	log() << warning << "PixelMapuiv not available." << std::endl;
}

static void fallbackPixelMapusv(api::GLenum map, api::GLint mapsize, const api::GLushort* values)
{
	log() << warning << "PixelMapusv not available." << std::endl;
}

static void fallbackPixelStoref(api::GLenum pname, api::GLfloat param)
{
	log() << warning << "PixelStoref not available." << std::endl;
}

static void fallbackPixelStorei(api::GLenum pname, api::GLint param)
{
	log() << warning << "PixelStorei not available." << std::endl;
}

static void fallbackPixelTexGenParameterfSGIS(api::GLenum pname, api::GLfloat param)
{
	log() << warning << "PixelTexGenParameterfSGIS not available." << std::endl;
}

static void fallbackPixelTexGenParameterfvSGIS(api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "PixelTexGenParameterfvSGIS not available." << std::endl;
}

static void fallbackPixelTexGenParameteriSGIS(api::GLenum pname, api::GLint param)
{
	log() << warning << "PixelTexGenParameteriSGIS not available." << std::endl;
}

static void fallbackPixelTexGenParameterivSGIS(api::GLenum pname, const api::GLint* params)
{
	log() << warning << "PixelTexGenParameterivSGIS not available." << std::endl;
}

static void fallbackPixelTexGenSGIX(api::GLenum mode)
{
	log() << warning << "PixelTexGenSGIX not available." << std::endl;
}

static void fallbackPixelTransferf(api::GLenum pname, api::GLfloat param)
{
	log() << warning << "PixelTransferf not available." << std::endl;
}

static void fallbackPixelTransferi(api::GLenum pname, api::GLint param)
{
	log() << warning << "PixelTransferi not available." << std::endl;
}

static void fallbackPixelTransformParameterfEXT(api::GLenum target, api::GLenum pname, api::GLfloat param)
{
	log() << warning << "PixelTransformParameterfEXT not available." << std::endl;
}

static void fallbackPixelTransformParameterfvEXT(api::GLenum target, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "PixelTransformParameterfvEXT not available." << std::endl;
}

static void fallbackPixelTransformParameteriEXT(api::GLenum target, api::GLenum pname, api::GLint param)
{
	log() << warning << "PixelTransformParameteriEXT not available." << std::endl;
}

static void fallbackPixelTransformParameterivEXT(api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "PixelTransformParameterivEXT not available." << std::endl;
}

static void fallbackPixelZoom(api::GLfloat xfactor, api::GLfloat yfactor)
{
	log() << warning << "PixelZoom not available." << std::endl;
}

static void fallbackPointParameterf(api::GLenum pname, api::GLfloat param)
{
	log() << warning << "PointParameterf not available." << std::endl;
}

static void fallbackPointParameterfARB(api::GLenum pname, api::GLfloat param)
{
	log() << warning << "PointParameterfARB not available." << std::endl;
}

static void fallbackPointParameterfEXT(api::GLenum pname, api::GLfloat param)
{
	log() << warning << "PointParameterfEXT not available." << std::endl;
}

static void fallbackPointParameterfSGIS(api::GLenum pname, api::GLfloat param)
{
	log() << warning << "PointParameterfSGIS not available." << std::endl;
}

static void fallbackPointParameterfv(api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "PointParameterfv not available." << std::endl;
}

static void fallbackPointParameterfvARB(api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "PointParameterfvARB not available." << std::endl;
}

static void fallbackPointParameterfvEXT(api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "PointParameterfvEXT not available." << std::endl;
}

static void fallbackPointParameterfvSGIS(api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "PointParameterfvSGIS not available." << std::endl;
}

static void fallbackPointParameteri(api::GLenum pname, api::GLint param)
{
	log() << warning << "PointParameteri not available." << std::endl;
}

static void fallbackPointParameteriNV(api::GLenum pname, api::GLint param)
{
	log() << warning << "PointParameteriNV not available." << std::endl;
}

static void fallbackPointParameteriv(api::GLenum pname, const api::GLint* params)
{
	log() << warning << "PointParameteriv not available." << std::endl;
}

static void fallbackPointParameterivNV(api::GLenum pname, const api::GLint* params)
{
	log() << warning << "PointParameterivNV not available." << std::endl;
}

static void fallbackPointSize(api::GLfloat size)
{
	log() << warning << "PointSize not available." << std::endl;
}

static api::GLint fallbackPollAsyncSGIX(api::GLuint* markerp)
{
	log() << warning << "PollAsyncSGIX not available." << std::endl;
}

static api::GLint fallbackPollInstrumentsSGIX(api::GLint* marker_p)
{
	log() << warning << "PollInstrumentsSGIX not available." << std::endl;
}

static void fallbackPolygonMode(api::GLenum face, api::GLenum mode)
{
	log() << warning << "PolygonMode not available." << std::endl;
}

static void fallbackPolygonOffset(api::GLfloat factor, api::GLfloat units)
{
	log() << warning << "PolygonOffset not available." << std::endl;
}

static void fallbackPolygonOffsetEXT(api::GLfloat factor, api::GLfloat bias)
{
	log() << warning << "PolygonOffsetEXT not available." << std::endl;
}

static void fallbackPolygonStipple(const api::GLubyte* mask)
{
	log() << warning << "PolygonStipple not available." << std::endl;
}

static void fallbackPopAttrib()
{
	log() << warning << "PopAttrib not available." << std::endl;
}

static void fallbackPopClientAttrib()
{
	log() << warning << "PopClientAttrib not available." << std::endl;
}

static void fallbackPopMatrix()
{
	log() << warning << "PopMatrix not available." << std::endl;
}

static void fallbackPopName()
{
	log() << warning << "PopName not available." << std::endl;
}

static void fallbackPresentFrameDualFillNV(api::GLuint video_slot, api::GLuint64EXT minPresentTime, api::GLuint beginPresentTimeId, api::GLuint presentDurationId, api::GLenum type, api::GLenum target0, api::GLuint fill0, api::GLenum target1, api::GLuint fill1, api::GLenum target2, api::GLuint fill2, api::GLenum target3, api::GLuint fill3)
{
	log() << warning << "PresentFrameDualFillNV not available." << std::endl;
}

static void fallbackPresentFrameKeyedNV(api::GLuint video_slot, api::GLuint64EXT minPresentTime, api::GLuint beginPresentTimeId, api::GLuint presentDurationId, api::GLenum type, api::GLenum target0, api::GLuint fill0, api::GLuint key0, api::GLenum target1, api::GLuint fill1, api::GLuint key1)
{
	log() << warning << "PresentFrameKeyedNV not available." << std::endl;
}

static void fallbackPrimitiveRestartIndex(api::GLuint index)
{
	log() << warning << "PrimitiveRestartIndex not available." << std::endl;
}

static void fallbackPrimitiveRestartIndexNV(api::GLuint index)
{
	log() << warning << "PrimitiveRestartIndexNV not available." << std::endl;
}

static void fallbackPrimitiveRestartNV()
{
	log() << warning << "PrimitiveRestartNV not available." << std::endl;
}

static void fallbackPrioritizeTextures(api::GLsizei n, const api::GLuint* textures, const api::GLclampf* priorities)
{
	log() << warning << "PrioritizeTextures not available." << std::endl;
}

static void fallbackPrioritizeTexturesEXT(api::GLsizei n, const api::GLuint* textures, const api::GLclampf* priorities)
{
	log() << warning << "PrioritizeTexturesEXT not available." << std::endl;
}

static void fallbackProgramBufferParametersIivNV(api::GLenum target, api::GLuint buffer, api::GLuint index, api::GLsizei count, const api::GLint* params)
{
	log() << warning << "ProgramBufferParametersIivNV not available." << std::endl;
}

static void fallbackProgramBufferParametersIuivNV(api::GLenum target, api::GLuint buffer, api::GLuint index, api::GLsizei count, const api::GLuint* params)
{
	log() << warning << "ProgramBufferParametersIuivNV not available." << std::endl;
}

static void fallbackProgramBufferParametersfvNV(api::GLenum target, api::GLuint buffer, api::GLuint index, api::GLsizei count, const api::GLfloat* params)
{
	log() << warning << "ProgramBufferParametersfvNV not available." << std::endl;
}

static void fallbackProgramEnvParameter4dARB(api::GLenum target, api::GLuint index, api::GLdouble x, api::GLdouble y, api::GLdouble z, api::GLdouble w)
{
	log() << warning << "ProgramEnvParameter4dARB not available." << std::endl;
}

static void fallbackProgramEnvParameter4dvARB(api::GLenum target, api::GLuint index, const api::GLdouble* params)
{
	log() << warning << "ProgramEnvParameter4dvARB not available." << std::endl;
}

static void fallbackProgramEnvParameter4fARB(api::GLenum target, api::GLuint index, api::GLfloat x, api::GLfloat y, api::GLfloat z, api::GLfloat w)
{
	log() << warning << "ProgramEnvParameter4fARB not available." << std::endl;
}

static void fallbackProgramEnvParameter4fvARB(api::GLenum target, api::GLuint index, const api::GLfloat* params)
{
	log() << warning << "ProgramEnvParameter4fvARB not available." << std::endl;
}

static void fallbackProgramEnvParameterI4iNV(api::GLenum target, api::GLuint index, api::GLint x, api::GLint y, api::GLint z, api::GLint w)
{
	log() << warning << "ProgramEnvParameterI4iNV not available." << std::endl;
}

static void fallbackProgramEnvParameterI4ivNV(api::GLenum target, api::GLuint index, const api::GLint* params)
{
	log() << warning << "ProgramEnvParameterI4ivNV not available." << std::endl;
}

static void fallbackProgramEnvParameterI4uiNV(api::GLenum target, api::GLuint index, api::GLuint x, api::GLuint y, api::GLuint z, api::GLuint w)
{
	log() << warning << "ProgramEnvParameterI4uiNV not available." << std::endl;
}

static void fallbackProgramEnvParameterI4uivNV(api::GLenum target, api::GLuint index, const api::GLuint* params)
{
	log() << warning << "ProgramEnvParameterI4uivNV not available." << std::endl;
}

static void fallbackProgramEnvParameters4fvEXT(api::GLenum target, api::GLuint index, api::GLsizei count, const api::GLfloat* params)
{
	log() << warning << "ProgramEnvParameters4fvEXT not available." << std::endl;
}

static void fallbackProgramEnvParametersI4ivNV(api::GLenum target, api::GLuint index, api::GLsizei count, const api::GLint* params)
{
	log() << warning << "ProgramEnvParametersI4ivNV not available." << std::endl;
}

static void fallbackProgramEnvParametersI4uivNV(api::GLenum target, api::GLuint index, api::GLsizei count, const api::GLuint* params)
{
	log() << warning << "ProgramEnvParametersI4uivNV not available." << std::endl;
}

static void fallbackProgramLocalParameter4dARB(api::GLenum target, api::GLuint index, api::GLdouble x, api::GLdouble y, api::GLdouble z, api::GLdouble w)
{
	log() << warning << "ProgramLocalParameter4dARB not available." << std::endl;
}

static void fallbackProgramLocalParameter4dvARB(api::GLenum target, api::GLuint index, const api::GLdouble* params)
{
	log() << warning << "ProgramLocalParameter4dvARB not available." << std::endl;
}

static void fallbackProgramLocalParameter4fARB(api::GLenum target, api::GLuint index, api::GLfloat x, api::GLfloat y, api::GLfloat z, api::GLfloat w)
{
	log() << warning << "ProgramLocalParameter4fARB not available." << std::endl;
}

static void fallbackProgramLocalParameter4fvARB(api::GLenum target, api::GLuint index, const api::GLfloat* params)
{
	log() << warning << "ProgramLocalParameter4fvARB not available." << std::endl;
}

static void fallbackProgramLocalParameterI4iNV(api::GLenum target, api::GLuint index, api::GLint x, api::GLint y, api::GLint z, api::GLint w)
{
	log() << warning << "ProgramLocalParameterI4iNV not available." << std::endl;
}

static void fallbackProgramLocalParameterI4ivNV(api::GLenum target, api::GLuint index, const api::GLint* params)
{
	log() << warning << "ProgramLocalParameterI4ivNV not available." << std::endl;
}

static void fallbackProgramLocalParameterI4uiNV(api::GLenum target, api::GLuint index, api::GLuint x, api::GLuint y, api::GLuint z, api::GLuint w)
{
	log() << warning << "ProgramLocalParameterI4uiNV not available." << std::endl;
}

static void fallbackProgramLocalParameterI4uivNV(api::GLenum target, api::GLuint index, const api::GLuint* params)
{
	log() << warning << "ProgramLocalParameterI4uivNV not available." << std::endl;
}

static void fallbackProgramLocalParameters4fvEXT(api::GLenum target, api::GLuint index, api::GLsizei count, const api::GLfloat* params)
{
	log() << warning << "ProgramLocalParameters4fvEXT not available." << std::endl;
}

static void fallbackProgramLocalParametersI4ivNV(api::GLenum target, api::GLuint index, api::GLsizei count, const api::GLint* params)
{
	log() << warning << "ProgramLocalParametersI4ivNV not available." << std::endl;
}

static void fallbackProgramLocalParametersI4uivNV(api::GLenum target, api::GLuint index, api::GLsizei count, const api::GLuint* params)
{
	log() << warning << "ProgramLocalParametersI4uivNV not available." << std::endl;
}

static void fallbackProgramNamedParameter4dNV(api::GLuint id, api::GLsizei len, const api::GLubyte* name, api::GLdouble x, api::GLdouble y, api::GLdouble z, api::GLdouble w)
{
	log() << warning << "ProgramNamedParameter4dNV not available." << std::endl;
}

static void fallbackProgramNamedParameter4dvNV(api::GLuint id, api::GLsizei len, const api::GLubyte* name, const api::GLdouble* v)
{
	log() << warning << "ProgramNamedParameter4dvNV not available." << std::endl;
}

static void fallbackProgramNamedParameter4fNV(api::GLuint id, api::GLsizei len, const api::GLubyte* name, api::GLfloat x, api::GLfloat y, api::GLfloat z, api::GLfloat w)
{
	log() << warning << "ProgramNamedParameter4fNV not available." << std::endl;
}

static void fallbackProgramNamedParameter4fvNV(api::GLuint id, api::GLsizei len, const api::GLubyte* name, const api::GLfloat* v)
{
	log() << warning << "ProgramNamedParameter4fvNV not available." << std::endl;
}

static void fallbackProgramParameter4dNV(api::GLenum target, api::GLuint index, api::GLdouble x, api::GLdouble y, api::GLdouble z, api::GLdouble w)
{
	log() << warning << "ProgramParameter4dNV not available." << std::endl;
}

static void fallbackProgramParameter4dvNV(api::GLenum target, api::GLuint index, const api::GLdouble* v)
{
	log() << warning << "ProgramParameter4dvNV not available." << std::endl;
}

static void fallbackProgramParameter4fNV(api::GLenum target, api::GLuint index, api::GLfloat x, api::GLfloat y, api::GLfloat z, api::GLfloat w)
{
	log() << warning << "ProgramParameter4fNV not available." << std::endl;
}

static void fallbackProgramParameter4fvNV(api::GLenum target, api::GLuint index, const api::GLfloat* v)
{
	log() << warning << "ProgramParameter4fvNV not available." << std::endl;
}

static void fallbackProgramParameteri(api::GLuint program, api::GLenum pname, api::GLint value)
{
	log() << warning << "ProgramParameteri not available." << std::endl;
}

static void fallbackProgramParameteriARB(api::GLuint program, api::GLenum pname, api::GLint value)
{
	log() << warning << "ProgramParameteriARB not available." << std::endl;
}

static void fallbackProgramParameteriEXT(api::GLuint program, api::GLenum pname, api::GLint value)
{
	log() << warning << "ProgramParameteriEXT not available." << std::endl;
}

static void fallbackProgramParameters4dvNV(api::GLenum target, api::GLuint index, api::GLuint count, const api::GLdouble* v)
{
	log() << warning << "ProgramParameters4dvNV not available." << std::endl;
}

static void fallbackProgramParameters4fvNV(api::GLenum target, api::GLuint index, api::GLuint count, const api::GLfloat* v)
{
	log() << warning << "ProgramParameters4fvNV not available." << std::endl;
}

static void fallbackProgramStringARB(api::GLenum target, api::GLenum format, api::GLsizei len, const api::GLvoid* string)
{
	log() << warning << "ProgramStringARB not available." << std::endl;
}

static void fallbackProgramUniform1dEXT(api::GLuint program, api::GLint location, api::GLdouble x)
{
	log() << warning << "ProgramUniform1dEXT not available." << std::endl;
}

static void fallbackProgramUniform1dvEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLdouble* value)
{
	log() << warning << "ProgramUniform1dvEXT not available." << std::endl;
}

static void fallbackProgramUniform1fEXT(api::GLuint program, api::GLint location, api::GLfloat v0)
{
	log() << warning << "ProgramUniform1fEXT not available." << std::endl;
}

static void fallbackProgramUniform1fvEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLfloat* value)
{
	log() << warning << "ProgramUniform1fvEXT not available." << std::endl;
}

static void fallbackProgramUniform1iEXT(api::GLuint program, api::GLint location, api::GLint v0)
{
	log() << warning << "ProgramUniform1iEXT not available." << std::endl;
}

static void fallbackProgramUniform1ivEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLint* value)
{
	log() << warning << "ProgramUniform1ivEXT not available." << std::endl;
}

static void fallbackProgramUniform1uiEXT(api::GLuint program, api::GLint location, api::GLuint v0)
{
	log() << warning << "ProgramUniform1uiEXT not available." << std::endl;
}

static void fallbackProgramUniform1uivEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLuint* value)
{
	log() << warning << "ProgramUniform1uivEXT not available." << std::endl;
}

static void fallbackProgramUniform2dEXT(api::GLuint program, api::GLint location, api::GLdouble x, api::GLdouble y)
{
	log() << warning << "ProgramUniform2dEXT not available." << std::endl;
}

static void fallbackProgramUniform2dvEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLdouble* value)
{
	log() << warning << "ProgramUniform2dvEXT not available." << std::endl;
}

static void fallbackProgramUniform2fEXT(api::GLuint program, api::GLint location, api::GLfloat v0, api::GLfloat v1)
{
	log() << warning << "ProgramUniform2fEXT not available." << std::endl;
}

static void fallbackProgramUniform2fvEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLfloat* value)
{
	log() << warning << "ProgramUniform2fvEXT not available." << std::endl;
}

static void fallbackProgramUniform2iEXT(api::GLuint program, api::GLint location, api::GLint v0, api::GLint v1)
{
	log() << warning << "ProgramUniform2iEXT not available." << std::endl;
}

static void fallbackProgramUniform2ivEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLint* value)
{
	log() << warning << "ProgramUniform2ivEXT not available." << std::endl;
}

static void fallbackProgramUniform2uiEXT(api::GLuint program, api::GLint location, api::GLuint v0, api::GLuint v1)
{
	log() << warning << "ProgramUniform2uiEXT not available." << std::endl;
}

static void fallbackProgramUniform2uivEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLuint* value)
{
	log() << warning << "ProgramUniform2uivEXT not available." << std::endl;
}

static void fallbackProgramUniform3dEXT(api::GLuint program, api::GLint location, api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "ProgramUniform3dEXT not available." << std::endl;
}

static void fallbackProgramUniform3dvEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLdouble* value)
{
	log() << warning << "ProgramUniform3dvEXT not available." << std::endl;
}

static void fallbackProgramUniform3fEXT(api::GLuint program, api::GLint location, api::GLfloat v0, api::GLfloat v1, api::GLfloat v2)
{
	log() << warning << "ProgramUniform3fEXT not available." << std::endl;
}

static void fallbackProgramUniform3fvEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLfloat* value)
{
	log() << warning << "ProgramUniform3fvEXT not available." << std::endl;
}

static void fallbackProgramUniform3iEXT(api::GLuint program, api::GLint location, api::GLint v0, api::GLint v1, api::GLint v2)
{
	log() << warning << "ProgramUniform3iEXT not available." << std::endl;
}

static void fallbackProgramUniform3ivEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLint* value)
{
	log() << warning << "ProgramUniform3ivEXT not available." << std::endl;
}

static void fallbackProgramUniform3uiEXT(api::GLuint program, api::GLint location, api::GLuint v0, api::GLuint v1, api::GLuint v2)
{
	log() << warning << "ProgramUniform3uiEXT not available." << std::endl;
}

static void fallbackProgramUniform3uivEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLuint* value)
{
	log() << warning << "ProgramUniform3uivEXT not available." << std::endl;
}

static void fallbackProgramUniform4dEXT(api::GLuint program, api::GLint location, api::GLdouble x, api::GLdouble y, api::GLdouble z, api::GLdouble w)
{
	log() << warning << "ProgramUniform4dEXT not available." << std::endl;
}

static void fallbackProgramUniform4dvEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLdouble* value)
{
	log() << warning << "ProgramUniform4dvEXT not available." << std::endl;
}

static void fallbackProgramUniform4fEXT(api::GLuint program, api::GLint location, api::GLfloat v0, api::GLfloat v1, api::GLfloat v2, api::GLfloat v3)
{
	log() << warning << "ProgramUniform4fEXT not available." << std::endl;
}

static void fallbackProgramUniform4fvEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLfloat* value)
{
	log() << warning << "ProgramUniform4fvEXT not available." << std::endl;
}

static void fallbackProgramUniform4iEXT(api::GLuint program, api::GLint location, api::GLint v0, api::GLint v1, api::GLint v2, api::GLint v3)
{
	log() << warning << "ProgramUniform4iEXT not available." << std::endl;
}

static void fallbackProgramUniform4ivEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLint* value)
{
	log() << warning << "ProgramUniform4ivEXT not available." << std::endl;
}

static void fallbackProgramUniform4uiEXT(api::GLuint program, api::GLint location, api::GLuint v0, api::GLuint v1, api::GLuint v2, api::GLuint v3)
{
	log() << warning << "ProgramUniform4uiEXT not available." << std::endl;
}

static void fallbackProgramUniform4uivEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLuint* value)
{
	log() << warning << "ProgramUniform4uivEXT not available." << std::endl;
}

static void fallbackProgramUniformMatrix2dvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "ProgramUniformMatrix2dvEXT not available." << std::endl;
}

static void fallbackProgramUniformMatrix2fvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "ProgramUniformMatrix2fvEXT not available." << std::endl;
}

static void fallbackProgramUniformMatrix2x3dvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "ProgramUniformMatrix2x3dvEXT not available." << std::endl;
}

static void fallbackProgramUniformMatrix2x3fvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "ProgramUniformMatrix2x3fvEXT not available." << std::endl;
}

static void fallbackProgramUniformMatrix2x4dvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "ProgramUniformMatrix2x4dvEXT not available." << std::endl;
}

static void fallbackProgramUniformMatrix2x4fvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "ProgramUniformMatrix2x4fvEXT not available." << std::endl;
}

static void fallbackProgramUniformMatrix3dvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "ProgramUniformMatrix3dvEXT not available." << std::endl;
}

static void fallbackProgramUniformMatrix3fvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "ProgramUniformMatrix3fvEXT not available." << std::endl;
}

static void fallbackProgramUniformMatrix3x2dvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "ProgramUniformMatrix3x2dvEXT not available." << std::endl;
}

static void fallbackProgramUniformMatrix3x2fvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "ProgramUniformMatrix3x2fvEXT not available." << std::endl;
}

static void fallbackProgramUniformMatrix3x4dvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "ProgramUniformMatrix3x4dvEXT not available." << std::endl;
}

static void fallbackProgramUniformMatrix3x4fvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "ProgramUniformMatrix3x4fvEXT not available." << std::endl;
}

static void fallbackProgramUniformMatrix4dvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "ProgramUniformMatrix4dvEXT not available." << std::endl;
}

static void fallbackProgramUniformMatrix4fvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "ProgramUniformMatrix4fvEXT not available." << std::endl;
}

static void fallbackProgramUniformMatrix4x2dvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "ProgramUniformMatrix4x2dvEXT not available." << std::endl;
}

static void fallbackProgramUniformMatrix4x2fvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "ProgramUniformMatrix4x2fvEXT not available." << std::endl;
}

static void fallbackProgramUniformMatrix4x3dvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "ProgramUniformMatrix4x3dvEXT not available." << std::endl;
}

static void fallbackProgramUniformMatrix4x3fvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "ProgramUniformMatrix4x3fvEXT not available." << std::endl;
}

static void fallbackProgramUniformui64NV(api::GLuint program, api::GLint location, api::GLuint64EXT value)
{
	log() << warning << "ProgramUniformui64NV not available." << std::endl;
}

static void fallbackProgramUniformui64vNV(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLuint64EXT* value)
{
	log() << warning << "ProgramUniformui64vNV not available." << std::endl;
}

static void fallbackProgramVertexLimitNV(api::GLenum target, api::GLint limit)
{
	log() << warning << "ProgramVertexLimitNV not available." << std::endl;
}

static void fallbackProvokingVertex(api::GLenum mode)
{
	log() << warning << "ProvokingVertex not available." << std::endl;
}

static void fallbackProvokingVertexEXT(api::GLenum mode)
{
	log() << warning << "ProvokingVertexEXT not available." << std::endl;
}

static void fallbackPushAttrib(api::GLbitfield mask)
{
	log() << warning << "PushAttrib not available." << std::endl;
}

static void fallbackPushClientAttrib(api::GLbitfield mask)
{
	log() << warning << "PushClientAttrib not available." << std::endl;
}

static void fallbackPushClientAttribDefaultEXT(api::GLbitfield mask)
{
	log() << warning << "PushClientAttribDefaultEXT not available." << std::endl;
}

static void fallbackPushMatrix()
{
	log() << warning << "PushMatrix not available." << std::endl;
}

static void fallbackPushName(api::GLuint name)
{
	log() << warning << "PushName not available." << std::endl;
}

static void fallbackQueryCounter(api::GLuint id, api::GLenum target)
{
	log() << warning << "QueryCounter not available." << std::endl;
}

static void fallbackRasterPos2d(api::GLdouble x, api::GLdouble y)
{
	log() << warning << "RasterPos2d not available." << std::endl;
}

static void fallbackRasterPos2dv(const api::GLdouble* v)
{
	log() << warning << "RasterPos2dv not available." << std::endl;
}

static void fallbackRasterPos2f(api::GLfloat x, api::GLfloat y)
{
	log() << warning << "RasterPos2f not available." << std::endl;
}

static void fallbackRasterPos2fv(const api::GLfloat* v)
{
	log() << warning << "RasterPos2fv not available." << std::endl;
}

static void fallbackRasterPos2i(api::GLint x, api::GLint y)
{
	log() << warning << "RasterPos2i not available." << std::endl;
}

static void fallbackRasterPos2iv(const api::GLint* v)
{
	log() << warning << "RasterPos2iv not available." << std::endl;
}

static void fallbackRasterPos2s(api::GLshort x, api::GLshort y)
{
	log() << warning << "RasterPos2s not available." << std::endl;
}

static void fallbackRasterPos2sv(const api::GLshort* v)
{
	log() << warning << "RasterPos2sv not available." << std::endl;
}

static void fallbackRasterPos3d(api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "RasterPos3d not available." << std::endl;
}

static void fallbackRasterPos3dv(const api::GLdouble* v)
{
	log() << warning << "RasterPos3dv not available." << std::endl;
}

static void fallbackRasterPos3f(api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "RasterPos3f not available." << std::endl;
}

static void fallbackRasterPos3fv(const api::GLfloat* v)
{
	log() << warning << "RasterPos3fv not available." << std::endl;
}

static void fallbackRasterPos3i(api::GLint x, api::GLint y, api::GLint z)
{
	log() << warning << "RasterPos3i not available." << std::endl;
}

static void fallbackRasterPos3iv(const api::GLint* v)
{
	log() << warning << "RasterPos3iv not available." << std::endl;
}

static void fallbackRasterPos3s(api::GLshort x, api::GLshort y, api::GLshort z)
{
	log() << warning << "RasterPos3s not available." << std::endl;
}

static void fallbackRasterPos3sv(const api::GLshort* v)
{
	log() << warning << "RasterPos3sv not available." << std::endl;
}

static void fallbackRasterPos4d(api::GLdouble x, api::GLdouble y, api::GLdouble z, api::GLdouble w)
{
	log() << warning << "RasterPos4d not available." << std::endl;
}

static void fallbackRasterPos4dv(const api::GLdouble* v)
{
	log() << warning << "RasterPos4dv not available." << std::endl;
}

static void fallbackRasterPos4f(api::GLfloat x, api::GLfloat y, api::GLfloat z, api::GLfloat w)
{
	log() << warning << "RasterPos4f not available." << std::endl;
}

static void fallbackRasterPos4fv(const api::GLfloat* v)
{
	log() << warning << "RasterPos4fv not available." << std::endl;
}

static void fallbackRasterPos4i(api::GLint x, api::GLint y, api::GLint z, api::GLint w)
{
	log() << warning << "RasterPos4i not available." << std::endl;
}

static void fallbackRasterPos4iv(const api::GLint* v)
{
	log() << warning << "RasterPos4iv not available." << std::endl;
}

static void fallbackRasterPos4s(api::GLshort x, api::GLshort y, api::GLshort z, api::GLshort w)
{
	log() << warning << "RasterPos4s not available." << std::endl;
}

static void fallbackRasterPos4sv(const api::GLshort* v)
{
	log() << warning << "RasterPos4sv not available." << std::endl;
}

static void fallbackReadBuffer(api::GLenum mode)
{
	log() << warning << "ReadBuffer not available." << std::endl;
}

static void fallbackReadInstrumentsSGIX(api::GLint marker)
{
	log() << warning << "ReadInstrumentsSGIX not available." << std::endl;
}

static void fallbackReadPixels(api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height, api::GLenum format, api::GLenum type, api::GLvoid* pixels)
{
	log() << warning << "ReadPixels not available." << std::endl;
}

static void fallbackRectd(api::GLdouble x1, api::GLdouble y1, api::GLdouble x2, api::GLdouble y2)
{
	log() << warning << "Rectd not available." << std::endl;
}

static void fallbackRectdv(const api::GLdouble* v1, const api::GLdouble* v2)
{
	log() << warning << "Rectdv not available." << std::endl;
}

static void fallbackRectf(api::GLfloat x1, api::GLfloat y1, api::GLfloat x2, api::GLfloat y2)
{
	log() << warning << "Rectf not available." << std::endl;
}

static void fallbackRectfv(const api::GLfloat* v1, const api::GLfloat* v2)
{
	log() << warning << "Rectfv not available." << std::endl;
}

static void fallbackRecti(api::GLint x1, api::GLint y1, api::GLint x2, api::GLint y2)
{
	log() << warning << "Recti not available." << std::endl;
}

static void fallbackRectiv(const api::GLint* v1, const api::GLint* v2)
{
	log() << warning << "Rectiv not available." << std::endl;
}

static void fallbackRects(api::GLshort x1, api::GLshort y1, api::GLshort x2, api::GLshort y2)
{
	log() << warning << "Rects not available." << std::endl;
}

static void fallbackRectsv(const api::GLshort* v1, const api::GLshort* v2)
{
	log() << warning << "Rectsv not available." << std::endl;
}

static void fallbackReferencePlaneSGIX(const api::GLdouble* equation)
{
	log() << warning << "ReferencePlaneSGIX not available." << std::endl;
}

static api::GLint fallbackRenderMode(api::GLenum mode)
{
	log() << warning << "RenderMode not available." << std::endl;
}

static void fallbackRenderbufferStorage(api::GLenum target, api::GLenum internalformat, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "RenderbufferStorage not available." << std::endl;
}

static void fallbackRenderbufferStorageEXT(api::GLenum target, api::GLenum internalformat, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "RenderbufferStorageEXT not available." << std::endl;
}

static void fallbackRenderbufferStorageMultisample(api::GLenum target, api::GLsizei samples, api::GLenum internalformat, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "RenderbufferStorageMultisample not available." << std::endl;
}

static void fallbackRenderbufferStorageMultisampleCoverageNV(api::GLenum target, api::GLsizei coverageSamples, api::GLsizei colorSamples, api::GLenum internalformat, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "RenderbufferStorageMultisampleCoverageNV not available." << std::endl;
}

static void fallbackRenderbufferStorageMultisampleEXT(api::GLenum target, api::GLsizei samples, api::GLenum internalformat, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "RenderbufferStorageMultisampleEXT not available." << std::endl;
}

static void fallbackReplacementCodePointerSUN(api::GLenum type, api::GLsizei stride, const api::GLvoid** pointer)
{
	log() << warning << "ReplacementCodePointerSUN not available." << std::endl;
}

static void fallbackReplacementCodeubSUN(api::GLubyte code)
{
	log() << warning << "ReplacementCodeubSUN not available." << std::endl;
}

static void fallbackReplacementCodeubvSUN(const api::GLubyte* code)
{
	log() << warning << "ReplacementCodeubvSUN not available." << std::endl;
}

static void fallbackReplacementCodeuiColor3fVertex3fSUN(api::GLuint rc, api::GLfloat r, api::GLfloat g, api::GLfloat b, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "ReplacementCodeuiColor3fVertex3fSUN not available." << std::endl;
}

static void fallbackReplacementCodeuiColor3fVertex3fvSUN(const api::GLuint* rc, const api::GLfloat* c, const api::GLfloat* v)
{
	log() << warning << "ReplacementCodeuiColor3fVertex3fvSUN not available." << std::endl;
}

static void fallbackReplacementCodeuiColor4fNormal3fVertex3fSUN(api::GLuint rc, api::GLfloat r, api::GLfloat g, api::GLfloat b, api::GLfloat a, api::GLfloat nx, api::GLfloat ny, api::GLfloat nz, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "ReplacementCodeuiColor4fNormal3fVertex3fSUN not available." << std::endl;
}

static void fallbackReplacementCodeuiColor4fNormal3fVertex3fvSUN(const api::GLuint* rc, const api::GLfloat* c, const api::GLfloat* n, const api::GLfloat* v)
{
	log() << warning << "ReplacementCodeuiColor4fNormal3fVertex3fvSUN not available." << std::endl;
}

static void fallbackReplacementCodeuiColor4ubVertex3fSUN(api::GLuint rc, api::GLubyte r, api::GLubyte g, api::GLubyte b, api::GLubyte a, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "ReplacementCodeuiColor4ubVertex3fSUN not available." << std::endl;
}

static void fallbackReplacementCodeuiColor4ubVertex3fvSUN(const api::GLuint* rc, const api::GLubyte* c, const api::GLfloat* v)
{
	log() << warning << "ReplacementCodeuiColor4ubVertex3fvSUN not available." << std::endl;
}

static void fallbackReplacementCodeuiNormal3fVertex3fSUN(api::GLuint rc, api::GLfloat nx, api::GLfloat ny, api::GLfloat nz, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "ReplacementCodeuiNormal3fVertex3fSUN not available." << std::endl;
}

static void fallbackReplacementCodeuiNormal3fVertex3fvSUN(const api::GLuint* rc, const api::GLfloat* n, const api::GLfloat* v)
{
	log() << warning << "ReplacementCodeuiNormal3fVertex3fvSUN not available." << std::endl;
}

static void fallbackReplacementCodeuiSUN(api::GLuint code)
{
	log() << warning << "ReplacementCodeuiSUN not available." << std::endl;
}

static void fallbackReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN(api::GLuint rc, api::GLfloat s, api::GLfloat t, api::GLfloat r, api::GLfloat g, api::GLfloat b, api::GLfloat a, api::GLfloat nx, api::GLfloat ny, api::GLfloat nz, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "ReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN not available." << std::endl;
}

static void fallbackReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN(const api::GLuint* rc, const api::GLfloat* tc, const api::GLfloat* c, const api::GLfloat* n, const api::GLfloat* v)
{
	log() << warning << "ReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN not available." << std::endl;
}

static void fallbackReplacementCodeuiTexCoord2fNormal3fVertex3fSUN(api::GLuint rc, api::GLfloat s, api::GLfloat t, api::GLfloat nx, api::GLfloat ny, api::GLfloat nz, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "ReplacementCodeuiTexCoord2fNormal3fVertex3fSUN not available." << std::endl;
}

static void fallbackReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN(const api::GLuint* rc, const api::GLfloat* tc, const api::GLfloat* n, const api::GLfloat* v)
{
	log() << warning << "ReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN not available." << std::endl;
}

static void fallbackReplacementCodeuiTexCoord2fVertex3fSUN(api::GLuint rc, api::GLfloat s, api::GLfloat t, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "ReplacementCodeuiTexCoord2fVertex3fSUN not available." << std::endl;
}

static void fallbackReplacementCodeuiTexCoord2fVertex3fvSUN(const api::GLuint* rc, const api::GLfloat* tc, const api::GLfloat* v)
{
	log() << warning << "ReplacementCodeuiTexCoord2fVertex3fvSUN not available." << std::endl;
}

static void fallbackReplacementCodeuiVertex3fSUN(api::GLuint rc, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "ReplacementCodeuiVertex3fSUN not available." << std::endl;
}

static void fallbackReplacementCodeuiVertex3fvSUN(const api::GLuint* rc, const api::GLfloat* v)
{
	log() << warning << "ReplacementCodeuiVertex3fvSUN not available." << std::endl;
}

static void fallbackReplacementCodeuivSUN(const api::GLuint* code)
{
	log() << warning << "ReplacementCodeuivSUN not available." << std::endl;
}

static void fallbackReplacementCodeusSUN(api::GLushort code)
{
	log() << warning << "ReplacementCodeusSUN not available." << std::endl;
}

static void fallbackReplacementCodeusvSUN(const api::GLushort* code)
{
	log() << warning << "ReplacementCodeusvSUN not available." << std::endl;
}

static void fallbackRequestResidentProgramsNV(api::GLsizei n, const api::GLuint* programs)
{
	log() << warning << "RequestResidentProgramsNV not available." << std::endl;
}

static void fallbackResetHistogram(api::GLenum target)
{
	log() << warning << "ResetHistogram not available." << std::endl;
}

static void fallbackResetHistogramEXT(api::GLenum target)
{
	log() << warning << "ResetHistogramEXT not available." << std::endl;
}

static void fallbackResetMinmax(api::GLenum target)
{
	log() << warning << "ResetMinmax not available." << std::endl;
}

static void fallbackResetMinmaxEXT(api::GLenum target)
{
	log() << warning << "ResetMinmaxEXT not available." << std::endl;
}

static void fallbackResizeBuffersMESA()
{
	log() << warning << "ResizeBuffersMESA not available." << std::endl;
}

static void fallbackResumeTransformFeedback()
{
	log() << warning << "ResumeTransformFeedback not available." << std::endl;
}

static void fallbackResumeTransformFeedbackNV()
{
	log() << warning << "ResumeTransformFeedbackNV not available." << std::endl;
}

static void fallbackRotated(api::GLdouble angle, api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "Rotated not available." << std::endl;
}

static void fallbackRotatef(api::GLfloat angle, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "Rotatef not available." << std::endl;
}

static void fallbackSampleCoverage(api::GLclampf value, api::GLboolean invert)
{
	log() << warning << "SampleCoverage not available." << std::endl;
}

static void fallbackSampleCoverageARB(api::GLclampf value, api::GLboolean invert)
{
	log() << warning << "SampleCoverageARB not available." << std::endl;
}

static void fallbackSampleMapATI(api::GLuint dst, api::GLuint interp, api::GLenum swizzle)
{
	log() << warning << "SampleMapATI not available." << std::endl;
}

static void fallbackSampleMaskEXT(api::GLclampf value, api::GLboolean invert)
{
	log() << warning << "SampleMaskEXT not available." << std::endl;
}

static void fallbackSampleMaskIndexedNV(api::GLuint index, api::GLbitfield mask)
{
	log() << warning << "SampleMaskIndexedNV not available." << std::endl;
}

static void fallbackSampleMaskSGIS(api::GLclampf value, api::GLboolean invert)
{
	log() << warning << "SampleMaskSGIS not available." << std::endl;
}

static void fallbackSampleMaski(api::GLuint index, api::GLbitfield mask)
{
	log() << warning << "SampleMaski not available." << std::endl;
}

static void fallbackSamplePatternEXT(api::GLenum pattern)
{
	log() << warning << "SamplePatternEXT not available." << std::endl;
}

static void fallbackSamplePatternSGIS(api::GLenum pattern)
{
	log() << warning << "SamplePatternSGIS not available." << std::endl;
}

static void fallbackSamplerParameterIiv(api::GLuint sampler, api::GLenum pname, const api::GLint* param)
{
	log() << warning << "SamplerParameterIiv not available." << std::endl;
}

static void fallbackSamplerParameterIuiv(api::GLuint sampler, api::GLenum pname, const api::GLuint* param)
{
	log() << warning << "SamplerParameterIuiv not available." << std::endl;
}

static void fallbackSamplerParameterf(api::GLuint sampler, api::GLenum pname, api::GLfloat param)
{
	log() << warning << "SamplerParameterf not available." << std::endl;
}

static void fallbackSamplerParameterfv(api::GLuint sampler, api::GLenum pname, const api::GLfloat* param)
{
	log() << warning << "SamplerParameterfv not available." << std::endl;
}

static void fallbackSamplerParameteri(api::GLuint sampler, api::GLenum pname, api::GLint param)
{
	log() << warning << "SamplerParameteri not available." << std::endl;
}

static void fallbackSamplerParameteriv(api::GLuint sampler, api::GLenum pname, const api::GLint* param)
{
	log() << warning << "SamplerParameteriv not available." << std::endl;
}

static void fallbackScaled(api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "Scaled not available." << std::endl;
}

static void fallbackScalef(api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "Scalef not available." << std::endl;
}

static void fallbackScissor(api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "Scissor not available." << std::endl;
}

static void fallbackSecondaryColor3b(api::GLbyte red, api::GLbyte green, api::GLbyte blue)
{
	log() << warning << "SecondaryColor3b not available." << std::endl;
}

static void fallbackSecondaryColor3bEXT(api::GLbyte red, api::GLbyte green, api::GLbyte blue)
{
	log() << warning << "SecondaryColor3bEXT not available." << std::endl;
}

static void fallbackSecondaryColor3bv(const api::GLbyte* v)
{
	log() << warning << "SecondaryColor3bv not available." << std::endl;
}

static void fallbackSecondaryColor3bvEXT(const api::GLbyte* v)
{
	log() << warning << "SecondaryColor3bvEXT not available." << std::endl;
}

static void fallbackSecondaryColor3d(api::GLdouble red, api::GLdouble green, api::GLdouble blue)
{
	log() << warning << "SecondaryColor3d not available." << std::endl;
}

static void fallbackSecondaryColor3dEXT(api::GLdouble red, api::GLdouble green, api::GLdouble blue)
{
	log() << warning << "SecondaryColor3dEXT not available." << std::endl;
}

static void fallbackSecondaryColor3dv(const api::GLdouble* v)
{
	log() << warning << "SecondaryColor3dv not available." << std::endl;
}

static void fallbackSecondaryColor3dvEXT(const api::GLdouble* v)
{
	log() << warning << "SecondaryColor3dvEXT not available." << std::endl;
}

static void fallbackSecondaryColor3f(api::GLfloat red, api::GLfloat green, api::GLfloat blue)
{
	log() << warning << "SecondaryColor3f not available." << std::endl;
}

static void fallbackSecondaryColor3fEXT(api::GLfloat red, api::GLfloat green, api::GLfloat blue)
{
	log() << warning << "SecondaryColor3fEXT not available." << std::endl;
}

static void fallbackSecondaryColor3fv(const api::GLfloat* v)
{
	log() << warning << "SecondaryColor3fv not available." << std::endl;
}

static void fallbackSecondaryColor3fvEXT(const api::GLfloat* v)
{
	log() << warning << "SecondaryColor3fvEXT not available." << std::endl;
}

static void fallbackSecondaryColor3hNV(api::GLhalfNV red, api::GLhalfNV green, api::GLhalfNV blue)
{
	log() << warning << "SecondaryColor3hNV not available." << std::endl;
}

static void fallbackSecondaryColor3hvNV(const api::GLhalfNV* v)
{
	log() << warning << "SecondaryColor3hvNV not available." << std::endl;
}

static void fallbackSecondaryColor3i(api::GLint red, api::GLint green, api::GLint blue)
{
	log() << warning << "SecondaryColor3i not available." << std::endl;
}

static void fallbackSecondaryColor3iEXT(api::GLint red, api::GLint green, api::GLint blue)
{
	log() << warning << "SecondaryColor3iEXT not available." << std::endl;
}

static void fallbackSecondaryColor3iv(const api::GLint* v)
{
	log() << warning << "SecondaryColor3iv not available." << std::endl;
}

static void fallbackSecondaryColor3ivEXT(const api::GLint* v)
{
	log() << warning << "SecondaryColor3ivEXT not available." << std::endl;
}

static void fallbackSecondaryColor3s(api::GLshort red, api::GLshort green, api::GLshort blue)
{
	log() << warning << "SecondaryColor3s not available." << std::endl;
}

static void fallbackSecondaryColor3sEXT(api::GLshort red, api::GLshort green, api::GLshort blue)
{
	log() << warning << "SecondaryColor3sEXT not available." << std::endl;
}

static void fallbackSecondaryColor3sv(const api::GLshort* v)
{
	log() << warning << "SecondaryColor3sv not available." << std::endl;
}

static void fallbackSecondaryColor3svEXT(const api::GLshort* v)
{
	log() << warning << "SecondaryColor3svEXT not available." << std::endl;
}

static void fallbackSecondaryColor3ub(api::GLubyte red, api::GLubyte green, api::GLubyte blue)
{
	log() << warning << "SecondaryColor3ub not available." << std::endl;
}

static void fallbackSecondaryColor3ubEXT(api::GLubyte red, api::GLubyte green, api::GLubyte blue)
{
	log() << warning << "SecondaryColor3ubEXT not available." << std::endl;
}

static void fallbackSecondaryColor3ubv(const api::GLubyte* v)
{
	log() << warning << "SecondaryColor3ubv not available." << std::endl;
}

static void fallbackSecondaryColor3ubvEXT(const api::GLubyte* v)
{
	log() << warning << "SecondaryColor3ubvEXT not available." << std::endl;
}

static void fallbackSecondaryColor3ui(api::GLuint red, api::GLuint green, api::GLuint blue)
{
	log() << warning << "SecondaryColor3ui not available." << std::endl;
}

static void fallbackSecondaryColor3uiEXT(api::GLuint red, api::GLuint green, api::GLuint blue)
{
	log() << warning << "SecondaryColor3uiEXT not available." << std::endl;
}

static void fallbackSecondaryColor3uiv(const api::GLuint* v)
{
	log() << warning << "SecondaryColor3uiv not available." << std::endl;
}

static void fallbackSecondaryColor3uivEXT(const api::GLuint* v)
{
	log() << warning << "SecondaryColor3uivEXT not available." << std::endl;
}

static void fallbackSecondaryColor3us(api::GLushort red, api::GLushort green, api::GLushort blue)
{
	log() << warning << "SecondaryColor3us not available." << std::endl;
}

static void fallbackSecondaryColor3usEXT(api::GLushort red, api::GLushort green, api::GLushort blue)
{
	log() << warning << "SecondaryColor3usEXT not available." << std::endl;
}

static void fallbackSecondaryColor3usv(const api::GLushort* v)
{
	log() << warning << "SecondaryColor3usv not available." << std::endl;
}

static void fallbackSecondaryColor3usvEXT(const api::GLushort* v)
{
	log() << warning << "SecondaryColor3usvEXT not available." << std::endl;
}

static void fallbackSecondaryColorFormatNV(api::GLint size, api::GLenum type, api::GLsizei stride)
{
	log() << warning << "SecondaryColorFormatNV not available." << std::endl;
}

static void fallbackSecondaryColorP3ui(api::GLenum type, api::GLuint color)
{
	log() << warning << "SecondaryColorP3ui not available." << std::endl;
}

static void fallbackSecondaryColorP3uiv(api::GLenum type, const api::GLuint* color)
{
	log() << warning << "SecondaryColorP3uiv not available." << std::endl;
}

static void fallbackSecondaryColorPointer(api::GLint size, api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "SecondaryColorPointer not available." << std::endl;
}

static void fallbackSecondaryColorPointerEXT(api::GLint size, api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "SecondaryColorPointerEXT not available." << std::endl;
}

static void fallbackSecondaryColorPointerListIBM(api::GLint size, api::GLenum type, api::GLint stride, const api::GLvoid** pointer, api::GLint ptrstride)
{
	log() << warning << "SecondaryColorPointerListIBM not available." << std::endl;
}

static void fallbackSelectBuffer(api::GLsizei size, api::GLuint* buffer)
{
	log() << warning << "SelectBuffer not available." << std::endl;
}

static void fallbackSelectPerfMonitorCountersAMD(api::GLuint monitor, api::GLboolean enable, api::GLuint group, api::GLint numCounters, api::GLuint* counterList)
{
	log() << warning << "SelectPerfMonitorCountersAMD not available." << std::endl;
}

static void fallbackSeparableFilter2D(api::GLenum target, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLenum format, api::GLenum type, const api::GLvoid* row, const api::GLvoid* column)
{
	log() << warning << "SeparableFilter2D not available." << std::endl;
}

static void fallbackSeparableFilter2DEXT(api::GLenum target, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLenum format, api::GLenum type, const api::GLvoid* row, const api::GLvoid* column)
{
	log() << warning << "SeparableFilter2DEXT not available." << std::endl;
}

static void fallbackSetFenceAPPLE(api::GLuint fence)
{
	log() << warning << "SetFenceAPPLE not available." << std::endl;
}

static void fallbackSetFenceNV(api::GLuint fence, api::GLenum condition)
{
	log() << warning << "SetFenceNV not available." << std::endl;
}

static void fallbackSetFragmentShaderConstantATI(api::GLuint dst, const api::GLfloat* value)
{
	log() << warning << "SetFragmentShaderConstantATI not available." << std::endl;
}

static void fallbackSetInvariantEXT(api::GLuint id, api::GLenum type, const api::GLvoid* addr)
{
	log() << warning << "SetInvariantEXT not available." << std::endl;
}

static void fallbackSetLocalConstantEXT(api::GLuint id, api::GLenum type, const api::GLvoid* addr)
{
	log() << warning << "SetLocalConstantEXT not available." << std::endl;
}

static void fallbackShadeModel(api::GLenum mode)
{
	log() << warning << "ShadeModel not available." << std::endl;
}

static void fallbackShaderOp1EXT(api::GLenum op, api::GLuint res, api::GLuint arg1)
{
	log() << warning << "ShaderOp1EXT not available." << std::endl;
}

static void fallbackShaderOp2EXT(api::GLenum op, api::GLuint res, api::GLuint arg1, api::GLuint arg2)
{
	log() << warning << "ShaderOp2EXT not available." << std::endl;
}

static void fallbackShaderOp3EXT(api::GLenum op, api::GLuint res, api::GLuint arg1, api::GLuint arg2, api::GLuint arg3)
{
	log() << warning << "ShaderOp3EXT not available." << std::endl;
}

static void fallbackShaderSource(api::GLuint shader, api::GLsizei count, const api::GLchar** string, const api::GLint* length)
{
	log() << warning << "ShaderSource not available." << std::endl;
}

static void fallbackShaderSourceARB(api::GLhandleARB shaderObj, api::GLsizei count, const api::GLcharARB** string, const api::GLint* length)
{
	log() << warning << "ShaderSourceARB not available." << std::endl;
}

static void fallbackSharpenTexFuncSGIS(api::GLenum target, api::GLsizei n, const api::GLfloat* points)
{
	log() << warning << "SharpenTexFuncSGIS not available." << std::endl;
}

static void fallbackSpriteParameterfSGIX(api::GLenum pname, api::GLfloat param)
{
	log() << warning << "SpriteParameterfSGIX not available." << std::endl;
}

static void fallbackSpriteParameterfvSGIX(api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "SpriteParameterfvSGIX not available." << std::endl;
}

static void fallbackSpriteParameteriSGIX(api::GLenum pname, api::GLint param)
{
	log() << warning << "SpriteParameteriSGIX not available." << std::endl;
}

static void fallbackSpriteParameterivSGIX(api::GLenum pname, const api::GLint* params)
{
	log() << warning << "SpriteParameterivSGIX not available." << std::endl;
}

static void fallbackStartInstrumentsSGIX()
{
	log() << warning << "StartInstrumentsSGIX not available." << std::endl;
}

static void fallbackStencilClearTagEXT(api::GLsizei stencilTagBits, api::GLuint stencilClearTag)
{
	log() << warning << "StencilClearTagEXT not available." << std::endl;
}

static void fallbackStencilFunc(api::GLenum func, api::GLint ref, api::GLuint mask)
{
	log() << warning << "StencilFunc not available." << std::endl;
}

static void fallbackStencilFuncSeparate(api::GLenum frontfunc, api::GLenum backfunc, api::GLint ref, api::GLuint mask)
{
	log() << warning << "StencilFuncSeparate not available." << std::endl;
}

static void fallbackStencilFuncSeparateATI(api::GLenum frontfunc, api::GLenum backfunc, api::GLint ref, api::GLuint mask)
{
	log() << warning << "StencilFuncSeparateATI not available." << std::endl;
}

static void fallbackStencilMask(api::GLuint mask)
{
	log() << warning << "StencilMask not available." << std::endl;
}

static void fallbackStencilMaskSeparate(api::GLenum face, api::GLuint mask)
{
	log() << warning << "StencilMaskSeparate not available." << std::endl;
}

static void fallbackStencilOp(api::GLenum fail, api::GLenum zfail, api::GLenum zpass)
{
	log() << warning << "StencilOp not available." << std::endl;
}

static void fallbackStencilOpSeparate(api::GLenum face, api::GLenum sfail, api::GLenum dpfail, api::GLenum dppass)
{
	log() << warning << "StencilOpSeparate not available." << std::endl;
}

static void fallbackStencilOpSeparateATI(api::GLenum face, api::GLenum sfail, api::GLenum dpfail, api::GLenum dppass)
{
	log() << warning << "StencilOpSeparateATI not available." << std::endl;
}

static void fallbackStopInstrumentsSGIX(api::GLint marker)
{
	log() << warning << "StopInstrumentsSGIX not available." << std::endl;
}

static void fallbackStringMarkerGREMEDY(api::GLsizei len, const api::GLvoid* string)
{
	log() << warning << "StringMarkerGREMEDY not available." << std::endl;
}

static void fallbackSwizzleEXT(api::GLuint res, api::GLuint in, api::GLenum outX, api::GLenum outY, api::GLenum outZ, api::GLenum outW)
{
	log() << warning << "SwizzleEXT not available." << std::endl;
}

static void fallbackTagSampleBufferSGIX()
{
	log() << warning << "TagSampleBufferSGIX not available." << std::endl;
}

static void fallbackTangent3bEXT(api::GLbyte tx, api::GLbyte ty, api::GLbyte tz)
{
	log() << warning << "Tangent3bEXT not available." << std::endl;
}

static void fallbackTangent3bvEXT(const api::GLbyte* v)
{
	log() << warning << "Tangent3bvEXT not available." << std::endl;
}

static void fallbackTangent3dEXT(api::GLdouble tx, api::GLdouble ty, api::GLdouble tz)
{
	log() << warning << "Tangent3dEXT not available." << std::endl;
}

static void fallbackTangent3dvEXT(const api::GLdouble* v)
{
	log() << warning << "Tangent3dvEXT not available." << std::endl;
}

static void fallbackTangent3fEXT(api::GLfloat tx, api::GLfloat ty, api::GLfloat tz)
{
	log() << warning << "Tangent3fEXT not available." << std::endl;
}

static void fallbackTangent3fvEXT(const api::GLfloat* v)
{
	log() << warning << "Tangent3fvEXT not available." << std::endl;
}

static void fallbackTangent3iEXT(api::GLint tx, api::GLint ty, api::GLint tz)
{
	log() << warning << "Tangent3iEXT not available." << std::endl;
}

static void fallbackTangent3ivEXT(const api::GLint* v)
{
	log() << warning << "Tangent3ivEXT not available." << std::endl;
}

static void fallbackTangent3sEXT(api::GLshort tx, api::GLshort ty, api::GLshort tz)
{
	log() << warning << "Tangent3sEXT not available." << std::endl;
}

static void fallbackTangent3svEXT(const api::GLshort* v)
{
	log() << warning << "Tangent3svEXT not available." << std::endl;
}

static void fallbackTangentPointerEXT(api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "TangentPointerEXT not available." << std::endl;
}

static void fallbackTbufferMask3DFX(api::GLuint mask)
{
	log() << warning << "TbufferMask3DFX not available." << std::endl;
}

static void fallbackTessellationFactorAMD(api::GLfloat factor)
{
	log() << warning << "TessellationFactorAMD not available." << std::endl;
}

static void fallbackTessellationModeAMD(api::GLenum mode)
{
	log() << warning << "TessellationModeAMD not available." << std::endl;
}

static api::GLboolean fallbackTestFenceAPPLE(api::GLuint fence)
{
	log() << warning << "TestFenceAPPLE not available." << std::endl;
}

static api::GLboolean fallbackTestFenceNV(api::GLuint fence)
{
	log() << warning << "TestFenceNV not available." << std::endl;
}

static api::GLboolean fallbackTestObjectAPPLE(api::GLenum object, api::GLuint name)
{
	log() << warning << "TestObjectAPPLE not available." << std::endl;
}

static void fallbackTexBuffer(api::GLenum target, api::GLenum internalformat, api::GLuint buffer)
{
	log() << warning << "TexBuffer not available." << std::endl;
}

static void fallbackTexBufferARB(api::GLenum target, api::GLenum internalformat, api::GLuint buffer)
{
	log() << warning << "TexBufferARB not available." << std::endl;
}

static void fallbackTexBufferEXT(api::GLenum target, api::GLenum internalformat, api::GLuint buffer)
{
	log() << warning << "TexBufferEXT not available." << std::endl;
}

static void fallbackTexBumpParameterfvATI(api::GLenum pname, const api::GLfloat* param)
{
	log() << warning << "TexBumpParameterfvATI not available." << std::endl;
}

static void fallbackTexBumpParameterivATI(api::GLenum pname, const api::GLint* param)
{
	log() << warning << "TexBumpParameterivATI not available." << std::endl;
}

static void fallbackTexCoord1d(api::GLdouble s)
{
	log() << warning << "TexCoord1d not available." << std::endl;
}

static void fallbackTexCoord1dv(const api::GLdouble* v)
{
	log() << warning << "TexCoord1dv not available." << std::endl;
}

static void fallbackTexCoord1f(api::GLfloat s)
{
	log() << warning << "TexCoord1f not available." << std::endl;
}

static void fallbackTexCoord1fv(const api::GLfloat* v)
{
	log() << warning << "TexCoord1fv not available." << std::endl;
}

static void fallbackTexCoord1hNV(api::GLhalfNV s)
{
	log() << warning << "TexCoord1hNV not available." << std::endl;
}

static void fallbackTexCoord1hvNV(const api::GLhalfNV* v)
{
	log() << warning << "TexCoord1hvNV not available." << std::endl;
}

static void fallbackTexCoord1i(api::GLint s)
{
	log() << warning << "TexCoord1i not available." << std::endl;
}

static void fallbackTexCoord1iv(const api::GLint* v)
{
	log() << warning << "TexCoord1iv not available." << std::endl;
}

static void fallbackTexCoord1s(api::GLshort s)
{
	log() << warning << "TexCoord1s not available." << std::endl;
}

static void fallbackTexCoord1sv(const api::GLshort* v)
{
	log() << warning << "TexCoord1sv not available." << std::endl;
}

static void fallbackTexCoord2d(api::GLdouble s, api::GLdouble t)
{
	log() << warning << "TexCoord2d not available." << std::endl;
}

static void fallbackTexCoord2dv(const api::GLdouble* v)
{
	log() << warning << "TexCoord2dv not available." << std::endl;
}

static void fallbackTexCoord2f(api::GLfloat s, api::GLfloat t)
{
	log() << warning << "TexCoord2f not available." << std::endl;
}

static void fallbackTexCoord2fColor3fVertex3fSUN(api::GLfloat s, api::GLfloat t, api::GLfloat r, api::GLfloat g, api::GLfloat b, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "TexCoord2fColor3fVertex3fSUN not available." << std::endl;
}

static void fallbackTexCoord2fColor3fVertex3fvSUN(const api::GLfloat* tc, const api::GLfloat* c, const api::GLfloat* v)
{
	log() << warning << "TexCoord2fColor3fVertex3fvSUN not available." << std::endl;
}

static void fallbackTexCoord2fColor4fNormal3fVertex3fSUN(api::GLfloat s, api::GLfloat t, api::GLfloat r, api::GLfloat g, api::GLfloat b, api::GLfloat a, api::GLfloat nx, api::GLfloat ny, api::GLfloat nz, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "TexCoord2fColor4fNormal3fVertex3fSUN not available." << std::endl;
}

static void fallbackTexCoord2fColor4fNormal3fVertex3fvSUN(const api::GLfloat* tc, const api::GLfloat* c, const api::GLfloat* n, const api::GLfloat* v)
{
	log() << warning << "TexCoord2fColor4fNormal3fVertex3fvSUN not available." << std::endl;
}

static void fallbackTexCoord2fColor4ubVertex3fSUN(api::GLfloat s, api::GLfloat t, api::GLubyte r, api::GLubyte g, api::GLubyte b, api::GLubyte a, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "TexCoord2fColor4ubVertex3fSUN not available." << std::endl;
}

static void fallbackTexCoord2fColor4ubVertex3fvSUN(const api::GLfloat* tc, const api::GLubyte* c, const api::GLfloat* v)
{
	log() << warning << "TexCoord2fColor4ubVertex3fvSUN not available." << std::endl;
}

static void fallbackTexCoord2fNormal3fVertex3fSUN(api::GLfloat s, api::GLfloat t, api::GLfloat nx, api::GLfloat ny, api::GLfloat nz, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "TexCoord2fNormal3fVertex3fSUN not available." << std::endl;
}

static void fallbackTexCoord2fNormal3fVertex3fvSUN(const api::GLfloat* tc, const api::GLfloat* n, const api::GLfloat* v)
{
	log() << warning << "TexCoord2fNormal3fVertex3fvSUN not available." << std::endl;
}

static void fallbackTexCoord2fVertex3fSUN(api::GLfloat s, api::GLfloat t, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "TexCoord2fVertex3fSUN not available." << std::endl;
}

static void fallbackTexCoord2fVertex3fvSUN(const api::GLfloat* tc, const api::GLfloat* v)
{
	log() << warning << "TexCoord2fVertex3fvSUN not available." << std::endl;
}

static void fallbackTexCoord2fv(const api::GLfloat* v)
{
	log() << warning << "TexCoord2fv not available." << std::endl;
}

static void fallbackTexCoord2hNV(api::GLhalfNV s, api::GLhalfNV t)
{
	log() << warning << "TexCoord2hNV not available." << std::endl;
}

static void fallbackTexCoord2hvNV(const api::GLhalfNV* v)
{
	log() << warning << "TexCoord2hvNV not available." << std::endl;
}

static void fallbackTexCoord2i(api::GLint s, api::GLint t)
{
	log() << warning << "TexCoord2i not available." << std::endl;
}

static void fallbackTexCoord2iv(const api::GLint* v)
{
	log() << warning << "TexCoord2iv not available." << std::endl;
}

static void fallbackTexCoord2s(api::GLshort s, api::GLshort t)
{
	log() << warning << "TexCoord2s not available." << std::endl;
}

static void fallbackTexCoord2sv(const api::GLshort* v)
{
	log() << warning << "TexCoord2sv not available." << std::endl;
}

static void fallbackTexCoord3d(api::GLdouble s, api::GLdouble t, api::GLdouble r)
{
	log() << warning << "TexCoord3d not available." << std::endl;
}

static void fallbackTexCoord3dv(const api::GLdouble* v)
{
	log() << warning << "TexCoord3dv not available." << std::endl;
}

static void fallbackTexCoord3f(api::GLfloat s, api::GLfloat t, api::GLfloat r)
{
	log() << warning << "TexCoord3f not available." << std::endl;
}

static void fallbackTexCoord3fv(const api::GLfloat* v)
{
	log() << warning << "TexCoord3fv not available." << std::endl;
}

static void fallbackTexCoord3hNV(api::GLhalfNV s, api::GLhalfNV t, api::GLhalfNV r)
{
	log() << warning << "TexCoord3hNV not available." << std::endl;
}

static void fallbackTexCoord3hvNV(const api::GLhalfNV* v)
{
	log() << warning << "TexCoord3hvNV not available." << std::endl;
}

static void fallbackTexCoord3i(api::GLint s, api::GLint t, api::GLint r)
{
	log() << warning << "TexCoord3i not available." << std::endl;
}

static void fallbackTexCoord3iv(const api::GLint* v)
{
	log() << warning << "TexCoord3iv not available." << std::endl;
}

static void fallbackTexCoord3s(api::GLshort s, api::GLshort t, api::GLshort r)
{
	log() << warning << "TexCoord3s not available." << std::endl;
}

static void fallbackTexCoord3sv(const api::GLshort* v)
{
	log() << warning << "TexCoord3sv not available." << std::endl;
}

static void fallbackTexCoord4d(api::GLdouble s, api::GLdouble t, api::GLdouble r, api::GLdouble q)
{
	log() << warning << "TexCoord4d not available." << std::endl;
}

static void fallbackTexCoord4dv(const api::GLdouble* v)
{
	log() << warning << "TexCoord4dv not available." << std::endl;
}

static void fallbackTexCoord4f(api::GLfloat s, api::GLfloat t, api::GLfloat r, api::GLfloat q)
{
	log() << warning << "TexCoord4f not available." << std::endl;
}

static void fallbackTexCoord4fColor4fNormal3fVertex4fSUN(api::GLfloat s, api::GLfloat t, api::GLfloat p, api::GLfloat q, api::GLfloat r, api::GLfloat g, api::GLfloat b, api::GLfloat a, api::GLfloat nx, api::GLfloat ny, api::GLfloat nz, api::GLfloat x, api::GLfloat y, api::GLfloat z, api::GLfloat w)
{
	log() << warning << "TexCoord4fColor4fNormal3fVertex4fSUN not available." << std::endl;
}

static void fallbackTexCoord4fColor4fNormal3fVertex4fvSUN(const api::GLfloat* tc, const api::GLfloat* c, const api::GLfloat* n, const api::GLfloat* v)
{
	log() << warning << "TexCoord4fColor4fNormal3fVertex4fvSUN not available." << std::endl;
}

static void fallbackTexCoord4fVertex4fSUN(api::GLfloat s, api::GLfloat t, api::GLfloat p, api::GLfloat q, api::GLfloat x, api::GLfloat y, api::GLfloat z, api::GLfloat w)
{
	log() << warning << "TexCoord4fVertex4fSUN not available." << std::endl;
}

static void fallbackTexCoord4fVertex4fvSUN(const api::GLfloat* tc, const api::GLfloat* v)
{
	log() << warning << "TexCoord4fVertex4fvSUN not available." << std::endl;
}

static void fallbackTexCoord4fv(const api::GLfloat* v)
{
	log() << warning << "TexCoord4fv not available." << std::endl;
}

static void fallbackTexCoord4hNV(api::GLhalfNV s, api::GLhalfNV t, api::GLhalfNV r, api::GLhalfNV q)
{
	log() << warning << "TexCoord4hNV not available." << std::endl;
}

static void fallbackTexCoord4hvNV(const api::GLhalfNV* v)
{
	log() << warning << "TexCoord4hvNV not available." << std::endl;
}

static void fallbackTexCoord4i(api::GLint s, api::GLint t, api::GLint r, api::GLint q)
{
	log() << warning << "TexCoord4i not available." << std::endl;
}

static void fallbackTexCoord4iv(const api::GLint* v)
{
	log() << warning << "TexCoord4iv not available." << std::endl;
}

static void fallbackTexCoord4s(api::GLshort s, api::GLshort t, api::GLshort r, api::GLshort q)
{
	log() << warning << "TexCoord4s not available." << std::endl;
}

static void fallbackTexCoord4sv(const api::GLshort* v)
{
	log() << warning << "TexCoord4sv not available." << std::endl;
}

static void fallbackTexCoordFormatNV(api::GLint size, api::GLenum type, api::GLsizei stride)
{
	log() << warning << "TexCoordFormatNV not available." << std::endl;
}

static void fallbackTexCoordP1ui(api::GLenum type, api::GLuint coords)
{
	log() << warning << "TexCoordP1ui not available." << std::endl;
}

static void fallbackTexCoordP1uiv(api::GLenum type, const api::GLuint* coords)
{
	log() << warning << "TexCoordP1uiv not available." << std::endl;
}

static void fallbackTexCoordP2ui(api::GLenum type, api::GLuint coords)
{
	log() << warning << "TexCoordP2ui not available." << std::endl;
}

static void fallbackTexCoordP2uiv(api::GLenum type, const api::GLuint* coords)
{
	log() << warning << "TexCoordP2uiv not available." << std::endl;
}

static void fallbackTexCoordP3ui(api::GLenum type, api::GLuint coords)
{
	log() << warning << "TexCoordP3ui not available." << std::endl;
}

static void fallbackTexCoordP3uiv(api::GLenum type, const api::GLuint* coords)
{
	log() << warning << "TexCoordP3uiv not available." << std::endl;
}

static void fallbackTexCoordP4ui(api::GLenum type, api::GLuint coords)
{
	log() << warning << "TexCoordP4ui not available." << std::endl;
}

static void fallbackTexCoordP4uiv(api::GLenum type, const api::GLuint* coords)
{
	log() << warning << "TexCoordP4uiv not available." << std::endl;
}

static void fallbackTexCoordPointer(api::GLint size, api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "TexCoordPointer not available." << std::endl;
}

static void fallbackTexCoordPointerEXT(api::GLint size, api::GLenum type, api::GLsizei stride, api::GLsizei count, const api::GLvoid* pointer)
{
	log() << warning << "TexCoordPointerEXT not available." << std::endl;
}

static void fallbackTexCoordPointerListIBM(api::GLint size, api::GLenum type, api::GLint stride, const api::GLvoid** pointer, api::GLint ptrstride)
{
	log() << warning << "TexCoordPointerListIBM not available." << std::endl;
}

static void fallbackTexCoordPointervINTEL(api::GLint size, api::GLenum type, const api::GLvoid** pointer)
{
	log() << warning << "TexCoordPointervINTEL not available." << std::endl;
}

static void fallbackTexEnvf(api::GLenum target, api::GLenum pname, api::GLfloat param)
{
	log() << warning << "TexEnvf not available." << std::endl;
}

static void fallbackTexEnvfv(api::GLenum target, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "TexEnvfv not available." << std::endl;
}

static void fallbackTexEnvi(api::GLenum target, api::GLenum pname, api::GLint param)
{
	log() << warning << "TexEnvi not available." << std::endl;
}

static void fallbackTexEnviv(api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "TexEnviv not available." << std::endl;
}

static void fallbackTexFilterFuncSGIS(api::GLenum target, api::GLenum filter, api::GLsizei n, const api::GLfloat* weights)
{
	log() << warning << "TexFilterFuncSGIS not available." << std::endl;
}

static void fallbackTexGend(api::GLenum coord, api::GLenum pname, api::GLdouble param)
{
	log() << warning << "TexGend not available." << std::endl;
}

static void fallbackTexGendv(api::GLenum coord, api::GLenum pname, const api::GLdouble* params)
{
	log() << warning << "TexGendv not available." << std::endl;
}

static void fallbackTexGenf(api::GLenum coord, api::GLenum pname, api::GLfloat param)
{
	log() << warning << "TexGenf not available." << std::endl;
}

static void fallbackTexGenfv(api::GLenum coord, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "TexGenfv not available." << std::endl;
}

static void fallbackTexGeni(api::GLenum coord, api::GLenum pname, api::GLint param)
{
	log() << warning << "TexGeni not available." << std::endl;
}

static void fallbackTexGeniv(api::GLenum coord, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "TexGeniv not available." << std::endl;
}

static void fallbackTexImage1D(api::GLenum target, api::GLint level, api::GLint internalformat, api::GLsizei width, api::GLint border, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "TexImage1D not available." << std::endl;
}

static void fallbackTexImage2D(api::GLenum target, api::GLint level, api::GLint internalformat, api::GLsizei width, api::GLsizei height, api::GLint border, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "TexImage2D not available." << std::endl;
}

static void fallbackTexImage2DMultisample(api::GLenum target, api::GLsizei samples, api::GLint internalformat, api::GLsizei width, api::GLsizei height, api::GLboolean fixedsamplelocations)
{
	log() << warning << "TexImage2DMultisample not available." << std::endl;
}

static void fallbackTexImage3D(api::GLenum target, api::GLint level, api::GLint internalformat, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLint border, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "TexImage3D not available." << std::endl;
}

static void fallbackTexImage3DEXT(api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLint border, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "TexImage3DEXT not available." << std::endl;
}

static void fallbackTexImage3DMultisample(api::GLenum target, api::GLsizei samples, api::GLint internalformat, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLboolean fixedsamplelocations)
{
	log() << warning << "TexImage3DMultisample not available." << std::endl;
}

static void fallbackTexImage4DSGIS(api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLsizei size4d, api::GLint border, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "TexImage4DSGIS not available." << std::endl;
}

static void fallbackTexParameterIiv(api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "TexParameterIiv not available." << std::endl;
}

static void fallbackTexParameterIivEXT(api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "TexParameterIivEXT not available." << std::endl;
}

static void fallbackTexParameterIuiv(api::GLenum target, api::GLenum pname, const api::GLuint* params)
{
	log() << warning << "TexParameterIuiv not available." << std::endl;
}

static void fallbackTexParameterIuivEXT(api::GLenum target, api::GLenum pname, const api::GLuint* params)
{
	log() << warning << "TexParameterIuivEXT not available." << std::endl;
}

static void fallbackTexParameterf(api::GLenum target, api::GLenum pname, api::GLfloat param)
{
	log() << warning << "TexParameterf not available." << std::endl;
}

static void fallbackTexParameterfv(api::GLenum target, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "TexParameterfv not available." << std::endl;
}

static void fallbackTexParameteri(api::GLenum target, api::GLenum pname, api::GLint param)
{
	log() << warning << "TexParameteri not available." << std::endl;
}

static void fallbackTexParameteriv(api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "TexParameteriv not available." << std::endl;
}

static void fallbackTexRenderbufferNV(api::GLenum target, api::GLuint renderbuffer)
{
	log() << warning << "TexRenderbufferNV not available." << std::endl;
}

static void fallbackTexSubImage1D(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLsizei width, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "TexSubImage1D not available." << std::endl;
}

static void fallbackTexSubImage1DEXT(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLsizei width, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "TexSubImage1DEXT not available." << std::endl;
}

static void fallbackTexSubImage2D(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLsizei width, api::GLsizei height, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "TexSubImage2D not available." << std::endl;
}

static void fallbackTexSubImage2DEXT(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLsizei width, api::GLsizei height, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "TexSubImage2DEXT not available." << std::endl;
}

static void fallbackTexSubImage3D(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint zoffset, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "TexSubImage3D not available." << std::endl;
}

static void fallbackTexSubImage3DEXT(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint zoffset, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "TexSubImage3DEXT not available." << std::endl;
}

static void fallbackTexSubImage4DSGIS(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint zoffset, api::GLint woffset, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLsizei size4d, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "TexSubImage4DSGIS not available." << std::endl;
}

static void fallbackTextureBarrierNV()
{
	log() << warning << "TextureBarrierNV not available." << std::endl;
}

static void fallbackTextureBufferEXT(api::GLuint texture, api::GLenum target, api::GLenum internalformat, api::GLuint buffer)
{
	log() << warning << "TextureBufferEXT not available." << std::endl;
}

static void fallbackTextureColorMaskSGIS(api::GLboolean red, api::GLboolean green, api::GLboolean blue, api::GLboolean alpha)
{
	log() << warning << "TextureColorMaskSGIS not available." << std::endl;
}

static void fallbackTextureImage1DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLint border, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "TextureImage1DEXT not available." << std::endl;
}

static void fallbackTextureImage2DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLint border, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "TextureImage2DEXT not available." << std::endl;
}

static void fallbackTextureImage3DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLint border, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "TextureImage3DEXT not available." << std::endl;
}

static void fallbackTextureLightEXT(api::GLenum pname)
{
	log() << warning << "TextureLightEXT not available." << std::endl;
}

static void fallbackTextureMaterialEXT(api::GLenum face, api::GLenum mode)
{
	log() << warning << "TextureMaterialEXT not available." << std::endl;
}

static void fallbackTextureNormalEXT(api::GLenum mode)
{
	log() << warning << "TextureNormalEXT not available." << std::endl;
}

static void fallbackTextureParameterIivEXT(api::GLuint texture, api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "TextureParameterIivEXT not available." << std::endl;
}

static void fallbackTextureParameterIuivEXT(api::GLuint texture, api::GLenum target, api::GLenum pname, const api::GLuint* params)
{
	log() << warning << "TextureParameterIuivEXT not available." << std::endl;
}

static void fallbackTextureParameterfEXT(api::GLuint texture, api::GLenum target, api::GLenum pname, api::GLfloat param)
{
	log() << warning << "TextureParameterfEXT not available." << std::endl;
}

static void fallbackTextureParameterfvEXT(api::GLuint texture, api::GLenum target, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "TextureParameterfvEXT not available." << std::endl;
}

static void fallbackTextureParameteriEXT(api::GLuint texture, api::GLenum target, api::GLenum pname, api::GLint param)
{
	log() << warning << "TextureParameteriEXT not available." << std::endl;
}

static void fallbackTextureParameterivEXT(api::GLuint texture, api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "TextureParameterivEXT not available." << std::endl;
}

static void fallbackTextureRangeAPPLE(api::GLenum target, api::GLsizei length, const api::GLvoid* pointer)
{
	log() << warning << "TextureRangeAPPLE not available." << std::endl;
}

static void fallbackTextureRenderbufferEXT(api::GLuint texture, api::GLenum target, api::GLuint renderbuffer)
{
	log() << warning << "TextureRenderbufferEXT not available." << std::endl;
}

static void fallbackTextureSubImage1DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLsizei width, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "TextureSubImage1DEXT not available." << std::endl;
}

static void fallbackTextureSubImage2DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLsizei width, api::GLsizei height, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "TextureSubImage2DEXT not available." << std::endl;
}

static void fallbackTextureSubImage3DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint zoffset, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "TextureSubImage3DEXT not available." << std::endl;
}

static void fallbackTrackMatrixNV(api::GLenum target, api::GLuint address, api::GLenum matrix, api::GLenum transform)
{
	log() << warning << "TrackMatrixNV not available." << std::endl;
}

static void fallbackTransformFeedbackAttribsNV(api::GLuint count, const api::GLint* attribs, api::GLenum bufferMode)
{
	log() << warning << "TransformFeedbackAttribsNV not available." << std::endl;
}

static void fallbackTransformFeedbackVaryings(api::GLuint program, api::GLsizei count, const api::GLchar** varyings, api::GLenum bufferMode)
{
	log() << warning << "TransformFeedbackVaryings not available." << std::endl;
}

static void fallbackTransformFeedbackVaryingsEXT(api::GLuint program, api::GLsizei count, const api::GLchar** varyings, api::GLenum bufferMode)
{
	log() << warning << "TransformFeedbackVaryingsEXT not available." << std::endl;
}

static void fallbackTransformFeedbackVaryingsNV(api::GLuint program, api::GLsizei count, const api::GLchar** varyings, api::GLenum bufferMode)
{
	log() << warning << "TransformFeedbackVaryingsNV not available." << std::endl;
}

static void fallbackTranslated(api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "Translated not available." << std::endl;
}

static void fallbackTranslatef(api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "Translatef not available." << std::endl;
}

static void fallbackUniform1d(api::GLint location, api::GLdouble x)
{
	log() << warning << "Uniform1d not available." << std::endl;
}

static void fallbackUniform1dv(api::GLint location, api::GLsizei count, const api::GLdouble* value)
{
	log() << warning << "Uniform1dv not available." << std::endl;
}

static void fallbackUniform1f(api::GLint location, api::GLfloat v0)
{
	log() << warning << "Uniform1f not available." << std::endl;
}

static void fallbackUniform1fARB(api::GLint location, api::GLfloat v0)
{
	log() << warning << "Uniform1fARB not available." << std::endl;
}

static void fallbackUniform1fv(api::GLint location, api::GLsizei count, const api::GLfloat* value)
{
	log() << warning << "Uniform1fv not available." << std::endl;
}

static void fallbackUniform1fvARB(api::GLint location, api::GLsizei count, const api::GLfloat* value)
{
	log() << warning << "Uniform1fvARB not available." << std::endl;
}

static void fallbackUniform1i(api::GLint location, api::GLint v0)
{
	log() << warning << "Uniform1i not available." << std::endl;
}

static void fallbackUniform1iARB(api::GLint location, api::GLint v0)
{
	log() << warning << "Uniform1iARB not available." << std::endl;
}

static void fallbackUniform1iv(api::GLint location, api::GLsizei count, const api::GLint* value)
{
	log() << warning << "Uniform1iv not available." << std::endl;
}

static void fallbackUniform1ivARB(api::GLint location, api::GLsizei count, const api::GLint* value)
{
	log() << warning << "Uniform1ivARB not available." << std::endl;
}

static void fallbackUniform1ui(api::GLint location, api::GLuint v0)
{
	log() << warning << "Uniform1ui not available." << std::endl;
}

static void fallbackUniform1uiEXT(api::GLint location, api::GLuint v0)
{
	log() << warning << "Uniform1uiEXT not available." << std::endl;
}

static void fallbackUniform1uiv(api::GLint location, api::GLsizei count, const api::GLuint* value)
{
	log() << warning << "Uniform1uiv not available." << std::endl;
}

static void fallbackUniform1uivEXT(api::GLint location, api::GLsizei count, const api::GLuint* value)
{
	log() << warning << "Uniform1uivEXT not available." << std::endl;
}

static void fallbackUniform2d(api::GLint location, api::GLdouble x, api::GLdouble y)
{
	log() << warning << "Uniform2d not available." << std::endl;
}

static void fallbackUniform2dv(api::GLint location, api::GLsizei count, const api::GLdouble* value)
{
	log() << warning << "Uniform2dv not available." << std::endl;
}

static void fallbackUniform2f(api::GLint location, api::GLfloat v0, api::GLfloat v1)
{
	log() << warning << "Uniform2f not available." << std::endl;
}

static void fallbackUniform2fARB(api::GLint location, api::GLfloat v0, api::GLfloat v1)
{
	log() << warning << "Uniform2fARB not available." << std::endl;
}

static void fallbackUniform2fv(api::GLint location, api::GLsizei count, const api::GLfloat* value)
{
	log() << warning << "Uniform2fv not available." << std::endl;
}

static void fallbackUniform2fvARB(api::GLint location, api::GLsizei count, const api::GLfloat* value)
{
	log() << warning << "Uniform2fvARB not available." << std::endl;
}

static void fallbackUniform2i(api::GLint location, api::GLint v0, api::GLint v1)
{
	log() << warning << "Uniform2i not available." << std::endl;
}

static void fallbackUniform2iARB(api::GLint location, api::GLint v0, api::GLint v1)
{
	log() << warning << "Uniform2iARB not available." << std::endl;
}

static void fallbackUniform2iv(api::GLint location, api::GLsizei count, const api::GLint* value)
{
	log() << warning << "Uniform2iv not available." << std::endl;
}

static void fallbackUniform2ivARB(api::GLint location, api::GLsizei count, const api::GLint* value)
{
	log() << warning << "Uniform2ivARB not available." << std::endl;
}

static void fallbackUniform2ui(api::GLint location, api::GLuint v0, api::GLuint v1)
{
	log() << warning << "Uniform2ui not available." << std::endl;
}

static void fallbackUniform2uiEXT(api::GLint location, api::GLuint v0, api::GLuint v1)
{
	log() << warning << "Uniform2uiEXT not available." << std::endl;
}

static void fallbackUniform2uiv(api::GLint location, api::GLsizei count, const api::GLuint* value)
{
	log() << warning << "Uniform2uiv not available." << std::endl;
}

static void fallbackUniform2uivEXT(api::GLint location, api::GLsizei count, const api::GLuint* value)
{
	log() << warning << "Uniform2uivEXT not available." << std::endl;
}

static void fallbackUniform3d(api::GLint location, api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "Uniform3d not available." << std::endl;
}

static void fallbackUniform3dv(api::GLint location, api::GLsizei count, const api::GLdouble* value)
{
	log() << warning << "Uniform3dv not available." << std::endl;
}

static void fallbackUniform3f(api::GLint location, api::GLfloat v0, api::GLfloat v1, api::GLfloat v2)
{
	log() << warning << "Uniform3f not available." << std::endl;
}

static void fallbackUniform3fARB(api::GLint location, api::GLfloat v0, api::GLfloat v1, api::GLfloat v2)
{
	log() << warning << "Uniform3fARB not available." << std::endl;
}

static void fallbackUniform3fv(api::GLint location, api::GLsizei count, const api::GLfloat* value)
{
	log() << warning << "Uniform3fv not available." << std::endl;
}

static void fallbackUniform3fvARB(api::GLint location, api::GLsizei count, const api::GLfloat* value)
{
	log() << warning << "Uniform3fvARB not available." << std::endl;
}

static void fallbackUniform3i(api::GLint location, api::GLint v0, api::GLint v1, api::GLint v2)
{
	log() << warning << "Uniform3i not available." << std::endl;
}

static void fallbackUniform3iARB(api::GLint location, api::GLint v0, api::GLint v1, api::GLint v2)
{
	log() << warning << "Uniform3iARB not available." << std::endl;
}

static void fallbackUniform3iv(api::GLint location, api::GLsizei count, const api::GLint* value)
{
	log() << warning << "Uniform3iv not available." << std::endl;
}

static void fallbackUniform3ivARB(api::GLint location, api::GLsizei count, const api::GLint* value)
{
	log() << warning << "Uniform3ivARB not available." << std::endl;
}

static void fallbackUniform3ui(api::GLint location, api::GLuint v0, api::GLuint v1, api::GLuint v2)
{
	log() << warning << "Uniform3ui not available." << std::endl;
}

static void fallbackUniform3uiEXT(api::GLint location, api::GLuint v0, api::GLuint v1, api::GLuint v2)
{
	log() << warning << "Uniform3uiEXT not available." << std::endl;
}

static void fallbackUniform3uiv(api::GLint location, api::GLsizei count, const api::GLuint* value)
{
	log() << warning << "Uniform3uiv not available." << std::endl;
}

static void fallbackUniform3uivEXT(api::GLint location, api::GLsizei count, const api::GLuint* value)
{
	log() << warning << "Uniform3uivEXT not available." << std::endl;
}

static void fallbackUniform4d(api::GLint location, api::GLdouble x, api::GLdouble y, api::GLdouble z, api::GLdouble w)
{
	log() << warning << "Uniform4d not available." << std::endl;
}

static void fallbackUniform4dv(api::GLint location, api::GLsizei count, const api::GLdouble* value)
{
	log() << warning << "Uniform4dv not available." << std::endl;
}

static void fallbackUniform4f(api::GLint location, api::GLfloat v0, api::GLfloat v1, api::GLfloat v2, api::GLfloat v3)
{
	log() << warning << "Uniform4f not available." << std::endl;
}

static void fallbackUniform4fARB(api::GLint location, api::GLfloat v0, api::GLfloat v1, api::GLfloat v2, api::GLfloat v3)
{
	log() << warning << "Uniform4fARB not available." << std::endl;
}

static void fallbackUniform4fv(api::GLint location, api::GLsizei count, const api::GLfloat* value)
{
	log() << warning << "Uniform4fv not available." << std::endl;
}

static void fallbackUniform4fvARB(api::GLint location, api::GLsizei count, const api::GLfloat* value)
{
	log() << warning << "Uniform4fvARB not available." << std::endl;
}

static void fallbackUniform4i(api::GLint location, api::GLint v0, api::GLint v1, api::GLint v2, api::GLint v3)
{
	log() << warning << "Uniform4i not available." << std::endl;
}

static void fallbackUniform4iARB(api::GLint location, api::GLint v0, api::GLint v1, api::GLint v2, api::GLint v3)
{
	log() << warning << "Uniform4iARB not available." << std::endl;
}

static void fallbackUniform4iv(api::GLint location, api::GLsizei count, const api::GLint* value)
{
	log() << warning << "Uniform4iv not available." << std::endl;
}

static void fallbackUniform4ivARB(api::GLint location, api::GLsizei count, const api::GLint* value)
{
	log() << warning << "Uniform4ivARB not available." << std::endl;
}

static void fallbackUniform4ui(api::GLint location, api::GLuint v0, api::GLuint v1, api::GLuint v2, api::GLuint v3)
{
	log() << warning << "Uniform4ui not available." << std::endl;
}

static void fallbackUniform4uiEXT(api::GLint location, api::GLuint v0, api::GLuint v1, api::GLuint v2, api::GLuint v3)
{
	log() << warning << "Uniform4uiEXT not available." << std::endl;
}

static void fallbackUniform4uiv(api::GLint location, api::GLsizei count, const api::GLuint* value)
{
	log() << warning << "Uniform4uiv not available." << std::endl;
}

static void fallbackUniform4uivEXT(api::GLint location, api::GLsizei count, const api::GLuint* value)
{
	log() << warning << "Uniform4uivEXT not available." << std::endl;
}

static void fallbackUniformBlockBinding(api::GLuint program, api::GLuint uniformBlockIndex, api::GLuint uniformBlockBinding)
{
	log() << warning << "UniformBlockBinding not available." << std::endl;
}

static void fallbackUniformBufferEXT(api::GLuint program, api::GLint location, api::GLuint buffer)
{
	log() << warning << "UniformBufferEXT not available." << std::endl;
}

static void fallbackUniformMatrix2dv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "UniformMatrix2dv not available." << std::endl;
}

static void fallbackUniformMatrix2fv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "UniformMatrix2fv not available." << std::endl;
}

static void fallbackUniformMatrix2fvARB(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "UniformMatrix2fvARB not available." << std::endl;
}

static void fallbackUniformMatrix2x3dv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "UniformMatrix2x3dv not available." << std::endl;
}

static void fallbackUniformMatrix2x3fv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "UniformMatrix2x3fv not available." << std::endl;
}

static void fallbackUniformMatrix2x4dv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "UniformMatrix2x4dv not available." << std::endl;
}

static void fallbackUniformMatrix2x4fv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "UniformMatrix2x4fv not available." << std::endl;
}

static void fallbackUniformMatrix3dv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "UniformMatrix3dv not available." << std::endl;
}

static void fallbackUniformMatrix3fv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "UniformMatrix3fv not available." << std::endl;
}

static void fallbackUniformMatrix3fvARB(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "UniformMatrix3fvARB not available." << std::endl;
}

static void fallbackUniformMatrix3x2dv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "UniformMatrix3x2dv not available." << std::endl;
}

static void fallbackUniformMatrix3x2fv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "UniformMatrix3x2fv not available." << std::endl;
}

static void fallbackUniformMatrix3x4dv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "UniformMatrix3x4dv not available." << std::endl;
}

static void fallbackUniformMatrix3x4fv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "UniformMatrix3x4fv not available." << std::endl;
}

static void fallbackUniformMatrix4dv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "UniformMatrix4dv not available." << std::endl;
}

static void fallbackUniformMatrix4fv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "UniformMatrix4fv not available." << std::endl;
}

static void fallbackUniformMatrix4fvARB(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "UniformMatrix4fvARB not available." << std::endl;
}

static void fallbackUniformMatrix4x2dv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "UniformMatrix4x2dv not available." << std::endl;
}

static void fallbackUniformMatrix4x2fv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "UniformMatrix4x2fv not available." << std::endl;
}

static void fallbackUniformMatrix4x3dv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "UniformMatrix4x3dv not available." << std::endl;
}

static void fallbackUniformMatrix4x3fv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "UniformMatrix4x3fv not available." << std::endl;
}

static void fallbackUniformSubroutinesuiv(api::GLenum shadertype, api::GLsizei count, const api::GLuint* indices)
{
	log() << warning << "UniformSubroutinesuiv not available." << std::endl;
}

static void fallbackUniformui64NV(api::GLint location, api::GLuint64EXT value)
{
	log() << warning << "Uniformui64NV not available." << std::endl;
}

static void fallbackUniformui64vNV(api::GLint location, api::GLsizei count, const api::GLuint64EXT* value)
{
	log() << warning << "Uniformui64vNV not available." << std::endl;
}

static void fallbackUnlockArraysEXT()
{
	log() << warning << "UnlockArraysEXT not available." << std::endl;
}

static api::GLboolean fallbackUnmapBuffer(api::GLenum target)
{
	log() << warning << "UnmapBuffer not available." << std::endl;
}

static api::GLboolean fallbackUnmapBufferARB(api::GLenum target)
{
	log() << warning << "UnmapBufferARB not available." << std::endl;
}

static api::GLboolean fallbackUnmapNamedBufferEXT(api::GLuint buffer)
{
	log() << warning << "UnmapNamedBufferEXT not available." << std::endl;
}

static void fallbackUnmapObjectBufferATI(api::GLuint buffer)
{
	log() << warning << "UnmapObjectBufferATI not available." << std::endl;
}

static void fallbackUpdateObjectBufferATI(api::GLuint buffer, api::GLuint offset, api::GLsizei size, const api::GLvoid* pointer, api::GLenum preserve)
{
	log() << warning << "UpdateObjectBufferATI not available." << std::endl;
}

static void fallbackUseProgram(api::GLuint program)
{
	log() << warning << "UseProgram not available." << std::endl;
}

static void fallbackUseProgramObjectARB(api::GLhandleARB programObj)
{
	log() << warning << "UseProgramObjectARB not available." << std::endl;
}

static void fallbackUseShaderProgramEXT(api::GLenum type, api::GLuint program)
{
	log() << warning << "UseShaderProgramEXT not available." << std::endl;
}

static void fallbackValidateProgram(api::GLuint program)
{
	log() << warning << "ValidateProgram not available." << std::endl;
}

static void fallbackValidateProgramARB(api::GLhandleARB programObj)
{
	log() << warning << "ValidateProgramARB not available." << std::endl;
}

static void fallbackVariantArrayObjectATI(api::GLuint id, api::GLenum type, api::GLsizei stride, api::GLuint buffer, api::GLuint offset)
{
	log() << warning << "VariantArrayObjectATI not available." << std::endl;
}

static void fallbackVariantPointerEXT(api::GLuint id, api::GLenum type, api::GLuint stride, const api::GLvoid* addr)
{
	log() << warning << "VariantPointerEXT not available." << std::endl;
}

static void fallbackVariantbvEXT(api::GLuint id, const api::GLbyte* addr)
{
	log() << warning << "VariantbvEXT not available." << std::endl;
}

static void fallbackVariantdvEXT(api::GLuint id, const api::GLdouble* addr)
{
	log() << warning << "VariantdvEXT not available." << std::endl;
}

static void fallbackVariantfvEXT(api::GLuint id, const api::GLfloat* addr)
{
	log() << warning << "VariantfvEXT not available." << std::endl;
}

static void fallbackVariantivEXT(api::GLuint id, const api::GLint* addr)
{
	log() << warning << "VariantivEXT not available." << std::endl;
}

static void fallbackVariantsvEXT(api::GLuint id, const api::GLshort* addr)
{
	log() << warning << "VariantsvEXT not available." << std::endl;
}

static void fallbackVariantubvEXT(api::GLuint id, const api::GLubyte* addr)
{
	log() << warning << "VariantubvEXT not available." << std::endl;
}

static void fallbackVariantuivEXT(api::GLuint id, const api::GLuint* addr)
{
	log() << warning << "VariantuivEXT not available." << std::endl;
}

static void fallbackVariantusvEXT(api::GLuint id, const api::GLushort* addr)
{
	log() << warning << "VariantusvEXT not available." << std::endl;
}

static void fallbackVertex2d(api::GLdouble x, api::GLdouble y)
{
	log() << warning << "Vertex2d not available." << std::endl;
}

static void fallbackVertex2dv(const api::GLdouble* v)
{
	log() << warning << "Vertex2dv not available." << std::endl;
}

static void fallbackVertex2f(api::GLfloat x, api::GLfloat y)
{
	log() << warning << "Vertex2f not available." << std::endl;
}

static void fallbackVertex2fv(const api::GLfloat* v)
{
	log() << warning << "Vertex2fv not available." << std::endl;
}

static void fallbackVertex2hNV(api::GLhalfNV x, api::GLhalfNV y)
{
	log() << warning << "Vertex2hNV not available." << std::endl;
}

static void fallbackVertex2hvNV(const api::GLhalfNV* v)
{
	log() << warning << "Vertex2hvNV not available." << std::endl;
}

static void fallbackVertex2i(api::GLint x, api::GLint y)
{
	log() << warning << "Vertex2i not available." << std::endl;
}

static void fallbackVertex2iv(const api::GLint* v)
{
	log() << warning << "Vertex2iv not available." << std::endl;
}

static void fallbackVertex2s(api::GLshort x, api::GLshort y)
{
	log() << warning << "Vertex2s not available." << std::endl;
}

static void fallbackVertex2sv(const api::GLshort* v)
{
	log() << warning << "Vertex2sv not available." << std::endl;
}

static void fallbackVertex3d(api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "Vertex3d not available." << std::endl;
}

static void fallbackVertex3dv(const api::GLdouble* v)
{
	log() << warning << "Vertex3dv not available." << std::endl;
}

static void fallbackVertex3f(api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "Vertex3f not available." << std::endl;
}

static void fallbackVertex3fv(const api::GLfloat* v)
{
	log() << warning << "Vertex3fv not available." << std::endl;
}

static void fallbackVertex3hNV(api::GLhalfNV x, api::GLhalfNV y, api::GLhalfNV z)
{
	log() << warning << "Vertex3hNV not available." << std::endl;
}

static void fallbackVertex3hvNV(const api::GLhalfNV* v)
{
	log() << warning << "Vertex3hvNV not available." << std::endl;
}

static void fallbackVertex3i(api::GLint x, api::GLint y, api::GLint z)
{
	log() << warning << "Vertex3i not available." << std::endl;
}

static void fallbackVertex3iv(const api::GLint* v)
{
	log() << warning << "Vertex3iv not available." << std::endl;
}

static void fallbackVertex3s(api::GLshort x, api::GLshort y, api::GLshort z)
{
	log() << warning << "Vertex3s not available." << std::endl;
}

static void fallbackVertex3sv(const api::GLshort* v)
{
	log() << warning << "Vertex3sv not available." << std::endl;
}

static void fallbackVertex4d(api::GLdouble x, api::GLdouble y, api::GLdouble z, api::GLdouble w)
{
	log() << warning << "Vertex4d not available." << std::endl;
}

static void fallbackVertex4dv(const api::GLdouble* v)
{
	log() << warning << "Vertex4dv not available." << std::endl;
}

static void fallbackVertex4f(api::GLfloat x, api::GLfloat y, api::GLfloat z, api::GLfloat w)
{
	log() << warning << "Vertex4f not available." << std::endl;
}

static void fallbackVertex4fv(const api::GLfloat* v)
{
	log() << warning << "Vertex4fv not available." << std::endl;
}

static void fallbackVertex4hNV(api::GLhalfNV x, api::GLhalfNV y, api::GLhalfNV z, api::GLhalfNV w)
{
	log() << warning << "Vertex4hNV not available." << std::endl;
}

static void fallbackVertex4hvNV(const api::GLhalfNV* v)
{
	log() << warning << "Vertex4hvNV not available." << std::endl;
}

static void fallbackVertex4i(api::GLint x, api::GLint y, api::GLint z, api::GLint w)
{
	log() << warning << "Vertex4i not available." << std::endl;
}

static void fallbackVertex4iv(const api::GLint* v)
{
	log() << warning << "Vertex4iv not available." << std::endl;
}

static void fallbackVertex4s(api::GLshort x, api::GLshort y, api::GLshort z, api::GLshort w)
{
	log() << warning << "Vertex4s not available." << std::endl;
}

static void fallbackVertex4sv(const api::GLshort* v)
{
	log() << warning << "Vertex4sv not available." << std::endl;
}

static void fallbackVertexArrayParameteriAPPLE(api::GLenum pname, api::GLint param)
{
	log() << warning << "VertexArrayParameteriAPPLE not available." << std::endl;
}

static void fallbackVertexArrayRangeAPPLE(api::GLsizei length, api::GLvoid* pointer)
{
	log() << warning << "VertexArrayRangeAPPLE not available." << std::endl;
}

static void fallbackVertexArrayRangeNV(api::GLsizei length, const api::GLvoid* pointer)
{
	log() << warning << "VertexArrayRangeNV not available." << std::endl;
}

static void fallbackVertexAttrib1d(api::GLuint index, api::GLdouble x)
{
	log() << warning << "VertexAttrib1d not available." << std::endl;
}

static void fallbackVertexAttrib1dARB(api::GLuint index, api::GLdouble x)
{
	log() << warning << "VertexAttrib1dARB not available." << std::endl;
}

static void fallbackVertexAttrib1dNV(api::GLuint index, api::GLdouble x)
{
	log() << warning << "VertexAttrib1dNV not available." << std::endl;
}

static void fallbackVertexAttrib1dv(api::GLuint index, const api::GLdouble* v)
{
	log() << warning << "VertexAttrib1dv not available." << std::endl;
}

static void fallbackVertexAttrib1dvARB(api::GLuint index, const api::GLdouble* v)
{
	log() << warning << "VertexAttrib1dvARB not available." << std::endl;
}

static void fallbackVertexAttrib1dvNV(api::GLuint index, const api::GLdouble* v)
{
	log() << warning << "VertexAttrib1dvNV not available." << std::endl;
}

static void fallbackVertexAttrib1f(api::GLuint index, api::GLfloat x)
{
	log() << warning << "VertexAttrib1f not available." << std::endl;
}

static void fallbackVertexAttrib1fARB(api::GLuint index, api::GLfloat x)
{
	log() << warning << "VertexAttrib1fARB not available." << std::endl;
}

static void fallbackVertexAttrib1fNV(api::GLuint index, api::GLfloat x)
{
	log() << warning << "VertexAttrib1fNV not available." << std::endl;
}

static void fallbackVertexAttrib1fv(api::GLuint index, const api::GLfloat* v)
{
	log() << warning << "VertexAttrib1fv not available." << std::endl;
}

static void fallbackVertexAttrib1fvARB(api::GLuint index, const api::GLfloat* v)
{
	log() << warning << "VertexAttrib1fvARB not available." << std::endl;
}

static void fallbackVertexAttrib1fvNV(api::GLuint index, const api::GLfloat* v)
{
	log() << warning << "VertexAttrib1fvNV not available." << std::endl;
}

static void fallbackVertexAttrib1hNV(api::GLuint index, api::GLhalfNV x)
{
	log() << warning << "VertexAttrib1hNV not available." << std::endl;
}

static void fallbackVertexAttrib1hvNV(api::GLuint index, const api::GLhalfNV* v)
{
	log() << warning << "VertexAttrib1hvNV not available." << std::endl;
}

static void fallbackVertexAttrib1s(api::GLuint index, api::GLshort x)
{
	log() << warning << "VertexAttrib1s not available." << std::endl;
}

static void fallbackVertexAttrib1sARB(api::GLuint index, api::GLshort x)
{
	log() << warning << "VertexAttrib1sARB not available." << std::endl;
}

static void fallbackVertexAttrib1sNV(api::GLuint index, api::GLshort x)
{
	log() << warning << "VertexAttrib1sNV not available." << std::endl;
}

static void fallbackVertexAttrib1sv(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "VertexAttrib1sv not available." << std::endl;
}

static void fallbackVertexAttrib1svARB(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "VertexAttrib1svARB not available." << std::endl;
}

static void fallbackVertexAttrib1svNV(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "VertexAttrib1svNV not available." << std::endl;
}

static void fallbackVertexAttrib2d(api::GLuint index, api::GLdouble x, api::GLdouble y)
{
	log() << warning << "VertexAttrib2d not available." << std::endl;
}

static void fallbackVertexAttrib2dARB(api::GLuint index, api::GLdouble x, api::GLdouble y)
{
	log() << warning << "VertexAttrib2dARB not available." << std::endl;
}

static void fallbackVertexAttrib2dNV(api::GLuint index, api::GLdouble x, api::GLdouble y)
{
	log() << warning << "VertexAttrib2dNV not available." << std::endl;
}

static void fallbackVertexAttrib2dv(api::GLuint index, const api::GLdouble* v)
{
	log() << warning << "VertexAttrib2dv not available." << std::endl;
}

static void fallbackVertexAttrib2dvARB(api::GLuint index, const api::GLdouble* v)
{
	log() << warning << "VertexAttrib2dvARB not available." << std::endl;
}

static void fallbackVertexAttrib2dvNV(api::GLuint index, const api::GLdouble* v)
{
	log() << warning << "VertexAttrib2dvNV not available." << std::endl;
}

static void fallbackVertexAttrib2f(api::GLuint index, api::GLfloat x, api::GLfloat y)
{
	log() << warning << "VertexAttrib2f not available." << std::endl;
}

static void fallbackVertexAttrib2fARB(api::GLuint index, api::GLfloat x, api::GLfloat y)
{
	log() << warning << "VertexAttrib2fARB not available." << std::endl;
}

static void fallbackVertexAttrib2fNV(api::GLuint index, api::GLfloat x, api::GLfloat y)
{
	log() << warning << "VertexAttrib2fNV not available." << std::endl;
}

static void fallbackVertexAttrib2fv(api::GLuint index, const api::GLfloat* v)
{
	log() << warning << "VertexAttrib2fv not available." << std::endl;
}

static void fallbackVertexAttrib2fvARB(api::GLuint index, const api::GLfloat* v)
{
	log() << warning << "VertexAttrib2fvARB not available." << std::endl;
}

static void fallbackVertexAttrib2fvNV(api::GLuint index, const api::GLfloat* v)
{
	log() << warning << "VertexAttrib2fvNV not available." << std::endl;
}

static void fallbackVertexAttrib2hNV(api::GLuint index, api::GLhalfNV x, api::GLhalfNV y)
{
	log() << warning << "VertexAttrib2hNV not available." << std::endl;
}

static void fallbackVertexAttrib2hvNV(api::GLuint index, const api::GLhalfNV* v)
{
	log() << warning << "VertexAttrib2hvNV not available." << std::endl;
}

static void fallbackVertexAttrib2s(api::GLuint index, api::GLshort x, api::GLshort y)
{
	log() << warning << "VertexAttrib2s not available." << std::endl;
}

static void fallbackVertexAttrib2sARB(api::GLuint index, api::GLshort x, api::GLshort y)
{
	log() << warning << "VertexAttrib2sARB not available." << std::endl;
}

static void fallbackVertexAttrib2sNV(api::GLuint index, api::GLshort x, api::GLshort y)
{
	log() << warning << "VertexAttrib2sNV not available." << std::endl;
}

static void fallbackVertexAttrib2sv(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "VertexAttrib2sv not available." << std::endl;
}

static void fallbackVertexAttrib2svARB(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "VertexAttrib2svARB not available." << std::endl;
}

static void fallbackVertexAttrib2svNV(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "VertexAttrib2svNV not available." << std::endl;
}

static void fallbackVertexAttrib3d(api::GLuint index, api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "VertexAttrib3d not available." << std::endl;
}

static void fallbackVertexAttrib3dARB(api::GLuint index, api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "VertexAttrib3dARB not available." << std::endl;
}

static void fallbackVertexAttrib3dNV(api::GLuint index, api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "VertexAttrib3dNV not available." << std::endl;
}

static void fallbackVertexAttrib3dv(api::GLuint index, const api::GLdouble* v)
{
	log() << warning << "VertexAttrib3dv not available." << std::endl;
}

static void fallbackVertexAttrib3dvARB(api::GLuint index, const api::GLdouble* v)
{
	log() << warning << "VertexAttrib3dvARB not available." << std::endl;
}

static void fallbackVertexAttrib3dvNV(api::GLuint index, const api::GLdouble* v)
{
	log() << warning << "VertexAttrib3dvNV not available." << std::endl;
}

static void fallbackVertexAttrib3f(api::GLuint index, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "VertexAttrib3f not available." << std::endl;
}

static void fallbackVertexAttrib3fARB(api::GLuint index, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "VertexAttrib3fARB not available." << std::endl;
}

static void fallbackVertexAttrib3fNV(api::GLuint index, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "VertexAttrib3fNV not available." << std::endl;
}

static void fallbackVertexAttrib3fv(api::GLuint index, const api::GLfloat* v)
{
	log() << warning << "VertexAttrib3fv not available." << std::endl;
}

static void fallbackVertexAttrib3fvARB(api::GLuint index, const api::GLfloat* v)
{
	log() << warning << "VertexAttrib3fvARB not available." << std::endl;
}

static void fallbackVertexAttrib3fvNV(api::GLuint index, const api::GLfloat* v)
{
	log() << warning << "VertexAttrib3fvNV not available." << std::endl;
}

static void fallbackVertexAttrib3hNV(api::GLuint index, api::GLhalfNV x, api::GLhalfNV y, api::GLhalfNV z)
{
	log() << warning << "VertexAttrib3hNV not available." << std::endl;
}

static void fallbackVertexAttrib3hvNV(api::GLuint index, const api::GLhalfNV* v)
{
	log() << warning << "VertexAttrib3hvNV not available." << std::endl;
}

static void fallbackVertexAttrib3s(api::GLuint index, api::GLshort x, api::GLshort y, api::GLshort z)
{
	log() << warning << "VertexAttrib3s not available." << std::endl;
}

static void fallbackVertexAttrib3sARB(api::GLuint index, api::GLshort x, api::GLshort y, api::GLshort z)
{
	log() << warning << "VertexAttrib3sARB not available." << std::endl;
}

static void fallbackVertexAttrib3sNV(api::GLuint index, api::GLshort x, api::GLshort y, api::GLshort z)
{
	log() << warning << "VertexAttrib3sNV not available." << std::endl;
}

static void fallbackVertexAttrib3sv(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "VertexAttrib3sv not available." << std::endl;
}

static void fallbackVertexAttrib3svARB(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "VertexAttrib3svARB not available." << std::endl;
}

static void fallbackVertexAttrib3svNV(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "VertexAttrib3svNV not available." << std::endl;
}

static void fallbackVertexAttrib4Nbv(api::GLuint index, const api::GLbyte* v)
{
	log() << warning << "VertexAttrib4Nbv not available." << std::endl;
}

static void fallbackVertexAttrib4NbvARB(api::GLuint index, const api::GLbyte* v)
{
	log() << warning << "VertexAttrib4NbvARB not available." << std::endl;
}

static void fallbackVertexAttrib4Niv(api::GLuint index, const api::GLint* v)
{
	log() << warning << "VertexAttrib4Niv not available." << std::endl;
}

static void fallbackVertexAttrib4NivARB(api::GLuint index, const api::GLint* v)
{
	log() << warning << "VertexAttrib4NivARB not available." << std::endl;
}

static void fallbackVertexAttrib4Nsv(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "VertexAttrib4Nsv not available." << std::endl;
}

static void fallbackVertexAttrib4NsvARB(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "VertexAttrib4NsvARB not available." << std::endl;
}

static void fallbackVertexAttrib4Nub(api::GLuint index, api::GLubyte x, api::GLubyte y, api::GLubyte z, api::GLubyte w)
{
	log() << warning << "VertexAttrib4Nub not available." << std::endl;
}

static void fallbackVertexAttrib4NubARB(api::GLuint index, api::GLubyte x, api::GLubyte y, api::GLubyte z, api::GLubyte w)
{
	log() << warning << "VertexAttrib4NubARB not available." << std::endl;
}

static void fallbackVertexAttrib4Nubv(api::GLuint index, const api::GLubyte* v)
{
	log() << warning << "VertexAttrib4Nubv not available." << std::endl;
}

static void fallbackVertexAttrib4NubvARB(api::GLuint index, const api::GLubyte* v)
{
	log() << warning << "VertexAttrib4NubvARB not available." << std::endl;
}

static void fallbackVertexAttrib4Nuiv(api::GLuint index, const api::GLuint* v)
{
	log() << warning << "VertexAttrib4Nuiv not available." << std::endl;
}

static void fallbackVertexAttrib4NuivARB(api::GLuint index, const api::GLuint* v)
{
	log() << warning << "VertexAttrib4NuivARB not available." << std::endl;
}

static void fallbackVertexAttrib4Nusv(api::GLuint index, const api::GLushort* v)
{
	log() << warning << "VertexAttrib4Nusv not available." << std::endl;
}

static void fallbackVertexAttrib4NusvARB(api::GLuint index, const api::GLushort* v)
{
	log() << warning << "VertexAttrib4NusvARB not available." << std::endl;
}

static void fallbackVertexAttrib4bv(api::GLuint index, const api::GLbyte* v)
{
	log() << warning << "VertexAttrib4bv not available." << std::endl;
}

static void fallbackVertexAttrib4bvARB(api::GLuint index, const api::GLbyte* v)
{
	log() << warning << "VertexAttrib4bvARB not available." << std::endl;
}

static void fallbackVertexAttrib4d(api::GLuint index, api::GLdouble x, api::GLdouble y, api::GLdouble z, api::GLdouble w)
{
	log() << warning << "VertexAttrib4d not available." << std::endl;
}

static void fallbackVertexAttrib4dARB(api::GLuint index, api::GLdouble x, api::GLdouble y, api::GLdouble z, api::GLdouble w)
{
	log() << warning << "VertexAttrib4dARB not available." << std::endl;
}

static void fallbackVertexAttrib4dNV(api::GLuint index, api::GLdouble x, api::GLdouble y, api::GLdouble z, api::GLdouble w)
{
	log() << warning << "VertexAttrib4dNV not available." << std::endl;
}

static void fallbackVertexAttrib4dv(api::GLuint index, const api::GLdouble* v)
{
	log() << warning << "VertexAttrib4dv not available." << std::endl;
}

static void fallbackVertexAttrib4dvARB(api::GLuint index, const api::GLdouble* v)
{
	log() << warning << "VertexAttrib4dvARB not available." << std::endl;
}

static void fallbackVertexAttrib4dvNV(api::GLuint index, const api::GLdouble* v)
{
	log() << warning << "VertexAttrib4dvNV not available." << std::endl;
}

static void fallbackVertexAttrib4f(api::GLuint index, api::GLfloat x, api::GLfloat y, api::GLfloat z, api::GLfloat w)
{
	log() << warning << "VertexAttrib4f not available." << std::endl;
}

static void fallbackVertexAttrib4fARB(api::GLuint index, api::GLfloat x, api::GLfloat y, api::GLfloat z, api::GLfloat w)
{
	log() << warning << "VertexAttrib4fARB not available." << std::endl;
}

static void fallbackVertexAttrib4fNV(api::GLuint index, api::GLfloat x, api::GLfloat y, api::GLfloat z, api::GLfloat w)
{
	log() << warning << "VertexAttrib4fNV not available." << std::endl;
}

static void fallbackVertexAttrib4fv(api::GLuint index, const api::GLfloat* v)
{
	log() << warning << "VertexAttrib4fv not available." << std::endl;
}

static void fallbackVertexAttrib4fvARB(api::GLuint index, const api::GLfloat* v)
{
	log() << warning << "VertexAttrib4fvARB not available." << std::endl;
}

static void fallbackVertexAttrib4fvNV(api::GLuint index, const api::GLfloat* v)
{
	log() << warning << "VertexAttrib4fvNV not available." << std::endl;
}

static void fallbackVertexAttrib4hNV(api::GLuint index, api::GLhalfNV x, api::GLhalfNV y, api::GLhalfNV z, api::GLhalfNV w)
{
	log() << warning << "VertexAttrib4hNV not available." << std::endl;
}

static void fallbackVertexAttrib4hvNV(api::GLuint index, const api::GLhalfNV* v)
{
	log() << warning << "VertexAttrib4hvNV not available." << std::endl;
}

static void fallbackVertexAttrib4iv(api::GLuint index, const api::GLint* v)
{
	log() << warning << "VertexAttrib4iv not available." << std::endl;
}

static void fallbackVertexAttrib4ivARB(api::GLuint index, const api::GLint* v)
{
	log() << warning << "VertexAttrib4ivARB not available." << std::endl;
}

static void fallbackVertexAttrib4s(api::GLuint index, api::GLshort x, api::GLshort y, api::GLshort z, api::GLshort w)
{
	log() << warning << "VertexAttrib4s not available." << std::endl;
}

static void fallbackVertexAttrib4sARB(api::GLuint index, api::GLshort x, api::GLshort y, api::GLshort z, api::GLshort w)
{
	log() << warning << "VertexAttrib4sARB not available." << std::endl;
}

static void fallbackVertexAttrib4sNV(api::GLuint index, api::GLshort x, api::GLshort y, api::GLshort z, api::GLshort w)
{
	log() << warning << "VertexAttrib4sNV not available." << std::endl;
}

static void fallbackVertexAttrib4sv(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "VertexAttrib4sv not available." << std::endl;
}

static void fallbackVertexAttrib4svARB(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "VertexAttrib4svARB not available." << std::endl;
}

static void fallbackVertexAttrib4svNV(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "VertexAttrib4svNV not available." << std::endl;
}

static void fallbackVertexAttrib4ubNV(api::GLuint index, api::GLubyte x, api::GLubyte y, api::GLubyte z, api::GLubyte w)
{
	log() << warning << "VertexAttrib4ubNV not available." << std::endl;
}

static void fallbackVertexAttrib4ubv(api::GLuint index, const api::GLubyte* v)
{
	log() << warning << "VertexAttrib4ubv not available." << std::endl;
}

static void fallbackVertexAttrib4ubvARB(api::GLuint index, const api::GLubyte* v)
{
	log() << warning << "VertexAttrib4ubvARB not available." << std::endl;
}

static void fallbackVertexAttrib4ubvNV(api::GLuint index, const api::GLubyte* v)
{
	log() << warning << "VertexAttrib4ubvNV not available." << std::endl;
}

static void fallbackVertexAttrib4uiv(api::GLuint index, const api::GLuint* v)
{
	log() << warning << "VertexAttrib4uiv not available." << std::endl;
}

static void fallbackVertexAttrib4uivARB(api::GLuint index, const api::GLuint* v)
{
	log() << warning << "VertexAttrib4uivARB not available." << std::endl;
}

static void fallbackVertexAttrib4usv(api::GLuint index, const api::GLushort* v)
{
	log() << warning << "VertexAttrib4usv not available." << std::endl;
}

static void fallbackVertexAttrib4usvARB(api::GLuint index, const api::GLushort* v)
{
	log() << warning << "VertexAttrib4usvARB not available." << std::endl;
}

static void fallbackVertexAttribArrayObjectATI(api::GLuint index, api::GLint size, api::GLenum type, api::GLboolean normalized, api::GLsizei stride, api::GLuint buffer, api::GLuint offset)
{
	log() << warning << "VertexAttribArrayObjectATI not available." << std::endl;
}

static void fallbackVertexAttribDivisorARB(api::GLuint index, api::GLuint divisor)
{
	log() << warning << "VertexAttribDivisorARB not available." << std::endl;
}

static void fallbackVertexAttribFormatNV(api::GLuint index, api::GLint size, api::GLenum type, api::GLboolean normalized, api::GLsizei stride)
{
	log() << warning << "VertexAttribFormatNV not available." << std::endl;
}

static void fallbackVertexAttribI1i(api::GLuint index, api::GLint x)
{
	log() << warning << "VertexAttribI1i not available." << std::endl;
}

static void fallbackVertexAttribI1iEXT(api::GLuint index, api::GLint x)
{
	log() << warning << "VertexAttribI1iEXT not available." << std::endl;
}

static void fallbackVertexAttribI1iv(api::GLuint index, const api::GLint* v)
{
	log() << warning << "VertexAttribI1iv not available." << std::endl;
}

static void fallbackVertexAttribI1ivEXT(api::GLuint index, const api::GLint* v)
{
	log() << warning << "VertexAttribI1ivEXT not available." << std::endl;
}

static void fallbackVertexAttribI1ui(api::GLuint index, api::GLuint x)
{
	log() << warning << "VertexAttribI1ui not available." << std::endl;
}

static void fallbackVertexAttribI1uiEXT(api::GLuint index, api::GLuint x)
{
	log() << warning << "VertexAttribI1uiEXT not available." << std::endl;
}

static void fallbackVertexAttribI1uiv(api::GLuint index, const api::GLuint* v)
{
	log() << warning << "VertexAttribI1uiv not available." << std::endl;
}

static void fallbackVertexAttribI1uivEXT(api::GLuint index, const api::GLuint* v)
{
	log() << warning << "VertexAttribI1uivEXT not available." << std::endl;
}

static void fallbackVertexAttribI2i(api::GLuint index, api::GLint x, api::GLint y)
{
	log() << warning << "VertexAttribI2i not available." << std::endl;
}

static void fallbackVertexAttribI2iEXT(api::GLuint index, api::GLint x, api::GLint y)
{
	log() << warning << "VertexAttribI2iEXT not available." << std::endl;
}

static void fallbackVertexAttribI2iv(api::GLuint index, const api::GLint* v)
{
	log() << warning << "VertexAttribI2iv not available." << std::endl;
}

static void fallbackVertexAttribI2ivEXT(api::GLuint index, const api::GLint* v)
{
	log() << warning << "VertexAttribI2ivEXT not available." << std::endl;
}

static void fallbackVertexAttribI2ui(api::GLuint index, api::GLuint x, api::GLuint y)
{
	log() << warning << "VertexAttribI2ui not available." << std::endl;
}

static void fallbackVertexAttribI2uiEXT(api::GLuint index, api::GLuint x, api::GLuint y)
{
	log() << warning << "VertexAttribI2uiEXT not available." << std::endl;
}

static void fallbackVertexAttribI2uiv(api::GLuint index, const api::GLuint* v)
{
	log() << warning << "VertexAttribI2uiv not available." << std::endl;
}

static void fallbackVertexAttribI2uivEXT(api::GLuint index, const api::GLuint* v)
{
	log() << warning << "VertexAttribI2uivEXT not available." << std::endl;
}

static void fallbackVertexAttribI3i(api::GLuint index, api::GLint x, api::GLint y, api::GLint z)
{
	log() << warning << "VertexAttribI3i not available." << std::endl;
}

static void fallbackVertexAttribI3iEXT(api::GLuint index, api::GLint x, api::GLint y, api::GLint z)
{
	log() << warning << "VertexAttribI3iEXT not available." << std::endl;
}

static void fallbackVertexAttribI3iv(api::GLuint index, const api::GLint* v)
{
	log() << warning << "VertexAttribI3iv not available." << std::endl;
}

static void fallbackVertexAttribI3ivEXT(api::GLuint index, const api::GLint* v)
{
	log() << warning << "VertexAttribI3ivEXT not available." << std::endl;
}

static void fallbackVertexAttribI3ui(api::GLuint index, api::GLuint x, api::GLuint y, api::GLuint z)
{
	log() << warning << "VertexAttribI3ui not available." << std::endl;
}

static void fallbackVertexAttribI3uiEXT(api::GLuint index, api::GLuint x, api::GLuint y, api::GLuint z)
{
	log() << warning << "VertexAttribI3uiEXT not available." << std::endl;
}

static void fallbackVertexAttribI3uiv(api::GLuint index, const api::GLuint* v)
{
	log() << warning << "VertexAttribI3uiv not available." << std::endl;
}

static void fallbackVertexAttribI3uivEXT(api::GLuint index, const api::GLuint* v)
{
	log() << warning << "VertexAttribI3uivEXT not available." << std::endl;
}

static void fallbackVertexAttribI4bv(api::GLuint index, const api::GLbyte* v)
{
	log() << warning << "VertexAttribI4bv not available." << std::endl;
}

static void fallbackVertexAttribI4bvEXT(api::GLuint index, const api::GLbyte* v)
{
	log() << warning << "VertexAttribI4bvEXT not available." << std::endl;
}

static void fallbackVertexAttribI4i(api::GLuint index, api::GLint x, api::GLint y, api::GLint z, api::GLint w)
{
	log() << warning << "VertexAttribI4i not available." << std::endl;
}

static void fallbackVertexAttribI4iEXT(api::GLuint index, api::GLint x, api::GLint y, api::GLint z, api::GLint w)
{
	log() << warning << "VertexAttribI4iEXT not available." << std::endl;
}

static void fallbackVertexAttribI4iv(api::GLuint index, const api::GLint* v)
{
	log() << warning << "VertexAttribI4iv not available." << std::endl;
}

static void fallbackVertexAttribI4ivEXT(api::GLuint index, const api::GLint* v)
{
	log() << warning << "VertexAttribI4ivEXT not available." << std::endl;
}

static void fallbackVertexAttribI4sv(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "VertexAttribI4sv not available." << std::endl;
}

static void fallbackVertexAttribI4svEXT(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "VertexAttribI4svEXT not available." << std::endl;
}

static void fallbackVertexAttribI4ubv(api::GLuint index, const api::GLubyte* v)
{
	log() << warning << "VertexAttribI4ubv not available." << std::endl;
}

static void fallbackVertexAttribI4ubvEXT(api::GLuint index, const api::GLubyte* v)
{
	log() << warning << "VertexAttribI4ubvEXT not available." << std::endl;
}

static void fallbackVertexAttribI4ui(api::GLuint index, api::GLuint x, api::GLuint y, api::GLuint z, api::GLuint w)
{
	log() << warning << "VertexAttribI4ui not available." << std::endl;
}

static void fallbackVertexAttribI4uiEXT(api::GLuint index, api::GLuint x, api::GLuint y, api::GLuint z, api::GLuint w)
{
	log() << warning << "VertexAttribI4uiEXT not available." << std::endl;
}

static void fallbackVertexAttribI4uiv(api::GLuint index, const api::GLuint* v)
{
	log() << warning << "VertexAttribI4uiv not available." << std::endl;
}

static void fallbackVertexAttribI4uivEXT(api::GLuint index, const api::GLuint* v)
{
	log() << warning << "VertexAttribI4uivEXT not available." << std::endl;
}

static void fallbackVertexAttribI4usv(api::GLuint index, const api::GLushort* v)
{
	log() << warning << "VertexAttribI4usv not available." << std::endl;
}

static void fallbackVertexAttribI4usvEXT(api::GLuint index, const api::GLushort* v)
{
	log() << warning << "VertexAttribI4usvEXT not available." << std::endl;
}

static void fallbackVertexAttribIFormatNV(api::GLuint index, api::GLint size, api::GLenum type, api::GLsizei stride)
{
	log() << warning << "VertexAttribIFormatNV not available." << std::endl;
}

static void fallbackVertexAttribIPointer(api::GLuint index, api::GLint size, api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "VertexAttribIPointer not available." << std::endl;
}

static void fallbackVertexAttribIPointerEXT(api::GLuint index, api::GLint size, api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "VertexAttribIPointerEXT not available." << std::endl;
}

static void fallbackVertexAttribP1ui(api::GLuint index, api::GLenum type, api::GLboolean normalized, api::GLuint value)
{
	log() << warning << "VertexAttribP1ui not available." << std::endl;
}

static void fallbackVertexAttribP1uiv(api::GLuint index, api::GLenum type, api::GLboolean normalized, const api::GLuint* value)
{
	log() << warning << "VertexAttribP1uiv not available." << std::endl;
}

static void fallbackVertexAttribP2ui(api::GLuint index, api::GLenum type, api::GLboolean normalized, api::GLuint value)
{
	log() << warning << "VertexAttribP2ui not available." << std::endl;
}

static void fallbackVertexAttribP2uiv(api::GLuint index, api::GLenum type, api::GLboolean normalized, const api::GLuint* value)
{
	log() << warning << "VertexAttribP2uiv not available." << std::endl;
}

static void fallbackVertexAttribP3ui(api::GLuint index, api::GLenum type, api::GLboolean normalized, api::GLuint value)
{
	log() << warning << "VertexAttribP3ui not available." << std::endl;
}

static void fallbackVertexAttribP3uiv(api::GLuint index, api::GLenum type, api::GLboolean normalized, const api::GLuint* value)
{
	log() << warning << "VertexAttribP3uiv not available." << std::endl;
}

static void fallbackVertexAttribP4ui(api::GLuint index, api::GLenum type, api::GLboolean normalized, api::GLuint value)
{
	log() << warning << "VertexAttribP4ui not available." << std::endl;
}

static void fallbackVertexAttribP4uiv(api::GLuint index, api::GLenum type, api::GLboolean normalized, const api::GLuint* value)
{
	log() << warning << "VertexAttribP4uiv not available." << std::endl;
}

static void fallbackVertexAttribPointer(api::GLuint index, api::GLint size, api::GLenum type, api::GLboolean normalized, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "VertexAttribPointer not available." << std::endl;
}

static void fallbackVertexAttribPointerARB(api::GLuint index, api::GLint size, api::GLenum type, api::GLboolean normalized, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "VertexAttribPointerARB not available." << std::endl;
}

static void fallbackVertexAttribPointerNV(api::GLuint index, api::GLint fsize, api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "VertexAttribPointerNV not available." << std::endl;
}

static void fallbackVertexAttribs1dvNV(api::GLuint index, api::GLsizei count, const api::GLdouble* v)
{
	log() << warning << "VertexAttribs1dvNV not available." << std::endl;
}

static void fallbackVertexAttribs1fvNV(api::GLuint index, api::GLsizei count, const api::GLfloat* v)
{
	log() << warning << "VertexAttribs1fvNV not available." << std::endl;
}

static void fallbackVertexAttribs1hvNV(api::GLuint index, api::GLsizei n, const api::GLhalfNV* v)
{
	log() << warning << "VertexAttribs1hvNV not available." << std::endl;
}

static void fallbackVertexAttribs1svNV(api::GLuint index, api::GLsizei count, const api::GLshort* v)
{
	log() << warning << "VertexAttribs1svNV not available." << std::endl;
}

static void fallbackVertexAttribs2dvNV(api::GLuint index, api::GLsizei count, const api::GLdouble* v)
{
	log() << warning << "VertexAttribs2dvNV not available." << std::endl;
}

static void fallbackVertexAttribs2fvNV(api::GLuint index, api::GLsizei count, const api::GLfloat* v)
{
	log() << warning << "VertexAttribs2fvNV not available." << std::endl;
}

static void fallbackVertexAttribs2hvNV(api::GLuint index, api::GLsizei n, const api::GLhalfNV* v)
{
	log() << warning << "VertexAttribs2hvNV not available." << std::endl;
}

static void fallbackVertexAttribs2svNV(api::GLuint index, api::GLsizei count, const api::GLshort* v)
{
	log() << warning << "VertexAttribs2svNV not available." << std::endl;
}

static void fallbackVertexAttribs3dvNV(api::GLuint index, api::GLsizei count, const api::GLdouble* v)
{
	log() << warning << "VertexAttribs3dvNV not available." << std::endl;
}

static void fallbackVertexAttribs3fvNV(api::GLuint index, api::GLsizei count, const api::GLfloat* v)
{
	log() << warning << "VertexAttribs3fvNV not available." << std::endl;
}

static void fallbackVertexAttribs3hvNV(api::GLuint index, api::GLsizei n, const api::GLhalfNV* v)
{
	log() << warning << "VertexAttribs3hvNV not available." << std::endl;
}

static void fallbackVertexAttribs3svNV(api::GLuint index, api::GLsizei count, const api::GLshort* v)
{
	log() << warning << "VertexAttribs3svNV not available." << std::endl;
}

static void fallbackVertexAttribs4dvNV(api::GLuint index, api::GLsizei count, const api::GLdouble* v)
{
	log() << warning << "VertexAttribs4dvNV not available." << std::endl;
}

static void fallbackVertexAttribs4fvNV(api::GLuint index, api::GLsizei count, const api::GLfloat* v)
{
	log() << warning << "VertexAttribs4fvNV not available." << std::endl;
}

static void fallbackVertexAttribs4hvNV(api::GLuint index, api::GLsizei n, const api::GLhalfNV* v)
{
	log() << warning << "VertexAttribs4hvNV not available." << std::endl;
}

static void fallbackVertexAttribs4svNV(api::GLuint index, api::GLsizei count, const api::GLshort* v)
{
	log() << warning << "VertexAttribs4svNV not available." << std::endl;
}

static void fallbackVertexAttribs4ubvNV(api::GLuint index, api::GLsizei count, const api::GLubyte* v)
{
	log() << warning << "VertexAttribs4ubvNV not available." << std::endl;
}

static void fallbackVertexBlendARB(api::GLint count)
{
	log() << warning << "VertexBlendARB not available." << std::endl;
}

static void fallbackVertexBlendEnvfATI(api::GLenum pname, api::GLfloat param)
{
	log() << warning << "VertexBlendEnvfATI not available." << std::endl;
}

static void fallbackVertexBlendEnviATI(api::GLenum pname, api::GLint param)
{
	log() << warning << "VertexBlendEnviATI not available." << std::endl;
}

static void fallbackVertexFormatNV(api::GLint size, api::GLenum type, api::GLsizei stride)
{
	log() << warning << "VertexFormatNV not available." << std::endl;
}

static void fallbackVertexP2ui(api::GLenum type, api::GLuint value)
{
	log() << warning << "VertexP2ui not available." << std::endl;
}

static void fallbackVertexP2uiv(api::GLenum type, const api::GLuint* value)
{
	log() << warning << "VertexP2uiv not available." << std::endl;
}

static void fallbackVertexP3ui(api::GLenum type, api::GLuint value)
{
	log() << warning << "VertexP3ui not available." << std::endl;
}

static void fallbackVertexP3uiv(api::GLenum type, const api::GLuint* value)
{
	log() << warning << "VertexP3uiv not available." << std::endl;
}

static void fallbackVertexP4ui(api::GLenum type, api::GLuint value)
{
	log() << warning << "VertexP4ui not available." << std::endl;
}

static void fallbackVertexP4uiv(api::GLenum type, const api::GLuint* value)
{
	log() << warning << "VertexP4uiv not available." << std::endl;
}

static void fallbackVertexPointer(api::GLint size, api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "VertexPointer not available." << std::endl;
}

static void fallbackVertexPointerEXT(api::GLint size, api::GLenum type, api::GLsizei stride, api::GLsizei count, const api::GLvoid* pointer)
{
	log() << warning << "VertexPointerEXT not available." << std::endl;
}

static void fallbackVertexPointerListIBM(api::GLint size, api::GLenum type, api::GLint stride, const api::GLvoid** pointer, api::GLint ptrstride)
{
	log() << warning << "VertexPointerListIBM not available." << std::endl;
}

static void fallbackVertexPointervINTEL(api::GLint size, api::GLenum type, const api::GLvoid** pointer)
{
	log() << warning << "VertexPointervINTEL not available." << std::endl;
}

static void fallbackVertexStream1dATI(api::GLenum stream, api::GLdouble x)
{
	log() << warning << "VertexStream1dATI not available." << std::endl;
}

static void fallbackVertexStream1dvATI(api::GLenum stream, const api::GLdouble* coords)
{
	log() << warning << "VertexStream1dvATI not available." << std::endl;
}

static void fallbackVertexStream1fATI(api::GLenum stream, api::GLfloat x)
{
	log() << warning << "VertexStream1fATI not available." << std::endl;
}

static void fallbackVertexStream1fvATI(api::GLenum stream, const api::GLfloat* coords)
{
	log() << warning << "VertexStream1fvATI not available." << std::endl;
}

static void fallbackVertexStream1iATI(api::GLenum stream, api::GLint x)
{
	log() << warning << "VertexStream1iATI not available." << std::endl;
}

static void fallbackVertexStream1ivATI(api::GLenum stream, const api::GLint* coords)
{
	log() << warning << "VertexStream1ivATI not available." << std::endl;
}

static void fallbackVertexStream1sATI(api::GLenum stream, api::GLshort x)
{
	log() << warning << "VertexStream1sATI not available." << std::endl;
}

static void fallbackVertexStream1svATI(api::GLenum stream, const api::GLshort* coords)
{
	log() << warning << "VertexStream1svATI not available." << std::endl;
}

static void fallbackVertexStream2dATI(api::GLenum stream, api::GLdouble x, api::GLdouble y)
{
	log() << warning << "VertexStream2dATI not available." << std::endl;
}

static void fallbackVertexStream2dvATI(api::GLenum stream, const api::GLdouble* coords)
{
	log() << warning << "VertexStream2dvATI not available." << std::endl;
}

static void fallbackVertexStream2fATI(api::GLenum stream, api::GLfloat x, api::GLfloat y)
{
	log() << warning << "VertexStream2fATI not available." << std::endl;
}

static void fallbackVertexStream2fvATI(api::GLenum stream, const api::GLfloat* coords)
{
	log() << warning << "VertexStream2fvATI not available." << std::endl;
}

static void fallbackVertexStream2iATI(api::GLenum stream, api::GLint x, api::GLint y)
{
	log() << warning << "VertexStream2iATI not available." << std::endl;
}

static void fallbackVertexStream2ivATI(api::GLenum stream, const api::GLint* coords)
{
	log() << warning << "VertexStream2ivATI not available." << std::endl;
}

static void fallbackVertexStream2sATI(api::GLenum stream, api::GLshort x, api::GLshort y)
{
	log() << warning << "VertexStream2sATI not available." << std::endl;
}

static void fallbackVertexStream2svATI(api::GLenum stream, const api::GLshort* coords)
{
	log() << warning << "VertexStream2svATI not available." << std::endl;
}

static void fallbackVertexStream3dATI(api::GLenum stream, api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "VertexStream3dATI not available." << std::endl;
}

static void fallbackVertexStream3dvATI(api::GLenum stream, const api::GLdouble* coords)
{
	log() << warning << "VertexStream3dvATI not available." << std::endl;
}

static void fallbackVertexStream3fATI(api::GLenum stream, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "VertexStream3fATI not available." << std::endl;
}

static void fallbackVertexStream3fvATI(api::GLenum stream, const api::GLfloat* coords)
{
	log() << warning << "VertexStream3fvATI not available." << std::endl;
}

static void fallbackVertexStream3iATI(api::GLenum stream, api::GLint x, api::GLint y, api::GLint z)
{
	log() << warning << "VertexStream3iATI not available." << std::endl;
}

static void fallbackVertexStream3ivATI(api::GLenum stream, const api::GLint* coords)
{
	log() << warning << "VertexStream3ivATI not available." << std::endl;
}

static void fallbackVertexStream3sATI(api::GLenum stream, api::GLshort x, api::GLshort y, api::GLshort z)
{
	log() << warning << "VertexStream3sATI not available." << std::endl;
}

static void fallbackVertexStream3svATI(api::GLenum stream, const api::GLshort* coords)
{
	log() << warning << "VertexStream3svATI not available." << std::endl;
}

static void fallbackVertexStream4dATI(api::GLenum stream, api::GLdouble x, api::GLdouble y, api::GLdouble z, api::GLdouble w)
{
	log() << warning << "VertexStream4dATI not available." << std::endl;
}

static void fallbackVertexStream4dvATI(api::GLenum stream, const api::GLdouble* coords)
{
	log() << warning << "VertexStream4dvATI not available." << std::endl;
}

static void fallbackVertexStream4fATI(api::GLenum stream, api::GLfloat x, api::GLfloat y, api::GLfloat z, api::GLfloat w)
{
	log() << warning << "VertexStream4fATI not available." << std::endl;
}

static void fallbackVertexStream4fvATI(api::GLenum stream, const api::GLfloat* coords)
{
	log() << warning << "VertexStream4fvATI not available." << std::endl;
}

static void fallbackVertexStream4iATI(api::GLenum stream, api::GLint x, api::GLint y, api::GLint z, api::GLint w)
{
	log() << warning << "VertexStream4iATI not available." << std::endl;
}

static void fallbackVertexStream4ivATI(api::GLenum stream, const api::GLint* coords)
{
	log() << warning << "VertexStream4ivATI not available." << std::endl;
}

static void fallbackVertexStream4sATI(api::GLenum stream, api::GLshort x, api::GLshort y, api::GLshort z, api::GLshort w)
{
	log() << warning << "VertexStream4sATI not available." << std::endl;
}

static void fallbackVertexStream4svATI(api::GLenum stream, const api::GLshort* coords)
{
	log() << warning << "VertexStream4svATI not available." << std::endl;
}

static void fallbackVertexWeightPointerEXT(api::GLsizei size, api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "VertexWeightPointerEXT not available." << std::endl;
}

static void fallbackVertexWeightfEXT(api::GLfloat weight)
{
	log() << warning << "VertexWeightfEXT not available." << std::endl;
}

static void fallbackVertexWeightfvEXT(const api::GLfloat* weight)
{
	log() << warning << "VertexWeightfvEXT not available." << std::endl;
}

static void fallbackVertexWeighthNV(api::GLhalfNV weight)
{
	log() << warning << "VertexWeighthNV not available." << std::endl;
}

static void fallbackVertexWeighthvNV(const api::GLhalfNV* weight)
{
	log() << warning << "VertexWeighthvNV not available." << std::endl;
}

static api::GLenum fallbackVideoCaptureNV(api::GLuint video_capture_slot)
{
	log() << warning << "VideoCaptureNV not available." << std::endl;
}

static void fallbackVideoCaptureStreamParameterdvNV(api::GLuint video_capture_slot, api::GLuint stream, api::GLenum pname, const api::GLdouble* params)
{
	log() << warning << "VideoCaptureStreamParameterdvNV not available." << std::endl;
}

static void fallbackVideoCaptureStreamParameterfvNV(api::GLuint video_capture_slot, api::GLuint stream, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "VideoCaptureStreamParameterfvNV not available." << std::endl;
}

static void fallbackVideoCaptureStreamParameterivNV(api::GLuint video_capture_slot, api::GLuint stream, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "VideoCaptureStreamParameterivNV not available." << std::endl;
}

static void fallbackViewport(api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "Viewport not available." << std::endl;
}

static void fallbackWaitSync(api::GLsync sync, api::GLbitfield flags, api::GLuint64 timeout)
{
	log() << warning << "WaitSync not available." << std::endl;
}

static void fallbackWeightPointerARB(api::GLint size, api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "WeightPointerARB not available." << std::endl;
}

static void fallbackWeightbvARB(api::GLint size, const api::GLbyte* weights)
{
	log() << warning << "WeightbvARB not available." << std::endl;
}

static void fallbackWeightdvARB(api::GLint size, const api::GLdouble* weights)
{
	log() << warning << "WeightdvARB not available." << std::endl;
}

static void fallbackWeightfvARB(api::GLint size, const api::GLfloat* weights)
{
	log() << warning << "WeightfvARB not available." << std::endl;
}

static void fallbackWeightivARB(api::GLint size, const api::GLint* weights)
{
	log() << warning << "WeightivARB not available." << std::endl;
}

static void fallbackWeightsvARB(api::GLint size, const api::GLshort* weights)
{
	log() << warning << "WeightsvARB not available." << std::endl;
}

static void fallbackWeightubvARB(api::GLint size, const api::GLubyte* weights)
{
	log() << warning << "WeightubvARB not available." << std::endl;
}

static void fallbackWeightuivARB(api::GLint size, const api::GLuint* weights)
{
	log() << warning << "WeightuivARB not available." << std::endl;
}

static void fallbackWeightusvARB(api::GLint size, const api::GLushort* weights)
{
	log() << warning << "WeightusvARB not available." << std::endl;
}

static void fallbackWindowPos2d(api::GLdouble x, api::GLdouble y)
{
	log() << warning << "WindowPos2d not available." << std::endl;
}

static void fallbackWindowPos2dARB(api::GLdouble x, api::GLdouble y)
{
	log() << warning << "WindowPos2dARB not available." << std::endl;
}

static void fallbackWindowPos2dMESA(api::GLdouble x, api::GLdouble y)
{
	log() << warning << "WindowPos2dMESA not available." << std::endl;
}

static void fallbackWindowPos2dv(const api::GLdouble* v)
{
	log() << warning << "WindowPos2dv not available." << std::endl;
}

static void fallbackWindowPos2dvARB(const api::GLdouble* v)
{
	log() << warning << "WindowPos2dvARB not available." << std::endl;
}

static void fallbackWindowPos2dvMESA(const api::GLdouble* v)
{
	log() << warning << "WindowPos2dvMESA not available." << std::endl;
}

static void fallbackWindowPos2f(api::GLfloat x, api::GLfloat y)
{
	log() << warning << "WindowPos2f not available." << std::endl;
}

static void fallbackWindowPos2fARB(api::GLfloat x, api::GLfloat y)
{
	log() << warning << "WindowPos2fARB not available." << std::endl;
}

static void fallbackWindowPos2fMESA(api::GLfloat x, api::GLfloat y)
{
	log() << warning << "WindowPos2fMESA not available." << std::endl;
}

static void fallbackWindowPos2fv(const api::GLfloat* v)
{
	log() << warning << "WindowPos2fv not available." << std::endl;
}

static void fallbackWindowPos2fvARB(const api::GLfloat* v)
{
	log() << warning << "WindowPos2fvARB not available." << std::endl;
}

static void fallbackWindowPos2fvMESA(const api::GLfloat* v)
{
	log() << warning << "WindowPos2fvMESA not available." << std::endl;
}

static void fallbackWindowPos2i(api::GLint x, api::GLint y)
{
	log() << warning << "WindowPos2i not available." << std::endl;
}

static void fallbackWindowPos2iARB(api::GLint x, api::GLint y)
{
	log() << warning << "WindowPos2iARB not available." << std::endl;
}

static void fallbackWindowPos2iMESA(api::GLint x, api::GLint y)
{
	log() << warning << "WindowPos2iMESA not available." << std::endl;
}

static void fallbackWindowPos2iv(const api::GLint* v)
{
	log() << warning << "WindowPos2iv not available." << std::endl;
}

static void fallbackWindowPos2ivARB(const api::GLint* v)
{
	log() << warning << "WindowPos2ivARB not available." << std::endl;
}

static void fallbackWindowPos2ivMESA(const api::GLint* v)
{
	log() << warning << "WindowPos2ivMESA not available." << std::endl;
}

static void fallbackWindowPos2s(api::GLshort x, api::GLshort y)
{
	log() << warning << "WindowPos2s not available." << std::endl;
}

static void fallbackWindowPos2sARB(api::GLshort x, api::GLshort y)
{
	log() << warning << "WindowPos2sARB not available." << std::endl;
}

static void fallbackWindowPos2sMESA(api::GLshort x, api::GLshort y)
{
	log() << warning << "WindowPos2sMESA not available." << std::endl;
}

static void fallbackWindowPos2sv(const api::GLshort* v)
{
	log() << warning << "WindowPos2sv not available." << std::endl;
}

static void fallbackWindowPos2svARB(const api::GLshort* v)
{
	log() << warning << "WindowPos2svARB not available." << std::endl;
}

static void fallbackWindowPos2svMESA(const api::GLshort* v)
{
	log() << warning << "WindowPos2svMESA not available." << std::endl;
}

static void fallbackWindowPos3d(api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "WindowPos3d not available." << std::endl;
}

static void fallbackWindowPos3dARB(api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "WindowPos3dARB not available." << std::endl;
}

static void fallbackWindowPos3dMESA(api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "WindowPos3dMESA not available." << std::endl;
}

static void fallbackWindowPos3dv(const api::GLdouble* v)
{
	log() << warning << "WindowPos3dv not available." << std::endl;
}

static void fallbackWindowPos3dvARB(const api::GLdouble* v)
{
	log() << warning << "WindowPos3dvARB not available." << std::endl;
}

static void fallbackWindowPos3dvMESA(const api::GLdouble* v)
{
	log() << warning << "WindowPos3dvMESA not available." << std::endl;
}

static void fallbackWindowPos3f(api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "WindowPos3f not available." << std::endl;
}

static void fallbackWindowPos3fARB(api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "WindowPos3fARB not available." << std::endl;
}

static void fallbackWindowPos3fMESA(api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "WindowPos3fMESA not available." << std::endl;
}

static void fallbackWindowPos3fv(const api::GLfloat* v)
{
	log() << warning << "WindowPos3fv not available." << std::endl;
}

static void fallbackWindowPos3fvARB(const api::GLfloat* v)
{
	log() << warning << "WindowPos3fvARB not available." << std::endl;
}

static void fallbackWindowPos3fvMESA(const api::GLfloat* v)
{
	log() << warning << "WindowPos3fvMESA not available." << std::endl;
}

static void fallbackWindowPos3i(api::GLint x, api::GLint y, api::GLint z)
{
	log() << warning << "WindowPos3i not available." << std::endl;
}

static void fallbackWindowPos3iARB(api::GLint x, api::GLint y, api::GLint z)
{
	log() << warning << "WindowPos3iARB not available." << std::endl;
}

static void fallbackWindowPos3iMESA(api::GLint x, api::GLint y, api::GLint z)
{
	log() << warning << "WindowPos3iMESA not available." << std::endl;
}

static void fallbackWindowPos3iv(const api::GLint* v)
{
	log() << warning << "WindowPos3iv not available." << std::endl;
}

static void fallbackWindowPos3ivARB(const api::GLint* v)
{
	log() << warning << "WindowPos3ivARB not available." << std::endl;
}

static void fallbackWindowPos3ivMESA(const api::GLint* v)
{
	log() << warning << "WindowPos3ivMESA not available." << std::endl;
}

static void fallbackWindowPos3s(api::GLshort x, api::GLshort y, api::GLshort z)
{
	log() << warning << "WindowPos3s not available." << std::endl;
}

static void fallbackWindowPos3sARB(api::GLshort x, api::GLshort y, api::GLshort z)
{
	log() << warning << "WindowPos3sARB not available." << std::endl;
}

static void fallbackWindowPos3sMESA(api::GLshort x, api::GLshort y, api::GLshort z)
{
	log() << warning << "WindowPos3sMESA not available." << std::endl;
}

static void fallbackWindowPos3sv(const api::GLshort* v)
{
	log() << warning << "WindowPos3sv not available." << std::endl;
}

static void fallbackWindowPos3svARB(const api::GLshort* v)
{
	log() << warning << "WindowPos3svARB not available." << std::endl;
}

static void fallbackWindowPos3svMESA(const api::GLshort* v)
{
	log() << warning << "WindowPos3svMESA not available." << std::endl;
}

static void fallbackWindowPos4dMESA(api::GLdouble x, api::GLdouble y, api::GLdouble z, api::GLdouble w)
{
	log() << warning << "WindowPos4dMESA not available." << std::endl;
}

static void fallbackWindowPos4dvMESA(const api::GLdouble* v)
{
	log() << warning << "WindowPos4dvMESA not available." << std::endl;
}

static void fallbackWindowPos4fMESA(api::GLfloat x, api::GLfloat y, api::GLfloat z, api::GLfloat w)
{
	log() << warning << "WindowPos4fMESA not available." << std::endl;
}

static void fallbackWindowPos4fvMESA(const api::GLfloat* v)
{
	log() << warning << "WindowPos4fvMESA not available." << std::endl;
}

static void fallbackWindowPos4iMESA(api::GLint x, api::GLint y, api::GLint z, api::GLint w)
{
	log() << warning << "WindowPos4iMESA not available." << std::endl;
}

static void fallbackWindowPos4ivMESA(const api::GLint* v)
{
	log() << warning << "WindowPos4ivMESA not available." << std::endl;
}

static void fallbackWindowPos4sMESA(api::GLshort x, api::GLshort y, api::GLshort z, api::GLshort w)
{
	log() << warning << "WindowPos4sMESA not available." << std::endl;
}

static void fallbackWindowPos4svMESA(const api::GLshort* v)
{
	log() << warning << "WindowPos4svMESA not available." << std::endl;
}

static void fallbackWriteMaskEXT(api::GLuint res, api::GLuint in, api::GLenum outX, api::GLenum outY, api::GLenum outZ, api::GLenum outW)
{
	log() << warning << "WriteMaskEXT not available." << std::endl;
}

api::api() :
	Accum(fallbackAccum),
	ActiveProgramEXT(fallbackActiveProgramEXT),
	ActiveStencilFaceEXT(fallbackActiveStencilFaceEXT),
	ActiveTexture(fallbackActiveTexture),
	ActiveTextureARB(fallbackActiveTextureARB),
	ActiveVaryingNV(fallbackActiveVaryingNV),
	AlphaFragmentOp1ATI(fallbackAlphaFragmentOp1ATI),
	AlphaFragmentOp2ATI(fallbackAlphaFragmentOp2ATI),
	AlphaFragmentOp3ATI(fallbackAlphaFragmentOp3ATI),
	AlphaFunc(fallbackAlphaFunc),
	ApplyTextureEXT(fallbackApplyTextureEXT),
	AreProgramsResidentNV(fallbackAreProgramsResidentNV),
	AreTexturesResident(fallbackAreTexturesResident),
	AreTexturesResidentEXT(fallbackAreTexturesResidentEXT),
	ArrayElement(fallbackArrayElement),
	ArrayElementEXT(fallbackArrayElementEXT),
	ArrayObjectATI(fallbackArrayObjectATI),
	AsyncMarkerSGIX(fallbackAsyncMarkerSGIX),
	AttachObjectARB(fallbackAttachObjectARB),
	AttachShader(fallbackAttachShader),
	Begin(fallbackBegin),
	BeginConditionalRender(fallbackBeginConditionalRender),
	BeginConditionalRenderNV(fallbackBeginConditionalRenderNV),
	BeginFragmentShaderATI(fallbackBeginFragmentShaderATI),
	BeginOcclusionQueryNV(fallbackBeginOcclusionQueryNV),
	BeginPerfMonitorAMD(fallbackBeginPerfMonitorAMD),
	BeginQuery(fallbackBeginQuery),
	BeginQueryARB(fallbackBeginQueryARB),
	BeginQueryIndexed(fallbackBeginQueryIndexed),
	BeginTransformFeedback(fallbackBeginTransformFeedback),
	BeginTransformFeedbackEXT(fallbackBeginTransformFeedbackEXT),
	BeginTransformFeedbackNV(fallbackBeginTransformFeedbackNV),
	BeginVertexShaderEXT(fallbackBeginVertexShaderEXT),
	BeginVideoCaptureNV(fallbackBeginVideoCaptureNV),
	BindAttribLocation(fallbackBindAttribLocation),
	BindAttribLocationARB(fallbackBindAttribLocationARB),
	BindBuffer(fallbackBindBuffer),
	BindBufferARB(fallbackBindBufferARB),
	BindBufferBase(fallbackBindBufferBase),
	BindBufferBaseEXT(fallbackBindBufferBaseEXT),
	BindBufferBaseNV(fallbackBindBufferBaseNV),
	BindBufferOffsetEXT(fallbackBindBufferOffsetEXT),
	BindBufferOffsetNV(fallbackBindBufferOffsetNV),
	BindBufferRange(fallbackBindBufferRange),
	BindBufferRangeEXT(fallbackBindBufferRangeEXT),
	BindBufferRangeNV(fallbackBindBufferRangeNV),
	BindFragDataLocation(fallbackBindFragDataLocation),
	BindFragDataLocationEXT(fallbackBindFragDataLocationEXT),
	BindFragDataLocationIndexed(fallbackBindFragDataLocationIndexed),
	BindFragmentShaderATI(fallbackBindFragmentShaderATI),
	BindFramebuffer(fallbackBindFramebuffer),
	BindFramebufferEXT(fallbackBindFramebufferEXT),
	BindLightParameterEXT(fallbackBindLightParameterEXT),
	BindMaterialParameterEXT(fallbackBindMaterialParameterEXT),
	BindMultiTextureEXT(fallbackBindMultiTextureEXT),
	BindParameterEXT(fallbackBindParameterEXT),
	BindProgramARB(fallbackBindProgramARB),
	BindProgramNV(fallbackBindProgramNV),
	BindRenderbuffer(fallbackBindRenderbuffer),
	BindRenderbufferEXT(fallbackBindRenderbufferEXT),
	BindSampler(fallbackBindSampler),
	BindTexGenParameterEXT(fallbackBindTexGenParameterEXT),
	BindTexture(fallbackBindTexture),
	BindTextureEXT(fallbackBindTextureEXT),
	BindTextureUnitParameterEXT(fallbackBindTextureUnitParameterEXT),
	BindTransformFeedback(fallbackBindTransformFeedback),
	BindTransformFeedbackNV(fallbackBindTransformFeedbackNV),
	BindVertexArray(fallbackBindVertexArray),
	BindVertexArrayAPPLE(fallbackBindVertexArrayAPPLE),
	BindVertexShaderEXT(fallbackBindVertexShaderEXT),
	BindVideoCaptureStreamBufferNV(fallbackBindVideoCaptureStreamBufferNV),
	BindVideoCaptureStreamTextureNV(fallbackBindVideoCaptureStreamTextureNV),
	Binormal3bEXT(fallbackBinormal3bEXT),
	Binormal3bvEXT(fallbackBinormal3bvEXT),
	Binormal3dEXT(fallbackBinormal3dEXT),
	Binormal3dvEXT(fallbackBinormal3dvEXT),
	Binormal3fEXT(fallbackBinormal3fEXT),
	Binormal3fvEXT(fallbackBinormal3fvEXT),
	Binormal3iEXT(fallbackBinormal3iEXT),
	Binormal3ivEXT(fallbackBinormal3ivEXT),
	Binormal3sEXT(fallbackBinormal3sEXT),
	Binormal3svEXT(fallbackBinormal3svEXT),
	BinormalPointerEXT(fallbackBinormalPointerEXT),
	Bitmap(fallbackBitmap),
	BlendColor(fallbackBlendColor),
	BlendColorEXT(fallbackBlendColorEXT),
	BlendEquation(fallbackBlendEquation),
	BlendEquationEXT(fallbackBlendEquationEXT),
	BlendEquationIndexedAMD(fallbackBlendEquationIndexedAMD),
	BlendEquationSeparate(fallbackBlendEquationSeparate),
	BlendEquationSeparateEXT(fallbackBlendEquationSeparateEXT),
	BlendEquationSeparateIndexedAMD(fallbackBlendEquationSeparateIndexedAMD),
	BlendEquationSeparatei(fallbackBlendEquationSeparatei),
	BlendEquationi(fallbackBlendEquationi),
	BlendFunc(fallbackBlendFunc),
	BlendFuncIndexedAMD(fallbackBlendFuncIndexedAMD),
	BlendFuncSeparate(fallbackBlendFuncSeparate),
	BlendFuncSeparateEXT(fallbackBlendFuncSeparateEXT),
	BlendFuncSeparateINGR(fallbackBlendFuncSeparateINGR),
	BlendFuncSeparateIndexedAMD(fallbackBlendFuncSeparateIndexedAMD),
	BlendFuncSeparatei(fallbackBlendFuncSeparatei),
	BlendFunci(fallbackBlendFunci),
	BlitFramebuffer(fallbackBlitFramebuffer),
	BlitFramebufferEXT(fallbackBlitFramebufferEXT),
	BufferAddressRangeNV(fallbackBufferAddressRangeNV),
	BufferData(fallbackBufferData),
	BufferDataARB(fallbackBufferDataARB),
	BufferParameteriAPPLE(fallbackBufferParameteriAPPLE),
	BufferSubData(fallbackBufferSubData),
	BufferSubDataARB(fallbackBufferSubDataARB),
	CallList(fallbackCallList),
	CallLists(fallbackCallLists),
	CheckFramebufferStatus(fallbackCheckFramebufferStatus),
	CheckFramebufferStatusEXT(fallbackCheckFramebufferStatusEXT),
	CheckNamedFramebufferStatusEXT(fallbackCheckNamedFramebufferStatusEXT),
	ClampColor(fallbackClampColor),
	ClampColorARB(fallbackClampColorARB),
	Clear(fallbackClear),
	ClearAccum(fallbackClearAccum),
	ClearBufferfi(fallbackClearBufferfi),
	ClearBufferfv(fallbackClearBufferfv),
	ClearBufferiv(fallbackClearBufferiv),
	ClearBufferuiv(fallbackClearBufferuiv),
	ClearColor(fallbackClearColor),
	ClearColorIiEXT(fallbackClearColorIiEXT),
	ClearColorIuiEXT(fallbackClearColorIuiEXT),
	ClearDepth(fallbackClearDepth),
	ClearDepthdNV(fallbackClearDepthdNV),
	ClearIndex(fallbackClearIndex),
	ClearStencil(fallbackClearStencil),
	ClientActiveTexture(fallbackClientActiveTexture),
	ClientActiveTextureARB(fallbackClientActiveTextureARB),
	ClientActiveVertexStreamATI(fallbackClientActiveVertexStreamATI),
	ClientAttribDefaultEXT(fallbackClientAttribDefaultEXT),
	ClientWaitSync(fallbackClientWaitSync),
	ClipPlane(fallbackClipPlane),
	Color3b(fallbackColor3b),
	Color3bv(fallbackColor3bv),
	Color3d(fallbackColor3d),
	Color3dv(fallbackColor3dv),
	Color3f(fallbackColor3f),
	Color3fVertex3fSUN(fallbackColor3fVertex3fSUN),
	Color3fVertex3fvSUN(fallbackColor3fVertex3fvSUN),
	Color3fv(fallbackColor3fv),
	Color3hNV(fallbackColor3hNV),
	Color3hvNV(fallbackColor3hvNV),
	Color3i(fallbackColor3i),
	Color3iv(fallbackColor3iv),
	Color3s(fallbackColor3s),
	Color3sv(fallbackColor3sv),
	Color3ub(fallbackColor3ub),
	Color3ubv(fallbackColor3ubv),
	Color3ui(fallbackColor3ui),
	Color3uiv(fallbackColor3uiv),
	Color3us(fallbackColor3us),
	Color3usv(fallbackColor3usv),
	Color4b(fallbackColor4b),
	Color4bv(fallbackColor4bv),
	Color4d(fallbackColor4d),
	Color4dv(fallbackColor4dv),
	Color4f(fallbackColor4f),
	Color4fNormal3fVertex3fSUN(fallbackColor4fNormal3fVertex3fSUN),
	Color4fNormal3fVertex3fvSUN(fallbackColor4fNormal3fVertex3fvSUN),
	Color4fv(fallbackColor4fv),
	Color4hNV(fallbackColor4hNV),
	Color4hvNV(fallbackColor4hvNV),
	Color4i(fallbackColor4i),
	Color4iv(fallbackColor4iv),
	Color4s(fallbackColor4s),
	Color4sv(fallbackColor4sv),
	Color4ub(fallbackColor4ub),
	Color4ubVertex2fSUN(fallbackColor4ubVertex2fSUN),
	Color4ubVertex2fvSUN(fallbackColor4ubVertex2fvSUN),
	Color4ubVertex3fSUN(fallbackColor4ubVertex3fSUN),
	Color4ubVertex3fvSUN(fallbackColor4ubVertex3fvSUN),
	Color4ubv(fallbackColor4ubv),
	Color4ui(fallbackColor4ui),
	Color4uiv(fallbackColor4uiv),
	Color4us(fallbackColor4us),
	Color4usv(fallbackColor4usv),
	ColorFormatNV(fallbackColorFormatNV),
	ColorFragmentOp1ATI(fallbackColorFragmentOp1ATI),
	ColorFragmentOp2ATI(fallbackColorFragmentOp2ATI),
	ColorFragmentOp3ATI(fallbackColorFragmentOp3ATI),
	ColorMask(fallbackColorMask),
	ColorMaskIndexedEXT(fallbackColorMaskIndexedEXT),
	ColorMaski(fallbackColorMaski),
	ColorMaterial(fallbackColorMaterial),
	ColorP3ui(fallbackColorP3ui),
	ColorP3uiv(fallbackColorP3uiv),
	ColorP4ui(fallbackColorP4ui),
	ColorP4uiv(fallbackColorP4uiv),
	ColorPointer(fallbackColorPointer),
	ColorPointerEXT(fallbackColorPointerEXT),
	ColorPointerListIBM(fallbackColorPointerListIBM),
	ColorPointervINTEL(fallbackColorPointervINTEL),
	ColorSubTable(fallbackColorSubTable),
	ColorSubTableEXT(fallbackColorSubTableEXT),
	ColorTable(fallbackColorTable),
	ColorTableEXT(fallbackColorTableEXT),
	ColorTableParameterfv(fallbackColorTableParameterfv),
	ColorTableParameterfvSGI(fallbackColorTableParameterfvSGI),
	ColorTableParameteriv(fallbackColorTableParameteriv),
	ColorTableParameterivSGI(fallbackColorTableParameterivSGI),
	ColorTableSGI(fallbackColorTableSGI),
	CombinerInputNV(fallbackCombinerInputNV),
	CombinerOutputNV(fallbackCombinerOutputNV),
	CombinerParameterfNV(fallbackCombinerParameterfNV),
	CombinerParameterfvNV(fallbackCombinerParameterfvNV),
	CombinerParameteriNV(fallbackCombinerParameteriNV),
	CombinerParameterivNV(fallbackCombinerParameterivNV),
	CombinerStageParameterfvNV(fallbackCombinerStageParameterfvNV),
	CompileShader(fallbackCompileShader),
	CompileShaderARB(fallbackCompileShaderARB),
	CompileShaderIncludeARB(fallbackCompileShaderIncludeARB),
	CompressedMultiTexImage1DEXT(fallbackCompressedMultiTexImage1DEXT),
	CompressedMultiTexImage2DEXT(fallbackCompressedMultiTexImage2DEXT),
	CompressedMultiTexImage3DEXT(fallbackCompressedMultiTexImage3DEXT),
	CompressedMultiTexSubImage1DEXT(fallbackCompressedMultiTexSubImage1DEXT),
	CompressedMultiTexSubImage2DEXT(fallbackCompressedMultiTexSubImage2DEXT),
	CompressedMultiTexSubImage3DEXT(fallbackCompressedMultiTexSubImage3DEXT),
	CompressedTexImage1D(fallbackCompressedTexImage1D),
	CompressedTexImage1DARB(fallbackCompressedTexImage1DARB),
	CompressedTexImage2D(fallbackCompressedTexImage2D),
	CompressedTexImage2DARB(fallbackCompressedTexImage2DARB),
	CompressedTexImage3D(fallbackCompressedTexImage3D),
	CompressedTexImage3DARB(fallbackCompressedTexImage3DARB),
	CompressedTexSubImage1D(fallbackCompressedTexSubImage1D),
	CompressedTexSubImage1DARB(fallbackCompressedTexSubImage1DARB),
	CompressedTexSubImage2D(fallbackCompressedTexSubImage2D),
	CompressedTexSubImage2DARB(fallbackCompressedTexSubImage2DARB),
	CompressedTexSubImage3D(fallbackCompressedTexSubImage3D),
	CompressedTexSubImage3DARB(fallbackCompressedTexSubImage3DARB),
	CompressedTextureImage1DEXT(fallbackCompressedTextureImage1DEXT),
	CompressedTextureImage2DEXT(fallbackCompressedTextureImage2DEXT),
	CompressedTextureImage3DEXT(fallbackCompressedTextureImage3DEXT),
	CompressedTextureSubImage1DEXT(fallbackCompressedTextureSubImage1DEXT),
	CompressedTextureSubImage2DEXT(fallbackCompressedTextureSubImage2DEXT),
	CompressedTextureSubImage3DEXT(fallbackCompressedTextureSubImage3DEXT),
	ConvolutionFilter1D(fallbackConvolutionFilter1D),
	ConvolutionFilter1DEXT(fallbackConvolutionFilter1DEXT),
	ConvolutionFilter2D(fallbackConvolutionFilter2D),
	ConvolutionFilter2DEXT(fallbackConvolutionFilter2DEXT),
	ConvolutionParameterf(fallbackConvolutionParameterf),
	ConvolutionParameterfEXT(fallbackConvolutionParameterfEXT),
	ConvolutionParameterfv(fallbackConvolutionParameterfv),
	ConvolutionParameterfvEXT(fallbackConvolutionParameterfvEXT),
	ConvolutionParameteri(fallbackConvolutionParameteri),
	ConvolutionParameteriEXT(fallbackConvolutionParameteriEXT),
	ConvolutionParameteriv(fallbackConvolutionParameteriv),
	ConvolutionParameterivEXT(fallbackConvolutionParameterivEXT),
	CopyBufferSubData(fallbackCopyBufferSubData),
	CopyColorSubTable(fallbackCopyColorSubTable),
	CopyColorSubTableEXT(fallbackCopyColorSubTableEXT),
	CopyColorTable(fallbackCopyColorTable),
	CopyColorTableSGI(fallbackCopyColorTableSGI),
	CopyConvolutionFilter1D(fallbackCopyConvolutionFilter1D),
	CopyConvolutionFilter1DEXT(fallbackCopyConvolutionFilter1DEXT),
	CopyConvolutionFilter2D(fallbackCopyConvolutionFilter2D),
	CopyConvolutionFilter2DEXT(fallbackCopyConvolutionFilter2DEXT),
	CopyImageSubDataNV(fallbackCopyImageSubDataNV),
	CopyMultiTexImage1DEXT(fallbackCopyMultiTexImage1DEXT),
	CopyMultiTexImage2DEXT(fallbackCopyMultiTexImage2DEXT),
	CopyMultiTexSubImage1DEXT(fallbackCopyMultiTexSubImage1DEXT),
	CopyMultiTexSubImage2DEXT(fallbackCopyMultiTexSubImage2DEXT),
	CopyMultiTexSubImage3DEXT(fallbackCopyMultiTexSubImage3DEXT),
	CopyPixels(fallbackCopyPixels),
	CopyTexImage1D(fallbackCopyTexImage1D),
	CopyTexImage1DEXT(fallbackCopyTexImage1DEXT),
	CopyTexImage2D(fallbackCopyTexImage2D),
	CopyTexImage2DEXT(fallbackCopyTexImage2DEXT),
	CopyTexSubImage1D(fallbackCopyTexSubImage1D),
	CopyTexSubImage1DEXT(fallbackCopyTexSubImage1DEXT),
	CopyTexSubImage2D(fallbackCopyTexSubImage2D),
	CopyTexSubImage2DEXT(fallbackCopyTexSubImage2DEXT),
	CopyTexSubImage3D(fallbackCopyTexSubImage3D),
	CopyTexSubImage3DEXT(fallbackCopyTexSubImage3DEXT),
	CopyTextureImage1DEXT(fallbackCopyTextureImage1DEXT),
	CopyTextureImage2DEXT(fallbackCopyTextureImage2DEXT),
	CopyTextureSubImage1DEXT(fallbackCopyTextureSubImage1DEXT),
	CopyTextureSubImage2DEXT(fallbackCopyTextureSubImage2DEXT),
	CopyTextureSubImage3DEXT(fallbackCopyTextureSubImage3DEXT),
	CreateProgram(fallbackCreateProgram),
	CreateProgramObjectARB(fallbackCreateProgramObjectARB),
	CreateShader(fallbackCreateShader),
	CreateShaderObjectARB(fallbackCreateShaderObjectARB),
	CreateShaderProgramEXT(fallbackCreateShaderProgramEXT),
	CullFace(fallbackCullFace),
	CullParameterdvEXT(fallbackCullParameterdvEXT),
	CullParameterfvEXT(fallbackCullParameterfvEXT),
	CurrentPaletteMatrixARB(fallbackCurrentPaletteMatrixARB),
	DeformSGIX(fallbackDeformSGIX),
	DeformationMap3dSGIX(fallbackDeformationMap3dSGIX),
	DeformationMap3fSGIX(fallbackDeformationMap3fSGIX),
	DeleteAsyncMarkersSGIX(fallbackDeleteAsyncMarkersSGIX),
	DeleteBuffers(fallbackDeleteBuffers),
	DeleteBuffersARB(fallbackDeleteBuffersARB),
	DeleteFencesAPPLE(fallbackDeleteFencesAPPLE),
	DeleteFencesNV(fallbackDeleteFencesNV),
	DeleteFragmentShaderATI(fallbackDeleteFragmentShaderATI),
	DeleteFramebuffers(fallbackDeleteFramebuffers),
	DeleteFramebuffersEXT(fallbackDeleteFramebuffersEXT),
	DeleteLists(fallbackDeleteLists),
	DeleteNamedStringARB(fallbackDeleteNamedStringARB),
	DeleteObjectARB(fallbackDeleteObjectARB),
	DeleteOcclusionQueriesNV(fallbackDeleteOcclusionQueriesNV),
	DeletePerfMonitorsAMD(fallbackDeletePerfMonitorsAMD),
	DeleteProgram(fallbackDeleteProgram),
	DeleteProgramsARB(fallbackDeleteProgramsARB),
	DeleteProgramsNV(fallbackDeleteProgramsNV),
	DeleteQueries(fallbackDeleteQueries),
	DeleteQueriesARB(fallbackDeleteQueriesARB),
	DeleteRenderbuffers(fallbackDeleteRenderbuffers),
	DeleteRenderbuffersEXT(fallbackDeleteRenderbuffersEXT),
	DeleteSamplers(fallbackDeleteSamplers),
	DeleteShader(fallbackDeleteShader),
	DeleteSync(fallbackDeleteSync),
	DeleteTextures(fallbackDeleteTextures),
	DeleteTexturesEXT(fallbackDeleteTexturesEXT),
	DeleteTransformFeedbacks(fallbackDeleteTransformFeedbacks),
	DeleteTransformFeedbacksNV(fallbackDeleteTransformFeedbacksNV),
	DeleteVertexArrays(fallbackDeleteVertexArrays),
	DeleteVertexArraysAPPLE(fallbackDeleteVertexArraysAPPLE),
	DeleteVertexShaderEXT(fallbackDeleteVertexShaderEXT),
	DepthBoundsEXT(fallbackDepthBoundsEXT),
	DepthBoundsdNV(fallbackDepthBoundsdNV),
	DepthFunc(fallbackDepthFunc),
	DepthMask(fallbackDepthMask),
	DepthRange(fallbackDepthRange),
	DepthRangedNV(fallbackDepthRangedNV),
	DetachObjectARB(fallbackDetachObjectARB),
	DetachShader(fallbackDetachShader),
	DetailTexFuncSGIS(fallbackDetailTexFuncSGIS),
	Disable(fallbackDisable),
	DisableClientState(fallbackDisableClientState),
	DisableClientStateIndexedEXT(fallbackDisableClientStateIndexedEXT),
	DisableIndexedEXT(fallbackDisableIndexedEXT),
	DisableVariantClientStateEXT(fallbackDisableVariantClientStateEXT),
	DisableVertexAttribAPPLE(fallbackDisableVertexAttribAPPLE),
	DisableVertexAttribArray(fallbackDisableVertexAttribArray),
	DisableVertexAttribArrayARB(fallbackDisableVertexAttribArrayARB),
	Disablei(fallbackDisablei),
	DrawArrays(fallbackDrawArrays),
	DrawArraysEXT(fallbackDrawArraysEXT),
	DrawArraysIndirect(fallbackDrawArraysIndirect),
	DrawArraysInstanced(fallbackDrawArraysInstanced),
	DrawArraysInstancedARB(fallbackDrawArraysInstancedARB),
	DrawArraysInstancedEXT(fallbackDrawArraysInstancedEXT),
	DrawBuffer(fallbackDrawBuffer),
	DrawBuffers(fallbackDrawBuffers),
	DrawBuffersARB(fallbackDrawBuffersARB),
	DrawBuffersATI(fallbackDrawBuffersATI),
	DrawElementArrayAPPLE(fallbackDrawElementArrayAPPLE),
	DrawElementArrayATI(fallbackDrawElementArrayATI),
	DrawElements(fallbackDrawElements),
	DrawElementsBaseVertex(fallbackDrawElementsBaseVertex),
	DrawElementsIndirect(fallbackDrawElementsIndirect),
	DrawElementsInstanced(fallbackDrawElementsInstanced),
	DrawElementsInstancedARB(fallbackDrawElementsInstancedARB),
	DrawElementsInstancedBaseVertex(fallbackDrawElementsInstancedBaseVertex),
	DrawElementsInstancedEXT(fallbackDrawElementsInstancedEXT),
	DrawMeshArraysSUN(fallbackDrawMeshArraysSUN),
	DrawPixels(fallbackDrawPixels),
	DrawRangeElementArrayAPPLE(fallbackDrawRangeElementArrayAPPLE),
	DrawRangeElementArrayATI(fallbackDrawRangeElementArrayATI),
	DrawRangeElements(fallbackDrawRangeElements),
	DrawRangeElementsBaseVertex(fallbackDrawRangeElementsBaseVertex),
	DrawRangeElementsEXT(fallbackDrawRangeElementsEXT),
	DrawTransformFeedback(fallbackDrawTransformFeedback),
	DrawTransformFeedbackNV(fallbackDrawTransformFeedbackNV),
	DrawTransformFeedbackStream(fallbackDrawTransformFeedbackStream),
	EdgeFlag(fallbackEdgeFlag),
	EdgeFlagFormatNV(fallbackEdgeFlagFormatNV),
	EdgeFlagPointer(fallbackEdgeFlagPointer),
	EdgeFlagPointerEXT(fallbackEdgeFlagPointerEXT),
	EdgeFlagPointerListIBM(fallbackEdgeFlagPointerListIBM),
	EdgeFlagv(fallbackEdgeFlagv),
	ElementPointerAPPLE(fallbackElementPointerAPPLE),
	ElementPointerATI(fallbackElementPointerATI),
	Enable(fallbackEnable),
	EnableClientState(fallbackEnableClientState),
	EnableClientStateIndexedEXT(fallbackEnableClientStateIndexedEXT),
	EnableIndexedEXT(fallbackEnableIndexedEXT),
	EnableVariantClientStateEXT(fallbackEnableVariantClientStateEXT),
	EnableVertexAttribAPPLE(fallbackEnableVertexAttribAPPLE),
	EnableVertexAttribArray(fallbackEnableVertexAttribArray),
	EnableVertexAttribArrayARB(fallbackEnableVertexAttribArrayARB),
	Enablei(fallbackEnablei),
	End(fallbackEnd),
	EndConditionalRender(fallbackEndConditionalRender),
	EndConditionalRenderNV(fallbackEndConditionalRenderNV),
	EndFragmentShaderATI(fallbackEndFragmentShaderATI),
	EndList(fallbackEndList),
	EndOcclusionQueryNV(fallbackEndOcclusionQueryNV),
	EndPerfMonitorAMD(fallbackEndPerfMonitorAMD),
	EndQuery(fallbackEndQuery),
	EndQueryARB(fallbackEndQueryARB),
	EndQueryIndexed(fallbackEndQueryIndexed),
	EndTransformFeedback(fallbackEndTransformFeedback),
	EndTransformFeedbackEXT(fallbackEndTransformFeedbackEXT),
	EndTransformFeedbackNV(fallbackEndTransformFeedbackNV),
	EndVertexShaderEXT(fallbackEndVertexShaderEXT),
	EndVideoCaptureNV(fallbackEndVideoCaptureNV),
	EvalCoord1d(fallbackEvalCoord1d),
	EvalCoord1dv(fallbackEvalCoord1dv),
	EvalCoord1f(fallbackEvalCoord1f),
	EvalCoord1fv(fallbackEvalCoord1fv),
	EvalCoord2d(fallbackEvalCoord2d),
	EvalCoord2dv(fallbackEvalCoord2dv),
	EvalCoord2f(fallbackEvalCoord2f),
	EvalCoord2fv(fallbackEvalCoord2fv),
	EvalMapsNV(fallbackEvalMapsNV),
	EvalMesh1(fallbackEvalMesh1),
	EvalMesh2(fallbackEvalMesh2),
	EvalPoint1(fallbackEvalPoint1),
	EvalPoint2(fallbackEvalPoint2),
	ExecuteProgramNV(fallbackExecuteProgramNV),
	ExtractComponentEXT(fallbackExtractComponentEXT),
	FeedbackBuffer(fallbackFeedbackBuffer),
	FenceSync(fallbackFenceSync),
	FinalCombinerInputNV(fallbackFinalCombinerInputNV),
	Finish(fallbackFinish),
	FinishAsyncSGIX(fallbackFinishAsyncSGIX),
	FinishFenceAPPLE(fallbackFinishFenceAPPLE),
	FinishFenceNV(fallbackFinishFenceNV),
	FinishObjectAPPLE(fallbackFinishObjectAPPLE),
	FinishTextureSUNX(fallbackFinishTextureSUNX),
	Flush(fallbackFlush),
	FlushMappedBufferRange(fallbackFlushMappedBufferRange),
	FlushMappedBufferRangeAPPLE(fallbackFlushMappedBufferRangeAPPLE),
	FlushPixelDataRangeNV(fallbackFlushPixelDataRangeNV),
	FlushRasterSGIX(fallbackFlushRasterSGIX),
	FlushVertexArrayRangeAPPLE(fallbackFlushVertexArrayRangeAPPLE),
	FlushVertexArrayRangeNV(fallbackFlushVertexArrayRangeNV),
	FogCoordFormatNV(fallbackFogCoordFormatNV),
	FogCoordPointer(fallbackFogCoordPointer),
	FogCoordPointerEXT(fallbackFogCoordPointerEXT),
	FogCoordPointerListIBM(fallbackFogCoordPointerListIBM),
	FogCoordd(fallbackFogCoordd),
	FogCoorddEXT(fallbackFogCoorddEXT),
	FogCoorddv(fallbackFogCoorddv),
	FogCoorddvEXT(fallbackFogCoorddvEXT),
	FogCoordf(fallbackFogCoordf),
	FogCoordfEXT(fallbackFogCoordfEXT),
	FogCoordfv(fallbackFogCoordfv),
	FogCoordfvEXT(fallbackFogCoordfvEXT),
	FogCoordhNV(fallbackFogCoordhNV),
	FogCoordhvNV(fallbackFogCoordhvNV),
	FogFuncSGIS(fallbackFogFuncSGIS),
	Fogf(fallbackFogf),
	Fogfv(fallbackFogfv),
	Fogi(fallbackFogi),
	Fogiv(fallbackFogiv),
	FragmentColorMaterialSGIX(fallbackFragmentColorMaterialSGIX),
	FragmentLightModelfSGIX(fallbackFragmentLightModelfSGIX),
	FragmentLightModelfvSGIX(fallbackFragmentLightModelfvSGIX),
	FragmentLightModeliSGIX(fallbackFragmentLightModeliSGIX),
	FragmentLightModelivSGIX(fallbackFragmentLightModelivSGIX),
	FragmentLightfSGIX(fallbackFragmentLightfSGIX),
	FragmentLightfvSGIX(fallbackFragmentLightfvSGIX),
	FragmentLightiSGIX(fallbackFragmentLightiSGIX),
	FragmentLightivSGIX(fallbackFragmentLightivSGIX),
	FragmentMaterialfSGIX(fallbackFragmentMaterialfSGIX),
	FragmentMaterialfvSGIX(fallbackFragmentMaterialfvSGIX),
	FragmentMaterialiSGIX(fallbackFragmentMaterialiSGIX),
	FragmentMaterialivSGIX(fallbackFragmentMaterialivSGIX),
	FrameTerminatorGREMEDY(fallbackFrameTerminatorGREMEDY),
	FrameZoomSGIX(fallbackFrameZoomSGIX),
	FramebufferDrawBufferEXT(fallbackFramebufferDrawBufferEXT),
	FramebufferDrawBuffersEXT(fallbackFramebufferDrawBuffersEXT),
	FramebufferReadBufferEXT(fallbackFramebufferReadBufferEXT),
	FramebufferRenderbuffer(fallbackFramebufferRenderbuffer),
	FramebufferRenderbufferEXT(fallbackFramebufferRenderbufferEXT),
	FramebufferTexture(fallbackFramebufferTexture),
	FramebufferTexture1D(fallbackFramebufferTexture1D),
	FramebufferTexture1DEXT(fallbackFramebufferTexture1DEXT),
	FramebufferTexture2D(fallbackFramebufferTexture2D),
	FramebufferTexture2DEXT(fallbackFramebufferTexture2DEXT),
	FramebufferTexture3D(fallbackFramebufferTexture3D),
	FramebufferTexture3DEXT(fallbackFramebufferTexture3DEXT),
	FramebufferTextureARB(fallbackFramebufferTextureARB),
	FramebufferTextureEXT(fallbackFramebufferTextureEXT),
	FramebufferTextureFaceARB(fallbackFramebufferTextureFaceARB),
	FramebufferTextureFaceEXT(fallbackFramebufferTextureFaceEXT),
	FramebufferTextureLayer(fallbackFramebufferTextureLayer),
	FramebufferTextureLayerARB(fallbackFramebufferTextureLayerARB),
	FramebufferTextureLayerEXT(fallbackFramebufferTextureLayerEXT),
	FreeObjectBufferATI(fallbackFreeObjectBufferATI),
	FrontFace(fallbackFrontFace),
	Frustum(fallbackFrustum),
	GenAsyncMarkersSGIX(fallbackGenAsyncMarkersSGIX),
	GenBuffers(fallbackGenBuffers),
	GenBuffersARB(fallbackGenBuffersARB),
	GenFencesAPPLE(fallbackGenFencesAPPLE),
	GenFencesNV(fallbackGenFencesNV),
	GenFragmentShadersATI(fallbackGenFragmentShadersATI),
	GenFramebuffers(fallbackGenFramebuffers),
	GenFramebuffersEXT(fallbackGenFramebuffersEXT),
	GenLists(fallbackGenLists),
	GenOcclusionQueriesNV(fallbackGenOcclusionQueriesNV),
	GenPerfMonitorsAMD(fallbackGenPerfMonitorsAMD),
	GenProgramsARB(fallbackGenProgramsARB),
	GenProgramsNV(fallbackGenProgramsNV),
	GenQueries(fallbackGenQueries),
	GenQueriesARB(fallbackGenQueriesARB),
	GenRenderbuffers(fallbackGenRenderbuffers),
	GenRenderbuffersEXT(fallbackGenRenderbuffersEXT),
	GenSamplers(fallbackGenSamplers),
	GenSymbolsEXT(fallbackGenSymbolsEXT),
	GenTextures(fallbackGenTextures),
	GenTexturesEXT(fallbackGenTexturesEXT),
	GenTransformFeedbacks(fallbackGenTransformFeedbacks),
	GenTransformFeedbacksNV(fallbackGenTransformFeedbacksNV),
	GenVertexArrays(fallbackGenVertexArrays),
	GenVertexArraysAPPLE(fallbackGenVertexArraysAPPLE),
	GenVertexShadersEXT(fallbackGenVertexShadersEXT),
	GenerateMipmap(fallbackGenerateMipmap),
	GenerateMipmapEXT(fallbackGenerateMipmapEXT),
	GenerateMultiTexMipmapEXT(fallbackGenerateMultiTexMipmapEXT),
	GenerateTextureMipmapEXT(fallbackGenerateTextureMipmapEXT),
	GetActiveAttrib(fallbackGetActiveAttrib),
	GetActiveAttribARB(fallbackGetActiveAttribARB),
	GetActiveSubroutineName(fallbackGetActiveSubroutineName),
	GetActiveSubroutineUniformName(fallbackGetActiveSubroutineUniformName),
	GetActiveSubroutineUniformiv(fallbackGetActiveSubroutineUniformiv),
	GetActiveUniform(fallbackGetActiveUniform),
	GetActiveUniformARB(fallbackGetActiveUniformARB),
	GetActiveUniformBlockName(fallbackGetActiveUniformBlockName),
	GetActiveUniformBlockiv(fallbackGetActiveUniformBlockiv),
	GetActiveUniformName(fallbackGetActiveUniformName),
	GetActiveUniformsiv(fallbackGetActiveUniformsiv),
	GetActiveVaryingNV(fallbackGetActiveVaryingNV),
	GetArrayObjectfvATI(fallbackGetArrayObjectfvATI),
	GetArrayObjectivATI(fallbackGetArrayObjectivATI),
	GetAttachedObjectsARB(fallbackGetAttachedObjectsARB),
	GetAttachedShaders(fallbackGetAttachedShaders),
	GetAttribLocation(fallbackGetAttribLocation),
	GetAttribLocationARB(fallbackGetAttribLocationARB),
	GetBooleanIndexedvEXT(fallbackGetBooleanIndexedvEXT),
	GetBooleani_v(fallbackGetBooleani_v),
	GetBooleanv(fallbackGetBooleanv),
	GetBufferParameteri64v(fallbackGetBufferParameteri64v),
	GetBufferParameteriv(fallbackGetBufferParameteriv),
	GetBufferParameterivARB(fallbackGetBufferParameterivARB),
	GetBufferParameterui64vNV(fallbackGetBufferParameterui64vNV),
	GetBufferPointerv(fallbackGetBufferPointerv),
	GetBufferPointervARB(fallbackGetBufferPointervARB),
	GetBufferSubData(fallbackGetBufferSubData),
	GetBufferSubDataARB(fallbackGetBufferSubDataARB),
	GetClipPlane(fallbackGetClipPlane),
	GetColorTable(fallbackGetColorTable),
	GetColorTableEXT(fallbackGetColorTableEXT),
	GetColorTableParameterfv(fallbackGetColorTableParameterfv),
	GetColorTableParameterfvEXT(fallbackGetColorTableParameterfvEXT),
	GetColorTableParameterfvSGI(fallbackGetColorTableParameterfvSGI),
	GetColorTableParameteriv(fallbackGetColorTableParameteriv),
	GetColorTableParameterivEXT(fallbackGetColorTableParameterivEXT),
	GetColorTableParameterivSGI(fallbackGetColorTableParameterivSGI),
	GetColorTableSGI(fallbackGetColorTableSGI),
	GetCombinerInputParameterfvNV(fallbackGetCombinerInputParameterfvNV),
	GetCombinerInputParameterivNV(fallbackGetCombinerInputParameterivNV),
	GetCombinerOutputParameterfvNV(fallbackGetCombinerOutputParameterfvNV),
	GetCombinerOutputParameterivNV(fallbackGetCombinerOutputParameterivNV),
	GetCombinerStageParameterfvNV(fallbackGetCombinerStageParameterfvNV),
	GetCompressedMultiTexImageEXT(fallbackGetCompressedMultiTexImageEXT),
	GetCompressedTexImage(fallbackGetCompressedTexImage),
	GetCompressedTexImageARB(fallbackGetCompressedTexImageARB),
	GetCompressedTextureImageEXT(fallbackGetCompressedTextureImageEXT),
	GetConvolutionFilter(fallbackGetConvolutionFilter),
	GetConvolutionFilterEXT(fallbackGetConvolutionFilterEXT),
	GetConvolutionParameterfv(fallbackGetConvolutionParameterfv),
	GetConvolutionParameterfvEXT(fallbackGetConvolutionParameterfvEXT),
	GetConvolutionParameteriv(fallbackGetConvolutionParameteriv),
	GetConvolutionParameterivEXT(fallbackGetConvolutionParameterivEXT),
	GetDetailTexFuncSGIS(fallbackGetDetailTexFuncSGIS),
	GetDoubleIndexedvEXT(fallbackGetDoubleIndexedvEXT),
	GetDoublev(fallbackGetDoublev),
	GetError(fallbackGetError),
	GetFenceivNV(fallbackGetFenceivNV),
	GetFinalCombinerInputParameterfvNV(fallbackGetFinalCombinerInputParameterfvNV),
	GetFinalCombinerInputParameterivNV(fallbackGetFinalCombinerInputParameterivNV),
	GetFloatIndexedvEXT(fallbackGetFloatIndexedvEXT),
	GetFloatv(fallbackGetFloatv),
	GetFogFuncSGIS(fallbackGetFogFuncSGIS),
	GetFragDataIndex(fallbackGetFragDataIndex),
	GetFragDataLocation(fallbackGetFragDataLocation),
	GetFragDataLocationEXT(fallbackGetFragDataLocationEXT),
	GetFragmentLightfvSGIX(fallbackGetFragmentLightfvSGIX),
	GetFragmentLightivSGIX(fallbackGetFragmentLightivSGIX),
	GetFragmentMaterialfvSGIX(fallbackGetFragmentMaterialfvSGIX),
	GetFragmentMaterialivSGIX(fallbackGetFragmentMaterialivSGIX),
	GetFramebufferAttachmentParameteriv(fallbackGetFramebufferAttachmentParameteriv),
	GetFramebufferAttachmentParameterivEXT(fallbackGetFramebufferAttachmentParameterivEXT),
	GetFramebufferParameterivEXT(fallbackGetFramebufferParameterivEXT),
	GetHandleARB(fallbackGetHandleARB),
	GetHistogram(fallbackGetHistogram),
	GetHistogramEXT(fallbackGetHistogramEXT),
	GetHistogramParameterfv(fallbackGetHistogramParameterfv),
	GetHistogramParameterfvEXT(fallbackGetHistogramParameterfvEXT),
	GetHistogramParameteriv(fallbackGetHistogramParameteriv),
	GetHistogramParameterivEXT(fallbackGetHistogramParameterivEXT),
	GetImageTransformParameterfvHP(fallbackGetImageTransformParameterfvHP),
	GetImageTransformParameterivHP(fallbackGetImageTransformParameterivHP),
	GetInfoLogARB(fallbackGetInfoLogARB),
	GetInstrumentsSGIX(fallbackGetInstrumentsSGIX),
	GetInteger64i_v(fallbackGetInteger64i_v),
	GetInteger64v(fallbackGetInteger64v),
	GetIntegerIndexedvEXT(fallbackGetIntegerIndexedvEXT),
	GetIntegeri_v(fallbackGetIntegeri_v),
	GetIntegerui64i_vNV(fallbackGetIntegerui64i_vNV),
	GetIntegerui64vNV(fallbackGetIntegerui64vNV),
	GetIntegerv(fallbackGetIntegerv),
	GetInvariantBooleanvEXT(fallbackGetInvariantBooleanvEXT),
	GetInvariantFloatvEXT(fallbackGetInvariantFloatvEXT),
	GetInvariantIntegervEXT(fallbackGetInvariantIntegervEXT),
	GetLightfv(fallbackGetLightfv),
	GetLightiv(fallbackGetLightiv),
	GetListParameterfvSGIX(fallbackGetListParameterfvSGIX),
	GetListParameterivSGIX(fallbackGetListParameterivSGIX),
	GetLocalConstantBooleanvEXT(fallbackGetLocalConstantBooleanvEXT),
	GetLocalConstantFloatvEXT(fallbackGetLocalConstantFloatvEXT),
	GetLocalConstantIntegervEXT(fallbackGetLocalConstantIntegervEXT),
	GetMapAttribParameterfvNV(fallbackGetMapAttribParameterfvNV),
	GetMapAttribParameterivNV(fallbackGetMapAttribParameterivNV),
	GetMapControlPointsNV(fallbackGetMapControlPointsNV),
	GetMapParameterfvNV(fallbackGetMapParameterfvNV),
	GetMapParameterivNV(fallbackGetMapParameterivNV),
	GetMapdv(fallbackGetMapdv),
	GetMapfv(fallbackGetMapfv),
	GetMapiv(fallbackGetMapiv),
	GetMaterialfv(fallbackGetMaterialfv),
	GetMaterialiv(fallbackGetMaterialiv),
	GetMinmax(fallbackGetMinmax),
	GetMinmaxEXT(fallbackGetMinmaxEXT),
	GetMinmaxParameterfv(fallbackGetMinmaxParameterfv),
	GetMinmaxParameterfvEXT(fallbackGetMinmaxParameterfvEXT),
	GetMinmaxParameteriv(fallbackGetMinmaxParameteriv),
	GetMinmaxParameterivEXT(fallbackGetMinmaxParameterivEXT),
	GetMultiTexEnvfvEXT(fallbackGetMultiTexEnvfvEXT),
	GetMultiTexEnvivEXT(fallbackGetMultiTexEnvivEXT),
	GetMultiTexGendvEXT(fallbackGetMultiTexGendvEXT),
	GetMultiTexGenfvEXT(fallbackGetMultiTexGenfvEXT),
	GetMultiTexGenivEXT(fallbackGetMultiTexGenivEXT),
	GetMultiTexImageEXT(fallbackGetMultiTexImageEXT),
	GetMultiTexLevelParameterfvEXT(fallbackGetMultiTexLevelParameterfvEXT),
	GetMultiTexLevelParameterivEXT(fallbackGetMultiTexLevelParameterivEXT),
	GetMultiTexParameterIivEXT(fallbackGetMultiTexParameterIivEXT),
	GetMultiTexParameterIuivEXT(fallbackGetMultiTexParameterIuivEXT),
	GetMultiTexParameterfvEXT(fallbackGetMultiTexParameterfvEXT),
	GetMultiTexParameterivEXT(fallbackGetMultiTexParameterivEXT),
	GetMultisamplefv(fallbackGetMultisamplefv),
	GetMultisamplefvNV(fallbackGetMultisamplefvNV),
	GetNamedBufferParameterivEXT(fallbackGetNamedBufferParameterivEXT),
	GetNamedBufferParameterui64vNV(fallbackGetNamedBufferParameterui64vNV),
	GetNamedBufferPointervEXT(fallbackGetNamedBufferPointervEXT),
	GetNamedBufferSubDataEXT(fallbackGetNamedBufferSubDataEXT),
	GetNamedFramebufferAttachmentParameterivEXT(fallbackGetNamedFramebufferAttachmentParameterivEXT),
	GetNamedProgramLocalParameterIivEXT(fallbackGetNamedProgramLocalParameterIivEXT),
	GetNamedProgramLocalParameterIuivEXT(fallbackGetNamedProgramLocalParameterIuivEXT),
	GetNamedProgramLocalParameterdvEXT(fallbackGetNamedProgramLocalParameterdvEXT),
	GetNamedProgramLocalParameterfvEXT(fallbackGetNamedProgramLocalParameterfvEXT),
	GetNamedProgramStringEXT(fallbackGetNamedProgramStringEXT),
	GetNamedProgramivEXT(fallbackGetNamedProgramivEXT),
	GetNamedRenderbufferParameterivEXT(fallbackGetNamedRenderbufferParameterivEXT),
	GetNamedStringARB(fallbackGetNamedStringARB),
	GetNamedStringivARB(fallbackGetNamedStringivARB),
	GetObjectBufferfvATI(fallbackGetObjectBufferfvATI),
	GetObjectBufferivATI(fallbackGetObjectBufferivATI),
	GetObjectParameterfvARB(fallbackGetObjectParameterfvARB),
	GetObjectParameterivAPPLE(fallbackGetObjectParameterivAPPLE),
	GetObjectParameterivARB(fallbackGetObjectParameterivARB),
	GetOcclusionQueryivNV(fallbackGetOcclusionQueryivNV),
	GetOcclusionQueryuivNV(fallbackGetOcclusionQueryuivNV),
	GetPerfMonitorCounterDataAMD(fallbackGetPerfMonitorCounterDataAMD),
	GetPerfMonitorCounterInfoAMD(fallbackGetPerfMonitorCounterInfoAMD),
	GetPerfMonitorCounterStringAMD(fallbackGetPerfMonitorCounterStringAMD),
	GetPerfMonitorCountersAMD(fallbackGetPerfMonitorCountersAMD),
	GetPerfMonitorGroupStringAMD(fallbackGetPerfMonitorGroupStringAMD),
	GetPerfMonitorGroupsAMD(fallbackGetPerfMonitorGroupsAMD),
	GetPixelMapfv(fallbackGetPixelMapfv),
	GetPixelMapuiv(fallbackGetPixelMapuiv),
	GetPixelMapusv(fallbackGetPixelMapusv),
	GetPixelTexGenParameterfvSGIS(fallbackGetPixelTexGenParameterfvSGIS),
	GetPixelTexGenParameterivSGIS(fallbackGetPixelTexGenParameterivSGIS),
	GetPointerIndexedvEXT(fallbackGetPointerIndexedvEXT),
	GetPointerv(fallbackGetPointerv),
	GetPointervEXT(fallbackGetPointervEXT),
	GetPolygonStipple(fallbackGetPolygonStipple),
	GetProgramEnvParameterIivNV(fallbackGetProgramEnvParameterIivNV),
	GetProgramEnvParameterIuivNV(fallbackGetProgramEnvParameterIuivNV),
	GetProgramEnvParameterdvARB(fallbackGetProgramEnvParameterdvARB),
	GetProgramEnvParameterfvARB(fallbackGetProgramEnvParameterfvARB),
	GetProgramInfoLog(fallbackGetProgramInfoLog),
	GetProgramLocalParameterIivNV(fallbackGetProgramLocalParameterIivNV),
	GetProgramLocalParameterIuivNV(fallbackGetProgramLocalParameterIuivNV),
	GetProgramLocalParameterdvARB(fallbackGetProgramLocalParameterdvARB),
	GetProgramLocalParameterfvARB(fallbackGetProgramLocalParameterfvARB),
	GetProgramNamedParameterdvNV(fallbackGetProgramNamedParameterdvNV),
	GetProgramNamedParameterfvNV(fallbackGetProgramNamedParameterfvNV),
	GetProgramParameterdvNV(fallbackGetProgramParameterdvNV),
	GetProgramParameterfvNV(fallbackGetProgramParameterfvNV),
	GetProgramStageiv(fallbackGetProgramStageiv),
	GetProgramStringARB(fallbackGetProgramStringARB),
	GetProgramStringNV(fallbackGetProgramStringNV),
	GetProgramiv(fallbackGetProgramiv),
	GetProgramivARB(fallbackGetProgramivARB),
	GetProgramivNV(fallbackGetProgramivNV),
	GetQueryIndexediv(fallbackGetQueryIndexediv),
	GetQueryObjecti64v(fallbackGetQueryObjecti64v),
	GetQueryObjecti64vEXT(fallbackGetQueryObjecti64vEXT),
	GetQueryObjectiv(fallbackGetQueryObjectiv),
	GetQueryObjectivARB(fallbackGetQueryObjectivARB),
	GetQueryObjectui64v(fallbackGetQueryObjectui64v),
	GetQueryObjectui64vEXT(fallbackGetQueryObjectui64vEXT),
	GetQueryObjectuiv(fallbackGetQueryObjectuiv),
	GetQueryObjectuivARB(fallbackGetQueryObjectuivARB),
	GetQueryiv(fallbackGetQueryiv),
	GetQueryivARB(fallbackGetQueryivARB),
	GetRenderbufferParameteriv(fallbackGetRenderbufferParameteriv),
	GetRenderbufferParameterivEXT(fallbackGetRenderbufferParameterivEXT),
	GetSamplerParameterIfv(fallbackGetSamplerParameterIfv),
	GetSamplerParameterIiv(fallbackGetSamplerParameterIiv),
	GetSamplerParameterfv(fallbackGetSamplerParameterfv),
	GetSamplerParameteriv(fallbackGetSamplerParameteriv),
	GetSeparableFilter(fallbackGetSeparableFilter),
	GetSeparableFilterEXT(fallbackGetSeparableFilterEXT),
	GetShaderInfoLog(fallbackGetShaderInfoLog),
	GetShaderSource(fallbackGetShaderSource),
	GetShaderSourceARB(fallbackGetShaderSourceARB),
	GetShaderiv(fallbackGetShaderiv),
	GetSharpenTexFuncSGIS(fallbackGetSharpenTexFuncSGIS),
	GetString(fallbackGetString),
	GetStringi(fallbackGetStringi),
	GetSubroutineIndex(fallbackGetSubroutineIndex),
	GetSubroutineUniformLocation(fallbackGetSubroutineUniformLocation),
	GetSynciv(fallbackGetSynciv),
	GetTexBumpParameterfvATI(fallbackGetTexBumpParameterfvATI),
	GetTexBumpParameterivATI(fallbackGetTexBumpParameterivATI),
	GetTexEnvfv(fallbackGetTexEnvfv),
	GetTexEnviv(fallbackGetTexEnviv),
	GetTexFilterFuncSGIS(fallbackGetTexFilterFuncSGIS),
	GetTexGendv(fallbackGetTexGendv),
	GetTexGenfv(fallbackGetTexGenfv),
	GetTexGeniv(fallbackGetTexGeniv),
	GetTexImage(fallbackGetTexImage),
	GetTexLevelParameterfv(fallbackGetTexLevelParameterfv),
	GetTexLevelParameteriv(fallbackGetTexLevelParameteriv),
	GetTexParameterIiv(fallbackGetTexParameterIiv),
	GetTexParameterIivEXT(fallbackGetTexParameterIivEXT),
	GetTexParameterIuiv(fallbackGetTexParameterIuiv),
	GetTexParameterIuivEXT(fallbackGetTexParameterIuivEXT),
	GetTexParameterPointervAPPLE(fallbackGetTexParameterPointervAPPLE),
	GetTexParameterfv(fallbackGetTexParameterfv),
	GetTexParameteriv(fallbackGetTexParameteriv),
	GetTextureImageEXT(fallbackGetTextureImageEXT),
	GetTextureLevelParameterfvEXT(fallbackGetTextureLevelParameterfvEXT),
	GetTextureLevelParameterivEXT(fallbackGetTextureLevelParameterivEXT),
	GetTextureParameterIivEXT(fallbackGetTextureParameterIivEXT),
	GetTextureParameterIuivEXT(fallbackGetTextureParameterIuivEXT),
	GetTextureParameterfvEXT(fallbackGetTextureParameterfvEXT),
	GetTextureParameterivEXT(fallbackGetTextureParameterivEXT),
	GetTrackMatrixivNV(fallbackGetTrackMatrixivNV),
	GetTransformFeedbackVarying(fallbackGetTransformFeedbackVarying),
	GetTransformFeedbackVaryingEXT(fallbackGetTransformFeedbackVaryingEXT),
	GetTransformFeedbackVaryingNV(fallbackGetTransformFeedbackVaryingNV),
	GetUniformBlockIndex(fallbackGetUniformBlockIndex),
	GetUniformBufferSizeEXT(fallbackGetUniformBufferSizeEXT),
	GetUniformIndices(fallbackGetUniformIndices),
	GetUniformLocation(fallbackGetUniformLocation),
	GetUniformLocationARB(fallbackGetUniformLocationARB),
	GetUniformOffsetEXT(fallbackGetUniformOffsetEXT),
	GetUniformSubroutineuiv(fallbackGetUniformSubroutineuiv),
	GetUniformdv(fallbackGetUniformdv),
	GetUniformfv(fallbackGetUniformfv),
	GetUniformfvARB(fallbackGetUniformfvARB),
	GetUniformiv(fallbackGetUniformiv),
	GetUniformivARB(fallbackGetUniformivARB),
	GetUniformui64vNV(fallbackGetUniformui64vNV),
	GetUniformuiv(fallbackGetUniformuiv),
	GetUniformuivEXT(fallbackGetUniformuivEXT),
	GetVariantArrayObjectfvATI(fallbackGetVariantArrayObjectfvATI),
	GetVariantArrayObjectivATI(fallbackGetVariantArrayObjectivATI),
	GetVariantBooleanvEXT(fallbackGetVariantBooleanvEXT),
	GetVariantFloatvEXT(fallbackGetVariantFloatvEXT),
	GetVariantIntegervEXT(fallbackGetVariantIntegervEXT),
	GetVariantPointervEXT(fallbackGetVariantPointervEXT),
	GetVaryingLocationNV(fallbackGetVaryingLocationNV),
	GetVertexAttribArrayObjectfvATI(fallbackGetVertexAttribArrayObjectfvATI),
	GetVertexAttribArrayObjectivATI(fallbackGetVertexAttribArrayObjectivATI),
	GetVertexAttribIiv(fallbackGetVertexAttribIiv),
	GetVertexAttribIivEXT(fallbackGetVertexAttribIivEXT),
	GetVertexAttribIuiv(fallbackGetVertexAttribIuiv),
	GetVertexAttribIuivEXT(fallbackGetVertexAttribIuivEXT),
	GetVertexAttribPointerv(fallbackGetVertexAttribPointerv),
	GetVertexAttribPointervARB(fallbackGetVertexAttribPointervARB),
	GetVertexAttribPointervNV(fallbackGetVertexAttribPointervNV),
	GetVertexAttribdv(fallbackGetVertexAttribdv),
	GetVertexAttribdvARB(fallbackGetVertexAttribdvARB),
	GetVertexAttribdvNV(fallbackGetVertexAttribdvNV),
	GetVertexAttribfv(fallbackGetVertexAttribfv),
	GetVertexAttribfvARB(fallbackGetVertexAttribfvARB),
	GetVertexAttribfvNV(fallbackGetVertexAttribfvNV),
	GetVertexAttribiv(fallbackGetVertexAttribiv),
	GetVertexAttribivARB(fallbackGetVertexAttribivARB),
	GetVertexAttribivNV(fallbackGetVertexAttribivNV),
	GetVideoCaptureStreamdvNV(fallbackGetVideoCaptureStreamdvNV),
	GetVideoCaptureStreamfvNV(fallbackGetVideoCaptureStreamfvNV),
	GetVideoCaptureStreamivNV(fallbackGetVideoCaptureStreamivNV),
	GetVideoCaptureivNV(fallbackGetVideoCaptureivNV),
	GetVideoi64vNV(fallbackGetVideoi64vNV),
	GetVideoivNV(fallbackGetVideoivNV),
	GetVideoui64vNV(fallbackGetVideoui64vNV),
	GetVideouivNV(fallbackGetVideouivNV),
	GlobalAlphaFactorbSUN(fallbackGlobalAlphaFactorbSUN),
	GlobalAlphaFactordSUN(fallbackGlobalAlphaFactordSUN),
	GlobalAlphaFactorfSUN(fallbackGlobalAlphaFactorfSUN),
	GlobalAlphaFactoriSUN(fallbackGlobalAlphaFactoriSUN),
	GlobalAlphaFactorsSUN(fallbackGlobalAlphaFactorsSUN),
	GlobalAlphaFactorubSUN(fallbackGlobalAlphaFactorubSUN),
	GlobalAlphaFactoruiSUN(fallbackGlobalAlphaFactoruiSUN),
	GlobalAlphaFactorusSUN(fallbackGlobalAlphaFactorusSUN),
	Hint(fallbackHint),
	HintPGI(fallbackHintPGI),
	Histogram(fallbackHistogram),
	HistogramEXT(fallbackHistogramEXT),
	IglooInterfaceSGIX(fallbackIglooInterfaceSGIX),
	ImageTransformParameterfHP(fallbackImageTransformParameterfHP),
	ImageTransformParameterfvHP(fallbackImageTransformParameterfvHP),
	ImageTransformParameteriHP(fallbackImageTransformParameteriHP),
	ImageTransformParameterivHP(fallbackImageTransformParameterivHP),
	IndexFormatNV(fallbackIndexFormatNV),
	IndexFuncEXT(fallbackIndexFuncEXT),
	IndexMask(fallbackIndexMask),
	IndexMaterialEXT(fallbackIndexMaterialEXT),
	IndexPointer(fallbackIndexPointer),
	IndexPointerEXT(fallbackIndexPointerEXT),
	IndexPointerListIBM(fallbackIndexPointerListIBM),
	Indexd(fallbackIndexd),
	Indexdv(fallbackIndexdv),
	Indexf(fallbackIndexf),
	Indexfv(fallbackIndexfv),
	Indexi(fallbackIndexi),
	Indexiv(fallbackIndexiv),
	Indexs(fallbackIndexs),
	Indexsv(fallbackIndexsv),
	Indexub(fallbackIndexub),
	Indexubv(fallbackIndexubv),
	InitNames(fallbackInitNames),
	InsertComponentEXT(fallbackInsertComponentEXT),
	InstrumentsBufferSGIX(fallbackInstrumentsBufferSGIX),
	InterleavedArrays(fallbackInterleavedArrays),
	IsAsyncMarkerSGIX(fallbackIsAsyncMarkerSGIX),
	IsBuffer(fallbackIsBuffer),
	IsBufferARB(fallbackIsBufferARB),
	IsBufferResidentNV(fallbackIsBufferResidentNV),
	IsEnabled(fallbackIsEnabled),
	IsEnabledIndexedEXT(fallbackIsEnabledIndexedEXT),
	IsEnabledi(fallbackIsEnabledi),
	IsFenceAPPLE(fallbackIsFenceAPPLE),
	IsFenceNV(fallbackIsFenceNV),
	IsFramebuffer(fallbackIsFramebuffer),
	IsFramebufferEXT(fallbackIsFramebufferEXT),
	IsList(fallbackIsList),
	IsNamedBufferResidentNV(fallbackIsNamedBufferResidentNV),
	IsNamedStringARB(fallbackIsNamedStringARB),
	IsObjectBufferATI(fallbackIsObjectBufferATI),
	IsOcclusionQueryNV(fallbackIsOcclusionQueryNV),
	IsProgram(fallbackIsProgram),
	IsProgramARB(fallbackIsProgramARB),
	IsProgramNV(fallbackIsProgramNV),
	IsQuery(fallbackIsQuery),
	IsQueryARB(fallbackIsQueryARB),
	IsRenderbuffer(fallbackIsRenderbuffer),
	IsRenderbufferEXT(fallbackIsRenderbufferEXT),
	IsSampler(fallbackIsSampler),
	IsShader(fallbackIsShader),
	IsSync(fallbackIsSync),
	IsTexture(fallbackIsTexture),
	IsTextureEXT(fallbackIsTextureEXT),
	IsTransformFeedback(fallbackIsTransformFeedback),
	IsTransformFeedbackNV(fallbackIsTransformFeedbackNV),
	IsVariantEnabledEXT(fallbackIsVariantEnabledEXT),
	IsVertexArray(fallbackIsVertexArray),
	IsVertexArrayAPPLE(fallbackIsVertexArrayAPPLE),
	IsVertexAttribEnabledAPPLE(fallbackIsVertexAttribEnabledAPPLE),
	LightEnviSGIX(fallbackLightEnviSGIX),
	LightModelf(fallbackLightModelf),
	LightModelfv(fallbackLightModelfv),
	LightModeli(fallbackLightModeli),
	LightModeliv(fallbackLightModeliv),
	Lightf(fallbackLightf),
	Lightfv(fallbackLightfv),
	Lighti(fallbackLighti),
	Lightiv(fallbackLightiv),
	LineStipple(fallbackLineStipple),
	LineWidth(fallbackLineWidth),
	LinkProgram(fallbackLinkProgram),
	LinkProgramARB(fallbackLinkProgramARB),
	ListBase(fallbackListBase),
	ListParameterfSGIX(fallbackListParameterfSGIX),
	ListParameterfvSGIX(fallbackListParameterfvSGIX),
	ListParameteriSGIX(fallbackListParameteriSGIX),
	ListParameterivSGIX(fallbackListParameterivSGIX),
	LoadIdentity(fallbackLoadIdentity),
	LoadIdentityDeformationMapSGIX(fallbackLoadIdentityDeformationMapSGIX),
	LoadMatrixd(fallbackLoadMatrixd),
	LoadMatrixf(fallbackLoadMatrixf),
	LoadName(fallbackLoadName),
	LoadProgramNV(fallbackLoadProgramNV),
	LoadTransposeMatrixd(fallbackLoadTransposeMatrixd),
	LoadTransposeMatrixdARB(fallbackLoadTransposeMatrixdARB),
	LoadTransposeMatrixf(fallbackLoadTransposeMatrixf),
	LoadTransposeMatrixfARB(fallbackLoadTransposeMatrixfARB),
	LockArraysEXT(fallbackLockArraysEXT),
	LogicOp(fallbackLogicOp),
	MakeBufferNonResidentNV(fallbackMakeBufferNonResidentNV),
	MakeBufferResidentNV(fallbackMakeBufferResidentNV),
	MakeNamedBufferNonResidentNV(fallbackMakeNamedBufferNonResidentNV),
	MakeNamedBufferResidentNV(fallbackMakeNamedBufferResidentNV),
	Map1d(fallbackMap1d),
	Map1f(fallbackMap1f),
	Map2d(fallbackMap2d),
	Map2f(fallbackMap2f),
	MapBuffer(fallbackMapBuffer),
	MapBufferARB(fallbackMapBufferARB),
	MapBufferRange(fallbackMapBufferRange),
	MapControlPointsNV(fallbackMapControlPointsNV),
	MapGrid1d(fallbackMapGrid1d),
	MapGrid1f(fallbackMapGrid1f),
	MapGrid2d(fallbackMapGrid2d),
	MapGrid2f(fallbackMapGrid2f),
	MapNamedBufferEXT(fallbackMapNamedBufferEXT),
	MapObjectBufferATI(fallbackMapObjectBufferATI),
	MapParameterfvNV(fallbackMapParameterfvNV),
	MapParameterivNV(fallbackMapParameterivNV),
	MapVertexAttrib1dAPPLE(fallbackMapVertexAttrib1dAPPLE),
	MapVertexAttrib1fAPPLE(fallbackMapVertexAttrib1fAPPLE),
	MapVertexAttrib2dAPPLE(fallbackMapVertexAttrib2dAPPLE),
	MapVertexAttrib2fAPPLE(fallbackMapVertexAttrib2fAPPLE),
	Materialf(fallbackMaterialf),
	Materialfv(fallbackMaterialfv),
	Materiali(fallbackMateriali),
	Materialiv(fallbackMaterialiv),
	MatrixFrustumEXT(fallbackMatrixFrustumEXT),
	MatrixIndexPointerARB(fallbackMatrixIndexPointerARB),
	MatrixIndexubvARB(fallbackMatrixIndexubvARB),
	MatrixIndexuivARB(fallbackMatrixIndexuivARB),
	MatrixIndexusvARB(fallbackMatrixIndexusvARB),
	MatrixLoadIdentityEXT(fallbackMatrixLoadIdentityEXT),
	MatrixLoadTransposedEXT(fallbackMatrixLoadTransposedEXT),
	MatrixLoadTransposefEXT(fallbackMatrixLoadTransposefEXT),
	MatrixLoaddEXT(fallbackMatrixLoaddEXT),
	MatrixLoadfEXT(fallbackMatrixLoadfEXT),
	MatrixMode(fallbackMatrixMode),
	MatrixMultTransposedEXT(fallbackMatrixMultTransposedEXT),
	MatrixMultTransposefEXT(fallbackMatrixMultTransposefEXT),
	MatrixMultdEXT(fallbackMatrixMultdEXT),
	MatrixMultfEXT(fallbackMatrixMultfEXT),
	MatrixOrthoEXT(fallbackMatrixOrthoEXT),
	MatrixPopEXT(fallbackMatrixPopEXT),
	MatrixPushEXT(fallbackMatrixPushEXT),
	MatrixRotatedEXT(fallbackMatrixRotatedEXT),
	MatrixRotatefEXT(fallbackMatrixRotatefEXT),
	MatrixScaledEXT(fallbackMatrixScaledEXT),
	MatrixScalefEXT(fallbackMatrixScalefEXT),
	MatrixTranslatedEXT(fallbackMatrixTranslatedEXT),
	MatrixTranslatefEXT(fallbackMatrixTranslatefEXT),
	MinSampleShading(fallbackMinSampleShading),
	Minmax(fallbackMinmax),
	MinmaxEXT(fallbackMinmaxEXT),
	MultMatrixd(fallbackMultMatrixd),
	MultMatrixf(fallbackMultMatrixf),
	MultTransposeMatrixd(fallbackMultTransposeMatrixd),
	MultTransposeMatrixdARB(fallbackMultTransposeMatrixdARB),
	MultTransposeMatrixf(fallbackMultTransposeMatrixf),
	MultTransposeMatrixfARB(fallbackMultTransposeMatrixfARB),
	MultiDrawArrays(fallbackMultiDrawArrays),
	MultiDrawArraysEXT(fallbackMultiDrawArraysEXT),
	MultiDrawElementArrayAPPLE(fallbackMultiDrawElementArrayAPPLE),
	MultiDrawElements(fallbackMultiDrawElements),
	MultiDrawElementsBaseVertex(fallbackMultiDrawElementsBaseVertex),
	MultiDrawElementsEXT(fallbackMultiDrawElementsEXT),
	MultiDrawRangeElementArrayAPPLE(fallbackMultiDrawRangeElementArrayAPPLE),
	MultiModeDrawArraysIBM(fallbackMultiModeDrawArraysIBM),
	MultiModeDrawElementsIBM(fallbackMultiModeDrawElementsIBM),
	MultiTexBufferEXT(fallbackMultiTexBufferEXT),
	MultiTexCoord1d(fallbackMultiTexCoord1d),
	MultiTexCoord1dARB(fallbackMultiTexCoord1dARB),
	MultiTexCoord1dv(fallbackMultiTexCoord1dv),
	MultiTexCoord1dvARB(fallbackMultiTexCoord1dvARB),
	MultiTexCoord1f(fallbackMultiTexCoord1f),
	MultiTexCoord1fARB(fallbackMultiTexCoord1fARB),
	MultiTexCoord1fv(fallbackMultiTexCoord1fv),
	MultiTexCoord1fvARB(fallbackMultiTexCoord1fvARB),
	MultiTexCoord1hNV(fallbackMultiTexCoord1hNV),
	MultiTexCoord1hvNV(fallbackMultiTexCoord1hvNV),
	MultiTexCoord1i(fallbackMultiTexCoord1i),
	MultiTexCoord1iARB(fallbackMultiTexCoord1iARB),
	MultiTexCoord1iv(fallbackMultiTexCoord1iv),
	MultiTexCoord1ivARB(fallbackMultiTexCoord1ivARB),
	MultiTexCoord1s(fallbackMultiTexCoord1s),
	MultiTexCoord1sARB(fallbackMultiTexCoord1sARB),
	MultiTexCoord1sv(fallbackMultiTexCoord1sv),
	MultiTexCoord1svARB(fallbackMultiTexCoord1svARB),
	MultiTexCoord2d(fallbackMultiTexCoord2d),
	MultiTexCoord2dARB(fallbackMultiTexCoord2dARB),
	MultiTexCoord2dv(fallbackMultiTexCoord2dv),
	MultiTexCoord2dvARB(fallbackMultiTexCoord2dvARB),
	MultiTexCoord2f(fallbackMultiTexCoord2f),
	MultiTexCoord2fARB(fallbackMultiTexCoord2fARB),
	MultiTexCoord2fv(fallbackMultiTexCoord2fv),
	MultiTexCoord2fvARB(fallbackMultiTexCoord2fvARB),
	MultiTexCoord2hNV(fallbackMultiTexCoord2hNV),
	MultiTexCoord2hvNV(fallbackMultiTexCoord2hvNV),
	MultiTexCoord2i(fallbackMultiTexCoord2i),
	MultiTexCoord2iARB(fallbackMultiTexCoord2iARB),
	MultiTexCoord2iv(fallbackMultiTexCoord2iv),
	MultiTexCoord2ivARB(fallbackMultiTexCoord2ivARB),
	MultiTexCoord2s(fallbackMultiTexCoord2s),
	MultiTexCoord2sARB(fallbackMultiTexCoord2sARB),
	MultiTexCoord2sv(fallbackMultiTexCoord2sv),
	MultiTexCoord2svARB(fallbackMultiTexCoord2svARB),
	MultiTexCoord3d(fallbackMultiTexCoord3d),
	MultiTexCoord3dARB(fallbackMultiTexCoord3dARB),
	MultiTexCoord3dv(fallbackMultiTexCoord3dv),
	MultiTexCoord3dvARB(fallbackMultiTexCoord3dvARB),
	MultiTexCoord3f(fallbackMultiTexCoord3f),
	MultiTexCoord3fARB(fallbackMultiTexCoord3fARB),
	MultiTexCoord3fv(fallbackMultiTexCoord3fv),
	MultiTexCoord3fvARB(fallbackMultiTexCoord3fvARB),
	MultiTexCoord3hNV(fallbackMultiTexCoord3hNV),
	MultiTexCoord3hvNV(fallbackMultiTexCoord3hvNV),
	MultiTexCoord3i(fallbackMultiTexCoord3i),
	MultiTexCoord3iARB(fallbackMultiTexCoord3iARB),
	MultiTexCoord3iv(fallbackMultiTexCoord3iv),
	MultiTexCoord3ivARB(fallbackMultiTexCoord3ivARB),
	MultiTexCoord3s(fallbackMultiTexCoord3s),
	MultiTexCoord3sARB(fallbackMultiTexCoord3sARB),
	MultiTexCoord3sv(fallbackMultiTexCoord3sv),
	MultiTexCoord3svARB(fallbackMultiTexCoord3svARB),
	MultiTexCoord4d(fallbackMultiTexCoord4d),
	MultiTexCoord4dARB(fallbackMultiTexCoord4dARB),
	MultiTexCoord4dv(fallbackMultiTexCoord4dv),
	MultiTexCoord4dvARB(fallbackMultiTexCoord4dvARB),
	MultiTexCoord4f(fallbackMultiTexCoord4f),
	MultiTexCoord4fARB(fallbackMultiTexCoord4fARB),
	MultiTexCoord4fv(fallbackMultiTexCoord4fv),
	MultiTexCoord4fvARB(fallbackMultiTexCoord4fvARB),
	MultiTexCoord4hNV(fallbackMultiTexCoord4hNV),
	MultiTexCoord4hvNV(fallbackMultiTexCoord4hvNV),
	MultiTexCoord4i(fallbackMultiTexCoord4i),
	MultiTexCoord4iARB(fallbackMultiTexCoord4iARB),
	MultiTexCoord4iv(fallbackMultiTexCoord4iv),
	MultiTexCoord4ivARB(fallbackMultiTexCoord4ivARB),
	MultiTexCoord4s(fallbackMultiTexCoord4s),
	MultiTexCoord4sARB(fallbackMultiTexCoord4sARB),
	MultiTexCoord4sv(fallbackMultiTexCoord4sv),
	MultiTexCoord4svARB(fallbackMultiTexCoord4svARB),
	MultiTexCoordP1ui(fallbackMultiTexCoordP1ui),
	MultiTexCoordP1uiv(fallbackMultiTexCoordP1uiv),
	MultiTexCoordP2ui(fallbackMultiTexCoordP2ui),
	MultiTexCoordP2uiv(fallbackMultiTexCoordP2uiv),
	MultiTexCoordP3ui(fallbackMultiTexCoordP3ui),
	MultiTexCoordP3uiv(fallbackMultiTexCoordP3uiv),
	MultiTexCoordP4ui(fallbackMultiTexCoordP4ui),
	MultiTexCoordP4uiv(fallbackMultiTexCoordP4uiv),
	MultiTexCoordPointerEXT(fallbackMultiTexCoordPointerEXT),
	MultiTexEnvfEXT(fallbackMultiTexEnvfEXT),
	MultiTexEnvfvEXT(fallbackMultiTexEnvfvEXT),
	MultiTexEnviEXT(fallbackMultiTexEnviEXT),
	MultiTexEnvivEXT(fallbackMultiTexEnvivEXT),
	MultiTexGendEXT(fallbackMultiTexGendEXT),
	MultiTexGendvEXT(fallbackMultiTexGendvEXT),
	MultiTexGenfEXT(fallbackMultiTexGenfEXT),
	MultiTexGenfvEXT(fallbackMultiTexGenfvEXT),
	MultiTexGeniEXT(fallbackMultiTexGeniEXT),
	MultiTexGenivEXT(fallbackMultiTexGenivEXT),
	MultiTexImage1DEXT(fallbackMultiTexImage1DEXT),
	MultiTexImage2DEXT(fallbackMultiTexImage2DEXT),
	MultiTexImage3DEXT(fallbackMultiTexImage3DEXT),
	MultiTexParameterIivEXT(fallbackMultiTexParameterIivEXT),
	MultiTexParameterIuivEXT(fallbackMultiTexParameterIuivEXT),
	MultiTexParameterfEXT(fallbackMultiTexParameterfEXT),
	MultiTexParameterfvEXT(fallbackMultiTexParameterfvEXT),
	MultiTexParameteriEXT(fallbackMultiTexParameteriEXT),
	MultiTexParameterivEXT(fallbackMultiTexParameterivEXT),
	MultiTexRenderbufferEXT(fallbackMultiTexRenderbufferEXT),
	MultiTexSubImage1DEXT(fallbackMultiTexSubImage1DEXT),
	MultiTexSubImage2DEXT(fallbackMultiTexSubImage2DEXT),
	MultiTexSubImage3DEXT(fallbackMultiTexSubImage3DEXT),
	NamedBufferDataEXT(fallbackNamedBufferDataEXT),
	NamedBufferSubDataEXT(fallbackNamedBufferSubDataEXT),
	NamedFramebufferRenderbufferEXT(fallbackNamedFramebufferRenderbufferEXT),
	NamedFramebufferTexture1DEXT(fallbackNamedFramebufferTexture1DEXT),
	NamedFramebufferTexture2DEXT(fallbackNamedFramebufferTexture2DEXT),
	NamedFramebufferTexture3DEXT(fallbackNamedFramebufferTexture3DEXT),
	NamedFramebufferTextureEXT(fallbackNamedFramebufferTextureEXT),
	NamedFramebufferTextureFaceEXT(fallbackNamedFramebufferTextureFaceEXT),
	NamedFramebufferTextureLayerEXT(fallbackNamedFramebufferTextureLayerEXT),
	NamedProgramLocalParameter4dEXT(fallbackNamedProgramLocalParameter4dEXT),
	NamedProgramLocalParameter4dvEXT(fallbackNamedProgramLocalParameter4dvEXT),
	NamedProgramLocalParameter4fEXT(fallbackNamedProgramLocalParameter4fEXT),
	NamedProgramLocalParameter4fvEXT(fallbackNamedProgramLocalParameter4fvEXT),
	NamedProgramLocalParameterI4iEXT(fallbackNamedProgramLocalParameterI4iEXT),
	NamedProgramLocalParameterI4ivEXT(fallbackNamedProgramLocalParameterI4ivEXT),
	NamedProgramLocalParameterI4uiEXT(fallbackNamedProgramLocalParameterI4uiEXT),
	NamedProgramLocalParameterI4uivEXT(fallbackNamedProgramLocalParameterI4uivEXT),
	NamedProgramLocalParameters4fvEXT(fallbackNamedProgramLocalParameters4fvEXT),
	NamedProgramLocalParametersI4ivEXT(fallbackNamedProgramLocalParametersI4ivEXT),
	NamedProgramLocalParametersI4uivEXT(fallbackNamedProgramLocalParametersI4uivEXT),
	NamedProgramStringEXT(fallbackNamedProgramStringEXT),
	NamedRenderbufferStorageEXT(fallbackNamedRenderbufferStorageEXT),
	NamedRenderbufferStorageMultisampleCoverageEXT(fallbackNamedRenderbufferStorageMultisampleCoverageEXT),
	NamedRenderbufferStorageMultisampleEXT(fallbackNamedRenderbufferStorageMultisampleEXT),
	NamedStringARB(fallbackNamedStringARB),
	NewList(fallbackNewList),
	NewObjectBufferATI(fallbackNewObjectBufferATI),
	Normal3b(fallbackNormal3b),
	Normal3bv(fallbackNormal3bv),
	Normal3d(fallbackNormal3d),
	Normal3dv(fallbackNormal3dv),
	Normal3f(fallbackNormal3f),
	Normal3fVertex3fSUN(fallbackNormal3fVertex3fSUN),
	Normal3fVertex3fvSUN(fallbackNormal3fVertex3fvSUN),
	Normal3fv(fallbackNormal3fv),
	Normal3hNV(fallbackNormal3hNV),
	Normal3hvNV(fallbackNormal3hvNV),
	Normal3i(fallbackNormal3i),
	Normal3iv(fallbackNormal3iv),
	Normal3s(fallbackNormal3s),
	Normal3sv(fallbackNormal3sv),
	NormalFormatNV(fallbackNormalFormatNV),
	NormalP3ui(fallbackNormalP3ui),
	NormalP3uiv(fallbackNormalP3uiv),
	NormalPointer(fallbackNormalPointer),
	NormalPointerEXT(fallbackNormalPointerEXT),
	NormalPointerListIBM(fallbackNormalPointerListIBM),
	NormalPointervINTEL(fallbackNormalPointervINTEL),
	NormalStream3bATI(fallbackNormalStream3bATI),
	NormalStream3bvATI(fallbackNormalStream3bvATI),
	NormalStream3dATI(fallbackNormalStream3dATI),
	NormalStream3dvATI(fallbackNormalStream3dvATI),
	NormalStream3fATI(fallbackNormalStream3fATI),
	NormalStream3fvATI(fallbackNormalStream3fvATI),
	NormalStream3iATI(fallbackNormalStream3iATI),
	NormalStream3ivATI(fallbackNormalStream3ivATI),
	NormalStream3sATI(fallbackNormalStream3sATI),
	NormalStream3svATI(fallbackNormalStream3svATI),
	ObjectPurgeableAPPLE(fallbackObjectPurgeableAPPLE),
	ObjectUnpurgeableAPPLE(fallbackObjectUnpurgeableAPPLE),
	Ortho(fallbackOrtho),
	PNTrianglesfATI(fallbackPNTrianglesfATI),
	PNTrianglesiATI(fallbackPNTrianglesiATI),
	PassTexCoordATI(fallbackPassTexCoordATI),
	PassThrough(fallbackPassThrough),
	PatchParameterfv(fallbackPatchParameterfv),
	PatchParameteri(fallbackPatchParameteri),
	PauseTransformFeedback(fallbackPauseTransformFeedback),
	PauseTransformFeedbackNV(fallbackPauseTransformFeedbackNV),
	PixelDataRangeNV(fallbackPixelDataRangeNV),
	PixelMapfv(fallbackPixelMapfv),
	PixelMapuiv(fallbackPixelMapuiv),
	PixelMapusv(fallbackPixelMapusv),
	PixelStoref(fallbackPixelStoref),
	PixelStorei(fallbackPixelStorei),
	PixelTexGenParameterfSGIS(fallbackPixelTexGenParameterfSGIS),
	PixelTexGenParameterfvSGIS(fallbackPixelTexGenParameterfvSGIS),
	PixelTexGenParameteriSGIS(fallbackPixelTexGenParameteriSGIS),
	PixelTexGenParameterivSGIS(fallbackPixelTexGenParameterivSGIS),
	PixelTexGenSGIX(fallbackPixelTexGenSGIX),
	PixelTransferf(fallbackPixelTransferf),
	PixelTransferi(fallbackPixelTransferi),
	PixelTransformParameterfEXT(fallbackPixelTransformParameterfEXT),
	PixelTransformParameterfvEXT(fallbackPixelTransformParameterfvEXT),
	PixelTransformParameteriEXT(fallbackPixelTransformParameteriEXT),
	PixelTransformParameterivEXT(fallbackPixelTransformParameterivEXT),
	PixelZoom(fallbackPixelZoom),
	PointParameterf(fallbackPointParameterf),
	PointParameterfARB(fallbackPointParameterfARB),
	PointParameterfEXT(fallbackPointParameterfEXT),
	PointParameterfSGIS(fallbackPointParameterfSGIS),
	PointParameterfv(fallbackPointParameterfv),
	PointParameterfvARB(fallbackPointParameterfvARB),
	PointParameterfvEXT(fallbackPointParameterfvEXT),
	PointParameterfvSGIS(fallbackPointParameterfvSGIS),
	PointParameteri(fallbackPointParameteri),
	PointParameteriNV(fallbackPointParameteriNV),
	PointParameteriv(fallbackPointParameteriv),
	PointParameterivNV(fallbackPointParameterivNV),
	PointSize(fallbackPointSize),
	PollAsyncSGIX(fallbackPollAsyncSGIX),
	PollInstrumentsSGIX(fallbackPollInstrumentsSGIX),
	PolygonMode(fallbackPolygonMode),
	PolygonOffset(fallbackPolygonOffset),
	PolygonOffsetEXT(fallbackPolygonOffsetEXT),
	PolygonStipple(fallbackPolygonStipple),
	PopAttrib(fallbackPopAttrib),
	PopClientAttrib(fallbackPopClientAttrib),
	PopMatrix(fallbackPopMatrix),
	PopName(fallbackPopName),
	PresentFrameDualFillNV(fallbackPresentFrameDualFillNV),
	PresentFrameKeyedNV(fallbackPresentFrameKeyedNV),
	PrimitiveRestartIndex(fallbackPrimitiveRestartIndex),
	PrimitiveRestartIndexNV(fallbackPrimitiveRestartIndexNV),
	PrimitiveRestartNV(fallbackPrimitiveRestartNV),
	PrioritizeTextures(fallbackPrioritizeTextures),
	PrioritizeTexturesEXT(fallbackPrioritizeTexturesEXT),
	ProgramBufferParametersIivNV(fallbackProgramBufferParametersIivNV),
	ProgramBufferParametersIuivNV(fallbackProgramBufferParametersIuivNV),
	ProgramBufferParametersfvNV(fallbackProgramBufferParametersfvNV),
	ProgramEnvParameter4dARB(fallbackProgramEnvParameter4dARB),
	ProgramEnvParameter4dvARB(fallbackProgramEnvParameter4dvARB),
	ProgramEnvParameter4fARB(fallbackProgramEnvParameter4fARB),
	ProgramEnvParameter4fvARB(fallbackProgramEnvParameter4fvARB),
	ProgramEnvParameterI4iNV(fallbackProgramEnvParameterI4iNV),
	ProgramEnvParameterI4ivNV(fallbackProgramEnvParameterI4ivNV),
	ProgramEnvParameterI4uiNV(fallbackProgramEnvParameterI4uiNV),
	ProgramEnvParameterI4uivNV(fallbackProgramEnvParameterI4uivNV),
	ProgramEnvParameters4fvEXT(fallbackProgramEnvParameters4fvEXT),
	ProgramEnvParametersI4ivNV(fallbackProgramEnvParametersI4ivNV),
	ProgramEnvParametersI4uivNV(fallbackProgramEnvParametersI4uivNV),
	ProgramLocalParameter4dARB(fallbackProgramLocalParameter4dARB),
	ProgramLocalParameter4dvARB(fallbackProgramLocalParameter4dvARB),
	ProgramLocalParameter4fARB(fallbackProgramLocalParameter4fARB),
	ProgramLocalParameter4fvARB(fallbackProgramLocalParameter4fvARB),
	ProgramLocalParameterI4iNV(fallbackProgramLocalParameterI4iNV),
	ProgramLocalParameterI4ivNV(fallbackProgramLocalParameterI4ivNV),
	ProgramLocalParameterI4uiNV(fallbackProgramLocalParameterI4uiNV),
	ProgramLocalParameterI4uivNV(fallbackProgramLocalParameterI4uivNV),
	ProgramLocalParameters4fvEXT(fallbackProgramLocalParameters4fvEXT),
	ProgramLocalParametersI4ivNV(fallbackProgramLocalParametersI4ivNV),
	ProgramLocalParametersI4uivNV(fallbackProgramLocalParametersI4uivNV),
	ProgramNamedParameter4dNV(fallbackProgramNamedParameter4dNV),
	ProgramNamedParameter4dvNV(fallbackProgramNamedParameter4dvNV),
	ProgramNamedParameter4fNV(fallbackProgramNamedParameter4fNV),
	ProgramNamedParameter4fvNV(fallbackProgramNamedParameter4fvNV),
	ProgramParameter4dNV(fallbackProgramParameter4dNV),
	ProgramParameter4dvNV(fallbackProgramParameter4dvNV),
	ProgramParameter4fNV(fallbackProgramParameter4fNV),
	ProgramParameter4fvNV(fallbackProgramParameter4fvNV),
	ProgramParameteri(fallbackProgramParameteri),
	ProgramParameteriARB(fallbackProgramParameteriARB),
	ProgramParameteriEXT(fallbackProgramParameteriEXT),
	ProgramParameters4dvNV(fallbackProgramParameters4dvNV),
	ProgramParameters4fvNV(fallbackProgramParameters4fvNV),
	ProgramStringARB(fallbackProgramStringARB),
	ProgramUniform1dEXT(fallbackProgramUniform1dEXT),
	ProgramUniform1dvEXT(fallbackProgramUniform1dvEXT),
	ProgramUniform1fEXT(fallbackProgramUniform1fEXT),
	ProgramUniform1fvEXT(fallbackProgramUniform1fvEXT),
	ProgramUniform1iEXT(fallbackProgramUniform1iEXT),
	ProgramUniform1ivEXT(fallbackProgramUniform1ivEXT),
	ProgramUniform1uiEXT(fallbackProgramUniform1uiEXT),
	ProgramUniform1uivEXT(fallbackProgramUniform1uivEXT),
	ProgramUniform2dEXT(fallbackProgramUniform2dEXT),
	ProgramUniform2dvEXT(fallbackProgramUniform2dvEXT),
	ProgramUniform2fEXT(fallbackProgramUniform2fEXT),
	ProgramUniform2fvEXT(fallbackProgramUniform2fvEXT),
	ProgramUniform2iEXT(fallbackProgramUniform2iEXT),
	ProgramUniform2ivEXT(fallbackProgramUniform2ivEXT),
	ProgramUniform2uiEXT(fallbackProgramUniform2uiEXT),
	ProgramUniform2uivEXT(fallbackProgramUniform2uivEXT),
	ProgramUniform3dEXT(fallbackProgramUniform3dEXT),
	ProgramUniform3dvEXT(fallbackProgramUniform3dvEXT),
	ProgramUniform3fEXT(fallbackProgramUniform3fEXT),
	ProgramUniform3fvEXT(fallbackProgramUniform3fvEXT),
	ProgramUniform3iEXT(fallbackProgramUniform3iEXT),
	ProgramUniform3ivEXT(fallbackProgramUniform3ivEXT),
	ProgramUniform3uiEXT(fallbackProgramUniform3uiEXT),
	ProgramUniform3uivEXT(fallbackProgramUniform3uivEXT),
	ProgramUniform4dEXT(fallbackProgramUniform4dEXT),
	ProgramUniform4dvEXT(fallbackProgramUniform4dvEXT),
	ProgramUniform4fEXT(fallbackProgramUniform4fEXT),
	ProgramUniform4fvEXT(fallbackProgramUniform4fvEXT),
	ProgramUniform4iEXT(fallbackProgramUniform4iEXT),
	ProgramUniform4ivEXT(fallbackProgramUniform4ivEXT),
	ProgramUniform4uiEXT(fallbackProgramUniform4uiEXT),
	ProgramUniform4uivEXT(fallbackProgramUniform4uivEXT),
	ProgramUniformMatrix2dvEXT(fallbackProgramUniformMatrix2dvEXT),
	ProgramUniformMatrix2fvEXT(fallbackProgramUniformMatrix2fvEXT),
	ProgramUniformMatrix2x3dvEXT(fallbackProgramUniformMatrix2x3dvEXT),
	ProgramUniformMatrix2x3fvEXT(fallbackProgramUniformMatrix2x3fvEXT),
	ProgramUniformMatrix2x4dvEXT(fallbackProgramUniformMatrix2x4dvEXT),
	ProgramUniformMatrix2x4fvEXT(fallbackProgramUniformMatrix2x4fvEXT),
	ProgramUniformMatrix3dvEXT(fallbackProgramUniformMatrix3dvEXT),
	ProgramUniformMatrix3fvEXT(fallbackProgramUniformMatrix3fvEXT),
	ProgramUniformMatrix3x2dvEXT(fallbackProgramUniformMatrix3x2dvEXT),
	ProgramUniformMatrix3x2fvEXT(fallbackProgramUniformMatrix3x2fvEXT),
	ProgramUniformMatrix3x4dvEXT(fallbackProgramUniformMatrix3x4dvEXT),
	ProgramUniformMatrix3x4fvEXT(fallbackProgramUniformMatrix3x4fvEXT),
	ProgramUniformMatrix4dvEXT(fallbackProgramUniformMatrix4dvEXT),
	ProgramUniformMatrix4fvEXT(fallbackProgramUniformMatrix4fvEXT),
	ProgramUniformMatrix4x2dvEXT(fallbackProgramUniformMatrix4x2dvEXT),
	ProgramUniformMatrix4x2fvEXT(fallbackProgramUniformMatrix4x2fvEXT),
	ProgramUniformMatrix4x3dvEXT(fallbackProgramUniformMatrix4x3dvEXT),
	ProgramUniformMatrix4x3fvEXT(fallbackProgramUniformMatrix4x3fvEXT),
	ProgramUniformui64NV(fallbackProgramUniformui64NV),
	ProgramUniformui64vNV(fallbackProgramUniformui64vNV),
	ProgramVertexLimitNV(fallbackProgramVertexLimitNV),
	ProvokingVertex(fallbackProvokingVertex),
	ProvokingVertexEXT(fallbackProvokingVertexEXT),
	PushAttrib(fallbackPushAttrib),
	PushClientAttrib(fallbackPushClientAttrib),
	PushClientAttribDefaultEXT(fallbackPushClientAttribDefaultEXT),
	PushMatrix(fallbackPushMatrix),
	PushName(fallbackPushName),
	QueryCounter(fallbackQueryCounter),
	RasterPos2d(fallbackRasterPos2d),
	RasterPos2dv(fallbackRasterPos2dv),
	RasterPos2f(fallbackRasterPos2f),
	RasterPos2fv(fallbackRasterPos2fv),
	RasterPos2i(fallbackRasterPos2i),
	RasterPos2iv(fallbackRasterPos2iv),
	RasterPos2s(fallbackRasterPos2s),
	RasterPos2sv(fallbackRasterPos2sv),
	RasterPos3d(fallbackRasterPos3d),
	RasterPos3dv(fallbackRasterPos3dv),
	RasterPos3f(fallbackRasterPos3f),
	RasterPos3fv(fallbackRasterPos3fv),
	RasterPos3i(fallbackRasterPos3i),
	RasterPos3iv(fallbackRasterPos3iv),
	RasterPos3s(fallbackRasterPos3s),
	RasterPos3sv(fallbackRasterPos3sv),
	RasterPos4d(fallbackRasterPos4d),
	RasterPos4dv(fallbackRasterPos4dv),
	RasterPos4f(fallbackRasterPos4f),
	RasterPos4fv(fallbackRasterPos4fv),
	RasterPos4i(fallbackRasterPos4i),
	RasterPos4iv(fallbackRasterPos4iv),
	RasterPos4s(fallbackRasterPos4s),
	RasterPos4sv(fallbackRasterPos4sv),
	ReadBuffer(fallbackReadBuffer),
	ReadInstrumentsSGIX(fallbackReadInstrumentsSGIX),
	ReadPixels(fallbackReadPixels),
	Rectd(fallbackRectd),
	Rectdv(fallbackRectdv),
	Rectf(fallbackRectf),
	Rectfv(fallbackRectfv),
	Recti(fallbackRecti),
	Rectiv(fallbackRectiv),
	Rects(fallbackRects),
	Rectsv(fallbackRectsv),
	ReferencePlaneSGIX(fallbackReferencePlaneSGIX),
	RenderMode(fallbackRenderMode),
	RenderbufferStorage(fallbackRenderbufferStorage),
	RenderbufferStorageEXT(fallbackRenderbufferStorageEXT),
	RenderbufferStorageMultisample(fallbackRenderbufferStorageMultisample),
	RenderbufferStorageMultisampleCoverageNV(fallbackRenderbufferStorageMultisampleCoverageNV),
	RenderbufferStorageMultisampleEXT(fallbackRenderbufferStorageMultisampleEXT),
	ReplacementCodePointerSUN(fallbackReplacementCodePointerSUN),
	ReplacementCodeubSUN(fallbackReplacementCodeubSUN),
	ReplacementCodeubvSUN(fallbackReplacementCodeubvSUN),
	ReplacementCodeuiColor3fVertex3fSUN(fallbackReplacementCodeuiColor3fVertex3fSUN),
	ReplacementCodeuiColor3fVertex3fvSUN(fallbackReplacementCodeuiColor3fVertex3fvSUN),
	ReplacementCodeuiColor4fNormal3fVertex3fSUN(fallbackReplacementCodeuiColor4fNormal3fVertex3fSUN),
	ReplacementCodeuiColor4fNormal3fVertex3fvSUN(fallbackReplacementCodeuiColor4fNormal3fVertex3fvSUN),
	ReplacementCodeuiColor4ubVertex3fSUN(fallbackReplacementCodeuiColor4ubVertex3fSUN),
	ReplacementCodeuiColor4ubVertex3fvSUN(fallbackReplacementCodeuiColor4ubVertex3fvSUN),
	ReplacementCodeuiNormal3fVertex3fSUN(fallbackReplacementCodeuiNormal3fVertex3fSUN),
	ReplacementCodeuiNormal3fVertex3fvSUN(fallbackReplacementCodeuiNormal3fVertex3fvSUN),
	ReplacementCodeuiSUN(fallbackReplacementCodeuiSUN),
	ReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN(fallbackReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN),
	ReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN(fallbackReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN),
	ReplacementCodeuiTexCoord2fNormal3fVertex3fSUN(fallbackReplacementCodeuiTexCoord2fNormal3fVertex3fSUN),
	ReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN(fallbackReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN),
	ReplacementCodeuiTexCoord2fVertex3fSUN(fallbackReplacementCodeuiTexCoord2fVertex3fSUN),
	ReplacementCodeuiTexCoord2fVertex3fvSUN(fallbackReplacementCodeuiTexCoord2fVertex3fvSUN),
	ReplacementCodeuiVertex3fSUN(fallbackReplacementCodeuiVertex3fSUN),
	ReplacementCodeuiVertex3fvSUN(fallbackReplacementCodeuiVertex3fvSUN),
	ReplacementCodeuivSUN(fallbackReplacementCodeuivSUN),
	ReplacementCodeusSUN(fallbackReplacementCodeusSUN),
	ReplacementCodeusvSUN(fallbackReplacementCodeusvSUN),
	RequestResidentProgramsNV(fallbackRequestResidentProgramsNV),
	ResetHistogram(fallbackResetHistogram),
	ResetHistogramEXT(fallbackResetHistogramEXT),
	ResetMinmax(fallbackResetMinmax),
	ResetMinmaxEXT(fallbackResetMinmaxEXT),
	ResizeBuffersMESA(fallbackResizeBuffersMESA),
	ResumeTransformFeedback(fallbackResumeTransformFeedback),
	ResumeTransformFeedbackNV(fallbackResumeTransformFeedbackNV),
	Rotated(fallbackRotated),
	Rotatef(fallbackRotatef),
	SampleCoverage(fallbackSampleCoverage),
	SampleCoverageARB(fallbackSampleCoverageARB),
	SampleMapATI(fallbackSampleMapATI),
	SampleMaskEXT(fallbackSampleMaskEXT),
	SampleMaskIndexedNV(fallbackSampleMaskIndexedNV),
	SampleMaskSGIS(fallbackSampleMaskSGIS),
	SampleMaski(fallbackSampleMaski),
	SamplePatternEXT(fallbackSamplePatternEXT),
	SamplePatternSGIS(fallbackSamplePatternSGIS),
	SamplerParameterIiv(fallbackSamplerParameterIiv),
	SamplerParameterIuiv(fallbackSamplerParameterIuiv),
	SamplerParameterf(fallbackSamplerParameterf),
	SamplerParameterfv(fallbackSamplerParameterfv),
	SamplerParameteri(fallbackSamplerParameteri),
	SamplerParameteriv(fallbackSamplerParameteriv),
	Scaled(fallbackScaled),
	Scalef(fallbackScalef),
	Scissor(fallbackScissor),
	SecondaryColor3b(fallbackSecondaryColor3b),
	SecondaryColor3bEXT(fallbackSecondaryColor3bEXT),
	SecondaryColor3bv(fallbackSecondaryColor3bv),
	SecondaryColor3bvEXT(fallbackSecondaryColor3bvEXT),
	SecondaryColor3d(fallbackSecondaryColor3d),
	SecondaryColor3dEXT(fallbackSecondaryColor3dEXT),
	SecondaryColor3dv(fallbackSecondaryColor3dv),
	SecondaryColor3dvEXT(fallbackSecondaryColor3dvEXT),
	SecondaryColor3f(fallbackSecondaryColor3f),
	SecondaryColor3fEXT(fallbackSecondaryColor3fEXT),
	SecondaryColor3fv(fallbackSecondaryColor3fv),
	SecondaryColor3fvEXT(fallbackSecondaryColor3fvEXT),
	SecondaryColor3hNV(fallbackSecondaryColor3hNV),
	SecondaryColor3hvNV(fallbackSecondaryColor3hvNV),
	SecondaryColor3i(fallbackSecondaryColor3i),
	SecondaryColor3iEXT(fallbackSecondaryColor3iEXT),
	SecondaryColor3iv(fallbackSecondaryColor3iv),
	SecondaryColor3ivEXT(fallbackSecondaryColor3ivEXT),
	SecondaryColor3s(fallbackSecondaryColor3s),
	SecondaryColor3sEXT(fallbackSecondaryColor3sEXT),
	SecondaryColor3sv(fallbackSecondaryColor3sv),
	SecondaryColor3svEXT(fallbackSecondaryColor3svEXT),
	SecondaryColor3ub(fallbackSecondaryColor3ub),
	SecondaryColor3ubEXT(fallbackSecondaryColor3ubEXT),
	SecondaryColor3ubv(fallbackSecondaryColor3ubv),
	SecondaryColor3ubvEXT(fallbackSecondaryColor3ubvEXT),
	SecondaryColor3ui(fallbackSecondaryColor3ui),
	SecondaryColor3uiEXT(fallbackSecondaryColor3uiEXT),
	SecondaryColor3uiv(fallbackSecondaryColor3uiv),
	SecondaryColor3uivEXT(fallbackSecondaryColor3uivEXT),
	SecondaryColor3us(fallbackSecondaryColor3us),
	SecondaryColor3usEXT(fallbackSecondaryColor3usEXT),
	SecondaryColor3usv(fallbackSecondaryColor3usv),
	SecondaryColor3usvEXT(fallbackSecondaryColor3usvEXT),
	SecondaryColorFormatNV(fallbackSecondaryColorFormatNV),
	SecondaryColorP3ui(fallbackSecondaryColorP3ui),
	SecondaryColorP3uiv(fallbackSecondaryColorP3uiv),
	SecondaryColorPointer(fallbackSecondaryColorPointer),
	SecondaryColorPointerEXT(fallbackSecondaryColorPointerEXT),
	SecondaryColorPointerListIBM(fallbackSecondaryColorPointerListIBM),
	SelectBuffer(fallbackSelectBuffer),
	SelectPerfMonitorCountersAMD(fallbackSelectPerfMonitorCountersAMD),
	SeparableFilter2D(fallbackSeparableFilter2D),
	SeparableFilter2DEXT(fallbackSeparableFilter2DEXT),
	SetFenceAPPLE(fallbackSetFenceAPPLE),
	SetFenceNV(fallbackSetFenceNV),
	SetFragmentShaderConstantATI(fallbackSetFragmentShaderConstantATI),
	SetInvariantEXT(fallbackSetInvariantEXT),
	SetLocalConstantEXT(fallbackSetLocalConstantEXT),
	ShadeModel(fallbackShadeModel),
	ShaderOp1EXT(fallbackShaderOp1EXT),
	ShaderOp2EXT(fallbackShaderOp2EXT),
	ShaderOp3EXT(fallbackShaderOp3EXT),
	ShaderSource(fallbackShaderSource),
	ShaderSourceARB(fallbackShaderSourceARB),
	SharpenTexFuncSGIS(fallbackSharpenTexFuncSGIS),
	SpriteParameterfSGIX(fallbackSpriteParameterfSGIX),
	SpriteParameterfvSGIX(fallbackSpriteParameterfvSGIX),
	SpriteParameteriSGIX(fallbackSpriteParameteriSGIX),
	SpriteParameterivSGIX(fallbackSpriteParameterivSGIX),
	StartInstrumentsSGIX(fallbackStartInstrumentsSGIX),
	StencilClearTagEXT(fallbackStencilClearTagEXT),
	StencilFunc(fallbackStencilFunc),
	StencilFuncSeparate(fallbackStencilFuncSeparate),
	StencilFuncSeparateATI(fallbackStencilFuncSeparateATI),
	StencilMask(fallbackStencilMask),
	StencilMaskSeparate(fallbackStencilMaskSeparate),
	StencilOp(fallbackStencilOp),
	StencilOpSeparate(fallbackStencilOpSeparate),
	StencilOpSeparateATI(fallbackStencilOpSeparateATI),
	StopInstrumentsSGIX(fallbackStopInstrumentsSGIX),
	StringMarkerGREMEDY(fallbackStringMarkerGREMEDY),
	SwizzleEXT(fallbackSwizzleEXT),
	TagSampleBufferSGIX(fallbackTagSampleBufferSGIX),
	Tangent3bEXT(fallbackTangent3bEXT),
	Tangent3bvEXT(fallbackTangent3bvEXT),
	Tangent3dEXT(fallbackTangent3dEXT),
	Tangent3dvEXT(fallbackTangent3dvEXT),
	Tangent3fEXT(fallbackTangent3fEXT),
	Tangent3fvEXT(fallbackTangent3fvEXT),
	Tangent3iEXT(fallbackTangent3iEXT),
	Tangent3ivEXT(fallbackTangent3ivEXT),
	Tangent3sEXT(fallbackTangent3sEXT),
	Tangent3svEXT(fallbackTangent3svEXT),
	TangentPointerEXT(fallbackTangentPointerEXT),
	TbufferMask3DFX(fallbackTbufferMask3DFX),
	TessellationFactorAMD(fallbackTessellationFactorAMD),
	TessellationModeAMD(fallbackTessellationModeAMD),
	TestFenceAPPLE(fallbackTestFenceAPPLE),
	TestFenceNV(fallbackTestFenceNV),
	TestObjectAPPLE(fallbackTestObjectAPPLE),
	TexBuffer(fallbackTexBuffer),
	TexBufferARB(fallbackTexBufferARB),
	TexBufferEXT(fallbackTexBufferEXT),
	TexBumpParameterfvATI(fallbackTexBumpParameterfvATI),
	TexBumpParameterivATI(fallbackTexBumpParameterivATI),
	TexCoord1d(fallbackTexCoord1d),
	TexCoord1dv(fallbackTexCoord1dv),
	TexCoord1f(fallbackTexCoord1f),
	TexCoord1fv(fallbackTexCoord1fv),
	TexCoord1hNV(fallbackTexCoord1hNV),
	TexCoord1hvNV(fallbackTexCoord1hvNV),
	TexCoord1i(fallbackTexCoord1i),
	TexCoord1iv(fallbackTexCoord1iv),
	TexCoord1s(fallbackTexCoord1s),
	TexCoord1sv(fallbackTexCoord1sv),
	TexCoord2d(fallbackTexCoord2d),
	TexCoord2dv(fallbackTexCoord2dv),
	TexCoord2f(fallbackTexCoord2f),
	TexCoord2fColor3fVertex3fSUN(fallbackTexCoord2fColor3fVertex3fSUN),
	TexCoord2fColor3fVertex3fvSUN(fallbackTexCoord2fColor3fVertex3fvSUN),
	TexCoord2fColor4fNormal3fVertex3fSUN(fallbackTexCoord2fColor4fNormal3fVertex3fSUN),
	TexCoord2fColor4fNormal3fVertex3fvSUN(fallbackTexCoord2fColor4fNormal3fVertex3fvSUN),
	TexCoord2fColor4ubVertex3fSUN(fallbackTexCoord2fColor4ubVertex3fSUN),
	TexCoord2fColor4ubVertex3fvSUN(fallbackTexCoord2fColor4ubVertex3fvSUN),
	TexCoord2fNormal3fVertex3fSUN(fallbackTexCoord2fNormal3fVertex3fSUN),
	TexCoord2fNormal3fVertex3fvSUN(fallbackTexCoord2fNormal3fVertex3fvSUN),
	TexCoord2fVertex3fSUN(fallbackTexCoord2fVertex3fSUN),
	TexCoord2fVertex3fvSUN(fallbackTexCoord2fVertex3fvSUN),
	TexCoord2fv(fallbackTexCoord2fv),
	TexCoord2hNV(fallbackTexCoord2hNV),
	TexCoord2hvNV(fallbackTexCoord2hvNV),
	TexCoord2i(fallbackTexCoord2i),
	TexCoord2iv(fallbackTexCoord2iv),
	TexCoord2s(fallbackTexCoord2s),
	TexCoord2sv(fallbackTexCoord2sv),
	TexCoord3d(fallbackTexCoord3d),
	TexCoord3dv(fallbackTexCoord3dv),
	TexCoord3f(fallbackTexCoord3f),
	TexCoord3fv(fallbackTexCoord3fv),
	TexCoord3hNV(fallbackTexCoord3hNV),
	TexCoord3hvNV(fallbackTexCoord3hvNV),
	TexCoord3i(fallbackTexCoord3i),
	TexCoord3iv(fallbackTexCoord3iv),
	TexCoord3s(fallbackTexCoord3s),
	TexCoord3sv(fallbackTexCoord3sv),
	TexCoord4d(fallbackTexCoord4d),
	TexCoord4dv(fallbackTexCoord4dv),
	TexCoord4f(fallbackTexCoord4f),
	TexCoord4fColor4fNormal3fVertex4fSUN(fallbackTexCoord4fColor4fNormal3fVertex4fSUN),
	TexCoord4fColor4fNormal3fVertex4fvSUN(fallbackTexCoord4fColor4fNormal3fVertex4fvSUN),
	TexCoord4fVertex4fSUN(fallbackTexCoord4fVertex4fSUN),
	TexCoord4fVertex4fvSUN(fallbackTexCoord4fVertex4fvSUN),
	TexCoord4fv(fallbackTexCoord4fv),
	TexCoord4hNV(fallbackTexCoord4hNV),
	TexCoord4hvNV(fallbackTexCoord4hvNV),
	TexCoord4i(fallbackTexCoord4i),
	TexCoord4iv(fallbackTexCoord4iv),
	TexCoord4s(fallbackTexCoord4s),
	TexCoord4sv(fallbackTexCoord4sv),
	TexCoordFormatNV(fallbackTexCoordFormatNV),
	TexCoordP1ui(fallbackTexCoordP1ui),
	TexCoordP1uiv(fallbackTexCoordP1uiv),
	TexCoordP2ui(fallbackTexCoordP2ui),
	TexCoordP2uiv(fallbackTexCoordP2uiv),
	TexCoordP3ui(fallbackTexCoordP3ui),
	TexCoordP3uiv(fallbackTexCoordP3uiv),
	TexCoordP4ui(fallbackTexCoordP4ui),
	TexCoordP4uiv(fallbackTexCoordP4uiv),
	TexCoordPointer(fallbackTexCoordPointer),
	TexCoordPointerEXT(fallbackTexCoordPointerEXT),
	TexCoordPointerListIBM(fallbackTexCoordPointerListIBM),
	TexCoordPointervINTEL(fallbackTexCoordPointervINTEL),
	TexEnvf(fallbackTexEnvf),
	TexEnvfv(fallbackTexEnvfv),
	TexEnvi(fallbackTexEnvi),
	TexEnviv(fallbackTexEnviv),
	TexFilterFuncSGIS(fallbackTexFilterFuncSGIS),
	TexGend(fallbackTexGend),
	TexGendv(fallbackTexGendv),
	TexGenf(fallbackTexGenf),
	TexGenfv(fallbackTexGenfv),
	TexGeni(fallbackTexGeni),
	TexGeniv(fallbackTexGeniv),
	TexImage1D(fallbackTexImage1D),
	TexImage2D(fallbackTexImage2D),
	TexImage2DMultisample(fallbackTexImage2DMultisample),
	TexImage3D(fallbackTexImage3D),
	TexImage3DEXT(fallbackTexImage3DEXT),
	TexImage3DMultisample(fallbackTexImage3DMultisample),
	TexImage4DSGIS(fallbackTexImage4DSGIS),
	TexParameterIiv(fallbackTexParameterIiv),
	TexParameterIivEXT(fallbackTexParameterIivEXT),
	TexParameterIuiv(fallbackTexParameterIuiv),
	TexParameterIuivEXT(fallbackTexParameterIuivEXT),
	TexParameterf(fallbackTexParameterf),
	TexParameterfv(fallbackTexParameterfv),
	TexParameteri(fallbackTexParameteri),
	TexParameteriv(fallbackTexParameteriv),
	TexRenderbufferNV(fallbackTexRenderbufferNV),
	TexSubImage1D(fallbackTexSubImage1D),
	TexSubImage1DEXT(fallbackTexSubImage1DEXT),
	TexSubImage2D(fallbackTexSubImage2D),
	TexSubImage2DEXT(fallbackTexSubImage2DEXT),
	TexSubImage3D(fallbackTexSubImage3D),
	TexSubImage3DEXT(fallbackTexSubImage3DEXT),
	TexSubImage4DSGIS(fallbackTexSubImage4DSGIS),
	TextureBarrierNV(fallbackTextureBarrierNV),
	TextureBufferEXT(fallbackTextureBufferEXT),
	TextureColorMaskSGIS(fallbackTextureColorMaskSGIS),
	TextureImage1DEXT(fallbackTextureImage1DEXT),
	TextureImage2DEXT(fallbackTextureImage2DEXT),
	TextureImage3DEXT(fallbackTextureImage3DEXT),
	TextureLightEXT(fallbackTextureLightEXT),
	TextureMaterialEXT(fallbackTextureMaterialEXT),
	TextureNormalEXT(fallbackTextureNormalEXT),
	TextureParameterIivEXT(fallbackTextureParameterIivEXT),
	TextureParameterIuivEXT(fallbackTextureParameterIuivEXT),
	TextureParameterfEXT(fallbackTextureParameterfEXT),
	TextureParameterfvEXT(fallbackTextureParameterfvEXT),
	TextureParameteriEXT(fallbackTextureParameteriEXT),
	TextureParameterivEXT(fallbackTextureParameterivEXT),
	TextureRangeAPPLE(fallbackTextureRangeAPPLE),
	TextureRenderbufferEXT(fallbackTextureRenderbufferEXT),
	TextureSubImage1DEXT(fallbackTextureSubImage1DEXT),
	TextureSubImage2DEXT(fallbackTextureSubImage2DEXT),
	TextureSubImage3DEXT(fallbackTextureSubImage3DEXT),
	TrackMatrixNV(fallbackTrackMatrixNV),
	TransformFeedbackAttribsNV(fallbackTransformFeedbackAttribsNV),
	TransformFeedbackVaryings(fallbackTransformFeedbackVaryings),
	TransformFeedbackVaryingsEXT(fallbackTransformFeedbackVaryingsEXT),
	TransformFeedbackVaryingsNV(fallbackTransformFeedbackVaryingsNV),
	Translated(fallbackTranslated),
	Translatef(fallbackTranslatef),
	Uniform1d(fallbackUniform1d),
	Uniform1dv(fallbackUniform1dv),
	Uniform1f(fallbackUniform1f),
	Uniform1fARB(fallbackUniform1fARB),
	Uniform1fv(fallbackUniform1fv),
	Uniform1fvARB(fallbackUniform1fvARB),
	Uniform1i(fallbackUniform1i),
	Uniform1iARB(fallbackUniform1iARB),
	Uniform1iv(fallbackUniform1iv),
	Uniform1ivARB(fallbackUniform1ivARB),
	Uniform1ui(fallbackUniform1ui),
	Uniform1uiEXT(fallbackUniform1uiEXT),
	Uniform1uiv(fallbackUniform1uiv),
	Uniform1uivEXT(fallbackUniform1uivEXT),
	Uniform2d(fallbackUniform2d),
	Uniform2dv(fallbackUniform2dv),
	Uniform2f(fallbackUniform2f),
	Uniform2fARB(fallbackUniform2fARB),
	Uniform2fv(fallbackUniform2fv),
	Uniform2fvARB(fallbackUniform2fvARB),
	Uniform2i(fallbackUniform2i),
	Uniform2iARB(fallbackUniform2iARB),
	Uniform2iv(fallbackUniform2iv),
	Uniform2ivARB(fallbackUniform2ivARB),
	Uniform2ui(fallbackUniform2ui),
	Uniform2uiEXT(fallbackUniform2uiEXT),
	Uniform2uiv(fallbackUniform2uiv),
	Uniform2uivEXT(fallbackUniform2uivEXT),
	Uniform3d(fallbackUniform3d),
	Uniform3dv(fallbackUniform3dv),
	Uniform3f(fallbackUniform3f),
	Uniform3fARB(fallbackUniform3fARB),
	Uniform3fv(fallbackUniform3fv),
	Uniform3fvARB(fallbackUniform3fvARB),
	Uniform3i(fallbackUniform3i),
	Uniform3iARB(fallbackUniform3iARB),
	Uniform3iv(fallbackUniform3iv),
	Uniform3ivARB(fallbackUniform3ivARB),
	Uniform3ui(fallbackUniform3ui),
	Uniform3uiEXT(fallbackUniform3uiEXT),
	Uniform3uiv(fallbackUniform3uiv),
	Uniform3uivEXT(fallbackUniform3uivEXT),
	Uniform4d(fallbackUniform4d),
	Uniform4dv(fallbackUniform4dv),
	Uniform4f(fallbackUniform4f),
	Uniform4fARB(fallbackUniform4fARB),
	Uniform4fv(fallbackUniform4fv),
	Uniform4fvARB(fallbackUniform4fvARB),
	Uniform4i(fallbackUniform4i),
	Uniform4iARB(fallbackUniform4iARB),
	Uniform4iv(fallbackUniform4iv),
	Uniform4ivARB(fallbackUniform4ivARB),
	Uniform4ui(fallbackUniform4ui),
	Uniform4uiEXT(fallbackUniform4uiEXT),
	Uniform4uiv(fallbackUniform4uiv),
	Uniform4uivEXT(fallbackUniform4uivEXT),
	UniformBlockBinding(fallbackUniformBlockBinding),
	UniformBufferEXT(fallbackUniformBufferEXT),
	UniformMatrix2dv(fallbackUniformMatrix2dv),
	UniformMatrix2fv(fallbackUniformMatrix2fv),
	UniformMatrix2fvARB(fallbackUniformMatrix2fvARB),
	UniformMatrix2x3dv(fallbackUniformMatrix2x3dv),
	UniformMatrix2x3fv(fallbackUniformMatrix2x3fv),
	UniformMatrix2x4dv(fallbackUniformMatrix2x4dv),
	UniformMatrix2x4fv(fallbackUniformMatrix2x4fv),
	UniformMatrix3dv(fallbackUniformMatrix3dv),
	UniformMatrix3fv(fallbackUniformMatrix3fv),
	UniformMatrix3fvARB(fallbackUniformMatrix3fvARB),
	UniformMatrix3x2dv(fallbackUniformMatrix3x2dv),
	UniformMatrix3x2fv(fallbackUniformMatrix3x2fv),
	UniformMatrix3x4dv(fallbackUniformMatrix3x4dv),
	UniformMatrix3x4fv(fallbackUniformMatrix3x4fv),
	UniformMatrix4dv(fallbackUniformMatrix4dv),
	UniformMatrix4fv(fallbackUniformMatrix4fv),
	UniformMatrix4fvARB(fallbackUniformMatrix4fvARB),
	UniformMatrix4x2dv(fallbackUniformMatrix4x2dv),
	UniformMatrix4x2fv(fallbackUniformMatrix4x2fv),
	UniformMatrix4x3dv(fallbackUniformMatrix4x3dv),
	UniformMatrix4x3fv(fallbackUniformMatrix4x3fv),
	UniformSubroutinesuiv(fallbackUniformSubroutinesuiv),
	Uniformui64NV(fallbackUniformui64NV),
	Uniformui64vNV(fallbackUniformui64vNV),
	UnlockArraysEXT(fallbackUnlockArraysEXT),
	UnmapBuffer(fallbackUnmapBuffer),
	UnmapBufferARB(fallbackUnmapBufferARB),
	UnmapNamedBufferEXT(fallbackUnmapNamedBufferEXT),
	UnmapObjectBufferATI(fallbackUnmapObjectBufferATI),
	UpdateObjectBufferATI(fallbackUpdateObjectBufferATI),
	UseProgram(fallbackUseProgram),
	UseProgramObjectARB(fallbackUseProgramObjectARB),
	UseShaderProgramEXT(fallbackUseShaderProgramEXT),
	ValidateProgram(fallbackValidateProgram),
	ValidateProgramARB(fallbackValidateProgramARB),
	VariantArrayObjectATI(fallbackVariantArrayObjectATI),
	VariantPointerEXT(fallbackVariantPointerEXT),
	VariantbvEXT(fallbackVariantbvEXT),
	VariantdvEXT(fallbackVariantdvEXT),
	VariantfvEXT(fallbackVariantfvEXT),
	VariantivEXT(fallbackVariantivEXT),
	VariantsvEXT(fallbackVariantsvEXT),
	VariantubvEXT(fallbackVariantubvEXT),
	VariantuivEXT(fallbackVariantuivEXT),
	VariantusvEXT(fallbackVariantusvEXT),
	Vertex2d(fallbackVertex2d),
	Vertex2dv(fallbackVertex2dv),
	Vertex2f(fallbackVertex2f),
	Vertex2fv(fallbackVertex2fv),
	Vertex2hNV(fallbackVertex2hNV),
	Vertex2hvNV(fallbackVertex2hvNV),
	Vertex2i(fallbackVertex2i),
	Vertex2iv(fallbackVertex2iv),
	Vertex2s(fallbackVertex2s),
	Vertex2sv(fallbackVertex2sv),
	Vertex3d(fallbackVertex3d),
	Vertex3dv(fallbackVertex3dv),
	Vertex3f(fallbackVertex3f),
	Vertex3fv(fallbackVertex3fv),
	Vertex3hNV(fallbackVertex3hNV),
	Vertex3hvNV(fallbackVertex3hvNV),
	Vertex3i(fallbackVertex3i),
	Vertex3iv(fallbackVertex3iv),
	Vertex3s(fallbackVertex3s),
	Vertex3sv(fallbackVertex3sv),
	Vertex4d(fallbackVertex4d),
	Vertex4dv(fallbackVertex4dv),
	Vertex4f(fallbackVertex4f),
	Vertex4fv(fallbackVertex4fv),
	Vertex4hNV(fallbackVertex4hNV),
	Vertex4hvNV(fallbackVertex4hvNV),
	Vertex4i(fallbackVertex4i),
	Vertex4iv(fallbackVertex4iv),
	Vertex4s(fallbackVertex4s),
	Vertex4sv(fallbackVertex4sv),
	VertexArrayParameteriAPPLE(fallbackVertexArrayParameteriAPPLE),
	VertexArrayRangeAPPLE(fallbackVertexArrayRangeAPPLE),
	VertexArrayRangeNV(fallbackVertexArrayRangeNV),
	VertexAttrib1d(fallbackVertexAttrib1d),
	VertexAttrib1dARB(fallbackVertexAttrib1dARB),
	VertexAttrib1dNV(fallbackVertexAttrib1dNV),
	VertexAttrib1dv(fallbackVertexAttrib1dv),
	VertexAttrib1dvARB(fallbackVertexAttrib1dvARB),
	VertexAttrib1dvNV(fallbackVertexAttrib1dvNV),
	VertexAttrib1f(fallbackVertexAttrib1f),
	VertexAttrib1fARB(fallbackVertexAttrib1fARB),
	VertexAttrib1fNV(fallbackVertexAttrib1fNV),
	VertexAttrib1fv(fallbackVertexAttrib1fv),
	VertexAttrib1fvARB(fallbackVertexAttrib1fvARB),
	VertexAttrib1fvNV(fallbackVertexAttrib1fvNV),
	VertexAttrib1hNV(fallbackVertexAttrib1hNV),
	VertexAttrib1hvNV(fallbackVertexAttrib1hvNV),
	VertexAttrib1s(fallbackVertexAttrib1s),
	VertexAttrib1sARB(fallbackVertexAttrib1sARB),
	VertexAttrib1sNV(fallbackVertexAttrib1sNV),
	VertexAttrib1sv(fallbackVertexAttrib1sv),
	VertexAttrib1svARB(fallbackVertexAttrib1svARB),
	VertexAttrib1svNV(fallbackVertexAttrib1svNV),
	VertexAttrib2d(fallbackVertexAttrib2d),
	VertexAttrib2dARB(fallbackVertexAttrib2dARB),
	VertexAttrib2dNV(fallbackVertexAttrib2dNV),
	VertexAttrib2dv(fallbackVertexAttrib2dv),
	VertexAttrib2dvARB(fallbackVertexAttrib2dvARB),
	VertexAttrib2dvNV(fallbackVertexAttrib2dvNV),
	VertexAttrib2f(fallbackVertexAttrib2f),
	VertexAttrib2fARB(fallbackVertexAttrib2fARB),
	VertexAttrib2fNV(fallbackVertexAttrib2fNV),
	VertexAttrib2fv(fallbackVertexAttrib2fv),
	VertexAttrib2fvARB(fallbackVertexAttrib2fvARB),
	VertexAttrib2fvNV(fallbackVertexAttrib2fvNV),
	VertexAttrib2hNV(fallbackVertexAttrib2hNV),
	VertexAttrib2hvNV(fallbackVertexAttrib2hvNV),
	VertexAttrib2s(fallbackVertexAttrib2s),
	VertexAttrib2sARB(fallbackVertexAttrib2sARB),
	VertexAttrib2sNV(fallbackVertexAttrib2sNV),
	VertexAttrib2sv(fallbackVertexAttrib2sv),
	VertexAttrib2svARB(fallbackVertexAttrib2svARB),
	VertexAttrib2svNV(fallbackVertexAttrib2svNV),
	VertexAttrib3d(fallbackVertexAttrib3d),
	VertexAttrib3dARB(fallbackVertexAttrib3dARB),
	VertexAttrib3dNV(fallbackVertexAttrib3dNV),
	VertexAttrib3dv(fallbackVertexAttrib3dv),
	VertexAttrib3dvARB(fallbackVertexAttrib3dvARB),
	VertexAttrib3dvNV(fallbackVertexAttrib3dvNV),
	VertexAttrib3f(fallbackVertexAttrib3f),
	VertexAttrib3fARB(fallbackVertexAttrib3fARB),
	VertexAttrib3fNV(fallbackVertexAttrib3fNV),
	VertexAttrib3fv(fallbackVertexAttrib3fv),
	VertexAttrib3fvARB(fallbackVertexAttrib3fvARB),
	VertexAttrib3fvNV(fallbackVertexAttrib3fvNV),
	VertexAttrib3hNV(fallbackVertexAttrib3hNV),
	VertexAttrib3hvNV(fallbackVertexAttrib3hvNV),
	VertexAttrib3s(fallbackVertexAttrib3s),
	VertexAttrib3sARB(fallbackVertexAttrib3sARB),
	VertexAttrib3sNV(fallbackVertexAttrib3sNV),
	VertexAttrib3sv(fallbackVertexAttrib3sv),
	VertexAttrib3svARB(fallbackVertexAttrib3svARB),
	VertexAttrib3svNV(fallbackVertexAttrib3svNV),
	VertexAttrib4Nbv(fallbackVertexAttrib4Nbv),
	VertexAttrib4NbvARB(fallbackVertexAttrib4NbvARB),
	VertexAttrib4Niv(fallbackVertexAttrib4Niv),
	VertexAttrib4NivARB(fallbackVertexAttrib4NivARB),
	VertexAttrib4Nsv(fallbackVertexAttrib4Nsv),
	VertexAttrib4NsvARB(fallbackVertexAttrib4NsvARB),
	VertexAttrib4Nub(fallbackVertexAttrib4Nub),
	VertexAttrib4NubARB(fallbackVertexAttrib4NubARB),
	VertexAttrib4Nubv(fallbackVertexAttrib4Nubv),
	VertexAttrib4NubvARB(fallbackVertexAttrib4NubvARB),
	VertexAttrib4Nuiv(fallbackVertexAttrib4Nuiv),
	VertexAttrib4NuivARB(fallbackVertexAttrib4NuivARB),
	VertexAttrib4Nusv(fallbackVertexAttrib4Nusv),
	VertexAttrib4NusvARB(fallbackVertexAttrib4NusvARB),
	VertexAttrib4bv(fallbackVertexAttrib4bv),
	VertexAttrib4bvARB(fallbackVertexAttrib4bvARB),
	VertexAttrib4d(fallbackVertexAttrib4d),
	VertexAttrib4dARB(fallbackVertexAttrib4dARB),
	VertexAttrib4dNV(fallbackVertexAttrib4dNV),
	VertexAttrib4dv(fallbackVertexAttrib4dv),
	VertexAttrib4dvARB(fallbackVertexAttrib4dvARB),
	VertexAttrib4dvNV(fallbackVertexAttrib4dvNV),
	VertexAttrib4f(fallbackVertexAttrib4f),
	VertexAttrib4fARB(fallbackVertexAttrib4fARB),
	VertexAttrib4fNV(fallbackVertexAttrib4fNV),
	VertexAttrib4fv(fallbackVertexAttrib4fv),
	VertexAttrib4fvARB(fallbackVertexAttrib4fvARB),
	VertexAttrib4fvNV(fallbackVertexAttrib4fvNV),
	VertexAttrib4hNV(fallbackVertexAttrib4hNV),
	VertexAttrib4hvNV(fallbackVertexAttrib4hvNV),
	VertexAttrib4iv(fallbackVertexAttrib4iv),
	VertexAttrib4ivARB(fallbackVertexAttrib4ivARB),
	VertexAttrib4s(fallbackVertexAttrib4s),
	VertexAttrib4sARB(fallbackVertexAttrib4sARB),
	VertexAttrib4sNV(fallbackVertexAttrib4sNV),
	VertexAttrib4sv(fallbackVertexAttrib4sv),
	VertexAttrib4svARB(fallbackVertexAttrib4svARB),
	VertexAttrib4svNV(fallbackVertexAttrib4svNV),
	VertexAttrib4ubNV(fallbackVertexAttrib4ubNV),
	VertexAttrib4ubv(fallbackVertexAttrib4ubv),
	VertexAttrib4ubvARB(fallbackVertexAttrib4ubvARB),
	VertexAttrib4ubvNV(fallbackVertexAttrib4ubvNV),
	VertexAttrib4uiv(fallbackVertexAttrib4uiv),
	VertexAttrib4uivARB(fallbackVertexAttrib4uivARB),
	VertexAttrib4usv(fallbackVertexAttrib4usv),
	VertexAttrib4usvARB(fallbackVertexAttrib4usvARB),
	VertexAttribArrayObjectATI(fallbackVertexAttribArrayObjectATI),
	VertexAttribDivisorARB(fallbackVertexAttribDivisorARB),
	VertexAttribFormatNV(fallbackVertexAttribFormatNV),
	VertexAttribI1i(fallbackVertexAttribI1i),
	VertexAttribI1iEXT(fallbackVertexAttribI1iEXT),
	VertexAttribI1iv(fallbackVertexAttribI1iv),
	VertexAttribI1ivEXT(fallbackVertexAttribI1ivEXT),
	VertexAttribI1ui(fallbackVertexAttribI1ui),
	VertexAttribI1uiEXT(fallbackVertexAttribI1uiEXT),
	VertexAttribI1uiv(fallbackVertexAttribI1uiv),
	VertexAttribI1uivEXT(fallbackVertexAttribI1uivEXT),
	VertexAttribI2i(fallbackVertexAttribI2i),
	VertexAttribI2iEXT(fallbackVertexAttribI2iEXT),
	VertexAttribI2iv(fallbackVertexAttribI2iv),
	VertexAttribI2ivEXT(fallbackVertexAttribI2ivEXT),
	VertexAttribI2ui(fallbackVertexAttribI2ui),
	VertexAttribI2uiEXT(fallbackVertexAttribI2uiEXT),
	VertexAttribI2uiv(fallbackVertexAttribI2uiv),
	VertexAttribI2uivEXT(fallbackVertexAttribI2uivEXT),
	VertexAttribI3i(fallbackVertexAttribI3i),
	VertexAttribI3iEXT(fallbackVertexAttribI3iEXT),
	VertexAttribI3iv(fallbackVertexAttribI3iv),
	VertexAttribI3ivEXT(fallbackVertexAttribI3ivEXT),
	VertexAttribI3ui(fallbackVertexAttribI3ui),
	VertexAttribI3uiEXT(fallbackVertexAttribI3uiEXT),
	VertexAttribI3uiv(fallbackVertexAttribI3uiv),
	VertexAttribI3uivEXT(fallbackVertexAttribI3uivEXT),
	VertexAttribI4bv(fallbackVertexAttribI4bv),
	VertexAttribI4bvEXT(fallbackVertexAttribI4bvEXT),
	VertexAttribI4i(fallbackVertexAttribI4i),
	VertexAttribI4iEXT(fallbackVertexAttribI4iEXT),
	VertexAttribI4iv(fallbackVertexAttribI4iv),
	VertexAttribI4ivEXT(fallbackVertexAttribI4ivEXT),
	VertexAttribI4sv(fallbackVertexAttribI4sv),
	VertexAttribI4svEXT(fallbackVertexAttribI4svEXT),
	VertexAttribI4ubv(fallbackVertexAttribI4ubv),
	VertexAttribI4ubvEXT(fallbackVertexAttribI4ubvEXT),
	VertexAttribI4ui(fallbackVertexAttribI4ui),
	VertexAttribI4uiEXT(fallbackVertexAttribI4uiEXT),
	VertexAttribI4uiv(fallbackVertexAttribI4uiv),
	VertexAttribI4uivEXT(fallbackVertexAttribI4uivEXT),
	VertexAttribI4usv(fallbackVertexAttribI4usv),
	VertexAttribI4usvEXT(fallbackVertexAttribI4usvEXT),
	VertexAttribIFormatNV(fallbackVertexAttribIFormatNV),
	VertexAttribIPointer(fallbackVertexAttribIPointer),
	VertexAttribIPointerEXT(fallbackVertexAttribIPointerEXT),
	VertexAttribP1ui(fallbackVertexAttribP1ui),
	VertexAttribP1uiv(fallbackVertexAttribP1uiv),
	VertexAttribP2ui(fallbackVertexAttribP2ui),
	VertexAttribP2uiv(fallbackVertexAttribP2uiv),
	VertexAttribP3ui(fallbackVertexAttribP3ui),
	VertexAttribP3uiv(fallbackVertexAttribP3uiv),
	VertexAttribP4ui(fallbackVertexAttribP4ui),
	VertexAttribP4uiv(fallbackVertexAttribP4uiv),
	VertexAttribPointer(fallbackVertexAttribPointer),
	VertexAttribPointerARB(fallbackVertexAttribPointerARB),
	VertexAttribPointerNV(fallbackVertexAttribPointerNV),
	VertexAttribs1dvNV(fallbackVertexAttribs1dvNV),
	VertexAttribs1fvNV(fallbackVertexAttribs1fvNV),
	VertexAttribs1hvNV(fallbackVertexAttribs1hvNV),
	VertexAttribs1svNV(fallbackVertexAttribs1svNV),
	VertexAttribs2dvNV(fallbackVertexAttribs2dvNV),
	VertexAttribs2fvNV(fallbackVertexAttribs2fvNV),
	VertexAttribs2hvNV(fallbackVertexAttribs2hvNV),
	VertexAttribs2svNV(fallbackVertexAttribs2svNV),
	VertexAttribs3dvNV(fallbackVertexAttribs3dvNV),
	VertexAttribs3fvNV(fallbackVertexAttribs3fvNV),
	VertexAttribs3hvNV(fallbackVertexAttribs3hvNV),
	VertexAttribs3svNV(fallbackVertexAttribs3svNV),
	VertexAttribs4dvNV(fallbackVertexAttribs4dvNV),
	VertexAttribs4fvNV(fallbackVertexAttribs4fvNV),
	VertexAttribs4hvNV(fallbackVertexAttribs4hvNV),
	VertexAttribs4svNV(fallbackVertexAttribs4svNV),
	VertexAttribs4ubvNV(fallbackVertexAttribs4ubvNV),
	VertexBlendARB(fallbackVertexBlendARB),
	VertexBlendEnvfATI(fallbackVertexBlendEnvfATI),
	VertexBlendEnviATI(fallbackVertexBlendEnviATI),
	VertexFormatNV(fallbackVertexFormatNV),
	VertexP2ui(fallbackVertexP2ui),
	VertexP2uiv(fallbackVertexP2uiv),
	VertexP3ui(fallbackVertexP3ui),
	VertexP3uiv(fallbackVertexP3uiv),
	VertexP4ui(fallbackVertexP4ui),
	VertexP4uiv(fallbackVertexP4uiv),
	VertexPointer(fallbackVertexPointer),
	VertexPointerEXT(fallbackVertexPointerEXT),
	VertexPointerListIBM(fallbackVertexPointerListIBM),
	VertexPointervINTEL(fallbackVertexPointervINTEL),
	VertexStream1dATI(fallbackVertexStream1dATI),
	VertexStream1dvATI(fallbackVertexStream1dvATI),
	VertexStream1fATI(fallbackVertexStream1fATI),
	VertexStream1fvATI(fallbackVertexStream1fvATI),
	VertexStream1iATI(fallbackVertexStream1iATI),
	VertexStream1ivATI(fallbackVertexStream1ivATI),
	VertexStream1sATI(fallbackVertexStream1sATI),
	VertexStream1svATI(fallbackVertexStream1svATI),
	VertexStream2dATI(fallbackVertexStream2dATI),
	VertexStream2dvATI(fallbackVertexStream2dvATI),
	VertexStream2fATI(fallbackVertexStream2fATI),
	VertexStream2fvATI(fallbackVertexStream2fvATI),
	VertexStream2iATI(fallbackVertexStream2iATI),
	VertexStream2ivATI(fallbackVertexStream2ivATI),
	VertexStream2sATI(fallbackVertexStream2sATI),
	VertexStream2svATI(fallbackVertexStream2svATI),
	VertexStream3dATI(fallbackVertexStream3dATI),
	VertexStream3dvATI(fallbackVertexStream3dvATI),
	VertexStream3fATI(fallbackVertexStream3fATI),
	VertexStream3fvATI(fallbackVertexStream3fvATI),
	VertexStream3iATI(fallbackVertexStream3iATI),
	VertexStream3ivATI(fallbackVertexStream3ivATI),
	VertexStream3sATI(fallbackVertexStream3sATI),
	VertexStream3svATI(fallbackVertexStream3svATI),
	VertexStream4dATI(fallbackVertexStream4dATI),
	VertexStream4dvATI(fallbackVertexStream4dvATI),
	VertexStream4fATI(fallbackVertexStream4fATI),
	VertexStream4fvATI(fallbackVertexStream4fvATI),
	VertexStream4iATI(fallbackVertexStream4iATI),
	VertexStream4ivATI(fallbackVertexStream4ivATI),
	VertexStream4sATI(fallbackVertexStream4sATI),
	VertexStream4svATI(fallbackVertexStream4svATI),
	VertexWeightPointerEXT(fallbackVertexWeightPointerEXT),
	VertexWeightfEXT(fallbackVertexWeightfEXT),
	VertexWeightfvEXT(fallbackVertexWeightfvEXT),
	VertexWeighthNV(fallbackVertexWeighthNV),
	VertexWeighthvNV(fallbackVertexWeighthvNV),
	VideoCaptureNV(fallbackVideoCaptureNV),
	VideoCaptureStreamParameterdvNV(fallbackVideoCaptureStreamParameterdvNV),
	VideoCaptureStreamParameterfvNV(fallbackVideoCaptureStreamParameterfvNV),
	VideoCaptureStreamParameterivNV(fallbackVideoCaptureStreamParameterivNV),
	Viewport(fallbackViewport),
	WaitSync(fallbackWaitSync),
	WeightPointerARB(fallbackWeightPointerARB),
	WeightbvARB(fallbackWeightbvARB),
	WeightdvARB(fallbackWeightdvARB),
	WeightfvARB(fallbackWeightfvARB),
	WeightivARB(fallbackWeightivARB),
	WeightsvARB(fallbackWeightsvARB),
	WeightubvARB(fallbackWeightubvARB),
	WeightuivARB(fallbackWeightuivARB),
	WeightusvARB(fallbackWeightusvARB),
	WindowPos2d(fallbackWindowPos2d),
	WindowPos2dARB(fallbackWindowPos2dARB),
	WindowPos2dMESA(fallbackWindowPos2dMESA),
	WindowPos2dv(fallbackWindowPos2dv),
	WindowPos2dvARB(fallbackWindowPos2dvARB),
	WindowPos2dvMESA(fallbackWindowPos2dvMESA),
	WindowPos2f(fallbackWindowPos2f),
	WindowPos2fARB(fallbackWindowPos2fARB),
	WindowPos2fMESA(fallbackWindowPos2fMESA),
	WindowPos2fv(fallbackWindowPos2fv),
	WindowPos2fvARB(fallbackWindowPos2fvARB),
	WindowPos2fvMESA(fallbackWindowPos2fvMESA),
	WindowPos2i(fallbackWindowPos2i),
	WindowPos2iARB(fallbackWindowPos2iARB),
	WindowPos2iMESA(fallbackWindowPos2iMESA),
	WindowPos2iv(fallbackWindowPos2iv),
	WindowPos2ivARB(fallbackWindowPos2ivARB),
	WindowPos2ivMESA(fallbackWindowPos2ivMESA),
	WindowPos2s(fallbackWindowPos2s),
	WindowPos2sARB(fallbackWindowPos2sARB),
	WindowPos2sMESA(fallbackWindowPos2sMESA),
	WindowPos2sv(fallbackWindowPos2sv),
	WindowPos2svARB(fallbackWindowPos2svARB),
	WindowPos2svMESA(fallbackWindowPos2svMESA),
	WindowPos3d(fallbackWindowPos3d),
	WindowPos3dARB(fallbackWindowPos3dARB),
	WindowPos3dMESA(fallbackWindowPos3dMESA),
	WindowPos3dv(fallbackWindowPos3dv),
	WindowPos3dvARB(fallbackWindowPos3dvARB),
	WindowPos3dvMESA(fallbackWindowPos3dvMESA),
	WindowPos3f(fallbackWindowPos3f),
	WindowPos3fARB(fallbackWindowPos3fARB),
	WindowPos3fMESA(fallbackWindowPos3fMESA),
	WindowPos3fv(fallbackWindowPos3fv),
	WindowPos3fvARB(fallbackWindowPos3fvARB),
	WindowPos3fvMESA(fallbackWindowPos3fvMESA),
	WindowPos3i(fallbackWindowPos3i),
	WindowPos3iARB(fallbackWindowPos3iARB),
	WindowPos3iMESA(fallbackWindowPos3iMESA),
	WindowPos3iv(fallbackWindowPos3iv),
	WindowPos3ivARB(fallbackWindowPos3ivARB),
	WindowPos3ivMESA(fallbackWindowPos3ivMESA),
	WindowPos3s(fallbackWindowPos3s),
	WindowPos3sARB(fallbackWindowPos3sARB),
	WindowPos3sMESA(fallbackWindowPos3sMESA),
	WindowPos3sv(fallbackWindowPos3sv),
	WindowPos3svARB(fallbackWindowPos3svARB),
	WindowPos3svMESA(fallbackWindowPos3svMESA),
	WindowPos4dMESA(fallbackWindowPos4dMESA),
	WindowPos4dvMESA(fallbackWindowPos4dvMESA),
	WindowPos4fMESA(fallbackWindowPos4fMESA),
	WindowPos4fvMESA(fallbackWindowPos4fvMESA),
	WindowPos4iMESA(fallbackWindowPos4iMESA),
	WindowPos4ivMESA(fallbackWindowPos4ivMESA),
	WindowPos4sMESA(fallbackWindowPos4sMESA),
	WindowPos4svMESA(fallbackWindowPos4svMESA),
	WriteMaskEXT(fallbackWriteMaskEXT)
{
}

} // namespace gl

} // namespace k3d
