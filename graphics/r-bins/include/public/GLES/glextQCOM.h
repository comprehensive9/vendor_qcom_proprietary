#ifndef __glextQCOM_h_
#define __glextQCOM_h_
/*
** Copyright (c) 2009-2010, 2012 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#ifndef __glext_h_
#   include <GLES/glext.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* GL_AMD_EGL_image_external_layout_specifier */
#ifndef GL_AMD_EGL_image_external_layout_specifier
#define GL_AMD_EGL_image_external_layout_specifier 1
// YUV format
#define GL_YUV_AMD                                           0x8BC7
// YUV types
#define GL_YUYV_AMD                                          0x8BC8
#define GL_UYVY_AMD                                          0x8BC9
#define GL_NV21_AMD                                          0x8BCA
#define GL_YV12_AMD                                          0x8BCB
#define GL_NV12_TILED_AMD                                    0x8BCC
#define GL_NV12_QCOM                                         0x8FB9
// YUV samplers
#define GL_SAMPLER_EXTERNAL_YUYV_AMD                         0x8BCD
#define GL_SAMPLER_EXTERNAL_UYVY_AMD                         0x8BCE
#define GL_SAMPLER_EXTERNAL_NV21_AMD                         0x8BCF
#define GL_SAMPLER_EXTERNAL_YV12_AMD                         0x8BD0
#define GL_SAMPLER_EXTERNAL_NV12_TILED_AMD                   0x8BD1
#define GL_SAMPLER_EXTERNAL_NV12_QCOM                        0x8FBA
#endif

/* QCOM_binning_control */
#ifndef GL_QCOM_BINNING_CONTROL
#define GL_QCOM_BINNING_CONTROL 1
#define GL_BINNING_CONTROL_HINT_QCOM                         0x8FB0
#define GL_CPU_OPTIMIZED_QCOM                                0x8FB1
#define GL_GPU_OPTIMIZED_QCOM                                0x8FB2
#define GL_RENDER_DIRECT_TO_FRAMEBUFFER_QCOM                 0x8FB3
#endif

/* GL_QCOM_extended_get3 */
#ifndef GL_QCOM_extended_get3
#define GL_QCOM_extended_get3
#define GL_BINNINGCONFIG_BIN_WIDTH_QCOM                  0x8BDD
#define GL_BINNINGCONFIG_BIN_HEIGHT_QCOM                 0x8BDE
#define GL_BINNINGCONFIG_NUM_BINS_X_QCOM                 0x8BDF
#define GL_BINNINGCONFIG_NUM_BINS_Y_QCOM                 0x8BE0
#define GL_BINNINGCONFIG_BIN_X_OFFSET_QCOM               0x8BE1
#define GL_BINNINGCONFIG_BIN_Y_OFFSET_QCOM               0x8BE2
#ifdef GL_GLEXT_PROTOTYPES
GL_API void GL_APIENTRY glExtGetBinningConfigParamivQCOM (GLenum pname, GLint *params);
#endif
typedef void (GL_APIENTRYP PFNGLEXTGETBINNINGCONFIGPARAMETERIVQCOMPROC) (GLenum pname, GLint *params);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __glextQCOM_h_ */

