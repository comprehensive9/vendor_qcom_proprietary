/*==============================================================================
 Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
==============================================================================*/
#ifndef TITAN480_CPAS_TOP_SECURE_H
#define TITAN480_CPAS_TOP_SECURE_H

/*----------------------------------------------------------------------
        Offset and Mask
----------------------------------------------------------------------*/

#define CPAS_TOP_SECURE_REGS_FIRST 0x0

#define CPAS_TOP_SECURE_REGS_LAST 0x40

#define CPAS_TOP_SECURE_REGS_COUNT 0xd

#define regCPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL 0x0  /*register offset*/
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_0_CPHY_T0_CP_MASK 0x1
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_0_CPHY_T0_CP_SHIFT 0x0
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_0_CPHY_T1_CP_MASK 0x2
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_0_CPHY_T1_CP_SHIFT 0x1
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_0_CPHY_T2_CP_MASK 0x4
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_0_CPHY_T2_CP_SHIFT 0x2
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_0_DPHY_L0_CP_MASK 0x8
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_0_DPHY_L0_CP_SHIFT 0x3
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_0_DPHY_L1_CP_MASK 0x10
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_0_DPHY_L1_CP_SHIFT 0x4
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_0_DPHY_L2_CP_MASK 0x20
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_0_DPHY_L2_CP_SHIFT 0x5
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_0_DPHY_L3_CP_MASK 0x40
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_0_DPHY_L3_CP_SHIFT 0x6
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_1_CPHY_T0_CP_MASK 0x80
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_1_CPHY_T0_CP_SHIFT 0x7
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_1_CPHY_T1_CP_MASK 0x100
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_1_CPHY_T1_CP_SHIFT 0x8
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_1_CPHY_T2_CP_MASK 0x200
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_1_CPHY_T2_CP_SHIFT 0x9
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_1_DPHY_L0_CP_MASK 0x400
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_1_DPHY_L0_CP_SHIFT 0xa
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_1_DPHY_L1_CP_MASK 0x800
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_1_DPHY_L1_CP_SHIFT 0xb
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_1_DPHY_L2_CP_MASK 0x1000
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_1_DPHY_L2_CP_SHIFT 0xc
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_1_DPHY_L3_CP_MASK 0x2000
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_1_DPHY_L3_CP_SHIFT 0xd
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_2_CPHY_T0_CP_MASK 0x4000
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_2_CPHY_T0_CP_SHIFT 0xe
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_2_CPHY_T1_CP_MASK 0x8000
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_2_CPHY_T1_CP_SHIFT 0xf
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_2_CPHY_T2_CP_MASK 0x10000
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_2_CPHY_T2_CP_SHIFT 0x10
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_2_DPHY_L0_CP_MASK 0x20000
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_2_DPHY_L0_CP_SHIFT 0x11
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_2_DPHY_L1_CP_MASK 0x40000
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_2_DPHY_L1_CP_SHIFT 0x12
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_2_DPHY_L2_CP_MASK 0x80000
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_2_DPHY_L2_CP_SHIFT 0x13
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_2_DPHY_L3_CP_MASK 0x100000
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_2_DPHY_L3_CP_SHIFT 0x14
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_3_CPHY_T0_CP_MASK 0x200000
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_3_CPHY_T0_CP_SHIFT 0x15
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_3_CPHY_T1_CP_MASK 0x400000
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_3_CPHY_T1_CP_SHIFT 0x16
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_3_CPHY_T2_CP_MASK 0x800000
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_3_CPHY_T2_CP_SHIFT 0x17
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_3_DPHY_L0_CP_MASK 0x1000000
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_3_DPHY_L0_CP_SHIFT 0x18
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_3_DPHY_L1_CP_MASK 0x2000000
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_3_DPHY_L1_CP_SHIFT 0x19
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_3_DPHY_L2_CP_MASK 0x4000000
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_3_DPHY_L2_CP_SHIFT 0x1a
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_3_DPHY_L3_CP_MASK 0x8000000
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_CSIPHY_3_DPHY_L3_CP_SHIFT 0x1b
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_UNUSED0_MASK 0xf0000000
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_UNUSED0_SHIFT 0x1c

#define regCPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL 0x4  /*register offset*/
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_HVX_STREAMING_DIS_MASK 0x1
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_HVX_STREAMING_DIS_SHIFT 0x0
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_VIDEO_OUT_CP_EN_MASK 0x2
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_VIDEO_OUT_CP_EN_SHIFT 0x1
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_UNUSED0_MASK 0xc
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_UNUSED0_SHIFT 0x2
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_FD_OUT_CP_EN_MASK 0x10
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_FD_OUT_CP_EN_SHIFT 0x4
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_PIXEL_RAW_DUMP_OUT_CP_EN_MASK 0x20
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_PIXEL_RAW_DUMP_OUT_CP_EN_SHIFT 0x5
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_DUAL_PDAF_OUT_CP_EN_MASK 0x40
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_DUAL_PDAF_OUT_CP_EN_SHIFT 0x6
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_RDI_0_OUT_CP_EN_MASK 0x80
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_RDI_0_OUT_CP_EN_SHIFT 0x7
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_STATS_HDR_BH_CP_EN_MASK 0x100
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_STATS_HDR_BH_CP_EN_SHIFT 0x8
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_STATS_HDR_BE_CP_EN_MASK 0x200
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_STATS_HDR_BE_CP_EN_SHIFT 0x9
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_STATS_TINTLESS_BG_CP_EN_MASK 0x400
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_STATS_TINTLESS_BG_CP_EN_SHIFT 0xa
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_STATS_BF_CP_EN_MASK 0x800
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_STATS_BF_CP_EN_SHIFT 0xb
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_STATS_AWB_BG_CP_EN_MASK 0x1000
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_STATS_AWB_BG_CP_EN_SHIFT 0xc
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_STATS_BH_CP_EN_MASK 0x2000
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_STATS_BH_CP_EN_SHIFT 0xd
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_STATS_RS_CP_EN_MASK 0x4000
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_STATS_RS_CP_EN_SHIFT 0xe
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_STATS_CS_CP_EN_MASK 0x8000
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_STATS_CS_CP_EN_SHIFT 0xf
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_STATS_IHIST_CP_EN_MASK 0x10000
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_STATS_IHIST_CP_EN_SHIFT 0x10
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_TPG_DIS_MASK 0x20000
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_TPG_DIS_SHIFT 0x11
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_DISPLAY_FULL_OUT_CP_EN_MASK 0x40000
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_DISPLAY_FULL_OUT_CP_EN_SHIFT 0x12
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_UNUSED1_MASK 0x780000
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_UNUSED1_SHIFT 0x13
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_CAMIF_PD_CP_EN_MASK 0x800000
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_CAMIF_PD_CP_EN_SHIFT 0x17
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_LCR_CP_EN_MASK 0x1000000
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_LCR_CP_EN_SHIFT 0x18
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_RDI_1_OUT_CP_EN_MASK 0x2000000
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_RDI_1_OUT_CP_EN_SHIFT 0x19
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_RDI_2_OUT_CP_EN_MASK 0x4000000
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_RDI_2_OUT_CP_EN_SHIFT 0x1a
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_UNUSED2_MASK 0xf8000000
#define CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL_UNUSED2_SHIFT 0x1b

#define regCPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL 0x8  /*register offset*/
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_HVX_STREAMING_DIS_MASK 0x1
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_HVX_STREAMING_DIS_SHIFT 0x0
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_VIDEO_OUT_CP_EN_MASK 0x2
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_VIDEO_OUT_CP_EN_SHIFT 0x1
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_UNUSED0_MASK 0xc
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_UNUSED0_SHIFT 0x2
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_FD_OUT_CP_EN_MASK 0x10
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_FD_OUT_CP_EN_SHIFT 0x4
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_PIXEL_RAW_DUMP_OUT_CP_EN_MASK 0x20
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_PIXEL_RAW_DUMP_OUT_CP_EN_SHIFT 0x5
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_DUAL_PDAF_OUT_CP_EN_MASK 0x40
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_DUAL_PDAF_OUT_CP_EN_SHIFT 0x6
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_RDI_0_OUT_CP_EN_MASK 0x80
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_RDI_0_OUT_CP_EN_SHIFT 0x7
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_STATS_HDR_BH_CP_EN_MASK 0x100
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_STATS_HDR_BH_CP_EN_SHIFT 0x8
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_STATS_HDR_BE_CP_EN_MASK 0x200
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_STATS_HDR_BE_CP_EN_SHIFT 0x9
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_STATS_TINTLESS_BG_CP_EN_MASK 0x400
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_STATS_TINTLESS_BG_CP_EN_SHIFT 0xa
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_STATS_BF_CP_EN_MASK 0x800
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_STATS_BF_CP_EN_SHIFT 0xb
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_STATS_AWB_BG_CP_EN_MASK 0x1000
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_STATS_AWB_BG_CP_EN_SHIFT 0xc
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_STATS_BH_CP_EN_MASK 0x2000
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_STATS_BH_CP_EN_SHIFT 0xd
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_STATS_RS_CP_EN_MASK 0x4000
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_STATS_RS_CP_EN_SHIFT 0xe
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_STATS_CS_CP_EN_MASK 0x8000
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_STATS_CS_CP_EN_SHIFT 0xf
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_STATS_IHIST_CP_EN_MASK 0x10000
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_STATS_IHIST_CP_EN_SHIFT 0x10
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_TPG_DIS_MASK 0x20000
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_TPG_DIS_SHIFT 0x11
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_DISPLAY_FULL_OUT_CP_EN_MASK 0x40000
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_DISPLAY_FULL_OUT_CP_EN_SHIFT 0x12
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_UNUSED1_MASK 0x780000
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_UNUSED1_SHIFT 0x13
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_CAMIF_PD_CP_EN_MASK 0x800000
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_CAMIF_PD_CP_EN_SHIFT 0x17
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_LCR_CP_EN_MASK 0x1000000
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_LCR_CP_EN_SHIFT 0x18
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_RDI_1_OUT_CP_EN_MASK 0x2000000
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_RDI_1_OUT_CP_EN_SHIFT 0x19
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_RDI_2_OUT_CP_EN_MASK 0x4000000
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_RDI_2_OUT_CP_EN_SHIFT 0x1a
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_UNUSED2_MASK 0xf8000000
#define CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL_UNUSED2_SHIFT 0x1b

#define regCPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_OUTPUT_CP_MAPPING_CTRL 0xc  /*register offset*/
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_OUTPUT_CP_MAPPING_CTRL_RDI_0_OUT_CP_EN_MASK 0x1
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_OUTPUT_CP_MAPPING_CTRL_RDI_0_OUT_CP_EN_SHIFT 0x0
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_OUTPUT_CP_MAPPING_CTRL_RDI_1_OUT_CP_EN_MASK 0x2
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_OUTPUT_CP_MAPPING_CTRL_RDI_1_OUT_CP_EN_SHIFT 0x1
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_OUTPUT_CP_MAPPING_CTRL_RDI_2_OUT_CP_EN_MASK 0x4
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_OUTPUT_CP_MAPPING_CTRL_RDI_2_OUT_CP_EN_SHIFT 0x2
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_OUTPUT_CP_MAPPING_CTRL_RDI_3_OUT_CP_EN_MASK 0x8
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_OUTPUT_CP_MAPPING_CTRL_RDI_3_OUT_CP_EN_SHIFT 0x3
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_OUTPUT_CP_MAPPING_CTRL_UNUSED0_MASK 0xf0
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_OUTPUT_CP_MAPPING_CTRL_UNUSED0_SHIFT 0x4
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_OUTPUT_CP_MAPPING_CTRL_TPG_DIS_MASK 0x100
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_OUTPUT_CP_MAPPING_CTRL_TPG_DIS_SHIFT 0x8
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_OUTPUT_CP_MAPPING_CTRL_UNUSED1_MASK 0xfffffe00
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_OUTPUT_CP_MAPPING_CTRL_UNUSED1_SHIFT 0x9

#define regCPAS_TOP_SECURE_CPAS_SEC_IPE0_OUTPUT_CP_MAPPING_CTRL 0x10  /*register offset*/
#define CPAS_TOP_SECURE_CPAS_SEC_IPE0_OUTPUT_CP_MAPPING_CTRL_DISP_C_CP_EN_MASK 0x1
#define CPAS_TOP_SECURE_CPAS_SEC_IPE0_OUTPUT_CP_MAPPING_CTRL_DISP_C_CP_EN_SHIFT 0x0
#define CPAS_TOP_SECURE_CPAS_SEC_IPE0_OUTPUT_CP_MAPPING_CTRL_DISP_Y_CP_EN_MASK 0x2
#define CPAS_TOP_SECURE_CPAS_SEC_IPE0_OUTPUT_CP_MAPPING_CTRL_DISP_Y_CP_EN_SHIFT 0x1
#define CPAS_TOP_SECURE_CPAS_SEC_IPE0_OUTPUT_CP_MAPPING_CTRL_VID_C_CP_EN_MASK 0x4
#define CPAS_TOP_SECURE_CPAS_SEC_IPE0_OUTPUT_CP_MAPPING_CTRL_VID_C_CP_EN_SHIFT 0x2
#define CPAS_TOP_SECURE_CPAS_SEC_IPE0_OUTPUT_CP_MAPPING_CTRL_VID_Y_CP_EN_MASK 0x8
#define CPAS_TOP_SECURE_CPAS_SEC_IPE0_OUTPUT_CP_MAPPING_CTRL_VID_Y_CP_EN_SHIFT 0x3
#define CPAS_TOP_SECURE_CPAS_SEC_IPE0_OUTPUT_CP_MAPPING_CTRL_TF_DS4_YC_CP_EN_MASK 0x10
#define CPAS_TOP_SECURE_CPAS_SEC_IPE0_OUTPUT_CP_MAPPING_CTRL_TF_DS4_YC_CP_EN_SHIFT 0x4
#define CPAS_TOP_SECURE_CPAS_SEC_IPE0_OUTPUT_CP_MAPPING_CTRL_TF_C_CP_EN_MASK 0x20
#define CPAS_TOP_SECURE_CPAS_SEC_IPE0_OUTPUT_CP_MAPPING_CTRL_TF_C_CP_EN_SHIFT 0x5
#define CPAS_TOP_SECURE_CPAS_SEC_IPE0_OUTPUT_CP_MAPPING_CTRL_TF_Y_CP_EN_MASK 0x40
#define CPAS_TOP_SECURE_CPAS_SEC_IPE0_OUTPUT_CP_MAPPING_CTRL_TF_Y_CP_EN_SHIFT 0x6
#define CPAS_TOP_SECURE_CPAS_SEC_IPE0_OUTPUT_CP_MAPPING_CTRL_TFI_CP_EN_MASK 0x80
#define CPAS_TOP_SECURE_CPAS_SEC_IPE0_OUTPUT_CP_MAPPING_CTRL_TFI_CP_EN_SHIFT 0x7
#define CPAS_TOP_SECURE_CPAS_SEC_IPE0_OUTPUT_CP_MAPPING_CTRL_PDI_CP_EN_MASK 0x100
#define CPAS_TOP_SECURE_CPAS_SEC_IPE0_OUTPUT_CP_MAPPING_CTRL_PDI_CP_EN_SHIFT 0x8
#define CPAS_TOP_SECURE_CPAS_SEC_IPE0_OUTPUT_CP_MAPPING_CTRL_UNUSED0_MASK 0xfffffe00
#define CPAS_TOP_SECURE_CPAS_SEC_IPE0_OUTPUT_CP_MAPPING_CTRL_UNUSED0_SHIFT 0x9

#define regCPAS_TOP_SECURE_CPAS_SEC_BPS_OUTPUT_CP_MAPPING_CTRL 0x18  /*register offset*/
#define CPAS_TOP_SECURE_CPAS_SEC_BPS_OUTPUT_CP_MAPPING_CTRL_REG_C_CP_EN_MASK 0x1
#define CPAS_TOP_SECURE_CPAS_SEC_BPS_OUTPUT_CP_MAPPING_CTRL_REG_C_CP_EN_SHIFT 0x0
#define CPAS_TOP_SECURE_CPAS_SEC_BPS_OUTPUT_CP_MAPPING_CTRL_REG_Y_CP_EN_MASK 0x2
#define CPAS_TOP_SECURE_CPAS_SEC_BPS_OUTPUT_CP_MAPPING_CTRL_REG_Y_CP_EN_SHIFT 0x1
#define CPAS_TOP_SECURE_CPAS_SEC_BPS_OUTPUT_CP_MAPPING_CTRL_FULL_DS4_C_CP_EN_MASK 0x4
#define CPAS_TOP_SECURE_CPAS_SEC_BPS_OUTPUT_CP_MAPPING_CTRL_FULL_DS4_C_CP_EN_SHIFT 0x2
#define CPAS_TOP_SECURE_CPAS_SEC_BPS_OUTPUT_CP_MAPPING_CTRL_FULL_DS4_Y_CP_EN_MASK 0x8
#define CPAS_TOP_SECURE_CPAS_SEC_BPS_OUTPUT_CP_MAPPING_CTRL_FULL_DS4_Y_CP_EN_SHIFT 0x3
#define CPAS_TOP_SECURE_CPAS_SEC_BPS_OUTPUT_CP_MAPPING_CTRL_DS4_DS16_YC_CP_EN_MASK 0x10
#define CPAS_TOP_SECURE_CPAS_SEC_BPS_OUTPUT_CP_MAPPING_CTRL_DS4_DS16_YC_CP_EN_SHIFT 0x4
#define CPAS_TOP_SECURE_CPAS_SEC_BPS_OUTPUT_CP_MAPPING_CTRL_STATS_HDR_BHIST_CP_EN_MASK 0x20
#define CPAS_TOP_SECURE_CPAS_SEC_BPS_OUTPUT_CP_MAPPING_CTRL_STATS_HDR_BHIST_CP_EN_SHIFT 0x5
#define CPAS_TOP_SECURE_CPAS_SEC_BPS_OUTPUT_CP_MAPPING_CTRL_STATS_BG_CP_EN_MASK 0x40
#define CPAS_TOP_SECURE_CPAS_SEC_BPS_OUTPUT_CP_MAPPING_CTRL_STATS_BG_CP_EN_SHIFT 0x6
#define CPAS_TOP_SECURE_CPAS_SEC_BPS_OUTPUT_CP_MAPPING_CTRL_UNUSED0_MASK 0xffffff80
#define CPAS_TOP_SECURE_CPAS_SEC_BPS_OUTPUT_CP_MAPPING_CTRL_UNUSED0_SHIFT 0x7

#define regCPAS_TOP_SECURE_CPAS_SEC_CDM_SECURE_ACCESS_CTRL 0x1c  /*register offset*/
#define CPAS_TOP_SECURE_CPAS_SEC_CDM_SECURE_ACCESS_CTRL_CDM_SECURE_ACCESS_MASK 0x1
#define CPAS_TOP_SECURE_CPAS_SEC_CDM_SECURE_ACCESS_CTRL_CDM_SECURE_ACCESS_SHIFT 0x0
#define CPAS_TOP_SECURE_CPAS_SEC_CDM_SECURE_ACCESS_CTRL_UNUSED0_MASK 0xfffffffe
#define CPAS_TOP_SECURE_CPAS_SEC_CDM_SECURE_ACCESS_CTRL_UNUSED0_SHIFT 0x1

#define regCPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_1_OUTPUT_CP_MAPPING_CTRL 0x24  /*register offset*/
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_1_OUTPUT_CP_MAPPING_CTRL_RDI_0_OUT_CP_EN_MASK 0x1
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_1_OUTPUT_CP_MAPPING_CTRL_RDI_0_OUT_CP_EN_SHIFT 0x0
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_1_OUTPUT_CP_MAPPING_CTRL_RDI_1_OUT_CP_EN_MASK 0x2
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_1_OUTPUT_CP_MAPPING_CTRL_RDI_1_OUT_CP_EN_SHIFT 0x1
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_1_OUTPUT_CP_MAPPING_CTRL_RDI_2_OUT_CP_EN_MASK 0x4
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_1_OUTPUT_CP_MAPPING_CTRL_RDI_2_OUT_CP_EN_SHIFT 0x2
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_1_OUTPUT_CP_MAPPING_CTRL_RDI_3_OUT_CP_EN_MASK 0x8
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_1_OUTPUT_CP_MAPPING_CTRL_RDI_3_OUT_CP_EN_SHIFT 0x3
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_1_OUTPUT_CP_MAPPING_CTRL_UNUSED0_MASK 0xf0
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_1_OUTPUT_CP_MAPPING_CTRL_UNUSED0_SHIFT 0x4
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_1_OUTPUT_CP_MAPPING_CTRL_TPG_DIS_MASK 0x100
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_1_OUTPUT_CP_MAPPING_CTRL_TPG_DIS_SHIFT 0x8
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_1_OUTPUT_CP_MAPPING_CTRL_UNUSED1_MASK 0xfffffe00
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_1_OUTPUT_CP_MAPPING_CTRL_UNUSED1_SHIFT 0x9

#define regCPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_1 0x28  /*register offset*/
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_1_CSIPHY_4_CPHY_T0_CP_MASK 0x1
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_1_CSIPHY_4_CPHY_T0_CP_SHIFT 0x0
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_1_CSIPHY_4_CPHY_T1_CP_MASK 0x2
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_1_CSIPHY_4_CPHY_T1_CP_SHIFT 0x1
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_1_CSIPHY_4_CPHY_T2_CP_MASK 0x4
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_1_CSIPHY_4_CPHY_T2_CP_SHIFT 0x2
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_1_CSIPHY_4_DPHY_L0_CP_MASK 0x8
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_1_CSIPHY_4_DPHY_L0_CP_SHIFT 0x3
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_1_CSIPHY_4_DPHY_L1_CP_MASK 0x10
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_1_CSIPHY_4_DPHY_L1_CP_SHIFT 0x4
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_1_CSIPHY_4_DPHY_L2_CP_MASK 0x20
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_1_CSIPHY_4_DPHY_L2_CP_SHIFT 0x5
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_1_CSIPHY_4_DPHY_L3_CP_MASK 0x40
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_1_CSIPHY_4_DPHY_L3_CP_SHIFT 0x6
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_1_UNUSED0_MASK 0x80
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_1_UNUSED0_SHIFT 0x7
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_1_CSIPHY_5_CPHY_T0_CP_MASK 0x100
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_1_CSIPHY_5_CPHY_T0_CP_SHIFT 0x8
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_1_CSIPHY_5_CPHY_T1_CP_MASK 0x200
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_1_CSIPHY_5_CPHY_T1_CP_SHIFT 0x9
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_1_CSIPHY_5_CPHY_T2_CP_MASK 0x400
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_1_CSIPHY_5_CPHY_T2_CP_SHIFT 0xa
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_1_CSIPHY_5_DPHY_L0_CP_MASK 0x800
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_1_CSIPHY_5_DPHY_L0_CP_SHIFT 0xb
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_1_CSIPHY_5_DPHY_L1_CP_MASK 0x1000
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_1_CSIPHY_5_DPHY_L1_CP_SHIFT 0xc
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_1_CSIPHY_5_DPHY_L2_CP_MASK 0x2000
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_1_CSIPHY_5_DPHY_L2_CP_SHIFT 0xd
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_1_CSIPHY_5_DPHY_L3_CP_MASK 0x4000
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_1_CSIPHY_5_DPHY_L3_CP_SHIFT 0xe
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_1_UNUSED1_MASK 0xffff8000
#define CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_1_UNUSED1_SHIFT 0xf

#define regCPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_2_OUTPUT_CP_MAPPING_CTRL 0x2c  /*register offset*/
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_2_OUTPUT_CP_MAPPING_CTRL_RDI_0_OUT_CP_EN_MASK 0x1
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_2_OUTPUT_CP_MAPPING_CTRL_RDI_0_OUT_CP_EN_SHIFT 0x0
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_2_OUTPUT_CP_MAPPING_CTRL_RDI_1_OUT_CP_EN_MASK 0x2
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_2_OUTPUT_CP_MAPPING_CTRL_RDI_1_OUT_CP_EN_SHIFT 0x1
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_2_OUTPUT_CP_MAPPING_CTRL_RDI_2_OUT_CP_EN_MASK 0x4
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_2_OUTPUT_CP_MAPPING_CTRL_RDI_2_OUT_CP_EN_SHIFT 0x2
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_2_OUTPUT_CP_MAPPING_CTRL_RDI_3_OUT_CP_EN_MASK 0x8
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_2_OUTPUT_CP_MAPPING_CTRL_RDI_3_OUT_CP_EN_SHIFT 0x3
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_2_OUTPUT_CP_MAPPING_CTRL_UNUSED0_MASK 0xf0
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_2_OUTPUT_CP_MAPPING_CTRL_UNUSED0_SHIFT 0x4
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_2_OUTPUT_CP_MAPPING_CTRL_TPG_DIS_MASK 0x100
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_2_OUTPUT_CP_MAPPING_CTRL_TPG_DIS_SHIFT 0x8
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_2_OUTPUT_CP_MAPPING_CTRL_UNUSED1_MASK 0xfffffe00
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_2_OUTPUT_CP_MAPPING_CTRL_UNUSED1_SHIFT 0x9

#define regCPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_3_OUTPUT_CP_MAPPING_CTRL 0x30  /*register offset*/
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_3_OUTPUT_CP_MAPPING_CTRL_RDI_0_OUT_CP_EN_MASK 0x1
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_3_OUTPUT_CP_MAPPING_CTRL_RDI_0_OUT_CP_EN_SHIFT 0x0
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_3_OUTPUT_CP_MAPPING_CTRL_RDI_1_OUT_CP_EN_MASK 0x2
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_3_OUTPUT_CP_MAPPING_CTRL_RDI_1_OUT_CP_EN_SHIFT 0x1
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_3_OUTPUT_CP_MAPPING_CTRL_RDI_2_OUT_CP_EN_MASK 0x4
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_3_OUTPUT_CP_MAPPING_CTRL_RDI_2_OUT_CP_EN_SHIFT 0x2
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_3_OUTPUT_CP_MAPPING_CTRL_RDI_3_OUT_CP_EN_MASK 0x8
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_3_OUTPUT_CP_MAPPING_CTRL_RDI_3_OUT_CP_EN_SHIFT 0x3
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_3_OUTPUT_CP_MAPPING_CTRL_UNUSED0_MASK 0xf0
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_3_OUTPUT_CP_MAPPING_CTRL_UNUSED0_SHIFT 0x4
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_3_OUTPUT_CP_MAPPING_CTRL_TPG_DIS_MASK 0x100
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_3_OUTPUT_CP_MAPPING_CTRL_TPG_DIS_SHIFT 0x8
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_3_OUTPUT_CP_MAPPING_CTRL_UNUSED1_MASK 0xfffffe00
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_3_OUTPUT_CP_MAPPING_CTRL_UNUSED1_SHIFT 0x9

#define regCPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_4_OUTPUT_CP_MAPPING_CTRL 0x34  /*register offset*/
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_4_OUTPUT_CP_MAPPING_CTRL_RDI_0_OUT_CP_EN_MASK 0x1
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_4_OUTPUT_CP_MAPPING_CTRL_RDI_0_OUT_CP_EN_SHIFT 0x0
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_4_OUTPUT_CP_MAPPING_CTRL_RDI_1_OUT_CP_EN_MASK 0x2
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_4_OUTPUT_CP_MAPPING_CTRL_RDI_1_OUT_CP_EN_SHIFT 0x1
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_4_OUTPUT_CP_MAPPING_CTRL_RDI_2_OUT_CP_EN_MASK 0x4
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_4_OUTPUT_CP_MAPPING_CTRL_RDI_2_OUT_CP_EN_SHIFT 0x2
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_4_OUTPUT_CP_MAPPING_CTRL_RDI_3_OUT_CP_EN_MASK 0x8
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_4_OUTPUT_CP_MAPPING_CTRL_RDI_3_OUT_CP_EN_SHIFT 0x3
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_4_OUTPUT_CP_MAPPING_CTRL_UNUSED0_MASK 0xf0
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_4_OUTPUT_CP_MAPPING_CTRL_UNUSED0_SHIFT 0x4
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_4_OUTPUT_CP_MAPPING_CTRL_TPG_DIS_MASK 0x100
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_4_OUTPUT_CP_MAPPING_CTRL_TPG_DIS_SHIFT 0x8
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_4_OUTPUT_CP_MAPPING_CTRL_UNUSED1_MASK 0xfffffe00
#define CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_4_OUTPUT_CP_MAPPING_CTRL_UNUSED1_SHIFT 0x9

#define regCPAS_TOP_SECURE_CPAS_SEC_SPARE 0x40  /*register offset*/
#define CPAS_TOP_SECURE_CPAS_SEC_SPARE_SPARE_MASK 0x1
#define CPAS_TOP_SECURE_CPAS_SEC_SPARE_SPARE_SHIFT 0x0
#define CPAS_TOP_SECURE_CPAS_SEC_SPARE_UNUSED0_MASK 0xfffffffe
#define CPAS_TOP_SECURE_CPAS_SEC_SPARE_UNUSED0_SHIFT 0x1

/*----------------------------------------------------------------------
        Register Data Structures
----------------------------------------------------------------------*/

typedef struct{
    unsigned  CSIPHY_0_CPHY_T0_CP : 1; /* 0:0 */
    unsigned  CSIPHY_0_CPHY_T1_CP : 1; /* 1:1 */
    unsigned  CSIPHY_0_CPHY_T2_CP : 1; /* 2:2 */
    unsigned  CSIPHY_0_DPHY_L0_CP : 1; /* 3:3 */
    unsigned  CSIPHY_0_DPHY_L1_CP : 1; /* 4:4 */
    unsigned  CSIPHY_0_DPHY_L2_CP : 1; /* 5:5 */
    unsigned  CSIPHY_0_DPHY_L3_CP : 1; /* 6:6 */
    unsigned  CSIPHY_1_CPHY_T0_CP : 1; /* 7:7 */
    unsigned  CSIPHY_1_CPHY_T1_CP : 1; /* 8:8 */
    unsigned  CSIPHY_1_CPHY_T2_CP : 1; /* 9:9 */
    unsigned  CSIPHY_1_DPHY_L0_CP : 1; /* 10:10 */
    unsigned  CSIPHY_1_DPHY_L1_CP : 1; /* 11:11 */
    unsigned  CSIPHY_1_DPHY_L2_CP : 1; /* 12:12 */
    unsigned  CSIPHY_1_DPHY_L3_CP : 1; /* 13:13 */
    unsigned  CSIPHY_2_CPHY_T0_CP : 1; /* 14:14 */
    unsigned  CSIPHY_2_CPHY_T1_CP : 1; /* 15:15 */
    unsigned  CSIPHY_2_CPHY_T2_CP : 1; /* 16:16 */
    unsigned  CSIPHY_2_DPHY_L0_CP : 1; /* 17:17 */
    unsigned  CSIPHY_2_DPHY_L1_CP : 1; /* 18:18 */
    unsigned  CSIPHY_2_DPHY_L2_CP : 1; /* 19:19 */
    unsigned  CSIPHY_2_DPHY_L3_CP : 1; /* 20:20 */
    unsigned  CSIPHY_3_CPHY_T0_CP : 1; /* 21:21 */
    unsigned  CSIPHY_3_CPHY_T1_CP : 1; /* 22:22 */
    unsigned  CSIPHY_3_CPHY_T2_CP : 1; /* 23:23 */
    unsigned  CSIPHY_3_DPHY_L0_CP : 1; /* 24:24 */
    unsigned  CSIPHY_3_DPHY_L1_CP : 1; /* 25:25 */
    unsigned  CSIPHY_3_DPHY_L2_CP : 1; /* 26:26 */
    unsigned  CSIPHY_3_DPHY_L3_CP : 1; /* 27:27 */
    unsigned  UNUSED0 : 4; /* 31:28 */
} _cpas_top_secure_cpas_sec_lane_cp_ctrl;

typedef union{
    _cpas_top_secure_cpas_sec_lane_cp_ctrl bitfields,bits;
    unsigned int u32All;

} CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL;

typedef struct{
    unsigned  HVX_STREAMING_DIS : 1; /* 0:0 */
    unsigned  VIDEO_OUT_CP_EN : 1; /* 1:1 */
    unsigned  UNUSED0 : 2; /* 3:2 */
    unsigned  FD_OUT_CP_EN : 1; /* 4:4 */
    unsigned  PIXEL_RAW_DUMP_OUT_CP_EN : 1; /* 5:5 */
    unsigned  DUAL_PDAF_OUT_CP_EN : 1; /* 6:6 */
    unsigned  RDI_0_OUT_CP_EN : 1; /* 7:7 */
    unsigned  STATS_HDR_BH_CP_EN : 1; /* 8:8 */
    unsigned  STATS_HDR_BE_CP_EN : 1; /* 9:9 */
    unsigned  STATS_TINTLESS_BG_CP_EN : 1; /* 10:10 */
    unsigned  STATS_BF_CP_EN : 1; /* 11:11 */
    unsigned  STATS_AWB_BG_CP_EN : 1; /* 12:12 */
    unsigned  STATS_BH_CP_EN : 1; /* 13:13 */
    unsigned  STATS_RS_CP_EN : 1; /* 14:14 */
    unsigned  STATS_CS_CP_EN : 1; /* 15:15 */
    unsigned  STATS_IHIST_CP_EN : 1; /* 16:16 */
    unsigned  TPG_DIS : 1; /* 17:17 */
    unsigned  DISPLAY_FULL_OUT_CP_EN : 1; /* 18:18 */
    unsigned  UNUSED1 : 4; /* 22:19 */
    unsigned  CAMIF_PD_CP_EN : 1; /* 23:23 */
    unsigned  LCR_CP_EN : 1; /* 24:24 */
    unsigned  RDI_1_OUT_CP_EN : 1; /* 25:25 */
    unsigned  RDI_2_OUT_CP_EN : 1; /* 26:26 */
    unsigned  UNUSED2 : 5; /* 31:27 */
} _cpas_top_secure_cpas_sec_ife0_output_cp_mapping_ctrl;

typedef union{
    _cpas_top_secure_cpas_sec_ife0_output_cp_mapping_ctrl bitfields,bits;
    unsigned int u32All;

} CPAS_TOP_SECURE_CPAS_SEC_IFE0_OUTPUT_CP_MAPPING_CTRL;

typedef struct{
    unsigned  HVX_STREAMING_DIS : 1; /* 0:0 */
    unsigned  VIDEO_OUT_CP_EN : 1; /* 1:1 */
    unsigned  UNUSED0 : 2; /* 3:2 */
    unsigned  FD_OUT_CP_EN : 1; /* 4:4 */
    unsigned  PIXEL_RAW_DUMP_OUT_CP_EN : 1; /* 5:5 */
    unsigned  DUAL_PDAF_OUT_CP_EN : 1; /* 6:6 */
    unsigned  RDI_0_OUT_CP_EN : 1; /* 7:7 */
    unsigned  STATS_HDR_BH_CP_EN : 1; /* 8:8 */
    unsigned  STATS_HDR_BE_CP_EN : 1; /* 9:9 */
    unsigned  STATS_TINTLESS_BG_CP_EN : 1; /* 10:10 */
    unsigned  STATS_BF_CP_EN : 1; /* 11:11 */
    unsigned  STATS_AWB_BG_CP_EN : 1; /* 12:12 */
    unsigned  STATS_BH_CP_EN : 1; /* 13:13 */
    unsigned  STATS_RS_CP_EN : 1; /* 14:14 */
    unsigned  STATS_CS_CP_EN : 1; /* 15:15 */
    unsigned  STATS_IHIST_CP_EN : 1; /* 16:16 */
    unsigned  TPG_DIS : 1; /* 17:17 */
    unsigned  DISPLAY_FULL_OUT_CP_EN : 1; /* 18:18 */
    unsigned  UNUSED1 : 4; /* 22:19 */
    unsigned  CAMIF_PD_CP_EN : 1; /* 23:23 */
    unsigned  LCR_CP_EN : 1; /* 24:24 */
    unsigned  RDI_1_OUT_CP_EN : 1; /* 25:25 */
    unsigned  RDI_2_OUT_CP_EN : 1; /* 26:26 */
    unsigned  UNUSED2 : 5; /* 31:27 */
} _cpas_top_secure_cpas_sec_ife1_output_cp_mapping_ctrl;

typedef union{
    _cpas_top_secure_cpas_sec_ife1_output_cp_mapping_ctrl bitfields,bits;
    unsigned int u32All;

} CPAS_TOP_SECURE_CPAS_SEC_IFE1_OUTPUT_CP_MAPPING_CTRL;

typedef struct{
    unsigned  RDI_0_OUT_CP_EN : 1; /* 0:0 */
    unsigned  RDI_1_OUT_CP_EN : 1; /* 1:1 */
    unsigned  RDI_2_OUT_CP_EN : 1; /* 2:2 */
    unsigned  RDI_3_OUT_CP_EN : 1; /* 3:3 */
    unsigned  UNUSED0 : 4; /* 7:4 */
    unsigned  TPG_DIS : 1; /* 8:8 */
    unsigned  UNUSED1 : 23; /* 31:9 */
} _cpas_top_secure_cpas_sec_ife_lite_output_cp_mapping_ctrl;

typedef union{
    _cpas_top_secure_cpas_sec_ife_lite_output_cp_mapping_ctrl bitfields,bits;
    unsigned int u32All;

} CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_OUTPUT_CP_MAPPING_CTRL;

typedef struct{
    unsigned  DISP_C_CP_EN : 1; /* 0:0 */
    unsigned  DISP_Y_CP_EN : 1; /* 1:1 */
    unsigned  VID_C_CP_EN : 1; /* 2:2 */
    unsigned  VID_Y_CP_EN : 1; /* 3:3 */
    unsigned  TF_DS4_YC_CP_EN : 1; /* 4:4 */
    unsigned  TF_C_CP_EN : 1; /* 5:5 */
    unsigned  TF_Y_CP_EN : 1; /* 6:6 */
    unsigned  TFI_CP_EN : 1; /* 7:7 */
    unsigned  PDI_CP_EN : 1; /* 8:8 */
    unsigned  UNUSED0 : 23; /* 31:9 */
} _cpas_top_secure_cpas_sec_ipe0_output_cp_mapping_ctrl;

typedef union{
    _cpas_top_secure_cpas_sec_ipe0_output_cp_mapping_ctrl bitfields,bits;
    unsigned int u32All;

} CPAS_TOP_SECURE_CPAS_SEC_IPE0_OUTPUT_CP_MAPPING_CTRL;

typedef struct{
    unsigned  REG_C_CP_EN : 1; /* 0:0 */
    unsigned  REG_Y_CP_EN : 1; /* 1:1 */
    unsigned  FULL_DS4_C_CP_EN : 1; /* 2:2 */
    unsigned  FULL_DS4_Y_CP_EN : 1; /* 3:3 */
    unsigned  DS4_DS16_YC_CP_EN : 1; /* 4:4 */
    unsigned  STATS_HDR_BHIST_CP_EN : 1; /* 5:5 */
    unsigned  STATS_BG_CP_EN : 1; /* 6:6 */
    unsigned  UNUSED0 : 25; /* 31:7 */
} _cpas_top_secure_cpas_sec_bps_output_cp_mapping_ctrl;

typedef union{
    _cpas_top_secure_cpas_sec_bps_output_cp_mapping_ctrl bitfields,bits;
    unsigned int u32All;

} CPAS_TOP_SECURE_CPAS_SEC_BPS_OUTPUT_CP_MAPPING_CTRL;

typedef struct{
    unsigned  CDM_SECURE_ACCESS : 1; /* 0:0 */
    unsigned  UNUSED0 : 31; /* 31:1 */
} _cpas_top_secure_cpas_sec_cdm_secure_access_ctrl;

typedef union{
    _cpas_top_secure_cpas_sec_cdm_secure_access_ctrl bitfields,bits;
    unsigned int u32All;

} CPAS_TOP_SECURE_CPAS_SEC_CDM_SECURE_ACCESS_CTRL;

typedef struct{
    unsigned  RDI_0_OUT_CP_EN : 1; /* 0:0 */
    unsigned  RDI_1_OUT_CP_EN : 1; /* 1:1 */
    unsigned  RDI_2_OUT_CP_EN : 1; /* 2:2 */
    unsigned  RDI_3_OUT_CP_EN : 1; /* 3:3 */
    unsigned  UNUSED0 : 4; /* 7:4 */
    unsigned  TPG_DIS : 1; /* 8:8 */
    unsigned  UNUSED1 : 23; /* 31:9 */
} _cpas_top_secure_cpas_sec_ife_lite_1_output_cp_mapping_ctrl;

typedef union{
    _cpas_top_secure_cpas_sec_ife_lite_1_output_cp_mapping_ctrl bitfields,bits;
    unsigned int u32All;

} CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_1_OUTPUT_CP_MAPPING_CTRL;

typedef struct{
    unsigned  CSIPHY_4_CPHY_T0_CP : 1; /* 0:0 */
    unsigned  CSIPHY_4_CPHY_T1_CP : 1; /* 1:1 */
    unsigned  CSIPHY_4_CPHY_T2_CP : 1; /* 2:2 */
    unsigned  CSIPHY_4_DPHY_L0_CP : 1; /* 3:3 */
    unsigned  CSIPHY_4_DPHY_L1_CP : 1; /* 4:4 */
    unsigned  CSIPHY_4_DPHY_L2_CP : 1; /* 5:5 */
    unsigned  CSIPHY_4_DPHY_L3_CP : 1; /* 6:6 */
    unsigned  UNUSED0 : 1; /* 7:7 */
    unsigned  CSIPHY_5_CPHY_T0_CP : 1; /* 8:8 */
    unsigned  CSIPHY_5_CPHY_T1_CP : 1; /* 9:9 */
    unsigned  CSIPHY_5_CPHY_T2_CP : 1; /* 10:10 */
    unsigned  CSIPHY_5_DPHY_L0_CP : 1; /* 11:11 */
    unsigned  CSIPHY_5_DPHY_L1_CP : 1; /* 12:12 */
    unsigned  CSIPHY_5_DPHY_L2_CP : 1; /* 13:13 */
    unsigned  CSIPHY_5_DPHY_L3_CP : 1; /* 14:14 */
    unsigned  UNUSED1 : 17; /* 31:15 */
} _cpas_top_secure_cpas_sec_lane_cp_ctrl_1;

typedef union{
    _cpas_top_secure_cpas_sec_lane_cp_ctrl_1 bitfields,bits;
    unsigned int u32All;

} CPAS_TOP_SECURE_CPAS_SEC_LANE_CP_CTRL_1;

typedef struct{
    unsigned  RDI_0_OUT_CP_EN : 1; /* 0:0 */
    unsigned  RDI_1_OUT_CP_EN : 1; /* 1:1 */
    unsigned  RDI_2_OUT_CP_EN : 1; /* 2:2 */
    unsigned  RDI_3_OUT_CP_EN : 1; /* 3:3 */
    unsigned  UNUSED0 : 4; /* 7:4 */
    unsigned  TPG_DIS : 1; /* 8:8 */
    unsigned  UNUSED1 : 23; /* 31:9 */
} _cpas_top_secure_cpas_sec_ife_lite_2_output_cp_mapping_ctrl;

typedef union{
    _cpas_top_secure_cpas_sec_ife_lite_2_output_cp_mapping_ctrl bitfields,bits;
    unsigned int u32All;

} CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_2_OUTPUT_CP_MAPPING_CTRL;

typedef struct{
    unsigned  RDI_0_OUT_CP_EN : 1; /* 0:0 */
    unsigned  RDI_1_OUT_CP_EN : 1; /* 1:1 */
    unsigned  RDI_2_OUT_CP_EN : 1; /* 2:2 */
    unsigned  RDI_3_OUT_CP_EN : 1; /* 3:3 */
    unsigned  UNUSED0 : 4; /* 7:4 */
    unsigned  TPG_DIS : 1; /* 8:8 */
    unsigned  UNUSED1 : 23; /* 31:9 */
} _cpas_top_secure_cpas_sec_ife_lite_3_output_cp_mapping_ctrl;

typedef union{
    _cpas_top_secure_cpas_sec_ife_lite_3_output_cp_mapping_ctrl bitfields,bits;
    unsigned int u32All;

} CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_3_OUTPUT_CP_MAPPING_CTRL;

typedef struct{
    unsigned  RDI_0_OUT_CP_EN : 1; /* 0:0 */
    unsigned  RDI_1_OUT_CP_EN : 1; /* 1:1 */
    unsigned  RDI_2_OUT_CP_EN : 1; /* 2:2 */
    unsigned  RDI_3_OUT_CP_EN : 1; /* 3:3 */
    unsigned  UNUSED0 : 4; /* 7:4 */
    unsigned  TPG_DIS : 1; /* 8:8 */
    unsigned  UNUSED1 : 23; /* 31:9 */
} _cpas_top_secure_cpas_sec_ife_lite_4_output_cp_mapping_ctrl;

typedef union{
    _cpas_top_secure_cpas_sec_ife_lite_4_output_cp_mapping_ctrl bitfields,bits;
    unsigned int u32All;

} CPAS_TOP_SECURE_CPAS_SEC_IFE_LITE_4_OUTPUT_CP_MAPPING_CTRL;

typedef struct{
    unsigned  SPARE : 1; /* 0:0 */
    unsigned  UNUSED0 : 31; /* 31:1 */
} _cpas_top_secure_cpas_sec_spare;

typedef union{
    _cpas_top_secure_cpas_sec_spare bitfields,bits;
    unsigned int u32All;

} CPAS_TOP_SECURE_CPAS_SEC_SPARE;

/*----------------------------------------------------------------------
        ENUM Data Structures
----------------------------------------------------------------------*/
#endif // TITAN480_CPAS_TOP_SECURE_H
