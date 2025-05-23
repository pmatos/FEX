#include <common/GeneratorInterface.h>

#define GL_GLEXT_PROTOTYPES 1
#define GLX_GLXEXT_PROTOTYPES 1

#include <GL/glx.h>
#include <GL/glxext.h>
#include <GL/gl.h>
#include <GL/glext.h>

#undef GL_ARB_viewport_array
#include "glcorearb.h"

#include <type_traits>

template<auto>
struct fex_gen_config {
  unsigned version = 1;
};

template<>
struct fex_gen_config<glXGetProcAddress> : fexgen::custom_host_impl, fexgen::custom_guest_entrypoint, fexgen::returns_guest_pointer {};

// internal use
void SetGuestMalloc(uintptr_t, uintptr_t);
void SetGuestXSync(uintptr_t, uintptr_t);
void SetGuestXGetVisualInfo(uintptr_t, uintptr_t);
void SetGuestXDisplayString(uintptr_t, uintptr_t);
template<>
struct fex_gen_config<SetGuestMalloc> : fexgen::custom_guest_entrypoint, fexgen::custom_host_impl {};
template<>
struct fex_gen_config<SetGuestXSync> : fexgen::custom_guest_entrypoint, fexgen::custom_host_impl {};
template<>
struct fex_gen_config<SetGuestXGetVisualInfo> : fexgen::custom_guest_entrypoint, fexgen::custom_host_impl {};
template<>
struct fex_gen_config<SetGuestXDisplayString> : fexgen::custom_guest_entrypoint, fexgen::custom_host_impl {};

template<typename>
struct fex_gen_type {};

// Assume void* always points to data with consistent layout.
// It's used in too many functions to annotate them all.
template<>
struct fex_gen_type<void> : fexgen::opaque_type {};

template<>
struct fex_gen_type<std::remove_pointer_t<GLXContext>> : fexgen::opaque_type {};
// NOTE: The data layout of this is almost the same between 64-bit and 32-bit,
//       but the total struct size is 4 bytes larger on 64-bit due to stricter
//       alignment requirements (8 vs 4 bytes). Since it's always allocated on
//       the host *and* never directly used in arrays, this is not a problem.
template<>
struct fex_gen_type<std::remove_pointer_t<GLXFBConfig>> : fexgen::opaque_type {};
template<>
struct fex_gen_type<std::remove_pointer_t<GLsync>> : fexgen::opaque_type {};

// NOTE: These should be opaque, but actually aren't because the respective libraries aren't thunked
template<>
struct fex_gen_type<_cl_context> : fexgen::opaque_type {};
template<>
struct fex_gen_type<_cl_event> : fexgen::opaque_type {};

// host_layout is manually customized for this. Mark as opaque to please the interface parser
template<>
struct fex_gen_type<_XDisplay> : fexgen::opaque_type {};

template<>
struct fex_gen_type<XVisualInfo> : fexgen::emit_layout_wrappers {};
template<>
struct fex_gen_type<Visual> : fexgen::opaque_type {}; // Used in XVisualInfo; treat as opaque

// Symbols queryable through glXGetProcAddr
namespace internal {
template<auto>
struct fex_gen_config : fexgen::generate_guest_symtable, fexgen::indirect_guest_calls {};

// Function, parameter index, parameter type [optional]
template<auto, int, typename = void>
struct fex_gen_param {};

template<>
struct fex_gen_config<glXQueryCurrentRendererStringMESA> {};
template<>
struct fex_gen_config<glXQueryRendererStringMESA> {};
template<>
struct fex_gen_config<glXGetContextIDEXT> {};
template<>
struct fex_gen_config<glXCreateContextWithConfigSGIX> {};
template<>
struct fex_gen_config<glXImportContextEXT> {};
template<>
struct fex_gen_config<glXGetCurrentReadDrawableSGI> {};
template<>
struct fex_gen_config<glXChooseFBConfigSGIX> : fexgen::custom_host_impl {};
template<>
struct fex_gen_param<glXChooseFBConfigSGIX, -1, GLXFBConfigSGIX*> : fexgen::ptr_passthrough {};
template<>
struct fex_gen_config<glXGetFBConfigFromVisualSGIX> : fexgen::custom_host_impl {};
template<>
struct fex_gen_param<glXGetFBConfigFromVisualSGIX, 1, XVisualInfo*> : fexgen::ptr_passthrough {};
template<>
struct fex_gen_config<glXCreateGLXPbufferSGIX> {};
template<>
struct fex_gen_config<glXCreateGLXPixmapWithConfigSGIX> {};
template<>
struct fex_gen_config<glXSwapBuffersMscOML> {};
template<>
struct fex_gen_config<glXGetFBConfigAttribSGIX> {};
template<>
struct fex_gen_config<glXGetMscRateOML> {};
template<>
struct fex_gen_config<glXGetSwapIntervalMESA> {};
template<>
struct fex_gen_config<glXGetSyncValuesOML> {};
template<>
struct fex_gen_config<glXGetVideoSyncSGI> {};
template<>
struct fex_gen_config<glXMakeCurrentReadSGI> {};
template<>
struct fex_gen_config<glXQueryContextInfoEXT> {};
template<>
struct fex_gen_config<glXQueryCurrentRendererIntegerMESA> {};
template<>
struct fex_gen_config<glXQueryRendererIntegerMESA> {};
template<>
struct fex_gen_config<glXSwapIntervalMESA> {};
template<>
struct fex_gen_config<glXSwapIntervalSGI> {};
template<>
struct fex_gen_config<glXWaitForMscOML> {};
template<>
struct fex_gen_config<glXWaitForSbcOML> {};
template<>
struct fex_gen_config<glXWaitVideoSyncSGI> {};
template<>
struct fex_gen_config<glXBindTexImageEXT> {};
template<>
struct fex_gen_config<glXCopySubBufferMESA> {};
template<>
struct fex_gen_config<glXDestroyGLXPbufferSGIX> {};
template<>
struct fex_gen_config<glXFreeContextEXT> {};
#ifndef IS_32BIT_THUNK
template<>
struct fex_gen_config<glXGetSelectedEventSGIX> {};
#else
template<>
struct fex_gen_config<glXGetSelectedEventSGIX> : fexgen::custom_host_impl {};
template<>
struct fex_gen_param<glXGetSelectedEventSGIX, 2, unsigned long*> : fexgen::ptr_passthrough {};
#endif

template<>
struct fex_gen_config<glXQueryGLXPbufferSGIX> {};
template<>
struct fex_gen_config<glXReleaseTexImageEXT> {};
template<>
struct fex_gen_config<glXSelectEventSGIX> {};
template<>
struct fex_gen_config<glXGetVisualFromFBConfigSGIX> : fexgen::custom_host_impl {};
template<>
struct fex_gen_param<glXGetVisualFromFBConfigSGIX, -1, XVisualInfo*> : fexgen::ptr_passthrough {};
template<>
struct fex_gen_config<glXGetClientString> {};
template<>
struct fex_gen_config<glXQueryExtensionsString> {};
template<>
struct fex_gen_config<glXQueryServerString> {};
template<>
struct fex_gen_config<glXGetCurrentDisplay> : fexgen::custom_host_impl {};
template<>
struct fex_gen_param<glXGetCurrentDisplay, -1, _XDisplay*> : fexgen::ptr_passthrough {};
template<>
struct fex_gen_config<glXCreateContext> : fexgen::custom_host_impl {};
template<>
struct fex_gen_param<glXCreateContext, 1, XVisualInfo*> : fexgen::ptr_passthrough {};
template<>
struct fex_gen_config<glXCreateNewContext> {};
template<>
struct fex_gen_config<glXGetCurrentContext> {};
template<>
struct fex_gen_config<glXGetCurrentDrawable> {};
template<>
struct fex_gen_config<glXGetCurrentReadDrawable> {};
template<>
struct fex_gen_config<glXChooseFBConfig> : fexgen::custom_host_impl {};
template<>
struct fex_gen_param<glXChooseFBConfig, -1, GLXFBConfig*> : fexgen::ptr_passthrough {};
template<>
struct fex_gen_config<glXGetFBConfigs> : fexgen::custom_host_impl {};
template<>
struct fex_gen_param<glXGetFBConfigs, -1, GLXFBConfig*> : fexgen::ptr_passthrough {};
template<>
struct fex_gen_config<glXCreatePbuffer> {};
template<>
struct fex_gen_config<glXCreateGLXPixmap> : fexgen::custom_host_impl {};
template<>
struct fex_gen_param<glXCreateGLXPixmap, 1, XVisualInfo*> : fexgen::ptr_passthrough {};
template<>
struct fex_gen_config<glXCreatePixmap> {};
template<>
struct fex_gen_config<glXCreateWindow> {};
template<>
struct fex_gen_config<glXGetConfig> : fexgen::custom_host_impl {};
template<>
struct fex_gen_param<glXGetConfig, 1, XVisualInfo*> : fexgen::ptr_passthrough {};
template<>
struct fex_gen_config<glXGetFBConfigAttrib> {};
template<>
struct fex_gen_config<glXIsDirect> {};
template<>
struct fex_gen_config<glXMakeContextCurrent> {};
template<>
struct fex_gen_config<glXMakeCurrent> {};
template<>
struct fex_gen_config<glXQueryContext> {};
template<>
struct fex_gen_config<glXQueryExtension> {};
template<>
struct fex_gen_config<glXQueryVersion> {};
template<>
struct fex_gen_config<glXCopyContext> {};
template<>
struct fex_gen_config<glXDestroyContext> {};
template<>
struct fex_gen_config<glXDestroyGLXPixmap> {};
template<>
struct fex_gen_config<glXDestroyPbuffer> {};
template<>
struct fex_gen_config<glXDestroyPixmap> {};
template<>
struct fex_gen_config<glXDestroyWindow> {};
#ifdef GLX_NV_vertex_array_range
template<>
struct fex_gen_config<glXAllocateMemoryNV> {};
template<>
struct fex_gen_config<glXFreeMemoryNV> {};
#endif
#ifndef IS_32BIT_THUNK
template<>
struct fex_gen_config<glXGetSelectedEvent> {};
#else
template<>
struct fex_gen_config<glXGetSelectedEvent> : fexgen::custom_host_impl {};
template<>
struct fex_gen_param<glXGetSelectedEvent, 2, unsigned long*> : fexgen::ptr_passthrough {};
#endif
template<>
struct fex_gen_config<glXQueryDrawable> {};
template<>
struct fex_gen_config<glXSelectEvent> {};
template<>
struct fex_gen_config<glXSwapBuffers> {};
template<>
struct fex_gen_config<glXUseXFont> {};
template<>
struct fex_gen_config<glXWaitGL> {};
template<>
struct fex_gen_config<glXChooseVisual> : fexgen::custom_host_impl {};
template<>
struct fex_gen_param<glXChooseVisual, -1, XVisualInfo*> : fexgen::ptr_passthrough {};
template<>
struct fex_gen_config<glXGetVisualFromFBConfig> : fexgen::custom_host_impl {};
template<>
struct fex_gen_param<glXGetVisualFromFBConfig, -1, XVisualInfo*> : fexgen::ptr_passthrough {};

// template<> struct fex_gen_config<glXCreateContextAttribs> {};
template<>
struct fex_gen_config<glXCreateContextAttribsARB> {};
template<>
struct fex_gen_config<glXSwapIntervalEXT> {};

template<>
struct fex_gen_config<glColorP3ui> {};
template<>
struct fex_gen_config<glColorP3uiv> {};
template<>
struct fex_gen_config<glColorP4ui> {};
template<>
struct fex_gen_config<glColorP4uiv> {};
template<>
struct fex_gen_config<glFogCoordd> {};
template<>
struct fex_gen_config<glFogCoorddv> {};
template<>
struct fex_gen_config<glFogCoordf> {};
template<>
struct fex_gen_config<glFogCoordfv> {};
template<>
struct fex_gen_config<glFogCoordPointer> {};
template<>
struct fex_gen_config<glGetnColorTableARB> {};
template<>
struct fex_gen_config<glGetnConvolutionFilterARB> {};
template<>
struct fex_gen_config<glGetnHistogramARB> {};
template<>
struct fex_gen_config<glGetnColorTable> {};
template<>
struct fex_gen_config<glGetnConvolutionFilter> {};
template<>
struct fex_gen_config<glGetnHistogram> {};
template<>
struct fex_gen_config<glGetnMapdv> {};
template<>
struct fex_gen_config<glGetnMapfv> {};
template<>
struct fex_gen_config<glGetnMapiv> {};
template<>
struct fex_gen_config<glGetnPixelMapfv> {};
template<>
struct fex_gen_config<glGetnPixelMapuiv> {};
template<>
struct fex_gen_config<glGetnPixelMapusv> {};
template<>
struct fex_gen_config<glGetnPolygonStipple> {};
template<>
struct fex_gen_config<glGetnSeparableFilter> {};
template<>
struct fex_gen_config<glGetnMinmax> {};
template<>
struct fex_gen_config<glGetnMapdvARB> {};
template<>
struct fex_gen_config<glGetnMapfvARB> {};
template<>
struct fex_gen_config<glGetnMapivARB> {};
template<>
struct fex_gen_config<glGetnMinmaxARB> {};
template<>
struct fex_gen_config<glGetnPixelMapfvARB> {};
template<>
struct fex_gen_config<glGetnPixelMapuivARB> {};
template<>
struct fex_gen_config<glGetnPixelMapusvARB> {};
template<>
struct fex_gen_config<glGetnPolygonStippleARB> {};
template<>
struct fex_gen_config<glGetnSeparableFilterARB> {};
template<>
struct fex_gen_config<glMultiTexCoordP1ui> {};
template<>
struct fex_gen_config<glMultiTexCoordP1uiv> {};
template<>
struct fex_gen_config<glMultiTexCoordP2ui> {};
template<>
struct fex_gen_config<glMultiTexCoordP2uiv> {};
template<>
struct fex_gen_config<glMultiTexCoordP3ui> {};
template<>
struct fex_gen_config<glMultiTexCoordP3uiv> {};
template<>
struct fex_gen_config<glMultiTexCoordP4ui> {};
template<>
struct fex_gen_config<glMultiTexCoordP4uiv> {};
template<>
struct fex_gen_config<glNormalP3ui> {};
template<>
struct fex_gen_config<glNormalP3uiv> {};
template<>
struct fex_gen_config<glSecondaryColor3b> {};
template<>
struct fex_gen_config<glSecondaryColor3bv> {};
template<>
struct fex_gen_config<glSecondaryColor3d> {};
template<>
struct fex_gen_config<glSecondaryColor3dv> {};
template<>
struct fex_gen_config<glSecondaryColor3f> {};
template<>
struct fex_gen_config<glSecondaryColor3fv> {};
template<>
struct fex_gen_config<glSecondaryColor3i> {};
template<>
struct fex_gen_config<glSecondaryColor3iv> {};
template<>
struct fex_gen_config<glSecondaryColor3s> {};
template<>
struct fex_gen_config<glSecondaryColor3sv> {};
template<>
struct fex_gen_config<glSecondaryColor3ub> {};
template<>
struct fex_gen_config<glSecondaryColor3ubv> {};
template<>
struct fex_gen_config<glSecondaryColor3ui> {};
template<>
struct fex_gen_config<glSecondaryColor3uiv> {};
template<>
struct fex_gen_config<glSecondaryColor3us> {};
template<>
struct fex_gen_config<glSecondaryColor3usv> {};
template<>
struct fex_gen_config<glSecondaryColorP3ui> {};
template<>
struct fex_gen_config<glSecondaryColorP3uiv> {};
template<>
struct fex_gen_config<glSecondaryColorPointer> {};
template<>
struct fex_gen_config<glTexCoordP1ui> {};
template<>
struct fex_gen_config<glTexCoordP1uiv> {};
template<>
struct fex_gen_config<glTexCoordP2ui> {};
template<>
struct fex_gen_config<glTexCoordP2uiv> {};
template<>
struct fex_gen_config<glTexCoordP3ui> {};
template<>
struct fex_gen_config<glTexCoordP3uiv> {};
template<>
struct fex_gen_config<glTexCoordP4ui> {};
template<>
struct fex_gen_config<glTexCoordP4uiv> {};
template<>
struct fex_gen_config<glVertexP2ui> {};
template<>
struct fex_gen_config<glVertexP2uiv> {};
template<>
struct fex_gen_config<glVertexP3ui> {};
template<>
struct fex_gen_config<glVertexP3uiv> {};
template<>
struct fex_gen_config<glVertexP4ui> {};
template<>
struct fex_gen_config<glVertexP4uiv> {};
template<>
struct fex_gen_config<glWindowPos2d> {};
template<>
struct fex_gen_config<glWindowPos2dv> {};
template<>
struct fex_gen_config<glWindowPos2f> {};
template<>
struct fex_gen_config<glWindowPos2fv> {};
template<>
struct fex_gen_config<glWindowPos2i> {};
template<>
struct fex_gen_config<glWindowPos2iv> {};
template<>
struct fex_gen_config<glWindowPos2s> {};
template<>
struct fex_gen_config<glWindowPos2sv> {};
template<>
struct fex_gen_config<glWindowPos3d> {};
template<>
struct fex_gen_config<glWindowPos3dv> {};
template<>
struct fex_gen_config<glWindowPos3f> {};
template<>
struct fex_gen_config<glWindowPos3fv> {};
template<>
struct fex_gen_config<glWindowPos3i> {};
template<>
struct fex_gen_config<glWindowPos3iv> {};
template<>
struct fex_gen_config<glWindowPos3s> {};
template<>
struct fex_gen_config<glWindowPos3sv> {};
template<>
struct fex_gen_config<glGetString> {};
template<>
struct fex_gen_config<glGetStringi> {};
template<>
struct fex_gen_config<glQueryMatrixxOES> {};
template<>
struct fex_gen_config<glAcquireKeyedMutexWin32EXT> {};
template<>
struct fex_gen_config<glAreProgramsResidentNV> {};
template<>
struct fex_gen_config<glAreTexturesResidentEXT> {};
template<>
struct fex_gen_config<glAreTexturesResident> {};
template<>
struct fex_gen_config<glIsAsyncMarkerSGIX> {};
template<>
struct fex_gen_config<glIsBufferARB> {};
template<>
struct fex_gen_config<glIsBuffer> {};
template<>
struct fex_gen_config<glIsBufferResidentNV> {};
template<>
struct fex_gen_config<glIsCommandListNV> {};
template<>
struct fex_gen_config<glIsEnabled> {};
template<>
struct fex_gen_config<glIsEnabledi> {};
template<>
struct fex_gen_config<glIsEnabledIndexedEXT> {};
template<>
struct fex_gen_config<glIsFenceAPPLE> {};
template<>
struct fex_gen_config<glIsFenceNV> {};
template<>
struct fex_gen_config<glIsFramebufferEXT> {};
template<>
struct fex_gen_config<glIsFramebuffer> {};
template<>
struct fex_gen_config<glIsImageHandleResidentARB> {};
template<>
struct fex_gen_config<glIsImageHandleResidentNV> {};
template<>
struct fex_gen_config<glIsList> {};
template<>
struct fex_gen_config<glIsMemoryObjectEXT> {};
template<>
struct fex_gen_config<glIsNameAMD> {};
template<>
struct fex_gen_config<glIsNamedBufferResidentNV> {};
template<>
struct fex_gen_config<glIsNamedStringARB> {};
template<>
struct fex_gen_config<glIsObjectBufferATI> {};
template<>
struct fex_gen_config<glIsOcclusionQueryNV> {};
template<>
struct fex_gen_config<glIsPathNV> {};
template<>
struct fex_gen_config<glIsPointInFillPathNV> {};
template<>
struct fex_gen_config<glIsPointInStrokePathNV> {};
template<>
struct fex_gen_config<glIsProgramARB> {};
template<>
struct fex_gen_config<glIsProgram> {};
template<>
struct fex_gen_config<glIsProgramNV> {};
template<>
struct fex_gen_config<glIsProgramPipeline> {};
template<>
struct fex_gen_config<glIsQueryARB> {};
template<>
struct fex_gen_config<glIsQuery> {};
template<>
struct fex_gen_config<glIsRenderbufferEXT> {};
template<>
struct fex_gen_config<glIsRenderbuffer> {};
template<>
struct fex_gen_config<glIsSampler> {};
template<>
struct fex_gen_config<glIsSemaphoreEXT> {};
template<>
struct fex_gen_config<glIsShader> {};
template<>
struct fex_gen_config<glIsStateNV> {};
template<>
struct fex_gen_config<glIsSync> {};
template<>
struct fex_gen_config<glIsTextureEXT> {};
template<>
struct fex_gen_config<glIsTexture> {};
template<>
struct fex_gen_config<glIsTextureHandleResidentARB> {};
template<>
struct fex_gen_config<glIsTextureHandleResidentNV> {};
template<>
struct fex_gen_config<glIsTransformFeedback> {};
template<>
struct fex_gen_config<glIsTransformFeedbackNV> {};
template<>
struct fex_gen_config<glIsVariantEnabledEXT> {};
template<>
struct fex_gen_config<glIsVertexArrayAPPLE> {};
template<>
struct fex_gen_config<glIsVertexArray> {};
template<>
struct fex_gen_config<glIsVertexAttribEnabledAPPLE> {};
template<>
struct fex_gen_config<glPointAlongPathNV> {};
template<>
struct fex_gen_config<glReleaseKeyedMutexWin32EXT> {};
template<>
struct fex_gen_config<glTestFenceAPPLE> {};
template<>
struct fex_gen_config<glTestFenceNV> {};
template<>
struct fex_gen_config<glTestObjectAPPLE> {};
template<>
struct fex_gen_config<glUnmapBufferARB> {};
template<>
struct fex_gen_config<glUnmapBuffer> {};
template<>
struct fex_gen_config<glUnmapNamedBufferEXT> {};
template<>
struct fex_gen_config<glUnmapNamedBuffer> {};
template<>
struct fex_gen_config<glVDPAUIsSurfaceNV> {};
template<>
struct fex_gen_config<glCheckFramebufferStatusEXT> {};
template<>
struct fex_gen_config<glCheckFramebufferStatus> {};
template<>
struct fex_gen_config<glCheckNamedFramebufferStatusEXT> {};
template<>
struct fex_gen_config<glCheckNamedFramebufferStatus> {};
template<>
struct fex_gen_config<glClientWaitSync> {};
template<>
struct fex_gen_config<glGetError> {};
template<>
struct fex_gen_config<glGetGraphicsResetStatus> {};
template<>
struct fex_gen_config<glGetGraphicsResetStatusARB> {};
template<>
struct fex_gen_config<glObjectPurgeableAPPLE> {};
template<>
struct fex_gen_config<glObjectUnpurgeableAPPLE> {};
template<>
struct fex_gen_config<glPathGlyphIndexArrayNV> {};
template<>
struct fex_gen_config<glPathGlyphIndexRangeNV> {};
template<>
struct fex_gen_config<glPathMemoryGlyphIndexArrayNV> {};
template<>
struct fex_gen_config<glVideoCaptureNV> {};
template<>
struct fex_gen_config<glGetPathLengthNV> {};
template<>
struct fex_gen_config<glCreateProgramObjectARB> {};
template<>
struct fex_gen_config<glCreateShaderObjectARB> {};
template<>
struct fex_gen_config<glGetHandleARB> {};
template<>
struct fex_gen_config<glFinishAsyncSGIX> {};
template<>
struct fex_gen_config<glGetAttribLocationARB> {};
template<>
struct fex_gen_config<glGetAttribLocation> {};
template<>
struct fex_gen_config<glGetFragDataIndex> {};
template<>
struct fex_gen_config<glGetFragDataLocationEXT> {};
template<>
struct fex_gen_config<glGetFragDataLocation> {};
template<>
struct fex_gen_config<glGetInstrumentsSGIX> {};
template<>
struct fex_gen_config<glGetProgramResourceLocation> {};
template<>
struct fex_gen_config<glGetProgramResourceLocationIndex> {};
template<>
struct fex_gen_config<glGetSubroutineUniformLocation> {};
template<>
struct fex_gen_config<glGetUniformBufferSizeEXT> {};
template<>
struct fex_gen_config<glGetUniformLocationARB> {};
template<>
struct fex_gen_config<glGetUniformLocation> {};
template<>
struct fex_gen_config<glGetVaryingLocationNV> {};
template<>
struct fex_gen_config<glPollAsyncSGIX> {};
template<>
struct fex_gen_config<glPollInstrumentsSGIX> {};
template<>
struct fex_gen_config<glQueryResourceNV> {};
template<>
struct fex_gen_config<glRenderMode> {};
template<>
struct fex_gen_config<glGetUniformOffsetEXT> {};
template<>
struct fex_gen_config<glCreateSyncFromCLeventARB> {};
template<>
struct fex_gen_config<glFenceSync> {};
template<>
struct fex_gen_config<glImportSyncEXT> {};
template<>
struct fex_gen_config<glGetImageHandleARB> {};
template<>
struct fex_gen_config<glGetImageHandleNV> {};
template<>
struct fex_gen_config<glGetTextureHandleARB> {};
template<>
struct fex_gen_config<glGetTextureHandleNV> {};
template<>
struct fex_gen_config<glGetTextureSamplerHandleARB> {};
template<>
struct fex_gen_config<glGetTextureSamplerHandleNV> {};
template<>
struct fex_gen_config<glAsyncCopyBufferSubDataNVX> {};
template<>
struct fex_gen_config<glAsyncCopyImageSubDataNVX> {};
template<>
struct fex_gen_config<glBindLightParameterEXT> {};
template<>
struct fex_gen_config<glBindMaterialParameterEXT> {};
template<>
struct fex_gen_config<glBindParameterEXT> {};
template<>
struct fex_gen_config<glBindTexGenParameterEXT> {};
template<>
struct fex_gen_config<glBindTextureUnitParameterEXT> {};
template<>
struct fex_gen_config<glCreateProgram> {};
template<>
struct fex_gen_config<glCreateProgressFenceNVX> {};
template<>
struct fex_gen_config<glCreateShader> {};
template<>
struct fex_gen_config<glCreateShaderProgramEXT> {};
template<>
struct fex_gen_config<glCreateShaderProgramv> : fexgen::custom_host_impl {};
template<>
struct fex_gen_param<glCreateShaderProgramv, 2, const GLchar* const*> : fexgen::ptr_passthrough {};
template<>
struct fex_gen_config<glGenAsyncMarkersSGIX> {};
template<>
struct fex_gen_config<glGenFragmentShadersATI> {};
template<>
struct fex_gen_config<glGenLists> {};
template<>
struct fex_gen_config<glGenPathsNV> {};
template<>
struct fex_gen_config<glGenSymbolsEXT> {};
template<>
struct fex_gen_config<glGenVertexShadersEXT> {};
template<>
struct fex_gen_config<glGetCommandHeaderNV> {};
template<>
struct fex_gen_config<glGetDebugMessageLogAMD> {};
template<>
struct fex_gen_config<glGetDebugMessageLogARB> {};
template<>
struct fex_gen_config<glGetDebugMessageLog> {};
template<>
struct fex_gen_config<glGetProgramResourceIndex> {};
template<>
struct fex_gen_config<glGetSubroutineIndex> {};
template<>
struct fex_gen_config<glGetUniformBlockIndex> {};
template<>
struct fex_gen_config<glNewObjectBufferATI> {};
template<>
struct fex_gen_config<glGetStageIndexNV> {};
template<>
struct fex_gen_config<glVDPAURegisterOutputSurfaceNV> {};
template<>
struct fex_gen_config<glVDPAURegisterVideoSurfaceNV> {};
template<>
struct fex_gen_config<glVDPAURegisterVideoSurfaceWithPictureStructureNV> {};
template<>
struct fex_gen_config<glAccum> {};
template<>
struct fex_gen_config<glAccumxOES> {};
template<>
struct fex_gen_config<glActiveProgramEXT> {};
template<>
struct fex_gen_config<glActiveShaderProgram> {};
template<>
struct fex_gen_config<glActiveStencilFaceEXT> {};
template<>
struct fex_gen_config<glActiveTextureARB> {};
template<>
struct fex_gen_config<glActiveTexture> {};
template<>
struct fex_gen_config<glActiveVaryingNV> {};
template<>
struct fex_gen_config<glAlphaFragmentOp1ATI> {};
template<>
struct fex_gen_config<glAlphaFragmentOp2ATI> {};
template<>
struct fex_gen_config<glAlphaFragmentOp3ATI> {};
template<>
struct fex_gen_config<glAlphaFunc> {};
template<>
struct fex_gen_config<glAlphaFuncxOES> {};
template<>
struct fex_gen_config<glAlphaToCoverageDitherControlNV> {};
template<>
struct fex_gen_config<glApplyFramebufferAttachmentCMAAINTEL> {};
template<>
struct fex_gen_config<glApplyTextureEXT> {};
template<>
struct fex_gen_config<glArrayElementEXT> {};
template<>
struct fex_gen_config<glArrayElement> {};
template<>
struct fex_gen_config<glArrayObjectATI> {};
template<>
struct fex_gen_config<glAsyncMarkerSGIX> {};
template<>
struct fex_gen_config<glAttachObjectARB> {};
template<>
struct fex_gen_config<glAttachShader> {};
template<>
struct fex_gen_config<glBeginConditionalRender> {};
template<>
struct fex_gen_config<glBeginConditionalRenderNV> {};
template<>
struct fex_gen_config<glBeginConditionalRenderNVX> {};
template<>
struct fex_gen_config<glBeginFragmentShaderATI> {};
template<>
struct fex_gen_config<glBegin> {};
template<>
struct fex_gen_config<glBeginOcclusionQueryNV> {};
template<>
struct fex_gen_config<glBeginPerfMonitorAMD> {};
template<>
struct fex_gen_config<glBeginPerfQueryINTEL> {};
template<>
struct fex_gen_config<glBeginQueryARB> {};
template<>
struct fex_gen_config<glBeginQuery> {};
template<>
struct fex_gen_config<glBeginQueryIndexed> {};
template<>
struct fex_gen_config<glBeginTransformFeedbackEXT> {};
template<>
struct fex_gen_config<glBeginTransformFeedback> {};
template<>
struct fex_gen_config<glBeginTransformFeedbackNV> {};
template<>
struct fex_gen_config<glBeginVertexShaderEXT> {};
template<>
struct fex_gen_config<glBeginVideoCaptureNV> {};
template<>
struct fex_gen_config<glBindAttribLocationARB> {};
template<>
struct fex_gen_config<glBindAttribLocation> {};
template<>
struct fex_gen_config<glBindBufferARB> {};
template<>
struct fex_gen_config<glBindBufferBaseEXT> {};
template<>
struct fex_gen_config<glBindBufferBase> {};
template<>
struct fex_gen_config<glBindBufferBaseNV> {};
template<>
struct fex_gen_config<glBindBuffer> {};
template<>
struct fex_gen_config<glBindBufferOffsetEXT> {};
template<>
struct fex_gen_config<glBindBufferOffsetNV> {};
template<>
struct fex_gen_config<glBindBufferRangeEXT> {};
template<>
struct fex_gen_config<glBindBufferRange> {};
template<>
struct fex_gen_config<glBindBufferRangeNV> {};
template<>
struct fex_gen_config<glBindBuffersBase> {};
#ifndef IS_32BIT_THUNK
template<>
struct fex_gen_config<glBindBuffersRange> {};
#else
template<>
struct fex_gen_config<glBindBuffersRange> : fexgen::custom_host_impl {};
template<>
struct fex_gen_param<glBindBuffersRange, 4, const GLintptr*> : fexgen::ptr_passthrough {};
template<>
struct fex_gen_param<glBindBuffersRange, 5, const GLsizeiptr*> : fexgen::ptr_passthrough {};
#endif
template<>
struct fex_gen_config<glBindFragDataLocationEXT> {};
template<>
struct fex_gen_config<glBindFragDataLocation> {};
template<>
struct fex_gen_config<glBindFragDataLocationIndexed> {};
template<>
struct fex_gen_config<glBindFragmentShaderATI> {};
template<>
struct fex_gen_config<glBindFramebufferEXT> {};
template<>
struct fex_gen_config<glBindFramebuffer> {};
template<>
struct fex_gen_config<glBindImageTextureEXT> {};
template<>
struct fex_gen_config<glBindImageTexture> {};
template<>
struct fex_gen_config<glBindImageTextures> {};
template<>
struct fex_gen_config<glBindMultiTextureEXT> {};
template<>
struct fex_gen_config<glBindProgramARB> {};
template<>
struct fex_gen_config<glBindProgramNV> {};
template<>
struct fex_gen_config<glBindProgramPipeline> {};
template<>
struct fex_gen_config<glBindRenderbufferEXT> {};
template<>
struct fex_gen_config<glBindRenderbuffer> {};
template<>
struct fex_gen_config<glBindSampler> {};
template<>
struct fex_gen_config<glBindSamplers> {};
template<>
struct fex_gen_config<glBindShadingRateImageNV> {};
template<>
struct fex_gen_config<glBindTextureEXT> {};
template<>
struct fex_gen_config<glBindTexture> {};
template<>
struct fex_gen_config<glBindTextures> {};
template<>
struct fex_gen_config<glBindTextureUnit> {};
template<>
struct fex_gen_config<glBindTransformFeedback> {};
template<>
struct fex_gen_config<glBindTransformFeedbackNV> {};
template<>
struct fex_gen_config<glBindVertexArrayAPPLE> {};
template<>
struct fex_gen_config<glBindVertexArray> {};
template<>
struct fex_gen_config<glBindVertexBuffer> {};
#ifndef IS_32BIT_THUNK
template<>
struct fex_gen_config<glBindVertexBuffers> {};
#else
template<>
struct fex_gen_config<glBindVertexBuffers> : fexgen::custom_host_impl {};
template<>
struct fex_gen_param<glBindVertexBuffers, 3, const GLintptr*> : fexgen::ptr_passthrough {};
#endif
template<>
struct fex_gen_config<glBindVertexShaderEXT> {};
template<>
struct fex_gen_config<glBindVideoCaptureStreamBufferNV> {};
template<>
struct fex_gen_config<glBindVideoCaptureStreamTextureNV> {};
template<>
struct fex_gen_config<glBinormal3bEXT> {};
template<>
struct fex_gen_config<glBinormal3bvEXT> {};
template<>
struct fex_gen_config<glBinormal3dEXT> {};
template<>
struct fex_gen_config<glBinormal3dvEXT> {};
template<>
struct fex_gen_config<glBinormal3fEXT> {};
template<>
struct fex_gen_config<glBinormal3fvEXT> {};
template<>
struct fex_gen_config<glBinormal3iEXT> {};
template<>
struct fex_gen_config<glBinormal3ivEXT> {};
template<>
struct fex_gen_config<glBinormal3sEXT> {};
template<>
struct fex_gen_config<glBinormal3svEXT> {};
template<>
struct fex_gen_config<glBinormalPointerEXT> {};
template<>
struct fex_gen_config<glBitmap> {};
template<>
struct fex_gen_config<glBitmapxOES> {};
template<>
struct fex_gen_config<glBlendBarrierKHR> {};
template<>
struct fex_gen_config<glBlendBarrierNV> {};
template<>
struct fex_gen_config<glBlendColorEXT> {};
template<>
struct fex_gen_config<glBlendColor> {};
template<>
struct fex_gen_config<glBlendColorxOES> {};
template<>
struct fex_gen_config<glBlendEquationEXT> {};
template<>
struct fex_gen_config<glBlendEquation> {};
template<>
struct fex_gen_config<glBlendEquationiARB> {};
template<>
struct fex_gen_config<glBlendEquationi> {};
template<>
struct fex_gen_config<glBlendEquationIndexedAMD> {};
template<>
struct fex_gen_config<glBlendEquationSeparateEXT> {};
template<>
struct fex_gen_config<glBlendEquationSeparate> {};
template<>
struct fex_gen_config<glBlendEquationSeparateiARB> {};
template<>
struct fex_gen_config<glBlendEquationSeparatei> {};
template<>
struct fex_gen_config<glBlendEquationSeparateIndexedAMD> {};
template<>
struct fex_gen_config<glBlendFunc> {};
template<>
struct fex_gen_config<glBlendFunciARB> {};
template<>
struct fex_gen_config<glBlendFunci> {};
template<>
struct fex_gen_config<glBlendFuncIndexedAMD> {};
template<>
struct fex_gen_config<glBlendFuncSeparateEXT> {};
template<>
struct fex_gen_config<glBlendFuncSeparate> {};
template<>
struct fex_gen_config<glBlendFuncSeparateiARB> {};
template<>
struct fex_gen_config<glBlendFuncSeparatei> {};
template<>
struct fex_gen_config<glBlendFuncSeparateIndexedAMD> {};
template<>
struct fex_gen_config<glBlendFuncSeparateINGR> {};
template<>
struct fex_gen_config<glBlendParameteriNV> {};
template<>
struct fex_gen_config<glBlitFramebufferEXT> {};
template<>
struct fex_gen_config<glBlitFramebuffer> {};
template<>
struct fex_gen_config<glBlitNamedFramebuffer> {};
template<>
struct fex_gen_config<glBufferAddressRangeNV> {};
template<>
struct fex_gen_config<glBufferAttachMemoryNV> {};
template<>
struct fex_gen_config<glBufferDataARB> {};
template<>
struct fex_gen_config<glBufferData> {};
template<>
struct fex_gen_config<glBufferPageCommitmentARB> {};
template<>
struct fex_gen_config<glBufferParameteriAPPLE> {};
template<>
struct fex_gen_config<glBufferStorageExternalEXT> {};
template<>
struct fex_gen_config<glBufferStorage> {};
template<>
struct fex_gen_config<glBufferStorageMemEXT> {};
template<>
struct fex_gen_config<glBufferSubDataARB> {};
template<>
struct fex_gen_config<glBufferSubData> {};
template<>
struct fex_gen_config<glCallCommandListNV> {};
template<>
struct fex_gen_config<glCallList> {};
template<>
struct fex_gen_config<glCallLists> {};
template<>
struct fex_gen_config<glClampColorARB> {};
template<>
struct fex_gen_config<glClampColor> {};
template<>
struct fex_gen_config<glClearAccum> {};
template<>
struct fex_gen_config<glClearAccumxOES> {};
template<>
struct fex_gen_config<glClearBufferData> {};
template<>
struct fex_gen_config<glClearBufferfi> {};
template<>
struct fex_gen_config<glClearBufferfv> {};
template<>
struct fex_gen_config<glClearBufferiv> {};
template<>
struct fex_gen_config<glClearBufferSubData> {};
template<>
struct fex_gen_config<glClearBufferuiv> {};
template<>
struct fex_gen_config<glClearColor> {};
template<>
struct fex_gen_config<glClearColorIiEXT> {};
template<>
struct fex_gen_config<glClearColorIuiEXT> {};
template<>
struct fex_gen_config<glClearColorxOES> {};
template<>
struct fex_gen_config<glClearDepthdNV> {};
template<>
struct fex_gen_config<glClearDepthf> {};
template<>
struct fex_gen_config<glClearDepthfOES> {};
template<>
struct fex_gen_config<glClearDepth> {};
template<>
struct fex_gen_config<glClearDepthxOES> {};
template<>
struct fex_gen_config<glClear> {};
template<>
struct fex_gen_config<glClearIndex> {};
template<>
struct fex_gen_config<glClearNamedBufferDataEXT> {};
template<>
struct fex_gen_config<glClearNamedBufferData> {};
template<>
struct fex_gen_config<glClearNamedBufferSubDataEXT> {};
template<>
struct fex_gen_config<glClearNamedBufferSubData> {};
template<>
struct fex_gen_config<glClearNamedFramebufferfi> {};
template<>
struct fex_gen_config<glClearNamedFramebufferfv> {};
template<>
struct fex_gen_config<glClearNamedFramebufferiv> {};
template<>
struct fex_gen_config<glClearNamedFramebufferuiv> {};
template<>
struct fex_gen_config<glClearStencil> {};
template<>
struct fex_gen_config<glClearTexImage> {};
template<>
struct fex_gen_config<glClearTexSubImage> {};
template<>
struct fex_gen_config<glClientActiveTextureARB> {};
template<>
struct fex_gen_config<glClientActiveTexture> {};
template<>
struct fex_gen_config<glClientActiveVertexStreamATI> {};
template<>
struct fex_gen_config<glClientAttribDefaultEXT> {};
template<>
struct fex_gen_config<glClientWaitSemaphoreui64NVX> {};
template<>
struct fex_gen_config<glClipControl> {};
template<>
struct fex_gen_config<glClipPlanefOES> {};
template<>
struct fex_gen_config<glClipPlane> {};
template<>
struct fex_gen_config<glClipPlanexOES> {};
template<>
struct fex_gen_config<glColor3b> {};
template<>
struct fex_gen_config<glColor3bv> {};
template<>
struct fex_gen_config<glColor3d> {};
template<>
struct fex_gen_config<glColor3dv> {};
template<>
struct fex_gen_config<glColor3f> {};
template<>
struct fex_gen_config<glColor3fv> {};
template<>
struct fex_gen_config<glColor3fVertex3fSUN> {};
template<>
struct fex_gen_config<glColor3fVertex3fvSUN> {};
template<>
struct fex_gen_config<glColor3hNV> {};
template<>
struct fex_gen_config<glColor3hvNV> {};
template<>
struct fex_gen_config<glColor3i> {};
template<>
struct fex_gen_config<glColor3iv> {};
template<>
struct fex_gen_config<glColor3s> {};
template<>
struct fex_gen_config<glColor3sv> {};
template<>
struct fex_gen_config<glColor3ub> {};
template<>
struct fex_gen_config<glColor3ubv> {};
template<>
struct fex_gen_config<glColor3ui> {};
template<>
struct fex_gen_config<glColor3uiv> {};
template<>
struct fex_gen_config<glColor3us> {};
template<>
struct fex_gen_config<glColor3usv> {};
template<>
struct fex_gen_config<glColor3xOES> {};
template<>
struct fex_gen_config<glColor3xvOES> {};
template<>
struct fex_gen_config<glColor4b> {};
template<>
struct fex_gen_config<glColor4bv> {};
template<>
struct fex_gen_config<glColor4d> {};
template<>
struct fex_gen_config<glColor4dv> {};
template<>
struct fex_gen_config<glColor4f> {};
template<>
struct fex_gen_config<glColor4fNormal3fVertex3fSUN> {};
template<>
struct fex_gen_config<glColor4fNormal3fVertex3fvSUN> {};
template<>
struct fex_gen_config<glColor4fv> {};
template<>
struct fex_gen_config<glColor4hNV> {};
template<>
struct fex_gen_config<glColor4hvNV> {};
template<>
struct fex_gen_config<glColor4i> {};
template<>
struct fex_gen_config<glColor4iv> {};
template<>
struct fex_gen_config<glColor4s> {};
template<>
struct fex_gen_config<glColor4sv> {};
template<>
struct fex_gen_config<glColor4ub> {};
template<>
struct fex_gen_config<glColor4ubv> {};
template<>
struct fex_gen_config<glColor4ubVertex2fSUN> {};
template<>
struct fex_gen_config<glColor4ubVertex2fvSUN> {};
template<>
struct fex_gen_config<glColor4ubVertex3fSUN> {};
template<>
struct fex_gen_config<glColor4ubVertex3fvSUN> {};
template<>
struct fex_gen_config<glColor4ui> {};
template<>
struct fex_gen_config<glColor4uiv> {};
template<>
struct fex_gen_config<glColor4us> {};
template<>
struct fex_gen_config<glColor4usv> {};
template<>
struct fex_gen_config<glColor4xOES> {};
template<>
struct fex_gen_config<glColor4xvOES> {};
template<>
struct fex_gen_config<glColorFormatNV> {};
template<>
struct fex_gen_config<glColorFragmentOp1ATI> {};
template<>
struct fex_gen_config<glColorFragmentOp2ATI> {};
template<>
struct fex_gen_config<glColorFragmentOp3ATI> {};
template<>
struct fex_gen_config<glColorMask> {};
template<>
struct fex_gen_config<glColorMaski> {};
template<>
struct fex_gen_config<glColorMaskIndexedEXT> {};
template<>
struct fex_gen_config<glColorMaterial> {};
template<>
struct fex_gen_config<glColorPointerEXT> {};
template<>
struct fex_gen_config<glColorPointer> {};
#ifndef IS_32BIT_THUNK
// TODO: 32-bit support
template<>
struct fex_gen_config<glColorPointerListIBM> {};
template<>
struct fex_gen_param<glColorPointerListIBM, 3, const void**> : fexgen::assume_compatible_data_layout {};
template<>
struct fex_gen_config<glColorPointervINTEL> {};
template<>
struct fex_gen_param<glColorPointervINTEL, 2, const void**> : fexgen::assume_compatible_data_layout {};
#endif
template<>
struct fex_gen_config<glColorSubTableEXT> {};
template<>
struct fex_gen_config<glColorSubTable> {};
template<>
struct fex_gen_config<glColorTableEXT> {};
template<>
struct fex_gen_config<glColorTable> {};
template<>
struct fex_gen_config<glColorTableParameterfv> {};
template<>
struct fex_gen_config<glColorTableParameterfvSGI> {};
template<>
struct fex_gen_config<glColorTableParameteriv> {};
template<>
struct fex_gen_config<glColorTableParameterivSGI> {};
template<>
struct fex_gen_config<glColorTableSGI> {};
template<>
struct fex_gen_config<glCombinerInputNV> {};
template<>
struct fex_gen_config<glCombinerOutputNV> {};
template<>
struct fex_gen_config<glCombinerParameterfNV> {};
template<>
struct fex_gen_config<glCombinerParameterfvNV> {};
template<>
struct fex_gen_config<glCombinerParameteriNV> {};
template<>
struct fex_gen_config<glCombinerParameterivNV> {};
template<>
struct fex_gen_config<glCombinerStageParameterfvNV> {};
template<>
struct fex_gen_config<glCommandListSegmentsNV> {};
template<>
struct fex_gen_config<glCompileCommandListNV> {};
template<>
struct fex_gen_config<glCompileShaderARB> {};
template<>
struct fex_gen_config<glCompileShader> {};
template<>
struct fex_gen_config<glCompileShaderIncludeARB> : fexgen::custom_host_impl {};
template<>
struct fex_gen_param<glCompileShaderIncludeARB, 2, const GLchar* const*> : fexgen::ptr_passthrough {};
template<>
struct fex_gen_config<glCompressedMultiTexImage1DEXT> {};
template<>
struct fex_gen_config<glCompressedMultiTexImage2DEXT> {};
template<>
struct fex_gen_config<glCompressedMultiTexImage3DEXT> {};
template<>
struct fex_gen_config<glCompressedMultiTexSubImage1DEXT> {};
template<>
struct fex_gen_config<glCompressedMultiTexSubImage2DEXT> {};
template<>
struct fex_gen_config<glCompressedMultiTexSubImage3DEXT> {};
template<>
struct fex_gen_config<glCompressedTexImage1DARB> {};
template<>
struct fex_gen_config<glCompressedTexImage1D> {};
template<>
struct fex_gen_config<glCompressedTexImage2DARB> {};
template<>
struct fex_gen_config<glCompressedTexImage2D> {};
template<>
struct fex_gen_config<glCompressedTexImage3DARB> {};
template<>
struct fex_gen_config<glCompressedTexImage3D> {};
template<>
struct fex_gen_config<glCompressedTexSubImage1DARB> {};
template<>
struct fex_gen_config<glCompressedTexSubImage1D> {};
template<>
struct fex_gen_config<glCompressedTexSubImage2DARB> {};
template<>
struct fex_gen_config<glCompressedTexSubImage2D> {};
template<>
struct fex_gen_config<glCompressedTexSubImage3DARB> {};
template<>
struct fex_gen_config<glCompressedTexSubImage3D> {};
template<>
struct fex_gen_config<glCompressedTextureImage1DEXT> {};
template<>
struct fex_gen_config<glCompressedTextureImage2DEXT> {};
template<>
struct fex_gen_config<glCompressedTextureImage3DEXT> {};
template<>
struct fex_gen_config<glCompressedTextureSubImage1DEXT> {};
template<>
struct fex_gen_config<glCompressedTextureSubImage1D> {};
template<>
struct fex_gen_config<glCompressedTextureSubImage2DEXT> {};
template<>
struct fex_gen_config<glCompressedTextureSubImage2D> {};
template<>
struct fex_gen_config<glCompressedTextureSubImage3DEXT> {};
template<>
struct fex_gen_config<glCompressedTextureSubImage3D> {};
template<>
struct fex_gen_config<glConservativeRasterParameterfNV> {};
template<>
struct fex_gen_config<glConservativeRasterParameteriNV> {};
template<>
struct fex_gen_config<glConvolutionFilter1DEXT> {};
template<>
struct fex_gen_config<glConvolutionFilter1D> {};
template<>
struct fex_gen_config<glConvolutionFilter2DEXT> {};
template<>
struct fex_gen_config<glConvolutionFilter2D> {};
template<>
struct fex_gen_config<glConvolutionParameterfEXT> {};
template<>
struct fex_gen_config<glConvolutionParameterf> {};
template<>
struct fex_gen_config<glConvolutionParameterfvEXT> {};
template<>
struct fex_gen_config<glConvolutionParameterfv> {};
template<>
struct fex_gen_config<glConvolutionParameteriEXT> {};
template<>
struct fex_gen_config<glConvolutionParameteri> {};
template<>
struct fex_gen_config<glConvolutionParameterivEXT> {};
template<>
struct fex_gen_config<glConvolutionParameteriv> {};
template<>
struct fex_gen_config<glConvolutionParameterxOES> {};
template<>
struct fex_gen_config<glConvolutionParameterxvOES> {};
template<>
struct fex_gen_config<glCopyBufferSubData> {};
template<>
struct fex_gen_config<glCopyColorSubTableEXT> {};
template<>
struct fex_gen_config<glCopyColorSubTable> {};
template<>
struct fex_gen_config<glCopyColorTable> {};
template<>
struct fex_gen_config<glCopyColorTableSGI> {};
template<>
struct fex_gen_config<glCopyConvolutionFilter1DEXT> {};
template<>
struct fex_gen_config<glCopyConvolutionFilter1D> {};
template<>
struct fex_gen_config<glCopyConvolutionFilter2DEXT> {};
template<>
struct fex_gen_config<glCopyConvolutionFilter2D> {};
template<>
struct fex_gen_config<glCopyImageSubData> {};
template<>
struct fex_gen_config<glCopyImageSubDataNV> {};
template<>
struct fex_gen_config<glCopyMultiTexImage1DEXT> {};
template<>
struct fex_gen_config<glCopyMultiTexImage2DEXT> {};
template<>
struct fex_gen_config<glCopyMultiTexSubImage1DEXT> {};
template<>
struct fex_gen_config<glCopyMultiTexSubImage2DEXT> {};
template<>
struct fex_gen_config<glCopyMultiTexSubImage3DEXT> {};
template<>
struct fex_gen_config<glCopyNamedBufferSubData> {};
template<>
struct fex_gen_config<glCopyPathNV> {};
template<>
struct fex_gen_config<glCopyPixels> {};
template<>
struct fex_gen_config<glCopyTexImage1DEXT> {};
template<>
struct fex_gen_config<glCopyTexImage1D> {};
template<>
struct fex_gen_config<glCopyTexImage2DEXT> {};
template<>
struct fex_gen_config<glCopyTexImage2D> {};
template<>
struct fex_gen_config<glCopyTexSubImage1DEXT> {};
template<>
struct fex_gen_config<glCopyTexSubImage1D> {};
template<>
struct fex_gen_config<glCopyTexSubImage2DEXT> {};
template<>
struct fex_gen_config<glCopyTexSubImage2D> {};
template<>
struct fex_gen_config<glCopyTexSubImage3DEXT> {};
template<>
struct fex_gen_config<glCopyTexSubImage3D> {};
template<>
struct fex_gen_config<glCopyTextureImage1DEXT> {};
template<>
struct fex_gen_config<glCopyTextureImage2DEXT> {};
template<>
struct fex_gen_config<glCopyTextureSubImage1DEXT> {};
template<>
struct fex_gen_config<glCopyTextureSubImage1D> {};
template<>
struct fex_gen_config<glCopyTextureSubImage2DEXT> {};
template<>
struct fex_gen_config<glCopyTextureSubImage2D> {};
template<>
struct fex_gen_config<glCopyTextureSubImage3DEXT> {};
template<>
struct fex_gen_config<glCopyTextureSubImage3D> {};
template<>
struct fex_gen_config<glCoverageModulationNV> {};
template<>
struct fex_gen_config<glCoverageModulationTableNV> {};
template<>
struct fex_gen_config<glCoverFillPathInstancedNV> {};
template<>
struct fex_gen_config<glCoverFillPathNV> {};
template<>
struct fex_gen_config<glCoverStrokePathInstancedNV> {};
template<>
struct fex_gen_config<glCoverStrokePathNV> {};
template<>
struct fex_gen_config<glCreateBuffers> {};
template<>
struct fex_gen_config<glCreateCommandListsNV> {};
template<>
struct fex_gen_config<glCreateFramebuffers> {};
template<>
struct fex_gen_config<glCreateMemoryObjectsEXT> {};
template<>
struct fex_gen_config<glCreatePerfQueryINTEL> {};
template<>
struct fex_gen_config<glCreateProgramPipelines> {};
template<>
struct fex_gen_config<glCreateQueries> {};
template<>
struct fex_gen_config<glCreateRenderbuffers> {};
template<>
struct fex_gen_config<glCreateSamplers> {};
template<>
struct fex_gen_config<glCreateStatesNV> {};
template<>
struct fex_gen_config<glCreateTextures> {};
template<>
struct fex_gen_config<glCreateTransformFeedbacks> {};
template<>
struct fex_gen_config<glCreateVertexArrays> {};
template<>
struct fex_gen_config<glCullFace> {};
template<>
struct fex_gen_config<glCullParameterdvEXT> {};
template<>
struct fex_gen_config<glCullParameterfvEXT> {};
template<>
struct fex_gen_config<glCurrentPaletteMatrixARB> {};
template<>
struct fex_gen_config<glDebugMessageCallbackAMD> : fexgen::callback_stub {};
template<>
struct fex_gen_config<glDebugMessageCallbackARB> : fexgen::callback_stub {};
template<>
struct fex_gen_config<glDebugMessageCallback> : fexgen::callback_stub {};
template<>
struct fex_gen_config<glDebugMessageControlARB> {};
template<>
struct fex_gen_config<glDebugMessageControl> {};
template<>
struct fex_gen_config<glDebugMessageEnableAMD> {};
template<>
struct fex_gen_config<glDebugMessageInsertAMD> {};
template<>
struct fex_gen_config<glDebugMessageInsertARB> {};
template<>
struct fex_gen_config<glDebugMessageInsert> {};
template<>
struct fex_gen_config<glDeformationMap3dSGIX> {};
template<>
struct fex_gen_config<glDeformationMap3fSGIX> {};
template<>
struct fex_gen_config<glDeformSGIX> {};
template<>
struct fex_gen_config<glDeleteAsyncMarkersSGIX> {};
template<>
struct fex_gen_config<glDeleteBuffersARB> {};
template<>
struct fex_gen_config<glDeleteBuffers> {};
template<>
struct fex_gen_config<glDeleteCommandListsNV> {};
template<>
struct fex_gen_config<glDeleteFencesAPPLE> {};
template<>
struct fex_gen_config<glDeleteFencesNV> {};
template<>
struct fex_gen_config<glDeleteFragmentShaderATI> {};
template<>
struct fex_gen_config<glDeleteFramebuffersEXT> {};
template<>
struct fex_gen_config<glDeleteFramebuffers> {};
template<>
struct fex_gen_config<glDeleteLists> {};
template<>
struct fex_gen_config<glDeleteMemoryObjectsEXT> {};
template<>
struct fex_gen_config<glDeleteNamedStringARB> {};
template<>
struct fex_gen_config<glDeleteNamesAMD> {};
template<>
struct fex_gen_config<glDeleteObjectARB> {};
template<>
struct fex_gen_config<glDeleteOcclusionQueriesNV> {};
template<>
struct fex_gen_config<glDeletePathsNV> {};
template<>
struct fex_gen_config<glDeletePerfMonitorsAMD> {};
template<>
struct fex_gen_config<glDeletePerfQueryINTEL> {};
template<>
struct fex_gen_config<glDeleteProgram> {};
template<>
struct fex_gen_config<glDeleteProgramPipelines> {};
template<>
struct fex_gen_config<glDeleteProgramsARB> {};
template<>
struct fex_gen_config<glDeleteProgramsNV> {};
template<>
struct fex_gen_config<glDeleteQueriesARB> {};
template<>
struct fex_gen_config<glDeleteQueries> {};
template<>
struct fex_gen_config<glDeleteQueryResourceTagNV> {};
template<>
struct fex_gen_config<glDeleteRenderbuffersEXT> {};
template<>
struct fex_gen_config<glDeleteRenderbuffers> {};
template<>
struct fex_gen_config<glDeleteSamplers> {};
template<>
struct fex_gen_config<glDeleteSemaphoresEXT> {};
template<>
struct fex_gen_config<glDeleteShader> {};
template<>
struct fex_gen_config<glDeleteStatesNV> {};
template<>
struct fex_gen_config<glDeleteSync> {};
template<>
struct fex_gen_config<glDeleteTexturesEXT> {};
template<>
struct fex_gen_config<glDeleteTextures> {};
template<>
struct fex_gen_config<glDeleteTransformFeedbacks> {};
template<>
struct fex_gen_config<glDeleteTransformFeedbacksNV> {};
template<>
struct fex_gen_config<glDeleteVertexArraysAPPLE> {};
template<>
struct fex_gen_config<glDeleteVertexArrays> {};
template<>
struct fex_gen_config<glDeleteVertexShaderEXT> {};
template<>
struct fex_gen_config<glDepthBoundsdNV> {};
template<>
struct fex_gen_config<glDepthBoundsEXT> {};
template<>
struct fex_gen_config<glDepthFunc> {};
template<>
struct fex_gen_config<glDepthMask> {};
template<>
struct fex_gen_config<glDepthRangeArraydvNV> {};
template<>
struct fex_gen_config<glDepthRangeArrayv> {};
template<>
struct fex_gen_config<glDepthRangedNV> {};
template<>
struct fex_gen_config<glDepthRangef> {};
template<>
struct fex_gen_config<glDepthRangefOES> {};
template<>
struct fex_gen_config<glDepthRange> {};
template<>
struct fex_gen_config<glDepthRangeIndexeddNV> {};
template<>
struct fex_gen_config<glDepthRangeIndexed> {};
template<>
struct fex_gen_config<glDepthRangexOES> {};
template<>
struct fex_gen_config<glDetachObjectARB> {};
template<>
struct fex_gen_config<glDetachShader> {};
template<>
struct fex_gen_config<glDetailTexFuncSGIS> {};
template<>
struct fex_gen_config<glDisableClientState> {};
template<>
struct fex_gen_config<glDisableClientStateiEXT> {};
template<>
struct fex_gen_config<glDisableClientStateIndexedEXT> {};
template<>
struct fex_gen_config<glDisable> {};
template<>
struct fex_gen_config<glDisablei> {};
template<>
struct fex_gen_config<glDisableIndexedEXT> {};
template<>
struct fex_gen_config<glDisableVariantClientStateEXT> {};
template<>
struct fex_gen_config<glDisableVertexArrayAttribEXT> {};
template<>
struct fex_gen_config<glDisableVertexArrayAttrib> {};
template<>
struct fex_gen_config<glDisableVertexArrayEXT> {};
template<>
struct fex_gen_config<glDisableVertexAttribAPPLE> {};
template<>
struct fex_gen_config<glDisableVertexAttribArrayARB> {};
template<>
struct fex_gen_config<glDisableVertexAttribArray> {};
template<>
struct fex_gen_config<glDispatchCompute> {};
template<>
struct fex_gen_config<glDispatchComputeGroupSizeARB> {};
template<>
struct fex_gen_config<glDispatchComputeIndirect> {};
template<>
struct fex_gen_config<glDrawArraysEXT> {};
template<>
struct fex_gen_config<glDrawArrays> {};
template<>
struct fex_gen_config<glDrawArraysIndirect> {};
template<>
struct fex_gen_config<glDrawArraysInstancedARB> {};
template<>
struct fex_gen_config<glDrawArraysInstancedBaseInstance> {};
template<>
struct fex_gen_config<glDrawArraysInstancedEXT> {};
template<>
struct fex_gen_config<glDrawArraysInstanced> {};
template<>
struct fex_gen_config<glDrawBuffer> {};
template<>
struct fex_gen_config<glDrawBuffersARB> {};
template<>
struct fex_gen_config<glDrawBuffersATI> {};
template<>
struct fex_gen_config<glDrawBuffers> {};
template<>
struct fex_gen_config<glDrawCommandsAddressNV> {};
#ifndef IS_32BIT_THUNK
// TODO: 32-bit support
template<>
struct fex_gen_config<glDrawCommandsNV> {};
#endif
template<>
struct fex_gen_config<glDrawCommandsStatesAddressNV> {};
#ifndef IS_32BIT_THUNK
// TODO: 32-bit support
template<>
struct fex_gen_config<glDrawCommandsStatesNV> {};
#endif
template<>
struct fex_gen_config<glDrawElementArrayAPPLE> {};
template<>
struct fex_gen_config<glDrawElementArrayATI> {};
template<>
struct fex_gen_config<glDrawElementsBaseVertex> {};
template<>
struct fex_gen_config<glDrawElements> {};
template<>
struct fex_gen_config<glDrawElementsIndirect> {};
template<>
struct fex_gen_config<glDrawElementsInstancedARB> {};
template<>
struct fex_gen_config<glDrawElementsInstancedBaseInstance> {};
template<>
struct fex_gen_config<glDrawElementsInstancedBaseVertexBaseInstance> {};
template<>
struct fex_gen_config<glDrawElementsInstancedBaseVertex> {};
template<>
struct fex_gen_config<glDrawElementsInstancedEXT> {};
template<>
struct fex_gen_config<glDrawElementsInstanced> {};
template<>
struct fex_gen_config<glDrawMeshArraysSUN> {};
template<>
struct fex_gen_config<glDrawMeshTasksIndirectNV> {};
template<>
struct fex_gen_config<glDrawMeshTasksNV> {};
template<>
struct fex_gen_config<glDrawPixels> {};
template<>
struct fex_gen_config<glDrawRangeElementArrayAPPLE> {};
template<>
struct fex_gen_config<glDrawRangeElementArrayATI> {};
template<>
struct fex_gen_config<glDrawRangeElementsBaseVertex> {};
template<>
struct fex_gen_config<glDrawRangeElementsEXT> {};
template<>
struct fex_gen_config<glDrawRangeElements> {};
template<>
struct fex_gen_config<glDrawTextureNV> {};
template<>
struct fex_gen_config<glDrawTransformFeedback> {};
template<>
struct fex_gen_config<glDrawTransformFeedbackInstanced> {};
template<>
struct fex_gen_config<glDrawTransformFeedbackNV> {};
template<>
struct fex_gen_config<glDrawTransformFeedbackStream> {};
template<>
struct fex_gen_config<glDrawTransformFeedbackStreamInstanced> {};
template<>
struct fex_gen_config<glDrawVkImageNV> {};
template<>
struct fex_gen_config<glEdgeFlagFormatNV> {};
template<>
struct fex_gen_config<glEdgeFlag> {};
template<>
struct fex_gen_config<glEdgeFlagPointerEXT> {};
template<>
struct fex_gen_config<glEdgeFlagPointer> {};
#ifndef IS_32BIT_THUNK
// TODO: 32-bit support
template<>
struct fex_gen_config<glEdgeFlagPointerListIBM> {};
template<>
struct fex_gen_param<glEdgeFlagPointerListIBM, 1, const GLboolean**> : fexgen::assume_compatible_data_layout {};
#endif
template<>
struct fex_gen_config<glEdgeFlagv> {};
template<>
struct fex_gen_config<glEGLImageTargetRenderbufferStorageOES> {};
template<>
struct fex_gen_config<glEGLImageTargetTexStorageEXT> {};
template<>
struct fex_gen_config<glEGLImageTargetTexture2DOES> {};
template<>
struct fex_gen_config<glEGLImageTargetTextureStorageEXT> {};
template<>
struct fex_gen_config<glElementPointerAPPLE> {};
template<>
struct fex_gen_config<glElementPointerATI> {};
template<>
struct fex_gen_config<glEnableClientState> {};
template<>
struct fex_gen_config<glEnableClientStateiEXT> {};
template<>
struct fex_gen_config<glEnableClientStateIndexedEXT> {};
template<>
struct fex_gen_config<glEnable> {};
template<>
struct fex_gen_config<glEnablei> {};
template<>
struct fex_gen_config<glEnableIndexedEXT> {};
template<>
struct fex_gen_config<glEnableVariantClientStateEXT> {};
template<>
struct fex_gen_config<glEnableVertexArrayAttribEXT> {};
template<>
struct fex_gen_config<glEnableVertexArrayAttrib> {};
template<>
struct fex_gen_config<glEnableVertexArrayEXT> {};
template<>
struct fex_gen_config<glEnableVertexAttribAPPLE> {};
template<>
struct fex_gen_config<glEnableVertexAttribArrayARB> {};
template<>
struct fex_gen_config<glEnableVertexAttribArray> {};
template<>
struct fex_gen_config<glEnd> {};
template<>
struct fex_gen_config<glEndConditionalRender> {};
template<>
struct fex_gen_config<glEndConditionalRenderNV> {};
template<>
struct fex_gen_config<glEndConditionalRenderNVX> {};
template<>
struct fex_gen_config<glEndFragmentShaderATI> {};
template<>
struct fex_gen_config<glEndList> {};
template<>
struct fex_gen_config<glEndOcclusionQueryNV> {};
template<>
struct fex_gen_config<glEndPerfMonitorAMD> {};
template<>
struct fex_gen_config<glEndPerfQueryINTEL> {};
template<>
struct fex_gen_config<glEndQueryARB> {};
template<>
struct fex_gen_config<glEndQuery> {};
template<>
struct fex_gen_config<glEndQueryIndexed> {};
template<>
struct fex_gen_config<glEndTransformFeedback> {};
template<>
struct fex_gen_config<glEndTransformFeedbackEXT> {};
template<>
struct fex_gen_config<glEndTransformFeedbackNV> {};
template<>
struct fex_gen_config<glEndVertexShaderEXT> {};
template<>
struct fex_gen_config<glEndVideoCaptureNV> {};
template<>
struct fex_gen_config<glEvalCoord1d> {};
template<>
struct fex_gen_config<glEvalCoord1dv> {};
template<>
struct fex_gen_config<glEvalCoord1f> {};
template<>
struct fex_gen_config<glEvalCoord1fv> {};
template<>
struct fex_gen_config<glEvalCoord1xOES> {};
template<>
struct fex_gen_config<glEvalCoord1xvOES> {};
template<>
struct fex_gen_config<glEvalCoord2d> {};
template<>
struct fex_gen_config<glEvalCoord2dv> {};
template<>
struct fex_gen_config<glEvalCoord2f> {};
template<>
struct fex_gen_config<glEvalCoord2fv> {};
template<>
struct fex_gen_config<glEvalCoord2xOES> {};
template<>
struct fex_gen_config<glEvalCoord2xvOES> {};
template<>
struct fex_gen_config<glEvalMapsNV> {};
template<>
struct fex_gen_config<glEvalMesh1> {};
template<>
struct fex_gen_config<glEvalMesh2> {};
template<>
struct fex_gen_config<glEvalPoint1> {};
template<>
struct fex_gen_config<glEvalPoint2> {};
template<>
struct fex_gen_config<glEvaluateDepthValuesARB> {};
template<>
struct fex_gen_config<glExecuteProgramNV> {};
template<>
struct fex_gen_config<glExtractComponentEXT> {};
template<>
struct fex_gen_config<glFeedbackBuffer> {};
template<>
struct fex_gen_config<glFeedbackBufferxOES> {};
template<>
struct fex_gen_config<glFinalCombinerInputNV> {};
template<>
struct fex_gen_config<glFinish> {};
template<>
struct fex_gen_config<glFinishFenceAPPLE> {};
template<>
struct fex_gen_config<glFinishFenceNV> {};
template<>
struct fex_gen_config<glFinishObjectAPPLE> {};
template<>
struct fex_gen_config<glFinishTextureSUNX> {};
template<>
struct fex_gen_config<glFlush> {};
template<>
struct fex_gen_config<glFlushMappedBufferRangeAPPLE> {};
template<>
struct fex_gen_config<glFlushMappedBufferRange> {};
template<>
struct fex_gen_config<glFlushMappedNamedBufferRangeEXT> {};
template<>
struct fex_gen_config<glFlushMappedNamedBufferRange> {};
template<>
struct fex_gen_config<glFlushPixelDataRangeNV> {};
template<>
struct fex_gen_config<glFlushRasterSGIX> {};
template<>
struct fex_gen_config<glFlushStaticDataIBM> {};
template<>
struct fex_gen_config<glFlushVertexArrayRangeAPPLE> {};
template<>
struct fex_gen_config<glFlushVertexArrayRangeNV> {};
template<>
struct fex_gen_config<glFogCoorddEXT> {};
template<>
struct fex_gen_config<glFogCoorddvEXT> {};
template<>
struct fex_gen_config<glFogCoordfEXT> {};
template<>
struct fex_gen_config<glFogCoordFormatNV> {};
template<>
struct fex_gen_config<glFogCoordfvEXT> {};
template<>
struct fex_gen_config<glFogCoordhNV> {};
template<>
struct fex_gen_config<glFogCoordhvNV> {};
template<>
struct fex_gen_config<glFogCoordPointerEXT> {};
#ifndef IS_32BIT_THUNK
// TODO: 32-bit support
template<>
struct fex_gen_config<glFogCoordPointerListIBM> {};
template<>
struct fex_gen_param<glFogCoordPointerListIBM, 2, const void**> : fexgen::assume_compatible_data_layout {};
#endif
template<>
struct fex_gen_config<glFogf> {};
template<>
struct fex_gen_config<glFogFuncSGIS> {};
template<>
struct fex_gen_config<glFogfv> {};
template<>
struct fex_gen_config<glFogi> {};
template<>
struct fex_gen_config<glFogiv> {};
template<>
struct fex_gen_config<glFogxOES> {};
template<>
struct fex_gen_config<glFogxvOES> {};
template<>
struct fex_gen_config<glFragmentColorMaterialSGIX> {};
template<>
struct fex_gen_config<glFragmentCoverageColorNV> {};
template<>
struct fex_gen_config<glFragmentLightfSGIX> {};
template<>
struct fex_gen_config<glFragmentLightfvSGIX> {};
template<>
struct fex_gen_config<glFragmentLightiSGIX> {};
template<>
struct fex_gen_config<glFragmentLightivSGIX> {};
template<>
struct fex_gen_config<glFragmentLightModelfSGIX> {};
template<>
struct fex_gen_config<glFragmentLightModelfvSGIX> {};
template<>
struct fex_gen_config<glFragmentLightModeliSGIX> {};
template<>
struct fex_gen_config<glFragmentLightModelivSGIX> {};
template<>
struct fex_gen_config<glFragmentMaterialfSGIX> {};
template<>
struct fex_gen_config<glFragmentMaterialfvSGIX> {};
template<>
struct fex_gen_config<glFragmentMaterialiSGIX> {};
template<>
struct fex_gen_config<glFragmentMaterialivSGIX> {};
template<>
struct fex_gen_config<glFramebufferDrawBufferEXT> {};
template<>
struct fex_gen_config<glFramebufferDrawBuffersEXT> {};
template<>
struct fex_gen_config<glFramebufferFetchBarrierEXT> {};
template<>
struct fex_gen_config<glFramebufferParameteri> {};
template<>
struct fex_gen_config<glFramebufferParameteriMESA> {};
template<>
struct fex_gen_config<glFramebufferReadBufferEXT> {};
template<>
struct fex_gen_config<glFramebufferRenderbufferEXT> {};
template<>
struct fex_gen_config<glFramebufferRenderbuffer> {};
template<>
struct fex_gen_config<glFramebufferSampleLocationsfvARB> {};
template<>
struct fex_gen_config<glFramebufferSampleLocationsfvNV> {};
template<>
struct fex_gen_config<glFramebufferSamplePositionsfvAMD> {};
template<>
struct fex_gen_config<glFramebufferTexture1DEXT> {};
template<>
struct fex_gen_config<glFramebufferTexture1D> {};
template<>
struct fex_gen_config<glFramebufferTexture2DEXT> {};
template<>
struct fex_gen_config<glFramebufferTexture2D> {};
template<>
struct fex_gen_config<glFramebufferTexture3DEXT> {};
template<>
struct fex_gen_config<glFramebufferTexture3D> {};
template<>
struct fex_gen_config<glFramebufferTextureARB> {};
template<>
struct fex_gen_config<glFramebufferTextureEXT> {};
template<>
struct fex_gen_config<glFramebufferTextureFaceARB> {};
template<>
struct fex_gen_config<glFramebufferTextureFaceEXT> {};
template<>
struct fex_gen_config<glFramebufferTexture> {};
template<>
struct fex_gen_config<glFramebufferTextureLayerARB> {};
template<>
struct fex_gen_config<glFramebufferTextureLayerEXT> {};
template<>
struct fex_gen_config<glFramebufferTextureLayer> {};
template<>
struct fex_gen_config<glFramebufferTextureMultiviewOVR> {};
template<>
struct fex_gen_config<glFrameTerminatorGREMEDY> {};
template<>
struct fex_gen_config<glFrameZoomSGIX> {};
template<>
struct fex_gen_config<glFreeObjectBufferATI> {};
template<>
struct fex_gen_config<glFrontFace> {};
template<>
struct fex_gen_config<glFrustumfOES> {};
template<>
struct fex_gen_config<glFrustum> {};
template<>
struct fex_gen_config<glFrustumxOES> {};
template<>
struct fex_gen_config<glGenBuffersARB> {};
template<>
struct fex_gen_config<glGenBuffers> {};
template<>
struct fex_gen_config<glGenerateMipmapEXT> {};
template<>
struct fex_gen_config<glGenerateMipmap> {};
template<>
struct fex_gen_config<glGenerateMultiTexMipmapEXT> {};
template<>
struct fex_gen_config<glGenerateTextureMipmapEXT> {};
template<>
struct fex_gen_config<glGenerateTextureMipmap> {};
template<>
struct fex_gen_config<glGenFencesAPPLE> {};
template<>
struct fex_gen_config<glGenFencesNV> {};
template<>
struct fex_gen_config<glGenFramebuffersEXT> {};
template<>
struct fex_gen_config<glGenFramebuffers> {};
template<>
struct fex_gen_config<glGenNamesAMD> {};
template<>
struct fex_gen_config<glGenOcclusionQueriesNV> {};
template<>
struct fex_gen_config<glGenPerfMonitorsAMD> {};
template<>
struct fex_gen_config<glGenProgramPipelines> {};
template<>
struct fex_gen_config<glGenProgramsARB> {};
template<>
struct fex_gen_config<glGenProgramsNV> {};
template<>
struct fex_gen_config<glGenQueriesARB> {};
template<>
struct fex_gen_config<glGenQueries> {};
template<>
struct fex_gen_config<glGenQueryResourceTagNV> {};
template<>
struct fex_gen_config<glGenRenderbuffersEXT> {};
template<>
struct fex_gen_config<glGenRenderbuffers> {};
template<>
struct fex_gen_config<glGenSamplers> {};
template<>
struct fex_gen_config<glGenSemaphoresEXT> {};
template<>
struct fex_gen_config<glGenTexturesEXT> {};
template<>
struct fex_gen_config<glGenTextures> {};
template<>
struct fex_gen_config<glGenTransformFeedbacks> {};
template<>
struct fex_gen_config<glGenTransformFeedbacksNV> {};
template<>
struct fex_gen_config<glGenVertexArraysAPPLE> {};
template<>
struct fex_gen_config<glGenVertexArrays> {};
template<>
struct fex_gen_config<glGetActiveAtomicCounterBufferiv> {};
template<>
struct fex_gen_config<glGetActiveAttribARB> {};
template<>
struct fex_gen_config<glGetActiveAttrib> {};
template<>
struct fex_gen_config<glGetActiveSubroutineName> {};
template<>
struct fex_gen_config<glGetActiveSubroutineUniformiv> {};
template<>
struct fex_gen_config<glGetActiveSubroutineUniformName> {};
template<>
struct fex_gen_config<glGetActiveUniformARB> {};
template<>
struct fex_gen_config<glGetActiveUniformBlockiv> {};
template<>
struct fex_gen_config<glGetActiveUniformBlockName> {};
template<>
struct fex_gen_config<glGetActiveUniform> {};
template<>
struct fex_gen_config<glGetActiveUniformName> {};
template<>
struct fex_gen_config<glGetActiveUniformsiv> {};
template<>
struct fex_gen_config<glGetActiveVaryingNV> {};
template<>
struct fex_gen_config<glGetArrayObjectfvATI> {};
template<>
struct fex_gen_config<glGetArrayObjectivATI> {};
template<>
struct fex_gen_config<glGetAttachedObjectsARB> {};
template<>
struct fex_gen_config<glGetAttachedShaders> {};
template<>
struct fex_gen_config<glGetBooleanIndexedvEXT> {};
template<>
struct fex_gen_config<glGetBooleani_v> {};
template<>
struct fex_gen_config<glGetBooleanv> {};
template<>
struct fex_gen_config<glGetBufferParameteri64v> {};
template<>
struct fex_gen_config<glGetBufferParameterivARB> {};
template<>
struct fex_gen_config<glGetBufferParameteriv> {};
template<>
struct fex_gen_config<glGetBufferParameterui64vNV> {};
template<>
struct fex_gen_config<glGetBufferPointerv> : fexgen::custom_host_impl {};
template<>
struct fex_gen_param<glGetBufferPointerv, 2, void**> : fexgen::ptr_passthrough {};
template<>
struct fex_gen_config<glGetBufferPointervARB> : fexgen::custom_host_impl {};
template<>
struct fex_gen_param<glGetBufferPointervARB, 2, void**> : fexgen::ptr_passthrough {};
template<>
struct fex_gen_config<glGetBufferSubDataARB> {};
template<>
struct fex_gen_config<glGetBufferSubData> {};
template<>
struct fex_gen_config<glGetClipPlanefOES> {};
template<>
struct fex_gen_config<glGetClipPlane> {};
template<>
struct fex_gen_config<glGetClipPlanexOES> {};
template<>
struct fex_gen_config<glGetColorTableEXT> {};
template<>
struct fex_gen_config<glGetColorTable> {};
template<>
struct fex_gen_config<glGetColorTableParameterfvEXT> {};
template<>
struct fex_gen_config<glGetColorTableParameterfv> {};
template<>
struct fex_gen_config<glGetColorTableParameterfvSGI> {};
template<>
struct fex_gen_config<glGetColorTableParameterivEXT> {};
template<>
struct fex_gen_config<glGetColorTableParameteriv> {};
template<>
struct fex_gen_config<glGetColorTableParameterivSGI> {};
template<>
struct fex_gen_config<glGetColorTableSGI> {};
template<>
struct fex_gen_config<glGetCombinerInputParameterfvNV> {};
template<>
struct fex_gen_config<glGetCombinerInputParameterivNV> {};
template<>
struct fex_gen_config<glGetCombinerOutputParameterfvNV> {};
template<>
struct fex_gen_config<glGetCombinerOutputParameterivNV> {};
template<>
struct fex_gen_config<glGetCombinerStageParameterfvNV> {};
template<>
struct fex_gen_config<glGetCompressedMultiTexImageEXT> {};
template<>
struct fex_gen_config<glGetCompressedTexImageARB> {};
template<>
struct fex_gen_config<glGetCompressedTexImage> {};
template<>
struct fex_gen_config<glGetCompressedTextureImageEXT> {};
template<>
struct fex_gen_config<glGetCompressedTextureImage> {};
template<>
struct fex_gen_config<glGetCompressedTextureSubImage> {};
template<>
struct fex_gen_config<glGetConvolutionFilterEXT> {};
template<>
struct fex_gen_config<glGetConvolutionFilter> {};
template<>
struct fex_gen_config<glGetConvolutionParameterfvEXT> {};
template<>
struct fex_gen_config<glGetConvolutionParameterfv> {};
template<>
struct fex_gen_config<glGetConvolutionParameterivEXT> {};
template<>
struct fex_gen_config<glGetConvolutionParameteriv> {};
template<>
struct fex_gen_config<glGetConvolutionParameterxvOES> {};
template<>
struct fex_gen_config<glGetCoverageModulationTableNV> {};
template<>
struct fex_gen_config<glGetDetailTexFuncSGIS> {};
template<>
struct fex_gen_config<glGetDoubleIndexedvEXT> {};
template<>
struct fex_gen_config<glGetDoublei_vEXT> {};
template<>
struct fex_gen_config<glGetDoublei_v> {};
template<>
struct fex_gen_config<glGetDoublev> {};
template<>
struct fex_gen_config<glGetFenceivNV> {};
template<>
struct fex_gen_config<glGetFinalCombinerInputParameterfvNV> {};
template<>
struct fex_gen_config<glGetFinalCombinerInputParameterivNV> {};
template<>
struct fex_gen_config<glGetFirstPerfQueryIdINTEL> {};
template<>
struct fex_gen_config<glGetFixedvOES> {};
template<>
struct fex_gen_config<glGetFloatIndexedvEXT> {};
template<>
struct fex_gen_config<glGetFloati_vEXT> {};
template<>
struct fex_gen_config<glGetFloati_v> {};
template<>
struct fex_gen_config<glGetFloatv> {};
template<>
struct fex_gen_config<glGetFogFuncSGIS> {};
template<>
struct fex_gen_config<glGetFragmentLightfvSGIX> {};
template<>
struct fex_gen_config<glGetFragmentLightivSGIX> {};
template<>
struct fex_gen_config<glGetFragmentMaterialfvSGIX> {};
template<>
struct fex_gen_config<glGetFragmentMaterialivSGIX> {};
template<>
struct fex_gen_config<glGetFramebufferAttachmentParameterivEXT> {};
template<>
struct fex_gen_config<glGetFramebufferAttachmentParameteriv> {};
template<>
struct fex_gen_config<glGetFramebufferParameterfvAMD> {};
template<>
struct fex_gen_config<glGetFramebufferParameterivEXT> {};
template<>
struct fex_gen_config<glGetFramebufferParameteriv> {};
template<>
struct fex_gen_config<glGetFramebufferParameterivMESA> {};
template<>
struct fex_gen_config<glGetHistogramEXT> {};
template<>
struct fex_gen_config<glGetHistogram> {};
template<>
struct fex_gen_config<glGetHistogramParameterfvEXT> {};
template<>
struct fex_gen_config<glGetHistogramParameterfv> {};
template<>
struct fex_gen_config<glGetHistogramParameterivEXT> {};
template<>
struct fex_gen_config<glGetHistogramParameteriv> {};
template<>
struct fex_gen_config<glGetHistogramParameterxvOES> {};
template<>
struct fex_gen_config<glGetImageTransformParameterfvHP> {};
template<>
struct fex_gen_config<glGetImageTransformParameterivHP> {};
template<>
struct fex_gen_config<glGetInfoLogARB> {};
template<>
struct fex_gen_config<glGetInteger64i_v> {};
template<>
struct fex_gen_config<glGetInteger64v> {};
template<>
struct fex_gen_config<glGetIntegerIndexedvEXT> {};
template<>
struct fex_gen_config<glGetIntegeri_v> {};
template<>
struct fex_gen_config<glGetIntegerui64i_vNV> {};
template<>
struct fex_gen_config<glGetIntegerui64vNV> {};
template<>
struct fex_gen_config<glGetIntegerv> {};
template<>
struct fex_gen_config<glGetInternalformati64v> {};
template<>
struct fex_gen_config<glGetInternalformativ> {};
template<>
struct fex_gen_config<glGetInternalformatSampleivNV> {};
template<>
struct fex_gen_config<glGetInvariantBooleanvEXT> {};
template<>
struct fex_gen_config<glGetInvariantFloatvEXT> {};
template<>
struct fex_gen_config<glGetInvariantIntegervEXT> {};
template<>
struct fex_gen_config<glGetLightfv> {};
template<>
struct fex_gen_config<glGetLightiv> {};
template<>
struct fex_gen_config<glGetLightxOES> {};
template<>
struct fex_gen_config<glGetListParameterfvSGIX> {};
template<>
struct fex_gen_config<glGetListParameterivSGIX> {};
template<>
struct fex_gen_config<glGetLocalConstantBooleanvEXT> {};
template<>
struct fex_gen_config<glGetLocalConstantFloatvEXT> {};
template<>
struct fex_gen_config<glGetLocalConstantIntegervEXT> {};
template<>
struct fex_gen_config<glGetMapAttribParameterfvNV> {};
template<>
struct fex_gen_config<glGetMapAttribParameterivNV> {};
template<>
struct fex_gen_config<glGetMapControlPointsNV> {};
template<>
struct fex_gen_config<glGetMapdv> {};
template<>
struct fex_gen_config<glGetMapfv> {};
template<>
struct fex_gen_config<glGetMapiv> {};
template<>
struct fex_gen_config<glGetMapParameterfvNV> {};
template<>
struct fex_gen_config<glGetMapParameterivNV> {};
template<>
struct fex_gen_config<glGetMapxvOES> {};
template<>
struct fex_gen_config<glGetMaterialfv> {};
template<>
struct fex_gen_config<glGetMaterialiv> {};
template<>
struct fex_gen_config<glGetMaterialxOES> {};
template<>
struct fex_gen_config<glGetMemoryObjectDetachedResourcesuivNV> {};
template<>
struct fex_gen_config<glGetMemoryObjectParameterivEXT> {};
template<>
struct fex_gen_config<glGetMinmaxEXT> {};
template<>
struct fex_gen_config<glGetMinmax> {};
template<>
struct fex_gen_config<glGetMinmaxParameterfvEXT> {};
template<>
struct fex_gen_config<glGetMinmaxParameterfv> {};
template<>
struct fex_gen_config<glGetMinmaxParameterivEXT> {};
template<>
struct fex_gen_config<glGetMinmaxParameteriv> {};
template<>
struct fex_gen_config<glGetMultisamplefv> {};
template<>
struct fex_gen_config<glGetMultisamplefvNV> {};
template<>
struct fex_gen_config<glGetMultiTexEnvfvEXT> {};
template<>
struct fex_gen_config<glGetMultiTexEnvivEXT> {};
template<>
struct fex_gen_config<glGetMultiTexGendvEXT> {};
template<>
struct fex_gen_config<glGetMultiTexGenfvEXT> {};
template<>
struct fex_gen_config<glGetMultiTexGenivEXT> {};
template<>
struct fex_gen_config<glGetMultiTexImageEXT> {};
template<>
struct fex_gen_config<glGetMultiTexLevelParameterfvEXT> {};
template<>
struct fex_gen_config<glGetMultiTexLevelParameterivEXT> {};
template<>
struct fex_gen_config<glGetMultiTexParameterfvEXT> {};
template<>
struct fex_gen_config<glGetMultiTexParameterIivEXT> {};
template<>
struct fex_gen_config<glGetMultiTexParameterIuivEXT> {};
template<>
struct fex_gen_config<glGetMultiTexParameterivEXT> {};
template<>
struct fex_gen_config<glGetNamedBufferParameteri64v> {};
template<>
struct fex_gen_config<glGetNamedBufferParameterivEXT> {};
template<>
struct fex_gen_config<glGetNamedBufferParameteriv> {};
template<>
struct fex_gen_config<glGetNamedBufferParameterui64vNV> {};
template<>
struct fex_gen_config<glGetNamedBufferPointerv> : fexgen::custom_host_impl {};
template<>
struct fex_gen_param<glGetNamedBufferPointerv, 2, void**> : fexgen::ptr_passthrough {};
template<>
struct fex_gen_config<glGetNamedBufferPointervEXT> : fexgen::custom_host_impl {};
template<>
struct fex_gen_param<glGetNamedBufferPointervEXT, 2, void**> : fexgen::ptr_passthrough {};
template<>
struct fex_gen_config<glGetNamedBufferSubDataEXT> {};
template<>
struct fex_gen_config<glGetNamedBufferSubData> {};
template<>
struct fex_gen_config<glGetNamedFramebufferAttachmentParameterivEXT> {};
template<>
struct fex_gen_config<glGetNamedFramebufferAttachmentParameteriv> {};
template<>
struct fex_gen_config<glGetNamedFramebufferParameterfvAMD> {};
template<>
struct fex_gen_config<glGetNamedFramebufferParameterivEXT> {};
template<>
struct fex_gen_config<glGetNamedFramebufferParameteriv> {};
template<>
struct fex_gen_config<glGetNamedProgramivEXT> {};
template<>
struct fex_gen_config<glGetNamedProgramLocalParameterdvEXT> {};
template<>
struct fex_gen_config<glGetNamedProgramLocalParameterfvEXT> {};
template<>
struct fex_gen_config<glGetNamedProgramLocalParameterIivEXT> {};
template<>
struct fex_gen_config<glGetNamedProgramLocalParameterIuivEXT> {};
template<>
struct fex_gen_config<glGetNamedProgramStringEXT> {};
template<>
struct fex_gen_config<glGetNamedRenderbufferParameterivEXT> {};
template<>
struct fex_gen_config<glGetNamedRenderbufferParameteriv> {};
template<>
struct fex_gen_config<glGetNamedStringARB> {};
template<>
struct fex_gen_config<glGetNamedStringivARB> {};
template<>
struct fex_gen_config<glGetnCompressedTexImageARB> {};
template<>
struct fex_gen_config<glGetnCompressedTexImage> {};
template<>
struct fex_gen_config<glGetNextPerfQueryIdINTEL> {};
template<>
struct fex_gen_config<glGetnTexImageARB> {};
template<>
struct fex_gen_config<glGetnTexImage> {};
template<>
struct fex_gen_config<glGetnUniformdvARB> {};
template<>
struct fex_gen_config<glGetnUniformdv> {};
template<>
struct fex_gen_config<glGetnUniformfvARB> {};
template<>
struct fex_gen_config<glGetnUniformfv> {};
template<>
struct fex_gen_config<glGetnUniformi64vARB> {};
template<>
struct fex_gen_config<glGetnUniformivARB> {};
template<>
struct fex_gen_config<glGetnUniformiv> {};
template<>
struct fex_gen_config<glGetnUniformui64vARB> {};
template<>
struct fex_gen_config<glGetnUniformuivARB> {};
template<>
struct fex_gen_config<glGetnUniformuiv> {};
template<>
struct fex_gen_config<glGetObjectBufferfvATI> {};
template<>
struct fex_gen_config<glGetObjectBufferivATI> {};
template<>
struct fex_gen_config<glGetObjectLabelEXT> {};
template<>
struct fex_gen_config<glGetObjectLabel> {};
template<>
struct fex_gen_config<glGetObjectParameterfvARB> {};
template<>
struct fex_gen_config<glGetObjectParameterivAPPLE> {};
template<>
struct fex_gen_config<glGetObjectParameterivARB> {};
template<>
struct fex_gen_config<glGetObjectPtrLabel> {};
template<>
struct fex_gen_config<glGetOcclusionQueryivNV> {};
template<>
struct fex_gen_config<glGetOcclusionQueryuivNV> {};
template<>
struct fex_gen_config<glGetPathCommandsNV> {};
template<>
struct fex_gen_config<glGetPathCoordsNV> {};
template<>
struct fex_gen_config<glGetPathDashArrayNV> {};
template<>
struct fex_gen_config<glGetPathMetricRangeNV> {};
template<>
struct fex_gen_config<glGetPathMetricsNV> {};
template<>
struct fex_gen_config<glGetPathParameterfvNV> {};
template<>
struct fex_gen_config<glGetPathParameterivNV> {};
template<>
struct fex_gen_config<glGetPathSpacingNV> {};
template<>
struct fex_gen_config<glGetPerfCounterInfoINTEL> {};
template<>
struct fex_gen_config<glGetPerfMonitorCounterDataAMD> {};
template<>
struct fex_gen_config<glGetPerfMonitorCounterInfoAMD> {};
template<>
struct fex_gen_config<glGetPerfMonitorCountersAMD> {};
template<>
struct fex_gen_config<glGetPerfMonitorCounterStringAMD> {};
template<>
struct fex_gen_config<glGetPerfMonitorGroupsAMD> {};
template<>
struct fex_gen_config<glGetPerfMonitorGroupStringAMD> {};
#ifndef IS_32BIT_THUNK
// TODO: 32-bit support
template<>
struct fex_gen_config<glGetPerfQueryDataINTEL> {};
#endif
template<>
struct fex_gen_config<glGetPerfQueryIdByNameINTEL> {};
template<>
struct fex_gen_config<glGetPerfQueryInfoINTEL> {};
template<>
struct fex_gen_config<glGetPixelMapfv> {};
template<>
struct fex_gen_config<glGetPixelMapuiv> {};
template<>
struct fex_gen_config<glGetPixelMapusv> {};
template<>
struct fex_gen_config<glGetPixelMapxv> {};
template<>
struct fex_gen_config<glGetPixelTexGenParameterfvSGIS> {};
template<>
struct fex_gen_config<glGetPixelTexGenParameterivSGIS> {};
template<>
struct fex_gen_config<glGetPixelTransformParameterfvEXT> {};
template<>
struct fex_gen_config<glGetPixelTransformParameterivEXT> {};
template<>
struct fex_gen_config<glGetPointerv> : fexgen::custom_host_impl {};
template<>
struct fex_gen_param<glGetPointerv, 1, void**> : fexgen::ptr_passthrough {};
template<>
struct fex_gen_config<glGetPointervEXT> : fexgen::custom_host_impl {};
template<>
struct fex_gen_param<glGetPointervEXT, 1, void**> : fexgen::ptr_passthrough {};
template<>
struct fex_gen_config<glGetPointeri_vEXT> : fexgen::custom_host_impl {};
template<>
struct fex_gen_param<glGetPointeri_vEXT, 2, void**> : fexgen::ptr_passthrough {};
template<>
struct fex_gen_config<glGetPointerIndexedvEXT> : fexgen::custom_host_impl {};
template<>
struct fex_gen_param<glGetPointerIndexedvEXT, 2, void**> : fexgen::ptr_passthrough {};
template<>
struct fex_gen_config<glGetPolygonStipple> {};
template<>
struct fex_gen_config<glGetProgramBinary> {};
template<>
struct fex_gen_config<glGetProgramEnvParameterdvARB> {};
template<>
struct fex_gen_config<glGetProgramEnvParameterfvARB> {};
template<>
struct fex_gen_config<glGetProgramEnvParameterIivNV> {};
template<>
struct fex_gen_config<glGetProgramEnvParameterIuivNV> {};
template<>
struct fex_gen_config<glGetProgramInfoLog> {};
template<>
struct fex_gen_config<glGetProgramInterfaceiv> {};
template<>
struct fex_gen_config<glGetProgramivARB> {};
template<>
struct fex_gen_config<glGetProgramiv> {};
template<>
struct fex_gen_config<glGetProgramivNV> {};
template<>
struct fex_gen_config<glGetProgramLocalParameterdvARB> {};
template<>
struct fex_gen_config<glGetProgramLocalParameterfvARB> {};
template<>
struct fex_gen_config<glGetProgramLocalParameterIivNV> {};
template<>
struct fex_gen_config<glGetProgramLocalParameterIuivNV> {};
template<>
struct fex_gen_config<glGetProgramNamedParameterdvNV> {};
template<>
struct fex_gen_config<glGetProgramNamedParameterfvNV> {};
template<>
struct fex_gen_config<glGetProgramParameterdvNV> {};
template<>
struct fex_gen_config<glGetProgramParameterfvNV> {};
template<>
struct fex_gen_config<glGetProgramPipelineInfoLog> {};
template<>
struct fex_gen_config<glGetProgramPipelineiv> {};
template<>
struct fex_gen_config<glGetProgramResourcefvNV> {};
template<>
struct fex_gen_config<glGetProgramResourceiv> {};
template<>
struct fex_gen_config<glGetProgramResourceName> {};
template<>
struct fex_gen_config<glGetProgramStageiv> {};
template<>
struct fex_gen_config<glGetProgramStringARB> {};
template<>
struct fex_gen_config<glGetProgramStringNV> {};
template<>
struct fex_gen_config<glGetProgramSubroutineParameteruivNV> {};
template<>
struct fex_gen_config<glGetQueryBufferObjecti64v> {};
template<>
struct fex_gen_config<glGetQueryBufferObjectiv> {};
template<>
struct fex_gen_config<glGetQueryBufferObjectui64v> {};
template<>
struct fex_gen_config<glGetQueryBufferObjectuiv> {};
template<>
struct fex_gen_config<glGetQueryIndexediv> {};
template<>
struct fex_gen_config<glGetQueryivARB> {};
template<>
struct fex_gen_config<glGetQueryiv> {};
template<>
struct fex_gen_config<glGetQueryObjecti64vEXT> {};
template<>
struct fex_gen_config<glGetQueryObjecti64v> {};
template<>
struct fex_gen_config<glGetQueryObjectivARB> {};
template<>
struct fex_gen_config<glGetQueryObjectiv> {};
template<>
struct fex_gen_config<glGetQueryObjectui64vEXT> {};
template<>
struct fex_gen_config<glGetQueryObjectui64v> {};
template<>
struct fex_gen_config<glGetQueryObjectuivARB> {};
template<>
struct fex_gen_config<glGetQueryObjectuiv> {};
template<>
struct fex_gen_config<glGetRenderbufferParameterivEXT> {};
template<>
struct fex_gen_config<glGetRenderbufferParameteriv> {};
template<>
struct fex_gen_config<glGetSamplerParameterfv> {};
template<>
struct fex_gen_config<glGetSamplerParameterIiv> {};
template<>
struct fex_gen_config<glGetSamplerParameterIuiv> {};
template<>
struct fex_gen_config<glGetSamplerParameteriv> {};
template<>
struct fex_gen_config<glGetSemaphoreParameterui64vEXT> {};
template<>
struct fex_gen_config<glGetSeparableFilterEXT> {};
template<>
struct fex_gen_config<glGetSeparableFilter> {};
template<>
struct fex_gen_config<glGetShaderInfoLog> {};
template<>
struct fex_gen_config<glGetShaderiv> {};
template<>
struct fex_gen_config<glGetShaderPrecisionFormat> {};
template<>
struct fex_gen_config<glGetShaderSourceARB> {};
template<>
struct fex_gen_config<glGetShaderSource> {};
template<>
struct fex_gen_config<glGetShadingRateImagePaletteNV> {};
template<>
struct fex_gen_config<glGetShadingRateSampleLocationivNV> {};
template<>
struct fex_gen_config<glGetSharpenTexFuncSGIS> {};
template<>
struct fex_gen_config<glGetSynciv> {};
template<>
struct fex_gen_config<glGetTexBumpParameterfvATI> {};
template<>
struct fex_gen_config<glGetTexBumpParameterivATI> {};
template<>
struct fex_gen_config<glGetTexEnvfv> {};
template<>
struct fex_gen_config<glGetTexEnviv> {};
template<>
struct fex_gen_config<glGetTexEnvxvOES> {};
template<>
struct fex_gen_config<glGetTexFilterFuncSGIS> {};
template<>
struct fex_gen_config<glGetTexGendv> {};
template<>
struct fex_gen_config<glGetTexGenfv> {};
template<>
struct fex_gen_config<glGetTexGeniv> {};
template<>
struct fex_gen_config<glGetTexGenxvOES> {};
template<>
struct fex_gen_config<glGetTexImage> {};
template<>
struct fex_gen_config<glGetTexLevelParameterfv> {};
template<>
struct fex_gen_config<glGetTexLevelParameteriv> {};
template<>
struct fex_gen_config<glGetTexLevelParameterxvOES> {};
template<>
struct fex_gen_config<glGetTexParameterfv> {};
template<>
struct fex_gen_config<glGetTexParameterIivEXT> {};
template<>
struct fex_gen_config<glGetTexParameterIiv> {};
template<>
struct fex_gen_config<glGetTexParameterIuivEXT> {};
template<>
struct fex_gen_config<glGetTexParameterIuiv> {};
template<>
struct fex_gen_config<glGetTexParameteriv> {};
#ifndef IS_32BIT_THUNK
// TODO: 32-bit support
template<>
struct fex_gen_config<glGetTexParameterPointervAPPLE> {};
template<>
struct fex_gen_param<glGetTexParameterPointervAPPLE, 2, void**> : fexgen::assume_compatible_data_layout {};
#endif
template<>
struct fex_gen_config<glGetTexParameterxvOES> {};
template<>
struct fex_gen_config<glGetTextureImageEXT> {};
template<>
struct fex_gen_config<glGetTextureImage> {};
template<>
struct fex_gen_config<glGetTextureLevelParameterfvEXT> {};
template<>
struct fex_gen_config<glGetTextureLevelParameterfv> {};
template<>
struct fex_gen_config<glGetTextureLevelParameterivEXT> {};
template<>
struct fex_gen_config<glGetTextureLevelParameteriv> {};
template<>
struct fex_gen_config<glGetTextureParameterfvEXT> {};
template<>
struct fex_gen_config<glGetTextureParameterfv> {};
template<>
struct fex_gen_config<glGetTextureParameterIivEXT> {};
template<>
struct fex_gen_config<glGetTextureParameterIiv> {};
template<>
struct fex_gen_config<glGetTextureParameterIuivEXT> {};
template<>
struct fex_gen_config<glGetTextureParameterIuiv> {};
template<>
struct fex_gen_config<glGetTextureParameterivEXT> {};
template<>
struct fex_gen_config<glGetTextureParameteriv> {};
template<>
struct fex_gen_config<glGetTextureSubImage> {};
template<>
struct fex_gen_config<glGetTrackMatrixivNV> {};
template<>
struct fex_gen_config<glGetTransformFeedbacki64_v> {};
template<>
struct fex_gen_config<glGetTransformFeedbackiv> {};
template<>
struct fex_gen_config<glGetTransformFeedbacki_v> {};
template<>
struct fex_gen_config<glGetTransformFeedbackVaryingEXT> {};
template<>
struct fex_gen_config<glGetTransformFeedbackVarying> {};
template<>
struct fex_gen_config<glGetTransformFeedbackVaryingNV> {};
template<>
struct fex_gen_config<glGetUniformdv> {};
template<>
struct fex_gen_config<glGetUniformfvARB> {};
template<>
struct fex_gen_config<glGetUniformfv> {};
template<>
struct fex_gen_config<glGetUniformi64vARB> {};
template<>
struct fex_gen_config<glGetUniformi64vNV> {};
#ifndef IS_32BIT_THUNK
template<>
struct fex_gen_config<glGetUniformIndices> {};
template<>
struct fex_gen_param<glGetUniformIndices, 2, const char* const*> : fexgen::assume_compatible_data_layout {};
#else
template<>
struct fex_gen_config<glGetUniformIndices> : fexgen::custom_host_impl {};
template<>
struct fex_gen_param<glGetUniformIndices, 2, const char* const*> : fexgen::ptr_passthrough {};
#endif
template<>
struct fex_gen_config<glGetUniformivARB> {};
template<>
struct fex_gen_config<glGetUniformiv> {};
template<>
struct fex_gen_config<glGetUniformSubroutineuiv> {};
template<>
struct fex_gen_config<glGetUniformui64vARB> {};
template<>
struct fex_gen_config<glGetUniformui64vNV> {};
template<>
struct fex_gen_config<glGetUniformuivEXT> {};
template<>
struct fex_gen_config<glGetUniformuiv> {};
template<>
struct fex_gen_config<glGetUnsignedBytei_vEXT> {};
template<>
struct fex_gen_config<glGetUnsignedBytevEXT> {};
template<>
struct fex_gen_config<glGetVariantArrayObjectfvATI> {};
template<>
struct fex_gen_config<glGetVariantArrayObjectivATI> {};
template<>
struct fex_gen_config<glGetVariantBooleanvEXT> {};
template<>
struct fex_gen_config<glGetVariantFloatvEXT> {};
template<>
struct fex_gen_config<glGetVariantIntegervEXT> {};
template<>
struct fex_gen_config<glGetVariantPointervEXT> : fexgen::custom_host_impl {};
template<>
struct fex_gen_param<glGetVariantPointervEXT, 2, void**> : fexgen::ptr_passthrough {};
template<>
struct fex_gen_config<glGetVertexArrayIndexed64iv> {};
template<>
struct fex_gen_config<glGetVertexArrayIndexediv> {};
template<>
struct fex_gen_config<glGetVertexArrayIntegeri_vEXT> {};
template<>
struct fex_gen_config<glGetVertexArrayIntegervEXT> {};
template<>
struct fex_gen_config<glGetVertexArrayiv> {};
template<>
struct fex_gen_config<glGetVertexArrayPointeri_vEXT> : fexgen::custom_host_impl {};
template<>
struct fex_gen_param<glGetVertexArrayPointeri_vEXT, 3, void**> : fexgen::ptr_passthrough {};
template<>
struct fex_gen_config<glGetVertexArrayPointervEXT> : fexgen::custom_host_impl {};
template<>
struct fex_gen_param<glGetVertexArrayPointervEXT, 2, void**> : fexgen::ptr_passthrough {};
template<>
struct fex_gen_config<glGetVertexAttribArrayObjectfvATI> {};
template<>
struct fex_gen_config<glGetVertexAttribArrayObjectivATI> {};
template<>
struct fex_gen_config<glGetVertexAttribdvARB> {};
template<>
struct fex_gen_config<glGetVertexAttribdv> {};
template<>
struct fex_gen_config<glGetVertexAttribdvNV> {};
template<>
struct fex_gen_config<glGetVertexAttribfvARB> {};
template<>
struct fex_gen_config<glGetVertexAttribfv> {};
template<>
struct fex_gen_config<glGetVertexAttribfvNV> {};
template<>
struct fex_gen_config<glGetVertexAttribIivEXT> {};
template<>
struct fex_gen_config<glGetVertexAttribIiv> {};
template<>
struct fex_gen_config<glGetVertexAttribIuivEXT> {};
template<>
struct fex_gen_config<glGetVertexAttribIuiv> {};
template<>
struct fex_gen_config<glGetVertexAttribivARB> {};
template<>
struct fex_gen_config<glGetVertexAttribiv> {};
template<>
struct fex_gen_config<glGetVertexAttribivNV> {};
template<>
struct fex_gen_config<glGetVertexAttribLdvEXT> {};
template<>
struct fex_gen_config<glGetVertexAttribLdv> {};
template<>
struct fex_gen_config<glGetVertexAttribLi64vNV> {};
template<>
struct fex_gen_config<glGetVertexAttribLui64vARB> {};
template<>
struct fex_gen_config<glGetVertexAttribLui64vNV> {};
template<>
struct fex_gen_config<glGetVertexAttribPointervARB> : fexgen::custom_host_impl {};
template<>
struct fex_gen_param<glGetVertexAttribPointervARB, 2, void**> : fexgen::ptr_passthrough {};
template<>
struct fex_gen_config<glGetVertexAttribPointerv> : fexgen::custom_host_impl {};
template<>
struct fex_gen_param<glGetVertexAttribPointerv, 2, void**> : fexgen::ptr_passthrough {};
template<>
struct fex_gen_config<glGetVertexAttribPointervNV> : fexgen::custom_host_impl {};
template<>
struct fex_gen_param<glGetVertexAttribPointervNV, 2, void**> : fexgen::ptr_passthrough {};
template<>
struct fex_gen_config<glGetVideoCaptureivNV> {};
template<>
struct fex_gen_config<glGetVideoCaptureStreamdvNV> {};
template<>
struct fex_gen_config<glGetVideoCaptureStreamfvNV> {};
template<>
struct fex_gen_config<glGetVideoCaptureStreamivNV> {};
template<>
struct fex_gen_config<glGetVideoi64vNV> {};
template<>
struct fex_gen_config<glGetVideoivNV> {};
template<>
struct fex_gen_config<glGetVideoui64vNV> {};
template<>
struct fex_gen_config<glGetVideouivNV> {};
template<>
struct fex_gen_config<glGlobalAlphaFactorbSUN> {};
template<>
struct fex_gen_config<glGlobalAlphaFactordSUN> {};
template<>
struct fex_gen_config<glGlobalAlphaFactorfSUN> {};
template<>
struct fex_gen_config<glGlobalAlphaFactoriSUN> {};
template<>
struct fex_gen_config<glGlobalAlphaFactorsSUN> {};
template<>
struct fex_gen_config<glGlobalAlphaFactorubSUN> {};
template<>
struct fex_gen_config<glGlobalAlphaFactoruiSUN> {};
template<>
struct fex_gen_config<glGlobalAlphaFactorusSUN> {};
template<>
struct fex_gen_config<glHint> {};
template<>
struct fex_gen_config<glHintPGI> {};
template<>
struct fex_gen_config<glHistogramEXT> {};
template<>
struct fex_gen_config<glHistogram> {};
template<>
struct fex_gen_config<glIglooInterfaceSGIX> {};
template<>
struct fex_gen_config<glImageTransformParameterfHP> {};
template<>
struct fex_gen_config<glImageTransformParameterfvHP> {};
template<>
struct fex_gen_config<glImageTransformParameteriHP> {};
template<>
struct fex_gen_config<glImageTransformParameterivHP> {};
template<>
struct fex_gen_config<glImportMemoryFdEXT> {};
template<>
struct fex_gen_config<glImportMemoryWin32HandleEXT> {};
template<>
struct fex_gen_config<glImportMemoryWin32NameEXT> {};
template<>
struct fex_gen_config<glImportSemaphoreFdEXT> {};
template<>
struct fex_gen_config<glImportSemaphoreWin32HandleEXT> {};
template<>
struct fex_gen_config<glImportSemaphoreWin32NameEXT> {};
template<>
struct fex_gen_config<glIndexd> {};
template<>
struct fex_gen_config<glIndexdv> {};
template<>
struct fex_gen_config<glIndexf> {};
template<>
struct fex_gen_config<glIndexFormatNV> {};
template<>
struct fex_gen_config<glIndexFuncEXT> {};
template<>
struct fex_gen_config<glIndexfv> {};
template<>
struct fex_gen_config<glIndexi> {};
template<>
struct fex_gen_config<glIndexiv> {};
template<>
struct fex_gen_config<glIndexMask> {};
template<>
struct fex_gen_config<glIndexMaterialEXT> {};
template<>
struct fex_gen_config<glIndexPointerEXT> {};
template<>
struct fex_gen_config<glIndexPointer> {};
#ifndef IS_32BIT_THUNK
// TODO: 32-bit support
template<>
struct fex_gen_config<glIndexPointerListIBM> {};
template<>
struct fex_gen_param<glIndexPointerListIBM, 2, const void**> : fexgen::assume_compatible_data_layout {};
#endif
template<>
struct fex_gen_config<glIndexs> {};
template<>
struct fex_gen_config<glIndexsv> {};
template<>
struct fex_gen_config<glIndexub> {};
template<>
struct fex_gen_config<glIndexubv> {};
template<>
struct fex_gen_config<glIndexxOES> {};
template<>
struct fex_gen_config<glIndexxvOES> {};
template<>
struct fex_gen_config<glInitNames> {};
template<>
struct fex_gen_config<glInsertComponentEXT> {};
template<>
struct fex_gen_config<glInsertEventMarkerEXT> {};
template<>
struct fex_gen_config<glInstrumentsBufferSGIX> {};
template<>
struct fex_gen_config<glInterleavedArrays> {};
template<>
struct fex_gen_config<glInterpolatePathsNV> {};
template<>
struct fex_gen_config<glInvalidateBufferData> {};
template<>
struct fex_gen_config<glInvalidateBufferSubData> {};
template<>
struct fex_gen_config<glInvalidateFramebuffer> {};
template<>
struct fex_gen_config<glInvalidateNamedFramebufferData> {};
template<>
struct fex_gen_config<glInvalidateNamedFramebufferSubData> {};
template<>
struct fex_gen_config<glInvalidateSubFramebuffer> {};
template<>
struct fex_gen_config<glInvalidateTexImage> {};
template<>
struct fex_gen_config<glInvalidateTexSubImage> {};
template<>
struct fex_gen_config<glLabelObjectEXT> {};
template<>
struct fex_gen_config<glLGPUCopyImageSubDataNVX> {};
template<>
struct fex_gen_config<glLGPUInterlockNVX> {};
template<>
struct fex_gen_config<glLGPUNamedBufferSubDataNVX> {};
template<>
struct fex_gen_config<glLightEnviSGIX> {};
template<>
struct fex_gen_config<glLightf> {};
template<>
struct fex_gen_config<glLightfv> {};
template<>
struct fex_gen_config<glLighti> {};
template<>
struct fex_gen_config<glLightiv> {};
template<>
struct fex_gen_config<glLightModelf> {};
template<>
struct fex_gen_config<glLightModelfv> {};
template<>
struct fex_gen_config<glLightModeli> {};
template<>
struct fex_gen_config<glLightModeliv> {};
template<>
struct fex_gen_config<glLightModelxOES> {};
template<>
struct fex_gen_config<glLightModelxvOES> {};
template<>
struct fex_gen_config<glLightxOES> {};
template<>
struct fex_gen_config<glLightxvOES> {};
template<>
struct fex_gen_config<glLineStipple> {};
template<>
struct fex_gen_config<glLineWidth> {};
template<>
struct fex_gen_config<glLineWidthxOES> {};
template<>
struct fex_gen_config<glLinkProgramARB> {};
template<>
struct fex_gen_config<glLinkProgram> {};
template<>
struct fex_gen_config<glListBase> {};
#ifndef IS_32BIT_THUNK
// TODO: 32-bit support
template<>
struct fex_gen_config<glListDrawCommandsStatesClientNV> {};
template<>
struct fex_gen_param<glListDrawCommandsStatesClientNV, 2, const void**> : fexgen::assume_compatible_data_layout {};
#endif
template<>
struct fex_gen_config<glListParameterfSGIX> {};
template<>
struct fex_gen_config<glListParameterfvSGIX> {};
template<>
struct fex_gen_config<glListParameteriSGIX> {};
template<>
struct fex_gen_config<glListParameterivSGIX> {};
template<>
struct fex_gen_config<glLoadIdentity> {};
template<>
struct fex_gen_config<glLoadIdentityDeformationMapSGIX> {};
template<>
struct fex_gen_config<glLoadMatrixd> {};
template<>
struct fex_gen_config<glLoadMatrixf> {};
template<>
struct fex_gen_config<glLoadMatrixxOES> {};
template<>
struct fex_gen_config<glLoadName> {};
template<>
struct fex_gen_config<glLoadProgramNV> {};
template<>
struct fex_gen_config<glLoadTransposeMatrixdARB> {};
template<>
struct fex_gen_config<glLoadTransposeMatrixd> {};
template<>
struct fex_gen_config<glLoadTransposeMatrixfARB> {};
template<>
struct fex_gen_config<glLoadTransposeMatrixf> {};
template<>
struct fex_gen_config<glLoadTransposeMatrixxOES> {};
template<>
struct fex_gen_config<glLockArraysEXT> {};
template<>
struct fex_gen_config<glLogicOp> {};
template<>
struct fex_gen_config<glMakeBufferNonResidentNV> {};
template<>
struct fex_gen_config<glMakeBufferResidentNV> {};
template<>
struct fex_gen_config<glMakeImageHandleNonResidentARB> {};
template<>
struct fex_gen_config<glMakeImageHandleNonResidentNV> {};
template<>
struct fex_gen_config<glMakeImageHandleResidentARB> {};
template<>
struct fex_gen_config<glMakeImageHandleResidentNV> {};
template<>
struct fex_gen_config<glMakeNamedBufferNonResidentNV> {};
template<>
struct fex_gen_config<glMakeNamedBufferResidentNV> {};
template<>
struct fex_gen_config<glMakeTextureHandleNonResidentARB> {};
template<>
struct fex_gen_config<glMakeTextureHandleNonResidentNV> {};
template<>
struct fex_gen_config<glMakeTextureHandleResidentARB> {};
template<>
struct fex_gen_config<glMakeTextureHandleResidentNV> {};
template<>
struct fex_gen_config<glMap1d> {};
template<>
struct fex_gen_config<glMap1f> {};
template<>
struct fex_gen_config<glMap1xOES> {};
template<>
struct fex_gen_config<glMap2d> {};
template<>
struct fex_gen_config<glMap2f> {};
template<>
struct fex_gen_config<glMap2xOES> {};
template<>
struct fex_gen_config<glMapBufferARB> {};
template<>
struct fex_gen_config<glMapBuffer> {};
template<>
struct fex_gen_config<glMapBufferRange> {};
template<>
struct fex_gen_config<glMapControlPointsNV> {};
template<>
struct fex_gen_config<glMapGrid1d> {};
template<>
struct fex_gen_config<glMapGrid1f> {};
template<>
struct fex_gen_config<glMapGrid1xOES> {};
template<>
struct fex_gen_config<glMapGrid2d> {};
template<>
struct fex_gen_config<glMapGrid2f> {};
template<>
struct fex_gen_config<glMapGrid2xOES> {};
template<>
struct fex_gen_config<glMapNamedBufferEXT> {};
template<>
struct fex_gen_config<glMapNamedBuffer> {};
template<>
struct fex_gen_config<glMapNamedBufferRangeEXT> {};
template<>
struct fex_gen_config<glMapNamedBufferRange> {};
template<>
struct fex_gen_config<glMapObjectBufferATI> {};
template<>
struct fex_gen_config<glMapParameterfvNV> {};
template<>
struct fex_gen_config<glMapParameterivNV> {};
template<>
struct fex_gen_config<glMapTexture2DINTEL> {};
template<>
struct fex_gen_config<glMapVertexAttrib1dAPPLE> {};
template<>
struct fex_gen_config<glMapVertexAttrib1fAPPLE> {};
template<>
struct fex_gen_config<glMapVertexAttrib2dAPPLE> {};
template<>
struct fex_gen_config<glMapVertexAttrib2fAPPLE> {};
template<>
struct fex_gen_config<glMaterialf> {};
template<>
struct fex_gen_config<glMaterialfv> {};
template<>
struct fex_gen_config<glMateriali> {};
template<>
struct fex_gen_config<glMaterialiv> {};
template<>
struct fex_gen_config<glMaterialxOES> {};
template<>
struct fex_gen_config<glMaterialxvOES> {};
template<>
struct fex_gen_config<glMatrixFrustumEXT> {};
template<>
struct fex_gen_config<glMatrixIndexPointerARB> {};
template<>
struct fex_gen_config<glMatrixIndexubvARB> {};
template<>
struct fex_gen_config<glMatrixIndexuivARB> {};
template<>
struct fex_gen_config<glMatrixIndexusvARB> {};
template<>
struct fex_gen_config<glMatrixLoad3x2fNV> {};
template<>
struct fex_gen_config<glMatrixLoad3x3fNV> {};
template<>
struct fex_gen_config<glMatrixLoaddEXT> {};
template<>
struct fex_gen_config<glMatrixLoadfEXT> {};
template<>
struct fex_gen_config<glMatrixLoadIdentityEXT> {};
template<>
struct fex_gen_config<glMatrixLoadTranspose3x3fNV> {};
template<>
struct fex_gen_config<glMatrixLoadTransposedEXT> {};
template<>
struct fex_gen_config<glMatrixLoadTransposefEXT> {};
template<>
struct fex_gen_config<glMatrixMode> {};
template<>
struct fex_gen_config<glMatrixMult3x2fNV> {};
template<>
struct fex_gen_config<glMatrixMult3x3fNV> {};
template<>
struct fex_gen_config<glMatrixMultdEXT> {};
template<>
struct fex_gen_config<glMatrixMultfEXT> {};
template<>
struct fex_gen_config<glMatrixMultTranspose3x3fNV> {};
template<>
struct fex_gen_config<glMatrixMultTransposedEXT> {};
template<>
struct fex_gen_config<glMatrixMultTransposefEXT> {};
template<>
struct fex_gen_config<glMatrixOrthoEXT> {};
template<>
struct fex_gen_config<glMatrixPopEXT> {};
template<>
struct fex_gen_config<glMatrixPushEXT> {};
template<>
struct fex_gen_config<glMatrixRotatedEXT> {};
template<>
struct fex_gen_config<glMatrixRotatefEXT> {};
template<>
struct fex_gen_config<glMatrixScaledEXT> {};
template<>
struct fex_gen_config<glMatrixScalefEXT> {};
template<>
struct fex_gen_config<glMatrixTranslatedEXT> {};
template<>
struct fex_gen_config<glMatrixTranslatefEXT> {};
template<>
struct fex_gen_config<glMaxShaderCompilerThreadsARB> {};
template<>
struct fex_gen_config<glMaxShaderCompilerThreadsKHR> {};
template<>
struct fex_gen_config<glMemoryBarrierByRegion> {};
template<>
struct fex_gen_config<glMemoryBarrierEXT> {};
template<>
struct fex_gen_config<glMemoryBarrier> {};
template<>
struct fex_gen_config<glMemoryObjectParameterivEXT> {};
template<>
struct fex_gen_config<glMinmaxEXT> {};
template<>
struct fex_gen_config<glMinmax> {};
template<>
struct fex_gen_config<glMinSampleShadingARB> {};
template<>
struct fex_gen_config<glMinSampleShading> {};
template<>
struct fex_gen_config<glMulticastBarrierNV> {};
template<>
struct fex_gen_config<glMulticastBlitFramebufferNV> {};
template<>
struct fex_gen_config<glMulticastBufferSubDataNV> {};
template<>
struct fex_gen_config<glMulticastCopyBufferSubDataNV> {};
template<>
struct fex_gen_config<glMulticastCopyImageSubDataNV> {};
template<>
struct fex_gen_config<glMulticastFramebufferSampleLocationsfvNV> {};
template<>
struct fex_gen_config<glMulticastGetQueryObjecti64vNV> {};
template<>
struct fex_gen_config<glMulticastGetQueryObjectivNV> {};
template<>
struct fex_gen_config<glMulticastGetQueryObjectui64vNV> {};
template<>
struct fex_gen_config<glMulticastGetQueryObjectuivNV> {};
template<>
struct fex_gen_config<glMulticastScissorArrayvNVX> {};
template<>
struct fex_gen_config<glMulticastViewportArrayvNVX> {};
template<>
struct fex_gen_config<glMulticastViewportPositionWScaleNVX> {};
template<>
struct fex_gen_config<glMulticastWaitSyncNV> {};
template<>
struct fex_gen_config<glMultiDrawArraysEXT> {};
template<>
struct fex_gen_config<glMultiDrawArrays> {};
template<>
struct fex_gen_config<glMultiDrawArraysIndirectAMD> {};
template<>
struct fex_gen_config<glMultiDrawArraysIndirectBindlessCountNV> {};
template<>
struct fex_gen_config<glMultiDrawArraysIndirectBindlessNV> {};
template<>
struct fex_gen_config<glMultiDrawArraysIndirectCountARB> {};
template<>
struct fex_gen_config<glMultiDrawArraysIndirectCount> {};
template<>
struct fex_gen_config<glMultiDrawArraysIndirect> {};
template<>
struct fex_gen_config<glMultiDrawElementArrayAPPLE> {};
#ifndef IS_32BIT_THUNK
// Needs manual handling: The type of this is actually int8_t**, int16_t**, or int32_t**, depending on the "type" argument
// TODO: Do these values get copied or do they have to stay valid past the call?
template<>
struct fex_gen_config<glMultiDrawElementsBaseVertex> {};
template<>
struct fex_gen_param<glMultiDrawElementsBaseVertex, 3, const void* const*> : fexgen::assume_compatible_data_layout {};
template<>
struct fex_gen_config<glMultiDrawElementsEXT> {};
template<>
struct fex_gen_param<glMultiDrawElementsEXT, 3, const void* const*> : fexgen::assume_compatible_data_layout {};
template<>
struct fex_gen_config<glMultiDrawElements> {};
template<>
struct fex_gen_param<glMultiDrawElements, 3, const void* const*> : fexgen::assume_compatible_data_layout {};
#endif
template<>
struct fex_gen_config<glMultiDrawElementsIndirectAMD> {};
template<>
struct fex_gen_config<glMultiDrawElementsIndirectBindlessCountNV> {};
template<>
struct fex_gen_config<glMultiDrawElementsIndirectBindlessNV> {};
template<>
struct fex_gen_config<glMultiDrawElementsIndirectCountARB> {};
template<>
struct fex_gen_config<glMultiDrawElementsIndirectCount> {};
template<>
struct fex_gen_config<glMultiDrawElementsIndirect> {};
template<>
struct fex_gen_config<glMultiDrawMeshTasksIndirectCountNV> {};
template<>
struct fex_gen_config<glMultiDrawMeshTasksIndirectNV> {};
template<>
struct fex_gen_config<glMultiDrawRangeElementArrayAPPLE> {};
template<>
struct fex_gen_config<glMultiModeDrawArraysIBM> {};
#ifndef IS_32BIT_THUNK
// TODO: 32-bit support
template<>
struct fex_gen_config<glMultiModeDrawElementsIBM> {};
template<>
struct fex_gen_param<glMultiModeDrawElementsIBM, 3, const void* const*> : fexgen::assume_compatible_data_layout {};
#endif
template<>
struct fex_gen_config<glMultiTexBufferEXT> {};
template<>
struct fex_gen_config<glMultiTexCoord1bOES> {};
template<>
struct fex_gen_config<glMultiTexCoord1bvOES> {};
template<>
struct fex_gen_config<glMultiTexCoord1dARB> {};
template<>
struct fex_gen_config<glMultiTexCoord1d> {};
template<>
struct fex_gen_config<glMultiTexCoord1dvARB> {};
template<>
struct fex_gen_config<glMultiTexCoord1dv> {};
template<>
struct fex_gen_config<glMultiTexCoord1fARB> {};
template<>
struct fex_gen_config<glMultiTexCoord1f> {};
template<>
struct fex_gen_config<glMultiTexCoord1fvARB> {};
template<>
struct fex_gen_config<glMultiTexCoord1fv> {};
template<>
struct fex_gen_config<glMultiTexCoord1hNV> {};
template<>
struct fex_gen_config<glMultiTexCoord1hvNV> {};
template<>
struct fex_gen_config<glMultiTexCoord1iARB> {};
template<>
struct fex_gen_config<glMultiTexCoord1i> {};
template<>
struct fex_gen_config<glMultiTexCoord1ivARB> {};
template<>
struct fex_gen_config<glMultiTexCoord1iv> {};
template<>
struct fex_gen_config<glMultiTexCoord1sARB> {};
template<>
struct fex_gen_config<glMultiTexCoord1s> {};
template<>
struct fex_gen_config<glMultiTexCoord1svARB> {};
template<>
struct fex_gen_config<glMultiTexCoord1sv> {};
template<>
struct fex_gen_config<glMultiTexCoord1xOES> {};
template<>
struct fex_gen_config<glMultiTexCoord1xvOES> {};
template<>
struct fex_gen_config<glMultiTexCoord2bOES> {};
template<>
struct fex_gen_config<glMultiTexCoord2bvOES> {};
template<>
struct fex_gen_config<glMultiTexCoord2dARB> {};
template<>
struct fex_gen_config<glMultiTexCoord2d> {};
template<>
struct fex_gen_config<glMultiTexCoord2dvARB> {};
template<>
struct fex_gen_config<glMultiTexCoord2dv> {};
template<>
struct fex_gen_config<glMultiTexCoord2fARB> {};
template<>
struct fex_gen_config<glMultiTexCoord2f> {};
template<>
struct fex_gen_config<glMultiTexCoord2fvARB> {};
template<>
struct fex_gen_config<glMultiTexCoord2fv> {};
template<>
struct fex_gen_config<glMultiTexCoord2hNV> {};
template<>
struct fex_gen_config<glMultiTexCoord2hvNV> {};
template<>
struct fex_gen_config<glMultiTexCoord2iARB> {};
template<>
struct fex_gen_config<glMultiTexCoord2i> {};
template<>
struct fex_gen_config<glMultiTexCoord2ivARB> {};
template<>
struct fex_gen_config<glMultiTexCoord2iv> {};
template<>
struct fex_gen_config<glMultiTexCoord2sARB> {};
template<>
struct fex_gen_config<glMultiTexCoord2s> {};
template<>
struct fex_gen_config<glMultiTexCoord2svARB> {};
template<>
struct fex_gen_config<glMultiTexCoord2sv> {};
template<>
struct fex_gen_config<glMultiTexCoord2xOES> {};
template<>
struct fex_gen_config<glMultiTexCoord2xvOES> {};
template<>
struct fex_gen_config<glMultiTexCoord3bOES> {};
template<>
struct fex_gen_config<glMultiTexCoord3bvOES> {};
template<>
struct fex_gen_config<glMultiTexCoord3dARB> {};
template<>
struct fex_gen_config<glMultiTexCoord3d> {};
template<>
struct fex_gen_config<glMultiTexCoord3dvARB> {};
template<>
struct fex_gen_config<glMultiTexCoord3dv> {};
template<>
struct fex_gen_config<glMultiTexCoord3fARB> {};
template<>
struct fex_gen_config<glMultiTexCoord3f> {};
template<>
struct fex_gen_config<glMultiTexCoord3fvARB> {};
template<>
struct fex_gen_config<glMultiTexCoord3fv> {};
template<>
struct fex_gen_config<glMultiTexCoord3hNV> {};
template<>
struct fex_gen_config<glMultiTexCoord3hvNV> {};
template<>
struct fex_gen_config<glMultiTexCoord3iARB> {};
template<>
struct fex_gen_config<glMultiTexCoord3i> {};
template<>
struct fex_gen_config<glMultiTexCoord3ivARB> {};
template<>
struct fex_gen_config<glMultiTexCoord3iv> {};
template<>
struct fex_gen_config<glMultiTexCoord3sARB> {};
template<>
struct fex_gen_config<glMultiTexCoord3s> {};
template<>
struct fex_gen_config<glMultiTexCoord3svARB> {};
template<>
struct fex_gen_config<glMultiTexCoord3sv> {};
template<>
struct fex_gen_config<glMultiTexCoord3xOES> {};
template<>
struct fex_gen_config<glMultiTexCoord3xvOES> {};
template<>
struct fex_gen_config<glMultiTexCoord4bOES> {};
template<>
struct fex_gen_config<glMultiTexCoord4bvOES> {};
template<>
struct fex_gen_config<glMultiTexCoord4dARB> {};
template<>
struct fex_gen_config<glMultiTexCoord4d> {};
template<>
struct fex_gen_config<glMultiTexCoord4dvARB> {};
template<>
struct fex_gen_config<glMultiTexCoord4dv> {};
template<>
struct fex_gen_config<glMultiTexCoord4fARB> {};
template<>
struct fex_gen_config<glMultiTexCoord4f> {};
template<>
struct fex_gen_config<glMultiTexCoord4fvARB> {};
template<>
struct fex_gen_config<glMultiTexCoord4fv> {};
template<>
struct fex_gen_config<glMultiTexCoord4hNV> {};
template<>
struct fex_gen_config<glMultiTexCoord4hvNV> {};
template<>
struct fex_gen_config<glMultiTexCoord4iARB> {};
template<>
struct fex_gen_config<glMultiTexCoord4i> {};
template<>
struct fex_gen_config<glMultiTexCoord4ivARB> {};
template<>
struct fex_gen_config<glMultiTexCoord4iv> {};
template<>
struct fex_gen_config<glMultiTexCoord4sARB> {};
template<>
struct fex_gen_config<glMultiTexCoord4s> {};
template<>
struct fex_gen_config<glMultiTexCoord4svARB> {};
template<>
struct fex_gen_config<glMultiTexCoord4sv> {};
template<>
struct fex_gen_config<glMultiTexCoord4xOES> {};
template<>
struct fex_gen_config<glMultiTexCoord4xvOES> {};
template<>
struct fex_gen_config<glMultiTexCoordPointerEXT> {};
template<>
struct fex_gen_config<glMultiTexEnvfEXT> {};
template<>
struct fex_gen_config<glMultiTexEnvfvEXT> {};
template<>
struct fex_gen_config<glMultiTexEnviEXT> {};
template<>
struct fex_gen_config<glMultiTexEnvivEXT> {};
template<>
struct fex_gen_config<glMultiTexGendEXT> {};
template<>
struct fex_gen_config<glMultiTexGendvEXT> {};
template<>
struct fex_gen_config<glMultiTexGenfEXT> {};
template<>
struct fex_gen_config<glMultiTexGenfvEXT> {};
template<>
struct fex_gen_config<glMultiTexGeniEXT> {};
template<>
struct fex_gen_config<glMultiTexGenivEXT> {};
template<>
struct fex_gen_config<glMultiTexImage1DEXT> {};
template<>
struct fex_gen_config<glMultiTexImage2DEXT> {};
template<>
struct fex_gen_config<glMultiTexImage3DEXT> {};
template<>
struct fex_gen_config<glMultiTexParameterfEXT> {};
template<>
struct fex_gen_config<glMultiTexParameterfvEXT> {};
template<>
struct fex_gen_config<glMultiTexParameteriEXT> {};
template<>
struct fex_gen_config<glMultiTexParameterIivEXT> {};
template<>
struct fex_gen_config<glMultiTexParameterIuivEXT> {};
template<>
struct fex_gen_config<glMultiTexParameterivEXT> {};
template<>
struct fex_gen_config<glMultiTexRenderbufferEXT> {};
template<>
struct fex_gen_config<glMultiTexSubImage1DEXT> {};
template<>
struct fex_gen_config<glMultiTexSubImage2DEXT> {};
template<>
struct fex_gen_config<glMultiTexSubImage3DEXT> {};
template<>
struct fex_gen_config<glMultMatrixd> {};
template<>
struct fex_gen_config<glMultMatrixf> {};
template<>
struct fex_gen_config<glMultMatrixxOES> {};
template<>
struct fex_gen_config<glMultTransposeMatrixdARB> {};
template<>
struct fex_gen_config<glMultTransposeMatrixd> {};
template<>
struct fex_gen_config<glMultTransposeMatrixfARB> {};
template<>
struct fex_gen_config<glMultTransposeMatrixf> {};
template<>
struct fex_gen_config<glMultTransposeMatrixxOES> {};
template<>
struct fex_gen_config<glNamedBufferAttachMemoryNV> {};
template<>
struct fex_gen_config<glNamedBufferDataEXT> {};
template<>
struct fex_gen_config<glNamedBufferData> {};
template<>
struct fex_gen_config<glNamedBufferPageCommitmentARB> {};
template<>
struct fex_gen_config<glNamedBufferPageCommitmentEXT> {};
template<>
struct fex_gen_config<glNamedBufferStorageExternalEXT> {};
template<>
struct fex_gen_config<glNamedBufferStorageEXT> {};
template<>
struct fex_gen_config<glNamedBufferStorage> {};
template<>
struct fex_gen_config<glNamedBufferStorageMemEXT> {};
template<>
struct fex_gen_config<glNamedBufferSubDataEXT> {};
template<>
struct fex_gen_config<glNamedBufferSubData> {};
template<>
struct fex_gen_config<glNamedCopyBufferSubDataEXT> {};
template<>
struct fex_gen_config<glNamedFramebufferDrawBuffer> {};
template<>
struct fex_gen_config<glNamedFramebufferDrawBuffers> {};
template<>
struct fex_gen_config<glNamedFramebufferParameteriEXT> {};
template<>
struct fex_gen_config<glNamedFramebufferParameteri> {};
template<>
struct fex_gen_config<glNamedFramebufferReadBuffer> {};
template<>
struct fex_gen_config<glNamedFramebufferRenderbufferEXT> {};
template<>
struct fex_gen_config<glNamedFramebufferRenderbuffer> {};
template<>
struct fex_gen_config<glNamedFramebufferSampleLocationsfvARB> {};
template<>
struct fex_gen_config<glNamedFramebufferSampleLocationsfvNV> {};
template<>
struct fex_gen_config<glNamedFramebufferSamplePositionsfvAMD> {};
template<>
struct fex_gen_config<glNamedFramebufferTexture1DEXT> {};
template<>
struct fex_gen_config<glNamedFramebufferTexture2DEXT> {};
template<>
struct fex_gen_config<glNamedFramebufferTexture3DEXT> {};
template<>
struct fex_gen_config<glNamedFramebufferTextureEXT> {};
template<>
struct fex_gen_config<glNamedFramebufferTextureFaceEXT> {};
template<>
struct fex_gen_config<glNamedFramebufferTexture> {};
template<>
struct fex_gen_config<glNamedFramebufferTextureLayerEXT> {};
template<>
struct fex_gen_config<glNamedFramebufferTextureLayer> {};
template<>
struct fex_gen_config<glNamedProgramLocalParameter4dEXT> {};
template<>
struct fex_gen_config<glNamedProgramLocalParameter4dvEXT> {};
template<>
struct fex_gen_config<glNamedProgramLocalParameter4fEXT> {};
template<>
struct fex_gen_config<glNamedProgramLocalParameter4fvEXT> {};
template<>
struct fex_gen_config<glNamedProgramLocalParameterI4iEXT> {};
template<>
struct fex_gen_config<glNamedProgramLocalParameterI4ivEXT> {};
template<>
struct fex_gen_config<glNamedProgramLocalParameterI4uiEXT> {};
template<>
struct fex_gen_config<glNamedProgramLocalParameterI4uivEXT> {};
template<>
struct fex_gen_config<glNamedProgramLocalParameters4fvEXT> {};
template<>
struct fex_gen_config<glNamedProgramLocalParametersI4ivEXT> {};
template<>
struct fex_gen_config<glNamedProgramLocalParametersI4uivEXT> {};
template<>
struct fex_gen_config<glNamedProgramStringEXT> {};
template<>
struct fex_gen_config<glNamedRenderbufferStorageEXT> {};
template<>
struct fex_gen_config<glNamedRenderbufferStorage> {};
template<>
struct fex_gen_config<glNamedRenderbufferStorageMultisampleAdvancedAMD> {};
template<>
struct fex_gen_config<glNamedRenderbufferStorageMultisampleCoverageEXT> {};
template<>
struct fex_gen_config<glNamedRenderbufferStorageMultisampleEXT> {};
template<>
struct fex_gen_config<glNamedRenderbufferStorageMultisample> {};
template<>
struct fex_gen_config<glNamedStringARB> {};
template<>
struct fex_gen_config<glNewList> {};
template<>
struct fex_gen_config<glNormal3b> {};
template<>
struct fex_gen_config<glNormal3bv> {};
template<>
struct fex_gen_config<glNormal3d> {};
template<>
struct fex_gen_config<glNormal3dv> {};
template<>
struct fex_gen_config<glNormal3f> {};
template<>
struct fex_gen_config<glNormal3fv> {};
template<>
struct fex_gen_config<glNormal3fVertex3fSUN> {};
template<>
struct fex_gen_config<glNormal3fVertex3fvSUN> {};
template<>
struct fex_gen_config<glNormal3hNV> {};
template<>
struct fex_gen_config<glNormal3hvNV> {};
template<>
struct fex_gen_config<glNormal3i> {};
template<>
struct fex_gen_config<glNormal3iv> {};
template<>
struct fex_gen_config<glNormal3s> {};
template<>
struct fex_gen_config<glNormal3sv> {};
template<>
struct fex_gen_config<glNormal3xOES> {};
template<>
struct fex_gen_config<glNormal3xvOES> {};
template<>
struct fex_gen_config<glNormalFormatNV> {};
template<>
struct fex_gen_config<glNormalPointerEXT> {};
template<>
struct fex_gen_config<glNormalPointer> {};
#ifndef IS_32BIT_THUNK
// TODO: 32-bit support
template<>
struct fex_gen_config<glNormalPointerListIBM> {};
template<>
struct fex_gen_param<glNormalPointerListIBM, 2, const void**> : fexgen::assume_compatible_data_layout {};
template<>
struct fex_gen_config<glNormalPointervINTEL> {};
template<>
struct fex_gen_param<glNormalPointervINTEL, 1, const void**> : fexgen::assume_compatible_data_layout {};
#endif
template<>
struct fex_gen_config<glNormalStream3bATI> {};
template<>
struct fex_gen_config<glNormalStream3bvATI> {};
template<>
struct fex_gen_config<glNormalStream3dATI> {};
template<>
struct fex_gen_config<glNormalStream3dvATI> {};
template<>
struct fex_gen_config<glNormalStream3fATI> {};
template<>
struct fex_gen_config<glNormalStream3fvATI> {};
template<>
struct fex_gen_config<glNormalStream3iATI> {};
template<>
struct fex_gen_config<glNormalStream3ivATI> {};
template<>
struct fex_gen_config<glNormalStream3sATI> {};
template<>
struct fex_gen_config<glNormalStream3svATI> {};
template<>
struct fex_gen_config<glObjectLabel> {};
template<>
struct fex_gen_config<glObjectPtrLabel> {};
template<>
struct fex_gen_config<glOrthofOES> {};
template<>
struct fex_gen_config<glOrtho> {};
template<>
struct fex_gen_config<glOrthoxOES> {};
template<>
struct fex_gen_config<glPassTexCoordATI> {};
template<>
struct fex_gen_config<glPassThrough> {};
template<>
struct fex_gen_config<glPassThroughxOES> {};
template<>
struct fex_gen_config<glPatchParameterfv> {};
template<>
struct fex_gen_config<glPatchParameteri> {};
template<>
struct fex_gen_config<glPathCommandsNV> {};
template<>
struct fex_gen_config<glPathCoordsNV> {};
template<>
struct fex_gen_config<glPathCoverDepthFuncNV> {};
template<>
struct fex_gen_config<glPathDashArrayNV> {};
template<>
struct fex_gen_config<glPathGlyphRangeNV> {};
template<>
struct fex_gen_config<glPathGlyphsNV> {};
template<>
struct fex_gen_config<glPathParameterfNV> {};
template<>
struct fex_gen_config<glPathParameterfvNV> {};
template<>
struct fex_gen_config<glPathParameteriNV> {};
template<>
struct fex_gen_config<glPathParameterivNV> {};
template<>
struct fex_gen_config<glPathStencilDepthOffsetNV> {};
template<>
struct fex_gen_config<glPathStencilFuncNV> {};
template<>
struct fex_gen_config<glPathStringNV> {};
template<>
struct fex_gen_config<glPathSubCommandsNV> {};
template<>
struct fex_gen_config<glPathSubCoordsNV> {};
template<>
struct fex_gen_config<glPauseTransformFeedback> {};
template<>
struct fex_gen_config<glPauseTransformFeedbackNV> {};
template<>
struct fex_gen_config<glPixelDataRangeNV> {};
template<>
struct fex_gen_config<glPixelMapfv> {};
template<>
struct fex_gen_config<glPixelMapuiv> {};
template<>
struct fex_gen_config<glPixelMapusv> {};
template<>
struct fex_gen_config<glPixelMapx> {};
template<>
struct fex_gen_config<glPixelStoref> {};
template<>
struct fex_gen_config<glPixelStorei> {};
template<>
struct fex_gen_config<glPixelStorex> {};
template<>
struct fex_gen_config<glPixelTexGenParameterfSGIS> {};
template<>
struct fex_gen_config<glPixelTexGenParameterfvSGIS> {};
template<>
struct fex_gen_config<glPixelTexGenParameteriSGIS> {};
template<>
struct fex_gen_config<glPixelTexGenParameterivSGIS> {};
template<>
struct fex_gen_config<glPixelTexGenSGIX> {};
template<>
struct fex_gen_config<glPixelTransferf> {};
template<>
struct fex_gen_config<glPixelTransferi> {};
template<>
struct fex_gen_config<glPixelTransferxOES> {};
template<>
struct fex_gen_config<glPixelTransformParameterfEXT> {};
template<>
struct fex_gen_config<glPixelTransformParameterfvEXT> {};
template<>
struct fex_gen_config<glPixelTransformParameteriEXT> {};
template<>
struct fex_gen_config<glPixelTransformParameterivEXT> {};
template<>
struct fex_gen_config<glPixelZoom> {};
template<>
struct fex_gen_config<glPixelZoomxOES> {};
template<>
struct fex_gen_config<glPNTrianglesfATI> {};
template<>
struct fex_gen_config<glPNTrianglesiATI> {};
template<>
struct fex_gen_config<glPointParameterfARB> {};
template<>
struct fex_gen_config<glPointParameterfEXT> {};
template<>
struct fex_gen_config<glPointParameterf> {};
template<>
struct fex_gen_config<glPointParameterfSGIS> {};
template<>
struct fex_gen_config<glPointParameterfvARB> {};
template<>
struct fex_gen_config<glPointParameterfvEXT> {};
template<>
struct fex_gen_config<glPointParameterfv> {};
template<>
struct fex_gen_config<glPointParameterfvSGIS> {};
template<>
struct fex_gen_config<glPointParameteri> {};
template<>
struct fex_gen_config<glPointParameteriNV> {};
template<>
struct fex_gen_config<glPointParameteriv> {};
template<>
struct fex_gen_config<glPointParameterivNV> {};
template<>
struct fex_gen_config<glPointParameterxvOES> {};
template<>
struct fex_gen_config<glPointSize> {};
template<>
struct fex_gen_config<glPointSizexOES> {};
template<>
struct fex_gen_config<glPolygonMode> {};
template<>
struct fex_gen_config<glPolygonOffsetClampEXT> {};
template<>
struct fex_gen_config<glPolygonOffsetClamp> {};
template<>
struct fex_gen_config<glPolygonOffsetEXT> {};
template<>
struct fex_gen_config<glPolygonOffset> {};
template<>
struct fex_gen_config<glPolygonOffsetxOES> {};
template<>
struct fex_gen_config<glPolygonStipple> {};
template<>
struct fex_gen_config<glPopAttrib> {};
template<>
struct fex_gen_config<glPopClientAttrib> {};
template<>
struct fex_gen_config<glPopDebugGroup> {};
template<>
struct fex_gen_config<glPopGroupMarkerEXT> {};
template<>
struct fex_gen_config<glPopMatrix> {};
template<>
struct fex_gen_config<glPopName> {};
template<>
struct fex_gen_config<glPresentFrameDualFillNV> {};
template<>
struct fex_gen_config<glPresentFrameKeyedNV> {};
template<>
struct fex_gen_config<glPrimitiveBoundingBoxARB> {};
template<>
struct fex_gen_config<glPrimitiveRestartIndex> {};
template<>
struct fex_gen_config<glPrimitiveRestartIndexNV> {};
template<>
struct fex_gen_config<glPrimitiveRestartNV> {};
template<>
struct fex_gen_config<glPrioritizeTexturesEXT> {};
template<>
struct fex_gen_config<glPrioritizeTextures> {};
template<>
struct fex_gen_config<glPrioritizeTexturesxOES> {};
template<>
struct fex_gen_config<glProgramBinary> {};
template<>
struct fex_gen_config<glProgramBufferParametersfvNV> {};
template<>
struct fex_gen_config<glProgramBufferParametersIivNV> {};
template<>
struct fex_gen_config<glProgramBufferParametersIuivNV> {};
template<>
struct fex_gen_config<glProgramEnvParameter4dARB> {};
template<>
struct fex_gen_config<glProgramEnvParameter4dvARB> {};
template<>
struct fex_gen_config<glProgramEnvParameter4fARB> {};
template<>
struct fex_gen_config<glProgramEnvParameter4fvARB> {};
template<>
struct fex_gen_config<glProgramEnvParameterI4iNV> {};
template<>
struct fex_gen_config<glProgramEnvParameterI4ivNV> {};
template<>
struct fex_gen_config<glProgramEnvParameterI4uiNV> {};
template<>
struct fex_gen_config<glProgramEnvParameterI4uivNV> {};
template<>
struct fex_gen_config<glProgramEnvParameters4fvEXT> {};
template<>
struct fex_gen_config<glProgramEnvParametersI4ivNV> {};
template<>
struct fex_gen_config<glProgramEnvParametersI4uivNV> {};
template<>
struct fex_gen_config<glProgramLocalParameter4dARB> {};
template<>
struct fex_gen_config<glProgramLocalParameter4dvARB> {};
template<>
struct fex_gen_config<glProgramLocalParameter4fARB> {};
template<>
struct fex_gen_config<glProgramLocalParameter4fvARB> {};
template<>
struct fex_gen_config<glProgramLocalParameterI4iNV> {};
template<>
struct fex_gen_config<glProgramLocalParameterI4ivNV> {};
template<>
struct fex_gen_config<glProgramLocalParameterI4uiNV> {};
template<>
struct fex_gen_config<glProgramLocalParameterI4uivNV> {};
template<>
struct fex_gen_config<glProgramLocalParameters4fvEXT> {};
template<>
struct fex_gen_config<glProgramLocalParametersI4ivNV> {};
template<>
struct fex_gen_config<glProgramLocalParametersI4uivNV> {};
template<>
struct fex_gen_config<glProgramNamedParameter4dNV> {};
template<>
struct fex_gen_config<glProgramNamedParameter4dvNV> {};
template<>
struct fex_gen_config<glProgramNamedParameter4fNV> {};
template<>
struct fex_gen_config<glProgramNamedParameter4fvNV> {};
template<>
struct fex_gen_config<glProgramParameter4dNV> {};
template<>
struct fex_gen_config<glProgramParameter4dvNV> {};
template<>
struct fex_gen_config<glProgramParameter4fNV> {};
template<>
struct fex_gen_config<glProgramParameter4fvNV> {};
template<>
struct fex_gen_config<glProgramParameteriARB> {};
template<>
struct fex_gen_config<glProgramParameteriEXT> {};
template<>
struct fex_gen_config<glProgramParameteri> {};
template<>
struct fex_gen_config<glProgramParameters4dvNV> {};
template<>
struct fex_gen_config<glProgramParameters4fvNV> {};
template<>
struct fex_gen_config<glProgramPathFragmentInputGenNV> {};
template<>
struct fex_gen_config<glProgramStringARB> {};
template<>
struct fex_gen_config<glProgramSubroutineParametersuivNV> {};
template<>
struct fex_gen_config<glProgramUniform1dEXT> {};
template<>
struct fex_gen_config<glProgramUniform1d> {};
template<>
struct fex_gen_config<glProgramUniform1dvEXT> {};
template<>
struct fex_gen_config<glProgramUniform1dv> {};
template<>
struct fex_gen_config<glProgramUniform1fEXT> {};
template<>
struct fex_gen_config<glProgramUniform1f> {};
template<>
struct fex_gen_config<glProgramUniform1fvEXT> {};
template<>
struct fex_gen_config<glProgramUniform1fv> {};
template<>
struct fex_gen_config<glProgramUniform1i64ARB> {};
template<>
struct fex_gen_config<glProgramUniform1i64NV> {};
template<>
struct fex_gen_config<glProgramUniform1i64vARB> {};
template<>
struct fex_gen_config<glProgramUniform1i64vNV> {};
template<>
struct fex_gen_config<glProgramUniform1iEXT> {};
template<>
struct fex_gen_config<glProgramUniform1i> {};
template<>
struct fex_gen_config<glProgramUniform1ivEXT> {};
template<>
struct fex_gen_config<glProgramUniform1iv> {};
template<>
struct fex_gen_config<glProgramUniform1ui64ARB> {};
template<>
struct fex_gen_config<glProgramUniform1ui64NV> {};
template<>
struct fex_gen_config<glProgramUniform1ui64vARB> {};
template<>
struct fex_gen_config<glProgramUniform1ui64vNV> {};
template<>
struct fex_gen_config<glProgramUniform1uiEXT> {};
template<>
struct fex_gen_config<glProgramUniform1ui> {};
template<>
struct fex_gen_config<glProgramUniform1uivEXT> {};
template<>
struct fex_gen_config<glProgramUniform1uiv> {};
template<>
struct fex_gen_config<glProgramUniform2dEXT> {};
template<>
struct fex_gen_config<glProgramUniform2d> {};
template<>
struct fex_gen_config<glProgramUniform2dvEXT> {};
template<>
struct fex_gen_config<glProgramUniform2dv> {};
template<>
struct fex_gen_config<glProgramUniform2fEXT> {};
template<>
struct fex_gen_config<glProgramUniform2f> {};
template<>
struct fex_gen_config<glProgramUniform2fvEXT> {};
template<>
struct fex_gen_config<glProgramUniform2fv> {};
template<>
struct fex_gen_config<glProgramUniform2i64ARB> {};
template<>
struct fex_gen_config<glProgramUniform2i64NV> {};
template<>
struct fex_gen_config<glProgramUniform2i64vARB> {};
template<>
struct fex_gen_config<glProgramUniform2i64vNV> {};
template<>
struct fex_gen_config<glProgramUniform2iEXT> {};
template<>
struct fex_gen_config<glProgramUniform2i> {};
template<>
struct fex_gen_config<glProgramUniform2ivEXT> {};
template<>
struct fex_gen_config<glProgramUniform2iv> {};
template<>
struct fex_gen_config<glProgramUniform2ui64ARB> {};
template<>
struct fex_gen_config<glProgramUniform2ui64NV> {};
template<>
struct fex_gen_config<glProgramUniform2ui64vARB> {};
template<>
struct fex_gen_config<glProgramUniform2ui64vNV> {};
template<>
struct fex_gen_config<glProgramUniform2uiEXT> {};
template<>
struct fex_gen_config<glProgramUniform2ui> {};
template<>
struct fex_gen_config<glProgramUniform2uivEXT> {};
template<>
struct fex_gen_config<glProgramUniform2uiv> {};
template<>
struct fex_gen_config<glProgramUniform3dEXT> {};
template<>
struct fex_gen_config<glProgramUniform3d> {};
template<>
struct fex_gen_config<glProgramUniform3dvEXT> {};
template<>
struct fex_gen_config<glProgramUniform3dv> {};
template<>
struct fex_gen_config<glProgramUniform3fEXT> {};
template<>
struct fex_gen_config<glProgramUniform3f> {};
template<>
struct fex_gen_config<glProgramUniform3fvEXT> {};
template<>
struct fex_gen_config<glProgramUniform3fv> {};
template<>
struct fex_gen_config<glProgramUniform3i64ARB> {};
template<>
struct fex_gen_config<glProgramUniform3i64NV> {};
template<>
struct fex_gen_config<glProgramUniform3i64vARB> {};
template<>
struct fex_gen_config<glProgramUniform3i64vNV> {};
template<>
struct fex_gen_config<glProgramUniform3iEXT> {};
template<>
struct fex_gen_config<glProgramUniform3i> {};
template<>
struct fex_gen_config<glProgramUniform3ivEXT> {};
template<>
struct fex_gen_config<glProgramUniform3iv> {};
template<>
struct fex_gen_config<glProgramUniform3ui64ARB> {};
template<>
struct fex_gen_config<glProgramUniform3ui64NV> {};
template<>
struct fex_gen_config<glProgramUniform3ui64vARB> {};
template<>
struct fex_gen_config<glProgramUniform3ui64vNV> {};
template<>
struct fex_gen_config<glProgramUniform3uiEXT> {};
template<>
struct fex_gen_config<glProgramUniform3ui> {};
template<>
struct fex_gen_config<glProgramUniform3uivEXT> {};
template<>
struct fex_gen_config<glProgramUniform3uiv> {};
template<>
struct fex_gen_config<glProgramUniform4dEXT> {};
template<>
struct fex_gen_config<glProgramUniform4d> {};
template<>
struct fex_gen_config<glProgramUniform4dvEXT> {};
template<>
struct fex_gen_config<glProgramUniform4dv> {};
template<>
struct fex_gen_config<glProgramUniform4fEXT> {};
template<>
struct fex_gen_config<glProgramUniform4f> {};
template<>
struct fex_gen_config<glProgramUniform4fvEXT> {};
template<>
struct fex_gen_config<glProgramUniform4fv> {};
template<>
struct fex_gen_config<glProgramUniform4i64ARB> {};
template<>
struct fex_gen_config<glProgramUniform4i64NV> {};
template<>
struct fex_gen_config<glProgramUniform4i64vARB> {};
template<>
struct fex_gen_config<glProgramUniform4i64vNV> {};
template<>
struct fex_gen_config<glProgramUniform4iEXT> {};
template<>
struct fex_gen_config<glProgramUniform4i> {};
template<>
struct fex_gen_config<glProgramUniform4ivEXT> {};
template<>
struct fex_gen_config<glProgramUniform4iv> {};
template<>
struct fex_gen_config<glProgramUniform4ui64ARB> {};
template<>
struct fex_gen_config<glProgramUniform4ui64NV> {};
template<>
struct fex_gen_config<glProgramUniform4ui64vARB> {};
template<>
struct fex_gen_config<glProgramUniform4ui64vNV> {};
template<>
struct fex_gen_config<glProgramUniform4uiEXT> {};
template<>
struct fex_gen_config<glProgramUniform4ui> {};
template<>
struct fex_gen_config<glProgramUniform4uivEXT> {};
template<>
struct fex_gen_config<glProgramUniform4uiv> {};
template<>
struct fex_gen_config<glProgramUniformHandleui64ARB> {};
template<>
struct fex_gen_config<glProgramUniformHandleui64NV> {};
template<>
struct fex_gen_config<glProgramUniformHandleui64vARB> {};
template<>
struct fex_gen_config<glProgramUniformHandleui64vNV> {};
template<>
struct fex_gen_config<glProgramUniformMatrix2dvEXT> {};
template<>
struct fex_gen_config<glProgramUniformMatrix2dv> {};
template<>
struct fex_gen_config<glProgramUniformMatrix2fvEXT> {};
template<>
struct fex_gen_config<glProgramUniformMatrix2fv> {};
template<>
struct fex_gen_config<glProgramUniformMatrix2x3dvEXT> {};
template<>
struct fex_gen_config<glProgramUniformMatrix2x3dv> {};
template<>
struct fex_gen_config<glProgramUniformMatrix2x3fvEXT> {};
template<>
struct fex_gen_config<glProgramUniformMatrix2x3fv> {};
template<>
struct fex_gen_config<glProgramUniformMatrix2x4dvEXT> {};
template<>
struct fex_gen_config<glProgramUniformMatrix2x4dv> {};
template<>
struct fex_gen_config<glProgramUniformMatrix2x4fvEXT> {};
template<>
struct fex_gen_config<glProgramUniformMatrix2x4fv> {};
template<>
struct fex_gen_config<glProgramUniformMatrix3dvEXT> {};
template<>
struct fex_gen_config<glProgramUniformMatrix3dv> {};
template<>
struct fex_gen_config<glProgramUniformMatrix3fvEXT> {};
template<>
struct fex_gen_config<glProgramUniformMatrix3fv> {};
template<>
struct fex_gen_config<glProgramUniformMatrix3x2dvEXT> {};
template<>
struct fex_gen_config<glProgramUniformMatrix3x2dv> {};
template<>
struct fex_gen_config<glProgramUniformMatrix3x2fvEXT> {};
template<>
struct fex_gen_config<glProgramUniformMatrix3x2fv> {};
template<>
struct fex_gen_config<glProgramUniformMatrix3x4dvEXT> {};
template<>
struct fex_gen_config<glProgramUniformMatrix3x4dv> {};
template<>
struct fex_gen_config<glProgramUniformMatrix3x4fvEXT> {};
template<>
struct fex_gen_config<glProgramUniformMatrix3x4fv> {};
template<>
struct fex_gen_config<glProgramUniformMatrix4dvEXT> {};
template<>
struct fex_gen_config<glProgramUniformMatrix4dv> {};
template<>
struct fex_gen_config<glProgramUniformMatrix4fvEXT> {};
template<>
struct fex_gen_config<glProgramUniformMatrix4fv> {};
template<>
struct fex_gen_config<glProgramUniformMatrix4x2dvEXT> {};
template<>
struct fex_gen_config<glProgramUniformMatrix4x2dv> {};
template<>
struct fex_gen_config<glProgramUniformMatrix4x2fvEXT> {};
template<>
struct fex_gen_config<glProgramUniformMatrix4x2fv> {};
template<>
struct fex_gen_config<glProgramUniformMatrix4x3dvEXT> {};
template<>
struct fex_gen_config<glProgramUniformMatrix4x3dv> {};
template<>
struct fex_gen_config<glProgramUniformMatrix4x3fvEXT> {};
template<>
struct fex_gen_config<glProgramUniformMatrix4x3fv> {};
template<>
struct fex_gen_config<glProgramUniformui64NV> {};
template<>
struct fex_gen_config<glProgramUniformui64vNV> {};
template<>
struct fex_gen_config<glProgramVertexLimitNV> {};
template<>
struct fex_gen_config<glProvokingVertexEXT> {};
template<>
struct fex_gen_config<glProvokingVertex> {};
template<>
struct fex_gen_config<glPushAttrib> {};
template<>
struct fex_gen_config<glPushClientAttribDefaultEXT> {};
template<>
struct fex_gen_config<glPushClientAttrib> {};
template<>
struct fex_gen_config<glPushDebugGroup> {};
template<>
struct fex_gen_config<glPushGroupMarkerEXT> {};
template<>
struct fex_gen_config<glPushMatrix> {};
template<>
struct fex_gen_config<glPushName> {};
template<>
struct fex_gen_config<glQueryCounter> {};
template<>
struct fex_gen_config<glQueryObjectParameteruiAMD> {};
template<>
struct fex_gen_config<glQueryResourceTagNV> {};
template<>
struct fex_gen_config<glRasterPos2d> {};
template<>
struct fex_gen_config<glRasterPos2dv> {};
template<>
struct fex_gen_config<glRasterPos2f> {};
template<>
struct fex_gen_config<glRasterPos2fv> {};
template<>
struct fex_gen_config<glRasterPos2i> {};
template<>
struct fex_gen_config<glRasterPos2iv> {};
template<>
struct fex_gen_config<glRasterPos2s> {};
template<>
struct fex_gen_config<glRasterPos2sv> {};
template<>
struct fex_gen_config<glRasterPos2xOES> {};
template<>
struct fex_gen_config<glRasterPos2xvOES> {};
template<>
struct fex_gen_config<glRasterPos3d> {};
template<>
struct fex_gen_config<glRasterPos3dv> {};
template<>
struct fex_gen_config<glRasterPos3f> {};
template<>
struct fex_gen_config<glRasterPos3fv> {};
template<>
struct fex_gen_config<glRasterPos3i> {};
template<>
struct fex_gen_config<glRasterPos3iv> {};
template<>
struct fex_gen_config<glRasterPos3s> {};
template<>
struct fex_gen_config<glRasterPos3sv> {};
template<>
struct fex_gen_config<glRasterPos3xOES> {};
template<>
struct fex_gen_config<glRasterPos3xvOES> {};
template<>
struct fex_gen_config<glRasterPos4d> {};
template<>
struct fex_gen_config<glRasterPos4dv> {};
template<>
struct fex_gen_config<glRasterPos4f> {};
template<>
struct fex_gen_config<glRasterPos4fv> {};
template<>
struct fex_gen_config<glRasterPos4i> {};
template<>
struct fex_gen_config<glRasterPos4iv> {};
template<>
struct fex_gen_config<glRasterPos4s> {};
template<>
struct fex_gen_config<glRasterPos4sv> {};
template<>
struct fex_gen_config<glRasterPos4xOES> {};
template<>
struct fex_gen_config<glRasterPos4xvOES> {};
template<>
struct fex_gen_config<glRasterSamplesEXT> {};
template<>
struct fex_gen_config<glReadBuffer> {};
template<>
struct fex_gen_config<glReadInstrumentsSGIX> {};
template<>
struct fex_gen_config<glReadnPixelsARB> {};
template<>
struct fex_gen_config<glReadnPixels> {};
template<>
struct fex_gen_config<glReadPixels> {};
template<>
struct fex_gen_config<glRectd> {};
template<>
struct fex_gen_config<glRectdv> {};
template<>
struct fex_gen_config<glRectf> {};
template<>
struct fex_gen_config<glRectfv> {};
template<>
struct fex_gen_config<glRecti> {};
template<>
struct fex_gen_config<glRectiv> {};
template<>
struct fex_gen_config<glRects> {};
template<>
struct fex_gen_config<glRectsv> {};
template<>
struct fex_gen_config<glRectxOES> {};
template<>
struct fex_gen_config<glRectxvOES> {};
template<>
struct fex_gen_config<glReferencePlaneSGIX> {};
template<>
struct fex_gen_config<glReleaseShaderCompiler> {};
template<>
struct fex_gen_config<glRenderbufferStorageEXT> {};
template<>
struct fex_gen_config<glRenderbufferStorage> {};
template<>
struct fex_gen_config<glRenderbufferStorageMultisampleAdvancedAMD> {};
template<>
struct fex_gen_config<glRenderbufferStorageMultisampleCoverageNV> {};
template<>
struct fex_gen_config<glRenderbufferStorageMultisampleEXT> {};
template<>
struct fex_gen_config<glRenderbufferStorageMultisample> {};
template<>
struct fex_gen_config<glRenderGpuMaskNV> {};
#ifndef IS_32BIT_THUNK
// TODO: 32-bit support
template<>
struct fex_gen_config<glReplacementCodePointerSUN> {};
template<>
struct fex_gen_param<glReplacementCodePointerSUN, 2, const void**> : fexgen::assume_compatible_data_layout {};
#endif
template<>
struct fex_gen_config<glReplacementCodeubSUN> {};
template<>
struct fex_gen_config<glReplacementCodeubvSUN> {};
template<>
struct fex_gen_config<glReplacementCodeuiColor3fVertex3fSUN> {};
template<>
struct fex_gen_config<glReplacementCodeuiColor3fVertex3fvSUN> {};
template<>
struct fex_gen_config<glReplacementCodeuiColor4fNormal3fVertex3fSUN> {};
template<>
struct fex_gen_config<glReplacementCodeuiColor4fNormal3fVertex3fvSUN> {};
template<>
struct fex_gen_config<glReplacementCodeuiColor4ubVertex3fSUN> {};
template<>
struct fex_gen_config<glReplacementCodeuiColor4ubVertex3fvSUN> {};
template<>
struct fex_gen_config<glReplacementCodeuiNormal3fVertex3fSUN> {};
template<>
struct fex_gen_config<glReplacementCodeuiNormal3fVertex3fvSUN> {};
template<>
struct fex_gen_config<glReplacementCodeuiSUN> {};
template<>
struct fex_gen_config<glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN> {};
template<>
struct fex_gen_config<glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN> {};
template<>
struct fex_gen_config<glReplacementCodeuiTexCoord2fNormal3fVertex3fSUN> {};
template<>
struct fex_gen_config<glReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN> {};
template<>
struct fex_gen_config<glReplacementCodeuiTexCoord2fVertex3fSUN> {};
template<>
struct fex_gen_config<glReplacementCodeuiTexCoord2fVertex3fvSUN> {};
template<>
struct fex_gen_config<glReplacementCodeuiVertex3fSUN> {};
template<>
struct fex_gen_config<glReplacementCodeuiVertex3fvSUN> {};
template<>
struct fex_gen_config<glReplacementCodeuivSUN> {};
template<>
struct fex_gen_config<glReplacementCodeusSUN> {};
template<>
struct fex_gen_config<glReplacementCodeusvSUN> {};
template<>
struct fex_gen_config<glRequestResidentProgramsNV> {};
template<>
struct fex_gen_config<glResetHistogramEXT> {};
template<>
struct fex_gen_config<glResetHistogram> {};
template<>
struct fex_gen_config<glResetMemoryObjectParameterNV> {};
template<>
struct fex_gen_config<glResetMinmaxEXT> {};
template<>
struct fex_gen_config<glResetMinmax> {};
template<>
struct fex_gen_config<glResizeBuffersMESA> {};
template<>
struct fex_gen_config<glResolveDepthValuesNV> {};
template<>
struct fex_gen_config<glResumeTransformFeedback> {};
template<>
struct fex_gen_config<glResumeTransformFeedbackNV> {};
template<>
struct fex_gen_config<glRotated> {};
template<>
struct fex_gen_config<glRotatef> {};
template<>
struct fex_gen_config<glRotatexOES> {};
template<>
struct fex_gen_config<glSampleCoverageARB> {};
template<>
struct fex_gen_config<glSampleCoverage> {};
template<>
struct fex_gen_config<glSampleMapATI> {};
template<>
struct fex_gen_config<glSampleMaskEXT> {};
template<>
struct fex_gen_config<glSampleMaski> {};
template<>
struct fex_gen_config<glSampleMaskIndexedNV> {};
template<>
struct fex_gen_config<glSampleMaskSGIS> {};
template<>
struct fex_gen_config<glSamplePatternEXT> {};
template<>
struct fex_gen_config<glSamplePatternSGIS> {};
template<>
struct fex_gen_config<glSamplerParameterf> {};
template<>
struct fex_gen_config<glSamplerParameterfv> {};
template<>
struct fex_gen_config<glSamplerParameteri> {};
template<>
struct fex_gen_config<glSamplerParameterIiv> {};
template<>
struct fex_gen_config<glSamplerParameterIuiv> {};
template<>
struct fex_gen_config<glSamplerParameteriv> {};
template<>
struct fex_gen_config<glScaled> {};
template<>
struct fex_gen_config<glScalef> {};
template<>
struct fex_gen_config<glScalexOES> {};
template<>
struct fex_gen_config<glScissorArrayv> {};
template<>
struct fex_gen_config<glScissorExclusiveArrayvNV> {};
template<>
struct fex_gen_config<glScissorExclusiveNV> {};
template<>
struct fex_gen_config<glScissor> {};
template<>
struct fex_gen_config<glScissorIndexed> {};
template<>
struct fex_gen_config<glScissorIndexedv> {};
template<>
struct fex_gen_config<glSecondaryColor3bEXT> {};
template<>
struct fex_gen_config<glSecondaryColor3bvEXT> {};
template<>
struct fex_gen_config<glSecondaryColor3dEXT> {};
template<>
struct fex_gen_config<glSecondaryColor3dvEXT> {};
template<>
struct fex_gen_config<glSecondaryColor3fEXT> {};
template<>
struct fex_gen_config<glSecondaryColor3fvEXT> {};
template<>
struct fex_gen_config<glSecondaryColor3hNV> {};
template<>
struct fex_gen_config<glSecondaryColor3hvNV> {};
template<>
struct fex_gen_config<glSecondaryColor3iEXT> {};
template<>
struct fex_gen_config<glSecondaryColor3ivEXT> {};
template<>
struct fex_gen_config<glSecondaryColor3sEXT> {};
template<>
struct fex_gen_config<glSecondaryColor3svEXT> {};
template<>
struct fex_gen_config<glSecondaryColor3ubEXT> {};
template<>
struct fex_gen_config<glSecondaryColor3ubvEXT> {};
template<>
struct fex_gen_config<glSecondaryColor3uiEXT> {};
template<>
struct fex_gen_config<glSecondaryColor3uivEXT> {};
template<>
struct fex_gen_config<glSecondaryColor3usEXT> {};
template<>
struct fex_gen_config<glSecondaryColor3usvEXT> {};
template<>
struct fex_gen_config<glSecondaryColorFormatNV> {};
template<>
struct fex_gen_config<glSecondaryColorPointerEXT> {};
#ifndef IS_32BIT_THUNK
// TODO: 32-bit support
template<>
struct fex_gen_config<glSecondaryColorPointerListIBM> {};
template<>
struct fex_gen_param<glSecondaryColorPointerListIBM, 3, const void**> : fexgen::assume_compatible_data_layout {};
#endif
template<>
struct fex_gen_config<glSelectBuffer> {};
template<>
struct fex_gen_config<glSelectPerfMonitorCountersAMD> {};
template<>
struct fex_gen_config<glSemaphoreParameterui64vEXT> {};
template<>
struct fex_gen_config<glSeparableFilter2DEXT> {};
template<>
struct fex_gen_config<glSeparableFilter2D> {};
template<>
struct fex_gen_config<glSetFenceAPPLE> {};
template<>
struct fex_gen_config<glSetFenceNV> {};
template<>
struct fex_gen_config<glSetFragmentShaderConstantATI> {};
template<>
struct fex_gen_config<glSetInvariantEXT> {};
template<>
struct fex_gen_config<glSetLocalConstantEXT> {};
template<>
struct fex_gen_config<glSetMultisamplefvAMD> {};
template<>
struct fex_gen_config<glShadeModel> {};
template<>
struct fex_gen_config<glShaderBinary> {};
template<>
struct fex_gen_config<glShaderOp1EXT> {};
template<>
struct fex_gen_config<glShaderOp2EXT> {};
template<>
struct fex_gen_config<glShaderOp3EXT> {};
template<>
struct fex_gen_config<glShaderSource> : fexgen::custom_host_impl {};
template<>
struct fex_gen_param<glShaderSource, 2, const GLchar* const*> : fexgen::ptr_passthrough {};
template<>
struct fex_gen_config<glShaderSourceARB> : fexgen::custom_host_impl {};
template<>
struct fex_gen_param<glShaderSourceARB, 2, const GLcharARB**> : fexgen::ptr_passthrough {};
template<>
struct fex_gen_config<glShaderStorageBlockBinding> {};
template<>
struct fex_gen_config<glShadingRateImageBarrierNV> {};
template<>
struct fex_gen_config<glShadingRateImagePaletteNV> {};
template<>
struct fex_gen_config<glShadingRateSampleOrderCustomNV> {};
template<>
struct fex_gen_config<glShadingRateSampleOrderNV> {};
template<>
struct fex_gen_config<glSharpenTexFuncSGIS> {};
template<>
struct fex_gen_config<glSignalSemaphoreEXT> {};
template<>
struct fex_gen_config<glSignalSemaphoreui64NVX> {};
template<>
struct fex_gen_config<glSignalVkFenceNV> {};
template<>
struct fex_gen_config<glSignalVkSemaphoreNV> {};
template<>
struct fex_gen_config<glSpecializeShaderARB> {};
template<>
struct fex_gen_config<glSpecializeShader> {};
template<>
struct fex_gen_config<glSpriteParameterfSGIX> {};
template<>
struct fex_gen_config<glSpriteParameterfvSGIX> {};
template<>
struct fex_gen_config<glSpriteParameteriSGIX> {};
template<>
struct fex_gen_config<glSpriteParameterivSGIX> {};
template<>
struct fex_gen_config<glStartInstrumentsSGIX> {};
template<>
struct fex_gen_config<glStateCaptureNV> {};
template<>
struct fex_gen_config<glStencilClearTagEXT> {};
template<>
struct fex_gen_config<glStencilFillPathInstancedNV> {};
template<>
struct fex_gen_config<glStencilFillPathNV> {};
template<>
struct fex_gen_config<glStencilFunc> {};
template<>
struct fex_gen_config<glStencilFuncSeparateATI> {};
template<>
struct fex_gen_config<glStencilFuncSeparate> {};
template<>
struct fex_gen_config<glStencilMask> {};
template<>
struct fex_gen_config<glStencilMaskSeparate> {};
template<>
struct fex_gen_config<glStencilOp> {};
template<>
struct fex_gen_config<glStencilOpSeparateATI> {};
template<>
struct fex_gen_config<glStencilOpSeparate> {};
template<>
struct fex_gen_config<glStencilOpValueAMD> {};
template<>
struct fex_gen_config<glStencilStrokePathInstancedNV> {};
template<>
struct fex_gen_config<glStencilStrokePathNV> {};
template<>
struct fex_gen_config<glStencilThenCoverFillPathInstancedNV> {};
template<>
struct fex_gen_config<glStencilThenCoverFillPathNV> {};
template<>
struct fex_gen_config<glStencilThenCoverStrokePathInstancedNV> {};
template<>
struct fex_gen_config<glStencilThenCoverStrokePathNV> {};
template<>
struct fex_gen_config<glStopInstrumentsSGIX> {};
template<>
struct fex_gen_config<glStringMarkerGREMEDY> {};
template<>
struct fex_gen_config<glSubpixelPrecisionBiasNV> {};
template<>
struct fex_gen_config<glSwizzleEXT> {};
template<>
struct fex_gen_config<glSyncTextureINTEL> {};
template<>
struct fex_gen_config<glTagSampleBufferSGIX> {};
template<>
struct fex_gen_config<glTangent3bEXT> {};
template<>
struct fex_gen_config<glTangent3bvEXT> {};
template<>
struct fex_gen_config<glTangent3dEXT> {};
template<>
struct fex_gen_config<glTangent3dvEXT> {};
template<>
struct fex_gen_config<glTangent3fEXT> {};
template<>
struct fex_gen_config<glTangent3fvEXT> {};
template<>
struct fex_gen_config<glTangent3iEXT> {};
template<>
struct fex_gen_config<glTangent3ivEXT> {};
template<>
struct fex_gen_config<glTangent3sEXT> {};
template<>
struct fex_gen_config<glTangent3svEXT> {};
template<>
struct fex_gen_config<glTangentPointerEXT> {};
template<>
struct fex_gen_config<glTbufferMask3DFX> {};
template<>
struct fex_gen_config<glTessellationFactorAMD> {};
template<>
struct fex_gen_config<glTessellationModeAMD> {};
template<>
struct fex_gen_config<glTexAttachMemoryNV> {};
template<>
struct fex_gen_config<glTexBufferARB> {};
template<>
struct fex_gen_config<glTexBufferEXT> {};
template<>
struct fex_gen_config<glTexBuffer> {};
template<>
struct fex_gen_config<glTexBufferRange> {};
template<>
struct fex_gen_config<glTexBumpParameterfvATI> {};
template<>
struct fex_gen_config<glTexBumpParameterivATI> {};
template<>
struct fex_gen_config<glTexCoord1bOES> {};
template<>
struct fex_gen_config<glTexCoord1bvOES> {};
template<>
struct fex_gen_config<glTexCoord1d> {};
template<>
struct fex_gen_config<glTexCoord1dv> {};
template<>
struct fex_gen_config<glTexCoord1f> {};
template<>
struct fex_gen_config<glTexCoord1fv> {};
template<>
struct fex_gen_config<glTexCoord1hNV> {};
template<>
struct fex_gen_config<glTexCoord1hvNV> {};
template<>
struct fex_gen_config<glTexCoord1i> {};
template<>
struct fex_gen_config<glTexCoord1iv> {};
template<>
struct fex_gen_config<glTexCoord1s> {};
template<>
struct fex_gen_config<glTexCoord1sv> {};
template<>
struct fex_gen_config<glTexCoord1xOES> {};
template<>
struct fex_gen_config<glTexCoord1xvOES> {};
template<>
struct fex_gen_config<glTexCoord2bOES> {};
template<>
struct fex_gen_config<glTexCoord2bvOES> {};
template<>
struct fex_gen_config<glTexCoord2d> {};
template<>
struct fex_gen_config<glTexCoord2dv> {};
template<>
struct fex_gen_config<glTexCoord2fColor3fVertex3fSUN> {};
template<>
struct fex_gen_config<glTexCoord2fColor3fVertex3fvSUN> {};
template<>
struct fex_gen_config<glTexCoord2fColor4fNormal3fVertex3fSUN> {};
template<>
struct fex_gen_config<glTexCoord2fColor4fNormal3fVertex3fvSUN> {};
template<>
struct fex_gen_config<glTexCoord2fColor4ubVertex3fSUN> {};
template<>
struct fex_gen_config<glTexCoord2fColor4ubVertex3fvSUN> {};
template<>
struct fex_gen_config<glTexCoord2f> {};
template<>
struct fex_gen_config<glTexCoord2fNormal3fVertex3fSUN> {};
template<>
struct fex_gen_config<glTexCoord2fNormal3fVertex3fvSUN> {};
template<>
struct fex_gen_config<glTexCoord2fv> {};
template<>
struct fex_gen_config<glTexCoord2fVertex3fSUN> {};
template<>
struct fex_gen_config<glTexCoord2fVertex3fvSUN> {};
template<>
struct fex_gen_config<glTexCoord2hNV> {};
template<>
struct fex_gen_config<glTexCoord2hvNV> {};
template<>
struct fex_gen_config<glTexCoord2i> {};
template<>
struct fex_gen_config<glTexCoord2iv> {};
template<>
struct fex_gen_config<glTexCoord2s> {};
template<>
struct fex_gen_config<glTexCoord2sv> {};
template<>
struct fex_gen_config<glTexCoord2xOES> {};
template<>
struct fex_gen_config<glTexCoord2xvOES> {};
template<>
struct fex_gen_config<glTexCoord3bOES> {};
template<>
struct fex_gen_config<glTexCoord3bvOES> {};
template<>
struct fex_gen_config<glTexCoord3d> {};
template<>
struct fex_gen_config<glTexCoord3dv> {};
template<>
struct fex_gen_config<glTexCoord3f> {};
template<>
struct fex_gen_config<glTexCoord3fv> {};
template<>
struct fex_gen_config<glTexCoord3hNV> {};
template<>
struct fex_gen_config<glTexCoord3hvNV> {};
template<>
struct fex_gen_config<glTexCoord3i> {};
template<>
struct fex_gen_config<glTexCoord3iv> {};
template<>
struct fex_gen_config<glTexCoord3s> {};
template<>
struct fex_gen_config<glTexCoord3sv> {};
template<>
struct fex_gen_config<glTexCoord3xOES> {};
template<>
struct fex_gen_config<glTexCoord3xvOES> {};
template<>
struct fex_gen_config<glTexCoord4bOES> {};
template<>
struct fex_gen_config<glTexCoord4bvOES> {};
template<>
struct fex_gen_config<glTexCoord4d> {};
template<>
struct fex_gen_config<glTexCoord4dv> {};
template<>
struct fex_gen_config<glTexCoord4fColor4fNormal3fVertex4fSUN> {};
template<>
struct fex_gen_config<glTexCoord4fColor4fNormal3fVertex4fvSUN> {};
template<>
struct fex_gen_config<glTexCoord4f> {};
template<>
struct fex_gen_config<glTexCoord4fv> {};
template<>
struct fex_gen_config<glTexCoord4fVertex4fSUN> {};
template<>
struct fex_gen_config<glTexCoord4fVertex4fvSUN> {};
template<>
struct fex_gen_config<glTexCoord4hNV> {};
template<>
struct fex_gen_config<glTexCoord4hvNV> {};
template<>
struct fex_gen_config<glTexCoord4i> {};
template<>
struct fex_gen_config<glTexCoord4iv> {};
template<>
struct fex_gen_config<glTexCoord4s> {};
template<>
struct fex_gen_config<glTexCoord4sv> {};
template<>
struct fex_gen_config<glTexCoord4xOES> {};
template<>
struct fex_gen_config<glTexCoord4xvOES> {};
template<>
struct fex_gen_config<glTexCoordFormatNV> {};
template<>
struct fex_gen_config<glTexCoordPointerEXT> {};
template<>
struct fex_gen_config<glTexCoordPointer> {};
#ifndef IS_32BIT_THUNK
// TODO: 32-bit support
template<>
struct fex_gen_config<glTexCoordPointerListIBM> {};
template<>
struct fex_gen_param<glTexCoordPointerListIBM, 3, const void**> : fexgen::assume_compatible_data_layout {};
template<>
struct fex_gen_config<glTexCoordPointervINTEL> {};
template<>
struct fex_gen_param<glTexCoordPointervINTEL, 2, const void**> : fexgen::assume_compatible_data_layout {};
#endif
template<>
struct fex_gen_config<glTexEnvf> {};
template<>
struct fex_gen_config<glTexEnvfv> {};
template<>
struct fex_gen_config<glTexEnvi> {};
template<>
struct fex_gen_config<glTexEnviv> {};
template<>
struct fex_gen_config<glTexEnvxOES> {};
template<>
struct fex_gen_config<glTexEnvxvOES> {};
template<>
struct fex_gen_config<glTexFilterFuncSGIS> {};
template<>
struct fex_gen_config<glTexGend> {};
template<>
struct fex_gen_config<glTexGendv> {};
template<>
struct fex_gen_config<glTexGenf> {};
template<>
struct fex_gen_config<glTexGenfv> {};
template<>
struct fex_gen_config<glTexGeni> {};
template<>
struct fex_gen_config<glTexGeniv> {};
template<>
struct fex_gen_config<glTexGenxOES> {};
template<>
struct fex_gen_config<glTexGenxvOES> {};
template<>
struct fex_gen_config<glTexImage1D> {};
template<>
struct fex_gen_config<glTexImage2D> {};
template<>
struct fex_gen_config<glTexImage2DMultisampleCoverageNV> {};
template<>
struct fex_gen_config<glTexImage2DMultisample> {};
template<>
struct fex_gen_config<glTexImage3DEXT> {};
template<>
struct fex_gen_config<glTexImage3D> {};
template<>
struct fex_gen_config<glTexImage3DMultisampleCoverageNV> {};
template<>
struct fex_gen_config<glTexImage3DMultisample> {};
template<>
struct fex_gen_config<glTexImage4DSGIS> {};
template<>
struct fex_gen_config<glTexPageCommitmentARB> {};
template<>
struct fex_gen_config<glTexParameterf> {};
template<>
struct fex_gen_config<glTexParameterfv> {};
template<>
struct fex_gen_config<glTexParameteri> {};
template<>
struct fex_gen_config<glTexParameterIivEXT> {};
template<>
struct fex_gen_config<glTexParameterIiv> {};
template<>
struct fex_gen_config<glTexParameterIuivEXT> {};
template<>
struct fex_gen_config<glTexParameterIuiv> {};
template<>
struct fex_gen_config<glTexParameteriv> {};
template<>
struct fex_gen_config<glTexParameterxOES> {};
template<>
struct fex_gen_config<glTexParameterxvOES> {};
template<>
struct fex_gen_config<glTexRenderbufferNV> {};
template<>
struct fex_gen_config<glTexStorage1D> {};
template<>
struct fex_gen_config<glTexStorage2D> {};
template<>
struct fex_gen_config<glTexStorage2DMultisample> {};
template<>
struct fex_gen_config<glTexStorage3D> {};
template<>
struct fex_gen_config<glTexStorage3DMultisample> {};
template<>
struct fex_gen_config<glTexStorageMem1DEXT> {};
template<>
struct fex_gen_config<glTexStorageMem2DEXT> {};
template<>
struct fex_gen_config<glTexStorageMem2DMultisampleEXT> {};
template<>
struct fex_gen_config<glTexStorageMem3DEXT> {};
template<>
struct fex_gen_config<glTexStorageMem3DMultisampleEXT> {};
template<>
struct fex_gen_config<glTexStorageSparseAMD> {};
template<>
struct fex_gen_config<glTexSubImage1DEXT> {};
template<>
struct fex_gen_config<glTexSubImage1D> {};
template<>
struct fex_gen_config<glTexSubImage2DEXT> {};
template<>
struct fex_gen_config<glTexSubImage2D> {};
template<>
struct fex_gen_config<glTexSubImage3DEXT> {};
template<>
struct fex_gen_config<glTexSubImage3D> {};
template<>
struct fex_gen_config<glTexSubImage4DSGIS> {};
template<>
struct fex_gen_config<glTextureAttachMemoryNV> {};
template<>
struct fex_gen_config<glTextureBarrier> {};
template<>
struct fex_gen_config<glTextureBarrierNV> {};
template<>
struct fex_gen_config<glTextureBufferEXT> {};
template<>
struct fex_gen_config<glTextureBuffer> {};
template<>
struct fex_gen_config<glTextureBufferRangeEXT> {};
template<>
struct fex_gen_config<glTextureBufferRange> {};
template<>
struct fex_gen_config<glTextureColorMaskSGIS> {};
template<>
struct fex_gen_config<glTextureImage1DEXT> {};
template<>
struct fex_gen_config<glTextureImage2DEXT> {};
template<>
struct fex_gen_config<glTextureImage2DMultisampleCoverageNV> {};
template<>
struct fex_gen_config<glTextureImage2DMultisampleNV> {};
template<>
struct fex_gen_config<glTextureImage3DEXT> {};
template<>
struct fex_gen_config<glTextureImage3DMultisampleCoverageNV> {};
template<>
struct fex_gen_config<glTextureImage3DMultisampleNV> {};
template<>
struct fex_gen_config<glTextureLightEXT> {};
template<>
struct fex_gen_config<glTextureMaterialEXT> {};
template<>
struct fex_gen_config<glTextureNormalEXT> {};
template<>
struct fex_gen_config<glTexturePageCommitmentEXT> {};
template<>
struct fex_gen_config<glTextureParameterfEXT> {};
template<>
struct fex_gen_config<glTextureParameterf> {};
template<>
struct fex_gen_config<glTextureParameterfvEXT> {};
template<>
struct fex_gen_config<glTextureParameterfv> {};
template<>
struct fex_gen_config<glTextureParameteriEXT> {};
template<>
struct fex_gen_config<glTextureParameteri> {};
template<>
struct fex_gen_config<glTextureParameterIivEXT> {};
template<>
struct fex_gen_config<glTextureParameterIiv> {};
template<>
struct fex_gen_config<glTextureParameterIuivEXT> {};
template<>
struct fex_gen_config<glTextureParameterIuiv> {};
template<>
struct fex_gen_config<glTextureParameterivEXT> {};
template<>
struct fex_gen_config<glTextureParameteriv> {};
#ifndef IS_32BIT_THUNK
// TODO: 32-bit support
template<>
struct fex_gen_config<glTextureRangeAPPLE> {};
#endif
template<>
struct fex_gen_config<glTextureRenderbufferEXT> {};
template<>
struct fex_gen_config<glTextureStorage1DEXT> {};
template<>
struct fex_gen_config<glTextureStorage1D> {};
template<>
struct fex_gen_config<glTextureStorage2DEXT> {};
template<>
struct fex_gen_config<glTextureStorage2D> {};
template<>
struct fex_gen_config<glTextureStorage2DMultisampleEXT> {};
template<>
struct fex_gen_config<glTextureStorage2DMultisample> {};
template<>
struct fex_gen_config<glTextureStorage3DEXT> {};
template<>
struct fex_gen_config<glTextureStorage3D> {};
template<>
struct fex_gen_config<glTextureStorage3DMultisampleEXT> {};
template<>
struct fex_gen_config<glTextureStorage3DMultisample> {};
template<>
struct fex_gen_config<glTextureStorageMem1DEXT> {};
template<>
struct fex_gen_config<glTextureStorageMem2DEXT> {};
template<>
struct fex_gen_config<glTextureStorageMem2DMultisampleEXT> {};
template<>
struct fex_gen_config<glTextureStorageMem3DEXT> {};
template<>
struct fex_gen_config<glTextureStorageMem3DMultisampleEXT> {};
template<>
struct fex_gen_config<glTextureStorageSparseAMD> {};
template<>
struct fex_gen_config<glTextureSubImage1DEXT> {};
template<>
struct fex_gen_config<glTextureSubImage1D> {};
template<>
struct fex_gen_config<glTextureSubImage2DEXT> {};
template<>
struct fex_gen_config<glTextureSubImage2D> {};
template<>
struct fex_gen_config<glTextureSubImage3DEXT> {};
template<>
struct fex_gen_config<glTextureSubImage3D> {};
template<>
struct fex_gen_config<glTextureView> {};
template<>
struct fex_gen_config<glTrackMatrixNV> {};
template<>
struct fex_gen_config<glTransformFeedbackAttribsNV> {};
template<>
struct fex_gen_config<glTransformFeedbackBufferBase> {};
template<>
struct fex_gen_config<glTransformFeedbackBufferRange> {};
template<>
struct fex_gen_config<glTransformFeedbackStreamAttribsNV> {};
#ifndef IS_32BIT_THUNK
// TODO
template<>
struct fex_gen_config<glTransformFeedbackVaryingsEXT> {};
template<>
struct fex_gen_param<glTransformFeedbackVaryingsEXT, 2, const char* const*> : fexgen::assume_compatible_data_layout {};
template<>
struct fex_gen_config<glTransformFeedbackVaryings> {};
template<>
struct fex_gen_param<glTransformFeedbackVaryings, 2, const char* const*> : fexgen::assume_compatible_data_layout {};
#endif
template<>
struct fex_gen_config<glTransformFeedbackVaryingsNV> {};
template<>
struct fex_gen_config<glTransformPathNV> {};
template<>
struct fex_gen_config<glTranslated> {};
template<>
struct fex_gen_config<glTranslatef> {};
template<>
struct fex_gen_config<glTranslatexOES> {};
template<>
struct fex_gen_config<glUniform1d> {};
template<>
struct fex_gen_config<glUniform1dv> {};
template<>
struct fex_gen_config<glUniform1fARB> {};
template<>
struct fex_gen_config<glUniform1f> {};
template<>
struct fex_gen_config<glUniform1fvARB> {};
template<>
struct fex_gen_config<glUniform1fv> {};
template<>
struct fex_gen_config<glUniform1i64ARB> {};
template<>
struct fex_gen_config<glUniform1i64NV> {};
template<>
struct fex_gen_config<glUniform1i64vARB> {};
template<>
struct fex_gen_config<glUniform1i64vNV> {};
template<>
struct fex_gen_config<glUniform1iARB> {};
template<>
struct fex_gen_config<glUniform1i> {};
template<>
struct fex_gen_config<glUniform1ivARB> {};
template<>
struct fex_gen_config<glUniform1iv> {};
template<>
struct fex_gen_config<glUniform1ui64ARB> {};
template<>
struct fex_gen_config<glUniform1ui64NV> {};
template<>
struct fex_gen_config<glUniform1ui64vARB> {};
template<>
struct fex_gen_config<glUniform1ui64vNV> {};
template<>
struct fex_gen_config<glUniform1uiEXT> {};
template<>
struct fex_gen_config<glUniform1ui> {};
template<>
struct fex_gen_config<glUniform1uivEXT> {};
template<>
struct fex_gen_config<glUniform1uiv> {};
template<>
struct fex_gen_config<glUniform2d> {};
template<>
struct fex_gen_config<glUniform2dv> {};
template<>
struct fex_gen_config<glUniform2fARB> {};
template<>
struct fex_gen_config<glUniform2f> {};
template<>
struct fex_gen_config<glUniform2fvARB> {};
template<>
struct fex_gen_config<glUniform2fv> {};
template<>
struct fex_gen_config<glUniform2i64ARB> {};
template<>
struct fex_gen_config<glUniform2i64NV> {};
template<>
struct fex_gen_config<glUniform2i64vARB> {};
template<>
struct fex_gen_config<glUniform2i64vNV> {};
template<>
struct fex_gen_config<glUniform2iARB> {};
template<>
struct fex_gen_config<glUniform2i> {};
template<>
struct fex_gen_config<glUniform2ivARB> {};
template<>
struct fex_gen_config<glUniform2iv> {};
template<>
struct fex_gen_config<glUniform2ui64ARB> {};
template<>
struct fex_gen_config<glUniform2ui64NV> {};
template<>
struct fex_gen_config<glUniform2ui64vARB> {};
template<>
struct fex_gen_config<glUniform2ui64vNV> {};
template<>
struct fex_gen_config<glUniform2uiEXT> {};
template<>
struct fex_gen_config<glUniform2ui> {};
template<>
struct fex_gen_config<glUniform2uivEXT> {};
template<>
struct fex_gen_config<glUniform2uiv> {};
template<>
struct fex_gen_config<glUniform3d> {};
template<>
struct fex_gen_config<glUniform3dv> {};
template<>
struct fex_gen_config<glUniform3fARB> {};
template<>
struct fex_gen_config<glUniform3f> {};
template<>
struct fex_gen_config<glUniform3fvARB> {};
template<>
struct fex_gen_config<glUniform3fv> {};
template<>
struct fex_gen_config<glUniform3i64ARB> {};
template<>
struct fex_gen_config<glUniform3i64NV> {};
template<>
struct fex_gen_config<glUniform3i64vARB> {};
template<>
struct fex_gen_config<glUniform3i64vNV> {};
template<>
struct fex_gen_config<glUniform3iARB> {};
template<>
struct fex_gen_config<glUniform3i> {};
template<>
struct fex_gen_config<glUniform3ivARB> {};
template<>
struct fex_gen_config<glUniform3iv> {};
template<>
struct fex_gen_config<glUniform3ui64ARB> {};
template<>
struct fex_gen_config<glUniform3ui64NV> {};
template<>
struct fex_gen_config<glUniform3ui64vARB> {};
template<>
struct fex_gen_config<glUniform3ui64vNV> {};
template<>
struct fex_gen_config<glUniform3uiEXT> {};
template<>
struct fex_gen_config<glUniform3ui> {};
template<>
struct fex_gen_config<glUniform3uivEXT> {};
template<>
struct fex_gen_config<glUniform3uiv> {};
template<>
struct fex_gen_config<glUniform4d> {};
template<>
struct fex_gen_config<glUniform4dv> {};
template<>
struct fex_gen_config<glUniform4fARB> {};
template<>
struct fex_gen_config<glUniform4f> {};
template<>
struct fex_gen_config<glUniform4fvARB> {};
template<>
struct fex_gen_config<glUniform4fv> {};
template<>
struct fex_gen_config<glUniform4i64ARB> {};
template<>
struct fex_gen_config<glUniform4i64NV> {};
template<>
struct fex_gen_config<glUniform4i64vARB> {};
template<>
struct fex_gen_config<glUniform4i64vNV> {};
template<>
struct fex_gen_config<glUniform4iARB> {};
template<>
struct fex_gen_config<glUniform4i> {};
template<>
struct fex_gen_config<glUniform4ivARB> {};
template<>
struct fex_gen_config<glUniform4iv> {};
template<>
struct fex_gen_config<glUniform4ui64ARB> {};
template<>
struct fex_gen_config<glUniform4ui64NV> {};
template<>
struct fex_gen_config<glUniform4ui64vARB> {};
template<>
struct fex_gen_config<glUniform4ui64vNV> {};
template<>
struct fex_gen_config<glUniform4uiEXT> {};
template<>
struct fex_gen_config<glUniform4ui> {};
template<>
struct fex_gen_config<glUniform4uivEXT> {};
template<>
struct fex_gen_config<glUniform4uiv> {};
template<>
struct fex_gen_config<glUniformBlockBinding> {};
template<>
struct fex_gen_config<glUniformBufferEXT> {};
template<>
struct fex_gen_config<glUniformHandleui64ARB> {};
template<>
struct fex_gen_config<glUniformHandleui64NV> {};
template<>
struct fex_gen_config<glUniformHandleui64vARB> {};
template<>
struct fex_gen_config<glUniformHandleui64vNV> {};
template<>
struct fex_gen_config<glUniformMatrix2dv> {};
template<>
struct fex_gen_config<glUniformMatrix2fvARB> {};
template<>
struct fex_gen_config<glUniformMatrix2fv> {};
template<>
struct fex_gen_config<glUniformMatrix2x3dv> {};
template<>
struct fex_gen_config<glUniformMatrix2x3fv> {};
template<>
struct fex_gen_config<glUniformMatrix2x4dv> {};
template<>
struct fex_gen_config<glUniformMatrix2x4fv> {};
template<>
struct fex_gen_config<glUniformMatrix3dv> {};
template<>
struct fex_gen_config<glUniformMatrix3fvARB> {};
template<>
struct fex_gen_config<glUniformMatrix3fv> {};
template<>
struct fex_gen_config<glUniformMatrix3x2dv> {};
template<>
struct fex_gen_config<glUniformMatrix3x2fv> {};
template<>
struct fex_gen_config<glUniformMatrix3x4dv> {};
template<>
struct fex_gen_config<glUniformMatrix3x4fv> {};
template<>
struct fex_gen_config<glUniformMatrix4dv> {};
template<>
struct fex_gen_config<glUniformMatrix4fvARB> {};
template<>
struct fex_gen_config<glUniformMatrix4fv> {};
template<>
struct fex_gen_config<glUniformMatrix4x2dv> {};
template<>
struct fex_gen_config<glUniformMatrix4x2fv> {};
template<>
struct fex_gen_config<glUniformMatrix4x3dv> {};
template<>
struct fex_gen_config<glUniformMatrix4x3fv> {};
template<>
struct fex_gen_config<glUniformSubroutinesuiv> {};
template<>
struct fex_gen_config<glUniformui64NV> {};
template<>
struct fex_gen_config<glUniformui64vNV> {};
template<>
struct fex_gen_config<glUnlockArraysEXT> {};
template<>
struct fex_gen_config<glUnmapObjectBufferATI> {};
template<>
struct fex_gen_config<glUnmapTexture2DINTEL> {};
#ifndef IS_32BIT_THUNK
// TODO: 32-bit support
template<>
struct fex_gen_config<glUpdateObjectBufferATI> {};
#endif
template<>
struct fex_gen_config<glUploadGpuMaskNVX> {};
template<>
struct fex_gen_config<glUseProgram> {};
template<>
struct fex_gen_config<glUseProgramObjectARB> {};
template<>
struct fex_gen_config<glUseProgramStages> {};
template<>
struct fex_gen_config<glUseShaderProgramEXT> {};
template<>
struct fex_gen_config<glValidateProgramARB> {};
template<>
struct fex_gen_config<glValidateProgram> {};
template<>
struct fex_gen_config<glValidateProgramPipeline> {};
template<>
struct fex_gen_config<glVariantArrayObjectATI> {};
template<>
struct fex_gen_config<glVariantbvEXT> {};
template<>
struct fex_gen_config<glVariantdvEXT> {};
template<>
struct fex_gen_config<glVariantfvEXT> {};
template<>
struct fex_gen_config<glVariantivEXT> {};
template<>
struct fex_gen_config<glVariantPointerEXT> {};
template<>
struct fex_gen_config<glVariantsvEXT> {};
template<>
struct fex_gen_config<glVariantubvEXT> {};
template<>
struct fex_gen_config<glVariantuivEXT> {};
template<>
struct fex_gen_config<glVariantusvEXT> {};
template<>
struct fex_gen_config<glVDPAUFiniNV> {};
template<>
struct fex_gen_config<glVDPAUGetSurfaceivNV> {};
template<>
struct fex_gen_config<glVDPAUInitNV> {};
#ifndef IS_32BIT_THUNK
// TODO: 32-bit support
template<>
struct fex_gen_config<glVDPAUMapSurfacesNV> {};
#endif
template<>
struct fex_gen_config<glVDPAUSurfaceAccessNV> {};
#ifndef IS_32BIT_THUNK
// TODO: 32-bit support
template<>
struct fex_gen_config<glVDPAUUnmapSurfacesNV> {};
#endif
template<>
struct fex_gen_config<glVDPAUUnregisterSurfaceNV> {};
template<>
struct fex_gen_config<glVertex2bOES> {};
template<>
struct fex_gen_config<glVertex2bvOES> {};
template<>
struct fex_gen_config<glVertex2d> {};
template<>
struct fex_gen_config<glVertex2dv> {};
template<>
struct fex_gen_config<glVertex2f> {};
template<>
struct fex_gen_config<glVertex2fv> {};
template<>
struct fex_gen_config<glVertex2hNV> {};
template<>
struct fex_gen_config<glVertex2hvNV> {};
template<>
struct fex_gen_config<glVertex2i> {};
template<>
struct fex_gen_config<glVertex2iv> {};
template<>
struct fex_gen_config<glVertex2s> {};
template<>
struct fex_gen_config<glVertex2sv> {};
template<>
struct fex_gen_config<glVertex2xOES> {};
template<>
struct fex_gen_config<glVertex2xvOES> {};
template<>
struct fex_gen_config<glVertex3bOES> {};
template<>
struct fex_gen_config<glVertex3bvOES> {};
template<>
struct fex_gen_config<glVertex3d> {};
template<>
struct fex_gen_config<glVertex3dv> {};
template<>
struct fex_gen_config<glVertex3f> {};
template<>
struct fex_gen_config<glVertex3fv> {};
template<>
struct fex_gen_config<glVertex3hNV> {};
template<>
struct fex_gen_config<glVertex3hvNV> {};
template<>
struct fex_gen_config<glVertex3i> {};
template<>
struct fex_gen_config<glVertex3iv> {};
template<>
struct fex_gen_config<glVertex3s> {};
template<>
struct fex_gen_config<glVertex3sv> {};
template<>
struct fex_gen_config<glVertex3xOES> {};
template<>
struct fex_gen_config<glVertex3xvOES> {};
template<>
struct fex_gen_config<glVertex4bOES> {};
template<>
struct fex_gen_config<glVertex4bvOES> {};
template<>
struct fex_gen_config<glVertex4d> {};
template<>
struct fex_gen_config<glVertex4dv> {};
template<>
struct fex_gen_config<glVertex4f> {};
template<>
struct fex_gen_config<glVertex4fv> {};
template<>
struct fex_gen_config<glVertex4hNV> {};
template<>
struct fex_gen_config<glVertex4hvNV> {};
template<>
struct fex_gen_config<glVertex4i> {};
template<>
struct fex_gen_config<glVertex4iv> {};
template<>
struct fex_gen_config<glVertex4s> {};
template<>
struct fex_gen_config<glVertex4sv> {};
template<>
struct fex_gen_config<glVertex4xOES> {};
template<>
struct fex_gen_config<glVertex4xvOES> {};
template<>
struct fex_gen_config<glVertexArrayAttribBinding> {};
template<>
struct fex_gen_config<glVertexArrayAttribFormat> {};
template<>
struct fex_gen_config<glVertexArrayAttribIFormat> {};
template<>
struct fex_gen_config<glVertexArrayAttribLFormat> {};
template<>
struct fex_gen_config<glVertexArrayBindingDivisor> {};
template<>
struct fex_gen_config<glVertexArrayBindVertexBufferEXT> {};
template<>
struct fex_gen_config<glVertexArrayColorOffsetEXT> {};
template<>
struct fex_gen_config<glVertexArrayEdgeFlagOffsetEXT> {};
template<>
struct fex_gen_config<glVertexArrayElementBuffer> {};
template<>
struct fex_gen_config<glVertexArrayFogCoordOffsetEXT> {};
template<>
struct fex_gen_config<glVertexArrayIndexOffsetEXT> {};
template<>
struct fex_gen_config<glVertexArrayMultiTexCoordOffsetEXT> {};
template<>
struct fex_gen_config<glVertexArrayNormalOffsetEXT> {};
template<>
struct fex_gen_config<glVertexArrayParameteriAPPLE> {};
template<>
struct fex_gen_config<glVertexArrayRangeAPPLE> {};
#ifndef IS_32BIT_THUNK
// TODO: 32-bit support
template<>
struct fex_gen_config<glVertexArrayRangeNV> {};
#endif
template<>
struct fex_gen_config<glVertexArraySecondaryColorOffsetEXT> {};
template<>
struct fex_gen_config<glVertexArrayTexCoordOffsetEXT> {};
template<>
struct fex_gen_config<glVertexArrayVertexAttribBindingEXT> {};
template<>
struct fex_gen_config<glVertexArrayVertexAttribDivisorEXT> {};
template<>
struct fex_gen_config<glVertexArrayVertexAttribFormatEXT> {};
template<>
struct fex_gen_config<glVertexArrayVertexAttribIFormatEXT> {};
template<>
struct fex_gen_config<glVertexArrayVertexAttribIOffsetEXT> {};
template<>
struct fex_gen_config<glVertexArrayVertexAttribLFormatEXT> {};
template<>
struct fex_gen_config<glVertexArrayVertexAttribLOffsetEXT> {};
template<>
struct fex_gen_config<glVertexArrayVertexAttribOffsetEXT> {};
template<>
struct fex_gen_config<glVertexArrayVertexBindingDivisorEXT> {};
template<>
struct fex_gen_config<glVertexArrayVertexBuffer> {};
#ifndef IS_32BIT_THUNK
template<>
struct fex_gen_config<glVertexArrayVertexBuffers> {};
#else
template<>
struct fex_gen_config<glVertexArrayVertexBuffers> : fexgen::custom_host_impl {};
template<>
struct fex_gen_param<glVertexArrayVertexBuffers, 4, const GLintptr*> : fexgen::ptr_passthrough {};
#endif
template<>
struct fex_gen_config<glVertexArrayVertexOffsetEXT> {};
template<>
struct fex_gen_config<glVertexAttrib1dARB> {};
template<>
struct fex_gen_config<glVertexAttrib1d> {};
template<>
struct fex_gen_config<glVertexAttrib1dNV> {};
template<>
struct fex_gen_config<glVertexAttrib1dvARB> {};
template<>
struct fex_gen_config<glVertexAttrib1dv> {};
template<>
struct fex_gen_config<glVertexAttrib1dvNV> {};
template<>
struct fex_gen_config<glVertexAttrib1fARB> {};
template<>
struct fex_gen_config<glVertexAttrib1f> {};
template<>
struct fex_gen_config<glVertexAttrib1fNV> {};
template<>
struct fex_gen_config<glVertexAttrib1fvARB> {};
template<>
struct fex_gen_config<glVertexAttrib1fv> {};
template<>
struct fex_gen_config<glVertexAttrib1fvNV> {};
template<>
struct fex_gen_config<glVertexAttrib1hNV> {};
template<>
struct fex_gen_config<glVertexAttrib1hvNV> {};
template<>
struct fex_gen_config<glVertexAttrib1sARB> {};
template<>
struct fex_gen_config<glVertexAttrib1s> {};
template<>
struct fex_gen_config<glVertexAttrib1sNV> {};
template<>
struct fex_gen_config<glVertexAttrib1svARB> {};
template<>
struct fex_gen_config<glVertexAttrib1sv> {};
template<>
struct fex_gen_config<glVertexAttrib1svNV> {};
template<>
struct fex_gen_config<glVertexAttrib2dARB> {};
template<>
struct fex_gen_config<glVertexAttrib2d> {};
template<>
struct fex_gen_config<glVertexAttrib2dNV> {};
template<>
struct fex_gen_config<glVertexAttrib2dvARB> {};
template<>
struct fex_gen_config<glVertexAttrib2dv> {};
template<>
struct fex_gen_config<glVertexAttrib2dvNV> {};
template<>
struct fex_gen_config<glVertexAttrib2fARB> {};
template<>
struct fex_gen_config<glVertexAttrib2f> {};
template<>
struct fex_gen_config<glVertexAttrib2fNV> {};
template<>
struct fex_gen_config<glVertexAttrib2fvARB> {};
template<>
struct fex_gen_config<glVertexAttrib2fv> {};
template<>
struct fex_gen_config<glVertexAttrib2fvNV> {};
template<>
struct fex_gen_config<glVertexAttrib2hNV> {};
template<>
struct fex_gen_config<glVertexAttrib2hvNV> {};
template<>
struct fex_gen_config<glVertexAttrib2sARB> {};
template<>
struct fex_gen_config<glVertexAttrib2s> {};
template<>
struct fex_gen_config<glVertexAttrib2sNV> {};
template<>
struct fex_gen_config<glVertexAttrib2svARB> {};
template<>
struct fex_gen_config<glVertexAttrib2sv> {};
template<>
struct fex_gen_config<glVertexAttrib2svNV> {};
template<>
struct fex_gen_config<glVertexAttrib3dARB> {};
template<>
struct fex_gen_config<glVertexAttrib3d> {};
template<>
struct fex_gen_config<glVertexAttrib3dNV> {};
template<>
struct fex_gen_config<glVertexAttrib3dvARB> {};
template<>
struct fex_gen_config<glVertexAttrib3dv> {};
template<>
struct fex_gen_config<glVertexAttrib3dvNV> {};
template<>
struct fex_gen_config<glVertexAttrib3fARB> {};
template<>
struct fex_gen_config<glVertexAttrib3f> {};
template<>
struct fex_gen_config<glVertexAttrib3fNV> {};
template<>
struct fex_gen_config<glVertexAttrib3fvARB> {};
template<>
struct fex_gen_config<glVertexAttrib3fv> {};
template<>
struct fex_gen_config<glVertexAttrib3fvNV> {};
template<>
struct fex_gen_config<glVertexAttrib3hNV> {};
template<>
struct fex_gen_config<glVertexAttrib3hvNV> {};
template<>
struct fex_gen_config<glVertexAttrib3sARB> {};
template<>
struct fex_gen_config<glVertexAttrib3s> {};
template<>
struct fex_gen_config<glVertexAttrib3sNV> {};
template<>
struct fex_gen_config<glVertexAttrib3svARB> {};
template<>
struct fex_gen_config<glVertexAttrib3sv> {};
template<>
struct fex_gen_config<glVertexAttrib3svNV> {};
template<>
struct fex_gen_config<glVertexAttrib4bvARB> {};
template<>
struct fex_gen_config<glVertexAttrib4bv> {};
template<>
struct fex_gen_config<glVertexAttrib4dARB> {};
template<>
struct fex_gen_config<glVertexAttrib4d> {};
template<>
struct fex_gen_config<glVertexAttrib4dNV> {};
template<>
struct fex_gen_config<glVertexAttrib4dvARB> {};
template<>
struct fex_gen_config<glVertexAttrib4dv> {};
template<>
struct fex_gen_config<glVertexAttrib4dvNV> {};
template<>
struct fex_gen_config<glVertexAttrib4fARB> {};
template<>
struct fex_gen_config<glVertexAttrib4f> {};
template<>
struct fex_gen_config<glVertexAttrib4fNV> {};
template<>
struct fex_gen_config<glVertexAttrib4fvARB> {};
template<>
struct fex_gen_config<glVertexAttrib4fv> {};
template<>
struct fex_gen_config<glVertexAttrib4fvNV> {};
template<>
struct fex_gen_config<glVertexAttrib4hNV> {};
template<>
struct fex_gen_config<glVertexAttrib4hvNV> {};
template<>
struct fex_gen_config<glVertexAttrib4ivARB> {};
template<>
struct fex_gen_config<glVertexAttrib4iv> {};
template<>
struct fex_gen_config<glVertexAttrib4NbvARB> {};
template<>
struct fex_gen_config<glVertexAttrib4Nbv> {};
template<>
struct fex_gen_config<glVertexAttrib4NivARB> {};
template<>
struct fex_gen_config<glVertexAttrib4Niv> {};
template<>
struct fex_gen_config<glVertexAttrib4NsvARB> {};
template<>
struct fex_gen_config<glVertexAttrib4Nsv> {};
template<>
struct fex_gen_config<glVertexAttrib4NubARB> {};
template<>
struct fex_gen_config<glVertexAttrib4Nub> {};
template<>
struct fex_gen_config<glVertexAttrib4NubvARB> {};
template<>
struct fex_gen_config<glVertexAttrib4Nubv> {};
template<>
struct fex_gen_config<glVertexAttrib4NuivARB> {};
template<>
struct fex_gen_config<glVertexAttrib4Nuiv> {};
template<>
struct fex_gen_config<glVertexAttrib4NusvARB> {};
template<>
struct fex_gen_config<glVertexAttrib4Nusv> {};
template<>
struct fex_gen_config<glVertexAttrib4sARB> {};
template<>
struct fex_gen_config<glVertexAttrib4s> {};
template<>
struct fex_gen_config<glVertexAttrib4sNV> {};
template<>
struct fex_gen_config<glVertexAttrib4svARB> {};
template<>
struct fex_gen_config<glVertexAttrib4sv> {};
template<>
struct fex_gen_config<glVertexAttrib4svNV> {};
template<>
struct fex_gen_config<glVertexAttrib4ubNV> {};
template<>
struct fex_gen_config<glVertexAttrib4ubvARB> {};
template<>
struct fex_gen_config<glVertexAttrib4ubv> {};
template<>
struct fex_gen_config<glVertexAttrib4ubvNV> {};
template<>
struct fex_gen_config<glVertexAttrib4uivARB> {};
template<>
struct fex_gen_config<glVertexAttrib4uiv> {};
template<>
struct fex_gen_config<glVertexAttrib4usvARB> {};
template<>
struct fex_gen_config<glVertexAttrib4usv> {};
template<>
struct fex_gen_config<glVertexAttribArrayObjectATI> {};
template<>
struct fex_gen_config<glVertexAttribBinding> {};
template<>
struct fex_gen_config<glVertexAttribDivisorARB> {};
template<>
struct fex_gen_config<glVertexAttribDivisor> {};
template<>
struct fex_gen_config<glVertexAttribFormat> {};
template<>
struct fex_gen_config<glVertexAttribFormatNV> {};
template<>
struct fex_gen_config<glVertexAttribI1iEXT> {};
template<>
struct fex_gen_config<glVertexAttribI1i> {};
template<>
struct fex_gen_config<glVertexAttribI1ivEXT> {};
template<>
struct fex_gen_config<glVertexAttribI1iv> {};
template<>
struct fex_gen_config<glVertexAttribI1uiEXT> {};
template<>
struct fex_gen_config<glVertexAttribI1ui> {};
template<>
struct fex_gen_config<glVertexAttribI1uivEXT> {};
template<>
struct fex_gen_config<glVertexAttribI1uiv> {};
template<>
struct fex_gen_config<glVertexAttribI2iEXT> {};
template<>
struct fex_gen_config<glVertexAttribI2i> {};
template<>
struct fex_gen_config<glVertexAttribI2ivEXT> {};
template<>
struct fex_gen_config<glVertexAttribI2iv> {};
template<>
struct fex_gen_config<glVertexAttribI2uiEXT> {};
template<>
struct fex_gen_config<glVertexAttribI2ui> {};
template<>
struct fex_gen_config<glVertexAttribI2uivEXT> {};
template<>
struct fex_gen_config<glVertexAttribI2uiv> {};
template<>
struct fex_gen_config<glVertexAttribI3iEXT> {};
template<>
struct fex_gen_config<glVertexAttribI3i> {};
template<>
struct fex_gen_config<glVertexAttribI3ivEXT> {};
template<>
struct fex_gen_config<glVertexAttribI3iv> {};
template<>
struct fex_gen_config<glVertexAttribI3uiEXT> {};
template<>
struct fex_gen_config<glVertexAttribI3ui> {};
template<>
struct fex_gen_config<glVertexAttribI3uivEXT> {};
template<>
struct fex_gen_config<glVertexAttribI3uiv> {};
template<>
struct fex_gen_config<glVertexAttribI4bvEXT> {};
template<>
struct fex_gen_config<glVertexAttribI4bv> {};
template<>
struct fex_gen_config<glVertexAttribI4iEXT> {};
template<>
struct fex_gen_config<glVertexAttribI4i> {};
template<>
struct fex_gen_config<glVertexAttribI4ivEXT> {};
template<>
struct fex_gen_config<glVertexAttribI4iv> {};
template<>
struct fex_gen_config<glVertexAttribI4svEXT> {};
template<>
struct fex_gen_config<glVertexAttribI4sv> {};
template<>
struct fex_gen_config<glVertexAttribI4ubvEXT> {};
template<>
struct fex_gen_config<glVertexAttribI4ubv> {};
template<>
struct fex_gen_config<glVertexAttribI4uiEXT> {};
template<>
struct fex_gen_config<glVertexAttribI4ui> {};
template<>
struct fex_gen_config<glVertexAttribI4uivEXT> {};
template<>
struct fex_gen_config<glVertexAttribI4uiv> {};
template<>
struct fex_gen_config<glVertexAttribI4usvEXT> {};
template<>
struct fex_gen_config<glVertexAttribI4usv> {};
template<>
struct fex_gen_config<glVertexAttribIFormat> {};
template<>
struct fex_gen_config<glVertexAttribIFormatNV> {};
template<>
struct fex_gen_config<glVertexAttribIPointerEXT> {};
template<>
struct fex_gen_config<glVertexAttribIPointer> {};
template<>
struct fex_gen_config<glVertexAttribL1dEXT> {};
template<>
struct fex_gen_config<glVertexAttribL1d> {};
template<>
struct fex_gen_config<glVertexAttribL1dvEXT> {};
template<>
struct fex_gen_config<glVertexAttribL1dv> {};
template<>
struct fex_gen_config<glVertexAttribL1i64NV> {};
template<>
struct fex_gen_config<glVertexAttribL1i64vNV> {};
template<>
struct fex_gen_config<glVertexAttribL1ui64ARB> {};
template<>
struct fex_gen_config<glVertexAttribL1ui64NV> {};
template<>
struct fex_gen_config<glVertexAttribL1ui64vARB> {};
template<>
struct fex_gen_config<glVertexAttribL1ui64vNV> {};
template<>
struct fex_gen_config<glVertexAttribL2dEXT> {};
template<>
struct fex_gen_config<glVertexAttribL2d> {};
template<>
struct fex_gen_config<glVertexAttribL2dvEXT> {};
template<>
struct fex_gen_config<glVertexAttribL2dv> {};
template<>
struct fex_gen_config<glVertexAttribL2i64NV> {};
template<>
struct fex_gen_config<glVertexAttribL2i64vNV> {};
template<>
struct fex_gen_config<glVertexAttribL2ui64NV> {};
template<>
struct fex_gen_config<glVertexAttribL2ui64vNV> {};
template<>
struct fex_gen_config<glVertexAttribL3dEXT> {};
template<>
struct fex_gen_config<glVertexAttribL3d> {};
template<>
struct fex_gen_config<glVertexAttribL3dvEXT> {};
template<>
struct fex_gen_config<glVertexAttribL3dv> {};
template<>
struct fex_gen_config<glVertexAttribL3i64NV> {};
template<>
struct fex_gen_config<glVertexAttribL3i64vNV> {};
template<>
struct fex_gen_config<glVertexAttribL3ui64NV> {};
template<>
struct fex_gen_config<glVertexAttribL3ui64vNV> {};
template<>
struct fex_gen_config<glVertexAttribL4dEXT> {};
template<>
struct fex_gen_config<glVertexAttribL4d> {};
template<>
struct fex_gen_config<glVertexAttribL4dvEXT> {};
template<>
struct fex_gen_config<glVertexAttribL4dv> {};
template<>
struct fex_gen_config<glVertexAttribL4i64NV> {};
template<>
struct fex_gen_config<glVertexAttribL4i64vNV> {};
template<>
struct fex_gen_config<glVertexAttribL4ui64NV> {};
template<>
struct fex_gen_config<glVertexAttribL4ui64vNV> {};
template<>
struct fex_gen_config<glVertexAttribLFormat> {};
template<>
struct fex_gen_config<glVertexAttribLFormatNV> {};
template<>
struct fex_gen_config<glVertexAttribLPointerEXT> {};
template<>
struct fex_gen_config<glVertexAttribLPointer> {};
template<>
struct fex_gen_config<glVertexAttribP1ui> {};
template<>
struct fex_gen_config<glVertexAttribP1uiv> {};
template<>
struct fex_gen_config<glVertexAttribP2ui> {};
template<>
struct fex_gen_config<glVertexAttribP2uiv> {};
template<>
struct fex_gen_config<glVertexAttribP3ui> {};
template<>
struct fex_gen_config<glVertexAttribP3uiv> {};
template<>
struct fex_gen_config<glVertexAttribP4ui> {};
template<>
struct fex_gen_config<glVertexAttribP4uiv> {};
template<>
struct fex_gen_config<glVertexAttribParameteriAMD> {};
template<>
struct fex_gen_config<glVertexAttribPointerARB> {};
template<>
struct fex_gen_config<glVertexAttribPointer> {};
template<>
struct fex_gen_config<glVertexAttribPointerNV> {};
template<>
struct fex_gen_config<glVertexAttribs1dvNV> {};
template<>
struct fex_gen_config<glVertexAttribs1fvNV> {};
template<>
struct fex_gen_config<glVertexAttribs1hvNV> {};
template<>
struct fex_gen_config<glVertexAttribs1svNV> {};
template<>
struct fex_gen_config<glVertexAttribs2dvNV> {};
template<>
struct fex_gen_config<glVertexAttribs2fvNV> {};
template<>
struct fex_gen_config<glVertexAttribs2hvNV> {};
template<>
struct fex_gen_config<glVertexAttribs2svNV> {};
template<>
struct fex_gen_config<glVertexAttribs3dvNV> {};
template<>
struct fex_gen_config<glVertexAttribs3fvNV> {};
template<>
struct fex_gen_config<glVertexAttribs3hvNV> {};
template<>
struct fex_gen_config<glVertexAttribs3svNV> {};
template<>
struct fex_gen_config<glVertexAttribs4dvNV> {};
template<>
struct fex_gen_config<glVertexAttribs4fvNV> {};
template<>
struct fex_gen_config<glVertexAttribs4hvNV> {};
template<>
struct fex_gen_config<glVertexAttribs4svNV> {};
template<>
struct fex_gen_config<glVertexAttribs4ubvNV> {};
template<>
struct fex_gen_config<glVertexBindingDivisor> {};
template<>
struct fex_gen_config<glVertexBlendARB> {};
template<>
struct fex_gen_config<glVertexBlendEnvfATI> {};
template<>
struct fex_gen_config<glVertexBlendEnviATI> {};
template<>
struct fex_gen_config<glVertexFormatNV> {};
template<>
struct fex_gen_config<glVertexPointerEXT> {};
template<>
struct fex_gen_config<glVertexPointer> {};
#ifndef IS_32BIT_THUNK
// TODO: 32-bit support
template<>
struct fex_gen_config<glVertexPointerListIBM> {};
template<>
struct fex_gen_param<glVertexPointerListIBM, 3, const void**> : fexgen::assume_compatible_data_layout {};
template<>
struct fex_gen_config<glVertexPointervINTEL> {};
template<>
struct fex_gen_param<glVertexPointervINTEL, 2, const void**> : fexgen::assume_compatible_data_layout {};
#endif
template<>
struct fex_gen_config<glVertexStream1dATI> {};
template<>
struct fex_gen_config<glVertexStream1dvATI> {};
template<>
struct fex_gen_config<glVertexStream1fATI> {};
template<>
struct fex_gen_config<glVertexStream1fvATI> {};
template<>
struct fex_gen_config<glVertexStream1iATI> {};
template<>
struct fex_gen_config<glVertexStream1ivATI> {};
template<>
struct fex_gen_config<glVertexStream1sATI> {};
template<>
struct fex_gen_config<glVertexStream1svATI> {};
template<>
struct fex_gen_config<glVertexStream2dATI> {};
template<>
struct fex_gen_config<glVertexStream2dvATI> {};
template<>
struct fex_gen_config<glVertexStream2fATI> {};
template<>
struct fex_gen_config<glVertexStream2fvATI> {};
template<>
struct fex_gen_config<glVertexStream2iATI> {};
template<>
struct fex_gen_config<glVertexStream2ivATI> {};
template<>
struct fex_gen_config<glVertexStream2sATI> {};
template<>
struct fex_gen_config<glVertexStream2svATI> {};
template<>
struct fex_gen_config<glVertexStream3dATI> {};
template<>
struct fex_gen_config<glVertexStream3dvATI> {};
template<>
struct fex_gen_config<glVertexStream3fATI> {};
template<>
struct fex_gen_config<glVertexStream3fvATI> {};
template<>
struct fex_gen_config<glVertexStream3iATI> {};
template<>
struct fex_gen_config<glVertexStream3ivATI> {};
template<>
struct fex_gen_config<glVertexStream3sATI> {};
template<>
struct fex_gen_config<glVertexStream3svATI> {};
template<>
struct fex_gen_config<glVertexStream4dATI> {};
template<>
struct fex_gen_config<glVertexStream4dvATI> {};
template<>
struct fex_gen_config<glVertexStream4fATI> {};
template<>
struct fex_gen_config<glVertexStream4fvATI> {};
template<>
struct fex_gen_config<glVertexStream4iATI> {};
template<>
struct fex_gen_config<glVertexStream4ivATI> {};
template<>
struct fex_gen_config<glVertexStream4sATI> {};
template<>
struct fex_gen_config<glVertexStream4svATI> {};
template<>
struct fex_gen_config<glVertexWeightfEXT> {};
template<>
struct fex_gen_config<glVertexWeightfvEXT> {};
template<>
struct fex_gen_config<glVertexWeighthNV> {};
template<>
struct fex_gen_config<glVertexWeighthvNV> {};
template<>
struct fex_gen_config<glVertexWeightPointerEXT> {};
template<>
struct fex_gen_config<glVideoCaptureStreamParameterdvNV> {};
template<>
struct fex_gen_config<glVideoCaptureStreamParameterfvNV> {};
template<>
struct fex_gen_config<glVideoCaptureStreamParameterivNV> {};
template<>
struct fex_gen_config<glViewportArrayv> {};
template<>
struct fex_gen_config<glViewport> {};
template<>
struct fex_gen_config<glViewportIndexedf> {};
template<>
struct fex_gen_config<glViewportIndexedfv> {};
template<>
struct fex_gen_config<glViewportPositionWScaleNV> {};
template<>
struct fex_gen_config<glViewportSwizzleNV> {};
template<>
struct fex_gen_config<glWaitSemaphoreEXT> {};
template<>
struct fex_gen_config<glWaitSemaphoreui64NVX> {};
template<>
struct fex_gen_config<glWaitSync> {};
template<>
struct fex_gen_config<glWaitVkSemaphoreNV> {};
template<>
struct fex_gen_config<glWeightbvARB> {};
template<>
struct fex_gen_config<glWeightdvARB> {};
template<>
struct fex_gen_config<glWeightfvARB> {};
template<>
struct fex_gen_config<glWeightivARB> {};
template<>
struct fex_gen_config<glWeightPathsNV> {};
template<>
struct fex_gen_config<glWeightPointerARB> {};
template<>
struct fex_gen_config<glWeightsvARB> {};
template<>
struct fex_gen_config<glWeightubvARB> {};
template<>
struct fex_gen_config<glWeightuivARB> {};
template<>
struct fex_gen_config<glWeightusvARB> {};
template<>
struct fex_gen_config<glWindowPos2dARB> {};
template<>
struct fex_gen_config<glWindowPos2dMESA> {};
template<>
struct fex_gen_config<glWindowPos2dvARB> {};
template<>
struct fex_gen_config<glWindowPos2dvMESA> {};
template<>
struct fex_gen_config<glWindowPos2fARB> {};
template<>
struct fex_gen_config<glWindowPos2fMESA> {};
template<>
struct fex_gen_config<glWindowPos2fvARB> {};
template<>
struct fex_gen_config<glWindowPos2fvMESA> {};
template<>
struct fex_gen_config<glWindowPos2iARB> {};
template<>
struct fex_gen_config<glWindowPos2iMESA> {};
template<>
struct fex_gen_config<glWindowPos2ivARB> {};
template<>
struct fex_gen_config<glWindowPos2ivMESA> {};
template<>
struct fex_gen_config<glWindowPos2sARB> {};
template<>
struct fex_gen_config<glWindowPos2sMESA> {};
template<>
struct fex_gen_config<glWindowPos2svARB> {};
template<>
struct fex_gen_config<glWindowPos2svMESA> {};
template<>
struct fex_gen_config<glWindowPos3dARB> {};
template<>
struct fex_gen_config<glWindowPos3dMESA> {};
template<>
struct fex_gen_config<glWindowPos3dvARB> {};
template<>
struct fex_gen_config<glWindowPos3dvMESA> {};
template<>
struct fex_gen_config<glWindowPos3fARB> {};
template<>
struct fex_gen_config<glWindowPos3fMESA> {};
template<>
struct fex_gen_config<glWindowPos3fvARB> {};
template<>
struct fex_gen_config<glWindowPos3fvMESA> {};
template<>
struct fex_gen_config<glWindowPos3iARB> {};
template<>
struct fex_gen_config<glWindowPos3iMESA> {};
template<>
struct fex_gen_config<glWindowPos3ivARB> {};
template<>
struct fex_gen_config<glWindowPos3ivMESA> {};
template<>
struct fex_gen_config<glWindowPos3sARB> {};
template<>
struct fex_gen_config<glWindowPos3sMESA> {};
template<>
struct fex_gen_config<glWindowPos3svARB> {};
template<>
struct fex_gen_config<glWindowPos3svMESA> {};
template<>
struct fex_gen_config<glWindowPos4dMESA> {};
template<>
struct fex_gen_config<glWindowPos4dvMESA> {};
template<>
struct fex_gen_config<glWindowPos4fMESA> {};
template<>
struct fex_gen_config<glWindowPos4fvMESA> {};
template<>
struct fex_gen_config<glWindowPos4iMESA> {};
template<>
struct fex_gen_config<glWindowPos4ivMESA> {};
template<>
struct fex_gen_config<glWindowPos4sMESA> {};
template<>
struct fex_gen_config<glWindowPos4svMESA> {};
template<>
struct fex_gen_config<glWindowRectanglesEXT> {};
template<>
struct fex_gen_config<glWriteMaskEXT> {};

// GLext.h
// template<> struct fex_gen_config<glGetVkProcAddrNV> : fexgen::custom_guest_entrypoint, fexgen::returns_guest_pointer{};
template<>
struct fex_gen_config<glPathColorGenNV> {};
template<>
struct fex_gen_config<glPathTexGenNV> {};
template<>
struct fex_gen_config<glPathFogGenNV> {};
template<>
struct fex_gen_config<glGetPathColorGenivNV> {};
template<>
struct fex_gen_config<glGetPathColorGenfvNV> {};
template<>
struct fex_gen_config<glGetPathTexGenivNV> {};
template<>
struct fex_gen_config<glGetPathTexGenfvNV> {};
template<>
struct fex_gen_config<glBlendEquationSeparateATI> {};

// glx.h
template<>
struct fex_gen_config<glXWaitX> {};
#ifdef GLX_ARB_render_texture
template<>
struct fex_gen_config<glXBindTexImageARB> {};
template<>
struct fex_gen_config<glXReleaseTexImageARB> {};
template<>
struct fex_gen_config<glXDrawableAttribARB> {};
#endif
#ifdef GLX_MESA_swap_frame_usage
template<>
struct fex_gen_config<glXGetFrameUsageMESA> {};
template<>
struct fex_gen_config<glXBeginFrameTrackingMESA> {};
template<>
struct fex_gen_config<glXEndFrameTrackingMESA> {};
template<>
struct fex_gen_config<glXQueryFrameTrackingMESA> {};
#endif

// glxext.h
template<>
struct fex_gen_config<glXGetGPUIDsAMD> {};
template<>
struct fex_gen_config<glXGetGPUInfoAMD> {};
template<>
struct fex_gen_config<glXGetContextGPUIDAMD> {};
template<>
struct fex_gen_config<glXCreateAssociatedContextAMD> {};
template<>
struct fex_gen_config<glXCreateAssociatedContextAttribsAMD> {};
template<>
struct fex_gen_config<glXDeleteAssociatedContextAMD> {};
template<>
struct fex_gen_config<glXMakeAssociatedContextCurrentAMD> {};
template<>
struct fex_gen_config<glXGetCurrentAssociatedContextAMD> {};
template<>
struct fex_gen_config<glXBlitContextFramebufferAMD> {};
template<>
struct fex_gen_config<glXGetCurrentDisplayEXT> : fexgen::custom_host_impl {};
template<>
struct fex_gen_param<glXGetCurrentDisplayEXT, -1, _XDisplay*> : fexgen::ptr_passthrough {};
template<>
struct fex_gen_config<glXGetAGPOffsetMESA> {};
template<>
struct fex_gen_config<glXCreateGLXPixmapMESA> : fexgen::custom_host_impl {};
template<>
struct fex_gen_param<glXCreateGLXPixmapMESA, 1, XVisualInfo*> : fexgen::ptr_passthrough {};
template<>
struct fex_gen_config<glXReleaseBuffersMESA> {};
template<>
struct fex_gen_config<glXSet3DfxModeMESA> {};
template<>
struct fex_gen_config<glXCopyBufferSubDataNV> {};
template<>
struct fex_gen_config<glXNamedCopyBufferSubDataNV> {};
template<>
struct fex_gen_config<glXCopyImageSubDataNV> {};
template<>
struct fex_gen_config<glXDelayBeforeSwapNV> {};
template<>
struct fex_gen_config<glXEnumerateVideoDevicesNV> {}; // TODO: Custom host impl
template<>
struct fex_gen_config<glXBindVideoDeviceNV> {};
template<>
struct fex_gen_config<glXJoinSwapGroupNV> {};
template<>
struct fex_gen_config<glXBindSwapBarrierNV> {};
template<>
struct fex_gen_config<glXQuerySwapGroupNV> {};
template<>
struct fex_gen_config<glXQueryMaxSwapGroupsNV> {};
template<>
struct fex_gen_config<glXQueryFrameCountNV> {};
template<>
struct fex_gen_config<glXResetFrameCountNV> {};
template<>
struct fex_gen_config<glXBindVideoCaptureDeviceNV> {};
#ifndef IS_32BIT_THUNK
// TODO: 32-bit support
template<>
struct fex_gen_config<glXEnumerateVideoCaptureDevicesNV> {};
#endif
template<>
struct fex_gen_config<glXLockVideoCaptureDeviceNV> {};
template<>
struct fex_gen_config<glXQueryVideoCaptureDeviceNV> {};
template<>
struct fex_gen_config<glXReleaseVideoCaptureDeviceNV> {};
template<>
struct fex_gen_config<glXGetVideoDeviceNV> {};
template<>
struct fex_gen_config<glXReleaseVideoDeviceNV> {};
template<>
struct fex_gen_config<glXBindVideoImageNV> {};
template<>
struct fex_gen_config<glXReleaseVideoImageNV> {};
#ifndef IS_32BIT_THUNK
// TODO: 32-bit support
template<>
struct fex_gen_config<glXSendPbufferToVideoNV> {};
template<>
struct fex_gen_config<glXGetVideoInfoNV> {};
#endif
template<>
struct fex_gen_config<glXQueryHyperpipeNetworkSGIX> {};
template<>
struct fex_gen_config<glXHyperpipeConfigSGIX> {};
template<>
struct fex_gen_config<glXQueryHyperpipeConfigSGIX> {};
template<>
struct fex_gen_config<glXDestroyHyperpipeConfigSGIX> {};
template<>
struct fex_gen_config<glXBindHyperpipeSGIX> {};
template<>
struct fex_gen_config<glXQueryHyperpipeBestAttribSGIX> {};
template<>
struct fex_gen_config<glXHyperpipeAttribSGIX> {};
template<>
struct fex_gen_config<glXQueryHyperpipeAttribSGIX> {};
template<>
struct fex_gen_config<glXBindSwapBarrierSGIX> {};
template<>
struct fex_gen_config<glXQueryMaxSwapBarriersSGIX> {};
template<>
struct fex_gen_config<glXJoinSwapGroupSGIX> {};
template<>
struct fex_gen_config<glXBindChannelToWindowSGIX> {};
template<>
struct fex_gen_config<glXChannelRectSGIX> {};
template<>
struct fex_gen_config<glXQueryChannelRectSGIX> {};
template<>
struct fex_gen_config<glXQueryChannelDeltasSGIX> {};
template<>
struct fex_gen_config<glXChannelRectSyncSGIX> {};
template<>
struct fex_gen_config<glXCushionSGI> {};
#ifndef IS_32BIT_THUNK
// TODO: 32-bit support
template<>
struct fex_gen_config<glXGetTransparentIndexSUN> {};
#endif
} // namespace internal
