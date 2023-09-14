//******************************************************************************************************************************
// Copyright (c) 2009-2016, 2019-2020, 2022 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//******************************************************************************************************************************

#ifndef __gl3extqcom_h_
#define __gl3extqcom_h_

#ifndef __gl2ext_h_
#   include <GLES2/gl2ext.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef GL_APIENTRYP
#   define GL_APIENTRYP GL_APIENTRY*
#endif

#include <KHR/khrplatform.h>
/*------------------------------------------------------------------------*
 * AMD extension tokens and functions
 *------------------------------------------------------------------------*/

/* private extension */
/* This gives ES 1.1 ability to turn on/off multisampling using glEnable/glDisable. In ES 2.0 this is always turned on */
#define GL_MULTISAMPLE                                 0x809D

/* private extension */
/* This gives ES 1.1 ability to query state with GL_FIXED type */
#ifndef GL_need_a_name
#define GL_need_a_name 1
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glGetFixedvAMD (GLenum pname, GLfixed *params);
#endif
typedef void (GL_APIENTRYP PFNGLGETFIXEDVAMDPROC) (GLenum pname, GLfixed *params);
#endif

/* AMD_logic_op */
#ifndef GL_AMD_logic_op
#define GL_AMD_logic_op 1
#define GL_COLOR_LOGIC_OP_AMD                          0x0BF2
#define GL_LOGIC_OP_MODE_AMD                           0x0BF0
#define GL_CLEAR_AMD                                   0x1500
#define GL_AND_AMD                                     0x1501
#define GL_AND_REVERSE_AMD                             0x1502
#define GL_COPY_AMD                                    0x1503
#define GL_AND_INVERTED_AMD                            0x1504
#define GL_NOOP_AMD                                    0x1505
#define GL_XOR_AMD                                     0x1506
#define GL_OR_AMD                                      0x1507
#define GL_NOR_AMD                                     0x1508
#define GL_EQUIV_AMD                                   0x1509
#define GL_INVERT_AMD                                  0x150A
#define GL_OR_REVERSE_AMD                              0x150B
#define GL_COPY_INVERTED_AMD                           0x150C
#define GL_OR_INVERTED_AMD                             0x150D
#define GL_NAND_AMD                                    0x150E
#define GL_SET_AMD                                     0x150F
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glLogicOpAMD (GLenum op);
#endif
typedef void (GL_APIENTRYP PFNGLLOGICOPAMDPROC) (GLenum op);
#endif

/* HintTarget */
#define GL_TEXTURE_COMPRESSION_HINT                     0x84EF    // USED FOR AUTOMATIC ONLINE TEXTURE COMPRESSION

/* AMD_fog_blend */
#ifndef GL_AMD_fog_blend
#define GL_AMD_fog_blend 1
#define GL_FOG_COLOR_AMD                                0x0B66
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glFogfvAMD (GLenum pname, const GLfloat *params);
#endif
typedef void (GL_APIENTRYP PFNGLFOGFVAMDPROC) (GLenum pname, const GLfloat *params);
#endif

/* OVR_multiview */
#ifndef GL_OVR_multiview
#define GL_OVR_multiview 1
#define GL_MAX_VIEWS_OVR                                       0x9631
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_NUM_VIEWS_OVR        0x9630
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_BASE_VIEW_INDEX_OVR  0x9632
GL_APICALL void GL_APIENTRY glFramebufferTextureMultiviewOVR(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint baseViewIndex, GLsizei numViews);
#endif /* GL_OVR_multiview */

/* GL_AMD_EGL_image_external_layout_specifier */
#ifndef GL_AMD_EGL_image_external_layout_specifier
#define GL_AMD_EGL_image_external_layout_specifier 1
// YUV format
#define GL_YUV_AMD                                      0x8BC7
// YUV types
#define GL_YUYV_AMD                                     0x8BC8
#define GL_UYVY_AMD                                     0x8BC9
#define GL_NV21_AMD                                     0x8BCA
#define GL_YV12_AMD                                     0x8BCB
#define GL_NV12_TILED_AMD                               0x8BCC
#define GL_NV12_QCOM                                    0x8FB9
// YUV samplers
#define GL_SAMPLER_EXTERNAL_YUYV_AMD                    0x8BCD
#define GL_SAMPLER_EXTERNAL_UYVY_AMD                    0x8BCE
#define GL_SAMPLER_EXTERNAL_NV21_AMD                    0x8BCF
#define GL_SAMPLER_EXTERNAL_YV12_AMD                    0x8BD0
#define GL_SAMPLER_EXTERNAL_NV12_TILED_AMD              0x8BD1
#define GL_SAMPLER_EXTERNAL_NV12_QCOM                   0x8FBA
#endif

/* QCOM_binning_control */
#ifndef GL_QCOM_BINNING_CONTROL
#define GL_QCOM_BINNING_CONTROL 1
#define GL_BINNING_CONTROL_HINT_QCOM                         0x8FB0
#define GL_CPU_OPTIMIZED_QCOM                                0x8FB1
#define GL_GPU_OPTIMIZED_QCOM                                0x8FB2
#define GL_RENDER_DIRECT_TO_FRAMEBUFFER_QCOM                 0x8FB3
#endif

/* QCOM_ADRENO_PANEL */
#ifndef GL_QCOM_ADRENO_PANEL
#define GL_QCOM_ADRENO_PANEL 1
#define GL_QCOM_ADRENO_PANEL_INFO               0x8FB7
#endif // GL_QCOM_ADRENO_PANEL

/* GL_AMD_panel_settings */
#ifndef GL_AMD_panel_settings
#define GL_AMD_panel_settings 1
#define GL_PANEL_MODE_IFD_AMD                   0x8880 //TEMP TOKEN
#define GL_PANEL_MODE_IFH_AMD                   0x8881 //TEMP TOKEN
#define GL_PANEL_POWERFLAGS_OVERRIDE_AMD        0x8882 //TEMP TOKEN
#define GL_PANEL_AUTO_TEX_COMPRESSION_AMD       0x8883 //TEMP TOKEN
#endif

/* GL_EXT_frag_depth */
#ifndef GL_EXT_frag_depth
#define GL_EXT_frag_depth 1
#endif

/* GL_EXT_texture_border_clamp */
#ifndef GL_EXT_texture_border_clamp
#define GL_EXT_texture_border_clamp 1
#define GL_TEXTURE_BORDER_COLOR_EXT                     0x1004
#define GL_CLAMP_TO_BORDER_EXT                          0x812D

#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glTexParameterIivEXT(GLenum target, GLenum pname, const GLint *params);
GL_APICALL void GL_APIENTRY glTexParameterIuivEXT(GLenum target, GLenum pname, const GLuint *params);
GL_APICALL void GL_APIENTRY glGetTexParameterIivEXT(GLenum target, GLenum pname, GLint *params);
GL_APICALL void GL_APIENTRY glGetTexParameterIuivEXT(GLenum target, GLenum pname, GLuint *params);
GL_APICALL void GL_APIENTRY glSamplerParameterIivEXT(GLuint sampler, GLenum pname, const GLint *params);
GL_APICALL void GL_APIENTRY glSamplerParameterIuivEXT(GLuint sampler, GLenum pname, const GLuint *params);
GL_APICALL void GL_APIENTRY glGetSamplerParameterIivEXT(GLuint sampler, GLenum pname, GLint *params);
GL_APICALL void GL_APIENTRY glGetSamplerParameterIuivEXT(GLuint sampler, GLenum pname, GLuint *params);
#endif
typedef void (GL_APIENTRYP PFNGLTEXPARAMETERIIVEXTPROC) (GLenum target, GLenum pname, const GLint *params);
typedef void (GL_APIENTRYP PFNGLTEXPARAMETERIUIVEXTPROC) (GLenum target, GLenum pname, const GLuint* params);
typedef void (GL_APIENTRYP PFNGLGETTEXPARAMETERIIVEXTPROC) (GLenum target, GLenum pname, GLint *params);
typedef void (GL_APIENTRYP PFNGLGETTEXPARAMETERIUIVEXTPROC) (GLenum target, GLenum pname, GLuint *params);
typedef void (GL_APIENTRYP PFNGLSAMPLERPARAMETERIIVEXTPROC) (GLuint sampler, GLenum pname, const GLint *params);
typedef void (GL_APIENTRYP PFNGLSAMPLERPARAMETERIUIVEXTPROC) (GLuint sampler, GLenum pname, const GLuint *params);
typedef void (GL_APIENTRYP PFNGLGETSAMPLERPARAMETERIIVEXTPROC) (GLuint sampler, GLenum pname, GLint *params);
typedef void (GL_APIENTRYP PFNGLGETSAMPLERPARAMETERIUIVEXTPROC) (GLuint sampler, GLenum pname, GLuint *params);
#endif

/* GL_QCOM_texture_format_ARGB */
#ifndef GL_QCOM_texture_format_ARGB
#define GL_QCOM_texture_format_ARGB 1
#define GL_ARGB_QCOM                                    0x8FA3
#define GL_UNSIGNED_SHORT_1_5_5_5_QCOM                  0x8FA2
#endif

/* GL_QCOM_texture_format_XRGB */
#ifndef GL_QCOM_texture_format_XRGB
#define GL_QCOM_texture_format_XRGB 1
#define GL_XRGB_QCOM                                    0x8FA1
#define GL_UNSIGNED_SHORT_1_5_5_5_QCOM                  0x8FA2
#endif

/* GL_QCOM_memory_extension */
#ifndef GL_QCOM_memory_extension
#define GL_QCOM_memory_extension 1

typedef khronos_int64_t     GLint64QCOM;
#ifndef _LINUX
#ifdef  GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glGetMemoryStatsQCOM(GLenum pname, GLenum usage, GLint64QCOM* param);
GL_APICALL void GL_APIENTRY glGetSizedMemoryStatsQCOM(GLsizei maxcount, GLsizei* count, GLint64QCOM *buf);
#endif
#endif

#define GL_VMEM_TOTAL_AVAILABLE_QCOM                     0x8FA4
#define GL_VMEM_USED_THIS_PROCESS_QCOM                   0x8FA5
#define GL_VMEM_USED_ALL_PROCESSES_QCOM                  0x8FA6
#define GL_VMEM_LARGEST_CONTIGUOUS_BLOCK_REMAINING_QCOM  0x8FA7
#define GL_HEAPMEM_TOTAL_AVAILABLE_QCOM                  0x8FA8
#define GL_HEAPMEM_USED_THIS_PROCESS_QCOM                0x8FA9

#define GL_CLIENT_VERTEX_ARRAY_QCOM                      0x8FAA
#define GL_OTHER_GL_USAGE_QCOM                           0x8FAB
#define GL_EGL_USAGE_QCOM                                0x8FAC
#define GL_2D_USAGE_QCOM                                 0x8FAD
#define GL_OTHER_USAGE_QCOM                              0x8FAE
#define GL_ALL_USAGE_QCOM                                0x8FAF

typedef void (GL_APIENTRYP PFNGLGETMEMORYSTATSQCOM) (GLenum pname, GLenum usage, GLint64QCOM* param);
typedef void (GL_APIENTRYP PFNGLGETSIZEDMEMORYSTATSQCOM) (GLsizei maxcount, GLsizei* count, GLint64QCOM* param);
#endif  //GL_QCOM_memory_extension

/* GL_EXT_texture_compression_s3tc */
#ifndef GL_EXT_texture_compression_s3tc
#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT                  0x83F0
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT                 0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT                 0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT                 0x83F3
#endif

/* GL_QCOM_get_device_hw_flags */
#ifndef GL_QCOM_get_device_hw_flags
#define GL_QCOM_get_device_hw_flags 1
#define GL_GET_DEVICE_HW_FLAGS_QCOM                      0x8FB5
#define GL_HW_FLAG_HW_NONE_QCOM                          0x0000           
#define GL_HW_FLAG_HW_FOG_QCOM                           0x0001           
#define GL_HW_FLAG_HW_FRAGDEPTH_QCOM                     0x0002
#define GL_HW_FLAG_HW_3DC_TEXFORMAT_QCOM                 0x0004
#define GL_HW_FLAG_HW_PATCH_FRAGCOORD_QCOM				 0x0008
#define GL_HW_FLAG_HW_INSTANCING_QCOM					 0x0010
#define GL_HW_FLAG_HW_FRAGCOORD_ZW_QCOM					 0x0020
#define GL_HW_FLAG_HW_SRGB_QCOM							 0x0040
#define GL_HW_FLAG_HW_USERCLIPPLANES_QCOM				 0x0080
#define GL_HW_FLAG_HW_TYPEWRITER_QCOM					 0x0100
#define GL_HW_FLAG_HW_DUAL_SOURCE_BLEND_QCOM			 0x0200
#define GL_HW_FLAG_HW_HALTI_QCOM						 0x0400
#define GL_HW_FLAG_HW_INTEGERS_QCOM                      0x0800
#define GL_HW_FLAG_HW_ANISO_QCOM                         0x1000
#define GL_HW_FLAG_HW_ASTC_TEXFORMAT_QCOM                0x2000
#define GL_HW_FLAG_HW_8K_TEX_AND_RENDER                  0x4000
#endif

/* R8 and RG88 texture definitions. */

#ifndef GL_QCOM_rg_texture_support
#define GL_QCOM_rg_texture_support 1
#define GL_RED_QCOM                                      0x1903
#define GL_RG_QCOM                                       0x8227
#endif

/* GL_XXX_draw_instanced */
#ifndef GL_XXX_draw_instanced
#define GL_XXX_draw_instanced 1
#define GL_VERTEX_ATTRIB_ARRAY_DIVISOR                   0x88FE
#ifdef  GL_GLEXT_PROTOTYPES
GL_APICALL void         GL_APIENTRY glDrawArraysInstancedXXX (GLenum mode, GLint first, GLsizei count, GLsizei primcount);
GL_APICALL void         GL_APIENTRY glDrawElementsInstancedXXX (GLenum mode, GLsizei count, GLenum type, const GLvoid* indices, GLsizei primcount);
#endif
#endif

/* GL_OVR_multiview_multisampled_render_to_texture */
#ifndef GL_OVR_multiview_multisampled_render_to_texture
#define GL_OVR_multiview_multisampled_render_to_texture 1
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glFramebufferTextureMultisampleMultiviewOVR(GLenum target, GLenum attachment, GLuint texture, GLint level, GLsizei samples, GLint baseView, GLsizei numViews);
#endif
typedef void (GL_APIENTRY PFNGLFRAMEBUFFERTEXTUREMULTISAMPLEMULTIVIEWOVR) (GLenum target, GLenum attachment, GLuint texture, GLint level, GLsizei samples, GLint baseView, GLsizei numViews);
#endif

/* GL_QCOM_overlapping_blit */
#ifndef GL_QCOM_overlapping_blit
#define GL_QCOM_overlapping_blit 1
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glBlitOverlapQCOM(GLint dest_x, GLint dest_y, GLint src_x, GLint src_y, GLint src_width, GLint src_height);
#endif
typedef void (GL_APIENTRYP PFNGLBLITOVERLAPQCOM)(GLint dest_x, GLint dest_y, GLint src_x, GLint src_y, GLint src_width, GLint src_height);
#endif

/* GL_QCOM_get_shader_stats */
#ifndef GL_QCOM_get_shader_stats
#define GL_QCOM_get_shader_stats 1
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glGetShaderStatsQCOM (GLuint shader, GLsizei maxLength, GLsizei *length, GLuint *data);
#endif
typedef void (GL_APIENTRYP PFNGLGETSHADERSTATSQCOMPROC) (GLuint shader, GLsizei maxLength, GLsizei *length, GLuint *data);
#endif

/* GL_QCOM_get_internal */
#ifndef GL_QCOM_get_internal
#define GL_QCOM_get_internal 1
#define GL_EXT_GET_VERTEX_ARRAY_OBJECTS         0x8BEC
#define	GL_EXT_GET_BINNING_CONFIGURATION		0x8BED
#define GL_EXT_GET_TRANSFORM_FEEDBACK_OBJECTS	0x8BEE
#define GL_EXT_GET_BINNING_GROUP_CONFIGURATION  0x88EF

#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL GLenum GL_APIENTRY glExtGetQCOM (const GLenum *param_list, GLsizei buffer_size, void *out_buffer, GLsizei *out_bytes_required);
#endif
typedef GLenum (GL_APIENTRYP PFNGLEXTGETQCOMPROC) (const GLenum *param_list, GLsizei buffer_size, void *out_buffer, GLsizei *out_bytes_required);
#endif

/* GL_QCOM_extended_get3 */
#ifndef GL_QCOM_extended_get3
#define GL_QCOM_extended_get3
#define GL_BINNINGCONFIG_BIN_WIDTH_QCOM         0x8BDD
#define GL_BINNINGCONFIG_BIN_HEIGHT_QCOM        0x8BDE
#define GL_BINNINGCONFIG_NUM_BINS_X_QCOM        0x8BDF
#define GL_BINNINGCONFIG_NUM_BINS_Y_QCOM        0x8BE0
#define GL_BINNINGCONFIG_BIN_X_OFFSET_QCOM      0x8BE1
#define GL_BINNINGCONFIG_BIN_Y_OFFSET_QCOM      0x8BE2
#define GL_TEXTURE_EXTERNAL_EGL_IMAGE_QCOM      0x8BE8
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glExtGetBinningConfigParamivQCOM (GLenum pname, GLint *params);
#endif
typedef void (GL_APIENTRYP PFNGLEXTGETBINNINGCONFIGPARAMETERIVQCOMPROC) (GLenum pname, GLint *params);
#endif

/* GL_QCOM_get_samplers */
#ifndef GL_QCOM_get_samplers
#define GL_QCOM_get_samplers 1
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glExtGetSamplersQCOM (GLuint *samplers, GLint maxSamplers, GLint *numSamplers);
#endif
typedef void (GL_APIENTRYP PFNGLEXTGETSAMPLERSQCOMPROC) (GLuint *samplers, GLint maxSamplers, GLint *numSamplers);
#endif

#ifndef GL_FRAMEBUFFER_INCOMPLETE_VIEW_TARGETS_OVR
#define GL_FRAMEBUFFER_INCOMPLETE_VIEW_TARGETS_OVR 0x9633
#endif

#define GL_XXX_uniform_buffer_objects 1

//---------------------------------------------------------------------
// UBOs.
//
#ifdef GL_XXX_uniform_buffer_objects

#define GL_UNIFORM_BUFFER                                   0x8A11
#define GL_UNIFORM_BUFFER_BINDING                           0x8A28
#define GL_UNIFORM_BUFFER_START                             0x8A29
#define GL_UNIFORM_BUFFER_SIZE                              0x8A2A
#define GL_MAX_VERTEX_UNIFORM_BLOCKS                        0x8A2B
#define GL_MAX_GEOMETRY_UNIFORM_BLOCKS                      0x8A2C
#define GL_MAX_FRAGMENT_UNIFORM_BLOCKS                      0x8A2D
#define GL_MAX_COMBINED_UNIFORM_BLOCKS                      0x8A2E
#define GL_MAX_UNIFORM_BUFFER_BINDINGS                      0x8A2F
#define GL_MAX_UNIFORM_BLOCK_SIZE                           0x8A30
#define GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS           0x8A31
#define GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS         0x8A32
#define GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS         0x8A33
#define GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT                  0x8A34
#define GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH             0x8A35
#define GL_ACTIVE_UNIFORM_BLOCKS                            0x8A36
#define GL_UNIFORM_TYPE                                     0x8A37
#define GL_UNIFORM_SIZE                                     0x8A38
#define GL_UNIFORM_NAME_LENGTH                              0x8A39
#define GL_UNIFORM_BLOCK_INDEX                              0x8A3A
#define GL_UNIFORM_OFFSET                                   0x8A3B
#define GL_UNIFORM_ARRAY_STRIDE                             0x8A3C
#define GL_UNIFORM_MATRIX_STRIDE                            0x8A3D
#define GL_UNIFORM_IS_ROW_MAJOR                             0x8A3E
#define GL_UNIFORM_BLOCK_BINDING                            0x8A3F
#define GL_UNIFORM_BLOCK_DATA_SIZE                          0x8A40
#define GL_UNIFORM_BLOCK_NAME_LENGTH                        0x8A41
#define GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS                    0x8A42
#define GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES             0x8A43
#define GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER        0x8A44
#define GL_UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER      0x8A45
#define GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER      0x8A46
#define GL_INVALID_INDEX                                    0xFFFFFFFFu

#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void   GL_APIENTRY GetUniformIndicesXXX          (GLuint program, GLsizei uniformCount, const char** uniformNames, GLuint* uniformIndices);
GL_APICALL void   GL_APIENTRY GetActiveUniformsivXXX        (GLuint program, GLsizei uniformCount, const GLuint* uniformIndices, GLenum pname, GLint* params);
GL_APICALL void   GL_APIENTRY GetActiveUniformNameXXX       (GLuint program, GLuint  uniformIndex, GLsizei bufSize, GLsizei* length, char* uniformName);
GL_APICALL GLuint GL_APIENTRY GetUniformBlockIndexXXX       (GLuint program, const char* uniformBlockName);
GL_APICALL void   GL_APIENTRY GetActiveUniformBlockivXXX    (GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint* params);
GL_APICALL void   GL_APIENTRY GetActiveUniformBlockNameXXX  (GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei* length, char* uniformBlockName);
GL_APICALL void   GL_APIENTRY UniformBlockBindingXXX        (GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding);
GL_APICALL void   GL_APIENTRY glUniformBlockBindingXXX      (GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding);
#endif

#if 0
typedef void (GL_APIENTRYP PFNGETUNIFORMINDICESPROC)         (GLuint program, GLsizei uniformCount, const char** uniformNames, GLuint* uniformIndices);
typedef void (GL_APIENTRYP PFNGETACTIVEUNIFORMSIVPROC)       (GLuint program, GLsizei uniformCount, const GLuint* uniformIndices, GLenum pname, int* params);
typedef void (GL_APIENTRYP PFNGETACTIVEUNIFORMNAMEPROC)      (GLuint program, GLuint  uniformIndex, GLsizei bufSize, GLsizei* length, char* uniformName);
typedef uint (GL_APIENTRYP PFNGETUNIFORMBLOCKINDEXPROC)      (GLuint program, const char* uniformBlockName);
typedef void (GL_APIENTRYP PFNGETACTIVEUNIFORMBLOCKIVPROC)   (GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint* params);
typedef void (GL_APIENTRYP PFNGETACTIVEUNIFORMBLOCKNAMEPROC) (GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei* length, char* uniformBlockName);
typedef void (GL_APIENTRYP PFNUNIFORMBLOCKBINDINGPROC)       (GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding);
typedef void (GL_APIENTRYP PFNBINDBUFFERRANGEPROC)           (enum target,    GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);
typedef void (GL_APIENTRYP PFNBINDBUFFERBASEPROC)            (enum target,    GLuint index, GLuint buffer);
typedef void (GL_APIENTRYP PFNGETINTEGERI_VPROC)             (enum target,    GLuint index, GLint* data);
#endif

#endif   /* GL_XXX_uniform_buffer_objects */

/* GL_QCOM_user_clip_planes */
#ifndef GL_QCOM_user_clip_planes
#define GL_QCOM_user_clip_planes 1
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glClipPlanefQCOM(GLenum p, GLfloat equation[4]);
#endif
typedef void (GL_APIENTRYP PFNGLCLIPPLANEFQCOM)(GLenum p, GLfloat equation[4]);
#endif

#ifndef GL_EXT_clip_cull_distance
#define GL_EXT_clip_cull_distance 1
#define GL_MAX_CLIP_DISTANCES_EXT                       0x0D32
#define GL_MAX_CULL_DISTANCES_EXT                       0x82F9
#define GL_MAX_COMBINED_CLIP_AND_CULL_DISTANCES_EXT     0x82FA

#define GL_CLIP_DISTANCE0_EXT             0x3000
#define GL_CLIP_DISTANCE1_EXT             0x3001
#define GL_CLIP_DISTANCE2_EXT             0x3002
#define GL_CLIP_DISTANCE3_EXT             0x3003
#define GL_CLIP_DISTANCE4_EXT             0x3004
#define GL_CLIP_DISTANCE5_EXT             0x3005
#define GL_CLIP_DISTANCE6_EXT             0x3006
#define GL_CLIP_DISTANCE7_EXT             0x3007

#define GL_MAX_CLIP_PLANES_QCOM         0x0D32
#define GL_CLIP_PLANE0_QCOM             0x3000
#define GL_CLIP_PLANE1_QCOM             0x3001
#define GL_CLIP_PLANE2_QCOM             0x3002
#define GL_CLIP_PLANE3_QCOM             0x3003
#define GL_CLIP_PLANE4_QCOM             0x3004
#define GL_CLIP_PLANE5_QCOM             0x3005
#endif // GL_EXT_clip_cull_distance

#ifndef GL_EXT_clip_control
#define GL_EXT_clip_control 1
#define GL_LOWER_LEFT_EXT          0x8CA1
#define GL_UPPER_LEFT_EXT          0x8CA2
#define GL_CLIP_ORIGIN_EXT         0x935C
#define GL_CLIP_DEPTH_MODE_EXT     0x935D
#define GL_NEGATIVE_ONE_TO_ONE_EXT 0x935E
#define GL_ZERO_TO_ONE_EXT         0x935F
typedef void (GL_APIENTRYP PFNGLCLIPCONTROLEXTPROC) (GLenum origin, GLenum depth);
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glClipControlEXT (GLenum origin, GLenum depth);
#endif
#endif /* GL_EXT_clip_control */

#ifndef GL_QCOM_blend_func_extended
#define GL_QCOM_blend_func_extended 1
#define GL_SRC1_COLOR_QCOM                    0x88F9
#define GL_SRC1_ALPHA_QCOM                    0x8589
#define GL_ONE_MINUS_SRC1_COLOR_QCOM          0x88FA
#define GL_ONE_MINUS_SRC1_ALPHA_QCOM          0x88FB
#define GL_MAX_DUAL_SOURCE_DRAW_BUFFERS_QCOM  0x88FC
#endif

/* GL_QCOM_YUV_target */
#ifndef GL_QCOM_YUV_target
#define GL_QCOM_YUV_target 1
#define GL_SAMPLER_601Y2R_QCOM                              0x8BE3
#define GL_SAMPLER_709Y2R_QCOM                              0x8BE4
#define GL_SAMPLER_601R2Y_QCOM                              0x8BE5
#define GL_SAMPLER_709R2Y_QCOM                              0x8BE6
#define GL_SAMPLER_Y2Y_QCOM                                 0x8BE7
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glFramebufferTexture2DExternalQCOM(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
GL_APICALL void GL_APIENTRY glFramebufferRenderbufferExternalQCOM(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
#endif
#define GL_RENDERBUFFER_EXTERNAL_QCOM                       0x8BE9
#define GL_RENDERBUFFER_BINDING_EXTERNAL_QCOM               0x8BEA
#endif

/* GL_EXT_YUV_target */
#ifndef GL_YUV_EXT
#define GL_YUV_EXT                                          0x8BE5
#endif
#ifndef GL_Y_EXT
#define GL_Y_EXT                                            0x8BE6
#endif

#ifdef QCOM_tiled_rendering
#define GL_COLOR_BUFFER_BIT0_QCOM       0x00000001
#define GL_COLOR_BUFFER_BIT1_QCOM       0x00000002
#define GL_COLOR_BUFFER_BIT2_QCOM       0x00000004
#define GL_COLOR_BUFFER_BIT3_QCOM       0x00000008
#define GL_COLOR_BUFFER_BIT4_QCOM       0x00000010
#define GL_COLOR_BUFFER_BIT5_QCOM       0x00000020
#define GL_COLOR_BUFFER_BIT6_QCOM       0x00000040
#define GL_COLOR_BUFFER_BIT7_QCOM       0x00000080
#define GL_DEPTH_BUFFER_BIT0_QCOM       0x00000100
#define GL_DEPTH_BUFFER_BIT1_QCOM       0x00000200
#define GL_DEPTH_BUFFER_BIT2_QCOM       0x00000400
#define GL_DEPTH_BUFFER_BIT3_QCOM       0x00000800
#define GL_DEPTH_BUFFER_BIT4_QCOM       0x00001000
#define GL_DEPTH_BUFFER_BIT5_QCOM       0x00002000
#define GL_DEPTH_BUFFER_BIT6_QCOM       0x00004000
#define GL_DEPTH_BUFFER_BIT7_QCOM       0x00008000
#define GL_STENCIL_BUFFER_BIT0_QCOM     0x00010000
#define GL_STENCIL_BUFFER_BIT1_QCOM     0x00020000
#define GL_STENCIL_BUFFER_BIT2_QCOM     0x00040000
#define GL_STENCIL_BUFFER_BIT3_QCOM     0x00080000
#define GL_STENCIL_BUFFER_BIT4_QCOM     0x00100000
#define GL_STENCIL_BUFFER_BIT5_QCOM     0x00200000
#define GL_STENCIL_BUFFER_BIT6_QCOM     0x00400000
#define GL_STENCIL_BUFFER_BIT7_QCOM     0x00800000
#define GL_MULTISAMPLE_BUFFER_BIT0_QCOM 0x01000000
#define GL_MULTISAMPLE_BUFFER_BIT1_QCOM 0x02000000
#define GL_MULTISAMPLE_BUFFER_BIT2_QCOM 0x04000000
#define GL_MULTISAMPLE_BUFFER_BIT3_QCOM 0x08000000
#define GL_MULTISAMPLE_BUFFER_BIT4_QCOM 0x10000000
#define GL_MULTISAMPLE_BUFFER_BIT5_QCOM 0x20000000
#define GL_MULTISAMPLE_BUFFER_BIT6_QCOM 0x40000000
#define GL_MULTISAMPLE_BUFFER_BIT7_QCOM 0x80000000
#endif

/* GL_QCOM_8k_textures */
#ifndef GL_QCOM_8k_textures
#define GL_QCOM_8k_textures
#define GL_ENABLE_8K_TEXTURES_AND_RENDERING_QCOM                0x8FBC
#endif

/* GL_QCOM_GPUSCOPE_API */
#ifndef GL_QCOM_GPUSCOPE_API
#define GL_QCOM_GPUSCOPE_API 1
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glSetGpuScopeCounterQCOM (GLboolean enable, const GLchar *groupname, GLuint counterid, GLuint *size);
GL_APICALL void GL_APIENTRY glGetGpuScopeCounterDataQCOM (GLvoid* data, GLuint size, GLboolean block);
GL_APICALL const GLchar* GL_APIENTRY glGetGpuScopePhaseInfoQCOM (GLuint phase_id);
#endif
typedef void (GL_APIENTRYP PFNGLSETGPUSCOPECOUNTERQCOMPROC) (GLboolean enable, const GLchar *groupname, GLuint counterid, GLuint *size);
typedef void (GL_APIENTRYP PFNGLGETGPUSCOPECOUNTERDATAQCOMPROC) (GLvoid* data, GLuint size, GLboolean block);
typedef const GLchar* (GL_APIENTRYP PFNGLGETGPUSCOPEPHASEINFOQCOMPROC) (GLuint phase_id);
#endif

/* GL_EXT_protected_texture */
#ifndef GL_EXT_protected_textures
#define GL_EXT_protected_textures
#define GL_TEXTURE_PROTECTED_EXT                                0x8BFA
#define GL_CONTEXT_FLAG_PROTECTED_CONTENT_BIT_EXT               0x00000010
#endif

#ifndef GL_ARB_texture_barrier
#define GL_ARB_texture_barrier 1
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glTextureBarrier();
#endif
typedef void (GL_APIENTRYP PFNGLTEXTUREBARRIER)();
#endif

#ifndef GL_QCOM_shader_framebuffer_fetch_non_coherent
#define GL_QCOM_shader_framebuffer_fetch_non_coherent 1
#define GL_FRAMEBUFFER_FETCH_NONCOHERENT_QCOM 0x96A2
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glFramebufferFetchBarrierQCOM();
#endif
typedef void (GL_APIENTRYP PFNGLFRAMEBUFFERFETCHBARRIERQCOM)();
#endif

#ifndef GL_EXT_memory_object
#define GL_EXT_memory_object 1
#define GL_TEXTURE_TILING_EXT             0x9580
#define GL_DEDICATED_MEMORY_OBJECT_EXT    0x9581
#define GL_PROTECTED_MEMORY_OBJECT_EXT    0x959B
#define GL_NUM_TILING_TYPES_EXT           0x9582
#define GL_TILING_TYPES_EXT               0x9583
#define GL_OPTIMAL_TILING_EXT             0x9584
#define GL_LINEAR_TILING_EXT              0x9585
#define GL_NUM_DEVICE_UUIDS_EXT           0x9596
#define GL_DEVICE_UUID_EXT                0x9597
#define GL_DRIVER_UUID_EXT                0x9598
#define GL_UUID_SIZE_EXT                  16
typedef void (GL_APIENTRYP PFNGLGETUNSIGNEDBYTEVEXTPROC) (GLenum pname, GLubyte *data);
typedef void (GL_APIENTRYP PFNGLGETUNSIGNEDBYTEI_VEXTPROC) (GLenum target, GLuint index, GLubyte *data);
typedef void (GL_APIENTRYP PFNGLDELETEMEMORYOBJECTSEXTPROC) (GLsizei n, const GLuint *memoryObjects);
typedef GLboolean (GL_APIENTRYP PFNGLISMEMORYOBJECTEXTPROC) (GLuint memoryObject);
typedef void (GL_APIENTRYP PFNGLCREATEMEMORYOBJECTSEXTPROC) (GLsizei n, GLuint *memoryObjects);
typedef void (GL_APIENTRYP PFNGLMEMORYOBJECTPARAMETERIVEXTPROC) (GLuint memoryObject, GLenum pname, const GLint *params);
typedef void (GL_APIENTRYP PFNGLGETMEMORYOBJECTPARAMETERIVEXTPROC) (GLuint memoryObject, GLenum pname, GLint *params);
typedef void (GL_APIENTRYP PFNGLTEXSTORAGEMEM2DEXTPROC) (GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLuint memory, GLuint64 offset);
typedef void (GL_APIENTRYP PFNGLTEXSTORAGEMEM2DMULTISAMPLEEXTPROC) (GLenum target, GLsizei samples, GLenum internalFormat, GLsizei width, GLsizei height, GLboolean fixedSampleLocations, GLuint memory, GLuint64 offset);
typedef void (GL_APIENTRYP PFNGLTEXSTORAGEMEM3DEXTPROC) (GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLuint memory, GLuint64 offset);
typedef void (GL_APIENTRYP PFNGLTEXSTORAGEMEM3DMULTISAMPLEEXTPROC) (GLenum target, GLsizei samples, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedSampleLocations, GLuint memory, GLuint64 offset);
typedef void (GL_APIENTRYP PFNGLBUFFERSTORAGEMEMEXTPROC) (GLenum target, GLsizeiptr size, GLuint memory, GLuint64 offset);
typedef void (GL_APIENTRYP PFNGLTEXTURESTORAGEMEM2DEXTPROC) (GLuint texture, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLuint memory, GLuint64 offset);
typedef void (GL_APIENTRYP PFNGLTEXTURESTORAGEMEM2DMULTISAMPLEEXTPROC) (GLuint texture, GLsizei samples, GLenum internalFormat, GLsizei width, GLsizei height, GLboolean fixedSampleLocations, GLuint memory, GLuint64 offset);
typedef void (GL_APIENTRYP PFNGLTEXTURESTORAGEMEM3DEXTPROC) (GLuint texture, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLuint memory, GLuint64 offset);
typedef void (GL_APIENTRYP PFNGLTEXTURESTORAGEMEM3DMULTISAMPLEEXTPROC) (GLuint texture, GLsizei samples, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedSampleLocations, GLuint memory, GLuint64 offset);
typedef void (GL_APIENTRYP PFNGLNAMEDBUFFERSTORAGEMEMEXTPROC) (GLuint buffer, GLsizeiptr size, GLuint memory, GLuint64 offset);
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glGetUnsignedBytevEXT (GLenum pname, GLubyte *data);
GL_APICALL void GL_APIENTRY glGetUnsignedBytei_vEXT (GLenum target, GLuint index, GLubyte *data);
GL_APICALL void GL_APIENTRY glDeleteMemoryObjectsEXT (GLsizei n, const GLuint *memoryObjects);
GL_APICALL GLboolean GL_APIENTRY glIsMemoryObjectEXT (GLuint memoryObject);
GL_APICALL void GL_APIENTRY glCreateMemoryObjectsEXT (GLsizei n, GLuint *memoryObjects);
GL_APICALL void GL_APIENTRY glMemoryObjectParameterivEXT (GLuint memoryObject, GLenum pname, const GLint *params);
GL_APICALL void GL_APIENTRY glGetMemoryObjectParameterivEXT (GLuint memoryObject, GLenum pname, GLint *params);
GL_APICALL void GL_APIENTRY glTexStorageMem2DEXT (GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLuint memory, GLuint64 offset);
GL_APICALL void GL_APIENTRY glTexStorageMem2DMultisampleEXT (GLenum target, GLsizei samples, GLenum internalFormat, GLsizei width, GLsizei height, GLboolean fixedSampleLocations, GLuint memory, GLuint64 offset);
GL_APICALL void GL_APIENTRY glTexStorageMem3DEXT (GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLuint memory, GLuint64 offset);
GL_APICALL void GL_APIENTRY glTexStorageMem3DMultisampleEXT (GLenum target, GLsizei samples, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedSampleLocations, GLuint memory, GLuint64 offset);
GL_APICALL void GL_APIENTRY glBufferStorageMemEXT (GLenum target, GLsizeiptr size, GLuint memory, GLuint64 offset);
GL_APICALL void GL_APIENTRY glTextureStorageMem2DEXT (GLuint texture, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLuint memory, GLuint64 offset);
GL_APICALL void GL_APIENTRY glTextureStorageMem2DMultisampleEXT (GLuint texture, GLsizei samples, GLenum internalFormat, GLsizei width, GLsizei height, GLboolean fixedSampleLocations, GLuint memory, GLuint64 offset);
GL_APICALL void GL_APIENTRY glTextureStorageMem3DEXT (GLuint texture, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLuint memory, GLuint64 offset);
GL_APICALL void GL_APIENTRY glTextureStorageMem3DMultisampleEXT (GLuint texture, GLsizei samples, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedSampleLocations, GLuint memory, GLuint64 offset);
GL_APICALL void GL_APIENTRY glNamedBufferStorageMemEXT (GLuint buffer, GLsizeiptr size, GLuint memory, GLuint64 offset);
#endif
#endif /* GL_EXT_memory_object */

#ifndef GL_EXT_memory_object_fd
#define GL_EXT_memory_object_fd 1
#define GL_HANDLE_TYPE_OPAQUE_FD_EXT      0x9586
typedef void (GL_APIENTRYP PFNGLIMPORTMEMORYFDEXTPROC) (GLuint memory, GLuint64 size, GLenum handleType, GLint fd);
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glImportMemoryFdEXT (GLuint memory, GLuint64 size, GLenum handleType, GLint fd);
#endif
#endif /* GL_EXT_memory_object_fd */

#ifndef GL_QCOM_texture_foveated
#define GL_QCOM_texture_foveated 1
#define GL_FOVEATION_ENABLE_BIT_QCOM                    0x00000001
#define GL_FOVEATION_SCALED_BIN_METHOD_BIT_QCOM         0x00000002
#define GL_TEXTURE_FOVEATED_FEATURE_BITS_QCOM           0x8BFB
#define GL_TEXTURE_FOVEATED_MIN_PIXEL_DENSITY_QCOM      0x8BFC
#define GL_TEXTURE_FOVEATED_CUTOFF_DENSITY_QCOM         0x96A0
#define GL_TEXTURE_FOVEATED_FEATURE_QUERY_QCOM          0x8BFD
#define GL_TEXTURE_FOVEATED_NUM_FOCAL_POINTS_QUERY_QCOM 0x8BFE
#define GL_FRAMEBUFFER_INCOMPLETE_FOVEATION_QCOM        0x8BFF
typedef void (GL_APIENTRYP PFNGLTEXTUREFOVEATIONPARAMETERSQCOMPROC) (GLuint texture, GLuint layer, GLuint focalPoint, GLfloat focalX, GLfloat focalY, GLfloat gainX, GLfloat gainY, GLfloat foveaArea);
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glTextureFoveationParametersQCOM (GLuint texture, GLuint layer, GLuint focalPoint, GLfloat focalX, GLfloat focalY, GLfloat gainX, GLfloat gainY, GLfloat foveaArea);
#endif
#endif /* GL_QCOM_texture_foveated */

#ifndef GL_QCOM_validate_shader_binary
#define GL_QCOM_validate_shader_binary 1
#define GL_VALIDATE_SHADER_BINARY_QCOM                     0x96A3
#define GL_VALIDATE_SHADER_CORRUPT_VERTEX_DCB_QCOM         0x00000001
#define GL_VALIDATE_SHADER_CORRUPT_BINNING_VERTEX_DCB_QCOM 0x00000002
#define GL_VALIDATE_SHADER_CORRUPT_FRAGMENT_DCB_QCOM       0x00000004
typedef void (GL_APIENTRYP PFNGLMODIFYDCBQCOMPROC) (GLuint program, GLuint corruptArea);
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glModifyDCBQCOM (GLuint program, GLuint corruptArea);
#endif
#endif /*GL_QCOM_validate_shader_binary*/

#ifndef GL_QCOM_persistent_workload
#define GL_QCOM_persistent_workload 1
typedef void (GL_APIENTRYP PFNLPERSISTENTWORKLOADSUBMITQCOMPROC) (GLboolean waitForCompletion);
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glPersistentWorkloadSubmitQCOM (GLboolean waitForCompletion);
#endif
#endif /* GL_QCOM_persistent_workload */

#ifndef GL_QCOM_motion_estimation
#define GL_QCOM_motion_estimation 1
#define GL_MOTION_ESTIMATION_SEARCH_BLOCK_X_QCOM 0x8C90
#define GL_MOTION_ESTIMATION_SEARCH_BLOCK_Y_QCOM 0x8C91
typedef void (GL_APIENTRYP PFNGLTEXESTIMATEMOTIONQCOMPROC) (GLuint ref, GLuint target, GLuint output);
typedef void (GL_APIENTRYP PFNGLTEXESTIMATEMOTIONREGIONSQCOMPROC) (GLuint ref, GLuint target, GLuint output, GLuint mask);
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glTexEstimateMotionQCOM(GLuint ref, GLuint target, GLuint output);
GL_APICALL void GL_APIENTRY glTexEstimateMotionRegionsQCOM(GLuint ref, GLuint target, GLuint output, GLuint mask);
#endif
#endif /* GL_QCOM_motion_estimation */

#ifndef GL_QCOM_frame_extrapolation
#define GL_QCOM_frame_extrapolation 1
typedef void (GL_APIENTRYP PFNGLEXTRAPOLATETEX2DQCOMPROC) (GLuint src1, GLuint src2, GLuint output, GLfloat scaleFactor);
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glExtrapolateTex2DQCOM(GLuint src1, GLuint src2, GLuint output, GLfloat scaleFactor);
#endif
#endif /* GL_QCOM_frame_extrapolation */


#ifdef __cplusplus
}
#endif

#endif /* __gl3extqcom_h_ */
