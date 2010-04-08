/////////////////////////////////////////////////////////////////////////////////
//
// Generated file, do not edit by hand!
//
/////////////////////////////////////////////////////////////////////////////////

#include <k3dsdk/log.h>
#include <k3dsdk/opengl/api.h>

namespace k3d
{

namespace gl
{

static void fallbackAccum(api::GLenum op, api::GLfloat value)
{
	log() << warning << "glAccum not available." << std::endl;
}

static void fallbackActiveProgramEXT(api::GLuint program)
{
	log() << warning << "glActiveProgramEXT not available." << std::endl;
}

static void fallbackActiveStencilFaceEXT(api::GLenum face)
{
	log() << warning << "glActiveStencilFaceEXT not available." << std::endl;
}

static void fallbackActiveTexture(api::GLenum texture)
{
	log() << warning << "glActiveTexture not available." << std::endl;
}

static void fallbackActiveTextureARB(api::GLenum texture)
{
	log() << warning << "glActiveTextureARB not available." << std::endl;
}

static void fallbackActiveVaryingNV(api::GLuint program, const api::GLchar* name)
{
	log() << warning << "glActiveVaryingNV not available." << std::endl;
}

static void fallbackAlphaFragmentOp1ATI(api::GLenum op, api::GLuint dst, api::GLuint dstMod, api::GLuint arg1, api::GLuint arg1Rep, api::GLuint arg1Mod)
{
	log() << warning << "glAlphaFragmentOp1ATI not available." << std::endl;
}

static void fallbackAlphaFragmentOp2ATI(api::GLenum op, api::GLuint dst, api::GLuint dstMod, api::GLuint arg1, api::GLuint arg1Rep, api::GLuint arg1Mod, api::GLuint arg2, api::GLuint arg2Rep, api::GLuint arg2Mod)
{
	log() << warning << "glAlphaFragmentOp2ATI not available." << std::endl;
}

static void fallbackAlphaFragmentOp3ATI(api::GLenum op, api::GLuint dst, api::GLuint dstMod, api::GLuint arg1, api::GLuint arg1Rep, api::GLuint arg1Mod, api::GLuint arg2, api::GLuint arg2Rep, api::GLuint arg2Mod, api::GLuint arg3, api::GLuint arg3Rep, api::GLuint arg3Mod)
{
	log() << warning << "glAlphaFragmentOp3ATI not available." << std::endl;
}

static void fallbackAlphaFunc(api::GLenum func, api::GLclampf ref)
{
	log() << warning << "glAlphaFunc not available." << std::endl;
}

static void fallbackApplyTextureEXT(api::GLenum mode)
{
	log() << warning << "glApplyTextureEXT not available." << std::endl;
}

static api::GLboolean fallbackAreProgramsResidentNV(api::GLsizei n, const api::GLuint* programs, api::GLboolean* residences)
{
	log() << warning << "glAreProgramsResidentNV not available." << std::endl;
}

static api::GLboolean fallbackAreTexturesResident(api::GLsizei n, const api::GLuint* textures, api::GLboolean* residences)
{
	log() << warning << "glAreTexturesResident not available." << std::endl;
}

static api::GLboolean fallbackAreTexturesResidentEXT(api::GLsizei n, const api::GLuint* textures, api::GLboolean* residences)
{
	log() << warning << "glAreTexturesResidentEXT not available." << std::endl;
}

static void fallbackArrayElement(api::GLint i)
{
	log() << warning << "glArrayElement not available." << std::endl;
}

static void fallbackArrayElementEXT(api::GLint i)
{
	log() << warning << "glArrayElementEXT not available." << std::endl;
}

static void fallbackArrayObjectATI(api::GLenum array, api::GLint size, api::GLenum type, api::GLsizei stride, api::GLuint buffer, api::GLuint offset)
{
	log() << warning << "glArrayObjectATI not available." << std::endl;
}

static void fallbackAsyncMarkerSGIX(api::GLuint marker)
{
	log() << warning << "glAsyncMarkerSGIX not available." << std::endl;
}

static void fallbackAttachObjectARB(api::GLhandleARB containerObj, api::GLhandleARB obj)
{
	log() << warning << "glAttachObjectARB not available." << std::endl;
}

static void fallbackAttachShader(api::GLuint program, api::GLuint shader)
{
	log() << warning << "glAttachShader not available." << std::endl;
}

static void fallbackBegin(api::GLenum mode)
{
	log() << warning << "glBegin not available." << std::endl;
}

static void fallbackBeginConditionalRender(api::GLuint id, api::GLenum mode)
{
	log() << warning << "glBeginConditionalRender not available." << std::endl;
}

static void fallbackBeginConditionalRenderNV(api::GLuint id, api::GLenum mode)
{
	log() << warning << "glBeginConditionalRenderNV not available." << std::endl;
}

static void fallbackBeginFragmentShaderATI()
{
	log() << warning << "glBeginFragmentShaderATI not available." << std::endl;
}

static void fallbackBeginOcclusionQueryNV(api::GLuint id)
{
	log() << warning << "glBeginOcclusionQueryNV not available." << std::endl;
}

static void fallbackBeginPerfMonitorAMD(api::GLuint monitor)
{
	log() << warning << "glBeginPerfMonitorAMD not available." << std::endl;
}

static void fallbackBeginQuery(api::GLenum target, api::GLuint id)
{
	log() << warning << "glBeginQuery not available." << std::endl;
}

static void fallbackBeginQueryARB(api::GLenum target, api::GLuint id)
{
	log() << warning << "glBeginQueryARB not available." << std::endl;
}

static void fallbackBeginQueryIndexed(api::GLenum target, api::GLuint index, api::GLuint id)
{
	log() << warning << "glBeginQueryIndexed not available." << std::endl;
}

static void fallbackBeginTransformFeedback(api::GLenum primitiveMode)
{
	log() << warning << "glBeginTransformFeedback not available." << std::endl;
}

static void fallbackBeginTransformFeedbackEXT(api::GLenum primitiveMode)
{
	log() << warning << "glBeginTransformFeedbackEXT not available." << std::endl;
}

static void fallbackBeginTransformFeedbackNV(api::GLenum primitiveMode)
{
	log() << warning << "glBeginTransformFeedbackNV not available." << std::endl;
}

static void fallbackBeginVertexShaderEXT()
{
	log() << warning << "glBeginVertexShaderEXT not available." << std::endl;
}

static void fallbackBeginVideoCaptureNV(api::GLuint video_capture_slot)
{
	log() << warning << "glBeginVideoCaptureNV not available." << std::endl;
}

static void fallbackBindAttribLocation(api::GLuint program, api::GLuint index, const api::GLchar* name)
{
	log() << warning << "glBindAttribLocation not available." << std::endl;
}

static void fallbackBindAttribLocationARB(api::GLhandleARB programObj, api::GLuint index, const api::GLcharARB* name)
{
	log() << warning << "glBindAttribLocationARB not available." << std::endl;
}

static void fallbackBindBuffer(api::GLenum target, api::GLuint buffer)
{
	log() << warning << "glBindBuffer not available." << std::endl;
}

static void fallbackBindBufferARB(api::GLenum target, api::GLuint buffer)
{
	log() << warning << "glBindBufferARB not available." << std::endl;
}

static void fallbackBindBufferBase(api::GLenum target, api::GLuint index, api::GLuint buffer)
{
	log() << warning << "glBindBufferBase not available." << std::endl;
}

static void fallbackBindBufferBaseEXT(api::GLenum target, api::GLuint index, api::GLuint buffer)
{
	log() << warning << "glBindBufferBaseEXT not available." << std::endl;
}

static void fallbackBindBufferBaseNV(api::GLenum target, api::GLuint index, api::GLuint buffer)
{
	log() << warning << "glBindBufferBaseNV not available." << std::endl;
}

static void fallbackBindBufferOffsetEXT(api::GLenum target, api::GLuint index, api::GLuint buffer, api::GLintptr offset)
{
	log() << warning << "glBindBufferOffsetEXT not available." << std::endl;
}

static void fallbackBindBufferOffsetNV(api::GLenum target, api::GLuint index, api::GLuint buffer, api::GLintptr offset)
{
	log() << warning << "glBindBufferOffsetNV not available." << std::endl;
}

static void fallbackBindBufferRange(api::GLenum target, api::GLuint index, api::GLuint buffer, api::GLintptr offset, api::GLsizeiptr size)
{
	log() << warning << "glBindBufferRange not available." << std::endl;
}

static void fallbackBindBufferRangeEXT(api::GLenum target, api::GLuint index, api::GLuint buffer, api::GLintptr offset, api::GLsizeiptr size)
{
	log() << warning << "glBindBufferRangeEXT not available." << std::endl;
}

static void fallbackBindBufferRangeNV(api::GLenum target, api::GLuint index, api::GLuint buffer, api::GLintptr offset, api::GLsizeiptr size)
{
	log() << warning << "glBindBufferRangeNV not available." << std::endl;
}

static void fallbackBindFragDataLocation(api::GLuint program, api::GLuint color, const api::GLchar* name)
{
	log() << warning << "glBindFragDataLocation not available." << std::endl;
}

static void fallbackBindFragDataLocationEXT(api::GLuint program, api::GLuint color, const api::GLchar* name)
{
	log() << warning << "glBindFragDataLocationEXT not available." << std::endl;
}

static void fallbackBindFragDataLocationIndexed(api::GLuint program, api::GLuint colorNumber, api::GLuint index, const api::GLchar* name)
{
	log() << warning << "glBindFragDataLocationIndexed not available." << std::endl;
}

static void fallbackBindFragmentShaderATI(api::GLuint id)
{
	log() << warning << "glBindFragmentShaderATI not available." << std::endl;
}

static void fallbackBindFramebuffer(api::GLenum target, api::GLuint framebuffer)
{
	log() << warning << "glBindFramebuffer not available." << std::endl;
}

static void fallbackBindFramebufferEXT(api::GLenum target, api::GLuint framebuffer)
{
	log() << warning << "glBindFramebufferEXT not available." << std::endl;
}

static api::GLuint fallbackBindLightParameterEXT(api::GLenum light, api::GLenum value)
{
	log() << warning << "glBindLightParameterEXT not available." << std::endl;
}

static api::GLuint fallbackBindMaterialParameterEXT(api::GLenum face, api::GLenum value)
{
	log() << warning << "glBindMaterialParameterEXT not available." << std::endl;
}

static void fallbackBindMultiTextureEXT(api::GLenum texunit, api::GLenum target, api::GLuint texture)
{
	log() << warning << "glBindMultiTextureEXT not available." << std::endl;
}

static api::GLuint fallbackBindParameterEXT(api::GLenum value)
{
	log() << warning << "glBindParameterEXT not available." << std::endl;
}

static void fallbackBindProgramARB(api::GLenum target, api::GLuint program)
{
	log() << warning << "glBindProgramARB not available." << std::endl;
}

static void fallbackBindProgramNV(api::GLenum target, api::GLuint id)
{
	log() << warning << "glBindProgramNV not available." << std::endl;
}

static void fallbackBindRenderbuffer(api::GLenum target, api::GLuint renderbuffer)
{
	log() << warning << "glBindRenderbuffer not available." << std::endl;
}

static void fallbackBindRenderbufferEXT(api::GLenum target, api::GLuint renderbuffer)
{
	log() << warning << "glBindRenderbufferEXT not available." << std::endl;
}

static void fallbackBindSampler(api::GLenum unit, api::GLuint sampler)
{
	log() << warning << "glBindSampler not available." << std::endl;
}

static api::GLuint fallbackBindTexGenParameterEXT(api::GLenum unit, api::GLenum coord, api::GLenum value)
{
	log() << warning << "glBindTexGenParameterEXT not available." << std::endl;
}

static void fallbackBindTexture(api::GLenum target, api::GLuint texture)
{
	log() << warning << "glBindTexture not available." << std::endl;
}

static void fallbackBindTextureEXT(api::GLenum target, api::GLuint texture)
{
	log() << warning << "glBindTextureEXT not available." << std::endl;
}

static api::GLuint fallbackBindTextureUnitParameterEXT(api::GLenum unit, api::GLenum value)
{
	log() << warning << "glBindTextureUnitParameterEXT not available." << std::endl;
}

static void fallbackBindTransformFeedback(api::GLenum target, api::GLuint id)
{
	log() << warning << "glBindTransformFeedback not available." << std::endl;
}

static void fallbackBindTransformFeedbackNV(api::GLenum target, api::GLuint id)
{
	log() << warning << "glBindTransformFeedbackNV not available." << std::endl;
}

static void fallbackBindVertexArray(api::GLuint array)
{
	log() << warning << "glBindVertexArray not available." << std::endl;
}

static void fallbackBindVertexArrayAPPLE(api::GLuint array)
{
	log() << warning << "glBindVertexArrayAPPLE not available." << std::endl;
}

static void fallbackBindVertexShaderEXT(api::GLuint id)
{
	log() << warning << "glBindVertexShaderEXT not available." << std::endl;
}

static void fallbackBindVideoCaptureStreamBufferNV(api::GLuint video_capture_slot, api::GLuint stream, api::GLenum frame_region, api::GLintptrARB offset)
{
	log() << warning << "glBindVideoCaptureStreamBufferNV not available." << std::endl;
}

static void fallbackBindVideoCaptureStreamTextureNV(api::GLuint video_capture_slot, api::GLuint stream, api::GLenum frame_region, api::GLenum target, api::GLuint texture)
{
	log() << warning << "glBindVideoCaptureStreamTextureNV not available." << std::endl;
}

static void fallbackBinormal3bEXT(api::GLbyte bx, api::GLbyte by, api::GLbyte bz)
{
	log() << warning << "glBinormal3bEXT not available." << std::endl;
}

static void fallbackBinormal3bvEXT(const api::GLbyte* v)
{
	log() << warning << "glBinormal3bvEXT not available." << std::endl;
}

static void fallbackBinormal3dEXT(api::GLdouble bx, api::GLdouble by, api::GLdouble bz)
{
	log() << warning << "glBinormal3dEXT not available." << std::endl;
}

static void fallbackBinormal3dvEXT(const api::GLdouble* v)
{
	log() << warning << "glBinormal3dvEXT not available." << std::endl;
}

static void fallbackBinormal3fEXT(api::GLfloat bx, api::GLfloat by, api::GLfloat bz)
{
	log() << warning << "glBinormal3fEXT not available." << std::endl;
}

static void fallbackBinormal3fvEXT(const api::GLfloat* v)
{
	log() << warning << "glBinormal3fvEXT not available." << std::endl;
}

static void fallbackBinormal3iEXT(api::GLint bx, api::GLint by, api::GLint bz)
{
	log() << warning << "glBinormal3iEXT not available." << std::endl;
}

static void fallbackBinormal3ivEXT(const api::GLint* v)
{
	log() << warning << "glBinormal3ivEXT not available." << std::endl;
}

static void fallbackBinormal3sEXT(api::GLshort bx, api::GLshort by, api::GLshort bz)
{
	log() << warning << "glBinormal3sEXT not available." << std::endl;
}

static void fallbackBinormal3svEXT(const api::GLshort* v)
{
	log() << warning << "glBinormal3svEXT not available." << std::endl;
}

static void fallbackBinormalPointerEXT(api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "glBinormalPointerEXT not available." << std::endl;
}

static void fallbackBitmap(api::GLsizei width, api::GLsizei height, api::GLfloat xorig, api::GLfloat yorig, api::GLfloat xmove, api::GLfloat ymove, const api::GLubyte* bitmap)
{
	log() << warning << "glBitmap not available." << std::endl;
}

static void fallbackBlendColor(api::GLclampf red, api::GLclampf green, api::GLclampf blue, api::GLclampf alpha)
{
	log() << warning << "glBlendColor not available." << std::endl;
}

static void fallbackBlendColorEXT(api::GLclampf red, api::GLclampf green, api::GLclampf blue, api::GLclampf alpha)
{
	log() << warning << "glBlendColorEXT not available." << std::endl;
}

static void fallbackBlendEquation(api::GLenum mode)
{
	log() << warning << "glBlendEquation not available." << std::endl;
}

static void fallbackBlendEquationEXT(api::GLenum mode)
{
	log() << warning << "glBlendEquationEXT not available." << std::endl;
}

static void fallbackBlendEquationIndexedAMD(api::GLuint buf, api::GLenum mode)
{
	log() << warning << "glBlendEquationIndexedAMD not available." << std::endl;
}

static void fallbackBlendEquationSeparate(api::GLenum modeRGB, api::GLenum modeAlpha)
{
	log() << warning << "glBlendEquationSeparate not available." << std::endl;
}

static void fallbackBlendEquationSeparateEXT(api::GLenum modeRGB, api::GLenum modeAlpha)
{
	log() << warning << "glBlendEquationSeparateEXT not available." << std::endl;
}

static void fallbackBlendEquationSeparateIndexedAMD(api::GLuint buf, api::GLenum modeRGB, api::GLenum modeAlpha)
{
	log() << warning << "glBlendEquationSeparateIndexedAMD not available." << std::endl;
}

static void fallbackBlendEquationSeparatei(api::GLuint buf, api::GLenum modeRGB, api::GLenum modeAlpha)
{
	log() << warning << "glBlendEquationSeparatei not available." << std::endl;
}

static void fallbackBlendEquationi(api::GLuint buf, api::GLenum mode)
{
	log() << warning << "glBlendEquationi not available." << std::endl;
}

static void fallbackBlendFunc(api::GLenum sfactor, api::GLenum dfactor)
{
	log() << warning << "glBlendFunc not available." << std::endl;
}

static void fallbackBlendFuncIndexedAMD(api::GLuint buf, api::GLenum src, api::GLenum dst)
{
	log() << warning << "glBlendFuncIndexedAMD not available." << std::endl;
}

static void fallbackBlendFuncSeparate(api::GLenum sfactorRGB, api::GLenum dfactorRGB, api::GLenum sfactorAlpha, api::GLenum dfactorAlpha)
{
	log() << warning << "glBlendFuncSeparate not available." << std::endl;
}

static void fallbackBlendFuncSeparateEXT(api::GLenum sfactorRGB, api::GLenum dfactorRGB, api::GLenum sfactorAlpha, api::GLenum dfactorAlpha)
{
	log() << warning << "glBlendFuncSeparateEXT not available." << std::endl;
}

static void fallbackBlendFuncSeparateINGR(api::GLenum sfactorRGB, api::GLenum dfactorRGB, api::GLenum sfactorAlpha, api::GLenum dfactorAlpha)
{
	log() << warning << "glBlendFuncSeparateINGR not available." << std::endl;
}

static void fallbackBlendFuncSeparateIndexedAMD(api::GLuint buf, api::GLenum srcRGB, api::GLenum dstRGB, api::GLenum srcAlpha, api::GLenum dstAlpha)
{
	log() << warning << "glBlendFuncSeparateIndexedAMD not available." << std::endl;
}

static void fallbackBlendFuncSeparatei(api::GLuint buf, api::GLenum srcRGB, api::GLenum dstRGB, api::GLenum srcAlpha, api::GLenum dstAlpha)
{
	log() << warning << "glBlendFuncSeparatei not available." << std::endl;
}

static void fallbackBlendFunci(api::GLuint buf, api::GLenum src, api::GLenum dst)
{
	log() << warning << "glBlendFunci not available." << std::endl;
}

static void fallbackBlitFramebuffer(api::GLint srcX0, api::GLint srcY0, api::GLint srcX1, api::GLint srcY1, api::GLint dstX0, api::GLint dstY0, api::GLint dstX1, api::GLint dstY1, api::GLbitfield mask, api::GLenum filter)
{
	log() << warning << "glBlitFramebuffer not available." << std::endl;
}

static void fallbackBlitFramebufferEXT(api::GLint srcX0, api::GLint srcY0, api::GLint srcX1, api::GLint srcY1, api::GLint dstX0, api::GLint dstY0, api::GLint dstX1, api::GLint dstY1, api::GLbitfield mask, api::GLenum filter)
{
	log() << warning << "glBlitFramebufferEXT not available." << std::endl;
}

static void fallbackBufferAddressRangeNV(api::GLenum pname, api::GLuint index, api::GLuint64EXT address, api::GLsizeiptr length)
{
	log() << warning << "glBufferAddressRangeNV not available." << std::endl;
}

static void fallbackBufferData(api::GLenum target, api::GLsizeiptr size, const api::GLvoid* data, api::GLenum usage)
{
	log() << warning << "glBufferData not available." << std::endl;
}

static void fallbackBufferDataARB(api::GLenum target, api::GLsizeiptrARB size, const api::GLvoid* data, api::GLenum usage)
{
	log() << warning << "glBufferDataARB not available." << std::endl;
}

static void fallbackBufferParameteriAPPLE(api::GLenum target, api::GLenum pname, api::GLint param)
{
	log() << warning << "glBufferParameteriAPPLE not available." << std::endl;
}

static void fallbackBufferSubData(api::GLenum target, api::GLintptr offset, api::GLsizeiptr size, const api::GLvoid* data)
{
	log() << warning << "glBufferSubData not available." << std::endl;
}

static void fallbackBufferSubDataARB(api::GLenum target, api::GLintptrARB offset, api::GLsizeiptrARB size, const api::GLvoid* data)
{
	log() << warning << "glBufferSubDataARB not available." << std::endl;
}

static void fallbackCallList(api::GLuint list)
{
	log() << warning << "glCallList not available." << std::endl;
}

static void fallbackCallLists(api::GLsizei n, api::GLenum type, const api::GLvoid* lists)
{
	log() << warning << "glCallLists not available." << std::endl;
}

static api::GLenum fallbackCheckFramebufferStatus(api::GLenum target)
{
	log() << warning << "glCheckFramebufferStatus not available." << std::endl;
}

static api::GLenum fallbackCheckFramebufferStatusEXT(api::GLenum target)
{
	log() << warning << "glCheckFramebufferStatusEXT not available." << std::endl;
}

static api::GLenum fallbackCheckNamedFramebufferStatusEXT(api::GLuint framebuffer, api::GLenum target)
{
	log() << warning << "glCheckNamedFramebufferStatusEXT not available." << std::endl;
}

static void fallbackClampColor(api::GLenum target, api::GLenum clamp)
{
	log() << warning << "glClampColor not available." << std::endl;
}

static void fallbackClampColorARB(api::GLenum target, api::GLenum clamp)
{
	log() << warning << "glClampColorARB not available." << std::endl;
}

static void fallbackClear(api::GLbitfield mask)
{
	log() << warning << "glClear not available." << std::endl;
}

static void fallbackClearAccum(api::GLfloat red, api::GLfloat green, api::GLfloat blue, api::GLfloat alpha)
{
	log() << warning << "glClearAccum not available." << std::endl;
}

static void fallbackClearBufferfi(api::GLenum buffer, api::GLint drawbuffer, api::GLfloat depth, api::GLint stencil)
{
	log() << warning << "glClearBufferfi not available." << std::endl;
}

static void fallbackClearBufferfv(api::GLenum buffer, api::GLint drawbuffer, const api::GLfloat* value)
{
	log() << warning << "glClearBufferfv not available." << std::endl;
}

static void fallbackClearBufferiv(api::GLenum buffer, api::GLint drawbuffer, const api::GLint* value)
{
	log() << warning << "glClearBufferiv not available." << std::endl;
}

static void fallbackClearBufferuiv(api::GLenum buffer, api::GLint drawbuffer, const api::GLuint* value)
{
	log() << warning << "glClearBufferuiv not available." << std::endl;
}

static void fallbackClearColor(api::GLclampf red, api::GLclampf green, api::GLclampf blue, api::GLclampf alpha)
{
	log() << warning << "glClearColor not available." << std::endl;
}

static void fallbackClearColorIiEXT(api::GLint red, api::GLint green, api::GLint blue, api::GLint alpha)
{
	log() << warning << "glClearColorIiEXT not available." << std::endl;
}

static void fallbackClearColorIuiEXT(api::GLuint red, api::GLuint green, api::GLuint blue, api::GLuint alpha)
{
	log() << warning << "glClearColorIuiEXT not available." << std::endl;
}

static void fallbackClearDepth(api::GLclampd depth)
{
	log() << warning << "glClearDepth not available." << std::endl;
}

static void fallbackClearDepthdNV(api::GLdouble depth)
{
	log() << warning << "glClearDepthdNV not available." << std::endl;
}

static void fallbackClearIndex(api::GLfloat c)
{
	log() << warning << "glClearIndex not available." << std::endl;
}

static void fallbackClearStencil(api::GLint s)
{
	log() << warning << "glClearStencil not available." << std::endl;
}

static void fallbackClientActiveTexture(api::GLenum texture)
{
	log() << warning << "glClientActiveTexture not available." << std::endl;
}

static void fallbackClientActiveTextureARB(api::GLenum texture)
{
	log() << warning << "glClientActiveTextureARB not available." << std::endl;
}

static void fallbackClientActiveVertexStreamATI(api::GLenum stream)
{
	log() << warning << "glClientActiveVertexStreamATI not available." << std::endl;
}

static void fallbackClientAttribDefaultEXT(api::GLbitfield mask)
{
	log() << warning << "glClientAttribDefaultEXT not available." << std::endl;
}

static api::GLenum fallbackClientWaitSync(api::GLsync sync, api::GLbitfield flags, api::GLuint64 timeout)
{
	log() << warning << "glClientWaitSync not available." << std::endl;
}

static void fallbackClipPlane(api::GLenum plane, const api::GLdouble* equation)
{
	log() << warning << "glClipPlane not available." << std::endl;
}

static void fallbackColor3b(api::GLbyte red, api::GLbyte green, api::GLbyte blue)
{
	log() << warning << "glColor3b not available." << std::endl;
}

static void fallbackColor3bv(const api::GLbyte* v)
{
	log() << warning << "glColor3bv not available." << std::endl;
}

static void fallbackColor3d(api::GLdouble red, api::GLdouble green, api::GLdouble blue)
{
	log() << warning << "glColor3d not available." << std::endl;
}

static void fallbackColor3dv(const api::GLdouble* v)
{
	log() << warning << "glColor3dv not available." << std::endl;
}

static void fallbackColor3f(api::GLfloat red, api::GLfloat green, api::GLfloat blue)
{
	log() << warning << "glColor3f not available." << std::endl;
}

static void fallbackColor3fVertex3fSUN(api::GLfloat r, api::GLfloat g, api::GLfloat b, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "glColor3fVertex3fSUN not available." << std::endl;
}

static void fallbackColor3fVertex3fvSUN(const api::GLfloat* c, const api::GLfloat* v)
{
	log() << warning << "glColor3fVertex3fvSUN not available." << std::endl;
}

static void fallbackColor3fv(const api::GLfloat* v)
{
	log() << warning << "glColor3fv not available." << std::endl;
}

static void fallbackColor3hNV(api::GLhalfNV red, api::GLhalfNV green, api::GLhalfNV blue)
{
	log() << warning << "glColor3hNV not available." << std::endl;
}

static void fallbackColor3hvNV(const api::GLhalfNV* v)
{
	log() << warning << "glColor3hvNV not available." << std::endl;
}

static void fallbackColor3i(api::GLint red, api::GLint green, api::GLint blue)
{
	log() << warning << "glColor3i not available." << std::endl;
}

static void fallbackColor3iv(const api::GLint* v)
{
	log() << warning << "glColor3iv not available." << std::endl;
}

static void fallbackColor3s(api::GLshort red, api::GLshort green, api::GLshort blue)
{
	log() << warning << "glColor3s not available." << std::endl;
}

static void fallbackColor3sv(const api::GLshort* v)
{
	log() << warning << "glColor3sv not available." << std::endl;
}

static void fallbackColor3ub(api::GLubyte red, api::GLubyte green, api::GLubyte blue)
{
	log() << warning << "glColor3ub not available." << std::endl;
}

static void fallbackColor3ubv(const api::GLubyte* v)
{
	log() << warning << "glColor3ubv not available." << std::endl;
}

static void fallbackColor3ui(api::GLuint red, api::GLuint green, api::GLuint blue)
{
	log() << warning << "glColor3ui not available." << std::endl;
}

static void fallbackColor3uiv(const api::GLuint* v)
{
	log() << warning << "glColor3uiv not available." << std::endl;
}

static void fallbackColor3us(api::GLushort red, api::GLushort green, api::GLushort blue)
{
	log() << warning << "glColor3us not available." << std::endl;
}

static void fallbackColor3usv(const api::GLushort* v)
{
	log() << warning << "glColor3usv not available." << std::endl;
}

static void fallbackColor4b(api::GLbyte red, api::GLbyte green, api::GLbyte blue, api::GLbyte alpha)
{
	log() << warning << "glColor4b not available." << std::endl;
}

static void fallbackColor4bv(const api::GLbyte* v)
{
	log() << warning << "glColor4bv not available." << std::endl;
}

static void fallbackColor4d(api::GLdouble red, api::GLdouble green, api::GLdouble blue, api::GLdouble alpha)
{
	log() << warning << "glColor4d not available." << std::endl;
}

static void fallbackColor4dv(const api::GLdouble* v)
{
	log() << warning << "glColor4dv not available." << std::endl;
}

static void fallbackColor4f(api::GLfloat red, api::GLfloat green, api::GLfloat blue, api::GLfloat alpha)
{
	log() << warning << "glColor4f not available." << std::endl;
}

static void fallbackColor4fNormal3fVertex3fSUN(api::GLfloat r, api::GLfloat g, api::GLfloat b, api::GLfloat a, api::GLfloat nx, api::GLfloat ny, api::GLfloat nz, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "glColor4fNormal3fVertex3fSUN not available." << std::endl;
}

static void fallbackColor4fNormal3fVertex3fvSUN(const api::GLfloat* c, const api::GLfloat* n, const api::GLfloat* v)
{
	log() << warning << "glColor4fNormal3fVertex3fvSUN not available." << std::endl;
}

static void fallbackColor4fv(const api::GLfloat* v)
{
	log() << warning << "glColor4fv not available." << std::endl;
}

static void fallbackColor4hNV(api::GLhalfNV red, api::GLhalfNV green, api::GLhalfNV blue, api::GLhalfNV alpha)
{
	log() << warning << "glColor4hNV not available." << std::endl;
}

static void fallbackColor4hvNV(const api::GLhalfNV* v)
{
	log() << warning << "glColor4hvNV not available." << std::endl;
}

static void fallbackColor4i(api::GLint red, api::GLint green, api::GLint blue, api::GLint alpha)
{
	log() << warning << "glColor4i not available." << std::endl;
}

static void fallbackColor4iv(const api::GLint* v)
{
	log() << warning << "glColor4iv not available." << std::endl;
}

static void fallbackColor4s(api::GLshort red, api::GLshort green, api::GLshort blue, api::GLshort alpha)
{
	log() << warning << "glColor4s not available." << std::endl;
}

static void fallbackColor4sv(const api::GLshort* v)
{
	log() << warning << "glColor4sv not available." << std::endl;
}

static void fallbackColor4ub(api::GLubyte red, api::GLubyte green, api::GLubyte blue, api::GLubyte alpha)
{
	log() << warning << "glColor4ub not available." << std::endl;
}

static void fallbackColor4ubVertex2fSUN(api::GLubyte r, api::GLubyte g, api::GLubyte b, api::GLubyte a, api::GLfloat x, api::GLfloat y)
{
	log() << warning << "glColor4ubVertex2fSUN not available." << std::endl;
}

static void fallbackColor4ubVertex2fvSUN(const api::GLubyte* c, const api::GLfloat* v)
{
	log() << warning << "glColor4ubVertex2fvSUN not available." << std::endl;
}

static void fallbackColor4ubVertex3fSUN(api::GLubyte r, api::GLubyte g, api::GLubyte b, api::GLubyte a, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "glColor4ubVertex3fSUN not available." << std::endl;
}

static void fallbackColor4ubVertex3fvSUN(const api::GLubyte* c, const api::GLfloat* v)
{
	log() << warning << "glColor4ubVertex3fvSUN not available." << std::endl;
}

static void fallbackColor4ubv(const api::GLubyte* v)
{
	log() << warning << "glColor4ubv not available." << std::endl;
}

static void fallbackColor4ui(api::GLuint red, api::GLuint green, api::GLuint blue, api::GLuint alpha)
{
	log() << warning << "glColor4ui not available." << std::endl;
}

static void fallbackColor4uiv(const api::GLuint* v)
{
	log() << warning << "glColor4uiv not available." << std::endl;
}

static void fallbackColor4us(api::GLushort red, api::GLushort green, api::GLushort blue, api::GLushort alpha)
{
	log() << warning << "glColor4us not available." << std::endl;
}

static void fallbackColor4usv(const api::GLushort* v)
{
	log() << warning << "glColor4usv not available." << std::endl;
}

static void fallbackColorFormatNV(api::GLint size, api::GLenum type, api::GLsizei stride)
{
	log() << warning << "glColorFormatNV not available." << std::endl;
}

static void fallbackColorFragmentOp1ATI(api::GLenum op, api::GLuint dst, api::GLuint dstMask, api::GLuint dstMod, api::GLuint arg1, api::GLuint arg1Rep, api::GLuint arg1Mod)
{
	log() << warning << "glColorFragmentOp1ATI not available." << std::endl;
}

static void fallbackColorFragmentOp2ATI(api::GLenum op, api::GLuint dst, api::GLuint dstMask, api::GLuint dstMod, api::GLuint arg1, api::GLuint arg1Rep, api::GLuint arg1Mod, api::GLuint arg2, api::GLuint arg2Rep, api::GLuint arg2Mod)
{
	log() << warning << "glColorFragmentOp2ATI not available." << std::endl;
}

static void fallbackColorFragmentOp3ATI(api::GLenum op, api::GLuint dst, api::GLuint dstMask, api::GLuint dstMod, api::GLuint arg1, api::GLuint arg1Rep, api::GLuint arg1Mod, api::GLuint arg2, api::GLuint arg2Rep, api::GLuint arg2Mod, api::GLuint arg3, api::GLuint arg3Rep, api::GLuint arg3Mod)
{
	log() << warning << "glColorFragmentOp3ATI not available." << std::endl;
}

static void fallbackColorMask(api::GLboolean red, api::GLboolean green, api::GLboolean blue, api::GLboolean alpha)
{
	log() << warning << "glColorMask not available." << std::endl;
}

static void fallbackColorMaskIndexedEXT(api::GLuint index, api::GLboolean r, api::GLboolean g, api::GLboolean b, api::GLboolean a)
{
	log() << warning << "glColorMaskIndexedEXT not available." << std::endl;
}

static void fallbackColorMaski(api::GLuint index, api::GLboolean r, api::GLboolean g, api::GLboolean b, api::GLboolean a)
{
	log() << warning << "glColorMaski not available." << std::endl;
}

static void fallbackColorMaterial(api::GLenum face, api::GLenum mode)
{
	log() << warning << "glColorMaterial not available." << std::endl;
}

static void fallbackColorP3ui(api::GLenum type, api::GLuint color)
{
	log() << warning << "glColorP3ui not available." << std::endl;
}

static void fallbackColorP3uiv(api::GLenum type, const api::GLuint* color)
{
	log() << warning << "glColorP3uiv not available." << std::endl;
}

static void fallbackColorP4ui(api::GLenum type, api::GLuint color)
{
	log() << warning << "glColorP4ui not available." << std::endl;
}

static void fallbackColorP4uiv(api::GLenum type, const api::GLuint* color)
{
	log() << warning << "glColorP4uiv not available." << std::endl;
}

static void fallbackColorPointer(api::GLint size, api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "glColorPointer not available." << std::endl;
}

static void fallbackColorPointerEXT(api::GLint size, api::GLenum type, api::GLsizei stride, api::GLsizei count, const api::GLvoid* pointer)
{
	log() << warning << "glColorPointerEXT not available." << std::endl;
}

static void fallbackColorPointerListIBM(api::GLint size, api::GLenum type, api::GLint stride, const api::GLvoid** pointer, api::GLint ptrstride)
{
	log() << warning << "glColorPointerListIBM not available." << std::endl;
}

static void fallbackColorPointervINTEL(api::GLint size, api::GLenum type, const api::GLvoid** pointer)
{
	log() << warning << "glColorPointervINTEL not available." << std::endl;
}

static void fallbackColorSubTable(api::GLenum target, api::GLsizei start, api::GLsizei count, api::GLenum format, api::GLenum type, const api::GLvoid* data)
{
	log() << warning << "glColorSubTable not available." << std::endl;
}

static void fallbackColorSubTableEXT(api::GLenum target, api::GLsizei start, api::GLsizei count, api::GLenum format, api::GLenum type, const api::GLvoid* data)
{
	log() << warning << "glColorSubTableEXT not available." << std::endl;
}

static void fallbackColorTable(api::GLenum target, api::GLenum internalformat, api::GLsizei width, api::GLenum format, api::GLenum type, const api::GLvoid* table)
{
	log() << warning << "glColorTable not available." << std::endl;
}

static void fallbackColorTableEXT(api::GLenum target, api::GLenum internalFormat, api::GLsizei width, api::GLenum format, api::GLenum type, const api::GLvoid* table)
{
	log() << warning << "glColorTableEXT not available." << std::endl;
}

static void fallbackColorTableParameterfv(api::GLenum target, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "glColorTableParameterfv not available." << std::endl;
}

static void fallbackColorTableParameterfvSGI(api::GLenum target, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "glColorTableParameterfvSGI not available." << std::endl;
}

static void fallbackColorTableParameteriv(api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "glColorTableParameteriv not available." << std::endl;
}

static void fallbackColorTableParameterivSGI(api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "glColorTableParameterivSGI not available." << std::endl;
}

static void fallbackColorTableSGI(api::GLenum target, api::GLenum internalformat, api::GLsizei width, api::GLenum format, api::GLenum type, const api::GLvoid* table)
{
	log() << warning << "glColorTableSGI not available." << std::endl;
}

static void fallbackCombinerInputNV(api::GLenum stage, api::GLenum portion, api::GLenum variable, api::GLenum input, api::GLenum mapping, api::GLenum componentUsage)
{
	log() << warning << "glCombinerInputNV not available." << std::endl;
}

static void fallbackCombinerOutputNV(api::GLenum stage, api::GLenum portion, api::GLenum abOutput, api::GLenum cdOutput, api::GLenum sumOutput, api::GLenum scale, api::GLenum bias, api::GLboolean abDotProduct, api::GLboolean cdDotProduct, api::GLboolean muxSum)
{
	log() << warning << "glCombinerOutputNV not available." << std::endl;
}

static void fallbackCombinerParameterfNV(api::GLenum pname, api::GLfloat param)
{
	log() << warning << "glCombinerParameterfNV not available." << std::endl;
}

static void fallbackCombinerParameterfvNV(api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "glCombinerParameterfvNV not available." << std::endl;
}

static void fallbackCombinerParameteriNV(api::GLenum pname, api::GLint param)
{
	log() << warning << "glCombinerParameteriNV not available." << std::endl;
}

static void fallbackCombinerParameterivNV(api::GLenum pname, const api::GLint* params)
{
	log() << warning << "glCombinerParameterivNV not available." << std::endl;
}

static void fallbackCombinerStageParameterfvNV(api::GLenum stage, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "glCombinerStageParameterfvNV not available." << std::endl;
}

static void fallbackCompileShader(api::GLuint shader)
{
	log() << warning << "glCompileShader not available." << std::endl;
}

static void fallbackCompileShaderARB(api::GLhandleARB shaderObj)
{
	log() << warning << "glCompileShaderARB not available." << std::endl;
}

static void fallbackCompileShaderIncludeARB(api::GLuint shader, api::GLsizei count, const api::GLchar** path, const api::GLint* length)
{
	log() << warning << "glCompileShaderIncludeARB not available." << std::endl;
}

static void fallbackCompressedMultiTexImage1DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLint border, api::GLsizei imageSize, const api::GLvoid* bits)
{
	log() << warning << "glCompressedMultiTexImage1DEXT not available." << std::endl;
}

static void fallbackCompressedMultiTexImage2DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLint border, api::GLsizei imageSize, const api::GLvoid* bits)
{
	log() << warning << "glCompressedMultiTexImage2DEXT not available." << std::endl;
}

static void fallbackCompressedMultiTexImage3DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLint border, api::GLsizei imageSize, const api::GLvoid* bits)
{
	log() << warning << "glCompressedMultiTexImage3DEXT not available." << std::endl;
}

static void fallbackCompressedMultiTexSubImage1DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLsizei width, api::GLenum format, api::GLsizei imageSize, const api::GLvoid* bits)
{
	log() << warning << "glCompressedMultiTexSubImage1DEXT not available." << std::endl;
}

static void fallbackCompressedMultiTexSubImage2DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLsizei width, api::GLsizei height, api::GLenum format, api::GLsizei imageSize, const api::GLvoid* bits)
{
	log() << warning << "glCompressedMultiTexSubImage2DEXT not available." << std::endl;
}

static void fallbackCompressedMultiTexSubImage3DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint zoffset, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLenum format, api::GLsizei imageSize, const api::GLvoid* bits)
{
	log() << warning << "glCompressedMultiTexSubImage3DEXT not available." << std::endl;
}

static void fallbackCompressedTexImage1D(api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLint border, api::GLsizei imageSize, const api::GLvoid* data)
{
	log() << warning << "glCompressedTexImage1D not available." << std::endl;
}

static void fallbackCompressedTexImage1DARB(api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLint border, api::GLsizei imageSize, const api::GLvoid* data)
{
	log() << warning << "glCompressedTexImage1DARB not available." << std::endl;
}

static void fallbackCompressedTexImage2D(api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLint border, api::GLsizei imageSize, const api::GLvoid* data)
{
	log() << warning << "glCompressedTexImage2D not available." << std::endl;
}

static void fallbackCompressedTexImage2DARB(api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLint border, api::GLsizei imageSize, const api::GLvoid* data)
{
	log() << warning << "glCompressedTexImage2DARB not available." << std::endl;
}

static void fallbackCompressedTexImage3D(api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLint border, api::GLsizei imageSize, const api::GLvoid* data)
{
	log() << warning << "glCompressedTexImage3D not available." << std::endl;
}

static void fallbackCompressedTexImage3DARB(api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLint border, api::GLsizei imageSize, const api::GLvoid* data)
{
	log() << warning << "glCompressedTexImage3DARB not available." << std::endl;
}

static void fallbackCompressedTexSubImage1D(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLsizei width, api::GLenum format, api::GLsizei imageSize, const api::GLvoid* data)
{
	log() << warning << "glCompressedTexSubImage1D not available." << std::endl;
}

static void fallbackCompressedTexSubImage1DARB(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLsizei width, api::GLenum format, api::GLsizei imageSize, const api::GLvoid* data)
{
	log() << warning << "glCompressedTexSubImage1DARB not available." << std::endl;
}

static void fallbackCompressedTexSubImage2D(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLsizei width, api::GLsizei height, api::GLenum format, api::GLsizei imageSize, const api::GLvoid* data)
{
	log() << warning << "glCompressedTexSubImage2D not available." << std::endl;
}

static void fallbackCompressedTexSubImage2DARB(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLsizei width, api::GLsizei height, api::GLenum format, api::GLsizei imageSize, const api::GLvoid* data)
{
	log() << warning << "glCompressedTexSubImage2DARB not available." << std::endl;
}

static void fallbackCompressedTexSubImage3D(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint zoffset, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLenum format, api::GLsizei imageSize, const api::GLvoid* data)
{
	log() << warning << "glCompressedTexSubImage3D not available." << std::endl;
}

static void fallbackCompressedTexSubImage3DARB(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint zoffset, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLenum format, api::GLsizei imageSize, const api::GLvoid* data)
{
	log() << warning << "glCompressedTexSubImage3DARB not available." << std::endl;
}

static void fallbackCompressedTextureImage1DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLint border, api::GLsizei imageSize, const api::GLvoid* bits)
{
	log() << warning << "glCompressedTextureImage1DEXT not available." << std::endl;
}

static void fallbackCompressedTextureImage2DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLint border, api::GLsizei imageSize, const api::GLvoid* bits)
{
	log() << warning << "glCompressedTextureImage2DEXT not available." << std::endl;
}

static void fallbackCompressedTextureImage3DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLint border, api::GLsizei imageSize, const api::GLvoid* bits)
{
	log() << warning << "glCompressedTextureImage3DEXT not available." << std::endl;
}

static void fallbackCompressedTextureSubImage1DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLsizei width, api::GLenum format, api::GLsizei imageSize, const api::GLvoid* bits)
{
	log() << warning << "glCompressedTextureSubImage1DEXT not available." << std::endl;
}

static void fallbackCompressedTextureSubImage2DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLsizei width, api::GLsizei height, api::GLenum format, api::GLsizei imageSize, const api::GLvoid* bits)
{
	log() << warning << "glCompressedTextureSubImage2DEXT not available." << std::endl;
}

static void fallbackCompressedTextureSubImage3DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint zoffset, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLenum format, api::GLsizei imageSize, const api::GLvoid* bits)
{
	log() << warning << "glCompressedTextureSubImage3DEXT not available." << std::endl;
}

static void fallbackConvolutionFilter1D(api::GLenum target, api::GLenum internalformat, api::GLsizei width, api::GLenum format, api::GLenum type, const api::GLvoid* image)
{
	log() << warning << "glConvolutionFilter1D not available." << std::endl;
}

static void fallbackConvolutionFilter1DEXT(api::GLenum target, api::GLenum internalformat, api::GLsizei width, api::GLenum format, api::GLenum type, const api::GLvoid* image)
{
	log() << warning << "glConvolutionFilter1DEXT not available." << std::endl;
}

static void fallbackConvolutionFilter2D(api::GLenum target, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLenum format, api::GLenum type, const api::GLvoid* image)
{
	log() << warning << "glConvolutionFilter2D not available." << std::endl;
}

static void fallbackConvolutionFilter2DEXT(api::GLenum target, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLenum format, api::GLenum type, const api::GLvoid* image)
{
	log() << warning << "glConvolutionFilter2DEXT not available." << std::endl;
}

static void fallbackConvolutionParameterf(api::GLenum target, api::GLenum pname, api::GLfloat params)
{
	log() << warning << "glConvolutionParameterf not available." << std::endl;
}

static void fallbackConvolutionParameterfEXT(api::GLenum target, api::GLenum pname, api::GLfloat params)
{
	log() << warning << "glConvolutionParameterfEXT not available." << std::endl;
}

static void fallbackConvolutionParameterfv(api::GLenum target, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "glConvolutionParameterfv not available." << std::endl;
}

static void fallbackConvolutionParameterfvEXT(api::GLenum target, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "glConvolutionParameterfvEXT not available." << std::endl;
}

static void fallbackConvolutionParameteri(api::GLenum target, api::GLenum pname, api::GLint params)
{
	log() << warning << "glConvolutionParameteri not available." << std::endl;
}

static void fallbackConvolutionParameteriEXT(api::GLenum target, api::GLenum pname, api::GLint params)
{
	log() << warning << "glConvolutionParameteriEXT not available." << std::endl;
}

static void fallbackConvolutionParameteriv(api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "glConvolutionParameteriv not available." << std::endl;
}

static void fallbackConvolutionParameterivEXT(api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "glConvolutionParameterivEXT not available." << std::endl;
}

static void fallbackCopyBufferSubData(api::GLenum readTarget, api::GLenum writeTarget, api::GLintptr readOffset, api::GLintptr writeOffset, api::GLsizeiptr size)
{
	log() << warning << "glCopyBufferSubData not available." << std::endl;
}

static void fallbackCopyColorSubTable(api::GLenum target, api::GLsizei start, api::GLint x, api::GLint y, api::GLsizei width)
{
	log() << warning << "glCopyColorSubTable not available." << std::endl;
}

static void fallbackCopyColorSubTableEXT(api::GLenum target, api::GLsizei start, api::GLint x, api::GLint y, api::GLsizei width)
{
	log() << warning << "glCopyColorSubTableEXT not available." << std::endl;
}

static void fallbackCopyColorTable(api::GLenum target, api::GLenum internalformat, api::GLint x, api::GLint y, api::GLsizei width)
{
	log() << warning << "glCopyColorTable not available." << std::endl;
}

static void fallbackCopyColorTableSGI(api::GLenum target, api::GLenum internalformat, api::GLint x, api::GLint y, api::GLsizei width)
{
	log() << warning << "glCopyColorTableSGI not available." << std::endl;
}

static void fallbackCopyConvolutionFilter1D(api::GLenum target, api::GLenum internalformat, api::GLint x, api::GLint y, api::GLsizei width)
{
	log() << warning << "glCopyConvolutionFilter1D not available." << std::endl;
}

static void fallbackCopyConvolutionFilter1DEXT(api::GLenum target, api::GLenum internalformat, api::GLint x, api::GLint y, api::GLsizei width)
{
	log() << warning << "glCopyConvolutionFilter1DEXT not available." << std::endl;
}

static void fallbackCopyConvolutionFilter2D(api::GLenum target, api::GLenum internalformat, api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "glCopyConvolutionFilter2D not available." << std::endl;
}

static void fallbackCopyConvolutionFilter2DEXT(api::GLenum target, api::GLenum internalformat, api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "glCopyConvolutionFilter2DEXT not available." << std::endl;
}

static void fallbackCopyImageSubDataNV(api::GLuint srcName, api::GLenum srcTarget, api::GLint srcLevel, api::GLint srcX, api::GLint srcY, api::GLint srcZ, api::GLuint dstName, api::GLenum dstTarget, api::GLint dstLevel, api::GLint dstX, api::GLint dstY, api::GLint dstZ, api::GLsizei width, api::GLsizei height, api::GLsizei depth)
{
	log() << warning << "glCopyImageSubDataNV not available." << std::endl;
}

static void fallbackCopyMultiTexImage1DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLint x, api::GLint y, api::GLsizei width, api::GLint border)
{
	log() << warning << "glCopyMultiTexImage1DEXT not available." << std::endl;
}

static void fallbackCopyMultiTexImage2DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height, api::GLint border)
{
	log() << warning << "glCopyMultiTexImage2DEXT not available." << std::endl;
}

static void fallbackCopyMultiTexSubImage1DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint x, api::GLint y, api::GLsizei width)
{
	log() << warning << "glCopyMultiTexSubImage1DEXT not available." << std::endl;
}

static void fallbackCopyMultiTexSubImage2DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "glCopyMultiTexSubImage2DEXT not available." << std::endl;
}

static void fallbackCopyMultiTexSubImage3DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint zoffset, api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "glCopyMultiTexSubImage3DEXT not available." << std::endl;
}

static void fallbackCopyPixels(api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height, api::GLenum type)
{
	log() << warning << "glCopyPixels not available." << std::endl;
}

static void fallbackCopyTexImage1D(api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLint x, api::GLint y, api::GLsizei width, api::GLint border)
{
	log() << warning << "glCopyTexImage1D not available." << std::endl;
}

static void fallbackCopyTexImage1DEXT(api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLint x, api::GLint y, api::GLsizei width, api::GLint border)
{
	log() << warning << "glCopyTexImage1DEXT not available." << std::endl;
}

static void fallbackCopyTexImage2D(api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height, api::GLint border)
{
	log() << warning << "glCopyTexImage2D not available." << std::endl;
}

static void fallbackCopyTexImage2DEXT(api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height, api::GLint border)
{
	log() << warning << "glCopyTexImage2DEXT not available." << std::endl;
}

static void fallbackCopyTexSubImage1D(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint x, api::GLint y, api::GLsizei width)
{
	log() << warning << "glCopyTexSubImage1D not available." << std::endl;
}

static void fallbackCopyTexSubImage1DEXT(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint x, api::GLint y, api::GLsizei width)
{
	log() << warning << "glCopyTexSubImage1DEXT not available." << std::endl;
}

static void fallbackCopyTexSubImage2D(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "glCopyTexSubImage2D not available." << std::endl;
}

static void fallbackCopyTexSubImage2DEXT(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "glCopyTexSubImage2DEXT not available." << std::endl;
}

static void fallbackCopyTexSubImage3D(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint zoffset, api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "glCopyTexSubImage3D not available." << std::endl;
}

static void fallbackCopyTexSubImage3DEXT(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint zoffset, api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "glCopyTexSubImage3DEXT not available." << std::endl;
}

static void fallbackCopyTextureImage1DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLint x, api::GLint y, api::GLsizei width, api::GLint border)
{
	log() << warning << "glCopyTextureImage1DEXT not available." << std::endl;
}

static void fallbackCopyTextureImage2DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height, api::GLint border)
{
	log() << warning << "glCopyTextureImage2DEXT not available." << std::endl;
}

static void fallbackCopyTextureSubImage1DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint x, api::GLint y, api::GLsizei width)
{
	log() << warning << "glCopyTextureSubImage1DEXT not available." << std::endl;
}

static void fallbackCopyTextureSubImage2DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "glCopyTextureSubImage2DEXT not available." << std::endl;
}

static void fallbackCopyTextureSubImage3DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint zoffset, api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "glCopyTextureSubImage3DEXT not available." << std::endl;
}

static api::GLuint fallbackCreateProgram()
{
	log() << warning << "glCreateProgram not available." << std::endl;
}

static api::GLhandleARB fallbackCreateProgramObjectARB()
{
	log() << warning << "glCreateProgramObjectARB not available." << std::endl;
}

static api::GLuint fallbackCreateShader(api::GLenum type)
{
	log() << warning << "glCreateShader not available." << std::endl;
}

static api::GLhandleARB fallbackCreateShaderObjectARB(api::GLenum shaderType)
{
	log() << warning << "glCreateShaderObjectARB not available." << std::endl;
}

static api::GLuint fallbackCreateShaderProgramEXT(api::GLenum type, const api::GLchar* string)
{
	log() << warning << "glCreateShaderProgramEXT not available." << std::endl;
}

static void fallbackCullFace(api::GLenum mode)
{
	log() << warning << "glCullFace not available." << std::endl;
}

static void fallbackCullParameterdvEXT(api::GLenum pname, api::GLdouble* params)
{
	log() << warning << "glCullParameterdvEXT not available." << std::endl;
}

static void fallbackCullParameterfvEXT(api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glCullParameterfvEXT not available." << std::endl;
}

static void fallbackCurrentPaletteMatrixARB(api::GLint index)
{
	log() << warning << "glCurrentPaletteMatrixARB not available." << std::endl;
}

static void fallbackDeformSGIX(api::GLbitfield mask)
{
	log() << warning << "glDeformSGIX not available." << std::endl;
}

static void fallbackDeformationMap3dSGIX(api::GLenum target, api::GLdouble u1, api::GLdouble u2, api::GLint ustride, api::GLint uorder, api::GLdouble v1, api::GLdouble v2, api::GLint vstride, api::GLint vorder, api::GLdouble w1, api::GLdouble w2, api::GLint wstride, api::GLint worder, const api::GLdouble* points)
{
	log() << warning << "glDeformationMap3dSGIX not available." << std::endl;
}

static void fallbackDeformationMap3fSGIX(api::GLenum target, api::GLfloat u1, api::GLfloat u2, api::GLint ustride, api::GLint uorder, api::GLfloat v1, api::GLfloat v2, api::GLint vstride, api::GLint vorder, api::GLfloat w1, api::GLfloat w2, api::GLint wstride, api::GLint worder, const api::GLfloat* points)
{
	log() << warning << "glDeformationMap3fSGIX not available." << std::endl;
}

static void fallbackDeleteAsyncMarkersSGIX(api::GLuint marker, api::GLsizei range)
{
	log() << warning << "glDeleteAsyncMarkersSGIX not available." << std::endl;
}

static void fallbackDeleteBuffers(api::GLsizei n, const api::GLuint* buffers)
{
	log() << warning << "glDeleteBuffers not available." << std::endl;
}

static void fallbackDeleteBuffersARB(api::GLsizei n, const api::GLuint* buffers)
{
	log() << warning << "glDeleteBuffersARB not available." << std::endl;
}

static void fallbackDeleteFencesAPPLE(api::GLsizei n, const api::GLuint* fences)
{
	log() << warning << "glDeleteFencesAPPLE not available." << std::endl;
}

static void fallbackDeleteFencesNV(api::GLsizei n, const api::GLuint* fences)
{
	log() << warning << "glDeleteFencesNV not available." << std::endl;
}

static void fallbackDeleteFragmentShaderATI(api::GLuint id)
{
	log() << warning << "glDeleteFragmentShaderATI not available." << std::endl;
}

static void fallbackDeleteFramebuffers(api::GLsizei n, const api::GLuint* framebuffers)
{
	log() << warning << "glDeleteFramebuffers not available." << std::endl;
}

static void fallbackDeleteFramebuffersEXT(api::GLsizei n, const api::GLuint* framebuffers)
{
	log() << warning << "glDeleteFramebuffersEXT not available." << std::endl;
}

static void fallbackDeleteLists(api::GLuint list, api::GLsizei range)
{
	log() << warning << "glDeleteLists not available." << std::endl;
}

static void fallbackDeleteNamedStringARB(api::GLint namelen, const api::GLchar* name)
{
	log() << warning << "glDeleteNamedStringARB not available." << std::endl;
}

static void fallbackDeleteObjectARB(api::GLhandleARB obj)
{
	log() << warning << "glDeleteObjectARB not available." << std::endl;
}

static void fallbackDeleteOcclusionQueriesNV(api::GLsizei n, const api::GLuint* ids)
{
	log() << warning << "glDeleteOcclusionQueriesNV not available." << std::endl;
}

static void fallbackDeletePerfMonitorsAMD(api::GLsizei n, api::GLuint* monitors)
{
	log() << warning << "glDeletePerfMonitorsAMD not available." << std::endl;
}

static void fallbackDeleteProgram(api::GLuint program)
{
	log() << warning << "glDeleteProgram not available." << std::endl;
}

static void fallbackDeleteProgramsARB(api::GLsizei n, const api::GLuint* programs)
{
	log() << warning << "glDeleteProgramsARB not available." << std::endl;
}

static void fallbackDeleteProgramsNV(api::GLsizei n, const api::GLuint* programs)
{
	log() << warning << "glDeleteProgramsNV not available." << std::endl;
}

static void fallbackDeleteQueries(api::GLsizei n, const api::GLuint* ids)
{
	log() << warning << "glDeleteQueries not available." << std::endl;
}

static void fallbackDeleteQueriesARB(api::GLsizei n, const api::GLuint* ids)
{
	log() << warning << "glDeleteQueriesARB not available." << std::endl;
}

static void fallbackDeleteRenderbuffers(api::GLsizei n, const api::GLuint* renderbuffers)
{
	log() << warning << "glDeleteRenderbuffers not available." << std::endl;
}

static void fallbackDeleteRenderbuffersEXT(api::GLsizei n, const api::GLuint* renderbuffers)
{
	log() << warning << "glDeleteRenderbuffersEXT not available." << std::endl;
}

static void fallbackDeleteSamplers(api::GLsizei count, const api::GLuint* samplers)
{
	log() << warning << "glDeleteSamplers not available." << std::endl;
}

static void fallbackDeleteShader(api::GLuint shader)
{
	log() << warning << "glDeleteShader not available." << std::endl;
}

static void fallbackDeleteSync(api::GLsync sync)
{
	log() << warning << "glDeleteSync not available." << std::endl;
}

static void fallbackDeleteTextures(api::GLsizei n, const api::GLuint* textures)
{
	log() << warning << "glDeleteTextures not available." << std::endl;
}

static void fallbackDeleteTexturesEXT(api::GLsizei n, const api::GLuint* textures)
{
	log() << warning << "glDeleteTexturesEXT not available." << std::endl;
}

static void fallbackDeleteTransformFeedbacks(api::GLsizei n, const api::GLuint* ids)
{
	log() << warning << "glDeleteTransformFeedbacks not available." << std::endl;
}

static void fallbackDeleteTransformFeedbacksNV(api::GLsizei n, const api::GLuint* ids)
{
	log() << warning << "glDeleteTransformFeedbacksNV not available." << std::endl;
}

static void fallbackDeleteVertexArrays(api::GLsizei n, const api::GLuint* arrays)
{
	log() << warning << "glDeleteVertexArrays not available." << std::endl;
}

static void fallbackDeleteVertexArraysAPPLE(api::GLsizei n, const api::GLuint* arrays)
{
	log() << warning << "glDeleteVertexArraysAPPLE not available." << std::endl;
}

static void fallbackDeleteVertexShaderEXT(api::GLuint id)
{
	log() << warning << "glDeleteVertexShaderEXT not available." << std::endl;
}

static void fallbackDepthBoundsEXT(api::GLclampd zmin, api::GLclampd zmax)
{
	log() << warning << "glDepthBoundsEXT not available." << std::endl;
}

static void fallbackDepthBoundsdNV(api::GLdouble zmin, api::GLdouble zmax)
{
	log() << warning << "glDepthBoundsdNV not available." << std::endl;
}

static void fallbackDepthFunc(api::GLenum func)
{
	log() << warning << "glDepthFunc not available." << std::endl;
}

static void fallbackDepthMask(api::GLboolean flag)
{
	log() << warning << "glDepthMask not available." << std::endl;
}

static void fallbackDepthRange(api::GLclampd near, api::GLclampd far)
{
	log() << warning << "glDepthRange not available." << std::endl;
}

static void fallbackDepthRangedNV(api::GLdouble zNear, api::GLdouble zFar)
{
	log() << warning << "glDepthRangedNV not available." << std::endl;
}

static void fallbackDetachObjectARB(api::GLhandleARB containerObj, api::GLhandleARB attachedObj)
{
	log() << warning << "glDetachObjectARB not available." << std::endl;
}

static void fallbackDetachShader(api::GLuint program, api::GLuint shader)
{
	log() << warning << "glDetachShader not available." << std::endl;
}

static void fallbackDetailTexFuncSGIS(api::GLenum target, api::GLsizei n, const api::GLfloat* points)
{
	log() << warning << "glDetailTexFuncSGIS not available." << std::endl;
}

static void fallbackDisable(api::GLenum cap)
{
	log() << warning << "glDisable not available." << std::endl;
}

static void fallbackDisableClientState(api::GLenum array)
{
	log() << warning << "glDisableClientState not available." << std::endl;
}

static void fallbackDisableClientStateIndexedEXT(api::GLenum array, api::GLuint index)
{
	log() << warning << "glDisableClientStateIndexedEXT not available." << std::endl;
}

static void fallbackDisableIndexedEXT(api::GLenum target, api::GLuint index)
{
	log() << warning << "glDisableIndexedEXT not available." << std::endl;
}

static void fallbackDisableVariantClientStateEXT(api::GLuint id)
{
	log() << warning << "glDisableVariantClientStateEXT not available." << std::endl;
}

static void fallbackDisableVertexAttribAPPLE(api::GLuint index, api::GLenum pname)
{
	log() << warning << "glDisableVertexAttribAPPLE not available." << std::endl;
}

static void fallbackDisableVertexAttribArray(api::GLuint index)
{
	log() << warning << "glDisableVertexAttribArray not available." << std::endl;
}

static void fallbackDisableVertexAttribArrayARB(api::GLuint index)
{
	log() << warning << "glDisableVertexAttribArrayARB not available." << std::endl;
}

static void fallbackDisablei(api::GLenum target, api::GLuint index)
{
	log() << warning << "glDisablei not available." << std::endl;
}

static void fallbackDrawArrays(api::GLenum mode, api::GLint first, api::GLsizei count)
{
	log() << warning << "glDrawArrays not available." << std::endl;
}

static void fallbackDrawArraysEXT(api::GLenum mode, api::GLint first, api::GLsizei count)
{
	log() << warning << "glDrawArraysEXT not available." << std::endl;
}

static void fallbackDrawArraysIndirect(api::GLenum mode, const api::GLvoid* indirect)
{
	log() << warning << "glDrawArraysIndirect not available." << std::endl;
}

static void fallbackDrawArraysInstanced(api::GLenum mode, api::GLint first, api::GLsizei count, api::GLsizei primcount)
{
	log() << warning << "glDrawArraysInstanced not available." << std::endl;
}

static void fallbackDrawArraysInstancedARB(api::GLenum mode, api::GLint first, api::GLsizei count, api::GLsizei primcount)
{
	log() << warning << "glDrawArraysInstancedARB not available." << std::endl;
}

static void fallbackDrawArraysInstancedEXT(api::GLenum mode, api::GLint start, api::GLsizei count, api::GLsizei primcount)
{
	log() << warning << "glDrawArraysInstancedEXT not available." << std::endl;
}

static void fallbackDrawBuffer(api::GLenum mode)
{
	log() << warning << "glDrawBuffer not available." << std::endl;
}

static void fallbackDrawBuffers(api::GLsizei n, const api::GLenum* bufs)
{
	log() << warning << "glDrawBuffers not available." << std::endl;
}

static void fallbackDrawBuffersARB(api::GLsizei n, const api::GLenum* bufs)
{
	log() << warning << "glDrawBuffersARB not available." << std::endl;
}

static void fallbackDrawBuffersATI(api::GLsizei n, const api::GLenum* bufs)
{
	log() << warning << "glDrawBuffersATI not available." << std::endl;
}

static void fallbackDrawElementArrayAPPLE(api::GLenum mode, api::GLint first, api::GLsizei count)
{
	log() << warning << "glDrawElementArrayAPPLE not available." << std::endl;
}

static void fallbackDrawElementArrayATI(api::GLenum mode, api::GLsizei count)
{
	log() << warning << "glDrawElementArrayATI not available." << std::endl;
}

static void fallbackDrawElements(api::GLenum mode, api::GLsizei count, api::GLenum type, const api::GLvoid* indices)
{
	log() << warning << "glDrawElements not available." << std::endl;
}

static void fallbackDrawElementsBaseVertex(api::GLenum mode, api::GLsizei count, api::GLenum type, const api::GLvoid* indices, api::GLint basevertex)
{
	log() << warning << "glDrawElementsBaseVertex not available." << std::endl;
}

static void fallbackDrawElementsIndirect(api::GLenum mode, api::GLenum type, const api::GLvoid* indirect)
{
	log() << warning << "glDrawElementsIndirect not available." << std::endl;
}

static void fallbackDrawElementsInstanced(api::GLenum mode, api::GLsizei count, api::GLenum type, const api::GLvoid* indices, api::GLsizei primcount)
{
	log() << warning << "glDrawElementsInstanced not available." << std::endl;
}

static void fallbackDrawElementsInstancedARB(api::GLenum mode, api::GLsizei count, api::GLenum type, const api::GLvoid* indices, api::GLsizei primcount)
{
	log() << warning << "glDrawElementsInstancedARB not available." << std::endl;
}

static void fallbackDrawElementsInstancedBaseVertex(api::GLenum mode, api::GLsizei count, api::GLenum type, const api::GLvoid* indices, api::GLsizei primcount, api::GLint basevertex)
{
	log() << warning << "glDrawElementsInstancedBaseVertex not available." << std::endl;
}

static void fallbackDrawElementsInstancedEXT(api::GLenum mode, api::GLsizei count, api::GLenum type, const api::GLvoid* indices, api::GLsizei primcount)
{
	log() << warning << "glDrawElementsInstancedEXT not available." << std::endl;
}

static void fallbackDrawMeshArraysSUN(api::GLenum mode, api::GLint first, api::GLsizei count, api::GLsizei width)
{
	log() << warning << "glDrawMeshArraysSUN not available." << std::endl;
}

static void fallbackDrawPixels(api::GLsizei width, api::GLsizei height, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "glDrawPixels not available." << std::endl;
}

static void fallbackDrawRangeElementArrayAPPLE(api::GLenum mode, api::GLuint start, api::GLuint end, api::GLint first, api::GLsizei count)
{
	log() << warning << "glDrawRangeElementArrayAPPLE not available." << std::endl;
}

static void fallbackDrawRangeElementArrayATI(api::GLenum mode, api::GLuint start, api::GLuint end, api::GLsizei count)
{
	log() << warning << "glDrawRangeElementArrayATI not available." << std::endl;
}

static void fallbackDrawRangeElements(api::GLenum mode, api::GLuint start, api::GLuint end, api::GLsizei count, api::GLenum type, const api::GLvoid* indices)
{
	log() << warning << "glDrawRangeElements not available." << std::endl;
}

static void fallbackDrawRangeElementsBaseVertex(api::GLenum mode, api::GLuint start, api::GLuint end, api::GLsizei count, api::GLenum type, const api::GLvoid* indices, api::GLint basevertex)
{
	log() << warning << "glDrawRangeElementsBaseVertex not available." << std::endl;
}

static void fallbackDrawRangeElementsEXT(api::GLenum mode, api::GLuint start, api::GLuint end, api::GLsizei count, api::GLenum type, const api::GLvoid* indices)
{
	log() << warning << "glDrawRangeElementsEXT not available." << std::endl;
}

static void fallbackDrawTransformFeedback(api::GLenum mode, api::GLuint id)
{
	log() << warning << "glDrawTransformFeedback not available." << std::endl;
}

static void fallbackDrawTransformFeedbackNV(api::GLenum mode, api::GLuint id)
{
	log() << warning << "glDrawTransformFeedbackNV not available." << std::endl;
}

static void fallbackDrawTransformFeedbackStream(api::GLenum mode, api::GLuint id, api::GLuint stream)
{
	log() << warning << "glDrawTransformFeedbackStream not available." << std::endl;
}

static void fallbackEdgeFlag(api::GLboolean flag)
{
	log() << warning << "glEdgeFlag not available." << std::endl;
}

static void fallbackEdgeFlagFormatNV(api::GLsizei stride)
{
	log() << warning << "glEdgeFlagFormatNV not available." << std::endl;
}

static void fallbackEdgeFlagPointer(api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "glEdgeFlagPointer not available." << std::endl;
}

static void fallbackEdgeFlagPointerEXT(api::GLsizei stride, api::GLsizei count, const api::GLboolean* pointer)
{
	log() << warning << "glEdgeFlagPointerEXT not available." << std::endl;
}

static void fallbackEdgeFlagPointerListIBM(api::GLint stride, const api::GLboolean** pointer, api::GLint ptrstride)
{
	log() << warning << "glEdgeFlagPointerListIBM not available." << std::endl;
}

static void fallbackEdgeFlagv(const api::GLboolean* flag)
{
	log() << warning << "glEdgeFlagv not available." << std::endl;
}

static void fallbackElementPointerAPPLE(api::GLenum type, const api::GLvoid* pointer)
{
	log() << warning << "glElementPointerAPPLE not available." << std::endl;
}

static void fallbackElementPointerATI(api::GLenum type, const api::GLvoid* pointer)
{
	log() << warning << "glElementPointerATI not available." << std::endl;
}

static void fallbackEnable(api::GLenum cap)
{
	log() << warning << "glEnable not available." << std::endl;
}

static void fallbackEnableClientState(api::GLenum array)
{
	log() << warning << "glEnableClientState not available." << std::endl;
}

static void fallbackEnableClientStateIndexedEXT(api::GLenum array, api::GLuint index)
{
	log() << warning << "glEnableClientStateIndexedEXT not available." << std::endl;
}

static void fallbackEnableIndexedEXT(api::GLenum target, api::GLuint index)
{
	log() << warning << "glEnableIndexedEXT not available." << std::endl;
}

static void fallbackEnableVariantClientStateEXT(api::GLuint id)
{
	log() << warning << "glEnableVariantClientStateEXT not available." << std::endl;
}

static void fallbackEnableVertexAttribAPPLE(api::GLuint index, api::GLenum pname)
{
	log() << warning << "glEnableVertexAttribAPPLE not available." << std::endl;
}

static void fallbackEnableVertexAttribArray(api::GLuint index)
{
	log() << warning << "glEnableVertexAttribArray not available." << std::endl;
}

static void fallbackEnableVertexAttribArrayARB(api::GLuint index)
{
	log() << warning << "glEnableVertexAttribArrayARB not available." << std::endl;
}

static void fallbackEnablei(api::GLenum target, api::GLuint index)
{
	log() << warning << "glEnablei not available." << std::endl;
}

static void fallbackEnd()
{
	log() << warning << "glEnd not available." << std::endl;
}

static void fallbackEndConditionalRender()
{
	log() << warning << "glEndConditionalRender not available." << std::endl;
}

static void fallbackEndConditionalRenderNV()
{
	log() << warning << "glEndConditionalRenderNV not available." << std::endl;
}

static void fallbackEndFragmentShaderATI()
{
	log() << warning << "glEndFragmentShaderATI not available." << std::endl;
}

static void fallbackEndList()
{
	log() << warning << "glEndList not available." << std::endl;
}

static void fallbackEndOcclusionQueryNV()
{
	log() << warning << "glEndOcclusionQueryNV not available." << std::endl;
}

static void fallbackEndPerfMonitorAMD(api::GLuint monitor)
{
	log() << warning << "glEndPerfMonitorAMD not available." << std::endl;
}

static void fallbackEndQuery(api::GLenum target)
{
	log() << warning << "glEndQuery not available." << std::endl;
}

static void fallbackEndQueryARB(api::GLenum target)
{
	log() << warning << "glEndQueryARB not available." << std::endl;
}

static void fallbackEndQueryIndexed(api::GLenum target, api::GLuint index)
{
	log() << warning << "glEndQueryIndexed not available." << std::endl;
}

static void fallbackEndTransformFeedback()
{
	log() << warning << "glEndTransformFeedback not available." << std::endl;
}

static void fallbackEndTransformFeedbackEXT()
{
	log() << warning << "glEndTransformFeedbackEXT not available." << std::endl;
}

static void fallbackEndTransformFeedbackNV()
{
	log() << warning << "glEndTransformFeedbackNV not available." << std::endl;
}

static void fallbackEndVertexShaderEXT()
{
	log() << warning << "glEndVertexShaderEXT not available." << std::endl;
}

static void fallbackEndVideoCaptureNV(api::GLuint video_capture_slot)
{
	log() << warning << "glEndVideoCaptureNV not available." << std::endl;
}

static void fallbackEvalCoord1d(api::GLdouble u)
{
	log() << warning << "glEvalCoord1d not available." << std::endl;
}

static void fallbackEvalCoord1dv(const api::GLdouble* u)
{
	log() << warning << "glEvalCoord1dv not available." << std::endl;
}

static void fallbackEvalCoord1f(api::GLfloat u)
{
	log() << warning << "glEvalCoord1f not available." << std::endl;
}

static void fallbackEvalCoord1fv(const api::GLfloat* u)
{
	log() << warning << "glEvalCoord1fv not available." << std::endl;
}

static void fallbackEvalCoord2d(api::GLdouble u, api::GLdouble v)
{
	log() << warning << "glEvalCoord2d not available." << std::endl;
}

static void fallbackEvalCoord2dv(const api::GLdouble* u)
{
	log() << warning << "glEvalCoord2dv not available." << std::endl;
}

static void fallbackEvalCoord2f(api::GLfloat u, api::GLfloat v)
{
	log() << warning << "glEvalCoord2f not available." << std::endl;
}

static void fallbackEvalCoord2fv(const api::GLfloat* u)
{
	log() << warning << "glEvalCoord2fv not available." << std::endl;
}

static void fallbackEvalMapsNV(api::GLenum target, api::GLenum mode)
{
	log() << warning << "glEvalMapsNV not available." << std::endl;
}

static void fallbackEvalMesh1(api::GLenum mode, api::GLint i1, api::GLint i2)
{
	log() << warning << "glEvalMesh1 not available." << std::endl;
}

static void fallbackEvalMesh2(api::GLenum mode, api::GLint i1, api::GLint i2, api::GLint j1, api::GLint j2)
{
	log() << warning << "glEvalMesh2 not available." << std::endl;
}

static void fallbackEvalPoint1(api::GLint i)
{
	log() << warning << "glEvalPoint1 not available." << std::endl;
}

static void fallbackEvalPoint2(api::GLint i, api::GLint j)
{
	log() << warning << "glEvalPoint2 not available." << std::endl;
}

static void fallbackExecuteProgramNV(api::GLenum target, api::GLuint id, const api::GLfloat* params)
{
	log() << warning << "glExecuteProgramNV not available." << std::endl;
}

static void fallbackExtractComponentEXT(api::GLuint res, api::GLuint src, api::GLuint num)
{
	log() << warning << "glExtractComponentEXT not available." << std::endl;
}

static void fallbackFeedbackBuffer(api::GLsizei size, api::GLenum type, api::GLfloat* buffer)
{
	log() << warning << "glFeedbackBuffer not available." << std::endl;
}

static api::GLsync fallbackFenceSync(api::GLenum condition, api::GLbitfield flags)
{
	log() << warning << "glFenceSync not available." << std::endl;
}

static void fallbackFinalCombinerInputNV(api::GLenum variable, api::GLenum input, api::GLenum mapping, api::GLenum componentUsage)
{
	log() << warning << "glFinalCombinerInputNV not available." << std::endl;
}

static void fallbackFinish()
{
	log() << warning << "glFinish not available." << std::endl;
}

static api::GLint fallbackFinishAsyncSGIX(api::GLuint* markerp)
{
	log() << warning << "glFinishAsyncSGIX not available." << std::endl;
}

static void fallbackFinishFenceAPPLE(api::GLuint fence)
{
	log() << warning << "glFinishFenceAPPLE not available." << std::endl;
}

static void fallbackFinishFenceNV(api::GLuint fence)
{
	log() << warning << "glFinishFenceNV not available." << std::endl;
}

static void fallbackFinishObjectAPPLE(api::GLenum object, api::GLint name)
{
	log() << warning << "glFinishObjectAPPLE not available." << std::endl;
}

static void fallbackFinishTextureSUNX()
{
	log() << warning << "glFinishTextureSUNX not available." << std::endl;
}

static void fallbackFlush()
{
	log() << warning << "glFlush not available." << std::endl;
}

static void fallbackFlushMappedBufferRange(api::GLenum target, api::GLintptr offset, api::GLsizeiptr length)
{
	log() << warning << "glFlushMappedBufferRange not available." << std::endl;
}

static void fallbackFlushMappedBufferRangeAPPLE(api::GLenum target, api::GLintptr offset, api::GLsizeiptr size)
{
	log() << warning << "glFlushMappedBufferRangeAPPLE not available." << std::endl;
}

static void fallbackFlushPixelDataRangeNV(api::GLenum target)
{
	log() << warning << "glFlushPixelDataRangeNV not available." << std::endl;
}

static void fallbackFlushRasterSGIX()
{
	log() << warning << "glFlushRasterSGIX not available." << std::endl;
}

static void fallbackFlushVertexArrayRangeAPPLE(api::GLsizei length, api::GLvoid* pointer)
{
	log() << warning << "glFlushVertexArrayRangeAPPLE not available." << std::endl;
}

static void fallbackFlushVertexArrayRangeNV()
{
	log() << warning << "glFlushVertexArrayRangeNV not available." << std::endl;
}

static void fallbackFogCoordFormatNV(api::GLenum type, api::GLsizei stride)
{
	log() << warning << "glFogCoordFormatNV not available." << std::endl;
}

static void fallbackFogCoordPointer(api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "glFogCoordPointer not available." << std::endl;
}

static void fallbackFogCoordPointerEXT(api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "glFogCoordPointerEXT not available." << std::endl;
}

static void fallbackFogCoordPointerListIBM(api::GLenum type, api::GLint stride, const api::GLvoid** pointer, api::GLint ptrstride)
{
	log() << warning << "glFogCoordPointerListIBM not available." << std::endl;
}

static void fallbackFogCoordd(api::GLdouble coord)
{
	log() << warning << "glFogCoordd not available." << std::endl;
}

static void fallbackFogCoorddEXT(api::GLdouble coord)
{
	log() << warning << "glFogCoorddEXT not available." << std::endl;
}

static void fallbackFogCoorddv(const api::GLdouble* coord)
{
	log() << warning << "glFogCoorddv not available." << std::endl;
}

static void fallbackFogCoorddvEXT(const api::GLdouble* coord)
{
	log() << warning << "glFogCoorddvEXT not available." << std::endl;
}

static void fallbackFogCoordf(api::GLfloat coord)
{
	log() << warning << "glFogCoordf not available." << std::endl;
}

static void fallbackFogCoordfEXT(api::GLfloat coord)
{
	log() << warning << "glFogCoordfEXT not available." << std::endl;
}

static void fallbackFogCoordfv(const api::GLfloat* coord)
{
	log() << warning << "glFogCoordfv not available." << std::endl;
}

static void fallbackFogCoordfvEXT(const api::GLfloat* coord)
{
	log() << warning << "glFogCoordfvEXT not available." << std::endl;
}

static void fallbackFogCoordhNV(api::GLhalfNV fog)
{
	log() << warning << "glFogCoordhNV not available." << std::endl;
}

static void fallbackFogCoordhvNV(const api::GLhalfNV* fog)
{
	log() << warning << "glFogCoordhvNV not available." << std::endl;
}

static void fallbackFogFuncSGIS(api::GLsizei n, const api::GLfloat* points)
{
	log() << warning << "glFogFuncSGIS not available." << std::endl;
}

static void fallbackFogf(api::GLenum pname, api::GLfloat param)
{
	log() << warning << "glFogf not available." << std::endl;
}

static void fallbackFogfv(api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "glFogfv not available." << std::endl;
}

static void fallbackFogi(api::GLenum pname, api::GLint param)
{
	log() << warning << "glFogi not available." << std::endl;
}

static void fallbackFogiv(api::GLenum pname, const api::GLint* params)
{
	log() << warning << "glFogiv not available." << std::endl;
}

static void fallbackFragmentColorMaterialSGIX(api::GLenum face, api::GLenum mode)
{
	log() << warning << "glFragmentColorMaterialSGIX not available." << std::endl;
}

static void fallbackFragmentLightModelfSGIX(api::GLenum pname, api::GLfloat param)
{
	log() << warning << "glFragmentLightModelfSGIX not available." << std::endl;
}

static void fallbackFragmentLightModelfvSGIX(api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "glFragmentLightModelfvSGIX not available." << std::endl;
}

static void fallbackFragmentLightModeliSGIX(api::GLenum pname, api::GLint param)
{
	log() << warning << "glFragmentLightModeliSGIX not available." << std::endl;
}

static void fallbackFragmentLightModelivSGIX(api::GLenum pname, const api::GLint* params)
{
	log() << warning << "glFragmentLightModelivSGIX not available." << std::endl;
}

static void fallbackFragmentLightfSGIX(api::GLenum light, api::GLenum pname, api::GLfloat param)
{
	log() << warning << "glFragmentLightfSGIX not available." << std::endl;
}

static void fallbackFragmentLightfvSGIX(api::GLenum light, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "glFragmentLightfvSGIX not available." << std::endl;
}

static void fallbackFragmentLightiSGIX(api::GLenum light, api::GLenum pname, api::GLint param)
{
	log() << warning << "glFragmentLightiSGIX not available." << std::endl;
}

static void fallbackFragmentLightivSGIX(api::GLenum light, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "glFragmentLightivSGIX not available." << std::endl;
}

static void fallbackFragmentMaterialfSGIX(api::GLenum face, api::GLenum pname, api::GLfloat param)
{
	log() << warning << "glFragmentMaterialfSGIX not available." << std::endl;
}

static void fallbackFragmentMaterialfvSGIX(api::GLenum face, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "glFragmentMaterialfvSGIX not available." << std::endl;
}

static void fallbackFragmentMaterialiSGIX(api::GLenum face, api::GLenum pname, api::GLint param)
{
	log() << warning << "glFragmentMaterialiSGIX not available." << std::endl;
}

static void fallbackFragmentMaterialivSGIX(api::GLenum face, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "glFragmentMaterialivSGIX not available." << std::endl;
}

static void fallbackFrameTerminatorGREMEDY()
{
	log() << warning << "glFrameTerminatorGREMEDY not available." << std::endl;
}

static void fallbackFrameZoomSGIX(api::GLint factor)
{
	log() << warning << "glFrameZoomSGIX not available." << std::endl;
}

static void fallbackFramebufferDrawBufferEXT(api::GLuint framebuffer, api::GLenum mode)
{
	log() << warning << "glFramebufferDrawBufferEXT not available." << std::endl;
}

static void fallbackFramebufferDrawBuffersEXT(api::GLuint framebuffer, api::GLsizei n, const api::GLenum* bufs)
{
	log() << warning << "glFramebufferDrawBuffersEXT not available." << std::endl;
}

static void fallbackFramebufferReadBufferEXT(api::GLuint framebuffer, api::GLenum mode)
{
	log() << warning << "glFramebufferReadBufferEXT not available." << std::endl;
}

static void fallbackFramebufferRenderbuffer(api::GLenum target, api::GLenum attachment, api::GLenum renderbuffertarget, api::GLuint renderbuffer)
{
	log() << warning << "glFramebufferRenderbuffer not available." << std::endl;
}

static void fallbackFramebufferRenderbufferEXT(api::GLenum target, api::GLenum attachment, api::GLenum renderbuffertarget, api::GLuint renderbuffer)
{
	log() << warning << "glFramebufferRenderbufferEXT not available." << std::endl;
}

static void fallbackFramebufferTexture(api::GLenum target, api::GLenum attachment, api::GLuint texture, api::GLint level)
{
	log() << warning << "glFramebufferTexture not available." << std::endl;
}

static void fallbackFramebufferTexture1D(api::GLenum target, api::GLenum attachment, api::GLenum textarget, api::GLuint texture, api::GLint level)
{
	log() << warning << "glFramebufferTexture1D not available." << std::endl;
}

static void fallbackFramebufferTexture1DEXT(api::GLenum target, api::GLenum attachment, api::GLenum textarget, api::GLuint texture, api::GLint level)
{
	log() << warning << "glFramebufferTexture1DEXT not available." << std::endl;
}

static void fallbackFramebufferTexture2D(api::GLenum target, api::GLenum attachment, api::GLenum textarget, api::GLuint texture, api::GLint level)
{
	log() << warning << "glFramebufferTexture2D not available." << std::endl;
}

static void fallbackFramebufferTexture2DEXT(api::GLenum target, api::GLenum attachment, api::GLenum textarget, api::GLuint texture, api::GLint level)
{
	log() << warning << "glFramebufferTexture2DEXT not available." << std::endl;
}

static void fallbackFramebufferTexture3D(api::GLenum target, api::GLenum attachment, api::GLenum textarget, api::GLuint texture, api::GLint level, api::GLint zoffset)
{
	log() << warning << "glFramebufferTexture3D not available." << std::endl;
}

static void fallbackFramebufferTexture3DEXT(api::GLenum target, api::GLenum attachment, api::GLenum textarget, api::GLuint texture, api::GLint level, api::GLint zoffset)
{
	log() << warning << "glFramebufferTexture3DEXT not available." << std::endl;
}

static void fallbackFramebufferTextureARB(api::GLenum target, api::GLenum attachment, api::GLuint texture, api::GLint level)
{
	log() << warning << "glFramebufferTextureARB not available." << std::endl;
}

static void fallbackFramebufferTextureEXT(api::GLenum target, api::GLenum attachment, api::GLuint texture, api::GLint level)
{
	log() << warning << "glFramebufferTextureEXT not available." << std::endl;
}

static void fallbackFramebufferTextureFaceARB(api::GLenum target, api::GLenum attachment, api::GLuint texture, api::GLint level, api::GLenum face)
{
	log() << warning << "glFramebufferTextureFaceARB not available." << std::endl;
}

static void fallbackFramebufferTextureFaceEXT(api::GLenum target, api::GLenum attachment, api::GLuint texture, api::GLint level, api::GLenum face)
{
	log() << warning << "glFramebufferTextureFaceEXT not available." << std::endl;
}

static void fallbackFramebufferTextureLayer(api::GLenum target, api::GLenum attachment, api::GLuint texture, api::GLint level, api::GLint layer)
{
	log() << warning << "glFramebufferTextureLayer not available." << std::endl;
}

static void fallbackFramebufferTextureLayerARB(api::GLenum target, api::GLenum attachment, api::GLuint texture, api::GLint level, api::GLint layer)
{
	log() << warning << "glFramebufferTextureLayerARB not available." << std::endl;
}

static void fallbackFramebufferTextureLayerEXT(api::GLenum target, api::GLenum attachment, api::GLuint texture, api::GLint level, api::GLint layer)
{
	log() << warning << "glFramebufferTextureLayerEXT not available." << std::endl;
}

static void fallbackFreeObjectBufferATI(api::GLuint buffer)
{
	log() << warning << "glFreeObjectBufferATI not available." << std::endl;
}

static void fallbackFrontFace(api::GLenum mode)
{
	log() << warning << "glFrontFace not available." << std::endl;
}

static void fallbackFrustum(api::GLdouble left, api::GLdouble right, api::GLdouble bottom, api::GLdouble top, api::GLdouble zNear, api::GLdouble zFar)
{
	log() << warning << "glFrustum not available." << std::endl;
}

static api::GLuint fallbackGenAsyncMarkersSGIX(api::GLsizei range)
{
	log() << warning << "glGenAsyncMarkersSGIX not available." << std::endl;
}

static void fallbackGenBuffers(api::GLsizei n, api::GLuint* buffers)
{
	log() << warning << "glGenBuffers not available." << std::endl;
}

static void fallbackGenBuffersARB(api::GLsizei n, api::GLuint* buffers)
{
	log() << warning << "glGenBuffersARB not available." << std::endl;
}

static void fallbackGenFencesAPPLE(api::GLsizei n, api::GLuint* fences)
{
	log() << warning << "glGenFencesAPPLE not available." << std::endl;
}

static void fallbackGenFencesNV(api::GLsizei n, api::GLuint* fences)
{
	log() << warning << "glGenFencesNV not available." << std::endl;
}

static api::GLuint fallbackGenFragmentShadersATI(api::GLuint range)
{
	log() << warning << "glGenFragmentShadersATI not available." << std::endl;
}

static void fallbackGenFramebuffers(api::GLsizei n, api::GLuint* framebuffers)
{
	log() << warning << "glGenFramebuffers not available." << std::endl;
}

static void fallbackGenFramebuffersEXT(api::GLsizei n, api::GLuint* framebuffers)
{
	log() << warning << "glGenFramebuffersEXT not available." << std::endl;
}

static api::GLuint fallbackGenLists(api::GLsizei range)
{
	log() << warning << "glGenLists not available." << std::endl;
}

static void fallbackGenOcclusionQueriesNV(api::GLsizei n, api::GLuint* ids)
{
	log() << warning << "glGenOcclusionQueriesNV not available." << std::endl;
}

static void fallbackGenPerfMonitorsAMD(api::GLsizei n, api::GLuint* monitors)
{
	log() << warning << "glGenPerfMonitorsAMD not available." << std::endl;
}

static void fallbackGenProgramsARB(api::GLsizei n, api::GLuint* programs)
{
	log() << warning << "glGenProgramsARB not available." << std::endl;
}

static void fallbackGenProgramsNV(api::GLsizei n, api::GLuint* programs)
{
	log() << warning << "glGenProgramsNV not available." << std::endl;
}

static void fallbackGenQueries(api::GLsizei n, api::GLuint* ids)
{
	log() << warning << "glGenQueries not available." << std::endl;
}

static void fallbackGenQueriesARB(api::GLsizei n, api::GLuint* ids)
{
	log() << warning << "glGenQueriesARB not available." << std::endl;
}

static void fallbackGenRenderbuffers(api::GLsizei n, api::GLuint* renderbuffers)
{
	log() << warning << "glGenRenderbuffers not available." << std::endl;
}

static void fallbackGenRenderbuffersEXT(api::GLsizei n, api::GLuint* renderbuffers)
{
	log() << warning << "glGenRenderbuffersEXT not available." << std::endl;
}

static void fallbackGenSamplers(api::GLsizei count, api::GLuint* samplers)
{
	log() << warning << "glGenSamplers not available." << std::endl;
}

static api::GLuint fallbackGenSymbolsEXT(api::GLenum datatype, api::GLenum storagetype, api::GLenum range, api::GLuint components)
{
	log() << warning << "glGenSymbolsEXT not available." << std::endl;
}

static void fallbackGenTextures(api::GLsizei n, api::GLuint* textures)
{
	log() << warning << "glGenTextures not available." << std::endl;
}

static void fallbackGenTexturesEXT(api::GLsizei n, api::GLuint* textures)
{
	log() << warning << "glGenTexturesEXT not available." << std::endl;
}

static void fallbackGenTransformFeedbacks(api::GLsizei n, api::GLuint* ids)
{
	log() << warning << "glGenTransformFeedbacks not available." << std::endl;
}

static void fallbackGenTransformFeedbacksNV(api::GLsizei n, api::GLuint* ids)
{
	log() << warning << "glGenTransformFeedbacksNV not available." << std::endl;
}

static void fallbackGenVertexArrays(api::GLsizei n, api::GLuint* arrays)
{
	log() << warning << "glGenVertexArrays not available." << std::endl;
}

static void fallbackGenVertexArraysAPPLE(api::GLsizei n, api::GLuint* arrays)
{
	log() << warning << "glGenVertexArraysAPPLE not available." << std::endl;
}

static api::GLuint fallbackGenVertexShadersEXT(api::GLuint range)
{
	log() << warning << "glGenVertexShadersEXT not available." << std::endl;
}

static void fallbackGenerateMipmap(api::GLenum target)
{
	log() << warning << "glGenerateMipmap not available." << std::endl;
}

static void fallbackGenerateMipmapEXT(api::GLenum target)
{
	log() << warning << "glGenerateMipmapEXT not available." << std::endl;
}

static void fallbackGenerateMultiTexMipmapEXT(api::GLenum texunit, api::GLenum target)
{
	log() << warning << "glGenerateMultiTexMipmapEXT not available." << std::endl;
}

static void fallbackGenerateTextureMipmapEXT(api::GLuint texture, api::GLenum target)
{
	log() << warning << "glGenerateTextureMipmapEXT not available." << std::endl;
}

static void fallbackGetActiveAttrib(api::GLuint program, api::GLuint index, api::GLsizei bufSize, api::GLsizei* length, api::GLint* size, api::GLenum* type, api::GLchar* name)
{
	log() << warning << "glGetActiveAttrib not available." << std::endl;
}

static void fallbackGetActiveAttribARB(api::GLhandleARB programObj, api::GLuint index, api::GLsizei maxLength, api::GLsizei* length, api::GLint* size, api::GLenum* type, api::GLcharARB* name)
{
	log() << warning << "glGetActiveAttribARB not available." << std::endl;
}

static void fallbackGetActiveSubroutineName(api::GLuint program, api::GLenum shadertype, api::GLuint index, api::GLsizei bufsize, api::GLsizei* length, api::GLchar* name)
{
	log() << warning << "glGetActiveSubroutineName not available." << std::endl;
}

static void fallbackGetActiveSubroutineUniformName(api::GLuint program, api::GLenum shadertype, api::GLuint index, api::GLsizei bufsize, api::GLsizei* length, api::GLchar* name)
{
	log() << warning << "glGetActiveSubroutineUniformName not available." << std::endl;
}

static void fallbackGetActiveSubroutineUniformiv(api::GLuint program, api::GLenum shadertype, api::GLuint index, api::GLenum pname, api::GLint* values)
{
	log() << warning << "glGetActiveSubroutineUniformiv not available." << std::endl;
}

static void fallbackGetActiveUniform(api::GLuint program, api::GLuint index, api::GLsizei bufSize, api::GLsizei* length, api::GLint* size, api::GLenum* type, api::GLchar* name)
{
	log() << warning << "glGetActiveUniform not available." << std::endl;
}

static void fallbackGetActiveUniformARB(api::GLhandleARB programObj, api::GLuint index, api::GLsizei maxLength, api::GLsizei* length, api::GLint* size, api::GLenum* type, api::GLcharARB* name)
{
	log() << warning << "glGetActiveUniformARB not available." << std::endl;
}

static void fallbackGetActiveUniformBlockName(api::GLuint program, api::GLuint uniformBlockIndex, api::GLsizei bufSize, api::GLsizei* length, api::GLchar* uniformBlockName)
{
	log() << warning << "glGetActiveUniformBlockName not available." << std::endl;
}

static void fallbackGetActiveUniformBlockiv(api::GLuint program, api::GLuint uniformBlockIndex, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetActiveUniformBlockiv not available." << std::endl;
}

static void fallbackGetActiveUniformName(api::GLuint program, api::GLuint uniformIndex, api::GLsizei bufSize, api::GLsizei* length, api::GLchar* uniformName)
{
	log() << warning << "glGetActiveUniformName not available." << std::endl;
}

static void fallbackGetActiveUniformsiv(api::GLuint program, api::GLsizei uniformCount, const api::GLuint* uniformIndices, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetActiveUniformsiv not available." << std::endl;
}

static void fallbackGetActiveVaryingNV(api::GLuint program, api::GLuint index, api::GLsizei bufSize, api::GLsizei* length, api::GLsizei* size, api::GLenum* type, api::GLchar* name)
{
	log() << warning << "glGetActiveVaryingNV not available." << std::endl;
}

static void fallbackGetArrayObjectfvATI(api::GLenum array, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glGetArrayObjectfvATI not available." << std::endl;
}

static void fallbackGetArrayObjectivATI(api::GLenum array, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetArrayObjectivATI not available." << std::endl;
}

static void fallbackGetAttachedObjectsARB(api::GLhandleARB containerObj, api::GLsizei maxCount, api::GLsizei* count, api::GLhandleARB* obj)
{
	log() << warning << "glGetAttachedObjectsARB not available." << std::endl;
}

static void fallbackGetAttachedShaders(api::GLuint program, api::GLsizei maxCount, api::GLsizei* count, api::GLuint* obj)
{
	log() << warning << "glGetAttachedShaders not available." << std::endl;
}

static api::GLint fallbackGetAttribLocation(api::GLuint program, const api::GLchar* name)
{
	log() << warning << "glGetAttribLocation not available." << std::endl;
}

static api::GLint fallbackGetAttribLocationARB(api::GLhandleARB programObj, const api::GLcharARB* name)
{
	log() << warning << "glGetAttribLocationARB not available." << std::endl;
}

static void fallbackGetBooleanIndexedvEXT(api::GLenum target, api::GLuint index, api::GLboolean* data)
{
	log() << warning << "glGetBooleanIndexedvEXT not available." << std::endl;
}

static void fallbackGetBooleani_v(api::GLenum target, api::GLuint index, api::GLboolean* data)
{
	log() << warning << "glGetBooleani_v not available." << std::endl;
}

static void fallbackGetBooleanv(api::GLenum pname, api::GLboolean* params)
{
	log() << warning << "glGetBooleanv not available." << std::endl;
}

static void fallbackGetBufferParameteri64v(api::GLenum target, api::GLenum pname, api::GLint64* params)
{
	log() << warning << "glGetBufferParameteri64v not available." << std::endl;
}

static void fallbackGetBufferParameteriv(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetBufferParameteriv not available." << std::endl;
}

static void fallbackGetBufferParameterivARB(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetBufferParameterivARB not available." << std::endl;
}

static void fallbackGetBufferParameterui64vNV(api::GLenum target, api::GLenum pname, api::GLuint64EXT* params)
{
	log() << warning << "glGetBufferParameterui64vNV not available." << std::endl;
}

static void fallbackGetBufferPointerv(api::GLenum target, api::GLenum pname, api::GLvoid** params)
{
	log() << warning << "glGetBufferPointerv not available." << std::endl;
}

static void fallbackGetBufferPointervARB(api::GLenum target, api::GLenum pname, api::GLvoid** params)
{
	log() << warning << "glGetBufferPointervARB not available." << std::endl;
}

static void fallbackGetBufferSubData(api::GLenum target, api::GLintptr offset, api::GLsizeiptr size, api::GLvoid* data)
{
	log() << warning << "glGetBufferSubData not available." << std::endl;
}

static void fallbackGetBufferSubDataARB(api::GLenum target, api::GLintptrARB offset, api::GLsizeiptrARB size, api::GLvoid* data)
{
	log() << warning << "glGetBufferSubDataARB not available." << std::endl;
}

static void fallbackGetClipPlane(api::GLenum plane, api::GLdouble* equation)
{
	log() << warning << "glGetClipPlane not available." << std::endl;
}

static void fallbackGetColorTable(api::GLenum target, api::GLenum format, api::GLenum type, api::GLvoid* table)
{
	log() << warning << "glGetColorTable not available." << std::endl;
}

static void fallbackGetColorTableEXT(api::GLenum target, api::GLenum format, api::GLenum type, api::GLvoid* data)
{
	log() << warning << "glGetColorTableEXT not available." << std::endl;
}

static void fallbackGetColorTableParameterfv(api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glGetColorTableParameterfv not available." << std::endl;
}

static void fallbackGetColorTableParameterfvEXT(api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glGetColorTableParameterfvEXT not available." << std::endl;
}

static void fallbackGetColorTableParameterfvSGI(api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glGetColorTableParameterfvSGI not available." << std::endl;
}

static void fallbackGetColorTableParameteriv(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetColorTableParameteriv not available." << std::endl;
}

static void fallbackGetColorTableParameterivEXT(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetColorTableParameterivEXT not available." << std::endl;
}

static void fallbackGetColorTableParameterivSGI(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetColorTableParameterivSGI not available." << std::endl;
}

static void fallbackGetColorTableSGI(api::GLenum target, api::GLenum format, api::GLenum type, api::GLvoid* table)
{
	log() << warning << "glGetColorTableSGI not available." << std::endl;
}

static void fallbackGetCombinerInputParameterfvNV(api::GLenum stage, api::GLenum portion, api::GLenum variable, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glGetCombinerInputParameterfvNV not available." << std::endl;
}

static void fallbackGetCombinerInputParameterivNV(api::GLenum stage, api::GLenum portion, api::GLenum variable, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetCombinerInputParameterivNV not available." << std::endl;
}

static void fallbackGetCombinerOutputParameterfvNV(api::GLenum stage, api::GLenum portion, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glGetCombinerOutputParameterfvNV not available." << std::endl;
}

static void fallbackGetCombinerOutputParameterivNV(api::GLenum stage, api::GLenum portion, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetCombinerOutputParameterivNV not available." << std::endl;
}

static void fallbackGetCombinerStageParameterfvNV(api::GLenum stage, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glGetCombinerStageParameterfvNV not available." << std::endl;
}

static void fallbackGetCompressedMultiTexImageEXT(api::GLenum texunit, api::GLenum target, api::GLint lod, api::GLvoid* img)
{
	log() << warning << "glGetCompressedMultiTexImageEXT not available." << std::endl;
}

static void fallbackGetCompressedTexImage(api::GLenum target, api::GLint level, api::GLvoid* img)
{
	log() << warning << "glGetCompressedTexImage not available." << std::endl;
}

static void fallbackGetCompressedTexImageARB(api::GLenum target, api::GLint level, api::GLvoid* img)
{
	log() << warning << "glGetCompressedTexImageARB not available." << std::endl;
}

static void fallbackGetCompressedTextureImageEXT(api::GLuint texture, api::GLenum target, api::GLint lod, api::GLvoid* img)
{
	log() << warning << "glGetCompressedTextureImageEXT not available." << std::endl;
}

static void fallbackGetConvolutionFilter(api::GLenum target, api::GLenum format, api::GLenum type, api::GLvoid* image)
{
	log() << warning << "glGetConvolutionFilter not available." << std::endl;
}

static void fallbackGetConvolutionFilterEXT(api::GLenum target, api::GLenum format, api::GLenum type, api::GLvoid* image)
{
	log() << warning << "glGetConvolutionFilterEXT not available." << std::endl;
}

static void fallbackGetConvolutionParameterfv(api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glGetConvolutionParameterfv not available." << std::endl;
}

static void fallbackGetConvolutionParameterfvEXT(api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glGetConvolutionParameterfvEXT not available." << std::endl;
}

static void fallbackGetConvolutionParameteriv(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetConvolutionParameteriv not available." << std::endl;
}

static void fallbackGetConvolutionParameterivEXT(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetConvolutionParameterivEXT not available." << std::endl;
}

static void fallbackGetDetailTexFuncSGIS(api::GLenum target, api::GLfloat* points)
{
	log() << warning << "glGetDetailTexFuncSGIS not available." << std::endl;
}

static void fallbackGetDoubleIndexedvEXT(api::GLenum target, api::GLuint index, api::GLdouble* data)
{
	log() << warning << "glGetDoubleIndexedvEXT not available." << std::endl;
}

static void fallbackGetDoublev(api::GLenum pname, api::GLdouble* params)
{
	log() << warning << "glGetDoublev not available." << std::endl;
}

static api::GLenum fallbackGetError()
{
	log() << warning << "glGetError not available." << std::endl;
}

static void fallbackGetFenceivNV(api::GLuint fence, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetFenceivNV not available." << std::endl;
}

static void fallbackGetFinalCombinerInputParameterfvNV(api::GLenum variable, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glGetFinalCombinerInputParameterfvNV not available." << std::endl;
}

static void fallbackGetFinalCombinerInputParameterivNV(api::GLenum variable, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetFinalCombinerInputParameterivNV not available." << std::endl;
}

static void fallbackGetFloatIndexedvEXT(api::GLenum target, api::GLuint index, api::GLfloat* data)
{
	log() << warning << "glGetFloatIndexedvEXT not available." << std::endl;
}

static void fallbackGetFloatv(api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glGetFloatv not available." << std::endl;
}

static void fallbackGetFogFuncSGIS(api::GLfloat* points)
{
	log() << warning << "glGetFogFuncSGIS not available." << std::endl;
}

static api::GLint fallbackGetFragDataIndex(api::GLuint program, const api::GLchar* name)
{
	log() << warning << "glGetFragDataIndex not available." << std::endl;
}

static api::GLint fallbackGetFragDataLocation(api::GLuint program, const api::GLchar* name)
{
	log() << warning << "glGetFragDataLocation not available." << std::endl;
}

static api::GLint fallbackGetFragDataLocationEXT(api::GLuint program, const api::GLchar* name)
{
	log() << warning << "glGetFragDataLocationEXT not available." << std::endl;
}

static void fallbackGetFragmentLightfvSGIX(api::GLenum light, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glGetFragmentLightfvSGIX not available." << std::endl;
}

static void fallbackGetFragmentLightivSGIX(api::GLenum light, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetFragmentLightivSGIX not available." << std::endl;
}

static void fallbackGetFragmentMaterialfvSGIX(api::GLenum face, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glGetFragmentMaterialfvSGIX not available." << std::endl;
}

static void fallbackGetFragmentMaterialivSGIX(api::GLenum face, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetFragmentMaterialivSGIX not available." << std::endl;
}

static void fallbackGetFramebufferAttachmentParameteriv(api::GLenum target, api::GLenum attachment, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetFramebufferAttachmentParameteriv not available." << std::endl;
}

static void fallbackGetFramebufferAttachmentParameterivEXT(api::GLenum target, api::GLenum attachment, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetFramebufferAttachmentParameterivEXT not available." << std::endl;
}

static void fallbackGetFramebufferParameterivEXT(api::GLuint framebuffer, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetFramebufferParameterivEXT not available." << std::endl;
}

static api::GLhandleARB fallbackGetHandleARB(api::GLenum pname)
{
	log() << warning << "glGetHandleARB not available." << std::endl;
}

static void fallbackGetHistogram(api::GLenum target, api::GLboolean reset, api::GLenum format, api::GLenum type, api::GLvoid* values)
{
	log() << warning << "glGetHistogram not available." << std::endl;
}

static void fallbackGetHistogramEXT(api::GLenum target, api::GLboolean reset, api::GLenum format, api::GLenum type, api::GLvoid* values)
{
	log() << warning << "glGetHistogramEXT not available." << std::endl;
}

static void fallbackGetHistogramParameterfv(api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glGetHistogramParameterfv not available." << std::endl;
}

static void fallbackGetHistogramParameterfvEXT(api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glGetHistogramParameterfvEXT not available." << std::endl;
}

static void fallbackGetHistogramParameteriv(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetHistogramParameteriv not available." << std::endl;
}

static void fallbackGetHistogramParameterivEXT(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetHistogramParameterivEXT not available." << std::endl;
}

static void fallbackGetImageTransformParameterfvHP(api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glGetImageTransformParameterfvHP not available." << std::endl;
}

static void fallbackGetImageTransformParameterivHP(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetImageTransformParameterivHP not available." << std::endl;
}

static void fallbackGetInfoLogARB(api::GLhandleARB obj, api::GLsizei maxLength, api::GLsizei* length, api::GLcharARB* infoLog)
{
	log() << warning << "glGetInfoLogARB not available." << std::endl;
}

static api::GLint fallbackGetInstrumentsSGIX()
{
	log() << warning << "glGetInstrumentsSGIX not available." << std::endl;
}

static void fallbackGetInteger64i_v(api::GLenum target, api::GLuint index, api::GLint64* data)
{
	log() << warning << "glGetInteger64i_v not available." << std::endl;
}

static void fallbackGetInteger64v(api::GLenum pname, api::GLint64* params)
{
	log() << warning << "glGetInteger64v not available." << std::endl;
}

static void fallbackGetIntegerIndexedvEXT(api::GLenum target, api::GLuint index, api::GLint* data)
{
	log() << warning << "glGetIntegerIndexedvEXT not available." << std::endl;
}

static void fallbackGetIntegeri_v(api::GLenum target, api::GLuint index, api::GLint* data)
{
	log() << warning << "glGetIntegeri_v not available." << std::endl;
}

static void fallbackGetIntegerui64i_vNV(api::GLenum value, api::GLuint index, api::GLuint64EXT* result)
{
	log() << warning << "glGetIntegerui64i_vNV not available." << std::endl;
}

static void fallbackGetIntegerui64vNV(api::GLenum value, api::GLuint64EXT* result)
{
	log() << warning << "glGetIntegerui64vNV not available." << std::endl;
}

static void fallbackGetIntegerv(api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetIntegerv not available." << std::endl;
}

static void fallbackGetInvariantBooleanvEXT(api::GLuint id, api::GLenum value, api::GLboolean* data)
{
	log() << warning << "glGetInvariantBooleanvEXT not available." << std::endl;
}

static void fallbackGetInvariantFloatvEXT(api::GLuint id, api::GLenum value, api::GLfloat* data)
{
	log() << warning << "glGetInvariantFloatvEXT not available." << std::endl;
}

static void fallbackGetInvariantIntegervEXT(api::GLuint id, api::GLenum value, api::GLint* data)
{
	log() << warning << "glGetInvariantIntegervEXT not available." << std::endl;
}

static void fallbackGetLightfv(api::GLenum light, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glGetLightfv not available." << std::endl;
}

static void fallbackGetLightiv(api::GLenum light, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetLightiv not available." << std::endl;
}

static void fallbackGetListParameterfvSGIX(api::GLuint list, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glGetListParameterfvSGIX not available." << std::endl;
}

static void fallbackGetListParameterivSGIX(api::GLuint list, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetListParameterivSGIX not available." << std::endl;
}

static void fallbackGetLocalConstantBooleanvEXT(api::GLuint id, api::GLenum value, api::GLboolean* data)
{
	log() << warning << "glGetLocalConstantBooleanvEXT not available." << std::endl;
}

static void fallbackGetLocalConstantFloatvEXT(api::GLuint id, api::GLenum value, api::GLfloat* data)
{
	log() << warning << "glGetLocalConstantFloatvEXT not available." << std::endl;
}

static void fallbackGetLocalConstantIntegervEXT(api::GLuint id, api::GLenum value, api::GLint* data)
{
	log() << warning << "glGetLocalConstantIntegervEXT not available." << std::endl;
}

static void fallbackGetMapAttribParameterfvNV(api::GLenum target, api::GLuint index, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glGetMapAttribParameterfvNV not available." << std::endl;
}

static void fallbackGetMapAttribParameterivNV(api::GLenum target, api::GLuint index, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetMapAttribParameterivNV not available." << std::endl;
}

static void fallbackGetMapControlPointsNV(api::GLenum target, api::GLuint index, api::GLenum type, api::GLsizei ustride, api::GLsizei vstride, api::GLboolean packed, api::GLvoid* points)
{
	log() << warning << "glGetMapControlPointsNV not available." << std::endl;
}

static void fallbackGetMapParameterfvNV(api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glGetMapParameterfvNV not available." << std::endl;
}

static void fallbackGetMapParameterivNV(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetMapParameterivNV not available." << std::endl;
}

static void fallbackGetMapdv(api::GLenum target, api::GLenum query, api::GLdouble* v)
{
	log() << warning << "glGetMapdv not available." << std::endl;
}

static void fallbackGetMapfv(api::GLenum target, api::GLenum query, api::GLfloat* v)
{
	log() << warning << "glGetMapfv not available." << std::endl;
}

static void fallbackGetMapiv(api::GLenum target, api::GLenum query, api::GLint* v)
{
	log() << warning << "glGetMapiv not available." << std::endl;
}

static void fallbackGetMaterialfv(api::GLenum face, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glGetMaterialfv not available." << std::endl;
}

static void fallbackGetMaterialiv(api::GLenum face, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetMaterialiv not available." << std::endl;
}

static void fallbackGetMinmax(api::GLenum target, api::GLboolean reset, api::GLenum format, api::GLenum type, api::GLvoid* values)
{
	log() << warning << "glGetMinmax not available." << std::endl;
}

static void fallbackGetMinmaxEXT(api::GLenum target, api::GLboolean reset, api::GLenum format, api::GLenum type, api::GLvoid* values)
{
	log() << warning << "glGetMinmaxEXT not available." << std::endl;
}

static void fallbackGetMinmaxParameterfv(api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glGetMinmaxParameterfv not available." << std::endl;
}

static void fallbackGetMinmaxParameterfvEXT(api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glGetMinmaxParameterfvEXT not available." << std::endl;
}

static void fallbackGetMinmaxParameteriv(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetMinmaxParameteriv not available." << std::endl;
}

static void fallbackGetMinmaxParameterivEXT(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetMinmaxParameterivEXT not available." << std::endl;
}

static void fallbackGetMultiTexEnvfvEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glGetMultiTexEnvfvEXT not available." << std::endl;
}

static void fallbackGetMultiTexEnvivEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetMultiTexEnvivEXT not available." << std::endl;
}

static void fallbackGetMultiTexGendvEXT(api::GLenum texunit, api::GLenum coord, api::GLenum pname, api::GLdouble* params)
{
	log() << warning << "glGetMultiTexGendvEXT not available." << std::endl;
}

static void fallbackGetMultiTexGenfvEXT(api::GLenum texunit, api::GLenum coord, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glGetMultiTexGenfvEXT not available." << std::endl;
}

static void fallbackGetMultiTexGenivEXT(api::GLenum texunit, api::GLenum coord, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetMultiTexGenivEXT not available." << std::endl;
}

static void fallbackGetMultiTexImageEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLenum format, api::GLenum type, api::GLvoid* pixels)
{
	log() << warning << "glGetMultiTexImageEXT not available." << std::endl;
}

static void fallbackGetMultiTexLevelParameterfvEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glGetMultiTexLevelParameterfvEXT not available." << std::endl;
}

static void fallbackGetMultiTexLevelParameterivEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetMultiTexLevelParameterivEXT not available." << std::endl;
}

static void fallbackGetMultiTexParameterIivEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetMultiTexParameterIivEXT not available." << std::endl;
}

static void fallbackGetMultiTexParameterIuivEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, api::GLuint* params)
{
	log() << warning << "glGetMultiTexParameterIuivEXT not available." << std::endl;
}

static void fallbackGetMultiTexParameterfvEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glGetMultiTexParameterfvEXT not available." << std::endl;
}

static void fallbackGetMultiTexParameterivEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetMultiTexParameterivEXT not available." << std::endl;
}

static void fallbackGetMultisamplefv(api::GLenum pname, api::GLuint index, api::GLfloat* val)
{
	log() << warning << "glGetMultisamplefv not available." << std::endl;
}

static void fallbackGetMultisamplefvNV(api::GLenum pname, api::GLuint index, api::GLfloat* val)
{
	log() << warning << "glGetMultisamplefvNV not available." << std::endl;
}

static void fallbackGetNamedBufferParameterivEXT(api::GLuint buffer, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetNamedBufferParameterivEXT not available." << std::endl;
}

static void fallbackGetNamedBufferParameterui64vNV(api::GLuint buffer, api::GLenum pname, api::GLuint64EXT* params)
{
	log() << warning << "glGetNamedBufferParameterui64vNV not available." << std::endl;
}

static void fallbackGetNamedBufferPointervEXT(api::GLuint buffer, api::GLenum pname, api::GLvoid** params)
{
	log() << warning << "glGetNamedBufferPointervEXT not available." << std::endl;
}

static void fallbackGetNamedBufferSubDataEXT(api::GLuint buffer, api::GLintptr offset, api::GLsizeiptr size, api::GLvoid* data)
{
	log() << warning << "glGetNamedBufferSubDataEXT not available." << std::endl;
}

static void fallbackGetNamedFramebufferAttachmentParameterivEXT(api::GLuint framebuffer, api::GLenum attachment, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetNamedFramebufferAttachmentParameterivEXT not available." << std::endl;
}

static void fallbackGetNamedProgramLocalParameterIivEXT(api::GLuint program, api::GLenum target, api::GLuint index, api::GLint* params)
{
	log() << warning << "glGetNamedProgramLocalParameterIivEXT not available." << std::endl;
}

static void fallbackGetNamedProgramLocalParameterIuivEXT(api::GLuint program, api::GLenum target, api::GLuint index, api::GLuint* params)
{
	log() << warning << "glGetNamedProgramLocalParameterIuivEXT not available." << std::endl;
}

static void fallbackGetNamedProgramLocalParameterdvEXT(api::GLuint program, api::GLenum target, api::GLuint index, api::GLdouble* params)
{
	log() << warning << "glGetNamedProgramLocalParameterdvEXT not available." << std::endl;
}

static void fallbackGetNamedProgramLocalParameterfvEXT(api::GLuint program, api::GLenum target, api::GLuint index, api::GLfloat* params)
{
	log() << warning << "glGetNamedProgramLocalParameterfvEXT not available." << std::endl;
}

static void fallbackGetNamedProgramStringEXT(api::GLuint program, api::GLenum target, api::GLenum pname, api::GLvoid* string)
{
	log() << warning << "glGetNamedProgramStringEXT not available." << std::endl;
}

static void fallbackGetNamedProgramivEXT(api::GLuint program, api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetNamedProgramivEXT not available." << std::endl;
}

static void fallbackGetNamedRenderbufferParameterivEXT(api::GLuint renderbuffer, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetNamedRenderbufferParameterivEXT not available." << std::endl;
}

static void fallbackGetNamedStringARB(api::GLint namelen, const api::GLchar* name, api::GLsizei bufSize, api::GLint* stringlen, api::GLchar* string)
{
	log() << warning << "glGetNamedStringARB not available." << std::endl;
}

static void fallbackGetNamedStringivARB(api::GLint namelen, const api::GLchar* name, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetNamedStringivARB not available." << std::endl;
}

static void fallbackGetObjectBufferfvATI(api::GLuint buffer, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glGetObjectBufferfvATI not available." << std::endl;
}

static void fallbackGetObjectBufferivATI(api::GLuint buffer, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetObjectBufferivATI not available." << std::endl;
}

static void fallbackGetObjectParameterfvARB(api::GLhandleARB obj, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glGetObjectParameterfvARB not available." << std::endl;
}

static void fallbackGetObjectParameterivAPPLE(api::GLenum objectType, api::GLuint name, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetObjectParameterivAPPLE not available." << std::endl;
}

static void fallbackGetObjectParameterivARB(api::GLhandleARB obj, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetObjectParameterivARB not available." << std::endl;
}

static void fallbackGetOcclusionQueryivNV(api::GLuint id, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetOcclusionQueryivNV not available." << std::endl;
}

static void fallbackGetOcclusionQueryuivNV(api::GLuint id, api::GLenum pname, api::GLuint* params)
{
	log() << warning << "glGetOcclusionQueryuivNV not available." << std::endl;
}

static void fallbackGetPerfMonitorCounterDataAMD(api::GLuint monitor, api::GLenum pname, api::GLsizei dataSize, api::GLuint* data, api::GLint* bytesWritten)
{
	log() << warning << "glGetPerfMonitorCounterDataAMD not available." << std::endl;
}

static void fallbackGetPerfMonitorCounterInfoAMD(api::GLuint group, api::GLuint counter, api::GLenum pname, void* data)
{
	log() << warning << "glGetPerfMonitorCounterInfoAMD not available." << std::endl;
}

static void fallbackGetPerfMonitorCounterStringAMD(api::GLuint group, api::GLuint counter, api::GLsizei bufSize, api::GLsizei* length, api::GLchar* counterString)
{
	log() << warning << "glGetPerfMonitorCounterStringAMD not available." << std::endl;
}

static void fallbackGetPerfMonitorCountersAMD(api::GLuint group, api::GLint* numCounters, api::GLint* maxActiveCounters, api::GLsizei counterSize, api::GLuint* counters)
{
	log() << warning << "glGetPerfMonitorCountersAMD not available." << std::endl;
}

static void fallbackGetPerfMonitorGroupStringAMD(api::GLuint group, api::GLsizei bufSize, api::GLsizei* length, api::GLchar* groupString)
{
	log() << warning << "glGetPerfMonitorGroupStringAMD not available." << std::endl;
}

static void fallbackGetPerfMonitorGroupsAMD(api::GLint* numGroups, api::GLsizei groupsSize, api::GLuint* groups)
{
	log() << warning << "glGetPerfMonitorGroupsAMD not available." << std::endl;
}

static void fallbackGetPixelMapfv(api::GLenum map, api::GLfloat* values)
{
	log() << warning << "glGetPixelMapfv not available." << std::endl;
}

static void fallbackGetPixelMapuiv(api::GLenum map, api::GLuint* values)
{
	log() << warning << "glGetPixelMapuiv not available." << std::endl;
}

static void fallbackGetPixelMapusv(api::GLenum map, api::GLushort* values)
{
	log() << warning << "glGetPixelMapusv not available." << std::endl;
}

static void fallbackGetPixelTexGenParameterfvSGIS(api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glGetPixelTexGenParameterfvSGIS not available." << std::endl;
}

static void fallbackGetPixelTexGenParameterivSGIS(api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetPixelTexGenParameterivSGIS not available." << std::endl;
}

static void fallbackGetPointerIndexedvEXT(api::GLenum target, api::GLuint index, api::GLvoid** data)
{
	log() << warning << "glGetPointerIndexedvEXT not available." << std::endl;
}

static void fallbackGetPointerv(api::GLenum pname, api::GLvoid** params)
{
	log() << warning << "glGetPointerv not available." << std::endl;
}

static void fallbackGetPointervEXT(api::GLenum pname, api::GLvoid** params)
{
	log() << warning << "glGetPointervEXT not available." << std::endl;
}

static void fallbackGetPolygonStipple(api::GLubyte* mask)
{
	log() << warning << "glGetPolygonStipple not available." << std::endl;
}

static void fallbackGetProgramEnvParameterIivNV(api::GLenum target, api::GLuint index, api::GLint* params)
{
	log() << warning << "glGetProgramEnvParameterIivNV not available." << std::endl;
}

static void fallbackGetProgramEnvParameterIuivNV(api::GLenum target, api::GLuint index, api::GLuint* params)
{
	log() << warning << "glGetProgramEnvParameterIuivNV not available." << std::endl;
}

static void fallbackGetProgramEnvParameterdvARB(api::GLenum target, api::GLuint index, api::GLdouble* params)
{
	log() << warning << "glGetProgramEnvParameterdvARB not available." << std::endl;
}

static void fallbackGetProgramEnvParameterfvARB(api::GLenum target, api::GLuint index, api::GLfloat* params)
{
	log() << warning << "glGetProgramEnvParameterfvARB not available." << std::endl;
}

static void fallbackGetProgramInfoLog(api::GLuint program, api::GLsizei bufSize, api::GLsizei* length, api::GLchar* infoLog)
{
	log() << warning << "glGetProgramInfoLog not available." << std::endl;
}

static void fallbackGetProgramLocalParameterIivNV(api::GLenum target, api::GLuint index, api::GLint* params)
{
	log() << warning << "glGetProgramLocalParameterIivNV not available." << std::endl;
}

static void fallbackGetProgramLocalParameterIuivNV(api::GLenum target, api::GLuint index, api::GLuint* params)
{
	log() << warning << "glGetProgramLocalParameterIuivNV not available." << std::endl;
}

static void fallbackGetProgramLocalParameterdvARB(api::GLenum target, api::GLuint index, api::GLdouble* params)
{
	log() << warning << "glGetProgramLocalParameterdvARB not available." << std::endl;
}

static void fallbackGetProgramLocalParameterfvARB(api::GLenum target, api::GLuint index, api::GLfloat* params)
{
	log() << warning << "glGetProgramLocalParameterfvARB not available." << std::endl;
}

static void fallbackGetProgramNamedParameterdvNV(api::GLuint id, api::GLsizei len, const api::GLubyte* name, api::GLdouble* params)
{
	log() << warning << "glGetProgramNamedParameterdvNV not available." << std::endl;
}

static void fallbackGetProgramNamedParameterfvNV(api::GLuint id, api::GLsizei len, const api::GLubyte* name, api::GLfloat* params)
{
	log() << warning << "glGetProgramNamedParameterfvNV not available." << std::endl;
}

static void fallbackGetProgramParameterdvNV(api::GLenum target, api::GLuint index, api::GLenum pname, api::GLdouble* params)
{
	log() << warning << "glGetProgramParameterdvNV not available." << std::endl;
}

static void fallbackGetProgramParameterfvNV(api::GLenum target, api::GLuint index, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glGetProgramParameterfvNV not available." << std::endl;
}

static void fallbackGetProgramStageiv(api::GLuint program, api::GLenum shadertype, api::GLenum pname, api::GLint* values)
{
	log() << warning << "glGetProgramStageiv not available." << std::endl;
}

static void fallbackGetProgramStringARB(api::GLenum target, api::GLenum pname, api::GLvoid* string)
{
	log() << warning << "glGetProgramStringARB not available." << std::endl;
}

static void fallbackGetProgramStringNV(api::GLuint id, api::GLenum pname, api::GLubyte* program)
{
	log() << warning << "glGetProgramStringNV not available." << std::endl;
}

static void fallbackGetProgramiv(api::GLuint program, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetProgramiv not available." << std::endl;
}

static void fallbackGetProgramivARB(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetProgramivARB not available." << std::endl;
}

static void fallbackGetProgramivNV(api::GLuint id, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetProgramivNV not available." << std::endl;
}

static void fallbackGetQueryIndexediv(api::GLenum target, api::GLuint index, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetQueryIndexediv not available." << std::endl;
}

static void fallbackGetQueryObjecti64v(api::GLuint id, api::GLenum pname, api::GLint64* params)
{
	log() << warning << "glGetQueryObjecti64v not available." << std::endl;
}

static void fallbackGetQueryObjecti64vEXT(api::GLuint id, api::GLenum pname, api::GLint64EXT* params)
{
	log() << warning << "glGetQueryObjecti64vEXT not available." << std::endl;
}

static void fallbackGetQueryObjectiv(api::GLuint id, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetQueryObjectiv not available." << std::endl;
}

static void fallbackGetQueryObjectivARB(api::GLuint id, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetQueryObjectivARB not available." << std::endl;
}

static void fallbackGetQueryObjectui64v(api::GLuint id, api::GLenum pname, api::GLuint64* params)
{
	log() << warning << "glGetQueryObjectui64v not available." << std::endl;
}

static void fallbackGetQueryObjectui64vEXT(api::GLuint id, api::GLenum pname, api::GLuint64EXT* params)
{
	log() << warning << "glGetQueryObjectui64vEXT not available." << std::endl;
}

static void fallbackGetQueryObjectuiv(api::GLuint id, api::GLenum pname, api::GLuint* params)
{
	log() << warning << "glGetQueryObjectuiv not available." << std::endl;
}

static void fallbackGetQueryObjectuivARB(api::GLuint id, api::GLenum pname, api::GLuint* params)
{
	log() << warning << "glGetQueryObjectuivARB not available." << std::endl;
}

static void fallbackGetQueryiv(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetQueryiv not available." << std::endl;
}

static void fallbackGetQueryivARB(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetQueryivARB not available." << std::endl;
}

static void fallbackGetRenderbufferParameteriv(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetRenderbufferParameteriv not available." << std::endl;
}

static void fallbackGetRenderbufferParameterivEXT(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetRenderbufferParameterivEXT not available." << std::endl;
}

static void fallbackGetSamplerParameterIfv(api::GLuint sampler, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glGetSamplerParameterIfv not available." << std::endl;
}

static void fallbackGetSamplerParameterIiv(api::GLuint sampler, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetSamplerParameterIiv not available." << std::endl;
}

static void fallbackGetSamplerParameterfv(api::GLuint sampler, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glGetSamplerParameterfv not available." << std::endl;
}

static void fallbackGetSamplerParameteriv(api::GLuint sampler, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetSamplerParameteriv not available." << std::endl;
}

static void fallbackGetSeparableFilter(api::GLenum target, api::GLenum format, api::GLenum type, api::GLvoid* row, api::GLvoid* column, api::GLvoid* span)
{
	log() << warning << "glGetSeparableFilter not available." << std::endl;
}

static void fallbackGetSeparableFilterEXT(api::GLenum target, api::GLenum format, api::GLenum type, api::GLvoid* row, api::GLvoid* column, api::GLvoid* span)
{
	log() << warning << "glGetSeparableFilterEXT not available." << std::endl;
}

static void fallbackGetShaderInfoLog(api::GLuint shader, api::GLsizei bufSize, api::GLsizei* length, api::GLchar* infoLog)
{
	log() << warning << "glGetShaderInfoLog not available." << std::endl;
}

static void fallbackGetShaderSource(api::GLuint shader, api::GLsizei bufSize, api::GLsizei* length, api::GLchar* source)
{
	log() << warning << "glGetShaderSource not available." << std::endl;
}

static void fallbackGetShaderSourceARB(api::GLhandleARB obj, api::GLsizei maxLength, api::GLsizei* length, api::GLcharARB* source)
{
	log() << warning << "glGetShaderSourceARB not available." << std::endl;
}

static void fallbackGetShaderiv(api::GLuint shader, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetShaderiv not available." << std::endl;
}

static void fallbackGetSharpenTexFuncSGIS(api::GLenum target, api::GLfloat* points)
{
	log() << warning << "glGetSharpenTexFuncSGIS not available." << std::endl;
}

static const api::GLubyte * fallbackGetString(api::GLenum name)
{
	log() << warning << "glGetString not available." << std::endl;
}

static const api::GLubyte * fallbackGetStringi(api::GLenum name, api::GLuint index)
{
	log() << warning << "glGetStringi not available." << std::endl;
}

static api::GLuint fallbackGetSubroutineIndex(api::GLuint program, api::GLenum shadertype, const api::GLchar* name)
{
	log() << warning << "glGetSubroutineIndex not available." << std::endl;
}

static api::GLint fallbackGetSubroutineUniformLocation(api::GLuint program, api::GLenum shadertype, const api::GLchar* name)
{
	log() << warning << "glGetSubroutineUniformLocation not available." << std::endl;
}

static void fallbackGetSynciv(api::GLsync sync, api::GLenum pname, api::GLsizei bufSize, api::GLsizei* length, api::GLint* values)
{
	log() << warning << "glGetSynciv not available." << std::endl;
}

static void fallbackGetTexBumpParameterfvATI(api::GLenum pname, api::GLfloat* param)
{
	log() << warning << "glGetTexBumpParameterfvATI not available." << std::endl;
}

static void fallbackGetTexBumpParameterivATI(api::GLenum pname, api::GLint* param)
{
	log() << warning << "glGetTexBumpParameterivATI not available." << std::endl;
}

static void fallbackGetTexEnvfv(api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glGetTexEnvfv not available." << std::endl;
}

static void fallbackGetTexEnviv(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetTexEnviv not available." << std::endl;
}

static void fallbackGetTexFilterFuncSGIS(api::GLenum target, api::GLenum filter, api::GLfloat* weights)
{
	log() << warning << "glGetTexFilterFuncSGIS not available." << std::endl;
}

static void fallbackGetTexGendv(api::GLenum coord, api::GLenum pname, api::GLdouble* params)
{
	log() << warning << "glGetTexGendv not available." << std::endl;
}

static void fallbackGetTexGenfv(api::GLenum coord, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glGetTexGenfv not available." << std::endl;
}

static void fallbackGetTexGeniv(api::GLenum coord, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetTexGeniv not available." << std::endl;
}

static void fallbackGetTexImage(api::GLenum target, api::GLint level, api::GLenum format, api::GLenum type, api::GLvoid* pixels)
{
	log() << warning << "glGetTexImage not available." << std::endl;
}

static void fallbackGetTexLevelParameterfv(api::GLenum target, api::GLint level, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glGetTexLevelParameterfv not available." << std::endl;
}

static void fallbackGetTexLevelParameteriv(api::GLenum target, api::GLint level, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetTexLevelParameteriv not available." << std::endl;
}

static void fallbackGetTexParameterIiv(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetTexParameterIiv not available." << std::endl;
}

static void fallbackGetTexParameterIivEXT(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetTexParameterIivEXT not available." << std::endl;
}

static void fallbackGetTexParameterIuiv(api::GLenum target, api::GLenum pname, api::GLuint* params)
{
	log() << warning << "glGetTexParameterIuiv not available." << std::endl;
}

static void fallbackGetTexParameterIuivEXT(api::GLenum target, api::GLenum pname, api::GLuint* params)
{
	log() << warning << "glGetTexParameterIuivEXT not available." << std::endl;
}

static void fallbackGetTexParameterPointervAPPLE(api::GLenum target, api::GLenum pname, api::GLvoid** params)
{
	log() << warning << "glGetTexParameterPointervAPPLE not available." << std::endl;
}

static void fallbackGetTexParameterfv(api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glGetTexParameterfv not available." << std::endl;
}

static void fallbackGetTexParameteriv(api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetTexParameteriv not available." << std::endl;
}

static void fallbackGetTextureImageEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLenum format, api::GLenum type, api::GLvoid* pixels)
{
	log() << warning << "glGetTextureImageEXT not available." << std::endl;
}

static void fallbackGetTextureLevelParameterfvEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glGetTextureLevelParameterfvEXT not available." << std::endl;
}

static void fallbackGetTextureLevelParameterivEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetTextureLevelParameterivEXT not available." << std::endl;
}

static void fallbackGetTextureParameterIivEXT(api::GLuint texture, api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetTextureParameterIivEXT not available." << std::endl;
}

static void fallbackGetTextureParameterIuivEXT(api::GLuint texture, api::GLenum target, api::GLenum pname, api::GLuint* params)
{
	log() << warning << "glGetTextureParameterIuivEXT not available." << std::endl;
}

static void fallbackGetTextureParameterfvEXT(api::GLuint texture, api::GLenum target, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glGetTextureParameterfvEXT not available." << std::endl;
}

static void fallbackGetTextureParameterivEXT(api::GLuint texture, api::GLenum target, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetTextureParameterivEXT not available." << std::endl;
}

static void fallbackGetTrackMatrixivNV(api::GLenum target, api::GLuint address, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetTrackMatrixivNV not available." << std::endl;
}

static void fallbackGetTransformFeedbackVarying(api::GLuint program, api::GLuint index, api::GLsizei bufSize, api::GLsizei* length, api::GLsizei* size, api::GLenum* type, api::GLchar* name)
{
	log() << warning << "glGetTransformFeedbackVarying not available." << std::endl;
}

static void fallbackGetTransformFeedbackVaryingEXT(api::GLuint program, api::GLuint index, api::GLsizei bufSize, api::GLsizei* length, api::GLsizei* size, api::GLenum* type, api::GLchar* name)
{
	log() << warning << "glGetTransformFeedbackVaryingEXT not available." << std::endl;
}

static void fallbackGetTransformFeedbackVaryingNV(api::GLuint program, api::GLuint index, api::GLint* location)
{
	log() << warning << "glGetTransformFeedbackVaryingNV not available." << std::endl;
}

static api::GLuint fallbackGetUniformBlockIndex(api::GLuint program, const api::GLchar* uniformBlockName)
{
	log() << warning << "glGetUniformBlockIndex not available." << std::endl;
}

static api::GLint fallbackGetUniformBufferSizeEXT(api::GLuint program, api::GLint location)
{
	log() << warning << "glGetUniformBufferSizeEXT not available." << std::endl;
}

static void fallbackGetUniformIndices(api::GLuint program, api::GLsizei uniformCount, const api::GLchar** uniformNames, api::GLuint* uniformIndices)
{
	log() << warning << "glGetUniformIndices not available." << std::endl;
}

static api::GLint fallbackGetUniformLocation(api::GLuint program, const api::GLchar* name)
{
	log() << warning << "glGetUniformLocation not available." << std::endl;
}

static api::GLint fallbackGetUniformLocationARB(api::GLhandleARB programObj, const api::GLcharARB* name)
{
	log() << warning << "glGetUniformLocationARB not available." << std::endl;
}

static api::GLintptr fallbackGetUniformOffsetEXT(api::GLuint program, api::GLint location)
{
	log() << warning << "glGetUniformOffsetEXT not available." << std::endl;
}

static void fallbackGetUniformSubroutineuiv(api::GLenum shadertype, api::GLint location, api::GLuint* params)
{
	log() << warning << "glGetUniformSubroutineuiv not available." << std::endl;
}

static void fallbackGetUniformdv(api::GLuint program, api::GLint location, api::GLdouble* params)
{
	log() << warning << "glGetUniformdv not available." << std::endl;
}

static void fallbackGetUniformfv(api::GLuint program, api::GLint location, api::GLfloat* params)
{
	log() << warning << "glGetUniformfv not available." << std::endl;
}

static void fallbackGetUniformfvARB(api::GLhandleARB programObj, api::GLint location, api::GLfloat* params)
{
	log() << warning << "glGetUniformfvARB not available." << std::endl;
}

static void fallbackGetUniformiv(api::GLuint program, api::GLint location, api::GLint* params)
{
	log() << warning << "glGetUniformiv not available." << std::endl;
}

static void fallbackGetUniformivARB(api::GLhandleARB programObj, api::GLint location, api::GLint* params)
{
	log() << warning << "glGetUniformivARB not available." << std::endl;
}

static void fallbackGetUniformui64vNV(api::GLuint program, api::GLint location, api::GLuint64EXT* params)
{
	log() << warning << "glGetUniformui64vNV not available." << std::endl;
}

static void fallbackGetUniformuiv(api::GLuint program, api::GLint location, api::GLuint* params)
{
	log() << warning << "glGetUniformuiv not available." << std::endl;
}

static void fallbackGetUniformuivEXT(api::GLuint program, api::GLint location, api::GLuint* params)
{
	log() << warning << "glGetUniformuivEXT not available." << std::endl;
}

static void fallbackGetVariantArrayObjectfvATI(api::GLuint id, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glGetVariantArrayObjectfvATI not available." << std::endl;
}

static void fallbackGetVariantArrayObjectivATI(api::GLuint id, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetVariantArrayObjectivATI not available." << std::endl;
}

static void fallbackGetVariantBooleanvEXT(api::GLuint id, api::GLenum value, api::GLboolean* data)
{
	log() << warning << "glGetVariantBooleanvEXT not available." << std::endl;
}

static void fallbackGetVariantFloatvEXT(api::GLuint id, api::GLenum value, api::GLfloat* data)
{
	log() << warning << "glGetVariantFloatvEXT not available." << std::endl;
}

static void fallbackGetVariantIntegervEXT(api::GLuint id, api::GLenum value, api::GLint* data)
{
	log() << warning << "glGetVariantIntegervEXT not available." << std::endl;
}

static void fallbackGetVariantPointervEXT(api::GLuint id, api::GLenum value, api::GLvoid** data)
{
	log() << warning << "glGetVariantPointervEXT not available." << std::endl;
}

static api::GLint fallbackGetVaryingLocationNV(api::GLuint program, const api::GLchar* name)
{
	log() << warning << "glGetVaryingLocationNV not available." << std::endl;
}

static void fallbackGetVertexAttribArrayObjectfvATI(api::GLuint index, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glGetVertexAttribArrayObjectfvATI not available." << std::endl;
}

static void fallbackGetVertexAttribArrayObjectivATI(api::GLuint index, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetVertexAttribArrayObjectivATI not available." << std::endl;
}

static void fallbackGetVertexAttribIiv(api::GLuint index, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetVertexAttribIiv not available." << std::endl;
}

static void fallbackGetVertexAttribIivEXT(api::GLuint index, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetVertexAttribIivEXT not available." << std::endl;
}

static void fallbackGetVertexAttribIuiv(api::GLuint index, api::GLenum pname, api::GLuint* params)
{
	log() << warning << "glGetVertexAttribIuiv not available." << std::endl;
}

static void fallbackGetVertexAttribIuivEXT(api::GLuint index, api::GLenum pname, api::GLuint* params)
{
	log() << warning << "glGetVertexAttribIuivEXT not available." << std::endl;
}

static void fallbackGetVertexAttribPointerv(api::GLuint index, api::GLenum pname, api::GLvoid** pointer)
{
	log() << warning << "glGetVertexAttribPointerv not available." << std::endl;
}

static void fallbackGetVertexAttribPointervARB(api::GLuint index, api::GLenum pname, api::GLvoid** pointer)
{
	log() << warning << "glGetVertexAttribPointervARB not available." << std::endl;
}

static void fallbackGetVertexAttribPointervNV(api::GLuint index, api::GLenum pname, api::GLvoid** pointer)
{
	log() << warning << "glGetVertexAttribPointervNV not available." << std::endl;
}

static void fallbackGetVertexAttribdv(api::GLuint index, api::GLenum pname, api::GLdouble* params)
{
	log() << warning << "glGetVertexAttribdv not available." << std::endl;
}

static void fallbackGetVertexAttribdvARB(api::GLuint index, api::GLenum pname, api::GLdouble* params)
{
	log() << warning << "glGetVertexAttribdvARB not available." << std::endl;
}

static void fallbackGetVertexAttribdvNV(api::GLuint index, api::GLenum pname, api::GLdouble* params)
{
	log() << warning << "glGetVertexAttribdvNV not available." << std::endl;
}

static void fallbackGetVertexAttribfv(api::GLuint index, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glGetVertexAttribfv not available." << std::endl;
}

static void fallbackGetVertexAttribfvARB(api::GLuint index, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glGetVertexAttribfvARB not available." << std::endl;
}

static void fallbackGetVertexAttribfvNV(api::GLuint index, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glGetVertexAttribfvNV not available." << std::endl;
}

static void fallbackGetVertexAttribiv(api::GLuint index, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetVertexAttribiv not available." << std::endl;
}

static void fallbackGetVertexAttribivARB(api::GLuint index, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetVertexAttribivARB not available." << std::endl;
}

static void fallbackGetVertexAttribivNV(api::GLuint index, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetVertexAttribivNV not available." << std::endl;
}

static void fallbackGetVideoCaptureStreamdvNV(api::GLuint video_capture_slot, api::GLuint stream, api::GLenum pname, api::GLdouble* params)
{
	log() << warning << "glGetVideoCaptureStreamdvNV not available." << std::endl;
}

static void fallbackGetVideoCaptureStreamfvNV(api::GLuint video_capture_slot, api::GLuint stream, api::GLenum pname, api::GLfloat* params)
{
	log() << warning << "glGetVideoCaptureStreamfvNV not available." << std::endl;
}

static void fallbackGetVideoCaptureStreamivNV(api::GLuint video_capture_slot, api::GLuint stream, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetVideoCaptureStreamivNV not available." << std::endl;
}

static void fallbackGetVideoCaptureivNV(api::GLuint video_capture_slot, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetVideoCaptureivNV not available." << std::endl;
}

static void fallbackGetVideoi64vNV(api::GLuint video_slot, api::GLenum pname, api::GLint64EXT* params)
{
	log() << warning << "glGetVideoi64vNV not available." << std::endl;
}

static void fallbackGetVideoivNV(api::GLuint video_slot, api::GLenum pname, api::GLint* params)
{
	log() << warning << "glGetVideoivNV not available." << std::endl;
}

static void fallbackGetVideoui64vNV(api::GLuint video_slot, api::GLenum pname, api::GLuint64EXT* params)
{
	log() << warning << "glGetVideoui64vNV not available." << std::endl;
}

static void fallbackGetVideouivNV(api::GLuint video_slot, api::GLenum pname, api::GLuint* params)
{
	log() << warning << "glGetVideouivNV not available." << std::endl;
}

static void fallbackGlobalAlphaFactorbSUN(api::GLbyte factor)
{
	log() << warning << "glGlobalAlphaFactorbSUN not available." << std::endl;
}

static void fallbackGlobalAlphaFactordSUN(api::GLdouble factor)
{
	log() << warning << "glGlobalAlphaFactordSUN not available." << std::endl;
}

static void fallbackGlobalAlphaFactorfSUN(api::GLfloat factor)
{
	log() << warning << "glGlobalAlphaFactorfSUN not available." << std::endl;
}

static void fallbackGlobalAlphaFactoriSUN(api::GLint factor)
{
	log() << warning << "glGlobalAlphaFactoriSUN not available." << std::endl;
}

static void fallbackGlobalAlphaFactorsSUN(api::GLshort factor)
{
	log() << warning << "glGlobalAlphaFactorsSUN not available." << std::endl;
}

static void fallbackGlobalAlphaFactorubSUN(api::GLubyte factor)
{
	log() << warning << "glGlobalAlphaFactorubSUN not available." << std::endl;
}

static void fallbackGlobalAlphaFactoruiSUN(api::GLuint factor)
{
	log() << warning << "glGlobalAlphaFactoruiSUN not available." << std::endl;
}

static void fallbackGlobalAlphaFactorusSUN(api::GLushort factor)
{
	log() << warning << "glGlobalAlphaFactorusSUN not available." << std::endl;
}

static void fallbackHint(api::GLenum target, api::GLenum mode)
{
	log() << warning << "glHint not available." << std::endl;
}

static void fallbackHintPGI(api::GLenum target, api::GLint mode)
{
	log() << warning << "glHintPGI not available." << std::endl;
}

static void fallbackHistogram(api::GLenum target, api::GLsizei width, api::GLenum internalformat, api::GLboolean sink)
{
	log() << warning << "glHistogram not available." << std::endl;
}

static void fallbackHistogramEXT(api::GLenum target, api::GLsizei width, api::GLenum internalformat, api::GLboolean sink)
{
	log() << warning << "glHistogramEXT not available." << std::endl;
}

static void fallbackIglooInterfaceSGIX(api::GLenum pname, const api::GLvoid* params)
{
	log() << warning << "glIglooInterfaceSGIX not available." << std::endl;
}

static void fallbackImageTransformParameterfHP(api::GLenum target, api::GLenum pname, api::GLfloat param)
{
	log() << warning << "glImageTransformParameterfHP not available." << std::endl;
}

static void fallbackImageTransformParameterfvHP(api::GLenum target, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "glImageTransformParameterfvHP not available." << std::endl;
}

static void fallbackImageTransformParameteriHP(api::GLenum target, api::GLenum pname, api::GLint param)
{
	log() << warning << "glImageTransformParameteriHP not available." << std::endl;
}

static void fallbackImageTransformParameterivHP(api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "glImageTransformParameterivHP not available." << std::endl;
}

static void fallbackIndexFormatNV(api::GLenum type, api::GLsizei stride)
{
	log() << warning << "glIndexFormatNV not available." << std::endl;
}

static void fallbackIndexFuncEXT(api::GLenum func, api::GLclampf ref)
{
	log() << warning << "glIndexFuncEXT not available." << std::endl;
}

static void fallbackIndexMask(api::GLuint mask)
{
	log() << warning << "glIndexMask not available." << std::endl;
}

static void fallbackIndexMaterialEXT(api::GLenum face, api::GLenum mode)
{
	log() << warning << "glIndexMaterialEXT not available." << std::endl;
}

static void fallbackIndexPointer(api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "glIndexPointer not available." << std::endl;
}

static void fallbackIndexPointerEXT(api::GLenum type, api::GLsizei stride, api::GLsizei count, const api::GLvoid* pointer)
{
	log() << warning << "glIndexPointerEXT not available." << std::endl;
}

static void fallbackIndexPointerListIBM(api::GLenum type, api::GLint stride, const api::GLvoid** pointer, api::GLint ptrstride)
{
	log() << warning << "glIndexPointerListIBM not available." << std::endl;
}

static void fallbackIndexd(api::GLdouble c)
{
	log() << warning << "glIndexd not available." << std::endl;
}

static void fallbackIndexdv(const api::GLdouble* c)
{
	log() << warning << "glIndexdv not available." << std::endl;
}

static void fallbackIndexf(api::GLfloat c)
{
	log() << warning << "glIndexf not available." << std::endl;
}

static void fallbackIndexfv(const api::GLfloat* c)
{
	log() << warning << "glIndexfv not available." << std::endl;
}

static void fallbackIndexi(api::GLint c)
{
	log() << warning << "glIndexi not available." << std::endl;
}

static void fallbackIndexiv(const api::GLint* c)
{
	log() << warning << "glIndexiv not available." << std::endl;
}

static void fallbackIndexs(api::GLshort c)
{
	log() << warning << "glIndexs not available." << std::endl;
}

static void fallbackIndexsv(const api::GLshort* c)
{
	log() << warning << "glIndexsv not available." << std::endl;
}

static void fallbackIndexub(api::GLubyte c)
{
	log() << warning << "glIndexub not available." << std::endl;
}

static void fallbackIndexubv(const api::GLubyte* c)
{
	log() << warning << "glIndexubv not available." << std::endl;
}

static void fallbackInitNames()
{
	log() << warning << "glInitNames not available." << std::endl;
}

static void fallbackInsertComponentEXT(api::GLuint res, api::GLuint src, api::GLuint num)
{
	log() << warning << "glInsertComponentEXT not available." << std::endl;
}

static void fallbackInstrumentsBufferSGIX(api::GLsizei size, api::GLint* buffer)
{
	log() << warning << "glInstrumentsBufferSGIX not available." << std::endl;
}

static void fallbackInterleavedArrays(api::GLenum format, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "glInterleavedArrays not available." << std::endl;
}

static api::GLboolean fallbackIsAsyncMarkerSGIX(api::GLuint marker)
{
	log() << warning << "glIsAsyncMarkerSGIX not available." << std::endl;
}

static api::GLboolean fallbackIsBuffer(api::GLuint buffer)
{
	log() << warning << "glIsBuffer not available." << std::endl;
}

static api::GLboolean fallbackIsBufferARB(api::GLuint buffer)
{
	log() << warning << "glIsBufferARB not available." << std::endl;
}

static api::GLboolean fallbackIsBufferResidentNV(api::GLenum target)
{
	log() << warning << "glIsBufferResidentNV not available." << std::endl;
}

static api::GLboolean fallbackIsEnabled(api::GLenum cap)
{
	log() << warning << "glIsEnabled not available." << std::endl;
}

static api::GLboolean fallbackIsEnabledIndexedEXT(api::GLenum target, api::GLuint index)
{
	log() << warning << "glIsEnabledIndexedEXT not available." << std::endl;
}

static api::GLboolean fallbackIsEnabledi(api::GLenum target, api::GLuint index)
{
	log() << warning << "glIsEnabledi not available." << std::endl;
}

static api::GLboolean fallbackIsFenceAPPLE(api::GLuint fence)
{
	log() << warning << "glIsFenceAPPLE not available." << std::endl;
}

static api::GLboolean fallbackIsFenceNV(api::GLuint fence)
{
	log() << warning << "glIsFenceNV not available." << std::endl;
}

static api::GLboolean fallbackIsFramebuffer(api::GLuint framebuffer)
{
	log() << warning << "glIsFramebuffer not available." << std::endl;
}

static api::GLboolean fallbackIsFramebufferEXT(api::GLuint framebuffer)
{
	log() << warning << "glIsFramebufferEXT not available." << std::endl;
}

static api::GLboolean fallbackIsList(api::GLuint list)
{
	log() << warning << "glIsList not available." << std::endl;
}

static api::GLboolean fallbackIsNamedBufferResidentNV(api::GLuint buffer)
{
	log() << warning << "glIsNamedBufferResidentNV not available." << std::endl;
}

static api::GLboolean fallbackIsNamedStringARB(api::GLint namelen, const api::GLchar* name)
{
	log() << warning << "glIsNamedStringARB not available." << std::endl;
}

static api::GLboolean fallbackIsObjectBufferATI(api::GLuint buffer)
{
	log() << warning << "glIsObjectBufferATI not available." << std::endl;
}

static api::GLboolean fallbackIsOcclusionQueryNV(api::GLuint id)
{
	log() << warning << "glIsOcclusionQueryNV not available." << std::endl;
}

static api::GLboolean fallbackIsProgram(api::GLuint program)
{
	log() << warning << "glIsProgram not available." << std::endl;
}

static api::GLboolean fallbackIsProgramARB(api::GLuint program)
{
	log() << warning << "glIsProgramARB not available." << std::endl;
}

static api::GLboolean fallbackIsProgramNV(api::GLuint id)
{
	log() << warning << "glIsProgramNV not available." << std::endl;
}

static api::GLboolean fallbackIsQuery(api::GLuint id)
{
	log() << warning << "glIsQuery not available." << std::endl;
}

static api::GLboolean fallbackIsQueryARB(api::GLuint id)
{
	log() << warning << "glIsQueryARB not available." << std::endl;
}

static api::GLboolean fallbackIsRenderbuffer(api::GLuint renderbuffer)
{
	log() << warning << "glIsRenderbuffer not available." << std::endl;
}

static api::GLboolean fallbackIsRenderbufferEXT(api::GLuint renderbuffer)
{
	log() << warning << "glIsRenderbufferEXT not available." << std::endl;
}

static api::GLboolean fallbackIsSampler(api::GLuint sampler)
{
	log() << warning << "glIsSampler not available." << std::endl;
}

static api::GLboolean fallbackIsShader(api::GLuint shader)
{
	log() << warning << "glIsShader not available." << std::endl;
}

static api::GLboolean fallbackIsSync(api::GLsync sync)
{
	log() << warning << "glIsSync not available." << std::endl;
}

static api::GLboolean fallbackIsTexture(api::GLuint texture)
{
	log() << warning << "glIsTexture not available." << std::endl;
}

static api::GLboolean fallbackIsTextureEXT(api::GLuint texture)
{
	log() << warning << "glIsTextureEXT not available." << std::endl;
}

static api::GLboolean fallbackIsTransformFeedback(api::GLuint id)
{
	log() << warning << "glIsTransformFeedback not available." << std::endl;
}

static api::GLboolean fallbackIsTransformFeedbackNV(api::GLuint id)
{
	log() << warning << "glIsTransformFeedbackNV not available." << std::endl;
}

static api::GLboolean fallbackIsVariantEnabledEXT(api::GLuint id, api::GLenum cap)
{
	log() << warning << "glIsVariantEnabledEXT not available." << std::endl;
}

static api::GLboolean fallbackIsVertexArray(api::GLuint array)
{
	log() << warning << "glIsVertexArray not available." << std::endl;
}

static api::GLboolean fallbackIsVertexArrayAPPLE(api::GLuint array)
{
	log() << warning << "glIsVertexArrayAPPLE not available." << std::endl;
}

static api::GLboolean fallbackIsVertexAttribEnabledAPPLE(api::GLuint index, api::GLenum pname)
{
	log() << warning << "glIsVertexAttribEnabledAPPLE not available." << std::endl;
}

static void fallbackLightEnviSGIX(api::GLenum pname, api::GLint param)
{
	log() << warning << "glLightEnviSGIX not available." << std::endl;
}

static void fallbackLightModelf(api::GLenum pname, api::GLfloat param)
{
	log() << warning << "glLightModelf not available." << std::endl;
}

static void fallbackLightModelfv(api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "glLightModelfv not available." << std::endl;
}

static void fallbackLightModeli(api::GLenum pname, api::GLint param)
{
	log() << warning << "glLightModeli not available." << std::endl;
}

static void fallbackLightModeliv(api::GLenum pname, const api::GLint* params)
{
	log() << warning << "glLightModeliv not available." << std::endl;
}

static void fallbackLightf(api::GLenum light, api::GLenum pname, api::GLfloat param)
{
	log() << warning << "glLightf not available." << std::endl;
}

static void fallbackLightfv(api::GLenum light, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "glLightfv not available." << std::endl;
}

static void fallbackLighti(api::GLenum light, api::GLenum pname, api::GLint param)
{
	log() << warning << "glLighti not available." << std::endl;
}

static void fallbackLightiv(api::GLenum light, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "glLightiv not available." << std::endl;
}

static void fallbackLineStipple(api::GLint factor, api::GLushort pattern)
{
	log() << warning << "glLineStipple not available." << std::endl;
}

static void fallbackLineWidth(api::GLfloat width)
{
	log() << warning << "glLineWidth not available." << std::endl;
}

static void fallbackLinkProgram(api::GLuint program)
{
	log() << warning << "glLinkProgram not available." << std::endl;
}

static void fallbackLinkProgramARB(api::GLhandleARB programObj)
{
	log() << warning << "glLinkProgramARB not available." << std::endl;
}

static void fallbackListBase(api::GLuint base)
{
	log() << warning << "glListBase not available." << std::endl;
}

static void fallbackListParameterfSGIX(api::GLuint list, api::GLenum pname, api::GLfloat param)
{
	log() << warning << "glListParameterfSGIX not available." << std::endl;
}

static void fallbackListParameterfvSGIX(api::GLuint list, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "glListParameterfvSGIX not available." << std::endl;
}

static void fallbackListParameteriSGIX(api::GLuint list, api::GLenum pname, api::GLint param)
{
	log() << warning << "glListParameteriSGIX not available." << std::endl;
}

static void fallbackListParameterivSGIX(api::GLuint list, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "glListParameterivSGIX not available." << std::endl;
}

static void fallbackLoadIdentity()
{
	log() << warning << "glLoadIdentity not available." << std::endl;
}

static void fallbackLoadIdentityDeformationMapSGIX(api::GLbitfield mask)
{
	log() << warning << "glLoadIdentityDeformationMapSGIX not available." << std::endl;
}

static void fallbackLoadMatrixd(const api::GLdouble* m)
{
	log() << warning << "glLoadMatrixd not available." << std::endl;
}

static void fallbackLoadMatrixf(const api::GLfloat* m)
{
	log() << warning << "glLoadMatrixf not available." << std::endl;
}

static void fallbackLoadName(api::GLuint name)
{
	log() << warning << "glLoadName not available." << std::endl;
}

static void fallbackLoadProgramNV(api::GLenum target, api::GLuint id, api::GLsizei len, const api::GLubyte* program)
{
	log() << warning << "glLoadProgramNV not available." << std::endl;
}

static void fallbackLoadTransposeMatrixd(const api::GLdouble* m)
{
	log() << warning << "glLoadTransposeMatrixd not available." << std::endl;
}

static void fallbackLoadTransposeMatrixdARB(const api::GLdouble* m)
{
	log() << warning << "glLoadTransposeMatrixdARB not available." << std::endl;
}

static void fallbackLoadTransposeMatrixf(const api::GLfloat* m)
{
	log() << warning << "glLoadTransposeMatrixf not available." << std::endl;
}

static void fallbackLoadTransposeMatrixfARB(const api::GLfloat* m)
{
	log() << warning << "glLoadTransposeMatrixfARB not available." << std::endl;
}

static void fallbackLockArraysEXT(api::GLint first, api::GLsizei count)
{
	log() << warning << "glLockArraysEXT not available." << std::endl;
}

static void fallbackLogicOp(api::GLenum opcode)
{
	log() << warning << "glLogicOp not available." << std::endl;
}

static void fallbackMakeBufferNonResidentNV(api::GLenum target)
{
	log() << warning << "glMakeBufferNonResidentNV not available." << std::endl;
}

static void fallbackMakeBufferResidentNV(api::GLenum target, api::GLenum access)
{
	log() << warning << "glMakeBufferResidentNV not available." << std::endl;
}

static void fallbackMakeNamedBufferNonResidentNV(api::GLuint buffer)
{
	log() << warning << "glMakeNamedBufferNonResidentNV not available." << std::endl;
}

static void fallbackMakeNamedBufferResidentNV(api::GLuint buffer, api::GLenum access)
{
	log() << warning << "glMakeNamedBufferResidentNV not available." << std::endl;
}

static void fallbackMap1d(api::GLenum target, api::GLdouble u1, api::GLdouble u2, api::GLint stride, api::GLint order, const api::GLdouble* points)
{
	log() << warning << "glMap1d not available." << std::endl;
}

static void fallbackMap1f(api::GLenum target, api::GLfloat u1, api::GLfloat u2, api::GLint stride, api::GLint order, const api::GLfloat* points)
{
	log() << warning << "glMap1f not available." << std::endl;
}

static void fallbackMap2d(api::GLenum target, api::GLdouble u1, api::GLdouble u2, api::GLint ustride, api::GLint uorder, api::GLdouble v1, api::GLdouble v2, api::GLint vstride, api::GLint vorder, const api::GLdouble* points)
{
	log() << warning << "glMap2d not available." << std::endl;
}

static void fallbackMap2f(api::GLenum target, api::GLfloat u1, api::GLfloat u2, api::GLint ustride, api::GLint uorder, api::GLfloat v1, api::GLfloat v2, api::GLint vstride, api::GLint vorder, const api::GLfloat* points)
{
	log() << warning << "glMap2f not available." << std::endl;
}

static api::GLvoid* fallbackMapBuffer(api::GLenum target, api::GLenum access)
{
	log() << warning << "glMapBuffer not available." << std::endl;
}

static api::GLvoid* fallbackMapBufferARB(api::GLenum target, api::GLenum access)
{
	log() << warning << "glMapBufferARB not available." << std::endl;
}

static api::GLvoid* fallbackMapBufferRange(api::GLenum target, api::GLintptr offset, api::GLsizeiptr length, api::GLbitfield access)
{
	log() << warning << "glMapBufferRange not available." << std::endl;
}

static void fallbackMapControlPointsNV(api::GLenum target, api::GLuint index, api::GLenum type, api::GLsizei ustride, api::GLsizei vstride, api::GLint uorder, api::GLint vorder, api::GLboolean packed, const api::GLvoid* points)
{
	log() << warning << "glMapControlPointsNV not available." << std::endl;
}

static void fallbackMapGrid1d(api::GLint un, api::GLdouble u1, api::GLdouble u2)
{
	log() << warning << "glMapGrid1d not available." << std::endl;
}

static void fallbackMapGrid1f(api::GLint un, api::GLfloat u1, api::GLfloat u2)
{
	log() << warning << "glMapGrid1f not available." << std::endl;
}

static void fallbackMapGrid2d(api::GLint un, api::GLdouble u1, api::GLdouble u2, api::GLint vn, api::GLdouble v1, api::GLdouble v2)
{
	log() << warning << "glMapGrid2d not available." << std::endl;
}

static void fallbackMapGrid2f(api::GLint un, api::GLfloat u1, api::GLfloat u2, api::GLint vn, api::GLfloat v1, api::GLfloat v2)
{
	log() << warning << "glMapGrid2f not available." << std::endl;
}

static api::GLvoid* fallbackMapNamedBufferEXT(api::GLuint buffer, api::GLenum access)
{
	log() << warning << "glMapNamedBufferEXT not available." << std::endl;
}

static api::GLvoid* fallbackMapObjectBufferATI(api::GLuint buffer)
{
	log() << warning << "glMapObjectBufferATI not available." << std::endl;
}

static void fallbackMapParameterfvNV(api::GLenum target, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "glMapParameterfvNV not available." << std::endl;
}

static void fallbackMapParameterivNV(api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "glMapParameterivNV not available." << std::endl;
}

static void fallbackMapVertexAttrib1dAPPLE(api::GLuint index, api::GLuint size, api::GLdouble u1, api::GLdouble u2, api::GLint stride, api::GLint order, const api::GLdouble* points)
{
	log() << warning << "glMapVertexAttrib1dAPPLE not available." << std::endl;
}

static void fallbackMapVertexAttrib1fAPPLE(api::GLuint index, api::GLuint size, api::GLfloat u1, api::GLfloat u2, api::GLint stride, api::GLint order, const api::GLfloat* points)
{
	log() << warning << "glMapVertexAttrib1fAPPLE not available." << std::endl;
}

static void fallbackMapVertexAttrib2dAPPLE(api::GLuint index, api::GLuint size, api::GLdouble u1, api::GLdouble u2, api::GLint ustride, api::GLint uorder, api::GLdouble v1, api::GLdouble v2, api::GLint vstride, api::GLint vorder, const api::GLdouble* points)
{
	log() << warning << "glMapVertexAttrib2dAPPLE not available." << std::endl;
}

static void fallbackMapVertexAttrib2fAPPLE(api::GLuint index, api::GLuint size, api::GLfloat u1, api::GLfloat u2, api::GLint ustride, api::GLint uorder, api::GLfloat v1, api::GLfloat v2, api::GLint vstride, api::GLint vorder, const api::GLfloat* points)
{
	log() << warning << "glMapVertexAttrib2fAPPLE not available." << std::endl;
}

static void fallbackMaterialf(api::GLenum face, api::GLenum pname, api::GLfloat param)
{
	log() << warning << "glMaterialf not available." << std::endl;
}

static void fallbackMaterialfv(api::GLenum face, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "glMaterialfv not available." << std::endl;
}

static void fallbackMateriali(api::GLenum face, api::GLenum pname, api::GLint param)
{
	log() << warning << "glMateriali not available." << std::endl;
}

static void fallbackMaterialiv(api::GLenum face, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "glMaterialiv not available." << std::endl;
}

static void fallbackMatrixFrustumEXT(api::GLenum mode, api::GLdouble left, api::GLdouble right, api::GLdouble bottom, api::GLdouble top, api::GLdouble zNear, api::GLdouble zFar)
{
	log() << warning << "glMatrixFrustumEXT not available." << std::endl;
}

static void fallbackMatrixIndexPointerARB(api::GLint size, api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "glMatrixIndexPointerARB not available." << std::endl;
}

static void fallbackMatrixIndexubvARB(api::GLint size, const api::GLubyte* indices)
{
	log() << warning << "glMatrixIndexubvARB not available." << std::endl;
}

static void fallbackMatrixIndexuivARB(api::GLint size, const api::GLuint* indices)
{
	log() << warning << "glMatrixIndexuivARB not available." << std::endl;
}

static void fallbackMatrixIndexusvARB(api::GLint size, const api::GLushort* indices)
{
	log() << warning << "glMatrixIndexusvARB not available." << std::endl;
}

static void fallbackMatrixLoadIdentityEXT(api::GLenum mode)
{
	log() << warning << "glMatrixLoadIdentityEXT not available." << std::endl;
}

static void fallbackMatrixLoadTransposedEXT(api::GLenum mode, const api::GLdouble* m)
{
	log() << warning << "glMatrixLoadTransposedEXT not available." << std::endl;
}

static void fallbackMatrixLoadTransposefEXT(api::GLenum mode, const api::GLfloat* m)
{
	log() << warning << "glMatrixLoadTransposefEXT not available." << std::endl;
}

static void fallbackMatrixLoaddEXT(api::GLenum mode, const api::GLdouble* m)
{
	log() << warning << "glMatrixLoaddEXT not available." << std::endl;
}

static void fallbackMatrixLoadfEXT(api::GLenum mode, const api::GLfloat* m)
{
	log() << warning << "glMatrixLoadfEXT not available." << std::endl;
}

static void fallbackMatrixMode(api::GLenum mode)
{
	log() << warning << "glMatrixMode not available." << std::endl;
}

static void fallbackMatrixMultTransposedEXT(api::GLenum mode, const api::GLdouble* m)
{
	log() << warning << "glMatrixMultTransposedEXT not available." << std::endl;
}

static void fallbackMatrixMultTransposefEXT(api::GLenum mode, const api::GLfloat* m)
{
	log() << warning << "glMatrixMultTransposefEXT not available." << std::endl;
}

static void fallbackMatrixMultdEXT(api::GLenum mode, const api::GLdouble* m)
{
	log() << warning << "glMatrixMultdEXT not available." << std::endl;
}

static void fallbackMatrixMultfEXT(api::GLenum mode, const api::GLfloat* m)
{
	log() << warning << "glMatrixMultfEXT not available." << std::endl;
}

static void fallbackMatrixOrthoEXT(api::GLenum mode, api::GLdouble left, api::GLdouble right, api::GLdouble bottom, api::GLdouble top, api::GLdouble zNear, api::GLdouble zFar)
{
	log() << warning << "glMatrixOrthoEXT not available." << std::endl;
}

static void fallbackMatrixPopEXT(api::GLenum mode)
{
	log() << warning << "glMatrixPopEXT not available." << std::endl;
}

static void fallbackMatrixPushEXT(api::GLenum mode)
{
	log() << warning << "glMatrixPushEXT not available." << std::endl;
}

static void fallbackMatrixRotatedEXT(api::GLenum mode, api::GLdouble angle, api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "glMatrixRotatedEXT not available." << std::endl;
}

static void fallbackMatrixRotatefEXT(api::GLenum mode, api::GLfloat angle, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "glMatrixRotatefEXT not available." << std::endl;
}

static void fallbackMatrixScaledEXT(api::GLenum mode, api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "glMatrixScaledEXT not available." << std::endl;
}

static void fallbackMatrixScalefEXT(api::GLenum mode, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "glMatrixScalefEXT not available." << std::endl;
}

static void fallbackMatrixTranslatedEXT(api::GLenum mode, api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "glMatrixTranslatedEXT not available." << std::endl;
}

static void fallbackMatrixTranslatefEXT(api::GLenum mode, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "glMatrixTranslatefEXT not available." << std::endl;
}

static void fallbackMinSampleShading(api::GLclampf value)
{
	log() << warning << "glMinSampleShading not available." << std::endl;
}

static void fallbackMinmax(api::GLenum target, api::GLenum internalformat, api::GLboolean sink)
{
	log() << warning << "glMinmax not available." << std::endl;
}

static void fallbackMinmaxEXT(api::GLenum target, api::GLenum internalformat, api::GLboolean sink)
{
	log() << warning << "glMinmaxEXT not available." << std::endl;
}

static void fallbackMultMatrixd(const api::GLdouble* m)
{
	log() << warning << "glMultMatrixd not available." << std::endl;
}

static void fallbackMultMatrixf(const api::GLfloat* m)
{
	log() << warning << "glMultMatrixf not available." << std::endl;
}

static void fallbackMultTransposeMatrixd(const api::GLdouble* m)
{
	log() << warning << "glMultTransposeMatrixd not available." << std::endl;
}

static void fallbackMultTransposeMatrixdARB(const api::GLdouble* m)
{
	log() << warning << "glMultTransposeMatrixdARB not available." << std::endl;
}

static void fallbackMultTransposeMatrixf(const api::GLfloat* m)
{
	log() << warning << "glMultTransposeMatrixf not available." << std::endl;
}

static void fallbackMultTransposeMatrixfARB(const api::GLfloat* m)
{
	log() << warning << "glMultTransposeMatrixfARB not available." << std::endl;
}

static void fallbackMultiDrawArrays(api::GLenum mode, api::GLint* first, api::GLsizei* count, api::GLsizei primcount)
{
	log() << warning << "glMultiDrawArrays not available." << std::endl;
}

static void fallbackMultiDrawArraysEXT(api::GLenum mode, api::GLint* first, api::GLsizei* count, api::GLsizei primcount)
{
	log() << warning << "glMultiDrawArraysEXT not available." << std::endl;
}

static void fallbackMultiDrawElementArrayAPPLE(api::GLenum mode, const api::GLint* first, const api::GLsizei* count, api::GLsizei primcount)
{
	log() << warning << "glMultiDrawElementArrayAPPLE not available." << std::endl;
}

static void fallbackMultiDrawElements(api::GLenum mode, const api::GLsizei* count, api::GLenum type, const api::GLvoid** indices, api::GLsizei primcount)
{
	log() << warning << "glMultiDrawElements not available." << std::endl;
}

static void fallbackMultiDrawElementsBaseVertex(api::GLenum mode, const api::GLsizei* count, api::GLenum type, const api::GLvoid** indices, api::GLsizei primcount, const api::GLint* basevertex)
{
	log() << warning << "glMultiDrawElementsBaseVertex not available." << std::endl;
}

static void fallbackMultiDrawElementsEXT(api::GLenum mode, const api::GLsizei* count, api::GLenum type, const api::GLvoid** indices, api::GLsizei primcount)
{
	log() << warning << "glMultiDrawElementsEXT not available." << std::endl;
}

static void fallbackMultiDrawRangeElementArrayAPPLE(api::GLenum mode, api::GLuint start, api::GLuint end, const api::GLint* first, const api::GLsizei* count, api::GLsizei primcount)
{
	log() << warning << "glMultiDrawRangeElementArrayAPPLE not available." << std::endl;
}

static void fallbackMultiModeDrawArraysIBM(const api::GLenum* mode, const api::GLint* first, const api::GLsizei* count, api::GLsizei primcount, api::GLint modestride)
{
	log() << warning << "glMultiModeDrawArraysIBM not available." << std::endl;
}

static void fallbackMultiModeDrawElementsIBM(const api::GLenum* mode, const api::GLsizei* count, api::GLenum type, const api::GLvoid* const* indices, api::GLsizei primcount, api::GLint modestride)
{
	log() << warning << "glMultiModeDrawElementsIBM not available." << std::endl;
}

static void fallbackMultiTexBufferEXT(api::GLenum texunit, api::GLenum target, api::GLenum internalformat, api::GLuint buffer)
{
	log() << warning << "glMultiTexBufferEXT not available." << std::endl;
}

static void fallbackMultiTexCoord1d(api::GLenum target, api::GLdouble s)
{
	log() << warning << "glMultiTexCoord1d not available." << std::endl;
}

static void fallbackMultiTexCoord1dARB(api::GLenum target, api::GLdouble s)
{
	log() << warning << "glMultiTexCoord1dARB not available." << std::endl;
}

static void fallbackMultiTexCoord1dv(api::GLenum target, const api::GLdouble* v)
{
	log() << warning << "glMultiTexCoord1dv not available." << std::endl;
}

static void fallbackMultiTexCoord1dvARB(api::GLenum target, const api::GLdouble* v)
{
	log() << warning << "glMultiTexCoord1dvARB not available." << std::endl;
}

static void fallbackMultiTexCoord1f(api::GLenum target, api::GLfloat s)
{
	log() << warning << "glMultiTexCoord1f not available." << std::endl;
}

static void fallbackMultiTexCoord1fARB(api::GLenum target, api::GLfloat s)
{
	log() << warning << "glMultiTexCoord1fARB not available." << std::endl;
}

static void fallbackMultiTexCoord1fv(api::GLenum target, const api::GLfloat* v)
{
	log() << warning << "glMultiTexCoord1fv not available." << std::endl;
}

static void fallbackMultiTexCoord1fvARB(api::GLenum target, const api::GLfloat* v)
{
	log() << warning << "glMultiTexCoord1fvARB not available." << std::endl;
}

static void fallbackMultiTexCoord1hNV(api::GLenum target, api::GLhalfNV s)
{
	log() << warning << "glMultiTexCoord1hNV not available." << std::endl;
}

static void fallbackMultiTexCoord1hvNV(api::GLenum target, const api::GLhalfNV* v)
{
	log() << warning << "glMultiTexCoord1hvNV not available." << std::endl;
}

static void fallbackMultiTexCoord1i(api::GLenum target, api::GLint s)
{
	log() << warning << "glMultiTexCoord1i not available." << std::endl;
}

static void fallbackMultiTexCoord1iARB(api::GLenum target, api::GLint s)
{
	log() << warning << "glMultiTexCoord1iARB not available." << std::endl;
}

static void fallbackMultiTexCoord1iv(api::GLenum target, const api::GLint* v)
{
	log() << warning << "glMultiTexCoord1iv not available." << std::endl;
}

static void fallbackMultiTexCoord1ivARB(api::GLenum target, const api::GLint* v)
{
	log() << warning << "glMultiTexCoord1ivARB not available." << std::endl;
}

static void fallbackMultiTexCoord1s(api::GLenum target, api::GLshort s)
{
	log() << warning << "glMultiTexCoord1s not available." << std::endl;
}

static void fallbackMultiTexCoord1sARB(api::GLenum target, api::GLshort s)
{
	log() << warning << "glMultiTexCoord1sARB not available." << std::endl;
}

static void fallbackMultiTexCoord1sv(api::GLenum target, const api::GLshort* v)
{
	log() << warning << "glMultiTexCoord1sv not available." << std::endl;
}

static void fallbackMultiTexCoord1svARB(api::GLenum target, const api::GLshort* v)
{
	log() << warning << "glMultiTexCoord1svARB not available." << std::endl;
}

static void fallbackMultiTexCoord2d(api::GLenum target, api::GLdouble s, api::GLdouble t)
{
	log() << warning << "glMultiTexCoord2d not available." << std::endl;
}

static void fallbackMultiTexCoord2dARB(api::GLenum target, api::GLdouble s, api::GLdouble t)
{
	log() << warning << "glMultiTexCoord2dARB not available." << std::endl;
}

static void fallbackMultiTexCoord2dv(api::GLenum target, const api::GLdouble* v)
{
	log() << warning << "glMultiTexCoord2dv not available." << std::endl;
}

static void fallbackMultiTexCoord2dvARB(api::GLenum target, const api::GLdouble* v)
{
	log() << warning << "glMultiTexCoord2dvARB not available." << std::endl;
}

static void fallbackMultiTexCoord2f(api::GLenum target, api::GLfloat s, api::GLfloat t)
{
	log() << warning << "glMultiTexCoord2f not available." << std::endl;
}

static void fallbackMultiTexCoord2fARB(api::GLenum target, api::GLfloat s, api::GLfloat t)
{
	log() << warning << "glMultiTexCoord2fARB not available." << std::endl;
}

static void fallbackMultiTexCoord2fv(api::GLenum target, const api::GLfloat* v)
{
	log() << warning << "glMultiTexCoord2fv not available." << std::endl;
}

static void fallbackMultiTexCoord2fvARB(api::GLenum target, const api::GLfloat* v)
{
	log() << warning << "glMultiTexCoord2fvARB not available." << std::endl;
}

static void fallbackMultiTexCoord2hNV(api::GLenum target, api::GLhalfNV s, api::GLhalfNV t)
{
	log() << warning << "glMultiTexCoord2hNV not available." << std::endl;
}

static void fallbackMultiTexCoord2hvNV(api::GLenum target, const api::GLhalfNV* v)
{
	log() << warning << "glMultiTexCoord2hvNV not available." << std::endl;
}

static void fallbackMultiTexCoord2i(api::GLenum target, api::GLint s, api::GLint t)
{
	log() << warning << "glMultiTexCoord2i not available." << std::endl;
}

static void fallbackMultiTexCoord2iARB(api::GLenum target, api::GLint s, api::GLint t)
{
	log() << warning << "glMultiTexCoord2iARB not available." << std::endl;
}

static void fallbackMultiTexCoord2iv(api::GLenum target, const api::GLint* v)
{
	log() << warning << "glMultiTexCoord2iv not available." << std::endl;
}

static void fallbackMultiTexCoord2ivARB(api::GLenum target, const api::GLint* v)
{
	log() << warning << "glMultiTexCoord2ivARB not available." << std::endl;
}

static void fallbackMultiTexCoord2s(api::GLenum target, api::GLshort s, api::GLshort t)
{
	log() << warning << "glMultiTexCoord2s not available." << std::endl;
}

static void fallbackMultiTexCoord2sARB(api::GLenum target, api::GLshort s, api::GLshort t)
{
	log() << warning << "glMultiTexCoord2sARB not available." << std::endl;
}

static void fallbackMultiTexCoord2sv(api::GLenum target, const api::GLshort* v)
{
	log() << warning << "glMultiTexCoord2sv not available." << std::endl;
}

static void fallbackMultiTexCoord2svARB(api::GLenum target, const api::GLshort* v)
{
	log() << warning << "glMultiTexCoord2svARB not available." << std::endl;
}

static void fallbackMultiTexCoord3d(api::GLenum target, api::GLdouble s, api::GLdouble t, api::GLdouble r)
{
	log() << warning << "glMultiTexCoord3d not available." << std::endl;
}

static void fallbackMultiTexCoord3dARB(api::GLenum target, api::GLdouble s, api::GLdouble t, api::GLdouble r)
{
	log() << warning << "glMultiTexCoord3dARB not available." << std::endl;
}

static void fallbackMultiTexCoord3dv(api::GLenum target, const api::GLdouble* v)
{
	log() << warning << "glMultiTexCoord3dv not available." << std::endl;
}

static void fallbackMultiTexCoord3dvARB(api::GLenum target, const api::GLdouble* v)
{
	log() << warning << "glMultiTexCoord3dvARB not available." << std::endl;
}

static void fallbackMultiTexCoord3f(api::GLenum target, api::GLfloat s, api::GLfloat t, api::GLfloat r)
{
	log() << warning << "glMultiTexCoord3f not available." << std::endl;
}

static void fallbackMultiTexCoord3fARB(api::GLenum target, api::GLfloat s, api::GLfloat t, api::GLfloat r)
{
	log() << warning << "glMultiTexCoord3fARB not available." << std::endl;
}

static void fallbackMultiTexCoord3fv(api::GLenum target, const api::GLfloat* v)
{
	log() << warning << "glMultiTexCoord3fv not available." << std::endl;
}

static void fallbackMultiTexCoord3fvARB(api::GLenum target, const api::GLfloat* v)
{
	log() << warning << "glMultiTexCoord3fvARB not available." << std::endl;
}

static void fallbackMultiTexCoord3hNV(api::GLenum target, api::GLhalfNV s, api::GLhalfNV t, api::GLhalfNV r)
{
	log() << warning << "glMultiTexCoord3hNV not available." << std::endl;
}

static void fallbackMultiTexCoord3hvNV(api::GLenum target, const api::GLhalfNV* v)
{
	log() << warning << "glMultiTexCoord3hvNV not available." << std::endl;
}

static void fallbackMultiTexCoord3i(api::GLenum target, api::GLint s, api::GLint t, api::GLint r)
{
	log() << warning << "glMultiTexCoord3i not available." << std::endl;
}

static void fallbackMultiTexCoord3iARB(api::GLenum target, api::GLint s, api::GLint t, api::GLint r)
{
	log() << warning << "glMultiTexCoord3iARB not available." << std::endl;
}

static void fallbackMultiTexCoord3iv(api::GLenum target, const api::GLint* v)
{
	log() << warning << "glMultiTexCoord3iv not available." << std::endl;
}

static void fallbackMultiTexCoord3ivARB(api::GLenum target, const api::GLint* v)
{
	log() << warning << "glMultiTexCoord3ivARB not available." << std::endl;
}

static void fallbackMultiTexCoord3s(api::GLenum target, api::GLshort s, api::GLshort t, api::GLshort r)
{
	log() << warning << "glMultiTexCoord3s not available." << std::endl;
}

static void fallbackMultiTexCoord3sARB(api::GLenum target, api::GLshort s, api::GLshort t, api::GLshort r)
{
	log() << warning << "glMultiTexCoord3sARB not available." << std::endl;
}

static void fallbackMultiTexCoord3sv(api::GLenum target, const api::GLshort* v)
{
	log() << warning << "glMultiTexCoord3sv not available." << std::endl;
}

static void fallbackMultiTexCoord3svARB(api::GLenum target, const api::GLshort* v)
{
	log() << warning << "glMultiTexCoord3svARB not available." << std::endl;
}

static void fallbackMultiTexCoord4d(api::GLenum target, api::GLdouble s, api::GLdouble t, api::GLdouble r, api::GLdouble q)
{
	log() << warning << "glMultiTexCoord4d not available." << std::endl;
}

static void fallbackMultiTexCoord4dARB(api::GLenum target, api::GLdouble s, api::GLdouble t, api::GLdouble r, api::GLdouble q)
{
	log() << warning << "glMultiTexCoord4dARB not available." << std::endl;
}

static void fallbackMultiTexCoord4dv(api::GLenum target, const api::GLdouble* v)
{
	log() << warning << "glMultiTexCoord4dv not available." << std::endl;
}

static void fallbackMultiTexCoord4dvARB(api::GLenum target, const api::GLdouble* v)
{
	log() << warning << "glMultiTexCoord4dvARB not available." << std::endl;
}

static void fallbackMultiTexCoord4f(api::GLenum target, api::GLfloat s, api::GLfloat t, api::GLfloat r, api::GLfloat q)
{
	log() << warning << "glMultiTexCoord4f not available." << std::endl;
}

static void fallbackMultiTexCoord4fARB(api::GLenum target, api::GLfloat s, api::GLfloat t, api::GLfloat r, api::GLfloat q)
{
	log() << warning << "glMultiTexCoord4fARB not available." << std::endl;
}

static void fallbackMultiTexCoord4fv(api::GLenum target, const api::GLfloat* v)
{
	log() << warning << "glMultiTexCoord4fv not available." << std::endl;
}

static void fallbackMultiTexCoord4fvARB(api::GLenum target, const api::GLfloat* v)
{
	log() << warning << "glMultiTexCoord4fvARB not available." << std::endl;
}

static void fallbackMultiTexCoord4hNV(api::GLenum target, api::GLhalfNV s, api::GLhalfNV t, api::GLhalfNV r, api::GLhalfNV q)
{
	log() << warning << "glMultiTexCoord4hNV not available." << std::endl;
}

static void fallbackMultiTexCoord4hvNV(api::GLenum target, const api::GLhalfNV* v)
{
	log() << warning << "glMultiTexCoord4hvNV not available." << std::endl;
}

static void fallbackMultiTexCoord4i(api::GLenum target, api::GLint s, api::GLint t, api::GLint r, api::GLint q)
{
	log() << warning << "glMultiTexCoord4i not available." << std::endl;
}

static void fallbackMultiTexCoord4iARB(api::GLenum target, api::GLint s, api::GLint t, api::GLint r, api::GLint q)
{
	log() << warning << "glMultiTexCoord4iARB not available." << std::endl;
}

static void fallbackMultiTexCoord4iv(api::GLenum target, const api::GLint* v)
{
	log() << warning << "glMultiTexCoord4iv not available." << std::endl;
}

static void fallbackMultiTexCoord4ivARB(api::GLenum target, const api::GLint* v)
{
	log() << warning << "glMultiTexCoord4ivARB not available." << std::endl;
}

static void fallbackMultiTexCoord4s(api::GLenum target, api::GLshort s, api::GLshort t, api::GLshort r, api::GLshort q)
{
	log() << warning << "glMultiTexCoord4s not available." << std::endl;
}

static void fallbackMultiTexCoord4sARB(api::GLenum target, api::GLshort s, api::GLshort t, api::GLshort r, api::GLshort q)
{
	log() << warning << "glMultiTexCoord4sARB not available." << std::endl;
}

static void fallbackMultiTexCoord4sv(api::GLenum target, const api::GLshort* v)
{
	log() << warning << "glMultiTexCoord4sv not available." << std::endl;
}

static void fallbackMultiTexCoord4svARB(api::GLenum target, const api::GLshort* v)
{
	log() << warning << "glMultiTexCoord4svARB not available." << std::endl;
}

static void fallbackMultiTexCoordP1ui(api::GLenum texture, api::GLenum type, api::GLuint coords)
{
	log() << warning << "glMultiTexCoordP1ui not available." << std::endl;
}

static void fallbackMultiTexCoordP1uiv(api::GLenum texture, api::GLenum type, const api::GLuint* coords)
{
	log() << warning << "glMultiTexCoordP1uiv not available." << std::endl;
}

static void fallbackMultiTexCoordP2ui(api::GLenum texture, api::GLenum type, api::GLuint coords)
{
	log() << warning << "glMultiTexCoordP2ui not available." << std::endl;
}

static void fallbackMultiTexCoordP2uiv(api::GLenum texture, api::GLenum type, const api::GLuint* coords)
{
	log() << warning << "glMultiTexCoordP2uiv not available." << std::endl;
}

static void fallbackMultiTexCoordP3ui(api::GLenum texture, api::GLenum type, api::GLuint coords)
{
	log() << warning << "glMultiTexCoordP3ui not available." << std::endl;
}

static void fallbackMultiTexCoordP3uiv(api::GLenum texture, api::GLenum type, const api::GLuint* coords)
{
	log() << warning << "glMultiTexCoordP3uiv not available." << std::endl;
}

static void fallbackMultiTexCoordP4ui(api::GLenum texture, api::GLenum type, api::GLuint coords)
{
	log() << warning << "glMultiTexCoordP4ui not available." << std::endl;
}

static void fallbackMultiTexCoordP4uiv(api::GLenum texture, api::GLenum type, const api::GLuint* coords)
{
	log() << warning << "glMultiTexCoordP4uiv not available." << std::endl;
}

static void fallbackMultiTexCoordPointerEXT(api::GLenum texunit, api::GLint size, api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "glMultiTexCoordPointerEXT not available." << std::endl;
}

static void fallbackMultiTexEnvfEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, api::GLfloat param)
{
	log() << warning << "glMultiTexEnvfEXT not available." << std::endl;
}

static void fallbackMultiTexEnvfvEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "glMultiTexEnvfvEXT not available." << std::endl;
}

static void fallbackMultiTexEnviEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, api::GLint param)
{
	log() << warning << "glMultiTexEnviEXT not available." << std::endl;
}

static void fallbackMultiTexEnvivEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "glMultiTexEnvivEXT not available." << std::endl;
}

static void fallbackMultiTexGendEXT(api::GLenum texunit, api::GLenum coord, api::GLenum pname, api::GLdouble param)
{
	log() << warning << "glMultiTexGendEXT not available." << std::endl;
}

static void fallbackMultiTexGendvEXT(api::GLenum texunit, api::GLenum coord, api::GLenum pname, const api::GLdouble* params)
{
	log() << warning << "glMultiTexGendvEXT not available." << std::endl;
}

static void fallbackMultiTexGenfEXT(api::GLenum texunit, api::GLenum coord, api::GLenum pname, api::GLfloat param)
{
	log() << warning << "glMultiTexGenfEXT not available." << std::endl;
}

static void fallbackMultiTexGenfvEXT(api::GLenum texunit, api::GLenum coord, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "glMultiTexGenfvEXT not available." << std::endl;
}

static void fallbackMultiTexGeniEXT(api::GLenum texunit, api::GLenum coord, api::GLenum pname, api::GLint param)
{
	log() << warning << "glMultiTexGeniEXT not available." << std::endl;
}

static void fallbackMultiTexGenivEXT(api::GLenum texunit, api::GLenum coord, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "glMultiTexGenivEXT not available." << std::endl;
}

static void fallbackMultiTexImage1DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLint border, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "glMultiTexImage1DEXT not available." << std::endl;
}

static void fallbackMultiTexImage2DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLint border, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "glMultiTexImage2DEXT not available." << std::endl;
}

static void fallbackMultiTexImage3DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLint border, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "glMultiTexImage3DEXT not available." << std::endl;
}

static void fallbackMultiTexParameterIivEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "glMultiTexParameterIivEXT not available." << std::endl;
}

static void fallbackMultiTexParameterIuivEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, const api::GLuint* params)
{
	log() << warning << "glMultiTexParameterIuivEXT not available." << std::endl;
}

static void fallbackMultiTexParameterfEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, api::GLfloat param)
{
	log() << warning << "glMultiTexParameterfEXT not available." << std::endl;
}

static void fallbackMultiTexParameterfvEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "glMultiTexParameterfvEXT not available." << std::endl;
}

static void fallbackMultiTexParameteriEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, api::GLint param)
{
	log() << warning << "glMultiTexParameteriEXT not available." << std::endl;
}

static void fallbackMultiTexParameterivEXT(api::GLenum texunit, api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "glMultiTexParameterivEXT not available." << std::endl;
}

static void fallbackMultiTexRenderbufferEXT(api::GLenum texunit, api::GLenum target, api::GLuint renderbuffer)
{
	log() << warning << "glMultiTexRenderbufferEXT not available." << std::endl;
}

static void fallbackMultiTexSubImage1DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLsizei width, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "glMultiTexSubImage1DEXT not available." << std::endl;
}

static void fallbackMultiTexSubImage2DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLsizei width, api::GLsizei height, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "glMultiTexSubImage2DEXT not available." << std::endl;
}

static void fallbackMultiTexSubImage3DEXT(api::GLenum texunit, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint zoffset, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "glMultiTexSubImage3DEXT not available." << std::endl;
}

static void fallbackNamedBufferDataEXT(api::GLuint buffer, api::GLsizeiptr size, const api::GLvoid* data, api::GLenum usage)
{
	log() << warning << "glNamedBufferDataEXT not available." << std::endl;
}

static void fallbackNamedBufferSubDataEXT(api::GLuint buffer, api::GLintptr offset, api::GLsizeiptr size, const api::GLvoid* data)
{
	log() << warning << "glNamedBufferSubDataEXT not available." << std::endl;
}

static void fallbackNamedFramebufferRenderbufferEXT(api::GLuint framebuffer, api::GLenum attachment, api::GLenum renderbuffertarget, api::GLuint renderbuffer)
{
	log() << warning << "glNamedFramebufferRenderbufferEXT not available." << std::endl;
}

static void fallbackNamedFramebufferTexture1DEXT(api::GLuint framebuffer, api::GLenum attachment, api::GLenum textarget, api::GLuint texture, api::GLint level)
{
	log() << warning << "glNamedFramebufferTexture1DEXT not available." << std::endl;
}

static void fallbackNamedFramebufferTexture2DEXT(api::GLuint framebuffer, api::GLenum attachment, api::GLenum textarget, api::GLuint texture, api::GLint level)
{
	log() << warning << "glNamedFramebufferTexture2DEXT not available." << std::endl;
}

static void fallbackNamedFramebufferTexture3DEXT(api::GLuint framebuffer, api::GLenum attachment, api::GLenum textarget, api::GLuint texture, api::GLint level, api::GLint zoffset)
{
	log() << warning << "glNamedFramebufferTexture3DEXT not available." << std::endl;
}

static void fallbackNamedFramebufferTextureEXT(api::GLuint framebuffer, api::GLenum attachment, api::GLuint texture, api::GLint level)
{
	log() << warning << "glNamedFramebufferTextureEXT not available." << std::endl;
}

static void fallbackNamedFramebufferTextureFaceEXT(api::GLuint framebuffer, api::GLenum attachment, api::GLuint texture, api::GLint level, api::GLenum face)
{
	log() << warning << "glNamedFramebufferTextureFaceEXT not available." << std::endl;
}

static void fallbackNamedFramebufferTextureLayerEXT(api::GLuint framebuffer, api::GLenum attachment, api::GLuint texture, api::GLint level, api::GLint layer)
{
	log() << warning << "glNamedFramebufferTextureLayerEXT not available." << std::endl;
}

static void fallbackNamedProgramLocalParameter4dEXT(api::GLuint program, api::GLenum target, api::GLuint index, api::GLdouble x, api::GLdouble y, api::GLdouble z, api::GLdouble w)
{
	log() << warning << "glNamedProgramLocalParameter4dEXT not available." << std::endl;
}

static void fallbackNamedProgramLocalParameter4dvEXT(api::GLuint program, api::GLenum target, api::GLuint index, const api::GLdouble* params)
{
	log() << warning << "glNamedProgramLocalParameter4dvEXT not available." << std::endl;
}

static void fallbackNamedProgramLocalParameter4fEXT(api::GLuint program, api::GLenum target, api::GLuint index, api::GLfloat x, api::GLfloat y, api::GLfloat z, api::GLfloat w)
{
	log() << warning << "glNamedProgramLocalParameter4fEXT not available." << std::endl;
}

static void fallbackNamedProgramLocalParameter4fvEXT(api::GLuint program, api::GLenum target, api::GLuint index, const api::GLfloat* params)
{
	log() << warning << "glNamedProgramLocalParameter4fvEXT not available." << std::endl;
}

static void fallbackNamedProgramLocalParameterI4iEXT(api::GLuint program, api::GLenum target, api::GLuint index, api::GLint x, api::GLint y, api::GLint z, api::GLint w)
{
	log() << warning << "glNamedProgramLocalParameterI4iEXT not available." << std::endl;
}

static void fallbackNamedProgramLocalParameterI4ivEXT(api::GLuint program, api::GLenum target, api::GLuint index, const api::GLint* params)
{
	log() << warning << "glNamedProgramLocalParameterI4ivEXT not available." << std::endl;
}

static void fallbackNamedProgramLocalParameterI4uiEXT(api::GLuint program, api::GLenum target, api::GLuint index, api::GLuint x, api::GLuint y, api::GLuint z, api::GLuint w)
{
	log() << warning << "glNamedProgramLocalParameterI4uiEXT not available." << std::endl;
}

static void fallbackNamedProgramLocalParameterI4uivEXT(api::GLuint program, api::GLenum target, api::GLuint index, const api::GLuint* params)
{
	log() << warning << "glNamedProgramLocalParameterI4uivEXT not available." << std::endl;
}

static void fallbackNamedProgramLocalParameters4fvEXT(api::GLuint program, api::GLenum target, api::GLuint index, api::GLsizei count, const api::GLfloat* params)
{
	log() << warning << "glNamedProgramLocalParameters4fvEXT not available." << std::endl;
}

static void fallbackNamedProgramLocalParametersI4ivEXT(api::GLuint program, api::GLenum target, api::GLuint index, api::GLsizei count, const api::GLint* params)
{
	log() << warning << "glNamedProgramLocalParametersI4ivEXT not available." << std::endl;
}

static void fallbackNamedProgramLocalParametersI4uivEXT(api::GLuint program, api::GLenum target, api::GLuint index, api::GLsizei count, const api::GLuint* params)
{
	log() << warning << "glNamedProgramLocalParametersI4uivEXT not available." << std::endl;
}

static void fallbackNamedProgramStringEXT(api::GLuint program, api::GLenum target, api::GLenum format, api::GLsizei len, const api::GLvoid* string)
{
	log() << warning << "glNamedProgramStringEXT not available." << std::endl;
}

static void fallbackNamedRenderbufferStorageEXT(api::GLuint renderbuffer, api::GLenum internalformat, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "glNamedRenderbufferStorageEXT not available." << std::endl;
}

static void fallbackNamedRenderbufferStorageMultisampleCoverageEXT(api::GLuint renderbuffer, api::GLsizei coverageSamples, api::GLsizei colorSamples, api::GLenum internalformat, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "glNamedRenderbufferStorageMultisampleCoverageEXT not available." << std::endl;
}

static void fallbackNamedRenderbufferStorageMultisampleEXT(api::GLuint renderbuffer, api::GLsizei samples, api::GLenum internalformat, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "glNamedRenderbufferStorageMultisampleEXT not available." << std::endl;
}

static void fallbackNamedStringARB(api::GLenum type, api::GLint namelen, const api::GLchar* name, api::GLint stringlen, const api::GLchar* string)
{
	log() << warning << "glNamedStringARB not available." << std::endl;
}

static void fallbackNewList(api::GLuint list, api::GLenum mode)
{
	log() << warning << "glNewList not available." << std::endl;
}

static api::GLuint fallbackNewObjectBufferATI(api::GLsizei size, const api::GLvoid* pointer, api::GLenum usage)
{
	log() << warning << "glNewObjectBufferATI not available." << std::endl;
}

static void fallbackNormal3b(api::GLbyte nx, api::GLbyte ny, api::GLbyte nz)
{
	log() << warning << "glNormal3b not available." << std::endl;
}

static void fallbackNormal3bv(const api::GLbyte* v)
{
	log() << warning << "glNormal3bv not available." << std::endl;
}

static void fallbackNormal3d(api::GLdouble nx, api::GLdouble ny, api::GLdouble nz)
{
	log() << warning << "glNormal3d not available." << std::endl;
}

static void fallbackNormal3dv(const api::GLdouble* v)
{
	log() << warning << "glNormal3dv not available." << std::endl;
}

static void fallbackNormal3f(api::GLfloat nx, api::GLfloat ny, api::GLfloat nz)
{
	log() << warning << "glNormal3f not available." << std::endl;
}

static void fallbackNormal3fVertex3fSUN(api::GLfloat nx, api::GLfloat ny, api::GLfloat nz, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "glNormal3fVertex3fSUN not available." << std::endl;
}

static void fallbackNormal3fVertex3fvSUN(const api::GLfloat* n, const api::GLfloat* v)
{
	log() << warning << "glNormal3fVertex3fvSUN not available." << std::endl;
}

static void fallbackNormal3fv(const api::GLfloat* v)
{
	log() << warning << "glNormal3fv not available." << std::endl;
}

static void fallbackNormal3hNV(api::GLhalfNV nx, api::GLhalfNV ny, api::GLhalfNV nz)
{
	log() << warning << "glNormal3hNV not available." << std::endl;
}

static void fallbackNormal3hvNV(const api::GLhalfNV* v)
{
	log() << warning << "glNormal3hvNV not available." << std::endl;
}

static void fallbackNormal3i(api::GLint nx, api::GLint ny, api::GLint nz)
{
	log() << warning << "glNormal3i not available." << std::endl;
}

static void fallbackNormal3iv(const api::GLint* v)
{
	log() << warning << "glNormal3iv not available." << std::endl;
}

static void fallbackNormal3s(api::GLshort nx, api::GLshort ny, api::GLshort nz)
{
	log() << warning << "glNormal3s not available." << std::endl;
}

static void fallbackNormal3sv(const api::GLshort* v)
{
	log() << warning << "glNormal3sv not available." << std::endl;
}

static void fallbackNormalFormatNV(api::GLenum type, api::GLsizei stride)
{
	log() << warning << "glNormalFormatNV not available." << std::endl;
}

static void fallbackNormalP3ui(api::GLenum type, api::GLuint coords)
{
	log() << warning << "glNormalP3ui not available." << std::endl;
}

static void fallbackNormalP3uiv(api::GLenum type, const api::GLuint* coords)
{
	log() << warning << "glNormalP3uiv not available." << std::endl;
}

static void fallbackNormalPointer(api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "glNormalPointer not available." << std::endl;
}

static void fallbackNormalPointerEXT(api::GLenum type, api::GLsizei stride, api::GLsizei count, const api::GLvoid* pointer)
{
	log() << warning << "glNormalPointerEXT not available." << std::endl;
}

static void fallbackNormalPointerListIBM(api::GLenum type, api::GLint stride, const api::GLvoid** pointer, api::GLint ptrstride)
{
	log() << warning << "glNormalPointerListIBM not available." << std::endl;
}

static void fallbackNormalPointervINTEL(api::GLenum type, const api::GLvoid** pointer)
{
	log() << warning << "glNormalPointervINTEL not available." << std::endl;
}

static void fallbackNormalStream3bATI(api::GLenum stream, api::GLbyte nx, api::GLbyte ny, api::GLbyte nz)
{
	log() << warning << "glNormalStream3bATI not available." << std::endl;
}

static void fallbackNormalStream3bvATI(api::GLenum stream, const api::GLbyte* coords)
{
	log() << warning << "glNormalStream3bvATI not available." << std::endl;
}

static void fallbackNormalStream3dATI(api::GLenum stream, api::GLdouble nx, api::GLdouble ny, api::GLdouble nz)
{
	log() << warning << "glNormalStream3dATI not available." << std::endl;
}

static void fallbackNormalStream3dvATI(api::GLenum stream, const api::GLdouble* coords)
{
	log() << warning << "glNormalStream3dvATI not available." << std::endl;
}

static void fallbackNormalStream3fATI(api::GLenum stream, api::GLfloat nx, api::GLfloat ny, api::GLfloat nz)
{
	log() << warning << "glNormalStream3fATI not available." << std::endl;
}

static void fallbackNormalStream3fvATI(api::GLenum stream, const api::GLfloat* coords)
{
	log() << warning << "glNormalStream3fvATI not available." << std::endl;
}

static void fallbackNormalStream3iATI(api::GLenum stream, api::GLint nx, api::GLint ny, api::GLint nz)
{
	log() << warning << "glNormalStream3iATI not available." << std::endl;
}

static void fallbackNormalStream3ivATI(api::GLenum stream, const api::GLint* coords)
{
	log() << warning << "glNormalStream3ivATI not available." << std::endl;
}

static void fallbackNormalStream3sATI(api::GLenum stream, api::GLshort nx, api::GLshort ny, api::GLshort nz)
{
	log() << warning << "glNormalStream3sATI not available." << std::endl;
}

static void fallbackNormalStream3svATI(api::GLenum stream, const api::GLshort* coords)
{
	log() << warning << "glNormalStream3svATI not available." << std::endl;
}

static api::GLenum fallbackObjectPurgeableAPPLE(api::GLenum objectType, api::GLuint name, api::GLenum option)
{
	log() << warning << "glObjectPurgeableAPPLE not available." << std::endl;
}

static api::GLenum fallbackObjectUnpurgeableAPPLE(api::GLenum objectType, api::GLuint name, api::GLenum option)
{
	log() << warning << "glObjectUnpurgeableAPPLE not available." << std::endl;
}

static void fallbackOrtho(api::GLdouble left, api::GLdouble right, api::GLdouble bottom, api::GLdouble top, api::GLdouble zNear, api::GLdouble zFar)
{
	log() << warning << "glOrtho not available." << std::endl;
}

static void fallbackPNTrianglesfATI(api::GLenum pname, api::GLfloat param)
{
	log() << warning << "glPNTrianglesfATI not available." << std::endl;
}

static void fallbackPNTrianglesiATI(api::GLenum pname, api::GLint param)
{
	log() << warning << "glPNTrianglesiATI not available." << std::endl;
}

static void fallbackPassTexCoordATI(api::GLuint dst, api::GLuint coord, api::GLenum swizzle)
{
	log() << warning << "glPassTexCoordATI not available." << std::endl;
}

static void fallbackPassThrough(api::GLfloat token)
{
	log() << warning << "glPassThrough not available." << std::endl;
}

static void fallbackPatchParameterfv(api::GLenum pname, const api::GLfloat* values)
{
	log() << warning << "glPatchParameterfv not available." << std::endl;
}

static void fallbackPatchParameteri(api::GLenum pname, api::GLint value)
{
	log() << warning << "glPatchParameteri not available." << std::endl;
}

static void fallbackPauseTransformFeedback()
{
	log() << warning << "glPauseTransformFeedback not available." << std::endl;
}

static void fallbackPauseTransformFeedbackNV()
{
	log() << warning << "glPauseTransformFeedbackNV not available." << std::endl;
}

static void fallbackPixelDataRangeNV(api::GLenum target, api::GLsizei length, api::GLvoid* pointer)
{
	log() << warning << "glPixelDataRangeNV not available." << std::endl;
}

static void fallbackPixelMapfv(api::GLenum map, api::GLint mapsize, const api::GLfloat* values)
{
	log() << warning << "glPixelMapfv not available." << std::endl;
}

static void fallbackPixelMapuiv(api::GLenum map, api::GLint mapsize, const api::GLuint* values)
{
	log() << warning << "glPixelMapuiv not available." << std::endl;
}

static void fallbackPixelMapusv(api::GLenum map, api::GLint mapsize, const api::GLushort* values)
{
	log() << warning << "glPixelMapusv not available." << std::endl;
}

static void fallbackPixelStoref(api::GLenum pname, api::GLfloat param)
{
	log() << warning << "glPixelStoref not available." << std::endl;
}

static void fallbackPixelStorei(api::GLenum pname, api::GLint param)
{
	log() << warning << "glPixelStorei not available." << std::endl;
}

static void fallbackPixelTexGenParameterfSGIS(api::GLenum pname, api::GLfloat param)
{
	log() << warning << "glPixelTexGenParameterfSGIS not available." << std::endl;
}

static void fallbackPixelTexGenParameterfvSGIS(api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "glPixelTexGenParameterfvSGIS not available." << std::endl;
}

static void fallbackPixelTexGenParameteriSGIS(api::GLenum pname, api::GLint param)
{
	log() << warning << "glPixelTexGenParameteriSGIS not available." << std::endl;
}

static void fallbackPixelTexGenParameterivSGIS(api::GLenum pname, const api::GLint* params)
{
	log() << warning << "glPixelTexGenParameterivSGIS not available." << std::endl;
}

static void fallbackPixelTexGenSGIX(api::GLenum mode)
{
	log() << warning << "glPixelTexGenSGIX not available." << std::endl;
}

static void fallbackPixelTransferf(api::GLenum pname, api::GLfloat param)
{
	log() << warning << "glPixelTransferf not available." << std::endl;
}

static void fallbackPixelTransferi(api::GLenum pname, api::GLint param)
{
	log() << warning << "glPixelTransferi not available." << std::endl;
}

static void fallbackPixelTransformParameterfEXT(api::GLenum target, api::GLenum pname, api::GLfloat param)
{
	log() << warning << "glPixelTransformParameterfEXT not available." << std::endl;
}

static void fallbackPixelTransformParameterfvEXT(api::GLenum target, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "glPixelTransformParameterfvEXT not available." << std::endl;
}

static void fallbackPixelTransformParameteriEXT(api::GLenum target, api::GLenum pname, api::GLint param)
{
	log() << warning << "glPixelTransformParameteriEXT not available." << std::endl;
}

static void fallbackPixelTransformParameterivEXT(api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "glPixelTransformParameterivEXT not available." << std::endl;
}

static void fallbackPixelZoom(api::GLfloat xfactor, api::GLfloat yfactor)
{
	log() << warning << "glPixelZoom not available." << std::endl;
}

static void fallbackPointParameterf(api::GLenum pname, api::GLfloat param)
{
	log() << warning << "glPointParameterf not available." << std::endl;
}

static void fallbackPointParameterfARB(api::GLenum pname, api::GLfloat param)
{
	log() << warning << "glPointParameterfARB not available." << std::endl;
}

static void fallbackPointParameterfEXT(api::GLenum pname, api::GLfloat param)
{
	log() << warning << "glPointParameterfEXT not available." << std::endl;
}

static void fallbackPointParameterfSGIS(api::GLenum pname, api::GLfloat param)
{
	log() << warning << "glPointParameterfSGIS not available." << std::endl;
}

static void fallbackPointParameterfv(api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "glPointParameterfv not available." << std::endl;
}

static void fallbackPointParameterfvARB(api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "glPointParameterfvARB not available." << std::endl;
}

static void fallbackPointParameterfvEXT(api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "glPointParameterfvEXT not available." << std::endl;
}

static void fallbackPointParameterfvSGIS(api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "glPointParameterfvSGIS not available." << std::endl;
}

static void fallbackPointParameteri(api::GLenum pname, api::GLint param)
{
	log() << warning << "glPointParameteri not available." << std::endl;
}

static void fallbackPointParameteriNV(api::GLenum pname, api::GLint param)
{
	log() << warning << "glPointParameteriNV not available." << std::endl;
}

static void fallbackPointParameteriv(api::GLenum pname, const api::GLint* params)
{
	log() << warning << "glPointParameteriv not available." << std::endl;
}

static void fallbackPointParameterivNV(api::GLenum pname, const api::GLint* params)
{
	log() << warning << "glPointParameterivNV not available." << std::endl;
}

static void fallbackPointSize(api::GLfloat size)
{
	log() << warning << "glPointSize not available." << std::endl;
}

static api::GLint fallbackPollAsyncSGIX(api::GLuint* markerp)
{
	log() << warning << "glPollAsyncSGIX not available." << std::endl;
}

static api::GLint fallbackPollInstrumentsSGIX(api::GLint* marker_p)
{
	log() << warning << "glPollInstrumentsSGIX not available." << std::endl;
}

static void fallbackPolygonMode(api::GLenum face, api::GLenum mode)
{
	log() << warning << "glPolygonMode not available." << std::endl;
}

static void fallbackPolygonOffset(api::GLfloat factor, api::GLfloat units)
{
	log() << warning << "glPolygonOffset not available." << std::endl;
}

static void fallbackPolygonOffsetEXT(api::GLfloat factor, api::GLfloat bias)
{
	log() << warning << "glPolygonOffsetEXT not available." << std::endl;
}

static void fallbackPolygonStipple(const api::GLubyte* mask)
{
	log() << warning << "glPolygonStipple not available." << std::endl;
}

static void fallbackPopAttrib()
{
	log() << warning << "glPopAttrib not available." << std::endl;
}

static void fallbackPopClientAttrib()
{
	log() << warning << "glPopClientAttrib not available." << std::endl;
}

static void fallbackPopMatrix()
{
	log() << warning << "glPopMatrix not available." << std::endl;
}

static void fallbackPopName()
{
	log() << warning << "glPopName not available." << std::endl;
}

static void fallbackPresentFrameDualFillNV(api::GLuint video_slot, api::GLuint64EXT minPresentTime, api::GLuint beginPresentTimeId, api::GLuint presentDurationId, api::GLenum type, api::GLenum target0, api::GLuint fill0, api::GLenum target1, api::GLuint fill1, api::GLenum target2, api::GLuint fill2, api::GLenum target3, api::GLuint fill3)
{
	log() << warning << "glPresentFrameDualFillNV not available." << std::endl;
}

static void fallbackPresentFrameKeyedNV(api::GLuint video_slot, api::GLuint64EXT minPresentTime, api::GLuint beginPresentTimeId, api::GLuint presentDurationId, api::GLenum type, api::GLenum target0, api::GLuint fill0, api::GLuint key0, api::GLenum target1, api::GLuint fill1, api::GLuint key1)
{
	log() << warning << "glPresentFrameKeyedNV not available." << std::endl;
}

static void fallbackPrimitiveRestartIndex(api::GLuint index)
{
	log() << warning << "glPrimitiveRestartIndex not available." << std::endl;
}

static void fallbackPrimitiveRestartIndexNV(api::GLuint index)
{
	log() << warning << "glPrimitiveRestartIndexNV not available." << std::endl;
}

static void fallbackPrimitiveRestartNV()
{
	log() << warning << "glPrimitiveRestartNV not available." << std::endl;
}

static void fallbackPrioritizeTextures(api::GLsizei n, const api::GLuint* textures, const api::GLclampf* priorities)
{
	log() << warning << "glPrioritizeTextures not available." << std::endl;
}

static void fallbackPrioritizeTexturesEXT(api::GLsizei n, const api::GLuint* textures, const api::GLclampf* priorities)
{
	log() << warning << "glPrioritizeTexturesEXT not available." << std::endl;
}

static void fallbackProgramBufferParametersIivNV(api::GLenum target, api::GLuint buffer, api::GLuint index, api::GLsizei count, const api::GLint* params)
{
	log() << warning << "glProgramBufferParametersIivNV not available." << std::endl;
}

static void fallbackProgramBufferParametersIuivNV(api::GLenum target, api::GLuint buffer, api::GLuint index, api::GLsizei count, const api::GLuint* params)
{
	log() << warning << "glProgramBufferParametersIuivNV not available." << std::endl;
}

static void fallbackProgramBufferParametersfvNV(api::GLenum target, api::GLuint buffer, api::GLuint index, api::GLsizei count, const api::GLfloat* params)
{
	log() << warning << "glProgramBufferParametersfvNV not available." << std::endl;
}

static void fallbackProgramEnvParameter4dARB(api::GLenum target, api::GLuint index, api::GLdouble x, api::GLdouble y, api::GLdouble z, api::GLdouble w)
{
	log() << warning << "glProgramEnvParameter4dARB not available." << std::endl;
}

static void fallbackProgramEnvParameter4dvARB(api::GLenum target, api::GLuint index, const api::GLdouble* params)
{
	log() << warning << "glProgramEnvParameter4dvARB not available." << std::endl;
}

static void fallbackProgramEnvParameter4fARB(api::GLenum target, api::GLuint index, api::GLfloat x, api::GLfloat y, api::GLfloat z, api::GLfloat w)
{
	log() << warning << "glProgramEnvParameter4fARB not available." << std::endl;
}

static void fallbackProgramEnvParameter4fvARB(api::GLenum target, api::GLuint index, const api::GLfloat* params)
{
	log() << warning << "glProgramEnvParameter4fvARB not available." << std::endl;
}

static void fallbackProgramEnvParameterI4iNV(api::GLenum target, api::GLuint index, api::GLint x, api::GLint y, api::GLint z, api::GLint w)
{
	log() << warning << "glProgramEnvParameterI4iNV not available." << std::endl;
}

static void fallbackProgramEnvParameterI4ivNV(api::GLenum target, api::GLuint index, const api::GLint* params)
{
	log() << warning << "glProgramEnvParameterI4ivNV not available." << std::endl;
}

static void fallbackProgramEnvParameterI4uiNV(api::GLenum target, api::GLuint index, api::GLuint x, api::GLuint y, api::GLuint z, api::GLuint w)
{
	log() << warning << "glProgramEnvParameterI4uiNV not available." << std::endl;
}

static void fallbackProgramEnvParameterI4uivNV(api::GLenum target, api::GLuint index, const api::GLuint* params)
{
	log() << warning << "glProgramEnvParameterI4uivNV not available." << std::endl;
}

static void fallbackProgramEnvParameters4fvEXT(api::GLenum target, api::GLuint index, api::GLsizei count, const api::GLfloat* params)
{
	log() << warning << "glProgramEnvParameters4fvEXT not available." << std::endl;
}

static void fallbackProgramEnvParametersI4ivNV(api::GLenum target, api::GLuint index, api::GLsizei count, const api::GLint* params)
{
	log() << warning << "glProgramEnvParametersI4ivNV not available." << std::endl;
}

static void fallbackProgramEnvParametersI4uivNV(api::GLenum target, api::GLuint index, api::GLsizei count, const api::GLuint* params)
{
	log() << warning << "glProgramEnvParametersI4uivNV not available." << std::endl;
}

static void fallbackProgramLocalParameter4dARB(api::GLenum target, api::GLuint index, api::GLdouble x, api::GLdouble y, api::GLdouble z, api::GLdouble w)
{
	log() << warning << "glProgramLocalParameter4dARB not available." << std::endl;
}

static void fallbackProgramLocalParameter4dvARB(api::GLenum target, api::GLuint index, const api::GLdouble* params)
{
	log() << warning << "glProgramLocalParameter4dvARB not available." << std::endl;
}

static void fallbackProgramLocalParameter4fARB(api::GLenum target, api::GLuint index, api::GLfloat x, api::GLfloat y, api::GLfloat z, api::GLfloat w)
{
	log() << warning << "glProgramLocalParameter4fARB not available." << std::endl;
}

static void fallbackProgramLocalParameter4fvARB(api::GLenum target, api::GLuint index, const api::GLfloat* params)
{
	log() << warning << "glProgramLocalParameter4fvARB not available." << std::endl;
}

static void fallbackProgramLocalParameterI4iNV(api::GLenum target, api::GLuint index, api::GLint x, api::GLint y, api::GLint z, api::GLint w)
{
	log() << warning << "glProgramLocalParameterI4iNV not available." << std::endl;
}

static void fallbackProgramLocalParameterI4ivNV(api::GLenum target, api::GLuint index, const api::GLint* params)
{
	log() << warning << "glProgramLocalParameterI4ivNV not available." << std::endl;
}

static void fallbackProgramLocalParameterI4uiNV(api::GLenum target, api::GLuint index, api::GLuint x, api::GLuint y, api::GLuint z, api::GLuint w)
{
	log() << warning << "glProgramLocalParameterI4uiNV not available." << std::endl;
}

static void fallbackProgramLocalParameterI4uivNV(api::GLenum target, api::GLuint index, const api::GLuint* params)
{
	log() << warning << "glProgramLocalParameterI4uivNV not available." << std::endl;
}

static void fallbackProgramLocalParameters4fvEXT(api::GLenum target, api::GLuint index, api::GLsizei count, const api::GLfloat* params)
{
	log() << warning << "glProgramLocalParameters4fvEXT not available." << std::endl;
}

static void fallbackProgramLocalParametersI4ivNV(api::GLenum target, api::GLuint index, api::GLsizei count, const api::GLint* params)
{
	log() << warning << "glProgramLocalParametersI4ivNV not available." << std::endl;
}

static void fallbackProgramLocalParametersI4uivNV(api::GLenum target, api::GLuint index, api::GLsizei count, const api::GLuint* params)
{
	log() << warning << "glProgramLocalParametersI4uivNV not available." << std::endl;
}

static void fallbackProgramNamedParameter4dNV(api::GLuint id, api::GLsizei len, const api::GLubyte* name, api::GLdouble x, api::GLdouble y, api::GLdouble z, api::GLdouble w)
{
	log() << warning << "glProgramNamedParameter4dNV not available." << std::endl;
}

static void fallbackProgramNamedParameter4dvNV(api::GLuint id, api::GLsizei len, const api::GLubyte* name, const api::GLdouble* v)
{
	log() << warning << "glProgramNamedParameter4dvNV not available." << std::endl;
}

static void fallbackProgramNamedParameter4fNV(api::GLuint id, api::GLsizei len, const api::GLubyte* name, api::GLfloat x, api::GLfloat y, api::GLfloat z, api::GLfloat w)
{
	log() << warning << "glProgramNamedParameter4fNV not available." << std::endl;
}

static void fallbackProgramNamedParameter4fvNV(api::GLuint id, api::GLsizei len, const api::GLubyte* name, const api::GLfloat* v)
{
	log() << warning << "glProgramNamedParameter4fvNV not available." << std::endl;
}

static void fallbackProgramParameter4dNV(api::GLenum target, api::GLuint index, api::GLdouble x, api::GLdouble y, api::GLdouble z, api::GLdouble w)
{
	log() << warning << "glProgramParameter4dNV not available." << std::endl;
}

static void fallbackProgramParameter4dvNV(api::GLenum target, api::GLuint index, const api::GLdouble* v)
{
	log() << warning << "glProgramParameter4dvNV not available." << std::endl;
}

static void fallbackProgramParameter4fNV(api::GLenum target, api::GLuint index, api::GLfloat x, api::GLfloat y, api::GLfloat z, api::GLfloat w)
{
	log() << warning << "glProgramParameter4fNV not available." << std::endl;
}

static void fallbackProgramParameter4fvNV(api::GLenum target, api::GLuint index, const api::GLfloat* v)
{
	log() << warning << "glProgramParameter4fvNV not available." << std::endl;
}

static void fallbackProgramParameteri(api::GLuint program, api::GLenum pname, api::GLint value)
{
	log() << warning << "glProgramParameteri not available." << std::endl;
}

static void fallbackProgramParameteriARB(api::GLuint program, api::GLenum pname, api::GLint value)
{
	log() << warning << "glProgramParameteriARB not available." << std::endl;
}

static void fallbackProgramParameteriEXT(api::GLuint program, api::GLenum pname, api::GLint value)
{
	log() << warning << "glProgramParameteriEXT not available." << std::endl;
}

static void fallbackProgramParameters4dvNV(api::GLenum target, api::GLuint index, api::GLuint count, const api::GLdouble* v)
{
	log() << warning << "glProgramParameters4dvNV not available." << std::endl;
}

static void fallbackProgramParameters4fvNV(api::GLenum target, api::GLuint index, api::GLuint count, const api::GLfloat* v)
{
	log() << warning << "glProgramParameters4fvNV not available." << std::endl;
}

static void fallbackProgramStringARB(api::GLenum target, api::GLenum format, api::GLsizei len, const api::GLvoid* string)
{
	log() << warning << "glProgramStringARB not available." << std::endl;
}

static void fallbackProgramUniform1dEXT(api::GLuint program, api::GLint location, api::GLdouble x)
{
	log() << warning << "glProgramUniform1dEXT not available." << std::endl;
}

static void fallbackProgramUniform1dvEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLdouble* value)
{
	log() << warning << "glProgramUniform1dvEXT not available." << std::endl;
}

static void fallbackProgramUniform1fEXT(api::GLuint program, api::GLint location, api::GLfloat v0)
{
	log() << warning << "glProgramUniform1fEXT not available." << std::endl;
}

static void fallbackProgramUniform1fvEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLfloat* value)
{
	log() << warning << "glProgramUniform1fvEXT not available." << std::endl;
}

static void fallbackProgramUniform1iEXT(api::GLuint program, api::GLint location, api::GLint v0)
{
	log() << warning << "glProgramUniform1iEXT not available." << std::endl;
}

static void fallbackProgramUniform1ivEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLint* value)
{
	log() << warning << "glProgramUniform1ivEXT not available." << std::endl;
}

static void fallbackProgramUniform1uiEXT(api::GLuint program, api::GLint location, api::GLuint v0)
{
	log() << warning << "glProgramUniform1uiEXT not available." << std::endl;
}

static void fallbackProgramUniform1uivEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLuint* value)
{
	log() << warning << "glProgramUniform1uivEXT not available." << std::endl;
}

static void fallbackProgramUniform2dEXT(api::GLuint program, api::GLint location, api::GLdouble x, api::GLdouble y)
{
	log() << warning << "glProgramUniform2dEXT not available." << std::endl;
}

static void fallbackProgramUniform2dvEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLdouble* value)
{
	log() << warning << "glProgramUniform2dvEXT not available." << std::endl;
}

static void fallbackProgramUniform2fEXT(api::GLuint program, api::GLint location, api::GLfloat v0, api::GLfloat v1)
{
	log() << warning << "glProgramUniform2fEXT not available." << std::endl;
}

static void fallbackProgramUniform2fvEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLfloat* value)
{
	log() << warning << "glProgramUniform2fvEXT not available." << std::endl;
}

static void fallbackProgramUniform2iEXT(api::GLuint program, api::GLint location, api::GLint v0, api::GLint v1)
{
	log() << warning << "glProgramUniform2iEXT not available." << std::endl;
}

static void fallbackProgramUniform2ivEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLint* value)
{
	log() << warning << "glProgramUniform2ivEXT not available." << std::endl;
}

static void fallbackProgramUniform2uiEXT(api::GLuint program, api::GLint location, api::GLuint v0, api::GLuint v1)
{
	log() << warning << "glProgramUniform2uiEXT not available." << std::endl;
}

static void fallbackProgramUniform2uivEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLuint* value)
{
	log() << warning << "glProgramUniform2uivEXT not available." << std::endl;
}

static void fallbackProgramUniform3dEXT(api::GLuint program, api::GLint location, api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "glProgramUniform3dEXT not available." << std::endl;
}

static void fallbackProgramUniform3dvEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLdouble* value)
{
	log() << warning << "glProgramUniform3dvEXT not available." << std::endl;
}

static void fallbackProgramUniform3fEXT(api::GLuint program, api::GLint location, api::GLfloat v0, api::GLfloat v1, api::GLfloat v2)
{
	log() << warning << "glProgramUniform3fEXT not available." << std::endl;
}

static void fallbackProgramUniform3fvEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLfloat* value)
{
	log() << warning << "glProgramUniform3fvEXT not available." << std::endl;
}

static void fallbackProgramUniform3iEXT(api::GLuint program, api::GLint location, api::GLint v0, api::GLint v1, api::GLint v2)
{
	log() << warning << "glProgramUniform3iEXT not available." << std::endl;
}

static void fallbackProgramUniform3ivEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLint* value)
{
	log() << warning << "glProgramUniform3ivEXT not available." << std::endl;
}

static void fallbackProgramUniform3uiEXT(api::GLuint program, api::GLint location, api::GLuint v0, api::GLuint v1, api::GLuint v2)
{
	log() << warning << "glProgramUniform3uiEXT not available." << std::endl;
}

static void fallbackProgramUniform3uivEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLuint* value)
{
	log() << warning << "glProgramUniform3uivEXT not available." << std::endl;
}

static void fallbackProgramUniform4dEXT(api::GLuint program, api::GLint location, api::GLdouble x, api::GLdouble y, api::GLdouble z, api::GLdouble w)
{
	log() << warning << "glProgramUniform4dEXT not available." << std::endl;
}

static void fallbackProgramUniform4dvEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLdouble* value)
{
	log() << warning << "glProgramUniform4dvEXT not available." << std::endl;
}

static void fallbackProgramUniform4fEXT(api::GLuint program, api::GLint location, api::GLfloat v0, api::GLfloat v1, api::GLfloat v2, api::GLfloat v3)
{
	log() << warning << "glProgramUniform4fEXT not available." << std::endl;
}

static void fallbackProgramUniform4fvEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLfloat* value)
{
	log() << warning << "glProgramUniform4fvEXT not available." << std::endl;
}

static void fallbackProgramUniform4iEXT(api::GLuint program, api::GLint location, api::GLint v0, api::GLint v1, api::GLint v2, api::GLint v3)
{
	log() << warning << "glProgramUniform4iEXT not available." << std::endl;
}

static void fallbackProgramUniform4ivEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLint* value)
{
	log() << warning << "glProgramUniform4ivEXT not available." << std::endl;
}

static void fallbackProgramUniform4uiEXT(api::GLuint program, api::GLint location, api::GLuint v0, api::GLuint v1, api::GLuint v2, api::GLuint v3)
{
	log() << warning << "glProgramUniform4uiEXT not available." << std::endl;
}

static void fallbackProgramUniform4uivEXT(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLuint* value)
{
	log() << warning << "glProgramUniform4uivEXT not available." << std::endl;
}

static void fallbackProgramUniformMatrix2dvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "glProgramUniformMatrix2dvEXT not available." << std::endl;
}

static void fallbackProgramUniformMatrix2fvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "glProgramUniformMatrix2fvEXT not available." << std::endl;
}

static void fallbackProgramUniformMatrix2x3dvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "glProgramUniformMatrix2x3dvEXT not available." << std::endl;
}

static void fallbackProgramUniformMatrix2x3fvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "glProgramUniformMatrix2x3fvEXT not available." << std::endl;
}

static void fallbackProgramUniformMatrix2x4dvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "glProgramUniformMatrix2x4dvEXT not available." << std::endl;
}

static void fallbackProgramUniformMatrix2x4fvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "glProgramUniformMatrix2x4fvEXT not available." << std::endl;
}

static void fallbackProgramUniformMatrix3dvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "glProgramUniformMatrix3dvEXT not available." << std::endl;
}

static void fallbackProgramUniformMatrix3fvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "glProgramUniformMatrix3fvEXT not available." << std::endl;
}

static void fallbackProgramUniformMatrix3x2dvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "glProgramUniformMatrix3x2dvEXT not available." << std::endl;
}

static void fallbackProgramUniformMatrix3x2fvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "glProgramUniformMatrix3x2fvEXT not available." << std::endl;
}

static void fallbackProgramUniformMatrix3x4dvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "glProgramUniformMatrix3x4dvEXT not available." << std::endl;
}

static void fallbackProgramUniformMatrix3x4fvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "glProgramUniformMatrix3x4fvEXT not available." << std::endl;
}

static void fallbackProgramUniformMatrix4dvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "glProgramUniformMatrix4dvEXT not available." << std::endl;
}

static void fallbackProgramUniformMatrix4fvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "glProgramUniformMatrix4fvEXT not available." << std::endl;
}

static void fallbackProgramUniformMatrix4x2dvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "glProgramUniformMatrix4x2dvEXT not available." << std::endl;
}

static void fallbackProgramUniformMatrix4x2fvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "glProgramUniformMatrix4x2fvEXT not available." << std::endl;
}

static void fallbackProgramUniformMatrix4x3dvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "glProgramUniformMatrix4x3dvEXT not available." << std::endl;
}

static void fallbackProgramUniformMatrix4x3fvEXT(api::GLuint program, api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "glProgramUniformMatrix4x3fvEXT not available." << std::endl;
}

static void fallbackProgramUniformui64NV(api::GLuint program, api::GLint location, api::GLuint64EXT value)
{
	log() << warning << "glProgramUniformui64NV not available." << std::endl;
}

static void fallbackProgramUniformui64vNV(api::GLuint program, api::GLint location, api::GLsizei count, const api::GLuint64EXT* value)
{
	log() << warning << "glProgramUniformui64vNV not available." << std::endl;
}

static void fallbackProgramVertexLimitNV(api::GLenum target, api::GLint limit)
{
	log() << warning << "glProgramVertexLimitNV not available." << std::endl;
}

static void fallbackProvokingVertex(api::GLenum mode)
{
	log() << warning << "glProvokingVertex not available." << std::endl;
}

static void fallbackProvokingVertexEXT(api::GLenum mode)
{
	log() << warning << "glProvokingVertexEXT not available." << std::endl;
}

static void fallbackPushAttrib(api::GLbitfield mask)
{
	log() << warning << "glPushAttrib not available." << std::endl;
}

static void fallbackPushClientAttrib(api::GLbitfield mask)
{
	log() << warning << "glPushClientAttrib not available." << std::endl;
}

static void fallbackPushClientAttribDefaultEXT(api::GLbitfield mask)
{
	log() << warning << "glPushClientAttribDefaultEXT not available." << std::endl;
}

static void fallbackPushMatrix()
{
	log() << warning << "glPushMatrix not available." << std::endl;
}

static void fallbackPushName(api::GLuint name)
{
	log() << warning << "glPushName not available." << std::endl;
}

static void fallbackQueryCounter(api::GLuint id, api::GLenum target)
{
	log() << warning << "glQueryCounter not available." << std::endl;
}

static void fallbackRasterPos2d(api::GLdouble x, api::GLdouble y)
{
	log() << warning << "glRasterPos2d not available." << std::endl;
}

static void fallbackRasterPos2dv(const api::GLdouble* v)
{
	log() << warning << "glRasterPos2dv not available." << std::endl;
}

static void fallbackRasterPos2f(api::GLfloat x, api::GLfloat y)
{
	log() << warning << "glRasterPos2f not available." << std::endl;
}

static void fallbackRasterPos2fv(const api::GLfloat* v)
{
	log() << warning << "glRasterPos2fv not available." << std::endl;
}

static void fallbackRasterPos2i(api::GLint x, api::GLint y)
{
	log() << warning << "glRasterPos2i not available." << std::endl;
}

static void fallbackRasterPos2iv(const api::GLint* v)
{
	log() << warning << "glRasterPos2iv not available." << std::endl;
}

static void fallbackRasterPos2s(api::GLshort x, api::GLshort y)
{
	log() << warning << "glRasterPos2s not available." << std::endl;
}

static void fallbackRasterPos2sv(const api::GLshort* v)
{
	log() << warning << "glRasterPos2sv not available." << std::endl;
}

static void fallbackRasterPos3d(api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "glRasterPos3d not available." << std::endl;
}

static void fallbackRasterPos3dv(const api::GLdouble* v)
{
	log() << warning << "glRasterPos3dv not available." << std::endl;
}

static void fallbackRasterPos3f(api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "glRasterPos3f not available." << std::endl;
}

static void fallbackRasterPos3fv(const api::GLfloat* v)
{
	log() << warning << "glRasterPos3fv not available." << std::endl;
}

static void fallbackRasterPos3i(api::GLint x, api::GLint y, api::GLint z)
{
	log() << warning << "glRasterPos3i not available." << std::endl;
}

static void fallbackRasterPos3iv(const api::GLint* v)
{
	log() << warning << "glRasterPos3iv not available." << std::endl;
}

static void fallbackRasterPos3s(api::GLshort x, api::GLshort y, api::GLshort z)
{
	log() << warning << "glRasterPos3s not available." << std::endl;
}

static void fallbackRasterPos3sv(const api::GLshort* v)
{
	log() << warning << "glRasterPos3sv not available." << std::endl;
}

static void fallbackRasterPos4d(api::GLdouble x, api::GLdouble y, api::GLdouble z, api::GLdouble w)
{
	log() << warning << "glRasterPos4d not available." << std::endl;
}

static void fallbackRasterPos4dv(const api::GLdouble* v)
{
	log() << warning << "glRasterPos4dv not available." << std::endl;
}

static void fallbackRasterPos4f(api::GLfloat x, api::GLfloat y, api::GLfloat z, api::GLfloat w)
{
	log() << warning << "glRasterPos4f not available." << std::endl;
}

static void fallbackRasterPos4fv(const api::GLfloat* v)
{
	log() << warning << "glRasterPos4fv not available." << std::endl;
}

static void fallbackRasterPos4i(api::GLint x, api::GLint y, api::GLint z, api::GLint w)
{
	log() << warning << "glRasterPos4i not available." << std::endl;
}

static void fallbackRasterPos4iv(const api::GLint* v)
{
	log() << warning << "glRasterPos4iv not available." << std::endl;
}

static void fallbackRasterPos4s(api::GLshort x, api::GLshort y, api::GLshort z, api::GLshort w)
{
	log() << warning << "glRasterPos4s not available." << std::endl;
}

static void fallbackRasterPos4sv(const api::GLshort* v)
{
	log() << warning << "glRasterPos4sv not available." << std::endl;
}

static void fallbackReadBuffer(api::GLenum mode)
{
	log() << warning << "glReadBuffer not available." << std::endl;
}

static void fallbackReadInstrumentsSGIX(api::GLint marker)
{
	log() << warning << "glReadInstrumentsSGIX not available." << std::endl;
}

static void fallbackReadPixels(api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height, api::GLenum format, api::GLenum type, api::GLvoid* pixels)
{
	log() << warning << "glReadPixels not available." << std::endl;
}

static void fallbackRectd(api::GLdouble x1, api::GLdouble y1, api::GLdouble x2, api::GLdouble y2)
{
	log() << warning << "glRectd not available." << std::endl;
}

static void fallbackRectdv(const api::GLdouble* v1, const api::GLdouble* v2)
{
	log() << warning << "glRectdv not available." << std::endl;
}

static void fallbackRectf(api::GLfloat x1, api::GLfloat y1, api::GLfloat x2, api::GLfloat y2)
{
	log() << warning << "glRectf not available." << std::endl;
}

static void fallbackRectfv(const api::GLfloat* v1, const api::GLfloat* v2)
{
	log() << warning << "glRectfv not available." << std::endl;
}

static void fallbackRecti(api::GLint x1, api::GLint y1, api::GLint x2, api::GLint y2)
{
	log() << warning << "glRecti not available." << std::endl;
}

static void fallbackRectiv(const api::GLint* v1, const api::GLint* v2)
{
	log() << warning << "glRectiv not available." << std::endl;
}

static void fallbackRects(api::GLshort x1, api::GLshort y1, api::GLshort x2, api::GLshort y2)
{
	log() << warning << "glRects not available." << std::endl;
}

static void fallbackRectsv(const api::GLshort* v1, const api::GLshort* v2)
{
	log() << warning << "glRectsv not available." << std::endl;
}

static void fallbackReferencePlaneSGIX(const api::GLdouble* equation)
{
	log() << warning << "glReferencePlaneSGIX not available." << std::endl;
}

static api::GLint fallbackRenderMode(api::GLenum mode)
{
	log() << warning << "glRenderMode not available." << std::endl;
}

static void fallbackRenderbufferStorage(api::GLenum target, api::GLenum internalformat, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "glRenderbufferStorage not available." << std::endl;
}

static void fallbackRenderbufferStorageEXT(api::GLenum target, api::GLenum internalformat, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "glRenderbufferStorageEXT not available." << std::endl;
}

static void fallbackRenderbufferStorageMultisample(api::GLenum target, api::GLsizei samples, api::GLenum internalformat, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "glRenderbufferStorageMultisample not available." << std::endl;
}

static void fallbackRenderbufferStorageMultisampleCoverageNV(api::GLenum target, api::GLsizei coverageSamples, api::GLsizei colorSamples, api::GLenum internalformat, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "glRenderbufferStorageMultisampleCoverageNV not available." << std::endl;
}

static void fallbackRenderbufferStorageMultisampleEXT(api::GLenum target, api::GLsizei samples, api::GLenum internalformat, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "glRenderbufferStorageMultisampleEXT not available." << std::endl;
}

static void fallbackReplacementCodePointerSUN(api::GLenum type, api::GLsizei stride, const api::GLvoid** pointer)
{
	log() << warning << "glReplacementCodePointerSUN not available." << std::endl;
}

static void fallbackReplacementCodeubSUN(api::GLubyte code)
{
	log() << warning << "glReplacementCodeubSUN not available." << std::endl;
}

static void fallbackReplacementCodeubvSUN(const api::GLubyte* code)
{
	log() << warning << "glReplacementCodeubvSUN not available." << std::endl;
}

static void fallbackReplacementCodeuiColor3fVertex3fSUN(api::GLuint rc, api::GLfloat r, api::GLfloat g, api::GLfloat b, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "glReplacementCodeuiColor3fVertex3fSUN not available." << std::endl;
}

static void fallbackReplacementCodeuiColor3fVertex3fvSUN(const api::GLuint* rc, const api::GLfloat* c, const api::GLfloat* v)
{
	log() << warning << "glReplacementCodeuiColor3fVertex3fvSUN not available." << std::endl;
}

static void fallbackReplacementCodeuiColor4fNormal3fVertex3fSUN(api::GLuint rc, api::GLfloat r, api::GLfloat g, api::GLfloat b, api::GLfloat a, api::GLfloat nx, api::GLfloat ny, api::GLfloat nz, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "glReplacementCodeuiColor4fNormal3fVertex3fSUN not available." << std::endl;
}

static void fallbackReplacementCodeuiColor4fNormal3fVertex3fvSUN(const api::GLuint* rc, const api::GLfloat* c, const api::GLfloat* n, const api::GLfloat* v)
{
	log() << warning << "glReplacementCodeuiColor4fNormal3fVertex3fvSUN not available." << std::endl;
}

static void fallbackReplacementCodeuiColor4ubVertex3fSUN(api::GLuint rc, api::GLubyte r, api::GLubyte g, api::GLubyte b, api::GLubyte a, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "glReplacementCodeuiColor4ubVertex3fSUN not available." << std::endl;
}

static void fallbackReplacementCodeuiColor4ubVertex3fvSUN(const api::GLuint* rc, const api::GLubyte* c, const api::GLfloat* v)
{
	log() << warning << "glReplacementCodeuiColor4ubVertex3fvSUN not available." << std::endl;
}

static void fallbackReplacementCodeuiNormal3fVertex3fSUN(api::GLuint rc, api::GLfloat nx, api::GLfloat ny, api::GLfloat nz, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "glReplacementCodeuiNormal3fVertex3fSUN not available." << std::endl;
}

static void fallbackReplacementCodeuiNormal3fVertex3fvSUN(const api::GLuint* rc, const api::GLfloat* n, const api::GLfloat* v)
{
	log() << warning << "glReplacementCodeuiNormal3fVertex3fvSUN not available." << std::endl;
}

static void fallbackReplacementCodeuiSUN(api::GLuint code)
{
	log() << warning << "glReplacementCodeuiSUN not available." << std::endl;
}

static void fallbackReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN(api::GLuint rc, api::GLfloat s, api::GLfloat t, api::GLfloat r, api::GLfloat g, api::GLfloat b, api::GLfloat a, api::GLfloat nx, api::GLfloat ny, api::GLfloat nz, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN not available." << std::endl;
}

static void fallbackReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN(const api::GLuint* rc, const api::GLfloat* tc, const api::GLfloat* c, const api::GLfloat* n, const api::GLfloat* v)
{
	log() << warning << "glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN not available." << std::endl;
}

static void fallbackReplacementCodeuiTexCoord2fNormal3fVertex3fSUN(api::GLuint rc, api::GLfloat s, api::GLfloat t, api::GLfloat nx, api::GLfloat ny, api::GLfloat nz, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "glReplacementCodeuiTexCoord2fNormal3fVertex3fSUN not available." << std::endl;
}

static void fallbackReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN(const api::GLuint* rc, const api::GLfloat* tc, const api::GLfloat* n, const api::GLfloat* v)
{
	log() << warning << "glReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN not available." << std::endl;
}

static void fallbackReplacementCodeuiTexCoord2fVertex3fSUN(api::GLuint rc, api::GLfloat s, api::GLfloat t, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "glReplacementCodeuiTexCoord2fVertex3fSUN not available." << std::endl;
}

static void fallbackReplacementCodeuiTexCoord2fVertex3fvSUN(const api::GLuint* rc, const api::GLfloat* tc, const api::GLfloat* v)
{
	log() << warning << "glReplacementCodeuiTexCoord2fVertex3fvSUN not available." << std::endl;
}

static void fallbackReplacementCodeuiVertex3fSUN(api::GLuint rc, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "glReplacementCodeuiVertex3fSUN not available." << std::endl;
}

static void fallbackReplacementCodeuiVertex3fvSUN(const api::GLuint* rc, const api::GLfloat* v)
{
	log() << warning << "glReplacementCodeuiVertex3fvSUN not available." << std::endl;
}

static void fallbackReplacementCodeuivSUN(const api::GLuint* code)
{
	log() << warning << "glReplacementCodeuivSUN not available." << std::endl;
}

static void fallbackReplacementCodeusSUN(api::GLushort code)
{
	log() << warning << "glReplacementCodeusSUN not available." << std::endl;
}

static void fallbackReplacementCodeusvSUN(const api::GLushort* code)
{
	log() << warning << "glReplacementCodeusvSUN not available." << std::endl;
}

static void fallbackRequestResidentProgramsNV(api::GLsizei n, const api::GLuint* programs)
{
	log() << warning << "glRequestResidentProgramsNV not available." << std::endl;
}

static void fallbackResetHistogram(api::GLenum target)
{
	log() << warning << "glResetHistogram not available." << std::endl;
}

static void fallbackResetHistogramEXT(api::GLenum target)
{
	log() << warning << "glResetHistogramEXT not available." << std::endl;
}

static void fallbackResetMinmax(api::GLenum target)
{
	log() << warning << "glResetMinmax not available." << std::endl;
}

static void fallbackResetMinmaxEXT(api::GLenum target)
{
	log() << warning << "glResetMinmaxEXT not available." << std::endl;
}

static void fallbackResizeBuffersMESA()
{
	log() << warning << "glResizeBuffersMESA not available." << std::endl;
}

static void fallbackResumeTransformFeedback()
{
	log() << warning << "glResumeTransformFeedback not available." << std::endl;
}

static void fallbackResumeTransformFeedbackNV()
{
	log() << warning << "glResumeTransformFeedbackNV not available." << std::endl;
}

static void fallbackRotated(api::GLdouble angle, api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "glRotated not available." << std::endl;
}

static void fallbackRotatef(api::GLfloat angle, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "glRotatef not available." << std::endl;
}

static void fallbackSampleCoverage(api::GLclampf value, api::GLboolean invert)
{
	log() << warning << "glSampleCoverage not available." << std::endl;
}

static void fallbackSampleCoverageARB(api::GLclampf value, api::GLboolean invert)
{
	log() << warning << "glSampleCoverageARB not available." << std::endl;
}

static void fallbackSampleMapATI(api::GLuint dst, api::GLuint interp, api::GLenum swizzle)
{
	log() << warning << "glSampleMapATI not available." << std::endl;
}

static void fallbackSampleMaskEXT(api::GLclampf value, api::GLboolean invert)
{
	log() << warning << "glSampleMaskEXT not available." << std::endl;
}

static void fallbackSampleMaskIndexedNV(api::GLuint index, api::GLbitfield mask)
{
	log() << warning << "glSampleMaskIndexedNV not available." << std::endl;
}

static void fallbackSampleMaskSGIS(api::GLclampf value, api::GLboolean invert)
{
	log() << warning << "glSampleMaskSGIS not available." << std::endl;
}

static void fallbackSampleMaski(api::GLuint index, api::GLbitfield mask)
{
	log() << warning << "glSampleMaski not available." << std::endl;
}

static void fallbackSamplePatternEXT(api::GLenum pattern)
{
	log() << warning << "glSamplePatternEXT not available." << std::endl;
}

static void fallbackSamplePatternSGIS(api::GLenum pattern)
{
	log() << warning << "glSamplePatternSGIS not available." << std::endl;
}

static void fallbackSamplerParameterIiv(api::GLuint sampler, api::GLenum pname, const api::GLint* param)
{
	log() << warning << "glSamplerParameterIiv not available." << std::endl;
}

static void fallbackSamplerParameterIuiv(api::GLuint sampler, api::GLenum pname, const api::GLuint* param)
{
	log() << warning << "glSamplerParameterIuiv not available." << std::endl;
}

static void fallbackSamplerParameterf(api::GLuint sampler, api::GLenum pname, api::GLfloat param)
{
	log() << warning << "glSamplerParameterf not available." << std::endl;
}

static void fallbackSamplerParameterfv(api::GLuint sampler, api::GLenum pname, const api::GLfloat* param)
{
	log() << warning << "glSamplerParameterfv not available." << std::endl;
}

static void fallbackSamplerParameteri(api::GLuint sampler, api::GLenum pname, api::GLint param)
{
	log() << warning << "glSamplerParameteri not available." << std::endl;
}

static void fallbackSamplerParameteriv(api::GLuint sampler, api::GLenum pname, const api::GLint* param)
{
	log() << warning << "glSamplerParameteriv not available." << std::endl;
}

static void fallbackScaled(api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "glScaled not available." << std::endl;
}

static void fallbackScalef(api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "glScalef not available." << std::endl;
}

static void fallbackScissor(api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "glScissor not available." << std::endl;
}

static void fallbackSecondaryColor3b(api::GLbyte red, api::GLbyte green, api::GLbyte blue)
{
	log() << warning << "glSecondaryColor3b not available." << std::endl;
}

static void fallbackSecondaryColor3bEXT(api::GLbyte red, api::GLbyte green, api::GLbyte blue)
{
	log() << warning << "glSecondaryColor3bEXT not available." << std::endl;
}

static void fallbackSecondaryColor3bv(const api::GLbyte* v)
{
	log() << warning << "glSecondaryColor3bv not available." << std::endl;
}

static void fallbackSecondaryColor3bvEXT(const api::GLbyte* v)
{
	log() << warning << "glSecondaryColor3bvEXT not available." << std::endl;
}

static void fallbackSecondaryColor3d(api::GLdouble red, api::GLdouble green, api::GLdouble blue)
{
	log() << warning << "glSecondaryColor3d not available." << std::endl;
}

static void fallbackSecondaryColor3dEXT(api::GLdouble red, api::GLdouble green, api::GLdouble blue)
{
	log() << warning << "glSecondaryColor3dEXT not available." << std::endl;
}

static void fallbackSecondaryColor3dv(const api::GLdouble* v)
{
	log() << warning << "glSecondaryColor3dv not available." << std::endl;
}

static void fallbackSecondaryColor3dvEXT(const api::GLdouble* v)
{
	log() << warning << "glSecondaryColor3dvEXT not available." << std::endl;
}

static void fallbackSecondaryColor3f(api::GLfloat red, api::GLfloat green, api::GLfloat blue)
{
	log() << warning << "glSecondaryColor3f not available." << std::endl;
}

static void fallbackSecondaryColor3fEXT(api::GLfloat red, api::GLfloat green, api::GLfloat blue)
{
	log() << warning << "glSecondaryColor3fEXT not available." << std::endl;
}

static void fallbackSecondaryColor3fv(const api::GLfloat* v)
{
	log() << warning << "glSecondaryColor3fv not available." << std::endl;
}

static void fallbackSecondaryColor3fvEXT(const api::GLfloat* v)
{
	log() << warning << "glSecondaryColor3fvEXT not available." << std::endl;
}

static void fallbackSecondaryColor3hNV(api::GLhalfNV red, api::GLhalfNV green, api::GLhalfNV blue)
{
	log() << warning << "glSecondaryColor3hNV not available." << std::endl;
}

static void fallbackSecondaryColor3hvNV(const api::GLhalfNV* v)
{
	log() << warning << "glSecondaryColor3hvNV not available." << std::endl;
}

static void fallbackSecondaryColor3i(api::GLint red, api::GLint green, api::GLint blue)
{
	log() << warning << "glSecondaryColor3i not available." << std::endl;
}

static void fallbackSecondaryColor3iEXT(api::GLint red, api::GLint green, api::GLint blue)
{
	log() << warning << "glSecondaryColor3iEXT not available." << std::endl;
}

static void fallbackSecondaryColor3iv(const api::GLint* v)
{
	log() << warning << "glSecondaryColor3iv not available." << std::endl;
}

static void fallbackSecondaryColor3ivEXT(const api::GLint* v)
{
	log() << warning << "glSecondaryColor3ivEXT not available." << std::endl;
}

static void fallbackSecondaryColor3s(api::GLshort red, api::GLshort green, api::GLshort blue)
{
	log() << warning << "glSecondaryColor3s not available." << std::endl;
}

static void fallbackSecondaryColor3sEXT(api::GLshort red, api::GLshort green, api::GLshort blue)
{
	log() << warning << "glSecondaryColor3sEXT not available." << std::endl;
}

static void fallbackSecondaryColor3sv(const api::GLshort* v)
{
	log() << warning << "glSecondaryColor3sv not available." << std::endl;
}

static void fallbackSecondaryColor3svEXT(const api::GLshort* v)
{
	log() << warning << "glSecondaryColor3svEXT not available." << std::endl;
}

static void fallbackSecondaryColor3ub(api::GLubyte red, api::GLubyte green, api::GLubyte blue)
{
	log() << warning << "glSecondaryColor3ub not available." << std::endl;
}

static void fallbackSecondaryColor3ubEXT(api::GLubyte red, api::GLubyte green, api::GLubyte blue)
{
	log() << warning << "glSecondaryColor3ubEXT not available." << std::endl;
}

static void fallbackSecondaryColor3ubv(const api::GLubyte* v)
{
	log() << warning << "glSecondaryColor3ubv not available." << std::endl;
}

static void fallbackSecondaryColor3ubvEXT(const api::GLubyte* v)
{
	log() << warning << "glSecondaryColor3ubvEXT not available." << std::endl;
}

static void fallbackSecondaryColor3ui(api::GLuint red, api::GLuint green, api::GLuint blue)
{
	log() << warning << "glSecondaryColor3ui not available." << std::endl;
}

static void fallbackSecondaryColor3uiEXT(api::GLuint red, api::GLuint green, api::GLuint blue)
{
	log() << warning << "glSecondaryColor3uiEXT not available." << std::endl;
}

static void fallbackSecondaryColor3uiv(const api::GLuint* v)
{
	log() << warning << "glSecondaryColor3uiv not available." << std::endl;
}

static void fallbackSecondaryColor3uivEXT(const api::GLuint* v)
{
	log() << warning << "glSecondaryColor3uivEXT not available." << std::endl;
}

static void fallbackSecondaryColor3us(api::GLushort red, api::GLushort green, api::GLushort blue)
{
	log() << warning << "glSecondaryColor3us not available." << std::endl;
}

static void fallbackSecondaryColor3usEXT(api::GLushort red, api::GLushort green, api::GLushort blue)
{
	log() << warning << "glSecondaryColor3usEXT not available." << std::endl;
}

static void fallbackSecondaryColor3usv(const api::GLushort* v)
{
	log() << warning << "glSecondaryColor3usv not available." << std::endl;
}

static void fallbackSecondaryColor3usvEXT(const api::GLushort* v)
{
	log() << warning << "glSecondaryColor3usvEXT not available." << std::endl;
}

static void fallbackSecondaryColorFormatNV(api::GLint size, api::GLenum type, api::GLsizei stride)
{
	log() << warning << "glSecondaryColorFormatNV not available." << std::endl;
}

static void fallbackSecondaryColorP3ui(api::GLenum type, api::GLuint color)
{
	log() << warning << "glSecondaryColorP3ui not available." << std::endl;
}

static void fallbackSecondaryColorP3uiv(api::GLenum type, const api::GLuint* color)
{
	log() << warning << "glSecondaryColorP3uiv not available." << std::endl;
}

static void fallbackSecondaryColorPointer(api::GLint size, api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "glSecondaryColorPointer not available." << std::endl;
}

static void fallbackSecondaryColorPointerEXT(api::GLint size, api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "glSecondaryColorPointerEXT not available." << std::endl;
}

static void fallbackSecondaryColorPointerListIBM(api::GLint size, api::GLenum type, api::GLint stride, const api::GLvoid** pointer, api::GLint ptrstride)
{
	log() << warning << "glSecondaryColorPointerListIBM not available." << std::endl;
}

static void fallbackSelectBuffer(api::GLsizei size, api::GLuint* buffer)
{
	log() << warning << "glSelectBuffer not available." << std::endl;
}

static void fallbackSelectPerfMonitorCountersAMD(api::GLuint monitor, api::GLboolean enable, api::GLuint group, api::GLint numCounters, api::GLuint* counterList)
{
	log() << warning << "glSelectPerfMonitorCountersAMD not available." << std::endl;
}

static void fallbackSeparableFilter2D(api::GLenum target, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLenum format, api::GLenum type, const api::GLvoid* row, const api::GLvoid* column)
{
	log() << warning << "glSeparableFilter2D not available." << std::endl;
}

static void fallbackSeparableFilter2DEXT(api::GLenum target, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLenum format, api::GLenum type, const api::GLvoid* row, const api::GLvoid* column)
{
	log() << warning << "glSeparableFilter2DEXT not available." << std::endl;
}

static void fallbackSetFenceAPPLE(api::GLuint fence)
{
	log() << warning << "glSetFenceAPPLE not available." << std::endl;
}

static void fallbackSetFenceNV(api::GLuint fence, api::GLenum condition)
{
	log() << warning << "glSetFenceNV not available." << std::endl;
}

static void fallbackSetFragmentShaderConstantATI(api::GLuint dst, const api::GLfloat* value)
{
	log() << warning << "glSetFragmentShaderConstantATI not available." << std::endl;
}

static void fallbackSetInvariantEXT(api::GLuint id, api::GLenum type, const api::GLvoid* addr)
{
	log() << warning << "glSetInvariantEXT not available." << std::endl;
}

static void fallbackSetLocalConstantEXT(api::GLuint id, api::GLenum type, const api::GLvoid* addr)
{
	log() << warning << "glSetLocalConstantEXT not available." << std::endl;
}

static void fallbackShadeModel(api::GLenum mode)
{
	log() << warning << "glShadeModel not available." << std::endl;
}

static void fallbackShaderOp1EXT(api::GLenum op, api::GLuint res, api::GLuint arg1)
{
	log() << warning << "glShaderOp1EXT not available." << std::endl;
}

static void fallbackShaderOp2EXT(api::GLenum op, api::GLuint res, api::GLuint arg1, api::GLuint arg2)
{
	log() << warning << "glShaderOp2EXT not available." << std::endl;
}

static void fallbackShaderOp3EXT(api::GLenum op, api::GLuint res, api::GLuint arg1, api::GLuint arg2, api::GLuint arg3)
{
	log() << warning << "glShaderOp3EXT not available." << std::endl;
}

static void fallbackShaderSource(api::GLuint shader, api::GLsizei count, const api::GLchar** string, const api::GLint* length)
{
	log() << warning << "glShaderSource not available." << std::endl;
}

static void fallbackShaderSourceARB(api::GLhandleARB shaderObj, api::GLsizei count, const api::GLcharARB** string, const api::GLint* length)
{
	log() << warning << "glShaderSourceARB not available." << std::endl;
}

static void fallbackSharpenTexFuncSGIS(api::GLenum target, api::GLsizei n, const api::GLfloat* points)
{
	log() << warning << "glSharpenTexFuncSGIS not available." << std::endl;
}

static void fallbackSpriteParameterfSGIX(api::GLenum pname, api::GLfloat param)
{
	log() << warning << "glSpriteParameterfSGIX not available." << std::endl;
}

static void fallbackSpriteParameterfvSGIX(api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "glSpriteParameterfvSGIX not available." << std::endl;
}

static void fallbackSpriteParameteriSGIX(api::GLenum pname, api::GLint param)
{
	log() << warning << "glSpriteParameteriSGIX not available." << std::endl;
}

static void fallbackSpriteParameterivSGIX(api::GLenum pname, const api::GLint* params)
{
	log() << warning << "glSpriteParameterivSGIX not available." << std::endl;
}

static void fallbackStartInstrumentsSGIX()
{
	log() << warning << "glStartInstrumentsSGIX not available." << std::endl;
}

static void fallbackStencilClearTagEXT(api::GLsizei stencilTagBits, api::GLuint stencilClearTag)
{
	log() << warning << "glStencilClearTagEXT not available." << std::endl;
}

static void fallbackStencilFunc(api::GLenum func, api::GLint ref, api::GLuint mask)
{
	log() << warning << "glStencilFunc not available." << std::endl;
}

static void fallbackStencilFuncSeparate(api::GLenum frontfunc, api::GLenum backfunc, api::GLint ref, api::GLuint mask)
{
	log() << warning << "glStencilFuncSeparate not available." << std::endl;
}

static void fallbackStencilFuncSeparateATI(api::GLenum frontfunc, api::GLenum backfunc, api::GLint ref, api::GLuint mask)
{
	log() << warning << "glStencilFuncSeparateATI not available." << std::endl;
}

static void fallbackStencilMask(api::GLuint mask)
{
	log() << warning << "glStencilMask not available." << std::endl;
}

static void fallbackStencilMaskSeparate(api::GLenum face, api::GLuint mask)
{
	log() << warning << "glStencilMaskSeparate not available." << std::endl;
}

static void fallbackStencilOp(api::GLenum fail, api::GLenum zfail, api::GLenum zpass)
{
	log() << warning << "glStencilOp not available." << std::endl;
}

static void fallbackStencilOpSeparate(api::GLenum face, api::GLenum sfail, api::GLenum dpfail, api::GLenum dppass)
{
	log() << warning << "glStencilOpSeparate not available." << std::endl;
}

static void fallbackStencilOpSeparateATI(api::GLenum face, api::GLenum sfail, api::GLenum dpfail, api::GLenum dppass)
{
	log() << warning << "glStencilOpSeparateATI not available." << std::endl;
}

static void fallbackStopInstrumentsSGIX(api::GLint marker)
{
	log() << warning << "glStopInstrumentsSGIX not available." << std::endl;
}

static void fallbackStringMarkerGREMEDY(api::GLsizei len, const api::GLvoid* string)
{
	log() << warning << "glStringMarkerGREMEDY not available." << std::endl;
}

static void fallbackSwizzleEXT(api::GLuint res, api::GLuint in, api::GLenum outX, api::GLenum outY, api::GLenum outZ, api::GLenum outW)
{
	log() << warning << "glSwizzleEXT not available." << std::endl;
}

static void fallbackTagSampleBufferSGIX()
{
	log() << warning << "glTagSampleBufferSGIX not available." << std::endl;
}

static void fallbackTangent3bEXT(api::GLbyte tx, api::GLbyte ty, api::GLbyte tz)
{
	log() << warning << "glTangent3bEXT not available." << std::endl;
}

static void fallbackTangent3bvEXT(const api::GLbyte* v)
{
	log() << warning << "glTangent3bvEXT not available." << std::endl;
}

static void fallbackTangent3dEXT(api::GLdouble tx, api::GLdouble ty, api::GLdouble tz)
{
	log() << warning << "glTangent3dEXT not available." << std::endl;
}

static void fallbackTangent3dvEXT(const api::GLdouble* v)
{
	log() << warning << "glTangent3dvEXT not available." << std::endl;
}

static void fallbackTangent3fEXT(api::GLfloat tx, api::GLfloat ty, api::GLfloat tz)
{
	log() << warning << "glTangent3fEXT not available." << std::endl;
}

static void fallbackTangent3fvEXT(const api::GLfloat* v)
{
	log() << warning << "glTangent3fvEXT not available." << std::endl;
}

static void fallbackTangent3iEXT(api::GLint tx, api::GLint ty, api::GLint tz)
{
	log() << warning << "glTangent3iEXT not available." << std::endl;
}

static void fallbackTangent3ivEXT(const api::GLint* v)
{
	log() << warning << "glTangent3ivEXT not available." << std::endl;
}

static void fallbackTangent3sEXT(api::GLshort tx, api::GLshort ty, api::GLshort tz)
{
	log() << warning << "glTangent3sEXT not available." << std::endl;
}

static void fallbackTangent3svEXT(const api::GLshort* v)
{
	log() << warning << "glTangent3svEXT not available." << std::endl;
}

static void fallbackTangentPointerEXT(api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "glTangentPointerEXT not available." << std::endl;
}

static void fallbackTbufferMask3DFX(api::GLuint mask)
{
	log() << warning << "glTbufferMask3DFX not available." << std::endl;
}

static void fallbackTessellationFactorAMD(api::GLfloat factor)
{
	log() << warning << "glTessellationFactorAMD not available." << std::endl;
}

static void fallbackTessellationModeAMD(api::GLenum mode)
{
	log() << warning << "glTessellationModeAMD not available." << std::endl;
}

static api::GLboolean fallbackTestFenceAPPLE(api::GLuint fence)
{
	log() << warning << "glTestFenceAPPLE not available." << std::endl;
}

static api::GLboolean fallbackTestFenceNV(api::GLuint fence)
{
	log() << warning << "glTestFenceNV not available." << std::endl;
}

static api::GLboolean fallbackTestObjectAPPLE(api::GLenum object, api::GLuint name)
{
	log() << warning << "glTestObjectAPPLE not available." << std::endl;
}

static void fallbackTexBuffer(api::GLenum target, api::GLenum internalformat, api::GLuint buffer)
{
	log() << warning << "glTexBuffer not available." << std::endl;
}

static void fallbackTexBufferARB(api::GLenum target, api::GLenum internalformat, api::GLuint buffer)
{
	log() << warning << "glTexBufferARB not available." << std::endl;
}

static void fallbackTexBufferEXT(api::GLenum target, api::GLenum internalformat, api::GLuint buffer)
{
	log() << warning << "glTexBufferEXT not available." << std::endl;
}

static void fallbackTexBumpParameterfvATI(api::GLenum pname, const api::GLfloat* param)
{
	log() << warning << "glTexBumpParameterfvATI not available." << std::endl;
}

static void fallbackTexBumpParameterivATI(api::GLenum pname, const api::GLint* param)
{
	log() << warning << "glTexBumpParameterivATI not available." << std::endl;
}

static void fallbackTexCoord1d(api::GLdouble s)
{
	log() << warning << "glTexCoord1d not available." << std::endl;
}

static void fallbackTexCoord1dv(const api::GLdouble* v)
{
	log() << warning << "glTexCoord1dv not available." << std::endl;
}

static void fallbackTexCoord1f(api::GLfloat s)
{
	log() << warning << "glTexCoord1f not available." << std::endl;
}

static void fallbackTexCoord1fv(const api::GLfloat* v)
{
	log() << warning << "glTexCoord1fv not available." << std::endl;
}

static void fallbackTexCoord1hNV(api::GLhalfNV s)
{
	log() << warning << "glTexCoord1hNV not available." << std::endl;
}

static void fallbackTexCoord1hvNV(const api::GLhalfNV* v)
{
	log() << warning << "glTexCoord1hvNV not available." << std::endl;
}

static void fallbackTexCoord1i(api::GLint s)
{
	log() << warning << "glTexCoord1i not available." << std::endl;
}

static void fallbackTexCoord1iv(const api::GLint* v)
{
	log() << warning << "glTexCoord1iv not available." << std::endl;
}

static void fallbackTexCoord1s(api::GLshort s)
{
	log() << warning << "glTexCoord1s not available." << std::endl;
}

static void fallbackTexCoord1sv(const api::GLshort* v)
{
	log() << warning << "glTexCoord1sv not available." << std::endl;
}

static void fallbackTexCoord2d(api::GLdouble s, api::GLdouble t)
{
	log() << warning << "glTexCoord2d not available." << std::endl;
}

static void fallbackTexCoord2dv(const api::GLdouble* v)
{
	log() << warning << "glTexCoord2dv not available." << std::endl;
}

static void fallbackTexCoord2f(api::GLfloat s, api::GLfloat t)
{
	log() << warning << "glTexCoord2f not available." << std::endl;
}

static void fallbackTexCoord2fColor3fVertex3fSUN(api::GLfloat s, api::GLfloat t, api::GLfloat r, api::GLfloat g, api::GLfloat b, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "glTexCoord2fColor3fVertex3fSUN not available." << std::endl;
}

static void fallbackTexCoord2fColor3fVertex3fvSUN(const api::GLfloat* tc, const api::GLfloat* c, const api::GLfloat* v)
{
	log() << warning << "glTexCoord2fColor3fVertex3fvSUN not available." << std::endl;
}

static void fallbackTexCoord2fColor4fNormal3fVertex3fSUN(api::GLfloat s, api::GLfloat t, api::GLfloat r, api::GLfloat g, api::GLfloat b, api::GLfloat a, api::GLfloat nx, api::GLfloat ny, api::GLfloat nz, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "glTexCoord2fColor4fNormal3fVertex3fSUN not available." << std::endl;
}

static void fallbackTexCoord2fColor4fNormal3fVertex3fvSUN(const api::GLfloat* tc, const api::GLfloat* c, const api::GLfloat* n, const api::GLfloat* v)
{
	log() << warning << "glTexCoord2fColor4fNormal3fVertex3fvSUN not available." << std::endl;
}

static void fallbackTexCoord2fColor4ubVertex3fSUN(api::GLfloat s, api::GLfloat t, api::GLubyte r, api::GLubyte g, api::GLubyte b, api::GLubyte a, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "glTexCoord2fColor4ubVertex3fSUN not available." << std::endl;
}

static void fallbackTexCoord2fColor4ubVertex3fvSUN(const api::GLfloat* tc, const api::GLubyte* c, const api::GLfloat* v)
{
	log() << warning << "glTexCoord2fColor4ubVertex3fvSUN not available." << std::endl;
}

static void fallbackTexCoord2fNormal3fVertex3fSUN(api::GLfloat s, api::GLfloat t, api::GLfloat nx, api::GLfloat ny, api::GLfloat nz, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "glTexCoord2fNormal3fVertex3fSUN not available." << std::endl;
}

static void fallbackTexCoord2fNormal3fVertex3fvSUN(const api::GLfloat* tc, const api::GLfloat* n, const api::GLfloat* v)
{
	log() << warning << "glTexCoord2fNormal3fVertex3fvSUN not available." << std::endl;
}

static void fallbackTexCoord2fVertex3fSUN(api::GLfloat s, api::GLfloat t, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "glTexCoord2fVertex3fSUN not available." << std::endl;
}

static void fallbackTexCoord2fVertex3fvSUN(const api::GLfloat* tc, const api::GLfloat* v)
{
	log() << warning << "glTexCoord2fVertex3fvSUN not available." << std::endl;
}

static void fallbackTexCoord2fv(const api::GLfloat* v)
{
	log() << warning << "glTexCoord2fv not available." << std::endl;
}

static void fallbackTexCoord2hNV(api::GLhalfNV s, api::GLhalfNV t)
{
	log() << warning << "glTexCoord2hNV not available." << std::endl;
}

static void fallbackTexCoord2hvNV(const api::GLhalfNV* v)
{
	log() << warning << "glTexCoord2hvNV not available." << std::endl;
}

static void fallbackTexCoord2i(api::GLint s, api::GLint t)
{
	log() << warning << "glTexCoord2i not available." << std::endl;
}

static void fallbackTexCoord2iv(const api::GLint* v)
{
	log() << warning << "glTexCoord2iv not available." << std::endl;
}

static void fallbackTexCoord2s(api::GLshort s, api::GLshort t)
{
	log() << warning << "glTexCoord2s not available." << std::endl;
}

static void fallbackTexCoord2sv(const api::GLshort* v)
{
	log() << warning << "glTexCoord2sv not available." << std::endl;
}

static void fallbackTexCoord3d(api::GLdouble s, api::GLdouble t, api::GLdouble r)
{
	log() << warning << "glTexCoord3d not available." << std::endl;
}

static void fallbackTexCoord3dv(const api::GLdouble* v)
{
	log() << warning << "glTexCoord3dv not available." << std::endl;
}

static void fallbackTexCoord3f(api::GLfloat s, api::GLfloat t, api::GLfloat r)
{
	log() << warning << "glTexCoord3f not available." << std::endl;
}

static void fallbackTexCoord3fv(const api::GLfloat* v)
{
	log() << warning << "glTexCoord3fv not available." << std::endl;
}

static void fallbackTexCoord3hNV(api::GLhalfNV s, api::GLhalfNV t, api::GLhalfNV r)
{
	log() << warning << "glTexCoord3hNV not available." << std::endl;
}

static void fallbackTexCoord3hvNV(const api::GLhalfNV* v)
{
	log() << warning << "glTexCoord3hvNV not available." << std::endl;
}

static void fallbackTexCoord3i(api::GLint s, api::GLint t, api::GLint r)
{
	log() << warning << "glTexCoord3i not available." << std::endl;
}

static void fallbackTexCoord3iv(const api::GLint* v)
{
	log() << warning << "glTexCoord3iv not available." << std::endl;
}

static void fallbackTexCoord3s(api::GLshort s, api::GLshort t, api::GLshort r)
{
	log() << warning << "glTexCoord3s not available." << std::endl;
}

static void fallbackTexCoord3sv(const api::GLshort* v)
{
	log() << warning << "glTexCoord3sv not available." << std::endl;
}

static void fallbackTexCoord4d(api::GLdouble s, api::GLdouble t, api::GLdouble r, api::GLdouble q)
{
	log() << warning << "glTexCoord4d not available." << std::endl;
}

static void fallbackTexCoord4dv(const api::GLdouble* v)
{
	log() << warning << "glTexCoord4dv not available." << std::endl;
}

static void fallbackTexCoord4f(api::GLfloat s, api::GLfloat t, api::GLfloat r, api::GLfloat q)
{
	log() << warning << "glTexCoord4f not available." << std::endl;
}

static void fallbackTexCoord4fColor4fNormal3fVertex4fSUN(api::GLfloat s, api::GLfloat t, api::GLfloat p, api::GLfloat q, api::GLfloat r, api::GLfloat g, api::GLfloat b, api::GLfloat a, api::GLfloat nx, api::GLfloat ny, api::GLfloat nz, api::GLfloat x, api::GLfloat y, api::GLfloat z, api::GLfloat w)
{
	log() << warning << "glTexCoord4fColor4fNormal3fVertex4fSUN not available." << std::endl;
}

static void fallbackTexCoord4fColor4fNormal3fVertex4fvSUN(const api::GLfloat* tc, const api::GLfloat* c, const api::GLfloat* n, const api::GLfloat* v)
{
	log() << warning << "glTexCoord4fColor4fNormal3fVertex4fvSUN not available." << std::endl;
}

static void fallbackTexCoord4fVertex4fSUN(api::GLfloat s, api::GLfloat t, api::GLfloat p, api::GLfloat q, api::GLfloat x, api::GLfloat y, api::GLfloat z, api::GLfloat w)
{
	log() << warning << "glTexCoord4fVertex4fSUN not available." << std::endl;
}

static void fallbackTexCoord4fVertex4fvSUN(const api::GLfloat* tc, const api::GLfloat* v)
{
	log() << warning << "glTexCoord4fVertex4fvSUN not available." << std::endl;
}

static void fallbackTexCoord4fv(const api::GLfloat* v)
{
	log() << warning << "glTexCoord4fv not available." << std::endl;
}

static void fallbackTexCoord4hNV(api::GLhalfNV s, api::GLhalfNV t, api::GLhalfNV r, api::GLhalfNV q)
{
	log() << warning << "glTexCoord4hNV not available." << std::endl;
}

static void fallbackTexCoord4hvNV(const api::GLhalfNV* v)
{
	log() << warning << "glTexCoord4hvNV not available." << std::endl;
}

static void fallbackTexCoord4i(api::GLint s, api::GLint t, api::GLint r, api::GLint q)
{
	log() << warning << "glTexCoord4i not available." << std::endl;
}

static void fallbackTexCoord4iv(const api::GLint* v)
{
	log() << warning << "glTexCoord4iv not available." << std::endl;
}

static void fallbackTexCoord4s(api::GLshort s, api::GLshort t, api::GLshort r, api::GLshort q)
{
	log() << warning << "glTexCoord4s not available." << std::endl;
}

static void fallbackTexCoord4sv(const api::GLshort* v)
{
	log() << warning << "glTexCoord4sv not available." << std::endl;
}

static void fallbackTexCoordFormatNV(api::GLint size, api::GLenum type, api::GLsizei stride)
{
	log() << warning << "glTexCoordFormatNV not available." << std::endl;
}

static void fallbackTexCoordP1ui(api::GLenum type, api::GLuint coords)
{
	log() << warning << "glTexCoordP1ui not available." << std::endl;
}

static void fallbackTexCoordP1uiv(api::GLenum type, const api::GLuint* coords)
{
	log() << warning << "glTexCoordP1uiv not available." << std::endl;
}

static void fallbackTexCoordP2ui(api::GLenum type, api::GLuint coords)
{
	log() << warning << "glTexCoordP2ui not available." << std::endl;
}

static void fallbackTexCoordP2uiv(api::GLenum type, const api::GLuint* coords)
{
	log() << warning << "glTexCoordP2uiv not available." << std::endl;
}

static void fallbackTexCoordP3ui(api::GLenum type, api::GLuint coords)
{
	log() << warning << "glTexCoordP3ui not available." << std::endl;
}

static void fallbackTexCoordP3uiv(api::GLenum type, const api::GLuint* coords)
{
	log() << warning << "glTexCoordP3uiv not available." << std::endl;
}

static void fallbackTexCoordP4ui(api::GLenum type, api::GLuint coords)
{
	log() << warning << "glTexCoordP4ui not available." << std::endl;
}

static void fallbackTexCoordP4uiv(api::GLenum type, const api::GLuint* coords)
{
	log() << warning << "glTexCoordP4uiv not available." << std::endl;
}

static void fallbackTexCoordPointer(api::GLint size, api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "glTexCoordPointer not available." << std::endl;
}

static void fallbackTexCoordPointerEXT(api::GLint size, api::GLenum type, api::GLsizei stride, api::GLsizei count, const api::GLvoid* pointer)
{
	log() << warning << "glTexCoordPointerEXT not available." << std::endl;
}

static void fallbackTexCoordPointerListIBM(api::GLint size, api::GLenum type, api::GLint stride, const api::GLvoid** pointer, api::GLint ptrstride)
{
	log() << warning << "glTexCoordPointerListIBM not available." << std::endl;
}

static void fallbackTexCoordPointervINTEL(api::GLint size, api::GLenum type, const api::GLvoid** pointer)
{
	log() << warning << "glTexCoordPointervINTEL not available." << std::endl;
}

static void fallbackTexEnvf(api::GLenum target, api::GLenum pname, api::GLfloat param)
{
	log() << warning << "glTexEnvf not available." << std::endl;
}

static void fallbackTexEnvfv(api::GLenum target, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "glTexEnvfv not available." << std::endl;
}

static void fallbackTexEnvi(api::GLenum target, api::GLenum pname, api::GLint param)
{
	log() << warning << "glTexEnvi not available." << std::endl;
}

static void fallbackTexEnviv(api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "glTexEnviv not available." << std::endl;
}

static void fallbackTexFilterFuncSGIS(api::GLenum target, api::GLenum filter, api::GLsizei n, const api::GLfloat* weights)
{
	log() << warning << "glTexFilterFuncSGIS not available." << std::endl;
}

static void fallbackTexGend(api::GLenum coord, api::GLenum pname, api::GLdouble param)
{
	log() << warning << "glTexGend not available." << std::endl;
}

static void fallbackTexGendv(api::GLenum coord, api::GLenum pname, const api::GLdouble* params)
{
	log() << warning << "glTexGendv not available." << std::endl;
}

static void fallbackTexGenf(api::GLenum coord, api::GLenum pname, api::GLfloat param)
{
	log() << warning << "glTexGenf not available." << std::endl;
}

static void fallbackTexGenfv(api::GLenum coord, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "glTexGenfv not available." << std::endl;
}

static void fallbackTexGeni(api::GLenum coord, api::GLenum pname, api::GLint param)
{
	log() << warning << "glTexGeni not available." << std::endl;
}

static void fallbackTexGeniv(api::GLenum coord, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "glTexGeniv not available." << std::endl;
}

static void fallbackTexImage1D(api::GLenum target, api::GLint level, api::GLint internalformat, api::GLsizei width, api::GLint border, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "glTexImage1D not available." << std::endl;
}

static void fallbackTexImage2D(api::GLenum target, api::GLint level, api::GLint internalformat, api::GLsizei width, api::GLsizei height, api::GLint border, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "glTexImage2D not available." << std::endl;
}

static void fallbackTexImage2DMultisample(api::GLenum target, api::GLsizei samples, api::GLint internalformat, api::GLsizei width, api::GLsizei height, api::GLboolean fixedsamplelocations)
{
	log() << warning << "glTexImage2DMultisample not available." << std::endl;
}

static void fallbackTexImage3D(api::GLenum target, api::GLint level, api::GLint internalformat, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLint border, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "glTexImage3D not available." << std::endl;
}

static void fallbackTexImage3DEXT(api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLint border, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "glTexImage3DEXT not available." << std::endl;
}

static void fallbackTexImage3DMultisample(api::GLenum target, api::GLsizei samples, api::GLint internalformat, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLboolean fixedsamplelocations)
{
	log() << warning << "glTexImage3DMultisample not available." << std::endl;
}

static void fallbackTexImage4DSGIS(api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLsizei size4d, api::GLint border, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "glTexImage4DSGIS not available." << std::endl;
}

static void fallbackTexParameterIiv(api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "glTexParameterIiv not available." << std::endl;
}

static void fallbackTexParameterIivEXT(api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "glTexParameterIivEXT not available." << std::endl;
}

static void fallbackTexParameterIuiv(api::GLenum target, api::GLenum pname, const api::GLuint* params)
{
	log() << warning << "glTexParameterIuiv not available." << std::endl;
}

static void fallbackTexParameterIuivEXT(api::GLenum target, api::GLenum pname, const api::GLuint* params)
{
	log() << warning << "glTexParameterIuivEXT not available." << std::endl;
}

static void fallbackTexParameterf(api::GLenum target, api::GLenum pname, api::GLfloat param)
{
	log() << warning << "glTexParameterf not available." << std::endl;
}

static void fallbackTexParameterfv(api::GLenum target, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "glTexParameterfv not available." << std::endl;
}

static void fallbackTexParameteri(api::GLenum target, api::GLenum pname, api::GLint param)
{
	log() << warning << "glTexParameteri not available." << std::endl;
}

static void fallbackTexParameteriv(api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "glTexParameteriv not available." << std::endl;
}

static void fallbackTexRenderbufferNV(api::GLenum target, api::GLuint renderbuffer)
{
	log() << warning << "glTexRenderbufferNV not available." << std::endl;
}

static void fallbackTexSubImage1D(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLsizei width, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "glTexSubImage1D not available." << std::endl;
}

static void fallbackTexSubImage1DEXT(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLsizei width, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "glTexSubImage1DEXT not available." << std::endl;
}

static void fallbackTexSubImage2D(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLsizei width, api::GLsizei height, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "glTexSubImage2D not available." << std::endl;
}

static void fallbackTexSubImage2DEXT(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLsizei width, api::GLsizei height, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "glTexSubImage2DEXT not available." << std::endl;
}

static void fallbackTexSubImage3D(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint zoffset, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "glTexSubImage3D not available." << std::endl;
}

static void fallbackTexSubImage3DEXT(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint zoffset, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "glTexSubImage3DEXT not available." << std::endl;
}

static void fallbackTexSubImage4DSGIS(api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint zoffset, api::GLint woffset, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLsizei size4d, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "glTexSubImage4DSGIS not available." << std::endl;
}

static void fallbackTextureBarrierNV()
{
	log() << warning << "glTextureBarrierNV not available." << std::endl;
}

static void fallbackTextureBufferEXT(api::GLuint texture, api::GLenum target, api::GLenum internalformat, api::GLuint buffer)
{
	log() << warning << "glTextureBufferEXT not available." << std::endl;
}

static void fallbackTextureColorMaskSGIS(api::GLboolean red, api::GLboolean green, api::GLboolean blue, api::GLboolean alpha)
{
	log() << warning << "glTextureColorMaskSGIS not available." << std::endl;
}

static void fallbackTextureImage1DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLint border, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "glTextureImage1DEXT not available." << std::endl;
}

static void fallbackTextureImage2DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLint border, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "glTextureImage2DEXT not available." << std::endl;
}

static void fallbackTextureImage3DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLenum internalformat, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLint border, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "glTextureImage3DEXT not available." << std::endl;
}

static void fallbackTextureLightEXT(api::GLenum pname)
{
	log() << warning << "glTextureLightEXT not available." << std::endl;
}

static void fallbackTextureMaterialEXT(api::GLenum face, api::GLenum mode)
{
	log() << warning << "glTextureMaterialEXT not available." << std::endl;
}

static void fallbackTextureNormalEXT(api::GLenum mode)
{
	log() << warning << "glTextureNormalEXT not available." << std::endl;
}

static void fallbackTextureParameterIivEXT(api::GLuint texture, api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "glTextureParameterIivEXT not available." << std::endl;
}

static void fallbackTextureParameterIuivEXT(api::GLuint texture, api::GLenum target, api::GLenum pname, const api::GLuint* params)
{
	log() << warning << "glTextureParameterIuivEXT not available." << std::endl;
}

static void fallbackTextureParameterfEXT(api::GLuint texture, api::GLenum target, api::GLenum pname, api::GLfloat param)
{
	log() << warning << "glTextureParameterfEXT not available." << std::endl;
}

static void fallbackTextureParameterfvEXT(api::GLuint texture, api::GLenum target, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "glTextureParameterfvEXT not available." << std::endl;
}

static void fallbackTextureParameteriEXT(api::GLuint texture, api::GLenum target, api::GLenum pname, api::GLint param)
{
	log() << warning << "glTextureParameteriEXT not available." << std::endl;
}

static void fallbackTextureParameterivEXT(api::GLuint texture, api::GLenum target, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "glTextureParameterivEXT not available." << std::endl;
}

static void fallbackTextureRangeAPPLE(api::GLenum target, api::GLsizei length, const api::GLvoid* pointer)
{
	log() << warning << "glTextureRangeAPPLE not available." << std::endl;
}

static void fallbackTextureRenderbufferEXT(api::GLuint texture, api::GLenum target, api::GLuint renderbuffer)
{
	log() << warning << "glTextureRenderbufferEXT not available." << std::endl;
}

static void fallbackTextureSubImage1DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLsizei width, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "glTextureSubImage1DEXT not available." << std::endl;
}

static void fallbackTextureSubImage2DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLsizei width, api::GLsizei height, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "glTextureSubImage2DEXT not available." << std::endl;
}

static void fallbackTextureSubImage3DEXT(api::GLuint texture, api::GLenum target, api::GLint level, api::GLint xoffset, api::GLint yoffset, api::GLint zoffset, api::GLsizei width, api::GLsizei height, api::GLsizei depth, api::GLenum format, api::GLenum type, const api::GLvoid* pixels)
{
	log() << warning << "glTextureSubImage3DEXT not available." << std::endl;
}

static void fallbackTrackMatrixNV(api::GLenum target, api::GLuint address, api::GLenum matrix, api::GLenum transform)
{
	log() << warning << "glTrackMatrixNV not available." << std::endl;
}

static void fallbackTransformFeedbackAttribsNV(api::GLuint count, const api::GLint* attribs, api::GLenum bufferMode)
{
	log() << warning << "glTransformFeedbackAttribsNV not available." << std::endl;
}

static void fallbackTransformFeedbackVaryings(api::GLuint program, api::GLsizei count, const api::GLchar** varyings, api::GLenum bufferMode)
{
	log() << warning << "glTransformFeedbackVaryings not available." << std::endl;
}

static void fallbackTransformFeedbackVaryingsEXT(api::GLuint program, api::GLsizei count, const api::GLchar** varyings, api::GLenum bufferMode)
{
	log() << warning << "glTransformFeedbackVaryingsEXT not available." << std::endl;
}

static void fallbackTransformFeedbackVaryingsNV(api::GLuint program, api::GLsizei count, const api::GLchar** varyings, api::GLenum bufferMode)
{
	log() << warning << "glTransformFeedbackVaryingsNV not available." << std::endl;
}

static void fallbackTranslated(api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "glTranslated not available." << std::endl;
}

static void fallbackTranslatef(api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "glTranslatef not available." << std::endl;
}

static void fallbackUniform1d(api::GLint location, api::GLdouble x)
{
	log() << warning << "glUniform1d not available." << std::endl;
}

static void fallbackUniform1dv(api::GLint location, api::GLsizei count, const api::GLdouble* value)
{
	log() << warning << "glUniform1dv not available." << std::endl;
}

static void fallbackUniform1f(api::GLint location, api::GLfloat v0)
{
	log() << warning << "glUniform1f not available." << std::endl;
}

static void fallbackUniform1fARB(api::GLint location, api::GLfloat v0)
{
	log() << warning << "glUniform1fARB not available." << std::endl;
}

static void fallbackUniform1fv(api::GLint location, api::GLsizei count, const api::GLfloat* value)
{
	log() << warning << "glUniform1fv not available." << std::endl;
}

static void fallbackUniform1fvARB(api::GLint location, api::GLsizei count, const api::GLfloat* value)
{
	log() << warning << "glUniform1fvARB not available." << std::endl;
}

static void fallbackUniform1i(api::GLint location, api::GLint v0)
{
	log() << warning << "glUniform1i not available." << std::endl;
}

static void fallbackUniform1iARB(api::GLint location, api::GLint v0)
{
	log() << warning << "glUniform1iARB not available." << std::endl;
}

static void fallbackUniform1iv(api::GLint location, api::GLsizei count, const api::GLint* value)
{
	log() << warning << "glUniform1iv not available." << std::endl;
}

static void fallbackUniform1ivARB(api::GLint location, api::GLsizei count, const api::GLint* value)
{
	log() << warning << "glUniform1ivARB not available." << std::endl;
}

static void fallbackUniform1ui(api::GLint location, api::GLuint v0)
{
	log() << warning << "glUniform1ui not available." << std::endl;
}

static void fallbackUniform1uiEXT(api::GLint location, api::GLuint v0)
{
	log() << warning << "glUniform1uiEXT not available." << std::endl;
}

static void fallbackUniform1uiv(api::GLint location, api::GLsizei count, const api::GLuint* value)
{
	log() << warning << "glUniform1uiv not available." << std::endl;
}

static void fallbackUniform1uivEXT(api::GLint location, api::GLsizei count, const api::GLuint* value)
{
	log() << warning << "glUniform1uivEXT not available." << std::endl;
}

static void fallbackUniform2d(api::GLint location, api::GLdouble x, api::GLdouble y)
{
	log() << warning << "glUniform2d not available." << std::endl;
}

static void fallbackUniform2dv(api::GLint location, api::GLsizei count, const api::GLdouble* value)
{
	log() << warning << "glUniform2dv not available." << std::endl;
}

static void fallbackUniform2f(api::GLint location, api::GLfloat v0, api::GLfloat v1)
{
	log() << warning << "glUniform2f not available." << std::endl;
}

static void fallbackUniform2fARB(api::GLint location, api::GLfloat v0, api::GLfloat v1)
{
	log() << warning << "glUniform2fARB not available." << std::endl;
}

static void fallbackUniform2fv(api::GLint location, api::GLsizei count, const api::GLfloat* value)
{
	log() << warning << "glUniform2fv not available." << std::endl;
}

static void fallbackUniform2fvARB(api::GLint location, api::GLsizei count, const api::GLfloat* value)
{
	log() << warning << "glUniform2fvARB not available." << std::endl;
}

static void fallbackUniform2i(api::GLint location, api::GLint v0, api::GLint v1)
{
	log() << warning << "glUniform2i not available." << std::endl;
}

static void fallbackUniform2iARB(api::GLint location, api::GLint v0, api::GLint v1)
{
	log() << warning << "glUniform2iARB not available." << std::endl;
}

static void fallbackUniform2iv(api::GLint location, api::GLsizei count, const api::GLint* value)
{
	log() << warning << "glUniform2iv not available." << std::endl;
}

static void fallbackUniform2ivARB(api::GLint location, api::GLsizei count, const api::GLint* value)
{
	log() << warning << "glUniform2ivARB not available." << std::endl;
}

static void fallbackUniform2ui(api::GLint location, api::GLuint v0, api::GLuint v1)
{
	log() << warning << "glUniform2ui not available." << std::endl;
}

static void fallbackUniform2uiEXT(api::GLint location, api::GLuint v0, api::GLuint v1)
{
	log() << warning << "glUniform2uiEXT not available." << std::endl;
}

static void fallbackUniform2uiv(api::GLint location, api::GLsizei count, const api::GLuint* value)
{
	log() << warning << "glUniform2uiv not available." << std::endl;
}

static void fallbackUniform2uivEXT(api::GLint location, api::GLsizei count, const api::GLuint* value)
{
	log() << warning << "glUniform2uivEXT not available." << std::endl;
}

static void fallbackUniform3d(api::GLint location, api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "glUniform3d not available." << std::endl;
}

static void fallbackUniform3dv(api::GLint location, api::GLsizei count, const api::GLdouble* value)
{
	log() << warning << "glUniform3dv not available." << std::endl;
}

static void fallbackUniform3f(api::GLint location, api::GLfloat v0, api::GLfloat v1, api::GLfloat v2)
{
	log() << warning << "glUniform3f not available." << std::endl;
}

static void fallbackUniform3fARB(api::GLint location, api::GLfloat v0, api::GLfloat v1, api::GLfloat v2)
{
	log() << warning << "glUniform3fARB not available." << std::endl;
}

static void fallbackUniform3fv(api::GLint location, api::GLsizei count, const api::GLfloat* value)
{
	log() << warning << "glUniform3fv not available." << std::endl;
}

static void fallbackUniform3fvARB(api::GLint location, api::GLsizei count, const api::GLfloat* value)
{
	log() << warning << "glUniform3fvARB not available." << std::endl;
}

static void fallbackUniform3i(api::GLint location, api::GLint v0, api::GLint v1, api::GLint v2)
{
	log() << warning << "glUniform3i not available." << std::endl;
}

static void fallbackUniform3iARB(api::GLint location, api::GLint v0, api::GLint v1, api::GLint v2)
{
	log() << warning << "glUniform3iARB not available." << std::endl;
}

static void fallbackUniform3iv(api::GLint location, api::GLsizei count, const api::GLint* value)
{
	log() << warning << "glUniform3iv not available." << std::endl;
}

static void fallbackUniform3ivARB(api::GLint location, api::GLsizei count, const api::GLint* value)
{
	log() << warning << "glUniform3ivARB not available." << std::endl;
}

static void fallbackUniform3ui(api::GLint location, api::GLuint v0, api::GLuint v1, api::GLuint v2)
{
	log() << warning << "glUniform3ui not available." << std::endl;
}

static void fallbackUniform3uiEXT(api::GLint location, api::GLuint v0, api::GLuint v1, api::GLuint v2)
{
	log() << warning << "glUniform3uiEXT not available." << std::endl;
}

static void fallbackUniform3uiv(api::GLint location, api::GLsizei count, const api::GLuint* value)
{
	log() << warning << "glUniform3uiv not available." << std::endl;
}

static void fallbackUniform3uivEXT(api::GLint location, api::GLsizei count, const api::GLuint* value)
{
	log() << warning << "glUniform3uivEXT not available." << std::endl;
}

static void fallbackUniform4d(api::GLint location, api::GLdouble x, api::GLdouble y, api::GLdouble z, api::GLdouble w)
{
	log() << warning << "glUniform4d not available." << std::endl;
}

static void fallbackUniform4dv(api::GLint location, api::GLsizei count, const api::GLdouble* value)
{
	log() << warning << "glUniform4dv not available." << std::endl;
}

static void fallbackUniform4f(api::GLint location, api::GLfloat v0, api::GLfloat v1, api::GLfloat v2, api::GLfloat v3)
{
	log() << warning << "glUniform4f not available." << std::endl;
}

static void fallbackUniform4fARB(api::GLint location, api::GLfloat v0, api::GLfloat v1, api::GLfloat v2, api::GLfloat v3)
{
	log() << warning << "glUniform4fARB not available." << std::endl;
}

static void fallbackUniform4fv(api::GLint location, api::GLsizei count, const api::GLfloat* value)
{
	log() << warning << "glUniform4fv not available." << std::endl;
}

static void fallbackUniform4fvARB(api::GLint location, api::GLsizei count, const api::GLfloat* value)
{
	log() << warning << "glUniform4fvARB not available." << std::endl;
}

static void fallbackUniform4i(api::GLint location, api::GLint v0, api::GLint v1, api::GLint v2, api::GLint v3)
{
	log() << warning << "glUniform4i not available." << std::endl;
}

static void fallbackUniform4iARB(api::GLint location, api::GLint v0, api::GLint v1, api::GLint v2, api::GLint v3)
{
	log() << warning << "glUniform4iARB not available." << std::endl;
}

static void fallbackUniform4iv(api::GLint location, api::GLsizei count, const api::GLint* value)
{
	log() << warning << "glUniform4iv not available." << std::endl;
}

static void fallbackUniform4ivARB(api::GLint location, api::GLsizei count, const api::GLint* value)
{
	log() << warning << "glUniform4ivARB not available." << std::endl;
}

static void fallbackUniform4ui(api::GLint location, api::GLuint v0, api::GLuint v1, api::GLuint v2, api::GLuint v3)
{
	log() << warning << "glUniform4ui not available." << std::endl;
}

static void fallbackUniform4uiEXT(api::GLint location, api::GLuint v0, api::GLuint v1, api::GLuint v2, api::GLuint v3)
{
	log() << warning << "glUniform4uiEXT not available." << std::endl;
}

static void fallbackUniform4uiv(api::GLint location, api::GLsizei count, const api::GLuint* value)
{
	log() << warning << "glUniform4uiv not available." << std::endl;
}

static void fallbackUniform4uivEXT(api::GLint location, api::GLsizei count, const api::GLuint* value)
{
	log() << warning << "glUniform4uivEXT not available." << std::endl;
}

static void fallbackUniformBlockBinding(api::GLuint program, api::GLuint uniformBlockIndex, api::GLuint uniformBlockBinding)
{
	log() << warning << "glUniformBlockBinding not available." << std::endl;
}

static void fallbackUniformBufferEXT(api::GLuint program, api::GLint location, api::GLuint buffer)
{
	log() << warning << "glUniformBufferEXT not available." << std::endl;
}

static void fallbackUniformMatrix2dv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "glUniformMatrix2dv not available." << std::endl;
}

static void fallbackUniformMatrix2fv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "glUniformMatrix2fv not available." << std::endl;
}

static void fallbackUniformMatrix2fvARB(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "glUniformMatrix2fvARB not available." << std::endl;
}

static void fallbackUniformMatrix2x3dv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "glUniformMatrix2x3dv not available." << std::endl;
}

static void fallbackUniformMatrix2x3fv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "glUniformMatrix2x3fv not available." << std::endl;
}

static void fallbackUniformMatrix2x4dv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "glUniformMatrix2x4dv not available." << std::endl;
}

static void fallbackUniformMatrix2x4fv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "glUniformMatrix2x4fv not available." << std::endl;
}

static void fallbackUniformMatrix3dv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "glUniformMatrix3dv not available." << std::endl;
}

static void fallbackUniformMatrix3fv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "glUniformMatrix3fv not available." << std::endl;
}

static void fallbackUniformMatrix3fvARB(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "glUniformMatrix3fvARB not available." << std::endl;
}

static void fallbackUniformMatrix3x2dv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "glUniformMatrix3x2dv not available." << std::endl;
}

static void fallbackUniformMatrix3x2fv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "glUniformMatrix3x2fv not available." << std::endl;
}

static void fallbackUniformMatrix3x4dv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "glUniformMatrix3x4dv not available." << std::endl;
}

static void fallbackUniformMatrix3x4fv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "glUniformMatrix3x4fv not available." << std::endl;
}

static void fallbackUniformMatrix4dv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "glUniformMatrix4dv not available." << std::endl;
}

static void fallbackUniformMatrix4fv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "glUniformMatrix4fv not available." << std::endl;
}

static void fallbackUniformMatrix4fvARB(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "glUniformMatrix4fvARB not available." << std::endl;
}

static void fallbackUniformMatrix4x2dv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "glUniformMatrix4x2dv not available." << std::endl;
}

static void fallbackUniformMatrix4x2fv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "glUniformMatrix4x2fv not available." << std::endl;
}

static void fallbackUniformMatrix4x3dv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLdouble* value)
{
	log() << warning << "glUniformMatrix4x3dv not available." << std::endl;
}

static void fallbackUniformMatrix4x3fv(api::GLint location, api::GLsizei count, api::GLboolean transpose, const api::GLfloat* value)
{
	log() << warning << "glUniformMatrix4x3fv not available." << std::endl;
}

static void fallbackUniformSubroutinesuiv(api::GLenum shadertype, api::GLsizei count, const api::GLuint* indices)
{
	log() << warning << "glUniformSubroutinesuiv not available." << std::endl;
}

static void fallbackUniformui64NV(api::GLint location, api::GLuint64EXT value)
{
	log() << warning << "glUniformui64NV not available." << std::endl;
}

static void fallbackUniformui64vNV(api::GLint location, api::GLsizei count, const api::GLuint64EXT* value)
{
	log() << warning << "glUniformui64vNV not available." << std::endl;
}

static void fallbackUnlockArraysEXT()
{
	log() << warning << "glUnlockArraysEXT not available." << std::endl;
}

static api::GLboolean fallbackUnmapBuffer(api::GLenum target)
{
	log() << warning << "glUnmapBuffer not available." << std::endl;
}

static api::GLboolean fallbackUnmapBufferARB(api::GLenum target)
{
	log() << warning << "glUnmapBufferARB not available." << std::endl;
}

static api::GLboolean fallbackUnmapNamedBufferEXT(api::GLuint buffer)
{
	log() << warning << "glUnmapNamedBufferEXT not available." << std::endl;
}

static void fallbackUnmapObjectBufferATI(api::GLuint buffer)
{
	log() << warning << "glUnmapObjectBufferATI not available." << std::endl;
}

static void fallbackUpdateObjectBufferATI(api::GLuint buffer, api::GLuint offset, api::GLsizei size, const api::GLvoid* pointer, api::GLenum preserve)
{
	log() << warning << "glUpdateObjectBufferATI not available." << std::endl;
}

static void fallbackUseProgram(api::GLuint program)
{
	log() << warning << "glUseProgram not available." << std::endl;
}

static void fallbackUseProgramObjectARB(api::GLhandleARB programObj)
{
	log() << warning << "glUseProgramObjectARB not available." << std::endl;
}

static void fallbackUseShaderProgramEXT(api::GLenum type, api::GLuint program)
{
	log() << warning << "glUseShaderProgramEXT not available." << std::endl;
}

static void fallbackValidateProgram(api::GLuint program)
{
	log() << warning << "glValidateProgram not available." << std::endl;
}

static void fallbackValidateProgramARB(api::GLhandleARB programObj)
{
	log() << warning << "glValidateProgramARB not available." << std::endl;
}

static void fallbackVariantArrayObjectATI(api::GLuint id, api::GLenum type, api::GLsizei stride, api::GLuint buffer, api::GLuint offset)
{
	log() << warning << "glVariantArrayObjectATI not available." << std::endl;
}

static void fallbackVariantPointerEXT(api::GLuint id, api::GLenum type, api::GLuint stride, const api::GLvoid* addr)
{
	log() << warning << "glVariantPointerEXT not available." << std::endl;
}

static void fallbackVariantbvEXT(api::GLuint id, const api::GLbyte* addr)
{
	log() << warning << "glVariantbvEXT not available." << std::endl;
}

static void fallbackVariantdvEXT(api::GLuint id, const api::GLdouble* addr)
{
	log() << warning << "glVariantdvEXT not available." << std::endl;
}

static void fallbackVariantfvEXT(api::GLuint id, const api::GLfloat* addr)
{
	log() << warning << "glVariantfvEXT not available." << std::endl;
}

static void fallbackVariantivEXT(api::GLuint id, const api::GLint* addr)
{
	log() << warning << "glVariantivEXT not available." << std::endl;
}

static void fallbackVariantsvEXT(api::GLuint id, const api::GLshort* addr)
{
	log() << warning << "glVariantsvEXT not available." << std::endl;
}

static void fallbackVariantubvEXT(api::GLuint id, const api::GLubyte* addr)
{
	log() << warning << "glVariantubvEXT not available." << std::endl;
}

static void fallbackVariantuivEXT(api::GLuint id, const api::GLuint* addr)
{
	log() << warning << "glVariantuivEXT not available." << std::endl;
}

static void fallbackVariantusvEXT(api::GLuint id, const api::GLushort* addr)
{
	log() << warning << "glVariantusvEXT not available." << std::endl;
}

static void fallbackVertex2d(api::GLdouble x, api::GLdouble y)
{
	log() << warning << "glVertex2d not available." << std::endl;
}

static void fallbackVertex2dv(const api::GLdouble* v)
{
	log() << warning << "glVertex2dv not available." << std::endl;
}

static void fallbackVertex2f(api::GLfloat x, api::GLfloat y)
{
	log() << warning << "glVertex2f not available." << std::endl;
}

static void fallbackVertex2fv(const api::GLfloat* v)
{
	log() << warning << "glVertex2fv not available." << std::endl;
}

static void fallbackVertex2hNV(api::GLhalfNV x, api::GLhalfNV y)
{
	log() << warning << "glVertex2hNV not available." << std::endl;
}

static void fallbackVertex2hvNV(const api::GLhalfNV* v)
{
	log() << warning << "glVertex2hvNV not available." << std::endl;
}

static void fallbackVertex2i(api::GLint x, api::GLint y)
{
	log() << warning << "glVertex2i not available." << std::endl;
}

static void fallbackVertex2iv(const api::GLint* v)
{
	log() << warning << "glVertex2iv not available." << std::endl;
}

static void fallbackVertex2s(api::GLshort x, api::GLshort y)
{
	log() << warning << "glVertex2s not available." << std::endl;
}

static void fallbackVertex2sv(const api::GLshort* v)
{
	log() << warning << "glVertex2sv not available." << std::endl;
}

static void fallbackVertex3d(api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "glVertex3d not available." << std::endl;
}

static void fallbackVertex3dv(const api::GLdouble* v)
{
	log() << warning << "glVertex3dv not available." << std::endl;
}

static void fallbackVertex3f(api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "glVertex3f not available." << std::endl;
}

static void fallbackVertex3fv(const api::GLfloat* v)
{
	log() << warning << "glVertex3fv not available." << std::endl;
}

static void fallbackVertex3hNV(api::GLhalfNV x, api::GLhalfNV y, api::GLhalfNV z)
{
	log() << warning << "glVertex3hNV not available." << std::endl;
}

static void fallbackVertex3hvNV(const api::GLhalfNV* v)
{
	log() << warning << "glVertex3hvNV not available." << std::endl;
}

static void fallbackVertex3i(api::GLint x, api::GLint y, api::GLint z)
{
	log() << warning << "glVertex3i not available." << std::endl;
}

static void fallbackVertex3iv(const api::GLint* v)
{
	log() << warning << "glVertex3iv not available." << std::endl;
}

static void fallbackVertex3s(api::GLshort x, api::GLshort y, api::GLshort z)
{
	log() << warning << "glVertex3s not available." << std::endl;
}

static void fallbackVertex3sv(const api::GLshort* v)
{
	log() << warning << "glVertex3sv not available." << std::endl;
}

static void fallbackVertex4d(api::GLdouble x, api::GLdouble y, api::GLdouble z, api::GLdouble w)
{
	log() << warning << "glVertex4d not available." << std::endl;
}

static void fallbackVertex4dv(const api::GLdouble* v)
{
	log() << warning << "glVertex4dv not available." << std::endl;
}

static void fallbackVertex4f(api::GLfloat x, api::GLfloat y, api::GLfloat z, api::GLfloat w)
{
	log() << warning << "glVertex4f not available." << std::endl;
}

static void fallbackVertex4fv(const api::GLfloat* v)
{
	log() << warning << "glVertex4fv not available." << std::endl;
}

static void fallbackVertex4hNV(api::GLhalfNV x, api::GLhalfNV y, api::GLhalfNV z, api::GLhalfNV w)
{
	log() << warning << "glVertex4hNV not available." << std::endl;
}

static void fallbackVertex4hvNV(const api::GLhalfNV* v)
{
	log() << warning << "glVertex4hvNV not available." << std::endl;
}

static void fallbackVertex4i(api::GLint x, api::GLint y, api::GLint z, api::GLint w)
{
	log() << warning << "glVertex4i not available." << std::endl;
}

static void fallbackVertex4iv(const api::GLint* v)
{
	log() << warning << "glVertex4iv not available." << std::endl;
}

static void fallbackVertex4s(api::GLshort x, api::GLshort y, api::GLshort z, api::GLshort w)
{
	log() << warning << "glVertex4s not available." << std::endl;
}

static void fallbackVertex4sv(const api::GLshort* v)
{
	log() << warning << "glVertex4sv not available." << std::endl;
}

static void fallbackVertexArrayParameteriAPPLE(api::GLenum pname, api::GLint param)
{
	log() << warning << "glVertexArrayParameteriAPPLE not available." << std::endl;
}

static void fallbackVertexArrayRangeAPPLE(api::GLsizei length, api::GLvoid* pointer)
{
	log() << warning << "glVertexArrayRangeAPPLE not available." << std::endl;
}

static void fallbackVertexArrayRangeNV(api::GLsizei length, const api::GLvoid* pointer)
{
	log() << warning << "glVertexArrayRangeNV not available." << std::endl;
}

static void fallbackVertexAttrib1d(api::GLuint index, api::GLdouble x)
{
	log() << warning << "glVertexAttrib1d not available." << std::endl;
}

static void fallbackVertexAttrib1dARB(api::GLuint index, api::GLdouble x)
{
	log() << warning << "glVertexAttrib1dARB not available." << std::endl;
}

static void fallbackVertexAttrib1dNV(api::GLuint index, api::GLdouble x)
{
	log() << warning << "glVertexAttrib1dNV not available." << std::endl;
}

static void fallbackVertexAttrib1dv(api::GLuint index, const api::GLdouble* v)
{
	log() << warning << "glVertexAttrib1dv not available." << std::endl;
}

static void fallbackVertexAttrib1dvARB(api::GLuint index, const api::GLdouble* v)
{
	log() << warning << "glVertexAttrib1dvARB not available." << std::endl;
}

static void fallbackVertexAttrib1dvNV(api::GLuint index, const api::GLdouble* v)
{
	log() << warning << "glVertexAttrib1dvNV not available." << std::endl;
}

static void fallbackVertexAttrib1f(api::GLuint index, api::GLfloat x)
{
	log() << warning << "glVertexAttrib1f not available." << std::endl;
}

static void fallbackVertexAttrib1fARB(api::GLuint index, api::GLfloat x)
{
	log() << warning << "glVertexAttrib1fARB not available." << std::endl;
}

static void fallbackVertexAttrib1fNV(api::GLuint index, api::GLfloat x)
{
	log() << warning << "glVertexAttrib1fNV not available." << std::endl;
}

static void fallbackVertexAttrib1fv(api::GLuint index, const api::GLfloat* v)
{
	log() << warning << "glVertexAttrib1fv not available." << std::endl;
}

static void fallbackVertexAttrib1fvARB(api::GLuint index, const api::GLfloat* v)
{
	log() << warning << "glVertexAttrib1fvARB not available." << std::endl;
}

static void fallbackVertexAttrib1fvNV(api::GLuint index, const api::GLfloat* v)
{
	log() << warning << "glVertexAttrib1fvNV not available." << std::endl;
}

static void fallbackVertexAttrib1hNV(api::GLuint index, api::GLhalfNV x)
{
	log() << warning << "glVertexAttrib1hNV not available." << std::endl;
}

static void fallbackVertexAttrib1hvNV(api::GLuint index, const api::GLhalfNV* v)
{
	log() << warning << "glVertexAttrib1hvNV not available." << std::endl;
}

static void fallbackVertexAttrib1s(api::GLuint index, api::GLshort x)
{
	log() << warning << "glVertexAttrib1s not available." << std::endl;
}

static void fallbackVertexAttrib1sARB(api::GLuint index, api::GLshort x)
{
	log() << warning << "glVertexAttrib1sARB not available." << std::endl;
}

static void fallbackVertexAttrib1sNV(api::GLuint index, api::GLshort x)
{
	log() << warning << "glVertexAttrib1sNV not available." << std::endl;
}

static void fallbackVertexAttrib1sv(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "glVertexAttrib1sv not available." << std::endl;
}

static void fallbackVertexAttrib1svARB(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "glVertexAttrib1svARB not available." << std::endl;
}

static void fallbackVertexAttrib1svNV(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "glVertexAttrib1svNV not available." << std::endl;
}

static void fallbackVertexAttrib2d(api::GLuint index, api::GLdouble x, api::GLdouble y)
{
	log() << warning << "glVertexAttrib2d not available." << std::endl;
}

static void fallbackVertexAttrib2dARB(api::GLuint index, api::GLdouble x, api::GLdouble y)
{
	log() << warning << "glVertexAttrib2dARB not available." << std::endl;
}

static void fallbackVertexAttrib2dNV(api::GLuint index, api::GLdouble x, api::GLdouble y)
{
	log() << warning << "glVertexAttrib2dNV not available." << std::endl;
}

static void fallbackVertexAttrib2dv(api::GLuint index, const api::GLdouble* v)
{
	log() << warning << "glVertexAttrib2dv not available." << std::endl;
}

static void fallbackVertexAttrib2dvARB(api::GLuint index, const api::GLdouble* v)
{
	log() << warning << "glVertexAttrib2dvARB not available." << std::endl;
}

static void fallbackVertexAttrib2dvNV(api::GLuint index, const api::GLdouble* v)
{
	log() << warning << "glVertexAttrib2dvNV not available." << std::endl;
}

static void fallbackVertexAttrib2f(api::GLuint index, api::GLfloat x, api::GLfloat y)
{
	log() << warning << "glVertexAttrib2f not available." << std::endl;
}

static void fallbackVertexAttrib2fARB(api::GLuint index, api::GLfloat x, api::GLfloat y)
{
	log() << warning << "glVertexAttrib2fARB not available." << std::endl;
}

static void fallbackVertexAttrib2fNV(api::GLuint index, api::GLfloat x, api::GLfloat y)
{
	log() << warning << "glVertexAttrib2fNV not available." << std::endl;
}

static void fallbackVertexAttrib2fv(api::GLuint index, const api::GLfloat* v)
{
	log() << warning << "glVertexAttrib2fv not available." << std::endl;
}

static void fallbackVertexAttrib2fvARB(api::GLuint index, const api::GLfloat* v)
{
	log() << warning << "glVertexAttrib2fvARB not available." << std::endl;
}

static void fallbackVertexAttrib2fvNV(api::GLuint index, const api::GLfloat* v)
{
	log() << warning << "glVertexAttrib2fvNV not available." << std::endl;
}

static void fallbackVertexAttrib2hNV(api::GLuint index, api::GLhalfNV x, api::GLhalfNV y)
{
	log() << warning << "glVertexAttrib2hNV not available." << std::endl;
}

static void fallbackVertexAttrib2hvNV(api::GLuint index, const api::GLhalfNV* v)
{
	log() << warning << "glVertexAttrib2hvNV not available." << std::endl;
}

static void fallbackVertexAttrib2s(api::GLuint index, api::GLshort x, api::GLshort y)
{
	log() << warning << "glVertexAttrib2s not available." << std::endl;
}

static void fallbackVertexAttrib2sARB(api::GLuint index, api::GLshort x, api::GLshort y)
{
	log() << warning << "glVertexAttrib2sARB not available." << std::endl;
}

static void fallbackVertexAttrib2sNV(api::GLuint index, api::GLshort x, api::GLshort y)
{
	log() << warning << "glVertexAttrib2sNV not available." << std::endl;
}

static void fallbackVertexAttrib2sv(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "glVertexAttrib2sv not available." << std::endl;
}

static void fallbackVertexAttrib2svARB(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "glVertexAttrib2svARB not available." << std::endl;
}

static void fallbackVertexAttrib2svNV(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "glVertexAttrib2svNV not available." << std::endl;
}

static void fallbackVertexAttrib3d(api::GLuint index, api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "glVertexAttrib3d not available." << std::endl;
}

static void fallbackVertexAttrib3dARB(api::GLuint index, api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "glVertexAttrib3dARB not available." << std::endl;
}

static void fallbackVertexAttrib3dNV(api::GLuint index, api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "glVertexAttrib3dNV not available." << std::endl;
}

static void fallbackVertexAttrib3dv(api::GLuint index, const api::GLdouble* v)
{
	log() << warning << "glVertexAttrib3dv not available." << std::endl;
}

static void fallbackVertexAttrib3dvARB(api::GLuint index, const api::GLdouble* v)
{
	log() << warning << "glVertexAttrib3dvARB not available." << std::endl;
}

static void fallbackVertexAttrib3dvNV(api::GLuint index, const api::GLdouble* v)
{
	log() << warning << "glVertexAttrib3dvNV not available." << std::endl;
}

static void fallbackVertexAttrib3f(api::GLuint index, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "glVertexAttrib3f not available." << std::endl;
}

static void fallbackVertexAttrib3fARB(api::GLuint index, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "glVertexAttrib3fARB not available." << std::endl;
}

static void fallbackVertexAttrib3fNV(api::GLuint index, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "glVertexAttrib3fNV not available." << std::endl;
}

static void fallbackVertexAttrib3fv(api::GLuint index, const api::GLfloat* v)
{
	log() << warning << "glVertexAttrib3fv not available." << std::endl;
}

static void fallbackVertexAttrib3fvARB(api::GLuint index, const api::GLfloat* v)
{
	log() << warning << "glVertexAttrib3fvARB not available." << std::endl;
}

static void fallbackVertexAttrib3fvNV(api::GLuint index, const api::GLfloat* v)
{
	log() << warning << "glVertexAttrib3fvNV not available." << std::endl;
}

static void fallbackVertexAttrib3hNV(api::GLuint index, api::GLhalfNV x, api::GLhalfNV y, api::GLhalfNV z)
{
	log() << warning << "glVertexAttrib3hNV not available." << std::endl;
}

static void fallbackVertexAttrib3hvNV(api::GLuint index, const api::GLhalfNV* v)
{
	log() << warning << "glVertexAttrib3hvNV not available." << std::endl;
}

static void fallbackVertexAttrib3s(api::GLuint index, api::GLshort x, api::GLshort y, api::GLshort z)
{
	log() << warning << "glVertexAttrib3s not available." << std::endl;
}

static void fallbackVertexAttrib3sARB(api::GLuint index, api::GLshort x, api::GLshort y, api::GLshort z)
{
	log() << warning << "glVertexAttrib3sARB not available." << std::endl;
}

static void fallbackVertexAttrib3sNV(api::GLuint index, api::GLshort x, api::GLshort y, api::GLshort z)
{
	log() << warning << "glVertexAttrib3sNV not available." << std::endl;
}

static void fallbackVertexAttrib3sv(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "glVertexAttrib3sv not available." << std::endl;
}

static void fallbackVertexAttrib3svARB(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "glVertexAttrib3svARB not available." << std::endl;
}

static void fallbackVertexAttrib3svNV(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "glVertexAttrib3svNV not available." << std::endl;
}

static void fallbackVertexAttrib4Nbv(api::GLuint index, const api::GLbyte* v)
{
	log() << warning << "glVertexAttrib4Nbv not available." << std::endl;
}

static void fallbackVertexAttrib4NbvARB(api::GLuint index, const api::GLbyte* v)
{
	log() << warning << "glVertexAttrib4NbvARB not available." << std::endl;
}

static void fallbackVertexAttrib4Niv(api::GLuint index, const api::GLint* v)
{
	log() << warning << "glVertexAttrib4Niv not available." << std::endl;
}

static void fallbackVertexAttrib4NivARB(api::GLuint index, const api::GLint* v)
{
	log() << warning << "glVertexAttrib4NivARB not available." << std::endl;
}

static void fallbackVertexAttrib4Nsv(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "glVertexAttrib4Nsv not available." << std::endl;
}

static void fallbackVertexAttrib4NsvARB(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "glVertexAttrib4NsvARB not available." << std::endl;
}

static void fallbackVertexAttrib4Nub(api::GLuint index, api::GLubyte x, api::GLubyte y, api::GLubyte z, api::GLubyte w)
{
	log() << warning << "glVertexAttrib4Nub not available." << std::endl;
}

static void fallbackVertexAttrib4NubARB(api::GLuint index, api::GLubyte x, api::GLubyte y, api::GLubyte z, api::GLubyte w)
{
	log() << warning << "glVertexAttrib4NubARB not available." << std::endl;
}

static void fallbackVertexAttrib4Nubv(api::GLuint index, const api::GLubyte* v)
{
	log() << warning << "glVertexAttrib4Nubv not available." << std::endl;
}

static void fallbackVertexAttrib4NubvARB(api::GLuint index, const api::GLubyte* v)
{
	log() << warning << "glVertexAttrib4NubvARB not available." << std::endl;
}

static void fallbackVertexAttrib4Nuiv(api::GLuint index, const api::GLuint* v)
{
	log() << warning << "glVertexAttrib4Nuiv not available." << std::endl;
}

static void fallbackVertexAttrib4NuivARB(api::GLuint index, const api::GLuint* v)
{
	log() << warning << "glVertexAttrib4NuivARB not available." << std::endl;
}

static void fallbackVertexAttrib4Nusv(api::GLuint index, const api::GLushort* v)
{
	log() << warning << "glVertexAttrib4Nusv not available." << std::endl;
}

static void fallbackVertexAttrib4NusvARB(api::GLuint index, const api::GLushort* v)
{
	log() << warning << "glVertexAttrib4NusvARB not available." << std::endl;
}

static void fallbackVertexAttrib4bv(api::GLuint index, const api::GLbyte* v)
{
	log() << warning << "glVertexAttrib4bv not available." << std::endl;
}

static void fallbackVertexAttrib4bvARB(api::GLuint index, const api::GLbyte* v)
{
	log() << warning << "glVertexAttrib4bvARB not available." << std::endl;
}

static void fallbackVertexAttrib4d(api::GLuint index, api::GLdouble x, api::GLdouble y, api::GLdouble z, api::GLdouble w)
{
	log() << warning << "glVertexAttrib4d not available." << std::endl;
}

static void fallbackVertexAttrib4dARB(api::GLuint index, api::GLdouble x, api::GLdouble y, api::GLdouble z, api::GLdouble w)
{
	log() << warning << "glVertexAttrib4dARB not available." << std::endl;
}

static void fallbackVertexAttrib4dNV(api::GLuint index, api::GLdouble x, api::GLdouble y, api::GLdouble z, api::GLdouble w)
{
	log() << warning << "glVertexAttrib4dNV not available." << std::endl;
}

static void fallbackVertexAttrib4dv(api::GLuint index, const api::GLdouble* v)
{
	log() << warning << "glVertexAttrib4dv not available." << std::endl;
}

static void fallbackVertexAttrib4dvARB(api::GLuint index, const api::GLdouble* v)
{
	log() << warning << "glVertexAttrib4dvARB not available." << std::endl;
}

static void fallbackVertexAttrib4dvNV(api::GLuint index, const api::GLdouble* v)
{
	log() << warning << "glVertexAttrib4dvNV not available." << std::endl;
}

static void fallbackVertexAttrib4f(api::GLuint index, api::GLfloat x, api::GLfloat y, api::GLfloat z, api::GLfloat w)
{
	log() << warning << "glVertexAttrib4f not available." << std::endl;
}

static void fallbackVertexAttrib4fARB(api::GLuint index, api::GLfloat x, api::GLfloat y, api::GLfloat z, api::GLfloat w)
{
	log() << warning << "glVertexAttrib4fARB not available." << std::endl;
}

static void fallbackVertexAttrib4fNV(api::GLuint index, api::GLfloat x, api::GLfloat y, api::GLfloat z, api::GLfloat w)
{
	log() << warning << "glVertexAttrib4fNV not available." << std::endl;
}

static void fallbackVertexAttrib4fv(api::GLuint index, const api::GLfloat* v)
{
	log() << warning << "glVertexAttrib4fv not available." << std::endl;
}

static void fallbackVertexAttrib4fvARB(api::GLuint index, const api::GLfloat* v)
{
	log() << warning << "glVertexAttrib4fvARB not available." << std::endl;
}

static void fallbackVertexAttrib4fvNV(api::GLuint index, const api::GLfloat* v)
{
	log() << warning << "glVertexAttrib4fvNV not available." << std::endl;
}

static void fallbackVertexAttrib4hNV(api::GLuint index, api::GLhalfNV x, api::GLhalfNV y, api::GLhalfNV z, api::GLhalfNV w)
{
	log() << warning << "glVertexAttrib4hNV not available." << std::endl;
}

static void fallbackVertexAttrib4hvNV(api::GLuint index, const api::GLhalfNV* v)
{
	log() << warning << "glVertexAttrib4hvNV not available." << std::endl;
}

static void fallbackVertexAttrib4iv(api::GLuint index, const api::GLint* v)
{
	log() << warning << "glVertexAttrib4iv not available." << std::endl;
}

static void fallbackVertexAttrib4ivARB(api::GLuint index, const api::GLint* v)
{
	log() << warning << "glVertexAttrib4ivARB not available." << std::endl;
}

static void fallbackVertexAttrib4s(api::GLuint index, api::GLshort x, api::GLshort y, api::GLshort z, api::GLshort w)
{
	log() << warning << "glVertexAttrib4s not available." << std::endl;
}

static void fallbackVertexAttrib4sARB(api::GLuint index, api::GLshort x, api::GLshort y, api::GLshort z, api::GLshort w)
{
	log() << warning << "glVertexAttrib4sARB not available." << std::endl;
}

static void fallbackVertexAttrib4sNV(api::GLuint index, api::GLshort x, api::GLshort y, api::GLshort z, api::GLshort w)
{
	log() << warning << "glVertexAttrib4sNV not available." << std::endl;
}

static void fallbackVertexAttrib4sv(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "glVertexAttrib4sv not available." << std::endl;
}

static void fallbackVertexAttrib4svARB(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "glVertexAttrib4svARB not available." << std::endl;
}

static void fallbackVertexAttrib4svNV(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "glVertexAttrib4svNV not available." << std::endl;
}

static void fallbackVertexAttrib4ubNV(api::GLuint index, api::GLubyte x, api::GLubyte y, api::GLubyte z, api::GLubyte w)
{
	log() << warning << "glVertexAttrib4ubNV not available." << std::endl;
}

static void fallbackVertexAttrib4ubv(api::GLuint index, const api::GLubyte* v)
{
	log() << warning << "glVertexAttrib4ubv not available." << std::endl;
}

static void fallbackVertexAttrib4ubvARB(api::GLuint index, const api::GLubyte* v)
{
	log() << warning << "glVertexAttrib4ubvARB not available." << std::endl;
}

static void fallbackVertexAttrib4ubvNV(api::GLuint index, const api::GLubyte* v)
{
	log() << warning << "glVertexAttrib4ubvNV not available." << std::endl;
}

static void fallbackVertexAttrib4uiv(api::GLuint index, const api::GLuint* v)
{
	log() << warning << "glVertexAttrib4uiv not available." << std::endl;
}

static void fallbackVertexAttrib4uivARB(api::GLuint index, const api::GLuint* v)
{
	log() << warning << "glVertexAttrib4uivARB not available." << std::endl;
}

static void fallbackVertexAttrib4usv(api::GLuint index, const api::GLushort* v)
{
	log() << warning << "glVertexAttrib4usv not available." << std::endl;
}

static void fallbackVertexAttrib4usvARB(api::GLuint index, const api::GLushort* v)
{
	log() << warning << "glVertexAttrib4usvARB not available." << std::endl;
}

static void fallbackVertexAttribArrayObjectATI(api::GLuint index, api::GLint size, api::GLenum type, api::GLboolean normalized, api::GLsizei stride, api::GLuint buffer, api::GLuint offset)
{
	log() << warning << "glVertexAttribArrayObjectATI not available." << std::endl;
}

static void fallbackVertexAttribDivisorARB(api::GLuint index, api::GLuint divisor)
{
	log() << warning << "glVertexAttribDivisorARB not available." << std::endl;
}

static void fallbackVertexAttribFormatNV(api::GLuint index, api::GLint size, api::GLenum type, api::GLboolean normalized, api::GLsizei stride)
{
	log() << warning << "glVertexAttribFormatNV not available." << std::endl;
}

static void fallbackVertexAttribI1i(api::GLuint index, api::GLint x)
{
	log() << warning << "glVertexAttribI1i not available." << std::endl;
}

static void fallbackVertexAttribI1iEXT(api::GLuint index, api::GLint x)
{
	log() << warning << "glVertexAttribI1iEXT not available." << std::endl;
}

static void fallbackVertexAttribI1iv(api::GLuint index, const api::GLint* v)
{
	log() << warning << "glVertexAttribI1iv not available." << std::endl;
}

static void fallbackVertexAttribI1ivEXT(api::GLuint index, const api::GLint* v)
{
	log() << warning << "glVertexAttribI1ivEXT not available." << std::endl;
}

static void fallbackVertexAttribI1ui(api::GLuint index, api::GLuint x)
{
	log() << warning << "glVertexAttribI1ui not available." << std::endl;
}

static void fallbackVertexAttribI1uiEXT(api::GLuint index, api::GLuint x)
{
	log() << warning << "glVertexAttribI1uiEXT not available." << std::endl;
}

static void fallbackVertexAttribI1uiv(api::GLuint index, const api::GLuint* v)
{
	log() << warning << "glVertexAttribI1uiv not available." << std::endl;
}

static void fallbackVertexAttribI1uivEXT(api::GLuint index, const api::GLuint* v)
{
	log() << warning << "glVertexAttribI1uivEXT not available." << std::endl;
}

static void fallbackVertexAttribI2i(api::GLuint index, api::GLint x, api::GLint y)
{
	log() << warning << "glVertexAttribI2i not available." << std::endl;
}

static void fallbackVertexAttribI2iEXT(api::GLuint index, api::GLint x, api::GLint y)
{
	log() << warning << "glVertexAttribI2iEXT not available." << std::endl;
}

static void fallbackVertexAttribI2iv(api::GLuint index, const api::GLint* v)
{
	log() << warning << "glVertexAttribI2iv not available." << std::endl;
}

static void fallbackVertexAttribI2ivEXT(api::GLuint index, const api::GLint* v)
{
	log() << warning << "glVertexAttribI2ivEXT not available." << std::endl;
}

static void fallbackVertexAttribI2ui(api::GLuint index, api::GLuint x, api::GLuint y)
{
	log() << warning << "glVertexAttribI2ui not available." << std::endl;
}

static void fallbackVertexAttribI2uiEXT(api::GLuint index, api::GLuint x, api::GLuint y)
{
	log() << warning << "glVertexAttribI2uiEXT not available." << std::endl;
}

static void fallbackVertexAttribI2uiv(api::GLuint index, const api::GLuint* v)
{
	log() << warning << "glVertexAttribI2uiv not available." << std::endl;
}

static void fallbackVertexAttribI2uivEXT(api::GLuint index, const api::GLuint* v)
{
	log() << warning << "glVertexAttribI2uivEXT not available." << std::endl;
}

static void fallbackVertexAttribI3i(api::GLuint index, api::GLint x, api::GLint y, api::GLint z)
{
	log() << warning << "glVertexAttribI3i not available." << std::endl;
}

static void fallbackVertexAttribI3iEXT(api::GLuint index, api::GLint x, api::GLint y, api::GLint z)
{
	log() << warning << "glVertexAttribI3iEXT not available." << std::endl;
}

static void fallbackVertexAttribI3iv(api::GLuint index, const api::GLint* v)
{
	log() << warning << "glVertexAttribI3iv not available." << std::endl;
}

static void fallbackVertexAttribI3ivEXT(api::GLuint index, const api::GLint* v)
{
	log() << warning << "glVertexAttribI3ivEXT not available." << std::endl;
}

static void fallbackVertexAttribI3ui(api::GLuint index, api::GLuint x, api::GLuint y, api::GLuint z)
{
	log() << warning << "glVertexAttribI3ui not available." << std::endl;
}

static void fallbackVertexAttribI3uiEXT(api::GLuint index, api::GLuint x, api::GLuint y, api::GLuint z)
{
	log() << warning << "glVertexAttribI3uiEXT not available." << std::endl;
}

static void fallbackVertexAttribI3uiv(api::GLuint index, const api::GLuint* v)
{
	log() << warning << "glVertexAttribI3uiv not available." << std::endl;
}

static void fallbackVertexAttribI3uivEXT(api::GLuint index, const api::GLuint* v)
{
	log() << warning << "glVertexAttribI3uivEXT not available." << std::endl;
}

static void fallbackVertexAttribI4bv(api::GLuint index, const api::GLbyte* v)
{
	log() << warning << "glVertexAttribI4bv not available." << std::endl;
}

static void fallbackVertexAttribI4bvEXT(api::GLuint index, const api::GLbyte* v)
{
	log() << warning << "glVertexAttribI4bvEXT not available." << std::endl;
}

static void fallbackVertexAttribI4i(api::GLuint index, api::GLint x, api::GLint y, api::GLint z, api::GLint w)
{
	log() << warning << "glVertexAttribI4i not available." << std::endl;
}

static void fallbackVertexAttribI4iEXT(api::GLuint index, api::GLint x, api::GLint y, api::GLint z, api::GLint w)
{
	log() << warning << "glVertexAttribI4iEXT not available." << std::endl;
}

static void fallbackVertexAttribI4iv(api::GLuint index, const api::GLint* v)
{
	log() << warning << "glVertexAttribI4iv not available." << std::endl;
}

static void fallbackVertexAttribI4ivEXT(api::GLuint index, const api::GLint* v)
{
	log() << warning << "glVertexAttribI4ivEXT not available." << std::endl;
}

static void fallbackVertexAttribI4sv(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "glVertexAttribI4sv not available." << std::endl;
}

static void fallbackVertexAttribI4svEXT(api::GLuint index, const api::GLshort* v)
{
	log() << warning << "glVertexAttribI4svEXT not available." << std::endl;
}

static void fallbackVertexAttribI4ubv(api::GLuint index, const api::GLubyte* v)
{
	log() << warning << "glVertexAttribI4ubv not available." << std::endl;
}

static void fallbackVertexAttribI4ubvEXT(api::GLuint index, const api::GLubyte* v)
{
	log() << warning << "glVertexAttribI4ubvEXT not available." << std::endl;
}

static void fallbackVertexAttribI4ui(api::GLuint index, api::GLuint x, api::GLuint y, api::GLuint z, api::GLuint w)
{
	log() << warning << "glVertexAttribI4ui not available." << std::endl;
}

static void fallbackVertexAttribI4uiEXT(api::GLuint index, api::GLuint x, api::GLuint y, api::GLuint z, api::GLuint w)
{
	log() << warning << "glVertexAttribI4uiEXT not available." << std::endl;
}

static void fallbackVertexAttribI4uiv(api::GLuint index, const api::GLuint* v)
{
	log() << warning << "glVertexAttribI4uiv not available." << std::endl;
}

static void fallbackVertexAttribI4uivEXT(api::GLuint index, const api::GLuint* v)
{
	log() << warning << "glVertexAttribI4uivEXT not available." << std::endl;
}

static void fallbackVertexAttribI4usv(api::GLuint index, const api::GLushort* v)
{
	log() << warning << "glVertexAttribI4usv not available." << std::endl;
}

static void fallbackVertexAttribI4usvEXT(api::GLuint index, const api::GLushort* v)
{
	log() << warning << "glVertexAttribI4usvEXT not available." << std::endl;
}

static void fallbackVertexAttribIFormatNV(api::GLuint index, api::GLint size, api::GLenum type, api::GLsizei stride)
{
	log() << warning << "glVertexAttribIFormatNV not available." << std::endl;
}

static void fallbackVertexAttribIPointer(api::GLuint index, api::GLint size, api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "glVertexAttribIPointer not available." << std::endl;
}

static void fallbackVertexAttribIPointerEXT(api::GLuint index, api::GLint size, api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "glVertexAttribIPointerEXT not available." << std::endl;
}

static void fallbackVertexAttribP1ui(api::GLuint index, api::GLenum type, api::GLboolean normalized, api::GLuint value)
{
	log() << warning << "glVertexAttribP1ui not available." << std::endl;
}

static void fallbackVertexAttribP1uiv(api::GLuint index, api::GLenum type, api::GLboolean normalized, const api::GLuint* value)
{
	log() << warning << "glVertexAttribP1uiv not available." << std::endl;
}

static void fallbackVertexAttribP2ui(api::GLuint index, api::GLenum type, api::GLboolean normalized, api::GLuint value)
{
	log() << warning << "glVertexAttribP2ui not available." << std::endl;
}

static void fallbackVertexAttribP2uiv(api::GLuint index, api::GLenum type, api::GLboolean normalized, const api::GLuint* value)
{
	log() << warning << "glVertexAttribP2uiv not available." << std::endl;
}

static void fallbackVertexAttribP3ui(api::GLuint index, api::GLenum type, api::GLboolean normalized, api::GLuint value)
{
	log() << warning << "glVertexAttribP3ui not available." << std::endl;
}

static void fallbackVertexAttribP3uiv(api::GLuint index, api::GLenum type, api::GLboolean normalized, const api::GLuint* value)
{
	log() << warning << "glVertexAttribP3uiv not available." << std::endl;
}

static void fallbackVertexAttribP4ui(api::GLuint index, api::GLenum type, api::GLboolean normalized, api::GLuint value)
{
	log() << warning << "glVertexAttribP4ui not available." << std::endl;
}

static void fallbackVertexAttribP4uiv(api::GLuint index, api::GLenum type, api::GLboolean normalized, const api::GLuint* value)
{
	log() << warning << "glVertexAttribP4uiv not available." << std::endl;
}

static void fallbackVertexAttribPointer(api::GLuint index, api::GLint size, api::GLenum type, api::GLboolean normalized, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "glVertexAttribPointer not available." << std::endl;
}

static void fallbackVertexAttribPointerARB(api::GLuint index, api::GLint size, api::GLenum type, api::GLboolean normalized, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "glVertexAttribPointerARB not available." << std::endl;
}

static void fallbackVertexAttribPointerNV(api::GLuint index, api::GLint fsize, api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "glVertexAttribPointerNV not available." << std::endl;
}

static void fallbackVertexAttribs1dvNV(api::GLuint index, api::GLsizei count, const api::GLdouble* v)
{
	log() << warning << "glVertexAttribs1dvNV not available." << std::endl;
}

static void fallbackVertexAttribs1fvNV(api::GLuint index, api::GLsizei count, const api::GLfloat* v)
{
	log() << warning << "glVertexAttribs1fvNV not available." << std::endl;
}

static void fallbackVertexAttribs1hvNV(api::GLuint index, api::GLsizei n, const api::GLhalfNV* v)
{
	log() << warning << "glVertexAttribs1hvNV not available." << std::endl;
}

static void fallbackVertexAttribs1svNV(api::GLuint index, api::GLsizei count, const api::GLshort* v)
{
	log() << warning << "glVertexAttribs1svNV not available." << std::endl;
}

static void fallbackVertexAttribs2dvNV(api::GLuint index, api::GLsizei count, const api::GLdouble* v)
{
	log() << warning << "glVertexAttribs2dvNV not available." << std::endl;
}

static void fallbackVertexAttribs2fvNV(api::GLuint index, api::GLsizei count, const api::GLfloat* v)
{
	log() << warning << "glVertexAttribs2fvNV not available." << std::endl;
}

static void fallbackVertexAttribs2hvNV(api::GLuint index, api::GLsizei n, const api::GLhalfNV* v)
{
	log() << warning << "glVertexAttribs2hvNV not available." << std::endl;
}

static void fallbackVertexAttribs2svNV(api::GLuint index, api::GLsizei count, const api::GLshort* v)
{
	log() << warning << "glVertexAttribs2svNV not available." << std::endl;
}

static void fallbackVertexAttribs3dvNV(api::GLuint index, api::GLsizei count, const api::GLdouble* v)
{
	log() << warning << "glVertexAttribs3dvNV not available." << std::endl;
}

static void fallbackVertexAttribs3fvNV(api::GLuint index, api::GLsizei count, const api::GLfloat* v)
{
	log() << warning << "glVertexAttribs3fvNV not available." << std::endl;
}

static void fallbackVertexAttribs3hvNV(api::GLuint index, api::GLsizei n, const api::GLhalfNV* v)
{
	log() << warning << "glVertexAttribs3hvNV not available." << std::endl;
}

static void fallbackVertexAttribs3svNV(api::GLuint index, api::GLsizei count, const api::GLshort* v)
{
	log() << warning << "glVertexAttribs3svNV not available." << std::endl;
}

static void fallbackVertexAttribs4dvNV(api::GLuint index, api::GLsizei count, const api::GLdouble* v)
{
	log() << warning << "glVertexAttribs4dvNV not available." << std::endl;
}

static void fallbackVertexAttribs4fvNV(api::GLuint index, api::GLsizei count, const api::GLfloat* v)
{
	log() << warning << "glVertexAttribs4fvNV not available." << std::endl;
}

static void fallbackVertexAttribs4hvNV(api::GLuint index, api::GLsizei n, const api::GLhalfNV* v)
{
	log() << warning << "glVertexAttribs4hvNV not available." << std::endl;
}

static void fallbackVertexAttribs4svNV(api::GLuint index, api::GLsizei count, const api::GLshort* v)
{
	log() << warning << "glVertexAttribs4svNV not available." << std::endl;
}

static void fallbackVertexAttribs4ubvNV(api::GLuint index, api::GLsizei count, const api::GLubyte* v)
{
	log() << warning << "glVertexAttribs4ubvNV not available." << std::endl;
}

static void fallbackVertexBlendARB(api::GLint count)
{
	log() << warning << "glVertexBlendARB not available." << std::endl;
}

static void fallbackVertexBlendEnvfATI(api::GLenum pname, api::GLfloat param)
{
	log() << warning << "glVertexBlendEnvfATI not available." << std::endl;
}

static void fallbackVertexBlendEnviATI(api::GLenum pname, api::GLint param)
{
	log() << warning << "glVertexBlendEnviATI not available." << std::endl;
}

static void fallbackVertexFormatNV(api::GLint size, api::GLenum type, api::GLsizei stride)
{
	log() << warning << "glVertexFormatNV not available." << std::endl;
}

static void fallbackVertexP2ui(api::GLenum type, api::GLuint value)
{
	log() << warning << "glVertexP2ui not available." << std::endl;
}

static void fallbackVertexP2uiv(api::GLenum type, const api::GLuint* value)
{
	log() << warning << "glVertexP2uiv not available." << std::endl;
}

static void fallbackVertexP3ui(api::GLenum type, api::GLuint value)
{
	log() << warning << "glVertexP3ui not available." << std::endl;
}

static void fallbackVertexP3uiv(api::GLenum type, const api::GLuint* value)
{
	log() << warning << "glVertexP3uiv not available." << std::endl;
}

static void fallbackVertexP4ui(api::GLenum type, api::GLuint value)
{
	log() << warning << "glVertexP4ui not available." << std::endl;
}

static void fallbackVertexP4uiv(api::GLenum type, const api::GLuint* value)
{
	log() << warning << "glVertexP4uiv not available." << std::endl;
}

static void fallbackVertexPointer(api::GLint size, api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "glVertexPointer not available." << std::endl;
}

static void fallbackVertexPointerEXT(api::GLint size, api::GLenum type, api::GLsizei stride, api::GLsizei count, const api::GLvoid* pointer)
{
	log() << warning << "glVertexPointerEXT not available." << std::endl;
}

static void fallbackVertexPointerListIBM(api::GLint size, api::GLenum type, api::GLint stride, const api::GLvoid** pointer, api::GLint ptrstride)
{
	log() << warning << "glVertexPointerListIBM not available." << std::endl;
}

static void fallbackVertexPointervINTEL(api::GLint size, api::GLenum type, const api::GLvoid** pointer)
{
	log() << warning << "glVertexPointervINTEL not available." << std::endl;
}

static void fallbackVertexStream1dATI(api::GLenum stream, api::GLdouble x)
{
	log() << warning << "glVertexStream1dATI not available." << std::endl;
}

static void fallbackVertexStream1dvATI(api::GLenum stream, const api::GLdouble* coords)
{
	log() << warning << "glVertexStream1dvATI not available." << std::endl;
}

static void fallbackVertexStream1fATI(api::GLenum stream, api::GLfloat x)
{
	log() << warning << "glVertexStream1fATI not available." << std::endl;
}

static void fallbackVertexStream1fvATI(api::GLenum stream, const api::GLfloat* coords)
{
	log() << warning << "glVertexStream1fvATI not available." << std::endl;
}

static void fallbackVertexStream1iATI(api::GLenum stream, api::GLint x)
{
	log() << warning << "glVertexStream1iATI not available." << std::endl;
}

static void fallbackVertexStream1ivATI(api::GLenum stream, const api::GLint* coords)
{
	log() << warning << "glVertexStream1ivATI not available." << std::endl;
}

static void fallbackVertexStream1sATI(api::GLenum stream, api::GLshort x)
{
	log() << warning << "glVertexStream1sATI not available." << std::endl;
}

static void fallbackVertexStream1svATI(api::GLenum stream, const api::GLshort* coords)
{
	log() << warning << "glVertexStream1svATI not available." << std::endl;
}

static void fallbackVertexStream2dATI(api::GLenum stream, api::GLdouble x, api::GLdouble y)
{
	log() << warning << "glVertexStream2dATI not available." << std::endl;
}

static void fallbackVertexStream2dvATI(api::GLenum stream, const api::GLdouble* coords)
{
	log() << warning << "glVertexStream2dvATI not available." << std::endl;
}

static void fallbackVertexStream2fATI(api::GLenum stream, api::GLfloat x, api::GLfloat y)
{
	log() << warning << "glVertexStream2fATI not available." << std::endl;
}

static void fallbackVertexStream2fvATI(api::GLenum stream, const api::GLfloat* coords)
{
	log() << warning << "glVertexStream2fvATI not available." << std::endl;
}

static void fallbackVertexStream2iATI(api::GLenum stream, api::GLint x, api::GLint y)
{
	log() << warning << "glVertexStream2iATI not available." << std::endl;
}

static void fallbackVertexStream2ivATI(api::GLenum stream, const api::GLint* coords)
{
	log() << warning << "glVertexStream2ivATI not available." << std::endl;
}

static void fallbackVertexStream2sATI(api::GLenum stream, api::GLshort x, api::GLshort y)
{
	log() << warning << "glVertexStream2sATI not available." << std::endl;
}

static void fallbackVertexStream2svATI(api::GLenum stream, const api::GLshort* coords)
{
	log() << warning << "glVertexStream2svATI not available." << std::endl;
}

static void fallbackVertexStream3dATI(api::GLenum stream, api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "glVertexStream3dATI not available." << std::endl;
}

static void fallbackVertexStream3dvATI(api::GLenum stream, const api::GLdouble* coords)
{
	log() << warning << "glVertexStream3dvATI not available." << std::endl;
}

static void fallbackVertexStream3fATI(api::GLenum stream, api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "glVertexStream3fATI not available." << std::endl;
}

static void fallbackVertexStream3fvATI(api::GLenum stream, const api::GLfloat* coords)
{
	log() << warning << "glVertexStream3fvATI not available." << std::endl;
}

static void fallbackVertexStream3iATI(api::GLenum stream, api::GLint x, api::GLint y, api::GLint z)
{
	log() << warning << "glVertexStream3iATI not available." << std::endl;
}

static void fallbackVertexStream3ivATI(api::GLenum stream, const api::GLint* coords)
{
	log() << warning << "glVertexStream3ivATI not available." << std::endl;
}

static void fallbackVertexStream3sATI(api::GLenum stream, api::GLshort x, api::GLshort y, api::GLshort z)
{
	log() << warning << "glVertexStream3sATI not available." << std::endl;
}

static void fallbackVertexStream3svATI(api::GLenum stream, const api::GLshort* coords)
{
	log() << warning << "glVertexStream3svATI not available." << std::endl;
}

static void fallbackVertexStream4dATI(api::GLenum stream, api::GLdouble x, api::GLdouble y, api::GLdouble z, api::GLdouble w)
{
	log() << warning << "glVertexStream4dATI not available." << std::endl;
}

static void fallbackVertexStream4dvATI(api::GLenum stream, const api::GLdouble* coords)
{
	log() << warning << "glVertexStream4dvATI not available." << std::endl;
}

static void fallbackVertexStream4fATI(api::GLenum stream, api::GLfloat x, api::GLfloat y, api::GLfloat z, api::GLfloat w)
{
	log() << warning << "glVertexStream4fATI not available." << std::endl;
}

static void fallbackVertexStream4fvATI(api::GLenum stream, const api::GLfloat* coords)
{
	log() << warning << "glVertexStream4fvATI not available." << std::endl;
}

static void fallbackVertexStream4iATI(api::GLenum stream, api::GLint x, api::GLint y, api::GLint z, api::GLint w)
{
	log() << warning << "glVertexStream4iATI not available." << std::endl;
}

static void fallbackVertexStream4ivATI(api::GLenum stream, const api::GLint* coords)
{
	log() << warning << "glVertexStream4ivATI not available." << std::endl;
}

static void fallbackVertexStream4sATI(api::GLenum stream, api::GLshort x, api::GLshort y, api::GLshort z, api::GLshort w)
{
	log() << warning << "glVertexStream4sATI not available." << std::endl;
}

static void fallbackVertexStream4svATI(api::GLenum stream, const api::GLshort* coords)
{
	log() << warning << "glVertexStream4svATI not available." << std::endl;
}

static void fallbackVertexWeightPointerEXT(api::GLsizei size, api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "glVertexWeightPointerEXT not available." << std::endl;
}

static void fallbackVertexWeightfEXT(api::GLfloat weight)
{
	log() << warning << "glVertexWeightfEXT not available." << std::endl;
}

static void fallbackVertexWeightfvEXT(const api::GLfloat* weight)
{
	log() << warning << "glVertexWeightfvEXT not available." << std::endl;
}

static void fallbackVertexWeighthNV(api::GLhalfNV weight)
{
	log() << warning << "glVertexWeighthNV not available." << std::endl;
}

static void fallbackVertexWeighthvNV(const api::GLhalfNV* weight)
{
	log() << warning << "glVertexWeighthvNV not available." << std::endl;
}

static api::GLenum fallbackVideoCaptureNV(api::GLuint video_capture_slot)
{
	log() << warning << "glVideoCaptureNV not available." << std::endl;
}

static void fallbackVideoCaptureStreamParameterdvNV(api::GLuint video_capture_slot, api::GLuint stream, api::GLenum pname, const api::GLdouble* params)
{
	log() << warning << "glVideoCaptureStreamParameterdvNV not available." << std::endl;
}

static void fallbackVideoCaptureStreamParameterfvNV(api::GLuint video_capture_slot, api::GLuint stream, api::GLenum pname, const api::GLfloat* params)
{
	log() << warning << "glVideoCaptureStreamParameterfvNV not available." << std::endl;
}

static void fallbackVideoCaptureStreamParameterivNV(api::GLuint video_capture_slot, api::GLuint stream, api::GLenum pname, const api::GLint* params)
{
	log() << warning << "glVideoCaptureStreamParameterivNV not available." << std::endl;
}

static void fallbackViewport(api::GLint x, api::GLint y, api::GLsizei width, api::GLsizei height)
{
	log() << warning << "glViewport not available." << std::endl;
}

static void fallbackWaitSync(api::GLsync sync, api::GLbitfield flags, api::GLuint64 timeout)
{
	log() << warning << "glWaitSync not available." << std::endl;
}

static void fallbackWeightPointerARB(api::GLint size, api::GLenum type, api::GLsizei stride, const api::GLvoid* pointer)
{
	log() << warning << "glWeightPointerARB not available." << std::endl;
}

static void fallbackWeightbvARB(api::GLint size, const api::GLbyte* weights)
{
	log() << warning << "glWeightbvARB not available." << std::endl;
}

static void fallbackWeightdvARB(api::GLint size, const api::GLdouble* weights)
{
	log() << warning << "glWeightdvARB not available." << std::endl;
}

static void fallbackWeightfvARB(api::GLint size, const api::GLfloat* weights)
{
	log() << warning << "glWeightfvARB not available." << std::endl;
}

static void fallbackWeightivARB(api::GLint size, const api::GLint* weights)
{
	log() << warning << "glWeightivARB not available." << std::endl;
}

static void fallbackWeightsvARB(api::GLint size, const api::GLshort* weights)
{
	log() << warning << "glWeightsvARB not available." << std::endl;
}

static void fallbackWeightubvARB(api::GLint size, const api::GLubyte* weights)
{
	log() << warning << "glWeightubvARB not available." << std::endl;
}

static void fallbackWeightuivARB(api::GLint size, const api::GLuint* weights)
{
	log() << warning << "glWeightuivARB not available." << std::endl;
}

static void fallbackWeightusvARB(api::GLint size, const api::GLushort* weights)
{
	log() << warning << "glWeightusvARB not available." << std::endl;
}

static void fallbackWindowPos2d(api::GLdouble x, api::GLdouble y)
{
	log() << warning << "glWindowPos2d not available." << std::endl;
}

static void fallbackWindowPos2dARB(api::GLdouble x, api::GLdouble y)
{
	log() << warning << "glWindowPos2dARB not available." << std::endl;
}

static void fallbackWindowPos2dMESA(api::GLdouble x, api::GLdouble y)
{
	log() << warning << "glWindowPos2dMESA not available." << std::endl;
}

static void fallbackWindowPos2dv(const api::GLdouble* v)
{
	log() << warning << "glWindowPos2dv not available." << std::endl;
}

static void fallbackWindowPos2dvARB(const api::GLdouble* v)
{
	log() << warning << "glWindowPos2dvARB not available." << std::endl;
}

static void fallbackWindowPos2dvMESA(const api::GLdouble* v)
{
	log() << warning << "glWindowPos2dvMESA not available." << std::endl;
}

static void fallbackWindowPos2f(api::GLfloat x, api::GLfloat y)
{
	log() << warning << "glWindowPos2f not available." << std::endl;
}

static void fallbackWindowPos2fARB(api::GLfloat x, api::GLfloat y)
{
	log() << warning << "glWindowPos2fARB not available." << std::endl;
}

static void fallbackWindowPos2fMESA(api::GLfloat x, api::GLfloat y)
{
	log() << warning << "glWindowPos2fMESA not available." << std::endl;
}

static void fallbackWindowPos2fv(const api::GLfloat* v)
{
	log() << warning << "glWindowPos2fv not available." << std::endl;
}

static void fallbackWindowPos2fvARB(const api::GLfloat* v)
{
	log() << warning << "glWindowPos2fvARB not available." << std::endl;
}

static void fallbackWindowPos2fvMESA(const api::GLfloat* v)
{
	log() << warning << "glWindowPos2fvMESA not available." << std::endl;
}

static void fallbackWindowPos2i(api::GLint x, api::GLint y)
{
	log() << warning << "glWindowPos2i not available." << std::endl;
}

static void fallbackWindowPos2iARB(api::GLint x, api::GLint y)
{
	log() << warning << "glWindowPos2iARB not available." << std::endl;
}

static void fallbackWindowPos2iMESA(api::GLint x, api::GLint y)
{
	log() << warning << "glWindowPos2iMESA not available." << std::endl;
}

static void fallbackWindowPos2iv(const api::GLint* v)
{
	log() << warning << "glWindowPos2iv not available." << std::endl;
}

static void fallbackWindowPos2ivARB(const api::GLint* v)
{
	log() << warning << "glWindowPos2ivARB not available." << std::endl;
}

static void fallbackWindowPos2ivMESA(const api::GLint* v)
{
	log() << warning << "glWindowPos2ivMESA not available." << std::endl;
}

static void fallbackWindowPos2s(api::GLshort x, api::GLshort y)
{
	log() << warning << "glWindowPos2s not available." << std::endl;
}

static void fallbackWindowPos2sARB(api::GLshort x, api::GLshort y)
{
	log() << warning << "glWindowPos2sARB not available." << std::endl;
}

static void fallbackWindowPos2sMESA(api::GLshort x, api::GLshort y)
{
	log() << warning << "glWindowPos2sMESA not available." << std::endl;
}

static void fallbackWindowPos2sv(const api::GLshort* v)
{
	log() << warning << "glWindowPos2sv not available." << std::endl;
}

static void fallbackWindowPos2svARB(const api::GLshort* v)
{
	log() << warning << "glWindowPos2svARB not available." << std::endl;
}

static void fallbackWindowPos2svMESA(const api::GLshort* v)
{
	log() << warning << "glWindowPos2svMESA not available." << std::endl;
}

static void fallbackWindowPos3d(api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "glWindowPos3d not available." << std::endl;
}

static void fallbackWindowPos3dARB(api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "glWindowPos3dARB not available." << std::endl;
}

static void fallbackWindowPos3dMESA(api::GLdouble x, api::GLdouble y, api::GLdouble z)
{
	log() << warning << "glWindowPos3dMESA not available." << std::endl;
}

static void fallbackWindowPos3dv(const api::GLdouble* v)
{
	log() << warning << "glWindowPos3dv not available." << std::endl;
}

static void fallbackWindowPos3dvARB(const api::GLdouble* v)
{
	log() << warning << "glWindowPos3dvARB not available." << std::endl;
}

static void fallbackWindowPos3dvMESA(const api::GLdouble* v)
{
	log() << warning << "glWindowPos3dvMESA not available." << std::endl;
}

static void fallbackWindowPos3f(api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "glWindowPos3f not available." << std::endl;
}

static void fallbackWindowPos3fARB(api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "glWindowPos3fARB not available." << std::endl;
}

static void fallbackWindowPos3fMESA(api::GLfloat x, api::GLfloat y, api::GLfloat z)
{
	log() << warning << "glWindowPos3fMESA not available." << std::endl;
}

static void fallbackWindowPos3fv(const api::GLfloat* v)
{
	log() << warning << "glWindowPos3fv not available." << std::endl;
}

static void fallbackWindowPos3fvARB(const api::GLfloat* v)
{
	log() << warning << "glWindowPos3fvARB not available." << std::endl;
}

static void fallbackWindowPos3fvMESA(const api::GLfloat* v)
{
	log() << warning << "glWindowPos3fvMESA not available." << std::endl;
}

static void fallbackWindowPos3i(api::GLint x, api::GLint y, api::GLint z)
{
	log() << warning << "glWindowPos3i not available." << std::endl;
}

static void fallbackWindowPos3iARB(api::GLint x, api::GLint y, api::GLint z)
{
	log() << warning << "glWindowPos3iARB not available." << std::endl;
}

static void fallbackWindowPos3iMESA(api::GLint x, api::GLint y, api::GLint z)
{
	log() << warning << "glWindowPos3iMESA not available." << std::endl;
}

static void fallbackWindowPos3iv(const api::GLint* v)
{
	log() << warning << "glWindowPos3iv not available." << std::endl;
}

static void fallbackWindowPos3ivARB(const api::GLint* v)
{
	log() << warning << "glWindowPos3ivARB not available." << std::endl;
}

static void fallbackWindowPos3ivMESA(const api::GLint* v)
{
	log() << warning << "glWindowPos3ivMESA not available." << std::endl;
}

static void fallbackWindowPos3s(api::GLshort x, api::GLshort y, api::GLshort z)
{
	log() << warning << "glWindowPos3s not available." << std::endl;
}

static void fallbackWindowPos3sARB(api::GLshort x, api::GLshort y, api::GLshort z)
{
	log() << warning << "glWindowPos3sARB not available." << std::endl;
}

static void fallbackWindowPos3sMESA(api::GLshort x, api::GLshort y, api::GLshort z)
{
	log() << warning << "glWindowPos3sMESA not available." << std::endl;
}

static void fallbackWindowPos3sv(const api::GLshort* v)
{
	log() << warning << "glWindowPos3sv not available." << std::endl;
}

static void fallbackWindowPos3svARB(const api::GLshort* v)
{
	log() << warning << "glWindowPos3svARB not available." << std::endl;
}

static void fallbackWindowPos3svMESA(const api::GLshort* v)
{
	log() << warning << "glWindowPos3svMESA not available." << std::endl;
}

static void fallbackWindowPos4dMESA(api::GLdouble x, api::GLdouble y, api::GLdouble z, api::GLdouble w)
{
	log() << warning << "glWindowPos4dMESA not available." << std::endl;
}

static void fallbackWindowPos4dvMESA(const api::GLdouble* v)
{
	log() << warning << "glWindowPos4dvMESA not available." << std::endl;
}

static void fallbackWindowPos4fMESA(api::GLfloat x, api::GLfloat y, api::GLfloat z, api::GLfloat w)
{
	log() << warning << "glWindowPos4fMESA not available." << std::endl;
}

static void fallbackWindowPos4fvMESA(const api::GLfloat* v)
{
	log() << warning << "glWindowPos4fvMESA not available." << std::endl;
}

static void fallbackWindowPos4iMESA(api::GLint x, api::GLint y, api::GLint z, api::GLint w)
{
	log() << warning << "glWindowPos4iMESA not available." << std::endl;
}

static void fallbackWindowPos4ivMESA(const api::GLint* v)
{
	log() << warning << "glWindowPos4ivMESA not available." << std::endl;
}

static void fallbackWindowPos4sMESA(api::GLshort x, api::GLshort y, api::GLshort z, api::GLshort w)
{
	log() << warning << "glWindowPos4sMESA not available." << std::endl;
}

static void fallbackWindowPos4svMESA(const api::GLshort* v)
{
	log() << warning << "glWindowPos4svMESA not available." << std::endl;
}

static void fallbackWriteMaskEXT(api::GLuint res, api::GLuint in, api::GLenum outX, api::GLenum outY, api::GLenum outZ, api::GLenum outW)
{
	log() << warning << "glWriteMaskEXT not available." << std::endl;
}

api::api() :
	glAccum(fallbackAccum),
	glActiveProgramEXT(fallbackActiveProgramEXT),
	glActiveStencilFaceEXT(fallbackActiveStencilFaceEXT),
	glActiveTexture(fallbackActiveTexture),
	glActiveTextureARB(fallbackActiveTextureARB),
	glActiveVaryingNV(fallbackActiveVaryingNV),
	glAlphaFragmentOp1ATI(fallbackAlphaFragmentOp1ATI),
	glAlphaFragmentOp2ATI(fallbackAlphaFragmentOp2ATI),
	glAlphaFragmentOp3ATI(fallbackAlphaFragmentOp3ATI),
	glAlphaFunc(fallbackAlphaFunc),
	glApplyTextureEXT(fallbackApplyTextureEXT),
	glAreProgramsResidentNV(fallbackAreProgramsResidentNV),
	glAreTexturesResident(fallbackAreTexturesResident),
	glAreTexturesResidentEXT(fallbackAreTexturesResidentEXT),
	glArrayElement(fallbackArrayElement),
	glArrayElementEXT(fallbackArrayElementEXT),
	glArrayObjectATI(fallbackArrayObjectATI),
	glAsyncMarkerSGIX(fallbackAsyncMarkerSGIX),
	glAttachObjectARB(fallbackAttachObjectARB),
	glAttachShader(fallbackAttachShader),
	glBegin(fallbackBegin),
	glBeginConditionalRender(fallbackBeginConditionalRender),
	glBeginConditionalRenderNV(fallbackBeginConditionalRenderNV),
	glBeginFragmentShaderATI(fallbackBeginFragmentShaderATI),
	glBeginOcclusionQueryNV(fallbackBeginOcclusionQueryNV),
	glBeginPerfMonitorAMD(fallbackBeginPerfMonitorAMD),
	glBeginQuery(fallbackBeginQuery),
	glBeginQueryARB(fallbackBeginQueryARB),
	glBeginQueryIndexed(fallbackBeginQueryIndexed),
	glBeginTransformFeedback(fallbackBeginTransformFeedback),
	glBeginTransformFeedbackEXT(fallbackBeginTransformFeedbackEXT),
	glBeginTransformFeedbackNV(fallbackBeginTransformFeedbackNV),
	glBeginVertexShaderEXT(fallbackBeginVertexShaderEXT),
	glBeginVideoCaptureNV(fallbackBeginVideoCaptureNV),
	glBindAttribLocation(fallbackBindAttribLocation),
	glBindAttribLocationARB(fallbackBindAttribLocationARB),
	glBindBuffer(fallbackBindBuffer),
	glBindBufferARB(fallbackBindBufferARB),
	glBindBufferBase(fallbackBindBufferBase),
	glBindBufferBaseEXT(fallbackBindBufferBaseEXT),
	glBindBufferBaseNV(fallbackBindBufferBaseNV),
	glBindBufferOffsetEXT(fallbackBindBufferOffsetEXT),
	glBindBufferOffsetNV(fallbackBindBufferOffsetNV),
	glBindBufferRange(fallbackBindBufferRange),
	glBindBufferRangeEXT(fallbackBindBufferRangeEXT),
	glBindBufferRangeNV(fallbackBindBufferRangeNV),
	glBindFragDataLocation(fallbackBindFragDataLocation),
	glBindFragDataLocationEXT(fallbackBindFragDataLocationEXT),
	glBindFragDataLocationIndexed(fallbackBindFragDataLocationIndexed),
	glBindFragmentShaderATI(fallbackBindFragmentShaderATI),
	glBindFramebuffer(fallbackBindFramebuffer),
	glBindFramebufferEXT(fallbackBindFramebufferEXT),
	glBindLightParameterEXT(fallbackBindLightParameterEXT),
	glBindMaterialParameterEXT(fallbackBindMaterialParameterEXT),
	glBindMultiTextureEXT(fallbackBindMultiTextureEXT),
	glBindParameterEXT(fallbackBindParameterEXT),
	glBindProgramARB(fallbackBindProgramARB),
	glBindProgramNV(fallbackBindProgramNV),
	glBindRenderbuffer(fallbackBindRenderbuffer),
	glBindRenderbufferEXT(fallbackBindRenderbufferEXT),
	glBindSampler(fallbackBindSampler),
	glBindTexGenParameterEXT(fallbackBindTexGenParameterEXT),
	glBindTexture(fallbackBindTexture),
	glBindTextureEXT(fallbackBindTextureEXT),
	glBindTextureUnitParameterEXT(fallbackBindTextureUnitParameterEXT),
	glBindTransformFeedback(fallbackBindTransformFeedback),
	glBindTransformFeedbackNV(fallbackBindTransformFeedbackNV),
	glBindVertexArray(fallbackBindVertexArray),
	glBindVertexArrayAPPLE(fallbackBindVertexArrayAPPLE),
	glBindVertexShaderEXT(fallbackBindVertexShaderEXT),
	glBindVideoCaptureStreamBufferNV(fallbackBindVideoCaptureStreamBufferNV),
	glBindVideoCaptureStreamTextureNV(fallbackBindVideoCaptureStreamTextureNV),
	glBinormal3bEXT(fallbackBinormal3bEXT),
	glBinormal3bvEXT(fallbackBinormal3bvEXT),
	glBinormal3dEXT(fallbackBinormal3dEXT),
	glBinormal3dvEXT(fallbackBinormal3dvEXT),
	glBinormal3fEXT(fallbackBinormal3fEXT),
	glBinormal3fvEXT(fallbackBinormal3fvEXT),
	glBinormal3iEXT(fallbackBinormal3iEXT),
	glBinormal3ivEXT(fallbackBinormal3ivEXT),
	glBinormal3sEXT(fallbackBinormal3sEXT),
	glBinormal3svEXT(fallbackBinormal3svEXT),
	glBinormalPointerEXT(fallbackBinormalPointerEXT),
	glBitmap(fallbackBitmap),
	glBlendColor(fallbackBlendColor),
	glBlendColorEXT(fallbackBlendColorEXT),
	glBlendEquation(fallbackBlendEquation),
	glBlendEquationEXT(fallbackBlendEquationEXT),
	glBlendEquationIndexedAMD(fallbackBlendEquationIndexedAMD),
	glBlendEquationSeparate(fallbackBlendEquationSeparate),
	glBlendEquationSeparateEXT(fallbackBlendEquationSeparateEXT),
	glBlendEquationSeparateIndexedAMD(fallbackBlendEquationSeparateIndexedAMD),
	glBlendEquationSeparatei(fallbackBlendEquationSeparatei),
	glBlendEquationi(fallbackBlendEquationi),
	glBlendFunc(fallbackBlendFunc),
	glBlendFuncIndexedAMD(fallbackBlendFuncIndexedAMD),
	glBlendFuncSeparate(fallbackBlendFuncSeparate),
	glBlendFuncSeparateEXT(fallbackBlendFuncSeparateEXT),
	glBlendFuncSeparateINGR(fallbackBlendFuncSeparateINGR),
	glBlendFuncSeparateIndexedAMD(fallbackBlendFuncSeparateIndexedAMD),
	glBlendFuncSeparatei(fallbackBlendFuncSeparatei),
	glBlendFunci(fallbackBlendFunci),
	glBlitFramebuffer(fallbackBlitFramebuffer),
	glBlitFramebufferEXT(fallbackBlitFramebufferEXT),
	glBufferAddressRangeNV(fallbackBufferAddressRangeNV),
	glBufferData(fallbackBufferData),
	glBufferDataARB(fallbackBufferDataARB),
	glBufferParameteriAPPLE(fallbackBufferParameteriAPPLE),
	glBufferSubData(fallbackBufferSubData),
	glBufferSubDataARB(fallbackBufferSubDataARB),
	glCallList(fallbackCallList),
	glCallLists(fallbackCallLists),
	glCheckFramebufferStatus(fallbackCheckFramebufferStatus),
	glCheckFramebufferStatusEXT(fallbackCheckFramebufferStatusEXT),
	glCheckNamedFramebufferStatusEXT(fallbackCheckNamedFramebufferStatusEXT),
	glClampColor(fallbackClampColor),
	glClampColorARB(fallbackClampColorARB),
	glClear(fallbackClear),
	glClearAccum(fallbackClearAccum),
	glClearBufferfi(fallbackClearBufferfi),
	glClearBufferfv(fallbackClearBufferfv),
	glClearBufferiv(fallbackClearBufferiv),
	glClearBufferuiv(fallbackClearBufferuiv),
	glClearColor(fallbackClearColor),
	glClearColorIiEXT(fallbackClearColorIiEXT),
	glClearColorIuiEXT(fallbackClearColorIuiEXT),
	glClearDepth(fallbackClearDepth),
	glClearDepthdNV(fallbackClearDepthdNV),
	glClearIndex(fallbackClearIndex),
	glClearStencil(fallbackClearStencil),
	glClientActiveTexture(fallbackClientActiveTexture),
	glClientActiveTextureARB(fallbackClientActiveTextureARB),
	glClientActiveVertexStreamATI(fallbackClientActiveVertexStreamATI),
	glClientAttribDefaultEXT(fallbackClientAttribDefaultEXT),
	glClientWaitSync(fallbackClientWaitSync),
	glClipPlane(fallbackClipPlane),
	glColor3b(fallbackColor3b),
	glColor3bv(fallbackColor3bv),
	glColor3d(fallbackColor3d),
	glColor3dv(fallbackColor3dv),
	glColor3f(fallbackColor3f),
	glColor3fVertex3fSUN(fallbackColor3fVertex3fSUN),
	glColor3fVertex3fvSUN(fallbackColor3fVertex3fvSUN),
	glColor3fv(fallbackColor3fv),
	glColor3hNV(fallbackColor3hNV),
	glColor3hvNV(fallbackColor3hvNV),
	glColor3i(fallbackColor3i),
	glColor3iv(fallbackColor3iv),
	glColor3s(fallbackColor3s),
	glColor3sv(fallbackColor3sv),
	glColor3ub(fallbackColor3ub),
	glColor3ubv(fallbackColor3ubv),
	glColor3ui(fallbackColor3ui),
	glColor3uiv(fallbackColor3uiv),
	glColor3us(fallbackColor3us),
	glColor3usv(fallbackColor3usv),
	glColor4b(fallbackColor4b),
	glColor4bv(fallbackColor4bv),
	glColor4d(fallbackColor4d),
	glColor4dv(fallbackColor4dv),
	glColor4f(fallbackColor4f),
	glColor4fNormal3fVertex3fSUN(fallbackColor4fNormal3fVertex3fSUN),
	glColor4fNormal3fVertex3fvSUN(fallbackColor4fNormal3fVertex3fvSUN),
	glColor4fv(fallbackColor4fv),
	glColor4hNV(fallbackColor4hNV),
	glColor4hvNV(fallbackColor4hvNV),
	glColor4i(fallbackColor4i),
	glColor4iv(fallbackColor4iv),
	glColor4s(fallbackColor4s),
	glColor4sv(fallbackColor4sv),
	glColor4ub(fallbackColor4ub),
	glColor4ubVertex2fSUN(fallbackColor4ubVertex2fSUN),
	glColor4ubVertex2fvSUN(fallbackColor4ubVertex2fvSUN),
	glColor4ubVertex3fSUN(fallbackColor4ubVertex3fSUN),
	glColor4ubVertex3fvSUN(fallbackColor4ubVertex3fvSUN),
	glColor4ubv(fallbackColor4ubv),
	glColor4ui(fallbackColor4ui),
	glColor4uiv(fallbackColor4uiv),
	glColor4us(fallbackColor4us),
	glColor4usv(fallbackColor4usv),
	glColorFormatNV(fallbackColorFormatNV),
	glColorFragmentOp1ATI(fallbackColorFragmentOp1ATI),
	glColorFragmentOp2ATI(fallbackColorFragmentOp2ATI),
	glColorFragmentOp3ATI(fallbackColorFragmentOp3ATI),
	glColorMask(fallbackColorMask),
	glColorMaskIndexedEXT(fallbackColorMaskIndexedEXT),
	glColorMaski(fallbackColorMaski),
	glColorMaterial(fallbackColorMaterial),
	glColorP3ui(fallbackColorP3ui),
	glColorP3uiv(fallbackColorP3uiv),
	glColorP4ui(fallbackColorP4ui),
	glColorP4uiv(fallbackColorP4uiv),
	glColorPointer(fallbackColorPointer),
	glColorPointerEXT(fallbackColorPointerEXT),
	glColorPointerListIBM(fallbackColorPointerListIBM),
	glColorPointervINTEL(fallbackColorPointervINTEL),
	glColorSubTable(fallbackColorSubTable),
	glColorSubTableEXT(fallbackColorSubTableEXT),
	glColorTable(fallbackColorTable),
	glColorTableEXT(fallbackColorTableEXT),
	glColorTableParameterfv(fallbackColorTableParameterfv),
	glColorTableParameterfvSGI(fallbackColorTableParameterfvSGI),
	glColorTableParameteriv(fallbackColorTableParameteriv),
	glColorTableParameterivSGI(fallbackColorTableParameterivSGI),
	glColorTableSGI(fallbackColorTableSGI),
	glCombinerInputNV(fallbackCombinerInputNV),
	glCombinerOutputNV(fallbackCombinerOutputNV),
	glCombinerParameterfNV(fallbackCombinerParameterfNV),
	glCombinerParameterfvNV(fallbackCombinerParameterfvNV),
	glCombinerParameteriNV(fallbackCombinerParameteriNV),
	glCombinerParameterivNV(fallbackCombinerParameterivNV),
	glCombinerStageParameterfvNV(fallbackCombinerStageParameterfvNV),
	glCompileShader(fallbackCompileShader),
	glCompileShaderARB(fallbackCompileShaderARB),
	glCompileShaderIncludeARB(fallbackCompileShaderIncludeARB),
	glCompressedMultiTexImage1DEXT(fallbackCompressedMultiTexImage1DEXT),
	glCompressedMultiTexImage2DEXT(fallbackCompressedMultiTexImage2DEXT),
	glCompressedMultiTexImage3DEXT(fallbackCompressedMultiTexImage3DEXT),
	glCompressedMultiTexSubImage1DEXT(fallbackCompressedMultiTexSubImage1DEXT),
	glCompressedMultiTexSubImage2DEXT(fallbackCompressedMultiTexSubImage2DEXT),
	glCompressedMultiTexSubImage3DEXT(fallbackCompressedMultiTexSubImage3DEXT),
	glCompressedTexImage1D(fallbackCompressedTexImage1D),
	glCompressedTexImage1DARB(fallbackCompressedTexImage1DARB),
	glCompressedTexImage2D(fallbackCompressedTexImage2D),
	glCompressedTexImage2DARB(fallbackCompressedTexImage2DARB),
	glCompressedTexImage3D(fallbackCompressedTexImage3D),
	glCompressedTexImage3DARB(fallbackCompressedTexImage3DARB),
	glCompressedTexSubImage1D(fallbackCompressedTexSubImage1D),
	glCompressedTexSubImage1DARB(fallbackCompressedTexSubImage1DARB),
	glCompressedTexSubImage2D(fallbackCompressedTexSubImage2D),
	glCompressedTexSubImage2DARB(fallbackCompressedTexSubImage2DARB),
	glCompressedTexSubImage3D(fallbackCompressedTexSubImage3D),
	glCompressedTexSubImage3DARB(fallbackCompressedTexSubImage3DARB),
	glCompressedTextureImage1DEXT(fallbackCompressedTextureImage1DEXT),
	glCompressedTextureImage2DEXT(fallbackCompressedTextureImage2DEXT),
	glCompressedTextureImage3DEXT(fallbackCompressedTextureImage3DEXT),
	glCompressedTextureSubImage1DEXT(fallbackCompressedTextureSubImage1DEXT),
	glCompressedTextureSubImage2DEXT(fallbackCompressedTextureSubImage2DEXT),
	glCompressedTextureSubImage3DEXT(fallbackCompressedTextureSubImage3DEXT),
	glConvolutionFilter1D(fallbackConvolutionFilter1D),
	glConvolutionFilter1DEXT(fallbackConvolutionFilter1DEXT),
	glConvolutionFilter2D(fallbackConvolutionFilter2D),
	glConvolutionFilter2DEXT(fallbackConvolutionFilter2DEXT),
	glConvolutionParameterf(fallbackConvolutionParameterf),
	glConvolutionParameterfEXT(fallbackConvolutionParameterfEXT),
	glConvolutionParameterfv(fallbackConvolutionParameterfv),
	glConvolutionParameterfvEXT(fallbackConvolutionParameterfvEXT),
	glConvolutionParameteri(fallbackConvolutionParameteri),
	glConvolutionParameteriEXT(fallbackConvolutionParameteriEXT),
	glConvolutionParameteriv(fallbackConvolutionParameteriv),
	glConvolutionParameterivEXT(fallbackConvolutionParameterivEXT),
	glCopyBufferSubData(fallbackCopyBufferSubData),
	glCopyColorSubTable(fallbackCopyColorSubTable),
	glCopyColorSubTableEXT(fallbackCopyColorSubTableEXT),
	glCopyColorTable(fallbackCopyColorTable),
	glCopyColorTableSGI(fallbackCopyColorTableSGI),
	glCopyConvolutionFilter1D(fallbackCopyConvolutionFilter1D),
	glCopyConvolutionFilter1DEXT(fallbackCopyConvolutionFilter1DEXT),
	glCopyConvolutionFilter2D(fallbackCopyConvolutionFilter2D),
	glCopyConvolutionFilter2DEXT(fallbackCopyConvolutionFilter2DEXT),
	glCopyImageSubDataNV(fallbackCopyImageSubDataNV),
	glCopyMultiTexImage1DEXT(fallbackCopyMultiTexImage1DEXT),
	glCopyMultiTexImage2DEXT(fallbackCopyMultiTexImage2DEXT),
	glCopyMultiTexSubImage1DEXT(fallbackCopyMultiTexSubImage1DEXT),
	glCopyMultiTexSubImage2DEXT(fallbackCopyMultiTexSubImage2DEXT),
	glCopyMultiTexSubImage3DEXT(fallbackCopyMultiTexSubImage3DEXT),
	glCopyPixels(fallbackCopyPixels),
	glCopyTexImage1D(fallbackCopyTexImage1D),
	glCopyTexImage1DEXT(fallbackCopyTexImage1DEXT),
	glCopyTexImage2D(fallbackCopyTexImage2D),
	glCopyTexImage2DEXT(fallbackCopyTexImage2DEXT),
	glCopyTexSubImage1D(fallbackCopyTexSubImage1D),
	glCopyTexSubImage1DEXT(fallbackCopyTexSubImage1DEXT),
	glCopyTexSubImage2D(fallbackCopyTexSubImage2D),
	glCopyTexSubImage2DEXT(fallbackCopyTexSubImage2DEXT),
	glCopyTexSubImage3D(fallbackCopyTexSubImage3D),
	glCopyTexSubImage3DEXT(fallbackCopyTexSubImage3DEXT),
	glCopyTextureImage1DEXT(fallbackCopyTextureImage1DEXT),
	glCopyTextureImage2DEXT(fallbackCopyTextureImage2DEXT),
	glCopyTextureSubImage1DEXT(fallbackCopyTextureSubImage1DEXT),
	glCopyTextureSubImage2DEXT(fallbackCopyTextureSubImage2DEXT),
	glCopyTextureSubImage3DEXT(fallbackCopyTextureSubImage3DEXT),
	glCreateProgram(fallbackCreateProgram),
	glCreateProgramObjectARB(fallbackCreateProgramObjectARB),
	glCreateShader(fallbackCreateShader),
	glCreateShaderObjectARB(fallbackCreateShaderObjectARB),
	glCreateShaderProgramEXT(fallbackCreateShaderProgramEXT),
	glCullFace(fallbackCullFace),
	glCullParameterdvEXT(fallbackCullParameterdvEXT),
	glCullParameterfvEXT(fallbackCullParameterfvEXT),
	glCurrentPaletteMatrixARB(fallbackCurrentPaletteMatrixARB),
	glDeformSGIX(fallbackDeformSGIX),
	glDeformationMap3dSGIX(fallbackDeformationMap3dSGIX),
	glDeformationMap3fSGIX(fallbackDeformationMap3fSGIX),
	glDeleteAsyncMarkersSGIX(fallbackDeleteAsyncMarkersSGIX),
	glDeleteBuffers(fallbackDeleteBuffers),
	glDeleteBuffersARB(fallbackDeleteBuffersARB),
	glDeleteFencesAPPLE(fallbackDeleteFencesAPPLE),
	glDeleteFencesNV(fallbackDeleteFencesNV),
	glDeleteFragmentShaderATI(fallbackDeleteFragmentShaderATI),
	glDeleteFramebuffers(fallbackDeleteFramebuffers),
	glDeleteFramebuffersEXT(fallbackDeleteFramebuffersEXT),
	glDeleteLists(fallbackDeleteLists),
	glDeleteNamedStringARB(fallbackDeleteNamedStringARB),
	glDeleteObjectARB(fallbackDeleteObjectARB),
	glDeleteOcclusionQueriesNV(fallbackDeleteOcclusionQueriesNV),
	glDeletePerfMonitorsAMD(fallbackDeletePerfMonitorsAMD),
	glDeleteProgram(fallbackDeleteProgram),
	glDeleteProgramsARB(fallbackDeleteProgramsARB),
	glDeleteProgramsNV(fallbackDeleteProgramsNV),
	glDeleteQueries(fallbackDeleteQueries),
	glDeleteQueriesARB(fallbackDeleteQueriesARB),
	glDeleteRenderbuffers(fallbackDeleteRenderbuffers),
	glDeleteRenderbuffersEXT(fallbackDeleteRenderbuffersEXT),
	glDeleteSamplers(fallbackDeleteSamplers),
	glDeleteShader(fallbackDeleteShader),
	glDeleteSync(fallbackDeleteSync),
	glDeleteTextures(fallbackDeleteTextures),
	glDeleteTexturesEXT(fallbackDeleteTexturesEXT),
	glDeleteTransformFeedbacks(fallbackDeleteTransformFeedbacks),
	glDeleteTransformFeedbacksNV(fallbackDeleteTransformFeedbacksNV),
	glDeleteVertexArrays(fallbackDeleteVertexArrays),
	glDeleteVertexArraysAPPLE(fallbackDeleteVertexArraysAPPLE),
	glDeleteVertexShaderEXT(fallbackDeleteVertexShaderEXT),
	glDepthBoundsEXT(fallbackDepthBoundsEXT),
	glDepthBoundsdNV(fallbackDepthBoundsdNV),
	glDepthFunc(fallbackDepthFunc),
	glDepthMask(fallbackDepthMask),
	glDepthRange(fallbackDepthRange),
	glDepthRangedNV(fallbackDepthRangedNV),
	glDetachObjectARB(fallbackDetachObjectARB),
	glDetachShader(fallbackDetachShader),
	glDetailTexFuncSGIS(fallbackDetailTexFuncSGIS),
	glDisable(fallbackDisable),
	glDisableClientState(fallbackDisableClientState),
	glDisableClientStateIndexedEXT(fallbackDisableClientStateIndexedEXT),
	glDisableIndexedEXT(fallbackDisableIndexedEXT),
	glDisableVariantClientStateEXT(fallbackDisableVariantClientStateEXT),
	glDisableVertexAttribAPPLE(fallbackDisableVertexAttribAPPLE),
	glDisableVertexAttribArray(fallbackDisableVertexAttribArray),
	glDisableVertexAttribArrayARB(fallbackDisableVertexAttribArrayARB),
	glDisablei(fallbackDisablei),
	glDrawArrays(fallbackDrawArrays),
	glDrawArraysEXT(fallbackDrawArraysEXT),
	glDrawArraysIndirect(fallbackDrawArraysIndirect),
	glDrawArraysInstanced(fallbackDrawArraysInstanced),
	glDrawArraysInstancedARB(fallbackDrawArraysInstancedARB),
	glDrawArraysInstancedEXT(fallbackDrawArraysInstancedEXT),
	glDrawBuffer(fallbackDrawBuffer),
	glDrawBuffers(fallbackDrawBuffers),
	glDrawBuffersARB(fallbackDrawBuffersARB),
	glDrawBuffersATI(fallbackDrawBuffersATI),
	glDrawElementArrayAPPLE(fallbackDrawElementArrayAPPLE),
	glDrawElementArrayATI(fallbackDrawElementArrayATI),
	glDrawElements(fallbackDrawElements),
	glDrawElementsBaseVertex(fallbackDrawElementsBaseVertex),
	glDrawElementsIndirect(fallbackDrawElementsIndirect),
	glDrawElementsInstanced(fallbackDrawElementsInstanced),
	glDrawElementsInstancedARB(fallbackDrawElementsInstancedARB),
	glDrawElementsInstancedBaseVertex(fallbackDrawElementsInstancedBaseVertex),
	glDrawElementsInstancedEXT(fallbackDrawElementsInstancedEXT),
	glDrawMeshArraysSUN(fallbackDrawMeshArraysSUN),
	glDrawPixels(fallbackDrawPixels),
	glDrawRangeElementArrayAPPLE(fallbackDrawRangeElementArrayAPPLE),
	glDrawRangeElementArrayATI(fallbackDrawRangeElementArrayATI),
	glDrawRangeElements(fallbackDrawRangeElements),
	glDrawRangeElementsBaseVertex(fallbackDrawRangeElementsBaseVertex),
	glDrawRangeElementsEXT(fallbackDrawRangeElementsEXT),
	glDrawTransformFeedback(fallbackDrawTransformFeedback),
	glDrawTransformFeedbackNV(fallbackDrawTransformFeedbackNV),
	glDrawTransformFeedbackStream(fallbackDrawTransformFeedbackStream),
	glEdgeFlag(fallbackEdgeFlag),
	glEdgeFlagFormatNV(fallbackEdgeFlagFormatNV),
	glEdgeFlagPointer(fallbackEdgeFlagPointer),
	glEdgeFlagPointerEXT(fallbackEdgeFlagPointerEXT),
	glEdgeFlagPointerListIBM(fallbackEdgeFlagPointerListIBM),
	glEdgeFlagv(fallbackEdgeFlagv),
	glElementPointerAPPLE(fallbackElementPointerAPPLE),
	glElementPointerATI(fallbackElementPointerATI),
	glEnable(fallbackEnable),
	glEnableClientState(fallbackEnableClientState),
	glEnableClientStateIndexedEXT(fallbackEnableClientStateIndexedEXT),
	glEnableIndexedEXT(fallbackEnableIndexedEXT),
	glEnableVariantClientStateEXT(fallbackEnableVariantClientStateEXT),
	glEnableVertexAttribAPPLE(fallbackEnableVertexAttribAPPLE),
	glEnableVertexAttribArray(fallbackEnableVertexAttribArray),
	glEnableVertexAttribArrayARB(fallbackEnableVertexAttribArrayARB),
	glEnablei(fallbackEnablei),
	glEnd(fallbackEnd),
	glEndConditionalRender(fallbackEndConditionalRender),
	glEndConditionalRenderNV(fallbackEndConditionalRenderNV),
	glEndFragmentShaderATI(fallbackEndFragmentShaderATI),
	glEndList(fallbackEndList),
	glEndOcclusionQueryNV(fallbackEndOcclusionQueryNV),
	glEndPerfMonitorAMD(fallbackEndPerfMonitorAMD),
	glEndQuery(fallbackEndQuery),
	glEndQueryARB(fallbackEndQueryARB),
	glEndQueryIndexed(fallbackEndQueryIndexed),
	glEndTransformFeedback(fallbackEndTransformFeedback),
	glEndTransformFeedbackEXT(fallbackEndTransformFeedbackEXT),
	glEndTransformFeedbackNV(fallbackEndTransformFeedbackNV),
	glEndVertexShaderEXT(fallbackEndVertexShaderEXT),
	glEndVideoCaptureNV(fallbackEndVideoCaptureNV),
	glEvalCoord1d(fallbackEvalCoord1d),
	glEvalCoord1dv(fallbackEvalCoord1dv),
	glEvalCoord1f(fallbackEvalCoord1f),
	glEvalCoord1fv(fallbackEvalCoord1fv),
	glEvalCoord2d(fallbackEvalCoord2d),
	glEvalCoord2dv(fallbackEvalCoord2dv),
	glEvalCoord2f(fallbackEvalCoord2f),
	glEvalCoord2fv(fallbackEvalCoord2fv),
	glEvalMapsNV(fallbackEvalMapsNV),
	glEvalMesh1(fallbackEvalMesh1),
	glEvalMesh2(fallbackEvalMesh2),
	glEvalPoint1(fallbackEvalPoint1),
	glEvalPoint2(fallbackEvalPoint2),
	glExecuteProgramNV(fallbackExecuteProgramNV),
	glExtractComponentEXT(fallbackExtractComponentEXT),
	glFeedbackBuffer(fallbackFeedbackBuffer),
	glFenceSync(fallbackFenceSync),
	glFinalCombinerInputNV(fallbackFinalCombinerInputNV),
	glFinish(fallbackFinish),
	glFinishAsyncSGIX(fallbackFinishAsyncSGIX),
	glFinishFenceAPPLE(fallbackFinishFenceAPPLE),
	glFinishFenceNV(fallbackFinishFenceNV),
	glFinishObjectAPPLE(fallbackFinishObjectAPPLE),
	glFinishTextureSUNX(fallbackFinishTextureSUNX),
	glFlush(fallbackFlush),
	glFlushMappedBufferRange(fallbackFlushMappedBufferRange),
	glFlushMappedBufferRangeAPPLE(fallbackFlushMappedBufferRangeAPPLE),
	glFlushPixelDataRangeNV(fallbackFlushPixelDataRangeNV),
	glFlushRasterSGIX(fallbackFlushRasterSGIX),
	glFlushVertexArrayRangeAPPLE(fallbackFlushVertexArrayRangeAPPLE),
	glFlushVertexArrayRangeNV(fallbackFlushVertexArrayRangeNV),
	glFogCoordFormatNV(fallbackFogCoordFormatNV),
	glFogCoordPointer(fallbackFogCoordPointer),
	glFogCoordPointerEXT(fallbackFogCoordPointerEXT),
	glFogCoordPointerListIBM(fallbackFogCoordPointerListIBM),
	glFogCoordd(fallbackFogCoordd),
	glFogCoorddEXT(fallbackFogCoorddEXT),
	glFogCoorddv(fallbackFogCoorddv),
	glFogCoorddvEXT(fallbackFogCoorddvEXT),
	glFogCoordf(fallbackFogCoordf),
	glFogCoordfEXT(fallbackFogCoordfEXT),
	glFogCoordfv(fallbackFogCoordfv),
	glFogCoordfvEXT(fallbackFogCoordfvEXT),
	glFogCoordhNV(fallbackFogCoordhNV),
	glFogCoordhvNV(fallbackFogCoordhvNV),
	glFogFuncSGIS(fallbackFogFuncSGIS),
	glFogf(fallbackFogf),
	glFogfv(fallbackFogfv),
	glFogi(fallbackFogi),
	glFogiv(fallbackFogiv),
	glFragmentColorMaterialSGIX(fallbackFragmentColorMaterialSGIX),
	glFragmentLightModelfSGIX(fallbackFragmentLightModelfSGIX),
	glFragmentLightModelfvSGIX(fallbackFragmentLightModelfvSGIX),
	glFragmentLightModeliSGIX(fallbackFragmentLightModeliSGIX),
	glFragmentLightModelivSGIX(fallbackFragmentLightModelivSGIX),
	glFragmentLightfSGIX(fallbackFragmentLightfSGIX),
	glFragmentLightfvSGIX(fallbackFragmentLightfvSGIX),
	glFragmentLightiSGIX(fallbackFragmentLightiSGIX),
	glFragmentLightivSGIX(fallbackFragmentLightivSGIX),
	glFragmentMaterialfSGIX(fallbackFragmentMaterialfSGIX),
	glFragmentMaterialfvSGIX(fallbackFragmentMaterialfvSGIX),
	glFragmentMaterialiSGIX(fallbackFragmentMaterialiSGIX),
	glFragmentMaterialivSGIX(fallbackFragmentMaterialivSGIX),
	glFrameTerminatorGREMEDY(fallbackFrameTerminatorGREMEDY),
	glFrameZoomSGIX(fallbackFrameZoomSGIX),
	glFramebufferDrawBufferEXT(fallbackFramebufferDrawBufferEXT),
	glFramebufferDrawBuffersEXT(fallbackFramebufferDrawBuffersEXT),
	glFramebufferReadBufferEXT(fallbackFramebufferReadBufferEXT),
	glFramebufferRenderbuffer(fallbackFramebufferRenderbuffer),
	glFramebufferRenderbufferEXT(fallbackFramebufferRenderbufferEXT),
	glFramebufferTexture(fallbackFramebufferTexture),
	glFramebufferTexture1D(fallbackFramebufferTexture1D),
	glFramebufferTexture1DEXT(fallbackFramebufferTexture1DEXT),
	glFramebufferTexture2D(fallbackFramebufferTexture2D),
	glFramebufferTexture2DEXT(fallbackFramebufferTexture2DEXT),
	glFramebufferTexture3D(fallbackFramebufferTexture3D),
	glFramebufferTexture3DEXT(fallbackFramebufferTexture3DEXT),
	glFramebufferTextureARB(fallbackFramebufferTextureARB),
	glFramebufferTextureEXT(fallbackFramebufferTextureEXT),
	glFramebufferTextureFaceARB(fallbackFramebufferTextureFaceARB),
	glFramebufferTextureFaceEXT(fallbackFramebufferTextureFaceEXT),
	glFramebufferTextureLayer(fallbackFramebufferTextureLayer),
	glFramebufferTextureLayerARB(fallbackFramebufferTextureLayerARB),
	glFramebufferTextureLayerEXT(fallbackFramebufferTextureLayerEXT),
	glFreeObjectBufferATI(fallbackFreeObjectBufferATI),
	glFrontFace(fallbackFrontFace),
	glFrustum(fallbackFrustum),
	glGenAsyncMarkersSGIX(fallbackGenAsyncMarkersSGIX),
	glGenBuffers(fallbackGenBuffers),
	glGenBuffersARB(fallbackGenBuffersARB),
	glGenFencesAPPLE(fallbackGenFencesAPPLE),
	glGenFencesNV(fallbackGenFencesNV),
	glGenFragmentShadersATI(fallbackGenFragmentShadersATI),
	glGenFramebuffers(fallbackGenFramebuffers),
	glGenFramebuffersEXT(fallbackGenFramebuffersEXT),
	glGenLists(fallbackGenLists),
	glGenOcclusionQueriesNV(fallbackGenOcclusionQueriesNV),
	glGenPerfMonitorsAMD(fallbackGenPerfMonitorsAMD),
	glGenProgramsARB(fallbackGenProgramsARB),
	glGenProgramsNV(fallbackGenProgramsNV),
	glGenQueries(fallbackGenQueries),
	glGenQueriesARB(fallbackGenQueriesARB),
	glGenRenderbuffers(fallbackGenRenderbuffers),
	glGenRenderbuffersEXT(fallbackGenRenderbuffersEXT),
	glGenSamplers(fallbackGenSamplers),
	glGenSymbolsEXT(fallbackGenSymbolsEXT),
	glGenTextures(fallbackGenTextures),
	glGenTexturesEXT(fallbackGenTexturesEXT),
	glGenTransformFeedbacks(fallbackGenTransformFeedbacks),
	glGenTransformFeedbacksNV(fallbackGenTransformFeedbacksNV),
	glGenVertexArrays(fallbackGenVertexArrays),
	glGenVertexArraysAPPLE(fallbackGenVertexArraysAPPLE),
	glGenVertexShadersEXT(fallbackGenVertexShadersEXT),
	glGenerateMipmap(fallbackGenerateMipmap),
	glGenerateMipmapEXT(fallbackGenerateMipmapEXT),
	glGenerateMultiTexMipmapEXT(fallbackGenerateMultiTexMipmapEXT),
	glGenerateTextureMipmapEXT(fallbackGenerateTextureMipmapEXT),
	glGetActiveAttrib(fallbackGetActiveAttrib),
	glGetActiveAttribARB(fallbackGetActiveAttribARB),
	glGetActiveSubroutineName(fallbackGetActiveSubroutineName),
	glGetActiveSubroutineUniformName(fallbackGetActiveSubroutineUniformName),
	glGetActiveSubroutineUniformiv(fallbackGetActiveSubroutineUniformiv),
	glGetActiveUniform(fallbackGetActiveUniform),
	glGetActiveUniformARB(fallbackGetActiveUniformARB),
	glGetActiveUniformBlockName(fallbackGetActiveUniformBlockName),
	glGetActiveUniformBlockiv(fallbackGetActiveUniformBlockiv),
	glGetActiveUniformName(fallbackGetActiveUniformName),
	glGetActiveUniformsiv(fallbackGetActiveUniformsiv),
	glGetActiveVaryingNV(fallbackGetActiveVaryingNV),
	glGetArrayObjectfvATI(fallbackGetArrayObjectfvATI),
	glGetArrayObjectivATI(fallbackGetArrayObjectivATI),
	glGetAttachedObjectsARB(fallbackGetAttachedObjectsARB),
	glGetAttachedShaders(fallbackGetAttachedShaders),
	glGetAttribLocation(fallbackGetAttribLocation),
	glGetAttribLocationARB(fallbackGetAttribLocationARB),
	glGetBooleanIndexedvEXT(fallbackGetBooleanIndexedvEXT),
	glGetBooleani_v(fallbackGetBooleani_v),
	glGetBooleanv(fallbackGetBooleanv),
	glGetBufferParameteri64v(fallbackGetBufferParameteri64v),
	glGetBufferParameteriv(fallbackGetBufferParameteriv),
	glGetBufferParameterivARB(fallbackGetBufferParameterivARB),
	glGetBufferParameterui64vNV(fallbackGetBufferParameterui64vNV),
	glGetBufferPointerv(fallbackGetBufferPointerv),
	glGetBufferPointervARB(fallbackGetBufferPointervARB),
	glGetBufferSubData(fallbackGetBufferSubData),
	glGetBufferSubDataARB(fallbackGetBufferSubDataARB),
	glGetClipPlane(fallbackGetClipPlane),
	glGetColorTable(fallbackGetColorTable),
	glGetColorTableEXT(fallbackGetColorTableEXT),
	glGetColorTableParameterfv(fallbackGetColorTableParameterfv),
	glGetColorTableParameterfvEXT(fallbackGetColorTableParameterfvEXT),
	glGetColorTableParameterfvSGI(fallbackGetColorTableParameterfvSGI),
	glGetColorTableParameteriv(fallbackGetColorTableParameteriv),
	glGetColorTableParameterivEXT(fallbackGetColorTableParameterivEXT),
	glGetColorTableParameterivSGI(fallbackGetColorTableParameterivSGI),
	glGetColorTableSGI(fallbackGetColorTableSGI),
	glGetCombinerInputParameterfvNV(fallbackGetCombinerInputParameterfvNV),
	glGetCombinerInputParameterivNV(fallbackGetCombinerInputParameterivNV),
	glGetCombinerOutputParameterfvNV(fallbackGetCombinerOutputParameterfvNV),
	glGetCombinerOutputParameterivNV(fallbackGetCombinerOutputParameterivNV),
	glGetCombinerStageParameterfvNV(fallbackGetCombinerStageParameterfvNV),
	glGetCompressedMultiTexImageEXT(fallbackGetCompressedMultiTexImageEXT),
	glGetCompressedTexImage(fallbackGetCompressedTexImage),
	glGetCompressedTexImageARB(fallbackGetCompressedTexImageARB),
	glGetCompressedTextureImageEXT(fallbackGetCompressedTextureImageEXT),
	glGetConvolutionFilter(fallbackGetConvolutionFilter),
	glGetConvolutionFilterEXT(fallbackGetConvolutionFilterEXT),
	glGetConvolutionParameterfv(fallbackGetConvolutionParameterfv),
	glGetConvolutionParameterfvEXT(fallbackGetConvolutionParameterfvEXT),
	glGetConvolutionParameteriv(fallbackGetConvolutionParameteriv),
	glGetConvolutionParameterivEXT(fallbackGetConvolutionParameterivEXT),
	glGetDetailTexFuncSGIS(fallbackGetDetailTexFuncSGIS),
	glGetDoubleIndexedvEXT(fallbackGetDoubleIndexedvEXT),
	glGetDoublev(fallbackGetDoublev),
	glGetError(fallbackGetError),
	glGetFenceivNV(fallbackGetFenceivNV),
	glGetFinalCombinerInputParameterfvNV(fallbackGetFinalCombinerInputParameterfvNV),
	glGetFinalCombinerInputParameterivNV(fallbackGetFinalCombinerInputParameterivNV),
	glGetFloatIndexedvEXT(fallbackGetFloatIndexedvEXT),
	glGetFloatv(fallbackGetFloatv),
	glGetFogFuncSGIS(fallbackGetFogFuncSGIS),
	glGetFragDataIndex(fallbackGetFragDataIndex),
	glGetFragDataLocation(fallbackGetFragDataLocation),
	glGetFragDataLocationEXT(fallbackGetFragDataLocationEXT),
	glGetFragmentLightfvSGIX(fallbackGetFragmentLightfvSGIX),
	glGetFragmentLightivSGIX(fallbackGetFragmentLightivSGIX),
	glGetFragmentMaterialfvSGIX(fallbackGetFragmentMaterialfvSGIX),
	glGetFragmentMaterialivSGIX(fallbackGetFragmentMaterialivSGIX),
	glGetFramebufferAttachmentParameteriv(fallbackGetFramebufferAttachmentParameteriv),
	glGetFramebufferAttachmentParameterivEXT(fallbackGetFramebufferAttachmentParameterivEXT),
	glGetFramebufferParameterivEXT(fallbackGetFramebufferParameterivEXT),
	glGetHandleARB(fallbackGetHandleARB),
	glGetHistogram(fallbackGetHistogram),
	glGetHistogramEXT(fallbackGetHistogramEXT),
	glGetHistogramParameterfv(fallbackGetHistogramParameterfv),
	glGetHistogramParameterfvEXT(fallbackGetHistogramParameterfvEXT),
	glGetHistogramParameteriv(fallbackGetHistogramParameteriv),
	glGetHistogramParameterivEXT(fallbackGetHistogramParameterivEXT),
	glGetImageTransformParameterfvHP(fallbackGetImageTransformParameterfvHP),
	glGetImageTransformParameterivHP(fallbackGetImageTransformParameterivHP),
	glGetInfoLogARB(fallbackGetInfoLogARB),
	glGetInstrumentsSGIX(fallbackGetInstrumentsSGIX),
	glGetInteger64i_v(fallbackGetInteger64i_v),
	glGetInteger64v(fallbackGetInteger64v),
	glGetIntegerIndexedvEXT(fallbackGetIntegerIndexedvEXT),
	glGetIntegeri_v(fallbackGetIntegeri_v),
	glGetIntegerui64i_vNV(fallbackGetIntegerui64i_vNV),
	glGetIntegerui64vNV(fallbackGetIntegerui64vNV),
	glGetIntegerv(fallbackGetIntegerv),
	glGetInvariantBooleanvEXT(fallbackGetInvariantBooleanvEXT),
	glGetInvariantFloatvEXT(fallbackGetInvariantFloatvEXT),
	glGetInvariantIntegervEXT(fallbackGetInvariantIntegervEXT),
	glGetLightfv(fallbackGetLightfv),
	glGetLightiv(fallbackGetLightiv),
	glGetListParameterfvSGIX(fallbackGetListParameterfvSGIX),
	glGetListParameterivSGIX(fallbackGetListParameterivSGIX),
	glGetLocalConstantBooleanvEXT(fallbackGetLocalConstantBooleanvEXT),
	glGetLocalConstantFloatvEXT(fallbackGetLocalConstantFloatvEXT),
	glGetLocalConstantIntegervEXT(fallbackGetLocalConstantIntegervEXT),
	glGetMapAttribParameterfvNV(fallbackGetMapAttribParameterfvNV),
	glGetMapAttribParameterivNV(fallbackGetMapAttribParameterivNV),
	glGetMapControlPointsNV(fallbackGetMapControlPointsNV),
	glGetMapParameterfvNV(fallbackGetMapParameterfvNV),
	glGetMapParameterivNV(fallbackGetMapParameterivNV),
	glGetMapdv(fallbackGetMapdv),
	glGetMapfv(fallbackGetMapfv),
	glGetMapiv(fallbackGetMapiv),
	glGetMaterialfv(fallbackGetMaterialfv),
	glGetMaterialiv(fallbackGetMaterialiv),
	glGetMinmax(fallbackGetMinmax),
	glGetMinmaxEXT(fallbackGetMinmaxEXT),
	glGetMinmaxParameterfv(fallbackGetMinmaxParameterfv),
	glGetMinmaxParameterfvEXT(fallbackGetMinmaxParameterfvEXT),
	glGetMinmaxParameteriv(fallbackGetMinmaxParameteriv),
	glGetMinmaxParameterivEXT(fallbackGetMinmaxParameterivEXT),
	glGetMultiTexEnvfvEXT(fallbackGetMultiTexEnvfvEXT),
	glGetMultiTexEnvivEXT(fallbackGetMultiTexEnvivEXT),
	glGetMultiTexGendvEXT(fallbackGetMultiTexGendvEXT),
	glGetMultiTexGenfvEXT(fallbackGetMultiTexGenfvEXT),
	glGetMultiTexGenivEXT(fallbackGetMultiTexGenivEXT),
	glGetMultiTexImageEXT(fallbackGetMultiTexImageEXT),
	glGetMultiTexLevelParameterfvEXT(fallbackGetMultiTexLevelParameterfvEXT),
	glGetMultiTexLevelParameterivEXT(fallbackGetMultiTexLevelParameterivEXT),
	glGetMultiTexParameterIivEXT(fallbackGetMultiTexParameterIivEXT),
	glGetMultiTexParameterIuivEXT(fallbackGetMultiTexParameterIuivEXT),
	glGetMultiTexParameterfvEXT(fallbackGetMultiTexParameterfvEXT),
	glGetMultiTexParameterivEXT(fallbackGetMultiTexParameterivEXT),
	glGetMultisamplefv(fallbackGetMultisamplefv),
	glGetMultisamplefvNV(fallbackGetMultisamplefvNV),
	glGetNamedBufferParameterivEXT(fallbackGetNamedBufferParameterivEXT),
	glGetNamedBufferParameterui64vNV(fallbackGetNamedBufferParameterui64vNV),
	glGetNamedBufferPointervEXT(fallbackGetNamedBufferPointervEXT),
	glGetNamedBufferSubDataEXT(fallbackGetNamedBufferSubDataEXT),
	glGetNamedFramebufferAttachmentParameterivEXT(fallbackGetNamedFramebufferAttachmentParameterivEXT),
	glGetNamedProgramLocalParameterIivEXT(fallbackGetNamedProgramLocalParameterIivEXT),
	glGetNamedProgramLocalParameterIuivEXT(fallbackGetNamedProgramLocalParameterIuivEXT),
	glGetNamedProgramLocalParameterdvEXT(fallbackGetNamedProgramLocalParameterdvEXT),
	glGetNamedProgramLocalParameterfvEXT(fallbackGetNamedProgramLocalParameterfvEXT),
	glGetNamedProgramStringEXT(fallbackGetNamedProgramStringEXT),
	glGetNamedProgramivEXT(fallbackGetNamedProgramivEXT),
	glGetNamedRenderbufferParameterivEXT(fallbackGetNamedRenderbufferParameterivEXT),
	glGetNamedStringARB(fallbackGetNamedStringARB),
	glGetNamedStringivARB(fallbackGetNamedStringivARB),
	glGetObjectBufferfvATI(fallbackGetObjectBufferfvATI),
	glGetObjectBufferivATI(fallbackGetObjectBufferivATI),
	glGetObjectParameterfvARB(fallbackGetObjectParameterfvARB),
	glGetObjectParameterivAPPLE(fallbackGetObjectParameterivAPPLE),
	glGetObjectParameterivARB(fallbackGetObjectParameterivARB),
	glGetOcclusionQueryivNV(fallbackGetOcclusionQueryivNV),
	glGetOcclusionQueryuivNV(fallbackGetOcclusionQueryuivNV),
	glGetPerfMonitorCounterDataAMD(fallbackGetPerfMonitorCounterDataAMD),
	glGetPerfMonitorCounterInfoAMD(fallbackGetPerfMonitorCounterInfoAMD),
	glGetPerfMonitorCounterStringAMD(fallbackGetPerfMonitorCounterStringAMD),
	glGetPerfMonitorCountersAMD(fallbackGetPerfMonitorCountersAMD),
	glGetPerfMonitorGroupStringAMD(fallbackGetPerfMonitorGroupStringAMD),
	glGetPerfMonitorGroupsAMD(fallbackGetPerfMonitorGroupsAMD),
	glGetPixelMapfv(fallbackGetPixelMapfv),
	glGetPixelMapuiv(fallbackGetPixelMapuiv),
	glGetPixelMapusv(fallbackGetPixelMapusv),
	glGetPixelTexGenParameterfvSGIS(fallbackGetPixelTexGenParameterfvSGIS),
	glGetPixelTexGenParameterivSGIS(fallbackGetPixelTexGenParameterivSGIS),
	glGetPointerIndexedvEXT(fallbackGetPointerIndexedvEXT),
	glGetPointerv(fallbackGetPointerv),
	glGetPointervEXT(fallbackGetPointervEXT),
	glGetPolygonStipple(fallbackGetPolygonStipple),
	glGetProgramEnvParameterIivNV(fallbackGetProgramEnvParameterIivNV),
	glGetProgramEnvParameterIuivNV(fallbackGetProgramEnvParameterIuivNV),
	glGetProgramEnvParameterdvARB(fallbackGetProgramEnvParameterdvARB),
	glGetProgramEnvParameterfvARB(fallbackGetProgramEnvParameterfvARB),
	glGetProgramInfoLog(fallbackGetProgramInfoLog),
	glGetProgramLocalParameterIivNV(fallbackGetProgramLocalParameterIivNV),
	glGetProgramLocalParameterIuivNV(fallbackGetProgramLocalParameterIuivNV),
	glGetProgramLocalParameterdvARB(fallbackGetProgramLocalParameterdvARB),
	glGetProgramLocalParameterfvARB(fallbackGetProgramLocalParameterfvARB),
	glGetProgramNamedParameterdvNV(fallbackGetProgramNamedParameterdvNV),
	glGetProgramNamedParameterfvNV(fallbackGetProgramNamedParameterfvNV),
	glGetProgramParameterdvNV(fallbackGetProgramParameterdvNV),
	glGetProgramParameterfvNV(fallbackGetProgramParameterfvNV),
	glGetProgramStageiv(fallbackGetProgramStageiv),
	glGetProgramStringARB(fallbackGetProgramStringARB),
	glGetProgramStringNV(fallbackGetProgramStringNV),
	glGetProgramiv(fallbackGetProgramiv),
	glGetProgramivARB(fallbackGetProgramivARB),
	glGetProgramivNV(fallbackGetProgramivNV),
	glGetQueryIndexediv(fallbackGetQueryIndexediv),
	glGetQueryObjecti64v(fallbackGetQueryObjecti64v),
	glGetQueryObjecti64vEXT(fallbackGetQueryObjecti64vEXT),
	glGetQueryObjectiv(fallbackGetQueryObjectiv),
	glGetQueryObjectivARB(fallbackGetQueryObjectivARB),
	glGetQueryObjectui64v(fallbackGetQueryObjectui64v),
	glGetQueryObjectui64vEXT(fallbackGetQueryObjectui64vEXT),
	glGetQueryObjectuiv(fallbackGetQueryObjectuiv),
	glGetQueryObjectuivARB(fallbackGetQueryObjectuivARB),
	glGetQueryiv(fallbackGetQueryiv),
	glGetQueryivARB(fallbackGetQueryivARB),
	glGetRenderbufferParameteriv(fallbackGetRenderbufferParameteriv),
	glGetRenderbufferParameterivEXT(fallbackGetRenderbufferParameterivEXT),
	glGetSamplerParameterIfv(fallbackGetSamplerParameterIfv),
	glGetSamplerParameterIiv(fallbackGetSamplerParameterIiv),
	glGetSamplerParameterfv(fallbackGetSamplerParameterfv),
	glGetSamplerParameteriv(fallbackGetSamplerParameteriv),
	glGetSeparableFilter(fallbackGetSeparableFilter),
	glGetSeparableFilterEXT(fallbackGetSeparableFilterEXT),
	glGetShaderInfoLog(fallbackGetShaderInfoLog),
	glGetShaderSource(fallbackGetShaderSource),
	glGetShaderSourceARB(fallbackGetShaderSourceARB),
	glGetShaderiv(fallbackGetShaderiv),
	glGetSharpenTexFuncSGIS(fallbackGetSharpenTexFuncSGIS),
	glGetString(fallbackGetString),
	glGetStringi(fallbackGetStringi),
	glGetSubroutineIndex(fallbackGetSubroutineIndex),
	glGetSubroutineUniformLocation(fallbackGetSubroutineUniformLocation),
	glGetSynciv(fallbackGetSynciv),
	glGetTexBumpParameterfvATI(fallbackGetTexBumpParameterfvATI),
	glGetTexBumpParameterivATI(fallbackGetTexBumpParameterivATI),
	glGetTexEnvfv(fallbackGetTexEnvfv),
	glGetTexEnviv(fallbackGetTexEnviv),
	glGetTexFilterFuncSGIS(fallbackGetTexFilterFuncSGIS),
	glGetTexGendv(fallbackGetTexGendv),
	glGetTexGenfv(fallbackGetTexGenfv),
	glGetTexGeniv(fallbackGetTexGeniv),
	glGetTexImage(fallbackGetTexImage),
	glGetTexLevelParameterfv(fallbackGetTexLevelParameterfv),
	glGetTexLevelParameteriv(fallbackGetTexLevelParameteriv),
	glGetTexParameterIiv(fallbackGetTexParameterIiv),
	glGetTexParameterIivEXT(fallbackGetTexParameterIivEXT),
	glGetTexParameterIuiv(fallbackGetTexParameterIuiv),
	glGetTexParameterIuivEXT(fallbackGetTexParameterIuivEXT),
	glGetTexParameterPointervAPPLE(fallbackGetTexParameterPointervAPPLE),
	glGetTexParameterfv(fallbackGetTexParameterfv),
	glGetTexParameteriv(fallbackGetTexParameteriv),
	glGetTextureImageEXT(fallbackGetTextureImageEXT),
	glGetTextureLevelParameterfvEXT(fallbackGetTextureLevelParameterfvEXT),
	glGetTextureLevelParameterivEXT(fallbackGetTextureLevelParameterivEXT),
	glGetTextureParameterIivEXT(fallbackGetTextureParameterIivEXT),
	glGetTextureParameterIuivEXT(fallbackGetTextureParameterIuivEXT),
	glGetTextureParameterfvEXT(fallbackGetTextureParameterfvEXT),
	glGetTextureParameterivEXT(fallbackGetTextureParameterivEXT),
	glGetTrackMatrixivNV(fallbackGetTrackMatrixivNV),
	glGetTransformFeedbackVarying(fallbackGetTransformFeedbackVarying),
	glGetTransformFeedbackVaryingEXT(fallbackGetTransformFeedbackVaryingEXT),
	glGetTransformFeedbackVaryingNV(fallbackGetTransformFeedbackVaryingNV),
	glGetUniformBlockIndex(fallbackGetUniformBlockIndex),
	glGetUniformBufferSizeEXT(fallbackGetUniformBufferSizeEXT),
	glGetUniformIndices(fallbackGetUniformIndices),
	glGetUniformLocation(fallbackGetUniformLocation),
	glGetUniformLocationARB(fallbackGetUniformLocationARB),
	glGetUniformOffsetEXT(fallbackGetUniformOffsetEXT),
	glGetUniformSubroutineuiv(fallbackGetUniformSubroutineuiv),
	glGetUniformdv(fallbackGetUniformdv),
	glGetUniformfv(fallbackGetUniformfv),
	glGetUniformfvARB(fallbackGetUniformfvARB),
	glGetUniformiv(fallbackGetUniformiv),
	glGetUniformivARB(fallbackGetUniformivARB),
	glGetUniformui64vNV(fallbackGetUniformui64vNV),
	glGetUniformuiv(fallbackGetUniformuiv),
	glGetUniformuivEXT(fallbackGetUniformuivEXT),
	glGetVariantArrayObjectfvATI(fallbackGetVariantArrayObjectfvATI),
	glGetVariantArrayObjectivATI(fallbackGetVariantArrayObjectivATI),
	glGetVariantBooleanvEXT(fallbackGetVariantBooleanvEXT),
	glGetVariantFloatvEXT(fallbackGetVariantFloatvEXT),
	glGetVariantIntegervEXT(fallbackGetVariantIntegervEXT),
	glGetVariantPointervEXT(fallbackGetVariantPointervEXT),
	glGetVaryingLocationNV(fallbackGetVaryingLocationNV),
	glGetVertexAttribArrayObjectfvATI(fallbackGetVertexAttribArrayObjectfvATI),
	glGetVertexAttribArrayObjectivATI(fallbackGetVertexAttribArrayObjectivATI),
	glGetVertexAttribIiv(fallbackGetVertexAttribIiv),
	glGetVertexAttribIivEXT(fallbackGetVertexAttribIivEXT),
	glGetVertexAttribIuiv(fallbackGetVertexAttribIuiv),
	glGetVertexAttribIuivEXT(fallbackGetVertexAttribIuivEXT),
	glGetVertexAttribPointerv(fallbackGetVertexAttribPointerv),
	glGetVertexAttribPointervARB(fallbackGetVertexAttribPointervARB),
	glGetVertexAttribPointervNV(fallbackGetVertexAttribPointervNV),
	glGetVertexAttribdv(fallbackGetVertexAttribdv),
	glGetVertexAttribdvARB(fallbackGetVertexAttribdvARB),
	glGetVertexAttribdvNV(fallbackGetVertexAttribdvNV),
	glGetVertexAttribfv(fallbackGetVertexAttribfv),
	glGetVertexAttribfvARB(fallbackGetVertexAttribfvARB),
	glGetVertexAttribfvNV(fallbackGetVertexAttribfvNV),
	glGetVertexAttribiv(fallbackGetVertexAttribiv),
	glGetVertexAttribivARB(fallbackGetVertexAttribivARB),
	glGetVertexAttribivNV(fallbackGetVertexAttribivNV),
	glGetVideoCaptureStreamdvNV(fallbackGetVideoCaptureStreamdvNV),
	glGetVideoCaptureStreamfvNV(fallbackGetVideoCaptureStreamfvNV),
	glGetVideoCaptureStreamivNV(fallbackGetVideoCaptureStreamivNV),
	glGetVideoCaptureivNV(fallbackGetVideoCaptureivNV),
	glGetVideoi64vNV(fallbackGetVideoi64vNV),
	glGetVideoivNV(fallbackGetVideoivNV),
	glGetVideoui64vNV(fallbackGetVideoui64vNV),
	glGetVideouivNV(fallbackGetVideouivNV),
	glGlobalAlphaFactorbSUN(fallbackGlobalAlphaFactorbSUN),
	glGlobalAlphaFactordSUN(fallbackGlobalAlphaFactordSUN),
	glGlobalAlphaFactorfSUN(fallbackGlobalAlphaFactorfSUN),
	glGlobalAlphaFactoriSUN(fallbackGlobalAlphaFactoriSUN),
	glGlobalAlphaFactorsSUN(fallbackGlobalAlphaFactorsSUN),
	glGlobalAlphaFactorubSUN(fallbackGlobalAlphaFactorubSUN),
	glGlobalAlphaFactoruiSUN(fallbackGlobalAlphaFactoruiSUN),
	glGlobalAlphaFactorusSUN(fallbackGlobalAlphaFactorusSUN),
	glHint(fallbackHint),
	glHintPGI(fallbackHintPGI),
	glHistogram(fallbackHistogram),
	glHistogramEXT(fallbackHistogramEXT),
	glIglooInterfaceSGIX(fallbackIglooInterfaceSGIX),
	glImageTransformParameterfHP(fallbackImageTransformParameterfHP),
	glImageTransformParameterfvHP(fallbackImageTransformParameterfvHP),
	glImageTransformParameteriHP(fallbackImageTransformParameteriHP),
	glImageTransformParameterivHP(fallbackImageTransformParameterivHP),
	glIndexFormatNV(fallbackIndexFormatNV),
	glIndexFuncEXT(fallbackIndexFuncEXT),
	glIndexMask(fallbackIndexMask),
	glIndexMaterialEXT(fallbackIndexMaterialEXT),
	glIndexPointer(fallbackIndexPointer),
	glIndexPointerEXT(fallbackIndexPointerEXT),
	glIndexPointerListIBM(fallbackIndexPointerListIBM),
	glIndexd(fallbackIndexd),
	glIndexdv(fallbackIndexdv),
	glIndexf(fallbackIndexf),
	glIndexfv(fallbackIndexfv),
	glIndexi(fallbackIndexi),
	glIndexiv(fallbackIndexiv),
	glIndexs(fallbackIndexs),
	glIndexsv(fallbackIndexsv),
	glIndexub(fallbackIndexub),
	glIndexubv(fallbackIndexubv),
	glInitNames(fallbackInitNames),
	glInsertComponentEXT(fallbackInsertComponentEXT),
	glInstrumentsBufferSGIX(fallbackInstrumentsBufferSGIX),
	glInterleavedArrays(fallbackInterleavedArrays),
	glIsAsyncMarkerSGIX(fallbackIsAsyncMarkerSGIX),
	glIsBuffer(fallbackIsBuffer),
	glIsBufferARB(fallbackIsBufferARB),
	glIsBufferResidentNV(fallbackIsBufferResidentNV),
	glIsEnabled(fallbackIsEnabled),
	glIsEnabledIndexedEXT(fallbackIsEnabledIndexedEXT),
	glIsEnabledi(fallbackIsEnabledi),
	glIsFenceAPPLE(fallbackIsFenceAPPLE),
	glIsFenceNV(fallbackIsFenceNV),
	glIsFramebuffer(fallbackIsFramebuffer),
	glIsFramebufferEXT(fallbackIsFramebufferEXT),
	glIsList(fallbackIsList),
	glIsNamedBufferResidentNV(fallbackIsNamedBufferResidentNV),
	glIsNamedStringARB(fallbackIsNamedStringARB),
	glIsObjectBufferATI(fallbackIsObjectBufferATI),
	glIsOcclusionQueryNV(fallbackIsOcclusionQueryNV),
	glIsProgram(fallbackIsProgram),
	glIsProgramARB(fallbackIsProgramARB),
	glIsProgramNV(fallbackIsProgramNV),
	glIsQuery(fallbackIsQuery),
	glIsQueryARB(fallbackIsQueryARB),
	glIsRenderbuffer(fallbackIsRenderbuffer),
	glIsRenderbufferEXT(fallbackIsRenderbufferEXT),
	glIsSampler(fallbackIsSampler),
	glIsShader(fallbackIsShader),
	glIsSync(fallbackIsSync),
	glIsTexture(fallbackIsTexture),
	glIsTextureEXT(fallbackIsTextureEXT),
	glIsTransformFeedback(fallbackIsTransformFeedback),
	glIsTransformFeedbackNV(fallbackIsTransformFeedbackNV),
	glIsVariantEnabledEXT(fallbackIsVariantEnabledEXT),
	glIsVertexArray(fallbackIsVertexArray),
	glIsVertexArrayAPPLE(fallbackIsVertexArrayAPPLE),
	glIsVertexAttribEnabledAPPLE(fallbackIsVertexAttribEnabledAPPLE),
	glLightEnviSGIX(fallbackLightEnviSGIX),
	glLightModelf(fallbackLightModelf),
	glLightModelfv(fallbackLightModelfv),
	glLightModeli(fallbackLightModeli),
	glLightModeliv(fallbackLightModeliv),
	glLightf(fallbackLightf),
	glLightfv(fallbackLightfv),
	glLighti(fallbackLighti),
	glLightiv(fallbackLightiv),
	glLineStipple(fallbackLineStipple),
	glLineWidth(fallbackLineWidth),
	glLinkProgram(fallbackLinkProgram),
	glLinkProgramARB(fallbackLinkProgramARB),
	glListBase(fallbackListBase),
	glListParameterfSGIX(fallbackListParameterfSGIX),
	glListParameterfvSGIX(fallbackListParameterfvSGIX),
	glListParameteriSGIX(fallbackListParameteriSGIX),
	glListParameterivSGIX(fallbackListParameterivSGIX),
	glLoadIdentity(fallbackLoadIdentity),
	glLoadIdentityDeformationMapSGIX(fallbackLoadIdentityDeformationMapSGIX),
	glLoadMatrixd(fallbackLoadMatrixd),
	glLoadMatrixf(fallbackLoadMatrixf),
	glLoadName(fallbackLoadName),
	glLoadProgramNV(fallbackLoadProgramNV),
	glLoadTransposeMatrixd(fallbackLoadTransposeMatrixd),
	glLoadTransposeMatrixdARB(fallbackLoadTransposeMatrixdARB),
	glLoadTransposeMatrixf(fallbackLoadTransposeMatrixf),
	glLoadTransposeMatrixfARB(fallbackLoadTransposeMatrixfARB),
	glLockArraysEXT(fallbackLockArraysEXT),
	glLogicOp(fallbackLogicOp),
	glMakeBufferNonResidentNV(fallbackMakeBufferNonResidentNV),
	glMakeBufferResidentNV(fallbackMakeBufferResidentNV),
	glMakeNamedBufferNonResidentNV(fallbackMakeNamedBufferNonResidentNV),
	glMakeNamedBufferResidentNV(fallbackMakeNamedBufferResidentNV),
	glMap1d(fallbackMap1d),
	glMap1f(fallbackMap1f),
	glMap2d(fallbackMap2d),
	glMap2f(fallbackMap2f),
	glMapBuffer(fallbackMapBuffer),
	glMapBufferARB(fallbackMapBufferARB),
	glMapBufferRange(fallbackMapBufferRange),
	glMapControlPointsNV(fallbackMapControlPointsNV),
	glMapGrid1d(fallbackMapGrid1d),
	glMapGrid1f(fallbackMapGrid1f),
	glMapGrid2d(fallbackMapGrid2d),
	glMapGrid2f(fallbackMapGrid2f),
	glMapNamedBufferEXT(fallbackMapNamedBufferEXT),
	glMapObjectBufferATI(fallbackMapObjectBufferATI),
	glMapParameterfvNV(fallbackMapParameterfvNV),
	glMapParameterivNV(fallbackMapParameterivNV),
	glMapVertexAttrib1dAPPLE(fallbackMapVertexAttrib1dAPPLE),
	glMapVertexAttrib1fAPPLE(fallbackMapVertexAttrib1fAPPLE),
	glMapVertexAttrib2dAPPLE(fallbackMapVertexAttrib2dAPPLE),
	glMapVertexAttrib2fAPPLE(fallbackMapVertexAttrib2fAPPLE),
	glMaterialf(fallbackMaterialf),
	glMaterialfv(fallbackMaterialfv),
	glMateriali(fallbackMateriali),
	glMaterialiv(fallbackMaterialiv),
	glMatrixFrustumEXT(fallbackMatrixFrustumEXT),
	glMatrixIndexPointerARB(fallbackMatrixIndexPointerARB),
	glMatrixIndexubvARB(fallbackMatrixIndexubvARB),
	glMatrixIndexuivARB(fallbackMatrixIndexuivARB),
	glMatrixIndexusvARB(fallbackMatrixIndexusvARB),
	glMatrixLoadIdentityEXT(fallbackMatrixLoadIdentityEXT),
	glMatrixLoadTransposedEXT(fallbackMatrixLoadTransposedEXT),
	glMatrixLoadTransposefEXT(fallbackMatrixLoadTransposefEXT),
	glMatrixLoaddEXT(fallbackMatrixLoaddEXT),
	glMatrixLoadfEXT(fallbackMatrixLoadfEXT),
	glMatrixMode(fallbackMatrixMode),
	glMatrixMultTransposedEXT(fallbackMatrixMultTransposedEXT),
	glMatrixMultTransposefEXT(fallbackMatrixMultTransposefEXT),
	glMatrixMultdEXT(fallbackMatrixMultdEXT),
	glMatrixMultfEXT(fallbackMatrixMultfEXT),
	glMatrixOrthoEXT(fallbackMatrixOrthoEXT),
	glMatrixPopEXT(fallbackMatrixPopEXT),
	glMatrixPushEXT(fallbackMatrixPushEXT),
	glMatrixRotatedEXT(fallbackMatrixRotatedEXT),
	glMatrixRotatefEXT(fallbackMatrixRotatefEXT),
	glMatrixScaledEXT(fallbackMatrixScaledEXT),
	glMatrixScalefEXT(fallbackMatrixScalefEXT),
	glMatrixTranslatedEXT(fallbackMatrixTranslatedEXT),
	glMatrixTranslatefEXT(fallbackMatrixTranslatefEXT),
	glMinSampleShading(fallbackMinSampleShading),
	glMinmax(fallbackMinmax),
	glMinmaxEXT(fallbackMinmaxEXT),
	glMultMatrixd(fallbackMultMatrixd),
	glMultMatrixf(fallbackMultMatrixf),
	glMultTransposeMatrixd(fallbackMultTransposeMatrixd),
	glMultTransposeMatrixdARB(fallbackMultTransposeMatrixdARB),
	glMultTransposeMatrixf(fallbackMultTransposeMatrixf),
	glMultTransposeMatrixfARB(fallbackMultTransposeMatrixfARB),
	glMultiDrawArrays(fallbackMultiDrawArrays),
	glMultiDrawArraysEXT(fallbackMultiDrawArraysEXT),
	glMultiDrawElementArrayAPPLE(fallbackMultiDrawElementArrayAPPLE),
	glMultiDrawElements(fallbackMultiDrawElements),
	glMultiDrawElementsBaseVertex(fallbackMultiDrawElementsBaseVertex),
	glMultiDrawElementsEXT(fallbackMultiDrawElementsEXT),
	glMultiDrawRangeElementArrayAPPLE(fallbackMultiDrawRangeElementArrayAPPLE),
	glMultiModeDrawArraysIBM(fallbackMultiModeDrawArraysIBM),
	glMultiModeDrawElementsIBM(fallbackMultiModeDrawElementsIBM),
	glMultiTexBufferEXT(fallbackMultiTexBufferEXT),
	glMultiTexCoord1d(fallbackMultiTexCoord1d),
	glMultiTexCoord1dARB(fallbackMultiTexCoord1dARB),
	glMultiTexCoord1dv(fallbackMultiTexCoord1dv),
	glMultiTexCoord1dvARB(fallbackMultiTexCoord1dvARB),
	glMultiTexCoord1f(fallbackMultiTexCoord1f),
	glMultiTexCoord1fARB(fallbackMultiTexCoord1fARB),
	glMultiTexCoord1fv(fallbackMultiTexCoord1fv),
	glMultiTexCoord1fvARB(fallbackMultiTexCoord1fvARB),
	glMultiTexCoord1hNV(fallbackMultiTexCoord1hNV),
	glMultiTexCoord1hvNV(fallbackMultiTexCoord1hvNV),
	glMultiTexCoord1i(fallbackMultiTexCoord1i),
	glMultiTexCoord1iARB(fallbackMultiTexCoord1iARB),
	glMultiTexCoord1iv(fallbackMultiTexCoord1iv),
	glMultiTexCoord1ivARB(fallbackMultiTexCoord1ivARB),
	glMultiTexCoord1s(fallbackMultiTexCoord1s),
	glMultiTexCoord1sARB(fallbackMultiTexCoord1sARB),
	glMultiTexCoord1sv(fallbackMultiTexCoord1sv),
	glMultiTexCoord1svARB(fallbackMultiTexCoord1svARB),
	glMultiTexCoord2d(fallbackMultiTexCoord2d),
	glMultiTexCoord2dARB(fallbackMultiTexCoord2dARB),
	glMultiTexCoord2dv(fallbackMultiTexCoord2dv),
	glMultiTexCoord2dvARB(fallbackMultiTexCoord2dvARB),
	glMultiTexCoord2f(fallbackMultiTexCoord2f),
	glMultiTexCoord2fARB(fallbackMultiTexCoord2fARB),
	glMultiTexCoord2fv(fallbackMultiTexCoord2fv),
	glMultiTexCoord2fvARB(fallbackMultiTexCoord2fvARB),
	glMultiTexCoord2hNV(fallbackMultiTexCoord2hNV),
	glMultiTexCoord2hvNV(fallbackMultiTexCoord2hvNV),
	glMultiTexCoord2i(fallbackMultiTexCoord2i),
	glMultiTexCoord2iARB(fallbackMultiTexCoord2iARB),
	glMultiTexCoord2iv(fallbackMultiTexCoord2iv),
	glMultiTexCoord2ivARB(fallbackMultiTexCoord2ivARB),
	glMultiTexCoord2s(fallbackMultiTexCoord2s),
	glMultiTexCoord2sARB(fallbackMultiTexCoord2sARB),
	glMultiTexCoord2sv(fallbackMultiTexCoord2sv),
	glMultiTexCoord2svARB(fallbackMultiTexCoord2svARB),
	glMultiTexCoord3d(fallbackMultiTexCoord3d),
	glMultiTexCoord3dARB(fallbackMultiTexCoord3dARB),
	glMultiTexCoord3dv(fallbackMultiTexCoord3dv),
	glMultiTexCoord3dvARB(fallbackMultiTexCoord3dvARB),
	glMultiTexCoord3f(fallbackMultiTexCoord3f),
	glMultiTexCoord3fARB(fallbackMultiTexCoord3fARB),
	glMultiTexCoord3fv(fallbackMultiTexCoord3fv),
	glMultiTexCoord3fvARB(fallbackMultiTexCoord3fvARB),
	glMultiTexCoord3hNV(fallbackMultiTexCoord3hNV),
	glMultiTexCoord3hvNV(fallbackMultiTexCoord3hvNV),
	glMultiTexCoord3i(fallbackMultiTexCoord3i),
	glMultiTexCoord3iARB(fallbackMultiTexCoord3iARB),
	glMultiTexCoord3iv(fallbackMultiTexCoord3iv),
	glMultiTexCoord3ivARB(fallbackMultiTexCoord3ivARB),
	glMultiTexCoord3s(fallbackMultiTexCoord3s),
	glMultiTexCoord3sARB(fallbackMultiTexCoord3sARB),
	glMultiTexCoord3sv(fallbackMultiTexCoord3sv),
	glMultiTexCoord3svARB(fallbackMultiTexCoord3svARB),
	glMultiTexCoord4d(fallbackMultiTexCoord4d),
	glMultiTexCoord4dARB(fallbackMultiTexCoord4dARB),
	glMultiTexCoord4dv(fallbackMultiTexCoord4dv),
	glMultiTexCoord4dvARB(fallbackMultiTexCoord4dvARB),
	glMultiTexCoord4f(fallbackMultiTexCoord4f),
	glMultiTexCoord4fARB(fallbackMultiTexCoord4fARB),
	glMultiTexCoord4fv(fallbackMultiTexCoord4fv),
	glMultiTexCoord4fvARB(fallbackMultiTexCoord4fvARB),
	glMultiTexCoord4hNV(fallbackMultiTexCoord4hNV),
	glMultiTexCoord4hvNV(fallbackMultiTexCoord4hvNV),
	glMultiTexCoord4i(fallbackMultiTexCoord4i),
	glMultiTexCoord4iARB(fallbackMultiTexCoord4iARB),
	glMultiTexCoord4iv(fallbackMultiTexCoord4iv),
	glMultiTexCoord4ivARB(fallbackMultiTexCoord4ivARB),
	glMultiTexCoord4s(fallbackMultiTexCoord4s),
	glMultiTexCoord4sARB(fallbackMultiTexCoord4sARB),
	glMultiTexCoord4sv(fallbackMultiTexCoord4sv),
	glMultiTexCoord4svARB(fallbackMultiTexCoord4svARB),
	glMultiTexCoordP1ui(fallbackMultiTexCoordP1ui),
	glMultiTexCoordP1uiv(fallbackMultiTexCoordP1uiv),
	glMultiTexCoordP2ui(fallbackMultiTexCoordP2ui),
	glMultiTexCoordP2uiv(fallbackMultiTexCoordP2uiv),
	glMultiTexCoordP3ui(fallbackMultiTexCoordP3ui),
	glMultiTexCoordP3uiv(fallbackMultiTexCoordP3uiv),
	glMultiTexCoordP4ui(fallbackMultiTexCoordP4ui),
	glMultiTexCoordP4uiv(fallbackMultiTexCoordP4uiv),
	glMultiTexCoordPointerEXT(fallbackMultiTexCoordPointerEXT),
	glMultiTexEnvfEXT(fallbackMultiTexEnvfEXT),
	glMultiTexEnvfvEXT(fallbackMultiTexEnvfvEXT),
	glMultiTexEnviEXT(fallbackMultiTexEnviEXT),
	glMultiTexEnvivEXT(fallbackMultiTexEnvivEXT),
	glMultiTexGendEXT(fallbackMultiTexGendEXT),
	glMultiTexGendvEXT(fallbackMultiTexGendvEXT),
	glMultiTexGenfEXT(fallbackMultiTexGenfEXT),
	glMultiTexGenfvEXT(fallbackMultiTexGenfvEXT),
	glMultiTexGeniEXT(fallbackMultiTexGeniEXT),
	glMultiTexGenivEXT(fallbackMultiTexGenivEXT),
	glMultiTexImage1DEXT(fallbackMultiTexImage1DEXT),
	glMultiTexImage2DEXT(fallbackMultiTexImage2DEXT),
	glMultiTexImage3DEXT(fallbackMultiTexImage3DEXT),
	glMultiTexParameterIivEXT(fallbackMultiTexParameterIivEXT),
	glMultiTexParameterIuivEXT(fallbackMultiTexParameterIuivEXT),
	glMultiTexParameterfEXT(fallbackMultiTexParameterfEXT),
	glMultiTexParameterfvEXT(fallbackMultiTexParameterfvEXT),
	glMultiTexParameteriEXT(fallbackMultiTexParameteriEXT),
	glMultiTexParameterivEXT(fallbackMultiTexParameterivEXT),
	glMultiTexRenderbufferEXT(fallbackMultiTexRenderbufferEXT),
	glMultiTexSubImage1DEXT(fallbackMultiTexSubImage1DEXT),
	glMultiTexSubImage2DEXT(fallbackMultiTexSubImage2DEXT),
	glMultiTexSubImage3DEXT(fallbackMultiTexSubImage3DEXT),
	glNamedBufferDataEXT(fallbackNamedBufferDataEXT),
	glNamedBufferSubDataEXT(fallbackNamedBufferSubDataEXT),
	glNamedFramebufferRenderbufferEXT(fallbackNamedFramebufferRenderbufferEXT),
	glNamedFramebufferTexture1DEXT(fallbackNamedFramebufferTexture1DEXT),
	glNamedFramebufferTexture2DEXT(fallbackNamedFramebufferTexture2DEXT),
	glNamedFramebufferTexture3DEXT(fallbackNamedFramebufferTexture3DEXT),
	glNamedFramebufferTextureEXT(fallbackNamedFramebufferTextureEXT),
	glNamedFramebufferTextureFaceEXT(fallbackNamedFramebufferTextureFaceEXT),
	glNamedFramebufferTextureLayerEXT(fallbackNamedFramebufferTextureLayerEXT),
	glNamedProgramLocalParameter4dEXT(fallbackNamedProgramLocalParameter4dEXT),
	glNamedProgramLocalParameter4dvEXT(fallbackNamedProgramLocalParameter4dvEXT),
	glNamedProgramLocalParameter4fEXT(fallbackNamedProgramLocalParameter4fEXT),
	glNamedProgramLocalParameter4fvEXT(fallbackNamedProgramLocalParameter4fvEXT),
	glNamedProgramLocalParameterI4iEXT(fallbackNamedProgramLocalParameterI4iEXT),
	glNamedProgramLocalParameterI4ivEXT(fallbackNamedProgramLocalParameterI4ivEXT),
	glNamedProgramLocalParameterI4uiEXT(fallbackNamedProgramLocalParameterI4uiEXT),
	glNamedProgramLocalParameterI4uivEXT(fallbackNamedProgramLocalParameterI4uivEXT),
	glNamedProgramLocalParameters4fvEXT(fallbackNamedProgramLocalParameters4fvEXT),
	glNamedProgramLocalParametersI4ivEXT(fallbackNamedProgramLocalParametersI4ivEXT),
	glNamedProgramLocalParametersI4uivEXT(fallbackNamedProgramLocalParametersI4uivEXT),
	glNamedProgramStringEXT(fallbackNamedProgramStringEXT),
	glNamedRenderbufferStorageEXT(fallbackNamedRenderbufferStorageEXT),
	glNamedRenderbufferStorageMultisampleCoverageEXT(fallbackNamedRenderbufferStorageMultisampleCoverageEXT),
	glNamedRenderbufferStorageMultisampleEXT(fallbackNamedRenderbufferStorageMultisampleEXT),
	glNamedStringARB(fallbackNamedStringARB),
	glNewList(fallbackNewList),
	glNewObjectBufferATI(fallbackNewObjectBufferATI),
	glNormal3b(fallbackNormal3b),
	glNormal3bv(fallbackNormal3bv),
	glNormal3d(fallbackNormal3d),
	glNormal3dv(fallbackNormal3dv),
	glNormal3f(fallbackNormal3f),
	glNormal3fVertex3fSUN(fallbackNormal3fVertex3fSUN),
	glNormal3fVertex3fvSUN(fallbackNormal3fVertex3fvSUN),
	glNormal3fv(fallbackNormal3fv),
	glNormal3hNV(fallbackNormal3hNV),
	glNormal3hvNV(fallbackNormal3hvNV),
	glNormal3i(fallbackNormal3i),
	glNormal3iv(fallbackNormal3iv),
	glNormal3s(fallbackNormal3s),
	glNormal3sv(fallbackNormal3sv),
	glNormalFormatNV(fallbackNormalFormatNV),
	glNormalP3ui(fallbackNormalP3ui),
	glNormalP3uiv(fallbackNormalP3uiv),
	glNormalPointer(fallbackNormalPointer),
	glNormalPointerEXT(fallbackNormalPointerEXT),
	glNormalPointerListIBM(fallbackNormalPointerListIBM),
	glNormalPointervINTEL(fallbackNormalPointervINTEL),
	glNormalStream3bATI(fallbackNormalStream3bATI),
	glNormalStream3bvATI(fallbackNormalStream3bvATI),
	glNormalStream3dATI(fallbackNormalStream3dATI),
	glNormalStream3dvATI(fallbackNormalStream3dvATI),
	glNormalStream3fATI(fallbackNormalStream3fATI),
	glNormalStream3fvATI(fallbackNormalStream3fvATI),
	glNormalStream3iATI(fallbackNormalStream3iATI),
	glNormalStream3ivATI(fallbackNormalStream3ivATI),
	glNormalStream3sATI(fallbackNormalStream3sATI),
	glNormalStream3svATI(fallbackNormalStream3svATI),
	glObjectPurgeableAPPLE(fallbackObjectPurgeableAPPLE),
	glObjectUnpurgeableAPPLE(fallbackObjectUnpurgeableAPPLE),
	glOrtho(fallbackOrtho),
	glPNTrianglesfATI(fallbackPNTrianglesfATI),
	glPNTrianglesiATI(fallbackPNTrianglesiATI),
	glPassTexCoordATI(fallbackPassTexCoordATI),
	glPassThrough(fallbackPassThrough),
	glPatchParameterfv(fallbackPatchParameterfv),
	glPatchParameteri(fallbackPatchParameteri),
	glPauseTransformFeedback(fallbackPauseTransformFeedback),
	glPauseTransformFeedbackNV(fallbackPauseTransformFeedbackNV),
	glPixelDataRangeNV(fallbackPixelDataRangeNV),
	glPixelMapfv(fallbackPixelMapfv),
	glPixelMapuiv(fallbackPixelMapuiv),
	glPixelMapusv(fallbackPixelMapusv),
	glPixelStoref(fallbackPixelStoref),
	glPixelStorei(fallbackPixelStorei),
	glPixelTexGenParameterfSGIS(fallbackPixelTexGenParameterfSGIS),
	glPixelTexGenParameterfvSGIS(fallbackPixelTexGenParameterfvSGIS),
	glPixelTexGenParameteriSGIS(fallbackPixelTexGenParameteriSGIS),
	glPixelTexGenParameterivSGIS(fallbackPixelTexGenParameterivSGIS),
	glPixelTexGenSGIX(fallbackPixelTexGenSGIX),
	glPixelTransferf(fallbackPixelTransferf),
	glPixelTransferi(fallbackPixelTransferi),
	glPixelTransformParameterfEXT(fallbackPixelTransformParameterfEXT),
	glPixelTransformParameterfvEXT(fallbackPixelTransformParameterfvEXT),
	glPixelTransformParameteriEXT(fallbackPixelTransformParameteriEXT),
	glPixelTransformParameterivEXT(fallbackPixelTransformParameterivEXT),
	glPixelZoom(fallbackPixelZoom),
	glPointParameterf(fallbackPointParameterf),
	glPointParameterfARB(fallbackPointParameterfARB),
	glPointParameterfEXT(fallbackPointParameterfEXT),
	glPointParameterfSGIS(fallbackPointParameterfSGIS),
	glPointParameterfv(fallbackPointParameterfv),
	glPointParameterfvARB(fallbackPointParameterfvARB),
	glPointParameterfvEXT(fallbackPointParameterfvEXT),
	glPointParameterfvSGIS(fallbackPointParameterfvSGIS),
	glPointParameteri(fallbackPointParameteri),
	glPointParameteriNV(fallbackPointParameteriNV),
	glPointParameteriv(fallbackPointParameteriv),
	glPointParameterivNV(fallbackPointParameterivNV),
	glPointSize(fallbackPointSize),
	glPollAsyncSGIX(fallbackPollAsyncSGIX),
	glPollInstrumentsSGIX(fallbackPollInstrumentsSGIX),
	glPolygonMode(fallbackPolygonMode),
	glPolygonOffset(fallbackPolygonOffset),
	glPolygonOffsetEXT(fallbackPolygonOffsetEXT),
	glPolygonStipple(fallbackPolygonStipple),
	glPopAttrib(fallbackPopAttrib),
	glPopClientAttrib(fallbackPopClientAttrib),
	glPopMatrix(fallbackPopMatrix),
	glPopName(fallbackPopName),
	glPresentFrameDualFillNV(fallbackPresentFrameDualFillNV),
	glPresentFrameKeyedNV(fallbackPresentFrameKeyedNV),
	glPrimitiveRestartIndex(fallbackPrimitiveRestartIndex),
	glPrimitiveRestartIndexNV(fallbackPrimitiveRestartIndexNV),
	glPrimitiveRestartNV(fallbackPrimitiveRestartNV),
	glPrioritizeTextures(fallbackPrioritizeTextures),
	glPrioritizeTexturesEXT(fallbackPrioritizeTexturesEXT),
	glProgramBufferParametersIivNV(fallbackProgramBufferParametersIivNV),
	glProgramBufferParametersIuivNV(fallbackProgramBufferParametersIuivNV),
	glProgramBufferParametersfvNV(fallbackProgramBufferParametersfvNV),
	glProgramEnvParameter4dARB(fallbackProgramEnvParameter4dARB),
	glProgramEnvParameter4dvARB(fallbackProgramEnvParameter4dvARB),
	glProgramEnvParameter4fARB(fallbackProgramEnvParameter4fARB),
	glProgramEnvParameter4fvARB(fallbackProgramEnvParameter4fvARB),
	glProgramEnvParameterI4iNV(fallbackProgramEnvParameterI4iNV),
	glProgramEnvParameterI4ivNV(fallbackProgramEnvParameterI4ivNV),
	glProgramEnvParameterI4uiNV(fallbackProgramEnvParameterI4uiNV),
	glProgramEnvParameterI4uivNV(fallbackProgramEnvParameterI4uivNV),
	glProgramEnvParameters4fvEXT(fallbackProgramEnvParameters4fvEXT),
	glProgramEnvParametersI4ivNV(fallbackProgramEnvParametersI4ivNV),
	glProgramEnvParametersI4uivNV(fallbackProgramEnvParametersI4uivNV),
	glProgramLocalParameter4dARB(fallbackProgramLocalParameter4dARB),
	glProgramLocalParameter4dvARB(fallbackProgramLocalParameter4dvARB),
	glProgramLocalParameter4fARB(fallbackProgramLocalParameter4fARB),
	glProgramLocalParameter4fvARB(fallbackProgramLocalParameter4fvARB),
	glProgramLocalParameterI4iNV(fallbackProgramLocalParameterI4iNV),
	glProgramLocalParameterI4ivNV(fallbackProgramLocalParameterI4ivNV),
	glProgramLocalParameterI4uiNV(fallbackProgramLocalParameterI4uiNV),
	glProgramLocalParameterI4uivNV(fallbackProgramLocalParameterI4uivNV),
	glProgramLocalParameters4fvEXT(fallbackProgramLocalParameters4fvEXT),
	glProgramLocalParametersI4ivNV(fallbackProgramLocalParametersI4ivNV),
	glProgramLocalParametersI4uivNV(fallbackProgramLocalParametersI4uivNV),
	glProgramNamedParameter4dNV(fallbackProgramNamedParameter4dNV),
	glProgramNamedParameter4dvNV(fallbackProgramNamedParameter4dvNV),
	glProgramNamedParameter4fNV(fallbackProgramNamedParameter4fNV),
	glProgramNamedParameter4fvNV(fallbackProgramNamedParameter4fvNV),
	glProgramParameter4dNV(fallbackProgramParameter4dNV),
	glProgramParameter4dvNV(fallbackProgramParameter4dvNV),
	glProgramParameter4fNV(fallbackProgramParameter4fNV),
	glProgramParameter4fvNV(fallbackProgramParameter4fvNV),
	glProgramParameteri(fallbackProgramParameteri),
	glProgramParameteriARB(fallbackProgramParameteriARB),
	glProgramParameteriEXT(fallbackProgramParameteriEXT),
	glProgramParameters4dvNV(fallbackProgramParameters4dvNV),
	glProgramParameters4fvNV(fallbackProgramParameters4fvNV),
	glProgramStringARB(fallbackProgramStringARB),
	glProgramUniform1dEXT(fallbackProgramUniform1dEXT),
	glProgramUniform1dvEXT(fallbackProgramUniform1dvEXT),
	glProgramUniform1fEXT(fallbackProgramUniform1fEXT),
	glProgramUniform1fvEXT(fallbackProgramUniform1fvEXT),
	glProgramUniform1iEXT(fallbackProgramUniform1iEXT),
	glProgramUniform1ivEXT(fallbackProgramUniform1ivEXT),
	glProgramUniform1uiEXT(fallbackProgramUniform1uiEXT),
	glProgramUniform1uivEXT(fallbackProgramUniform1uivEXT),
	glProgramUniform2dEXT(fallbackProgramUniform2dEXT),
	glProgramUniform2dvEXT(fallbackProgramUniform2dvEXT),
	glProgramUniform2fEXT(fallbackProgramUniform2fEXT),
	glProgramUniform2fvEXT(fallbackProgramUniform2fvEXT),
	glProgramUniform2iEXT(fallbackProgramUniform2iEXT),
	glProgramUniform2ivEXT(fallbackProgramUniform2ivEXT),
	glProgramUniform2uiEXT(fallbackProgramUniform2uiEXT),
	glProgramUniform2uivEXT(fallbackProgramUniform2uivEXT),
	glProgramUniform3dEXT(fallbackProgramUniform3dEXT),
	glProgramUniform3dvEXT(fallbackProgramUniform3dvEXT),
	glProgramUniform3fEXT(fallbackProgramUniform3fEXT),
	glProgramUniform3fvEXT(fallbackProgramUniform3fvEXT),
	glProgramUniform3iEXT(fallbackProgramUniform3iEXT),
	glProgramUniform3ivEXT(fallbackProgramUniform3ivEXT),
	glProgramUniform3uiEXT(fallbackProgramUniform3uiEXT),
	glProgramUniform3uivEXT(fallbackProgramUniform3uivEXT),
	glProgramUniform4dEXT(fallbackProgramUniform4dEXT),
	glProgramUniform4dvEXT(fallbackProgramUniform4dvEXT),
	glProgramUniform4fEXT(fallbackProgramUniform4fEXT),
	glProgramUniform4fvEXT(fallbackProgramUniform4fvEXT),
	glProgramUniform4iEXT(fallbackProgramUniform4iEXT),
	glProgramUniform4ivEXT(fallbackProgramUniform4ivEXT),
	glProgramUniform4uiEXT(fallbackProgramUniform4uiEXT),
	glProgramUniform4uivEXT(fallbackProgramUniform4uivEXT),
	glProgramUniformMatrix2dvEXT(fallbackProgramUniformMatrix2dvEXT),
	glProgramUniformMatrix2fvEXT(fallbackProgramUniformMatrix2fvEXT),
	glProgramUniformMatrix2x3dvEXT(fallbackProgramUniformMatrix2x3dvEXT),
	glProgramUniformMatrix2x3fvEXT(fallbackProgramUniformMatrix2x3fvEXT),
	glProgramUniformMatrix2x4dvEXT(fallbackProgramUniformMatrix2x4dvEXT),
	glProgramUniformMatrix2x4fvEXT(fallbackProgramUniformMatrix2x4fvEXT),
	glProgramUniformMatrix3dvEXT(fallbackProgramUniformMatrix3dvEXT),
	glProgramUniformMatrix3fvEXT(fallbackProgramUniformMatrix3fvEXT),
	glProgramUniformMatrix3x2dvEXT(fallbackProgramUniformMatrix3x2dvEXT),
	glProgramUniformMatrix3x2fvEXT(fallbackProgramUniformMatrix3x2fvEXT),
	glProgramUniformMatrix3x4dvEXT(fallbackProgramUniformMatrix3x4dvEXT),
	glProgramUniformMatrix3x4fvEXT(fallbackProgramUniformMatrix3x4fvEXT),
	glProgramUniformMatrix4dvEXT(fallbackProgramUniformMatrix4dvEXT),
	glProgramUniformMatrix4fvEXT(fallbackProgramUniformMatrix4fvEXT),
	glProgramUniformMatrix4x2dvEXT(fallbackProgramUniformMatrix4x2dvEXT),
	glProgramUniformMatrix4x2fvEXT(fallbackProgramUniformMatrix4x2fvEXT),
	glProgramUniformMatrix4x3dvEXT(fallbackProgramUniformMatrix4x3dvEXT),
	glProgramUniformMatrix4x3fvEXT(fallbackProgramUniformMatrix4x3fvEXT),
	glProgramUniformui64NV(fallbackProgramUniformui64NV),
	glProgramUniformui64vNV(fallbackProgramUniformui64vNV),
	glProgramVertexLimitNV(fallbackProgramVertexLimitNV),
	glProvokingVertex(fallbackProvokingVertex),
	glProvokingVertexEXT(fallbackProvokingVertexEXT),
	glPushAttrib(fallbackPushAttrib),
	glPushClientAttrib(fallbackPushClientAttrib),
	glPushClientAttribDefaultEXT(fallbackPushClientAttribDefaultEXT),
	glPushMatrix(fallbackPushMatrix),
	glPushName(fallbackPushName),
	glQueryCounter(fallbackQueryCounter),
	glRasterPos2d(fallbackRasterPos2d),
	glRasterPos2dv(fallbackRasterPos2dv),
	glRasterPos2f(fallbackRasterPos2f),
	glRasterPos2fv(fallbackRasterPos2fv),
	glRasterPos2i(fallbackRasterPos2i),
	glRasterPos2iv(fallbackRasterPos2iv),
	glRasterPos2s(fallbackRasterPos2s),
	glRasterPos2sv(fallbackRasterPos2sv),
	glRasterPos3d(fallbackRasterPos3d),
	glRasterPos3dv(fallbackRasterPos3dv),
	glRasterPos3f(fallbackRasterPos3f),
	glRasterPos3fv(fallbackRasterPos3fv),
	glRasterPos3i(fallbackRasterPos3i),
	glRasterPos3iv(fallbackRasterPos3iv),
	glRasterPos3s(fallbackRasterPos3s),
	glRasterPos3sv(fallbackRasterPos3sv),
	glRasterPos4d(fallbackRasterPos4d),
	glRasterPos4dv(fallbackRasterPos4dv),
	glRasterPos4f(fallbackRasterPos4f),
	glRasterPos4fv(fallbackRasterPos4fv),
	glRasterPos4i(fallbackRasterPos4i),
	glRasterPos4iv(fallbackRasterPos4iv),
	glRasterPos4s(fallbackRasterPos4s),
	glRasterPos4sv(fallbackRasterPos4sv),
	glReadBuffer(fallbackReadBuffer),
	glReadInstrumentsSGIX(fallbackReadInstrumentsSGIX),
	glReadPixels(fallbackReadPixels),
	glRectd(fallbackRectd),
	glRectdv(fallbackRectdv),
	glRectf(fallbackRectf),
	glRectfv(fallbackRectfv),
	glRecti(fallbackRecti),
	glRectiv(fallbackRectiv),
	glRects(fallbackRects),
	glRectsv(fallbackRectsv),
	glReferencePlaneSGIX(fallbackReferencePlaneSGIX),
	glRenderMode(fallbackRenderMode),
	glRenderbufferStorage(fallbackRenderbufferStorage),
	glRenderbufferStorageEXT(fallbackRenderbufferStorageEXT),
	glRenderbufferStorageMultisample(fallbackRenderbufferStorageMultisample),
	glRenderbufferStorageMultisampleCoverageNV(fallbackRenderbufferStorageMultisampleCoverageNV),
	glRenderbufferStorageMultisampleEXT(fallbackRenderbufferStorageMultisampleEXT),
	glReplacementCodePointerSUN(fallbackReplacementCodePointerSUN),
	glReplacementCodeubSUN(fallbackReplacementCodeubSUN),
	glReplacementCodeubvSUN(fallbackReplacementCodeubvSUN),
	glReplacementCodeuiColor3fVertex3fSUN(fallbackReplacementCodeuiColor3fVertex3fSUN),
	glReplacementCodeuiColor3fVertex3fvSUN(fallbackReplacementCodeuiColor3fVertex3fvSUN),
	glReplacementCodeuiColor4fNormal3fVertex3fSUN(fallbackReplacementCodeuiColor4fNormal3fVertex3fSUN),
	glReplacementCodeuiColor4fNormal3fVertex3fvSUN(fallbackReplacementCodeuiColor4fNormal3fVertex3fvSUN),
	glReplacementCodeuiColor4ubVertex3fSUN(fallbackReplacementCodeuiColor4ubVertex3fSUN),
	glReplacementCodeuiColor4ubVertex3fvSUN(fallbackReplacementCodeuiColor4ubVertex3fvSUN),
	glReplacementCodeuiNormal3fVertex3fSUN(fallbackReplacementCodeuiNormal3fVertex3fSUN),
	glReplacementCodeuiNormal3fVertex3fvSUN(fallbackReplacementCodeuiNormal3fVertex3fvSUN),
	glReplacementCodeuiSUN(fallbackReplacementCodeuiSUN),
	glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN(fallbackReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN),
	glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN(fallbackReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN),
	glReplacementCodeuiTexCoord2fNormal3fVertex3fSUN(fallbackReplacementCodeuiTexCoord2fNormal3fVertex3fSUN),
	glReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN(fallbackReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN),
	glReplacementCodeuiTexCoord2fVertex3fSUN(fallbackReplacementCodeuiTexCoord2fVertex3fSUN),
	glReplacementCodeuiTexCoord2fVertex3fvSUN(fallbackReplacementCodeuiTexCoord2fVertex3fvSUN),
	glReplacementCodeuiVertex3fSUN(fallbackReplacementCodeuiVertex3fSUN),
	glReplacementCodeuiVertex3fvSUN(fallbackReplacementCodeuiVertex3fvSUN),
	glReplacementCodeuivSUN(fallbackReplacementCodeuivSUN),
	glReplacementCodeusSUN(fallbackReplacementCodeusSUN),
	glReplacementCodeusvSUN(fallbackReplacementCodeusvSUN),
	glRequestResidentProgramsNV(fallbackRequestResidentProgramsNV),
	glResetHistogram(fallbackResetHistogram),
	glResetHistogramEXT(fallbackResetHistogramEXT),
	glResetMinmax(fallbackResetMinmax),
	glResetMinmaxEXT(fallbackResetMinmaxEXT),
	glResizeBuffersMESA(fallbackResizeBuffersMESA),
	glResumeTransformFeedback(fallbackResumeTransformFeedback),
	glResumeTransformFeedbackNV(fallbackResumeTransformFeedbackNV),
	glRotated(fallbackRotated),
	glRotatef(fallbackRotatef),
	glSampleCoverage(fallbackSampleCoverage),
	glSampleCoverageARB(fallbackSampleCoverageARB),
	glSampleMapATI(fallbackSampleMapATI),
	glSampleMaskEXT(fallbackSampleMaskEXT),
	glSampleMaskIndexedNV(fallbackSampleMaskIndexedNV),
	glSampleMaskSGIS(fallbackSampleMaskSGIS),
	glSampleMaski(fallbackSampleMaski),
	glSamplePatternEXT(fallbackSamplePatternEXT),
	glSamplePatternSGIS(fallbackSamplePatternSGIS),
	glSamplerParameterIiv(fallbackSamplerParameterIiv),
	glSamplerParameterIuiv(fallbackSamplerParameterIuiv),
	glSamplerParameterf(fallbackSamplerParameterf),
	glSamplerParameterfv(fallbackSamplerParameterfv),
	glSamplerParameteri(fallbackSamplerParameteri),
	glSamplerParameteriv(fallbackSamplerParameteriv),
	glScaled(fallbackScaled),
	glScalef(fallbackScalef),
	glScissor(fallbackScissor),
	glSecondaryColor3b(fallbackSecondaryColor3b),
	glSecondaryColor3bEXT(fallbackSecondaryColor3bEXT),
	glSecondaryColor3bv(fallbackSecondaryColor3bv),
	glSecondaryColor3bvEXT(fallbackSecondaryColor3bvEXT),
	glSecondaryColor3d(fallbackSecondaryColor3d),
	glSecondaryColor3dEXT(fallbackSecondaryColor3dEXT),
	glSecondaryColor3dv(fallbackSecondaryColor3dv),
	glSecondaryColor3dvEXT(fallbackSecondaryColor3dvEXT),
	glSecondaryColor3f(fallbackSecondaryColor3f),
	glSecondaryColor3fEXT(fallbackSecondaryColor3fEXT),
	glSecondaryColor3fv(fallbackSecondaryColor3fv),
	glSecondaryColor3fvEXT(fallbackSecondaryColor3fvEXT),
	glSecondaryColor3hNV(fallbackSecondaryColor3hNV),
	glSecondaryColor3hvNV(fallbackSecondaryColor3hvNV),
	glSecondaryColor3i(fallbackSecondaryColor3i),
	glSecondaryColor3iEXT(fallbackSecondaryColor3iEXT),
	glSecondaryColor3iv(fallbackSecondaryColor3iv),
	glSecondaryColor3ivEXT(fallbackSecondaryColor3ivEXT),
	glSecondaryColor3s(fallbackSecondaryColor3s),
	glSecondaryColor3sEXT(fallbackSecondaryColor3sEXT),
	glSecondaryColor3sv(fallbackSecondaryColor3sv),
	glSecondaryColor3svEXT(fallbackSecondaryColor3svEXT),
	glSecondaryColor3ub(fallbackSecondaryColor3ub),
	glSecondaryColor3ubEXT(fallbackSecondaryColor3ubEXT),
	glSecondaryColor3ubv(fallbackSecondaryColor3ubv),
	glSecondaryColor3ubvEXT(fallbackSecondaryColor3ubvEXT),
	glSecondaryColor3ui(fallbackSecondaryColor3ui),
	glSecondaryColor3uiEXT(fallbackSecondaryColor3uiEXT),
	glSecondaryColor3uiv(fallbackSecondaryColor3uiv),
	glSecondaryColor3uivEXT(fallbackSecondaryColor3uivEXT),
	glSecondaryColor3us(fallbackSecondaryColor3us),
	glSecondaryColor3usEXT(fallbackSecondaryColor3usEXT),
	glSecondaryColor3usv(fallbackSecondaryColor3usv),
	glSecondaryColor3usvEXT(fallbackSecondaryColor3usvEXT),
	glSecondaryColorFormatNV(fallbackSecondaryColorFormatNV),
	glSecondaryColorP3ui(fallbackSecondaryColorP3ui),
	glSecondaryColorP3uiv(fallbackSecondaryColorP3uiv),
	glSecondaryColorPointer(fallbackSecondaryColorPointer),
	glSecondaryColorPointerEXT(fallbackSecondaryColorPointerEXT),
	glSecondaryColorPointerListIBM(fallbackSecondaryColorPointerListIBM),
	glSelectBuffer(fallbackSelectBuffer),
	glSelectPerfMonitorCountersAMD(fallbackSelectPerfMonitorCountersAMD),
	glSeparableFilter2D(fallbackSeparableFilter2D),
	glSeparableFilter2DEXT(fallbackSeparableFilter2DEXT),
	glSetFenceAPPLE(fallbackSetFenceAPPLE),
	glSetFenceNV(fallbackSetFenceNV),
	glSetFragmentShaderConstantATI(fallbackSetFragmentShaderConstantATI),
	glSetInvariantEXT(fallbackSetInvariantEXT),
	glSetLocalConstantEXT(fallbackSetLocalConstantEXT),
	glShadeModel(fallbackShadeModel),
	glShaderOp1EXT(fallbackShaderOp1EXT),
	glShaderOp2EXT(fallbackShaderOp2EXT),
	glShaderOp3EXT(fallbackShaderOp3EXT),
	glShaderSource(fallbackShaderSource),
	glShaderSourceARB(fallbackShaderSourceARB),
	glSharpenTexFuncSGIS(fallbackSharpenTexFuncSGIS),
	glSpriteParameterfSGIX(fallbackSpriteParameterfSGIX),
	glSpriteParameterfvSGIX(fallbackSpriteParameterfvSGIX),
	glSpriteParameteriSGIX(fallbackSpriteParameteriSGIX),
	glSpriteParameterivSGIX(fallbackSpriteParameterivSGIX),
	glStartInstrumentsSGIX(fallbackStartInstrumentsSGIX),
	glStencilClearTagEXT(fallbackStencilClearTagEXT),
	glStencilFunc(fallbackStencilFunc),
	glStencilFuncSeparate(fallbackStencilFuncSeparate),
	glStencilFuncSeparateATI(fallbackStencilFuncSeparateATI),
	glStencilMask(fallbackStencilMask),
	glStencilMaskSeparate(fallbackStencilMaskSeparate),
	glStencilOp(fallbackStencilOp),
	glStencilOpSeparate(fallbackStencilOpSeparate),
	glStencilOpSeparateATI(fallbackStencilOpSeparateATI),
	glStopInstrumentsSGIX(fallbackStopInstrumentsSGIX),
	glStringMarkerGREMEDY(fallbackStringMarkerGREMEDY),
	glSwizzleEXT(fallbackSwizzleEXT),
	glTagSampleBufferSGIX(fallbackTagSampleBufferSGIX),
	glTangent3bEXT(fallbackTangent3bEXT),
	glTangent3bvEXT(fallbackTangent3bvEXT),
	glTangent3dEXT(fallbackTangent3dEXT),
	glTangent3dvEXT(fallbackTangent3dvEXT),
	glTangent3fEXT(fallbackTangent3fEXT),
	glTangent3fvEXT(fallbackTangent3fvEXT),
	glTangent3iEXT(fallbackTangent3iEXT),
	glTangent3ivEXT(fallbackTangent3ivEXT),
	glTangent3sEXT(fallbackTangent3sEXT),
	glTangent3svEXT(fallbackTangent3svEXT),
	glTangentPointerEXT(fallbackTangentPointerEXT),
	glTbufferMask3DFX(fallbackTbufferMask3DFX),
	glTessellationFactorAMD(fallbackTessellationFactorAMD),
	glTessellationModeAMD(fallbackTessellationModeAMD),
	glTestFenceAPPLE(fallbackTestFenceAPPLE),
	glTestFenceNV(fallbackTestFenceNV),
	glTestObjectAPPLE(fallbackTestObjectAPPLE),
	glTexBuffer(fallbackTexBuffer),
	glTexBufferARB(fallbackTexBufferARB),
	glTexBufferEXT(fallbackTexBufferEXT),
	glTexBumpParameterfvATI(fallbackTexBumpParameterfvATI),
	glTexBumpParameterivATI(fallbackTexBumpParameterivATI),
	glTexCoord1d(fallbackTexCoord1d),
	glTexCoord1dv(fallbackTexCoord1dv),
	glTexCoord1f(fallbackTexCoord1f),
	glTexCoord1fv(fallbackTexCoord1fv),
	glTexCoord1hNV(fallbackTexCoord1hNV),
	glTexCoord1hvNV(fallbackTexCoord1hvNV),
	glTexCoord1i(fallbackTexCoord1i),
	glTexCoord1iv(fallbackTexCoord1iv),
	glTexCoord1s(fallbackTexCoord1s),
	glTexCoord1sv(fallbackTexCoord1sv),
	glTexCoord2d(fallbackTexCoord2d),
	glTexCoord2dv(fallbackTexCoord2dv),
	glTexCoord2f(fallbackTexCoord2f),
	glTexCoord2fColor3fVertex3fSUN(fallbackTexCoord2fColor3fVertex3fSUN),
	glTexCoord2fColor3fVertex3fvSUN(fallbackTexCoord2fColor3fVertex3fvSUN),
	glTexCoord2fColor4fNormal3fVertex3fSUN(fallbackTexCoord2fColor4fNormal3fVertex3fSUN),
	glTexCoord2fColor4fNormal3fVertex3fvSUN(fallbackTexCoord2fColor4fNormal3fVertex3fvSUN),
	glTexCoord2fColor4ubVertex3fSUN(fallbackTexCoord2fColor4ubVertex3fSUN),
	glTexCoord2fColor4ubVertex3fvSUN(fallbackTexCoord2fColor4ubVertex3fvSUN),
	glTexCoord2fNormal3fVertex3fSUN(fallbackTexCoord2fNormal3fVertex3fSUN),
	glTexCoord2fNormal3fVertex3fvSUN(fallbackTexCoord2fNormal3fVertex3fvSUN),
	glTexCoord2fVertex3fSUN(fallbackTexCoord2fVertex3fSUN),
	glTexCoord2fVertex3fvSUN(fallbackTexCoord2fVertex3fvSUN),
	glTexCoord2fv(fallbackTexCoord2fv),
	glTexCoord2hNV(fallbackTexCoord2hNV),
	glTexCoord2hvNV(fallbackTexCoord2hvNV),
	glTexCoord2i(fallbackTexCoord2i),
	glTexCoord2iv(fallbackTexCoord2iv),
	glTexCoord2s(fallbackTexCoord2s),
	glTexCoord2sv(fallbackTexCoord2sv),
	glTexCoord3d(fallbackTexCoord3d),
	glTexCoord3dv(fallbackTexCoord3dv),
	glTexCoord3f(fallbackTexCoord3f),
	glTexCoord3fv(fallbackTexCoord3fv),
	glTexCoord3hNV(fallbackTexCoord3hNV),
	glTexCoord3hvNV(fallbackTexCoord3hvNV),
	glTexCoord3i(fallbackTexCoord3i),
	glTexCoord3iv(fallbackTexCoord3iv),
	glTexCoord3s(fallbackTexCoord3s),
	glTexCoord3sv(fallbackTexCoord3sv),
	glTexCoord4d(fallbackTexCoord4d),
	glTexCoord4dv(fallbackTexCoord4dv),
	glTexCoord4f(fallbackTexCoord4f),
	glTexCoord4fColor4fNormal3fVertex4fSUN(fallbackTexCoord4fColor4fNormal3fVertex4fSUN),
	glTexCoord4fColor4fNormal3fVertex4fvSUN(fallbackTexCoord4fColor4fNormal3fVertex4fvSUN),
	glTexCoord4fVertex4fSUN(fallbackTexCoord4fVertex4fSUN),
	glTexCoord4fVertex4fvSUN(fallbackTexCoord4fVertex4fvSUN),
	glTexCoord4fv(fallbackTexCoord4fv),
	glTexCoord4hNV(fallbackTexCoord4hNV),
	glTexCoord4hvNV(fallbackTexCoord4hvNV),
	glTexCoord4i(fallbackTexCoord4i),
	glTexCoord4iv(fallbackTexCoord4iv),
	glTexCoord4s(fallbackTexCoord4s),
	glTexCoord4sv(fallbackTexCoord4sv),
	glTexCoordFormatNV(fallbackTexCoordFormatNV),
	glTexCoordP1ui(fallbackTexCoordP1ui),
	glTexCoordP1uiv(fallbackTexCoordP1uiv),
	glTexCoordP2ui(fallbackTexCoordP2ui),
	glTexCoordP2uiv(fallbackTexCoordP2uiv),
	glTexCoordP3ui(fallbackTexCoordP3ui),
	glTexCoordP3uiv(fallbackTexCoordP3uiv),
	glTexCoordP4ui(fallbackTexCoordP4ui),
	glTexCoordP4uiv(fallbackTexCoordP4uiv),
	glTexCoordPointer(fallbackTexCoordPointer),
	glTexCoordPointerEXT(fallbackTexCoordPointerEXT),
	glTexCoordPointerListIBM(fallbackTexCoordPointerListIBM),
	glTexCoordPointervINTEL(fallbackTexCoordPointervINTEL),
	glTexEnvf(fallbackTexEnvf),
	glTexEnvfv(fallbackTexEnvfv),
	glTexEnvi(fallbackTexEnvi),
	glTexEnviv(fallbackTexEnviv),
	glTexFilterFuncSGIS(fallbackTexFilterFuncSGIS),
	glTexGend(fallbackTexGend),
	glTexGendv(fallbackTexGendv),
	glTexGenf(fallbackTexGenf),
	glTexGenfv(fallbackTexGenfv),
	glTexGeni(fallbackTexGeni),
	glTexGeniv(fallbackTexGeniv),
	glTexImage1D(fallbackTexImage1D),
	glTexImage2D(fallbackTexImage2D),
	glTexImage2DMultisample(fallbackTexImage2DMultisample),
	glTexImage3D(fallbackTexImage3D),
	glTexImage3DEXT(fallbackTexImage3DEXT),
	glTexImage3DMultisample(fallbackTexImage3DMultisample),
	glTexImage4DSGIS(fallbackTexImage4DSGIS),
	glTexParameterIiv(fallbackTexParameterIiv),
	glTexParameterIivEXT(fallbackTexParameterIivEXT),
	glTexParameterIuiv(fallbackTexParameterIuiv),
	glTexParameterIuivEXT(fallbackTexParameterIuivEXT),
	glTexParameterf(fallbackTexParameterf),
	glTexParameterfv(fallbackTexParameterfv),
	glTexParameteri(fallbackTexParameteri),
	glTexParameteriv(fallbackTexParameteriv),
	glTexRenderbufferNV(fallbackTexRenderbufferNV),
	glTexSubImage1D(fallbackTexSubImage1D),
	glTexSubImage1DEXT(fallbackTexSubImage1DEXT),
	glTexSubImage2D(fallbackTexSubImage2D),
	glTexSubImage2DEXT(fallbackTexSubImage2DEXT),
	glTexSubImage3D(fallbackTexSubImage3D),
	glTexSubImage3DEXT(fallbackTexSubImage3DEXT),
	glTexSubImage4DSGIS(fallbackTexSubImage4DSGIS),
	glTextureBarrierNV(fallbackTextureBarrierNV),
	glTextureBufferEXT(fallbackTextureBufferEXT),
	glTextureColorMaskSGIS(fallbackTextureColorMaskSGIS),
	glTextureImage1DEXT(fallbackTextureImage1DEXT),
	glTextureImage2DEXT(fallbackTextureImage2DEXT),
	glTextureImage3DEXT(fallbackTextureImage3DEXT),
	glTextureLightEXT(fallbackTextureLightEXT),
	glTextureMaterialEXT(fallbackTextureMaterialEXT),
	glTextureNormalEXT(fallbackTextureNormalEXT),
	glTextureParameterIivEXT(fallbackTextureParameterIivEXT),
	glTextureParameterIuivEXT(fallbackTextureParameterIuivEXT),
	glTextureParameterfEXT(fallbackTextureParameterfEXT),
	glTextureParameterfvEXT(fallbackTextureParameterfvEXT),
	glTextureParameteriEXT(fallbackTextureParameteriEXT),
	glTextureParameterivEXT(fallbackTextureParameterivEXT),
	glTextureRangeAPPLE(fallbackTextureRangeAPPLE),
	glTextureRenderbufferEXT(fallbackTextureRenderbufferEXT),
	glTextureSubImage1DEXT(fallbackTextureSubImage1DEXT),
	glTextureSubImage2DEXT(fallbackTextureSubImage2DEXT),
	glTextureSubImage3DEXT(fallbackTextureSubImage3DEXT),
	glTrackMatrixNV(fallbackTrackMatrixNV),
	glTransformFeedbackAttribsNV(fallbackTransformFeedbackAttribsNV),
	glTransformFeedbackVaryings(fallbackTransformFeedbackVaryings),
	glTransformFeedbackVaryingsEXT(fallbackTransformFeedbackVaryingsEXT),
	glTransformFeedbackVaryingsNV(fallbackTransformFeedbackVaryingsNV),
	glTranslated(fallbackTranslated),
	glTranslatef(fallbackTranslatef),
	glUniform1d(fallbackUniform1d),
	glUniform1dv(fallbackUniform1dv),
	glUniform1f(fallbackUniform1f),
	glUniform1fARB(fallbackUniform1fARB),
	glUniform1fv(fallbackUniform1fv),
	glUniform1fvARB(fallbackUniform1fvARB),
	glUniform1i(fallbackUniform1i),
	glUniform1iARB(fallbackUniform1iARB),
	glUniform1iv(fallbackUniform1iv),
	glUniform1ivARB(fallbackUniform1ivARB),
	glUniform1ui(fallbackUniform1ui),
	glUniform1uiEXT(fallbackUniform1uiEXT),
	glUniform1uiv(fallbackUniform1uiv),
	glUniform1uivEXT(fallbackUniform1uivEXT),
	glUniform2d(fallbackUniform2d),
	glUniform2dv(fallbackUniform2dv),
	glUniform2f(fallbackUniform2f),
	glUniform2fARB(fallbackUniform2fARB),
	glUniform2fv(fallbackUniform2fv),
	glUniform2fvARB(fallbackUniform2fvARB),
	glUniform2i(fallbackUniform2i),
	glUniform2iARB(fallbackUniform2iARB),
	glUniform2iv(fallbackUniform2iv),
	glUniform2ivARB(fallbackUniform2ivARB),
	glUniform2ui(fallbackUniform2ui),
	glUniform2uiEXT(fallbackUniform2uiEXT),
	glUniform2uiv(fallbackUniform2uiv),
	glUniform2uivEXT(fallbackUniform2uivEXT),
	glUniform3d(fallbackUniform3d),
	glUniform3dv(fallbackUniform3dv),
	glUniform3f(fallbackUniform3f),
	glUniform3fARB(fallbackUniform3fARB),
	glUniform3fv(fallbackUniform3fv),
	glUniform3fvARB(fallbackUniform3fvARB),
	glUniform3i(fallbackUniform3i),
	glUniform3iARB(fallbackUniform3iARB),
	glUniform3iv(fallbackUniform3iv),
	glUniform3ivARB(fallbackUniform3ivARB),
	glUniform3ui(fallbackUniform3ui),
	glUniform3uiEXT(fallbackUniform3uiEXT),
	glUniform3uiv(fallbackUniform3uiv),
	glUniform3uivEXT(fallbackUniform3uivEXT),
	glUniform4d(fallbackUniform4d),
	glUniform4dv(fallbackUniform4dv),
	glUniform4f(fallbackUniform4f),
	glUniform4fARB(fallbackUniform4fARB),
	glUniform4fv(fallbackUniform4fv),
	glUniform4fvARB(fallbackUniform4fvARB),
	glUniform4i(fallbackUniform4i),
	glUniform4iARB(fallbackUniform4iARB),
	glUniform4iv(fallbackUniform4iv),
	glUniform4ivARB(fallbackUniform4ivARB),
	glUniform4ui(fallbackUniform4ui),
	glUniform4uiEXT(fallbackUniform4uiEXT),
	glUniform4uiv(fallbackUniform4uiv),
	glUniform4uivEXT(fallbackUniform4uivEXT),
	glUniformBlockBinding(fallbackUniformBlockBinding),
	glUniformBufferEXT(fallbackUniformBufferEXT),
	glUniformMatrix2dv(fallbackUniformMatrix2dv),
	glUniformMatrix2fv(fallbackUniformMatrix2fv),
	glUniformMatrix2fvARB(fallbackUniformMatrix2fvARB),
	glUniformMatrix2x3dv(fallbackUniformMatrix2x3dv),
	glUniformMatrix2x3fv(fallbackUniformMatrix2x3fv),
	glUniformMatrix2x4dv(fallbackUniformMatrix2x4dv),
	glUniformMatrix2x4fv(fallbackUniformMatrix2x4fv),
	glUniformMatrix3dv(fallbackUniformMatrix3dv),
	glUniformMatrix3fv(fallbackUniformMatrix3fv),
	glUniformMatrix3fvARB(fallbackUniformMatrix3fvARB),
	glUniformMatrix3x2dv(fallbackUniformMatrix3x2dv),
	glUniformMatrix3x2fv(fallbackUniformMatrix3x2fv),
	glUniformMatrix3x4dv(fallbackUniformMatrix3x4dv),
	glUniformMatrix3x4fv(fallbackUniformMatrix3x4fv),
	glUniformMatrix4dv(fallbackUniformMatrix4dv),
	glUniformMatrix4fv(fallbackUniformMatrix4fv),
	glUniformMatrix4fvARB(fallbackUniformMatrix4fvARB),
	glUniformMatrix4x2dv(fallbackUniformMatrix4x2dv),
	glUniformMatrix4x2fv(fallbackUniformMatrix4x2fv),
	glUniformMatrix4x3dv(fallbackUniformMatrix4x3dv),
	glUniformMatrix4x3fv(fallbackUniformMatrix4x3fv),
	glUniformSubroutinesuiv(fallbackUniformSubroutinesuiv),
	glUniformui64NV(fallbackUniformui64NV),
	glUniformui64vNV(fallbackUniformui64vNV),
	glUnlockArraysEXT(fallbackUnlockArraysEXT),
	glUnmapBuffer(fallbackUnmapBuffer),
	glUnmapBufferARB(fallbackUnmapBufferARB),
	glUnmapNamedBufferEXT(fallbackUnmapNamedBufferEXT),
	glUnmapObjectBufferATI(fallbackUnmapObjectBufferATI),
	glUpdateObjectBufferATI(fallbackUpdateObjectBufferATI),
	glUseProgram(fallbackUseProgram),
	glUseProgramObjectARB(fallbackUseProgramObjectARB),
	glUseShaderProgramEXT(fallbackUseShaderProgramEXT),
	glValidateProgram(fallbackValidateProgram),
	glValidateProgramARB(fallbackValidateProgramARB),
	glVariantArrayObjectATI(fallbackVariantArrayObjectATI),
	glVariantPointerEXT(fallbackVariantPointerEXT),
	glVariantbvEXT(fallbackVariantbvEXT),
	glVariantdvEXT(fallbackVariantdvEXT),
	glVariantfvEXT(fallbackVariantfvEXT),
	glVariantivEXT(fallbackVariantivEXT),
	glVariantsvEXT(fallbackVariantsvEXT),
	glVariantubvEXT(fallbackVariantubvEXT),
	glVariantuivEXT(fallbackVariantuivEXT),
	glVariantusvEXT(fallbackVariantusvEXT),
	glVertex2d(fallbackVertex2d),
	glVertex2dv(fallbackVertex2dv),
	glVertex2f(fallbackVertex2f),
	glVertex2fv(fallbackVertex2fv),
	glVertex2hNV(fallbackVertex2hNV),
	glVertex2hvNV(fallbackVertex2hvNV),
	glVertex2i(fallbackVertex2i),
	glVertex2iv(fallbackVertex2iv),
	glVertex2s(fallbackVertex2s),
	glVertex2sv(fallbackVertex2sv),
	glVertex3d(fallbackVertex3d),
	glVertex3dv(fallbackVertex3dv),
	glVertex3f(fallbackVertex3f),
	glVertex3fv(fallbackVertex3fv),
	glVertex3hNV(fallbackVertex3hNV),
	glVertex3hvNV(fallbackVertex3hvNV),
	glVertex3i(fallbackVertex3i),
	glVertex3iv(fallbackVertex3iv),
	glVertex3s(fallbackVertex3s),
	glVertex3sv(fallbackVertex3sv),
	glVertex4d(fallbackVertex4d),
	glVertex4dv(fallbackVertex4dv),
	glVertex4f(fallbackVertex4f),
	glVertex4fv(fallbackVertex4fv),
	glVertex4hNV(fallbackVertex4hNV),
	glVertex4hvNV(fallbackVertex4hvNV),
	glVertex4i(fallbackVertex4i),
	glVertex4iv(fallbackVertex4iv),
	glVertex4s(fallbackVertex4s),
	glVertex4sv(fallbackVertex4sv),
	glVertexArrayParameteriAPPLE(fallbackVertexArrayParameteriAPPLE),
	glVertexArrayRangeAPPLE(fallbackVertexArrayRangeAPPLE),
	glVertexArrayRangeNV(fallbackVertexArrayRangeNV),
	glVertexAttrib1d(fallbackVertexAttrib1d),
	glVertexAttrib1dARB(fallbackVertexAttrib1dARB),
	glVertexAttrib1dNV(fallbackVertexAttrib1dNV),
	glVertexAttrib1dv(fallbackVertexAttrib1dv),
	glVertexAttrib1dvARB(fallbackVertexAttrib1dvARB),
	glVertexAttrib1dvNV(fallbackVertexAttrib1dvNV),
	glVertexAttrib1f(fallbackVertexAttrib1f),
	glVertexAttrib1fARB(fallbackVertexAttrib1fARB),
	glVertexAttrib1fNV(fallbackVertexAttrib1fNV),
	glVertexAttrib1fv(fallbackVertexAttrib1fv),
	glVertexAttrib1fvARB(fallbackVertexAttrib1fvARB),
	glVertexAttrib1fvNV(fallbackVertexAttrib1fvNV),
	glVertexAttrib1hNV(fallbackVertexAttrib1hNV),
	glVertexAttrib1hvNV(fallbackVertexAttrib1hvNV),
	glVertexAttrib1s(fallbackVertexAttrib1s),
	glVertexAttrib1sARB(fallbackVertexAttrib1sARB),
	glVertexAttrib1sNV(fallbackVertexAttrib1sNV),
	glVertexAttrib1sv(fallbackVertexAttrib1sv),
	glVertexAttrib1svARB(fallbackVertexAttrib1svARB),
	glVertexAttrib1svNV(fallbackVertexAttrib1svNV),
	glVertexAttrib2d(fallbackVertexAttrib2d),
	glVertexAttrib2dARB(fallbackVertexAttrib2dARB),
	glVertexAttrib2dNV(fallbackVertexAttrib2dNV),
	glVertexAttrib2dv(fallbackVertexAttrib2dv),
	glVertexAttrib2dvARB(fallbackVertexAttrib2dvARB),
	glVertexAttrib2dvNV(fallbackVertexAttrib2dvNV),
	glVertexAttrib2f(fallbackVertexAttrib2f),
	glVertexAttrib2fARB(fallbackVertexAttrib2fARB),
	glVertexAttrib2fNV(fallbackVertexAttrib2fNV),
	glVertexAttrib2fv(fallbackVertexAttrib2fv),
	glVertexAttrib2fvARB(fallbackVertexAttrib2fvARB),
	glVertexAttrib2fvNV(fallbackVertexAttrib2fvNV),
	glVertexAttrib2hNV(fallbackVertexAttrib2hNV),
	glVertexAttrib2hvNV(fallbackVertexAttrib2hvNV),
	glVertexAttrib2s(fallbackVertexAttrib2s),
	glVertexAttrib2sARB(fallbackVertexAttrib2sARB),
	glVertexAttrib2sNV(fallbackVertexAttrib2sNV),
	glVertexAttrib2sv(fallbackVertexAttrib2sv),
	glVertexAttrib2svARB(fallbackVertexAttrib2svARB),
	glVertexAttrib2svNV(fallbackVertexAttrib2svNV),
	glVertexAttrib3d(fallbackVertexAttrib3d),
	glVertexAttrib3dARB(fallbackVertexAttrib3dARB),
	glVertexAttrib3dNV(fallbackVertexAttrib3dNV),
	glVertexAttrib3dv(fallbackVertexAttrib3dv),
	glVertexAttrib3dvARB(fallbackVertexAttrib3dvARB),
	glVertexAttrib3dvNV(fallbackVertexAttrib3dvNV),
	glVertexAttrib3f(fallbackVertexAttrib3f),
	glVertexAttrib3fARB(fallbackVertexAttrib3fARB),
	glVertexAttrib3fNV(fallbackVertexAttrib3fNV),
	glVertexAttrib3fv(fallbackVertexAttrib3fv),
	glVertexAttrib3fvARB(fallbackVertexAttrib3fvARB),
	glVertexAttrib3fvNV(fallbackVertexAttrib3fvNV),
	glVertexAttrib3hNV(fallbackVertexAttrib3hNV),
	glVertexAttrib3hvNV(fallbackVertexAttrib3hvNV),
	glVertexAttrib3s(fallbackVertexAttrib3s),
	glVertexAttrib3sARB(fallbackVertexAttrib3sARB),
	glVertexAttrib3sNV(fallbackVertexAttrib3sNV),
	glVertexAttrib3sv(fallbackVertexAttrib3sv),
	glVertexAttrib3svARB(fallbackVertexAttrib3svARB),
	glVertexAttrib3svNV(fallbackVertexAttrib3svNV),
	glVertexAttrib4Nbv(fallbackVertexAttrib4Nbv),
	glVertexAttrib4NbvARB(fallbackVertexAttrib4NbvARB),
	glVertexAttrib4Niv(fallbackVertexAttrib4Niv),
	glVertexAttrib4NivARB(fallbackVertexAttrib4NivARB),
	glVertexAttrib4Nsv(fallbackVertexAttrib4Nsv),
	glVertexAttrib4NsvARB(fallbackVertexAttrib4NsvARB),
	glVertexAttrib4Nub(fallbackVertexAttrib4Nub),
	glVertexAttrib4NubARB(fallbackVertexAttrib4NubARB),
	glVertexAttrib4Nubv(fallbackVertexAttrib4Nubv),
	glVertexAttrib4NubvARB(fallbackVertexAttrib4NubvARB),
	glVertexAttrib4Nuiv(fallbackVertexAttrib4Nuiv),
	glVertexAttrib4NuivARB(fallbackVertexAttrib4NuivARB),
	glVertexAttrib4Nusv(fallbackVertexAttrib4Nusv),
	glVertexAttrib4NusvARB(fallbackVertexAttrib4NusvARB),
	glVertexAttrib4bv(fallbackVertexAttrib4bv),
	glVertexAttrib4bvARB(fallbackVertexAttrib4bvARB),
	glVertexAttrib4d(fallbackVertexAttrib4d),
	glVertexAttrib4dARB(fallbackVertexAttrib4dARB),
	glVertexAttrib4dNV(fallbackVertexAttrib4dNV),
	glVertexAttrib4dv(fallbackVertexAttrib4dv),
	glVertexAttrib4dvARB(fallbackVertexAttrib4dvARB),
	glVertexAttrib4dvNV(fallbackVertexAttrib4dvNV),
	glVertexAttrib4f(fallbackVertexAttrib4f),
	glVertexAttrib4fARB(fallbackVertexAttrib4fARB),
	glVertexAttrib4fNV(fallbackVertexAttrib4fNV),
	glVertexAttrib4fv(fallbackVertexAttrib4fv),
	glVertexAttrib4fvARB(fallbackVertexAttrib4fvARB),
	glVertexAttrib4fvNV(fallbackVertexAttrib4fvNV),
	glVertexAttrib4hNV(fallbackVertexAttrib4hNV),
	glVertexAttrib4hvNV(fallbackVertexAttrib4hvNV),
	glVertexAttrib4iv(fallbackVertexAttrib4iv),
	glVertexAttrib4ivARB(fallbackVertexAttrib4ivARB),
	glVertexAttrib4s(fallbackVertexAttrib4s),
	glVertexAttrib4sARB(fallbackVertexAttrib4sARB),
	glVertexAttrib4sNV(fallbackVertexAttrib4sNV),
	glVertexAttrib4sv(fallbackVertexAttrib4sv),
	glVertexAttrib4svARB(fallbackVertexAttrib4svARB),
	glVertexAttrib4svNV(fallbackVertexAttrib4svNV),
	glVertexAttrib4ubNV(fallbackVertexAttrib4ubNV),
	glVertexAttrib4ubv(fallbackVertexAttrib4ubv),
	glVertexAttrib4ubvARB(fallbackVertexAttrib4ubvARB),
	glVertexAttrib4ubvNV(fallbackVertexAttrib4ubvNV),
	glVertexAttrib4uiv(fallbackVertexAttrib4uiv),
	glVertexAttrib4uivARB(fallbackVertexAttrib4uivARB),
	glVertexAttrib4usv(fallbackVertexAttrib4usv),
	glVertexAttrib4usvARB(fallbackVertexAttrib4usvARB),
	glVertexAttribArrayObjectATI(fallbackVertexAttribArrayObjectATI),
	glVertexAttribDivisorARB(fallbackVertexAttribDivisorARB),
	glVertexAttribFormatNV(fallbackVertexAttribFormatNV),
	glVertexAttribI1i(fallbackVertexAttribI1i),
	glVertexAttribI1iEXT(fallbackVertexAttribI1iEXT),
	glVertexAttribI1iv(fallbackVertexAttribI1iv),
	glVertexAttribI1ivEXT(fallbackVertexAttribI1ivEXT),
	glVertexAttribI1ui(fallbackVertexAttribI1ui),
	glVertexAttribI1uiEXT(fallbackVertexAttribI1uiEXT),
	glVertexAttribI1uiv(fallbackVertexAttribI1uiv),
	glVertexAttribI1uivEXT(fallbackVertexAttribI1uivEXT),
	glVertexAttribI2i(fallbackVertexAttribI2i),
	glVertexAttribI2iEXT(fallbackVertexAttribI2iEXT),
	glVertexAttribI2iv(fallbackVertexAttribI2iv),
	glVertexAttribI2ivEXT(fallbackVertexAttribI2ivEXT),
	glVertexAttribI2ui(fallbackVertexAttribI2ui),
	glVertexAttribI2uiEXT(fallbackVertexAttribI2uiEXT),
	glVertexAttribI2uiv(fallbackVertexAttribI2uiv),
	glVertexAttribI2uivEXT(fallbackVertexAttribI2uivEXT),
	glVertexAttribI3i(fallbackVertexAttribI3i),
	glVertexAttribI3iEXT(fallbackVertexAttribI3iEXT),
	glVertexAttribI3iv(fallbackVertexAttribI3iv),
	glVertexAttribI3ivEXT(fallbackVertexAttribI3ivEXT),
	glVertexAttribI3ui(fallbackVertexAttribI3ui),
	glVertexAttribI3uiEXT(fallbackVertexAttribI3uiEXT),
	glVertexAttribI3uiv(fallbackVertexAttribI3uiv),
	glVertexAttribI3uivEXT(fallbackVertexAttribI3uivEXT),
	glVertexAttribI4bv(fallbackVertexAttribI4bv),
	glVertexAttribI4bvEXT(fallbackVertexAttribI4bvEXT),
	glVertexAttribI4i(fallbackVertexAttribI4i),
	glVertexAttribI4iEXT(fallbackVertexAttribI4iEXT),
	glVertexAttribI4iv(fallbackVertexAttribI4iv),
	glVertexAttribI4ivEXT(fallbackVertexAttribI4ivEXT),
	glVertexAttribI4sv(fallbackVertexAttribI4sv),
	glVertexAttribI4svEXT(fallbackVertexAttribI4svEXT),
	glVertexAttribI4ubv(fallbackVertexAttribI4ubv),
	glVertexAttribI4ubvEXT(fallbackVertexAttribI4ubvEXT),
	glVertexAttribI4ui(fallbackVertexAttribI4ui),
	glVertexAttribI4uiEXT(fallbackVertexAttribI4uiEXT),
	glVertexAttribI4uiv(fallbackVertexAttribI4uiv),
	glVertexAttribI4uivEXT(fallbackVertexAttribI4uivEXT),
	glVertexAttribI4usv(fallbackVertexAttribI4usv),
	glVertexAttribI4usvEXT(fallbackVertexAttribI4usvEXT),
	glVertexAttribIFormatNV(fallbackVertexAttribIFormatNV),
	glVertexAttribIPointer(fallbackVertexAttribIPointer),
	glVertexAttribIPointerEXT(fallbackVertexAttribIPointerEXT),
	glVertexAttribP1ui(fallbackVertexAttribP1ui),
	glVertexAttribP1uiv(fallbackVertexAttribP1uiv),
	glVertexAttribP2ui(fallbackVertexAttribP2ui),
	glVertexAttribP2uiv(fallbackVertexAttribP2uiv),
	glVertexAttribP3ui(fallbackVertexAttribP3ui),
	glVertexAttribP3uiv(fallbackVertexAttribP3uiv),
	glVertexAttribP4ui(fallbackVertexAttribP4ui),
	glVertexAttribP4uiv(fallbackVertexAttribP4uiv),
	glVertexAttribPointer(fallbackVertexAttribPointer),
	glVertexAttribPointerARB(fallbackVertexAttribPointerARB),
	glVertexAttribPointerNV(fallbackVertexAttribPointerNV),
	glVertexAttribs1dvNV(fallbackVertexAttribs1dvNV),
	glVertexAttribs1fvNV(fallbackVertexAttribs1fvNV),
	glVertexAttribs1hvNV(fallbackVertexAttribs1hvNV),
	glVertexAttribs1svNV(fallbackVertexAttribs1svNV),
	glVertexAttribs2dvNV(fallbackVertexAttribs2dvNV),
	glVertexAttribs2fvNV(fallbackVertexAttribs2fvNV),
	glVertexAttribs2hvNV(fallbackVertexAttribs2hvNV),
	glVertexAttribs2svNV(fallbackVertexAttribs2svNV),
	glVertexAttribs3dvNV(fallbackVertexAttribs3dvNV),
	glVertexAttribs3fvNV(fallbackVertexAttribs3fvNV),
	glVertexAttribs3hvNV(fallbackVertexAttribs3hvNV),
	glVertexAttribs3svNV(fallbackVertexAttribs3svNV),
	glVertexAttribs4dvNV(fallbackVertexAttribs4dvNV),
	glVertexAttribs4fvNV(fallbackVertexAttribs4fvNV),
	glVertexAttribs4hvNV(fallbackVertexAttribs4hvNV),
	glVertexAttribs4svNV(fallbackVertexAttribs4svNV),
	glVertexAttribs4ubvNV(fallbackVertexAttribs4ubvNV),
	glVertexBlendARB(fallbackVertexBlendARB),
	glVertexBlendEnvfATI(fallbackVertexBlendEnvfATI),
	glVertexBlendEnviATI(fallbackVertexBlendEnviATI),
	glVertexFormatNV(fallbackVertexFormatNV),
	glVertexP2ui(fallbackVertexP2ui),
	glVertexP2uiv(fallbackVertexP2uiv),
	glVertexP3ui(fallbackVertexP3ui),
	glVertexP3uiv(fallbackVertexP3uiv),
	glVertexP4ui(fallbackVertexP4ui),
	glVertexP4uiv(fallbackVertexP4uiv),
	glVertexPointer(fallbackVertexPointer),
	glVertexPointerEXT(fallbackVertexPointerEXT),
	glVertexPointerListIBM(fallbackVertexPointerListIBM),
	glVertexPointervINTEL(fallbackVertexPointervINTEL),
	glVertexStream1dATI(fallbackVertexStream1dATI),
	glVertexStream1dvATI(fallbackVertexStream1dvATI),
	glVertexStream1fATI(fallbackVertexStream1fATI),
	glVertexStream1fvATI(fallbackVertexStream1fvATI),
	glVertexStream1iATI(fallbackVertexStream1iATI),
	glVertexStream1ivATI(fallbackVertexStream1ivATI),
	glVertexStream1sATI(fallbackVertexStream1sATI),
	glVertexStream1svATI(fallbackVertexStream1svATI),
	glVertexStream2dATI(fallbackVertexStream2dATI),
	glVertexStream2dvATI(fallbackVertexStream2dvATI),
	glVertexStream2fATI(fallbackVertexStream2fATI),
	glVertexStream2fvATI(fallbackVertexStream2fvATI),
	glVertexStream2iATI(fallbackVertexStream2iATI),
	glVertexStream2ivATI(fallbackVertexStream2ivATI),
	glVertexStream2sATI(fallbackVertexStream2sATI),
	glVertexStream2svATI(fallbackVertexStream2svATI),
	glVertexStream3dATI(fallbackVertexStream3dATI),
	glVertexStream3dvATI(fallbackVertexStream3dvATI),
	glVertexStream3fATI(fallbackVertexStream3fATI),
	glVertexStream3fvATI(fallbackVertexStream3fvATI),
	glVertexStream3iATI(fallbackVertexStream3iATI),
	glVertexStream3ivATI(fallbackVertexStream3ivATI),
	glVertexStream3sATI(fallbackVertexStream3sATI),
	glVertexStream3svATI(fallbackVertexStream3svATI),
	glVertexStream4dATI(fallbackVertexStream4dATI),
	glVertexStream4dvATI(fallbackVertexStream4dvATI),
	glVertexStream4fATI(fallbackVertexStream4fATI),
	glVertexStream4fvATI(fallbackVertexStream4fvATI),
	glVertexStream4iATI(fallbackVertexStream4iATI),
	glVertexStream4ivATI(fallbackVertexStream4ivATI),
	glVertexStream4sATI(fallbackVertexStream4sATI),
	glVertexStream4svATI(fallbackVertexStream4svATI),
	glVertexWeightPointerEXT(fallbackVertexWeightPointerEXT),
	glVertexWeightfEXT(fallbackVertexWeightfEXT),
	glVertexWeightfvEXT(fallbackVertexWeightfvEXT),
	glVertexWeighthNV(fallbackVertexWeighthNV),
	glVertexWeighthvNV(fallbackVertexWeighthvNV),
	glVideoCaptureNV(fallbackVideoCaptureNV),
	glVideoCaptureStreamParameterdvNV(fallbackVideoCaptureStreamParameterdvNV),
	glVideoCaptureStreamParameterfvNV(fallbackVideoCaptureStreamParameterfvNV),
	glVideoCaptureStreamParameterivNV(fallbackVideoCaptureStreamParameterivNV),
	glViewport(fallbackViewport),
	glWaitSync(fallbackWaitSync),
	glWeightPointerARB(fallbackWeightPointerARB),
	glWeightbvARB(fallbackWeightbvARB),
	glWeightdvARB(fallbackWeightdvARB),
	glWeightfvARB(fallbackWeightfvARB),
	glWeightivARB(fallbackWeightivARB),
	glWeightsvARB(fallbackWeightsvARB),
	glWeightubvARB(fallbackWeightubvARB),
	glWeightuivARB(fallbackWeightuivARB),
	glWeightusvARB(fallbackWeightusvARB),
	glWindowPos2d(fallbackWindowPos2d),
	glWindowPos2dARB(fallbackWindowPos2dARB),
	glWindowPos2dMESA(fallbackWindowPos2dMESA),
	glWindowPos2dv(fallbackWindowPos2dv),
	glWindowPos2dvARB(fallbackWindowPos2dvARB),
	glWindowPos2dvMESA(fallbackWindowPos2dvMESA),
	glWindowPos2f(fallbackWindowPos2f),
	glWindowPos2fARB(fallbackWindowPos2fARB),
	glWindowPos2fMESA(fallbackWindowPos2fMESA),
	glWindowPos2fv(fallbackWindowPos2fv),
	glWindowPos2fvARB(fallbackWindowPos2fvARB),
	glWindowPos2fvMESA(fallbackWindowPos2fvMESA),
	glWindowPos2i(fallbackWindowPos2i),
	glWindowPos2iARB(fallbackWindowPos2iARB),
	glWindowPos2iMESA(fallbackWindowPos2iMESA),
	glWindowPos2iv(fallbackWindowPos2iv),
	glWindowPos2ivARB(fallbackWindowPos2ivARB),
	glWindowPos2ivMESA(fallbackWindowPos2ivMESA),
	glWindowPos2s(fallbackWindowPos2s),
	glWindowPos2sARB(fallbackWindowPos2sARB),
	glWindowPos2sMESA(fallbackWindowPos2sMESA),
	glWindowPos2sv(fallbackWindowPos2sv),
	glWindowPos2svARB(fallbackWindowPos2svARB),
	glWindowPos2svMESA(fallbackWindowPos2svMESA),
	glWindowPos3d(fallbackWindowPos3d),
	glWindowPos3dARB(fallbackWindowPos3dARB),
	glWindowPos3dMESA(fallbackWindowPos3dMESA),
	glWindowPos3dv(fallbackWindowPos3dv),
	glWindowPos3dvARB(fallbackWindowPos3dvARB),
	glWindowPos3dvMESA(fallbackWindowPos3dvMESA),
	glWindowPos3f(fallbackWindowPos3f),
	glWindowPos3fARB(fallbackWindowPos3fARB),
	glWindowPos3fMESA(fallbackWindowPos3fMESA),
	glWindowPos3fv(fallbackWindowPos3fv),
	glWindowPos3fvARB(fallbackWindowPos3fvARB),
	glWindowPos3fvMESA(fallbackWindowPos3fvMESA),
	glWindowPos3i(fallbackWindowPos3i),
	glWindowPos3iARB(fallbackWindowPos3iARB),
	glWindowPos3iMESA(fallbackWindowPos3iMESA),
	glWindowPos3iv(fallbackWindowPos3iv),
	glWindowPos3ivARB(fallbackWindowPos3ivARB),
	glWindowPos3ivMESA(fallbackWindowPos3ivMESA),
	glWindowPos3s(fallbackWindowPos3s),
	glWindowPos3sARB(fallbackWindowPos3sARB),
	glWindowPos3sMESA(fallbackWindowPos3sMESA),
	glWindowPos3sv(fallbackWindowPos3sv),
	glWindowPos3svARB(fallbackWindowPos3svARB),
	glWindowPos3svMESA(fallbackWindowPos3svMESA),
	glWindowPos4dMESA(fallbackWindowPos4dMESA),
	glWindowPos4dvMESA(fallbackWindowPos4dvMESA),
	glWindowPos4fMESA(fallbackWindowPos4fMESA),
	glWindowPos4fvMESA(fallbackWindowPos4fvMESA),
	glWindowPos4iMESA(fallbackWindowPos4iMESA),
	glWindowPos4ivMESA(fallbackWindowPos4ivMESA),
	glWindowPos4sMESA(fallbackWindowPos4sMESA),
	glWindowPos4svMESA(fallbackWindowPos4svMESA),
	glWriteMaskEXT(fallbackWriteMaskEXT)
{
}

} // namespace gl

} // namespace k3d
