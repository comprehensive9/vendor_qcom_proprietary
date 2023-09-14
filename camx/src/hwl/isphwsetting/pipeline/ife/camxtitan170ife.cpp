////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  camxtitan170ife.cpp
/// @brief IFE Node class implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "camxifeabf34.h"
#include "camxifeawbbgstats14.h"
#include "camxifebfstats23.h"
#include "camxifebhiststats14.h"
#include "camxifebls12.h"
#include "camxifebpcbcc50.h"
#include "camxifecamif.h"
#include "camxifecc12.h"
#include "camxifecrop10.h"
#include "camxifecsstats14.h"
#include "camxifecsstats14titan17x.h"
#include "camxifecst12.h"
#include "camxifedemosaic36.h"
#include "camxifedemux13.h"
#include "camxifeds410.h"
#include "camxifedualpd10.h"
#include "camxifegamma16.h"
#include "camxifegtm10.h"
#include "camxifehdr20.h"
#include "camxifehdrbestats15.h"
#include "camxifehdrbhiststats13.h"
#include "camxifehvx.h"
#include "camxifeihiststats12.h"
#include "camxifelinearization33.h"
#include "camxifelsc34.h"
#include "camxifemnds16.h"
#include "camxifepdpc11.h"
#include "camxifepedestal13.h"
#include "camxifeprecrop10.h"
#include "camxifer2pd10.h"
#include "camxiferoundclamp11.h"
#include "camxifersstats14.h"
#include "camxispstatsmodule.h"
#include "camxifetintlessbgstats15.h"
#include "camxifewb12.h"
#include "camxisppipeline.h"
#include "camxtitan170ife.h"
#include "camxswtmc11.h"
#include "camxswtmc12.h"
#include "camxisppipeline.h"
#include "camxcslifedefs.h"

UINT32 IFE170CGCOnCmds[] =
{
    regIFE_IFE_0_VFE_MODULE_LENS_CGC_OVERRIDE,    0xFFFFFFFF,
    regIFE_IFE_0_VFE_MODULE_STATS_CGC_OVERRIDE,   0xFFFFFFFF,
    regIFE_IFE_0_VFE_MODULE_COLOR_CGC_OVERRIDE,   0xFFFFFFFF,
    regIFE_IFE_0_VFE_MODULE_ZOOM_CGC_OVERRIDE,    0xFFFFFFFF,
    regIFE_IFE_0_VFE_MODULE_BUS_CGC_OVERRIDE,     0xFFFFFFFF,
    regIFE_IFE_0_VFE_MODULE_DUAL_PD_CGC_OVERRIDE, 0x1
};

static const UINT32 IFE170CGCNumRegs = sizeof(IFE170CGCOnCmds) / (2 * sizeof(UINT32));

CAMX_NAMESPACE_BEGIN

IFEIQModuleInfo IFEIQModuleItems2[] =
{
    {ISPIQModuleType::SWTMC,                    IFEPipelinePath::CommonPath,        TRUE,   SWTMC12::CreateIFE},
    {ISPIQModuleType::IFECAMIF,                 IFEPipelinePath::CommonPath,        TRUE,   IFECAMIF::Create},
    {ISPIQModuleType::IFEPedestalCorrection,    IFEPipelinePath::CommonPath,        FALSE,  IFEPedestal13::Create},
    {ISPIQModuleType::IFELinearization,         IFEPipelinePath::CommonPath,        TRUE,   IFELinearization33::Create},
    {ISPIQModuleType::IFEBLS,                   IFEPipelinePath::CommonPath,        TRUE,   IFEBLS12::Create },
    {ISPIQModuleType::IFEPDPC,                  IFEPipelinePath::CommonPath,        TRUE,   IFEPDPC11::Create},
    {ISPIQModuleType::IFEDemux,                 IFEPipelinePath::CommonPath,        TRUE,   IFEDemux13::Create},
    {ISPIQModuleType::IFEHDR,                   IFEPipelinePath::CommonPath,        TRUE,   IFEHDR20::Create},
    {ISPIQModuleType::IFEBPCBCC,                IFEPipelinePath::CommonPath,        TRUE,   IFEBPCBCC50::Create},
    {ISPIQModuleType::IFEABF,                   IFEPipelinePath::CommonPath,        TRUE,   IFEABF34::Create},
    {ISPIQModuleType::IFELSC,                   IFEPipelinePath::CommonPath,        TRUE,   IFELSC34::Create},
    {ISPIQModuleType::IFEWB,                    IFEPipelinePath::CommonPath,        TRUE,   IFEWB12::Create},
    {ISPIQModuleType::IFEDemosaic,              IFEPipelinePath::CommonPath,        TRUE,   IFEDemosaic36::Create},
    {ISPIQModuleType::IFECC,                    IFEPipelinePath::CommonPath,        TRUE,   IFECC12::Create},
    {ISPIQModuleType::IFEGTM,                   IFEPipelinePath::CommonPath,        TRUE,   IFEGTM10::Create},
    {ISPIQModuleType::IFEGamma,                 IFEPipelinePath::CommonPath,        TRUE,   IFEGamma16::Create},
    {ISPIQModuleType::IFECST,                   IFEPipelinePath::CommonPath,        TRUE,   IFECST12::Create},
    {ISPIQModuleType::IFEHVX,                   IFEPipelinePath::CommonPath,        TRUE,   IFEHVX::Create },
    {ISPIQModuleType::IFEMNDS,                  IFEPipelinePath::VideoFullPath,     TRUE,   IFEMNDS16::Create},
    {ISPIQModuleType::IFECrop,                  IFEPipelinePath::VideoFullPath,     TRUE,   IFECrop10::Create},
    {ISPIQModuleType::IFERoundClamp,            IFEPipelinePath::VideoFullPath,     TRUE,   IFERoundClamp11::Create},
    {ISPIQModuleType::IFEMNDS,                  IFEPipelinePath::FDPath,            TRUE,   IFEMNDS16::Create},
    {ISPIQModuleType::IFECrop,                  IFEPipelinePath::FDPath,            TRUE,   IFECrop10::Create},
    {ISPIQModuleType::IFERoundClamp,            IFEPipelinePath::FDPath,            TRUE,   IFERoundClamp11::Create},
    {ISPIQModuleType::IFEPreCrop,               IFEPipelinePath::VideoDS4Path,      TRUE,   IFEPreCrop10::Create},
    {ISPIQModuleType::IFEDS4  ,                 IFEPipelinePath::VideoDS4Path,      TRUE,   IFEDS410::Create},
    {ISPIQModuleType::IFECrop,                  IFEPipelinePath::VideoDS4Path,      TRUE,   IFECrop10::Create},
    {ISPIQModuleType::IFERoundClamp,            IFEPipelinePath::VideoDS4Path,      TRUE,   IFERoundClamp11::Create},
    {ISPIQModuleType::IFER2PD,                  IFEPipelinePath::VideoDS4Path,      TRUE,   IFER2PD10::Create},
    {ISPIQModuleType::IFEPreCrop,               IFEPipelinePath::VideoDS16Path,     TRUE,   IFEPreCrop10::Create},
    {ISPIQModuleType::IFEDS4  ,                 IFEPipelinePath::VideoDS16Path,     TRUE,   IFEDS410::Create},
    {ISPIQModuleType::IFECrop,                  IFEPipelinePath::VideoDS16Path,     TRUE,   IFECrop10::Create},
    {ISPIQModuleType::IFERoundClamp,            IFEPipelinePath::VideoDS16Path,     TRUE,   IFERoundClamp11::Create},
    {ISPIQModuleType::IFER2PD,                  IFEPipelinePath::VideoDS16Path,     TRUE,   IFER2PD10::Create},
    {ISPIQModuleType::IFECrop,                  IFEPipelinePath::PixelRawDumpPath,  TRUE,   IFECrop10::Create},
};

static IFEStatsModuleInfo IFEStatsModuleItems2[] =
{
    {ISPStatsModuleType::IFERS,         TRUE, RSStats14::Create},
    {ISPStatsModuleType::IFECS,         TRUE, CSStats14::Create},
    {ISPStatsModuleType::IFEIHist,      TRUE, IHistStats12::Create},
    {ISPStatsModuleType::IFEBHist,      TRUE, BHistStats14::Create},
    {ISPStatsModuleType::IFEHDRBE,      TRUE, HDRBEStats15::Create},
    {ISPStatsModuleType::IFEHDRBHist,   TRUE, HDRBHistStats13::Create},
    {ISPStatsModuleType::IFETintlessBG, TRUE, TintlessBGStats15::Create},
    {ISPStatsModuleType::IFEBF,         TRUE,  IFEBFStats23::Create},
    {ISPStatsModuleType::IFEAWBBG,      TRUE, AWBBGStats14::Create}
};

enum IFERegGroupTypes
{
    IFETop = 0,     ///< IFETop
    ChromaUp,       ///< ChromaUp
    Pedestal,       ///< Pedestal
    Linearization,  ///< Linearization
    BPC,            ///< BPC
    HDR,            ///< HDR
    ABF,            ///< ABF
    LSC,            ///< LSC
    ColorCorrect,   ///< ColorCorrect
    GTMDMI,         ///< GTM
    Gamma,          ///< Gamma
    Stats,          ///< Stats
    VidPath,        ///< VidDS16, VidDS4, VidFull paths
    DispPath,       ///< DispDS16, DispDS4, DispFull paths
    FD,             ///< FD
    PixelRaw,       ///< PixelRaw
    PDAF,           ///< PDAF
    PDOUT,          ///< PDOUT
    LCR,            ///< LCR
    Common          ///< Common
};

struct RegDumpInfo
{
    IFERegGroupTypes regGroupType;  ///< Register Group Type
    IFERegReadInfo   regReadInfo;   ///< Register Read Info
};

static RegDumpInfo   IFERegDump[] =
{
    // IFE TOP CFG
    {
        IFETop,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_HW_VERSION,
                    32,
                }
            },
        }
    },
    // TESTGEN
    {
        IFETop,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_HW_TESTGEN_CMD,
                    10,
                }
            },
        }
    },
    // PDAF
    {
        PDAF,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_PDAF_CFG,
                    11,
                }
            },
        }
    },
    // CAMIF
    {
        IFETop,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_CAMIF_RAW_CROP_WIDTH_CFG,
                    2,
                }
            },
        }
    },
    {
        IFETop,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_CAMIF_CMD,
                    11,
                }
            },
        }
    },
    // Pedestal
    {
        Pedestal,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_PEDESTAL_CFG,
                    11,
                }
            },
        }
    },
    // Demux
    {
        IFETop,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_DEMUX_CFG,
                    7,
                }
            },
        }
    },
    // HDR
    {
        HDR,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_HDR_CFG_0,
                    5,
                }
            },
        }
    },
    {
        HDR,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_HDR_CFG_5,
                    1,
                }
            },
        }
    },
    // HDR RECON
    {
        HDR,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_HDR_RECON_CFG_0,
                    5,
                }
            },
        }
    },
    {
        HDR,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_HDR_RECON_CFG_5,
                    2,
                }
            },
        }
    },
    // HDR MAC
    {
        HDR,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_HDR_MAC_CFG_0,
                    8,
                }
            },
        }
    },
    // BPC
    {
        BPC,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_BPC_CFG_0,
                    8,
                }
            },
        }
    },
    // ChromaUp
    {
        ChromaUp,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_CHROMA_UPSAMPLE_CFG,
                    1,
                }
            },
        }
    },
    // Color correct
    {
        ColorCorrect,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_COLOR_CORRECT_COEFF_0,
                    13,
                }
            },
        }
    },
    // COLOR XFROM
    {
        IFETop,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_COLOR_XFORM_CH0_COEFF_CFG_0,
                    12,
                }
            },
        }
    },
    // Demosaic
    {
        IFETop,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_DEMO_CFG,
                    11,
                }
            },
        }
    },
    // FD
    {
        FD,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_SCALE_FD_Y_CFG,
                    22,
                }
            },
        }
    },
    // FD Y
    {
        FD,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_FD_OUT_Y_CROP_RND_CLAMP_CFG,
                    5,
                }
            },
        }
    },
    // FD C
    {
        FD,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_FD_OUT_C_CROP_RND_CLAMP_CFG,
                    5,
                }
            },
        }
    },
    // LINEARIZATION
    {
        Linearization,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_BLACK_CFG,
                    33,
                }
            },
        }
    },
    // Video Path
    {
        VidPath,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_SCALE_VID_Y_CFG,
                    22,
                }
            },
        }
    },
    // Video Path DS
    {
        VidPath,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_DS_4TO1_Y_1ST_CFG,
                    8,
                }
            },
        }
    },
    // Video Path DS4 Y
    {
        VidPath,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_DS4_Y_PRE_CROP_LINE_CFG,
                    7,
                }
            },
        }
    },
    // Video Path DS4 C
    {
        VidPath,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_DS4_C_PRE_CROP_LINE_CFG,
                    7,
                }
            },
        }
    },
    // Video Path DS16 Y
    {
        VidPath,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_DS16_Y_PRE_CROP_LINE_CFG,
                    7,
                }
            },
        }
    },
    // Video Path DS16 C
    {
        VidPath,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_DS16_C_PRE_CROP_LINE_CFG,
                    7,
                }
            },
        }
    },
    // Video Path FULL OUT Y
    {
        VidPath,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_FULL_OUT_Y_CROP_RND_CLAMP_CFG,
                    5,
                }
            },
        }
    },
    // Video Path FULL OUT C
    {
        VidPath,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_FULL_OUT_C_CROP_RND_CLAMP_CFG,
                    5,
                }
            },
        }
    },
    // ABF
    {
        ABF,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_ABF_CFG,
                    25,
                }
            },
        }
    },
    // Black Level
    {
        IFETop,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_BLACK_LEVEL_CFG,
                    3,
                }
            },
        }
    },
    // LSC RollOff
    {
        LSC,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_ROLLOFF_CFG,
                    11,
                }
            },
        }
    },
    // GTM
    {
        GTMDMI,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_GTM_CFG,
                    1,
                }
            },
        }
    },
    // GAMMA
    {
        Gamma,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_RGB_LUT_CFG,
                    1
                }
            },
        }
    },
    // Stats
    {
        Stats,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_STATS_CFG,
                    1,
                }
            },
        }
    },
    // Stats HDR BE
    {
        Stats,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_STATS_HDR_BE_RGN_OFFSET_CFG,
                    7,
                }
            },
        }
    },
    {
        Stats,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_STATS_HDR_BE_CH_Y_CFG,
                    1,
                }
            },
        }
    },
    {
        Stats,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_STATS_HDR_BE_CFG,
                    1,
                }
            },
        }
    },
    // Stats HDR BHIST
    {
        Stats,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_STATS_HDR_BHIST_RGN_OFFSET_CFG,
                    2,
                }
            },
        }
    },
    // Stats BAF
    {
        Stats,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_STATS_BAF_CFG,
                    12,
                }
            },
        }
    },
    {
        Stats,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_STATS_BAF_V_IIR_CFG_0,
                    9,
                }
            },
        }
    },
    {
        Stats,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_STATS_BAF_V_TH_CFG,
                    13,
                }
            },
        }
    },
    {
        Stats,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_STATS_BAF_V_IIR_CFG_3,
                    4,
                }
            },
        }
    },
    // Stats TINTLESS
    {
        Stats,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_STATS_AEC_BG_CFG,
                    1,
                }
            },
        }
    },
    {
        Stats,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_STATS_AEC_BG_CH_Y_CFG,
                    1,
                }
            },
        }
    },
    {
        Stats,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_STATS_AEC_BG_RGN_OFFSET_CFG,
                    7,
                }
            },
        }
    },
    // Stats AWB BG
    {
        Stats,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_STATS_AWB_BG_CFG,
                    1,
                }
            },
        }
    },
    {
        Stats,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_STATS_AWB_BG_CH_Y_CFG,
                    1,
                }
            },
        }
    },
    {
        Stats,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_STATS_AWB_BG_RGN_OFFSET_CFG,
                    7,
                }
            },
        }
    },
    // Stats RS/CS/Ihist/Bhist
    {
        Stats,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_STATS_BHIST_RGN_OFFSET_CFG,
                    10,
                }
            },
        }
    },
    // CROP PixelRaw
    {
        PixelRaw,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_CROP_PIXEL_RAW_DUMP_WIDTH_CFG,
                    2,
                }
            },
        }
    },
    {
        PixelRaw,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_VFE_PIXEL_RAW_DUMP_CFG,
                    2,
                }
            },
        }
    },
    // BUS WR
    {
        Common,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_BUS_WR_HW_VERSION,
                    52
                }
            },
        }
    },
    // BUS WM 0
    {
        Common,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_BUS_WR_CLIENT_0_STATUS_0,
                    30
                }
            },
        }
    },
    // BUS WM 1
    {
        Common,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_BUS_WR_CLIENT_1_STATUS_0,
                    30
                }
            },
        }
    },
    // BUS WM 2
    {
        Common,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_BUS_WR_CLIENT_2_STATUS_0,
                    30
                }
            },
        }
    },
    // BUS WM 3
    {
        Common,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_BUS_WR_CLIENT_3_STATUS_0,
                    41
                }
            },
        }
    },
    // BUS WM 4
    {
        Common,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_BUS_WR_CLIENT_4_STATUS_0,
                    41
                }
            },
        }
    },
    // BUS WM 5
    {
        Common,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_BUS_WR_CLIENT_5_STATUS_0,
                    30
                }
            },
        }
    },
    // BUS WM 6
    {
        Common,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_BUS_WR_CLIENT_6_STATUS_0,
                    30
                }
            },
        }
    },
    // BUS WM 7
    {
        Common,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_BUS_WR_CLIENT_7_STATUS_0,
                    30
                }
            },
        }
    },
    // BUS WM 8
    {
        Common,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_BUS_WR_CLIENT_8_STATUS_0,
                    30
                }
            },
        }
    },
    // BUS WM 9
    {
        Common,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_BUS_WR_CLIENT_9_STATUS_0,
                    30
                }
            },
        }
    },
    // BUS WM 10
    {
        Common,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_BUS_WR_CLIENT_10_STATUS_0,
                    30
                }
            },
        }
    },
    // BUS WM 11
    {
        Common,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_BUS_WR_CLIENT_11_STATUS_0,
                    30
                }
            },
        }
    },
    // BUS WM 12
    {
        Common,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_BUS_WR_CLIENT_12_STATUS_0,
                    30
                }
            },
        }
    },
    // BUS WM 13
    {
        Common,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_BUS_WR_CLIENT_13_STATUS_0,
                    30
                }
            },
        }
    },
    // BUS WM 14
    {
        Common,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_BUS_WR_CLIENT_14_STATUS_0,
                    30
                }
            },
        }
    },
    // BUS WM 15
    {
        Common,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_BUS_WR_CLIENT_15_STATUS_0,
                    30
                }
            },
        }
    },
    // BUS WM 16
    {
        Common,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_BUS_WR_CLIENT_16_STATUS_0,
                    30
                }
            },
        }
    },
    // BUS WM 17
    {
        Common,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_BUS_WR_CLIENT_17_STATUS_0,
                    30
                }
            },
        }
    },
    // BUS WM 18
    {
        Common,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_BUS_WR_CLIENT_18_STATUS_0,
                    30
                }
            },
        }
    },
    // BUS WM 19
    {
        Common,
        {
            IFERegDumpReadTypeReg,
            0,
            {
                {
                    regIFE_IFE_0_BUS_WR_CLIENT_19_STATUS_0,
                    30
                }
            },
        }
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Titan170IFE::FetchPipelineCapability
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID Titan170IFE::FetchPipelineCapability(
    VOID* pPipelineCapability)
{
    IFECapabilityInfo*   pIFECapability = static_cast<IFECapabilityInfo*>(pPipelineCapability);

    pIFECapability->numIFEIQModule      = sizeof(IFEIQModuleItems2) / sizeof(IFEIQModuleInfo);
    pIFECapability->pIFEIQModuleList    = &IFEIQModuleItems2[0];
    pIFECapability->numIFEStatsModule   = sizeof(IFEStatsModuleItems2) / sizeof(IFEStatsModuleInfo);
    pIFECapability->pIFEStatsModuleList = &IFEStatsModuleItems2[0];

    pIFECapability->UBWCSupportedVersionMask = UBWCVersion2Mask;
    pIFECapability->UBWCLossySupport         = UBWCLossless;
    pIFECapability->lossy10bitWidth          = UINT32_MAX;
    pIFECapability->lossy10bitHeight         = UINT32_MAX;
    pIFECapability->lossy8bitWidth           = UINT32_MAX;
    pIFECapability->lossy8bitHeight          = UINT32_MAX;
    pIFECapability->ICAVersion               = ICAVersion10;
    pIFECapability->LDCSupport               = TRUE;


}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Titan170IFE::ProgramIQModuleEnableConfig()
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CamxResult Titan170IFE::ProgramIQModuleEnableConfig(
    ISPInputData*       pInputData,
    ISPInternalData*    pISPData,
    IFEOutputPath*      pIFEOutputPathInfo)
{
    CamxResult result = CamxResultSuccess;

    if ((NULL != pInputData) && (NULL != pISPData))
    {
        IFEModuleEnableConfig*          pModuleEnable           = &pInputData->pCalculatedData->moduleEnable;
        UINT32                          raw_dump                = 0;
        IQModulesCoreEnableConfig*      pIQmodule               = &pInputData->pCalculatedData->moduleEnable.IQModules;
        IQModulesCoreEnableConfig*      pCommonIFEIQConfig      = &pISPData->moduleEnable.IQModules;
        FDPathEnableConfig*             pFDModules              = &pModuleEnable->FDprocessingModules;
        FDPathEnableConfig*             pCommonIFEFDPath        = &pISPData->moduleEnable.FDprocessingModules;
        ImagePathEnableConfig*          pVideoModules           = &pModuleEnable->videoProcessingModules;
        ImagePathEnableConfig*          pCommonIFEVideoPath     = &pISPData->moduleEnable.videoProcessingModules;
        StatsModulesControl*            pStatsControl           = &pModuleEnable->statsModules;
        StatsModulesControl*            pCommonIFEStatsControl   = &pISPData->moduleEnable.statsModules;

        // reference to the bitfields
        _ife_ife_0_vfe_module_lens_en*                  pLensProcessingModuleConfig             =
            &m_moduleConfig.lensProcessingModuleConfig.bitfields;
        _ife_ife_0_vfe_module_color_en*                 pColorProcessingModuleConfig            =
            &m_moduleConfig.colorProcessingModuleConfig.bitfields;
        _ife_ife_0_vfe_module_zoom_en*                  pFrameProcessingModuleConfig            =
            &m_moduleConfig.frameProcessingModuleConfig.bitfields;
        _ife_ife_0_vfe_fd_out_y_crop_rnd_clamp_cfg*     pFDLumaCropRoundClampConfig             =
            &m_moduleConfig.FDLumaCropRoundClampConfig.bitfields;
        _ife_ife_0_vfe_fd_out_c_crop_rnd_clamp_cfg*     pFDChromaCropRoundClampConfig           =
            &m_moduleConfig.FDChromaCropRoundClampConfig.bitfields;
        _ife_ife_0_vfe_full_out_y_crop_rnd_clamp_cfg*   pVideoLumaCropRoundClampConfig          =
            &m_moduleConfig.videoLumaCropRoundClampConfig.bitfields;
        _ife_ife_0_vfe_full_out_c_crop_rnd_clamp_cfg*   pVideoChromaCropRoundClampConfig        =
            &m_moduleConfig.videoChromaCropRoundClampConfig.bitfields;
        _ife_ife_0_vfe_ds4_y_crop_rnd_clamp_cfg*        pVideoDS4LumaCropRoundClampConfig       =
            &m_moduleConfig.videoDS4LumaCropRoundClampConfig.bitfields;
        _ife_ife_0_vfe_ds4_c_crop_rnd_clamp_cfg*        pVideoDS4ChromaCropRoundClampConfig     =
            &m_moduleConfig.videoDS4ChromaCropRoundClampConfig.bitfields;
        _ife_ife_0_vfe_ds16_y_crop_rnd_clamp_cfg*       pVideoDS16LumaCropRoundClampConfig      =
            &m_moduleConfig.videoDS16LumaCropRoundClampConfig.bitfields;
        _ife_ife_0_vfe_ds16_c_crop_rnd_clamp_cfg*       pVideoDS16ChromaCropRoundClampConfig    =
            &m_moduleConfig.videoDS16ChromaCropRoundClampConfig.bitfields;
        _ife_ife_0_vfe_module_stats_en*                 pStatsEnable                            =
            &m_moduleConfig.statsEnable.bitfields;
        _ife_ife_0_vfe_stats_cfg *                      pStatsConfig                            =
            &m_moduleConfig.statsConfig.bitfields;
        _ife_ife_0_vfe_dsp_to_sel*                      pDSPConfig                              =
            &m_moduleConfig.DSPConfig.bitfields;


        // Update the lens processing module config registers
        pLensProcessingModuleConfig->PEDESTAL_EN        = pIQmodule->pedestalEnable                  |
                                                          pCommonIFEIQConfig->pedestalEnable;
        pLensProcessingModuleConfig->BLACK_EN           = pIQmodule->liniearizationEnable            |
                                                          pCommonIFEIQConfig->liniearizationEnable;
        pLensProcessingModuleConfig->DEMUX_EN           = pIQmodule->demuxEnable                     |
                                                          pCommonIFEIQConfig->demuxEnable;
        pLensProcessingModuleConfig->CHROMA_UPSAMPLE_EN = pIQmodule->chromaUpsampleEnable            |
                                                          pCommonIFEIQConfig->chromaUpsampleEnable;
        pLensProcessingModuleConfig->HDR_RECON_EN       = pIQmodule->HDRReconEnable                  |
                                                          pCommonIFEIQConfig->HDRReconEnable;
        pLensProcessingModuleConfig->HDR_MAC_EN         = pIQmodule->HDRMACEnable                    |
                                                          pCommonIFEIQConfig->HDRMACEnable;
        pLensProcessingModuleConfig->BPC_EN             = pIQmodule->BPCEnable                       |
                                                          pCommonIFEIQConfig->BPCEnable;
        pLensProcessingModuleConfig->ABF_EN             = pIQmodule->ABFEnable                       |
                                                          pCommonIFEIQConfig->ABFEnable;
        pLensProcessingModuleConfig->ROLLOFF_EN         = pIQmodule->rolloffEnable                   |
                                                          pCommonIFEIQConfig->rolloffEnable;
        pLensProcessingModuleConfig->DEMO_EN            = pIQmodule->demosaicEnable                  |
                                                          pCommonIFEIQConfig->demosaicEnable;
        pLensProcessingModuleConfig->BLACK_LEVEL_EN     = pIQmodule->BLSEnable                       |
                                                          pCommonIFEIQConfig->BLSEnable;
        pLensProcessingModuleConfig->PDAF_EN            = pIQmodule->PDAFEnable                      |
                                                          pCommonIFEIQConfig->PDAFEnable;
        pLensProcessingModuleConfig->BIN_CORR_EN        = pIQmodule->binningCorrectionEnable         |
                                                          pCommonIFEIQConfig->binningCorrectionEnable;
        pColorProcessingModuleConfig->COLOR_CORRECT_EN  = pIQmodule->CCEnable                        |
                                                          pCommonIFEIQConfig->CCEnable;
        pColorProcessingModuleConfig->GTM_EN            = pIQmodule->GTMEnable                       |
                                                          pCommonIFEIQConfig->GTMEnable;
        pColorProcessingModuleConfig->RGB_LUT_EN        = pIQmodule->gammaEnable                     |
                                                          pCommonIFEIQConfig->gammaEnable;

        // Update the frame processing module config registers
        pFrameProcessingModuleConfig->CST_EN                    = pIQmodule->CSTEnable |
                                                                  pCommonIFEIQConfig->CSTEnable;
        pFrameProcessingModuleConfig->SCALE_FD_EN               = pFDModules->MNDSEnable |
                                                                  pCommonIFEFDPath->MNDSEnable;
        pFrameProcessingModuleConfig->CROP_FD_EN                = pFDModules->CropEnable |
                                                                  pCommonIFEFDPath->CropEnable;
        pFrameProcessingModuleConfig->SCALE_VID_EN              = pVideoModules->MNDSEnable                  |
                                                                  pCommonIFEVideoPath->MNDSEnable;
        pFrameProcessingModuleConfig->CROP_VID_EN               = pVideoModules->CropEnable                  |
                                                                  pCommonIFEVideoPath->CropEnable;
        pFrameProcessingModuleConfig->DS_4TO1_2ND_POST_CROP_EN  = pVideoModules->DS16PostCropEnable          |
                                                                  pCommonIFEVideoPath->DS16PostCropEnable;
        pFrameProcessingModuleConfig->DS_4TO1_2ND_PRE_CROP_EN   = pVideoModules->DS16PreCropEnable           |
                                                                  pCommonIFEVideoPath->DS16PreCropEnable;
        pFrameProcessingModuleConfig->DS_4TO1_1ST_POST_CROP_EN  = pVideoModules->DS4PostCropEnable           |
                                                                  pCommonIFEVideoPath->DS4PostCropEnable;
        pFrameProcessingModuleConfig->DS_4TO1_1ST_PRE_CROP_EN   = pVideoModules->DS4PreCropEnable            |
                                                                  pCommonIFEVideoPath->DS4PreCropEnable;
        pFrameProcessingModuleConfig->DS_4TO1_Y_1ST_EN          = pVideoModules->DS4LumaEnable               |
                                                                  pCommonIFEVideoPath->DS4LumaEnable;
        pFrameProcessingModuleConfig->DS_4TO1_Y_2ND_EN          = pVideoModules->DS16LumaEnable              |
                                                                  pCommonIFEVideoPath->DS16LumaEnable;
        pFrameProcessingModuleConfig->DS_4TO1_C_1ST_EN          = pVideoModules->DS4ChromaEnable             |
                                                                  pCommonIFEVideoPath->DS4ChromaEnable;
        pFrameProcessingModuleConfig->DS_4TO1_C_2ND_EN          = pVideoModules->DS16ChromaEnable            |
                                                                  pCommonIFEVideoPath->DS16ChromaEnable;
        pFrameProcessingModuleConfig->R2PD_1ST_EN               = pVideoModules->DS4R2PDEnable               |
                                                                  pCommonIFEVideoPath->DS4R2PDEnable;
        pFrameProcessingModuleConfig->R2PD_2ND_EN               = pVideoModules->DS16R2PDEnable              |
                                                                  pCommonIFEVideoPath->DS16R2PDEnable;
        pFrameProcessingModuleConfig->PDAF_OUT_EN               = pIQmodule->PDAFPathEnable                  |
                                                                  pCommonIFEIQConfig->PDAFPathEnable;
        pFrameProcessingModuleConfig->PIXEL_RAW_DUMP_EN         = pIQmodule->pixelRawPathEnable              |
                                                                  pCommonIFEIQConfig->pixelRawPathEnable;

        // Update the FD path module enable config registers
        pFDLumaCropRoundClampConfig->CROP_EN        = pFDModules->RoundClampLumaCropEnable           |
                                                      pCommonIFEFDPath->RoundClampLumaCropEnable;
        pFDLumaCropRoundClampConfig->CH0_ROUND_EN   = pFDModules->RoundClampLumaRoundEnable          |
                                                      pCommonIFEFDPath->RoundClampLumaRoundEnable;
        pFDLumaCropRoundClampConfig->CH0_CLAMP_EN   = pFDModules->RoundClampLumaClampEnable          |
                                                      pCommonIFEFDPath->RoundClampLumaClampEnable;

        pFDChromaCropRoundClampConfig->CROP_EN      = pFDModules->RoundClampChromaCropEnable         |
                                                      pCommonIFEFDPath->RoundClampChromaCropEnable;
        pFDChromaCropRoundClampConfig->CH0_ROUND_EN = pFDModules->RoundClampChromaRoundEnable        |
                                                      pCommonIFEFDPath->RoundClampChromaRoundEnable;
        pFDChromaCropRoundClampConfig->CH0_CLAMP_EN = pFDModules->RoundClampChromaClampEnable        |
                                                      pCommonIFEFDPath->RoundClampChromaClampEnable;

        // Update the video path module enable config registers
        pVideoLumaCropRoundClampConfig->CROP_EN         = pVideoModules->RoundClampLumaCropEnable           |
                                                          pCommonIFEVideoPath->RoundClampLumaCropEnable;
        pVideoLumaCropRoundClampConfig->CH0_ROUND_EN    = pVideoModules->RoundClampLumaRoundEnable          |
                                                          pCommonIFEVideoPath->RoundClampLumaRoundEnable;
        pVideoLumaCropRoundClampConfig->CH0_CLAMP_EN    = pVideoModules->RoundClampLumaRoundEnable          |
                                                          pCommonIFEVideoPath->RoundClampLumaRoundEnable;

        pVideoChromaCropRoundClampConfig->CROP_EN       = pVideoModules->RoundClampChromaCropEnable         |
                                                          pCommonIFEVideoPath->RoundClampChromaCropEnable;
        pVideoChromaCropRoundClampConfig->CH0_ROUND_EN  = pVideoModules->RoundClampChromaRoundEnable        |
                                                          pCommonIFEVideoPath->RoundClampChromaRoundEnable;
        pVideoChromaCropRoundClampConfig->CH0_CLAMP_EN  = pVideoModules->RoundClampChromaRoundEnable        |
                                                          pCommonIFEVideoPath->RoundClampChromaRoundEnable;

        pVideoDS4LumaCropRoundClampConfig->CROP_EN      = pVideoModules->DS4RoundClampLumaCropEnable         |
                                                          pCommonIFEVideoPath->DS4RoundClampLumaCropEnable;
        pVideoDS4LumaCropRoundClampConfig->CH0_ROUND_EN = pVideoModules->DS4RoundClampLumaRoundEnable        |
                                                          pCommonIFEVideoPath->DS4RoundClampLumaRoundEnable;
        pVideoDS4LumaCropRoundClampConfig->CH0_CLAMP_EN = pVideoModules->DS4RoundClampLumaRoundEnable        |
                                                          pCommonIFEVideoPath->DS4RoundClampLumaRoundEnable;

        pVideoDS4ChromaCropRoundClampConfig->CROP_EN        = pVideoModules->DS4RoundClampChromaCropEnable         |
                                                              pCommonIFEVideoPath->DS4RoundClampChromaCropEnable;
        pVideoDS4ChromaCropRoundClampConfig->CH0_ROUND_EN   = pVideoModules->DS4RoundClampChromaRoundEnable        |
                                                              pCommonIFEVideoPath->DS4RoundClampChromaRoundEnable;
        pVideoDS4ChromaCropRoundClampConfig->CH0_CLAMP_EN   = pVideoModules->DS4RoundClampChromaRoundEnable        |
                                                              pCommonIFEVideoPath->DS4RoundClampChromaRoundEnable;

        pVideoDS16LumaCropRoundClampConfig->CROP_EN         = pVideoModules->DS16RoundClampLumaCropEnable           |
                                                              pCommonIFEVideoPath->DS16RoundClampLumaCropEnable;
        pVideoDS16LumaCropRoundClampConfig->CH0_ROUND_EN    = pVideoModules->DS16RoundClampLumaRoundEnable          |
                                                              pCommonIFEVideoPath->DS16RoundClampLumaRoundEnable;
        pVideoDS16LumaCropRoundClampConfig->CH0_CLAMP_EN    = pVideoModules->DS16RoundClampLumaRoundEnable          |
                                                              pCommonIFEVideoPath->DS16RoundClampLumaRoundEnable;

        pVideoDS16ChromaCropRoundClampConfig->CROP_EN       = pVideoModules->DS16RoundClampChromaCropEnable           |
                                                              pCommonIFEVideoPath->DS16RoundClampChromaCropEnable;
        pVideoDS16ChromaCropRoundClampConfig->CH0_ROUND_EN  = pVideoModules->DS16RoundClampChromaRoundEnable          |
                                                              pCommonIFEVideoPath->DS16RoundClampChromaRoundEnable;
        pVideoDS16ChromaCropRoundClampConfig->CH0_CLAMP_EN  = pVideoModules->DS16RoundClampChromaRoundEnable          |
                                                              pCommonIFEVideoPath->DS16RoundClampChromaRoundEnable;

        // Update the stats modules enable config registers
        pStatsEnable->HDR_BE_EN     = pIFEOutputPathInfo[IFEOutputPortStatsHDRBE].path;
        pStatsEnable->HDR_BHIST_EN  = pIFEOutputPathInfo[IFEOutputPortStatsHDRBHIST].path;
        pStatsEnable->BAF_EN        = pIFEOutputPathInfo[IFEOutputPortStatsBF].path;
        pStatsEnable->AWB_BG_EN     = pIFEOutputPathInfo[IFEOutputPortStatsAWBBG].path;
        pStatsEnable->SKIN_BHIST_EN = pIFEOutputPathInfo[IFEOutputPortStatsBHIST].path;
        pStatsEnable->RS_EN         = pIFEOutputPathInfo[IFEOutputPortStatsRS].path;
        pStatsEnable->CS_EN         = pIFEOutputPathInfo[IFEOutputPortStatsCS].path;
        pStatsEnable->IHIST_EN      = pIFEOutputPathInfo[IFEOutputPortStatsIHIST].path;
        pStatsEnable->AEC_BG_EN     = pIFEOutputPathInfo[IFEOutputPortStatsTLBG].path;

        // Update the stats modules control registers
        pStatsConfig->HDR_BE_FIELD_SEL      = pStatsControl->HDRBEFieldSelect       | pCommonIFEStatsControl->HDRBEFieldSelect;
        pStatsConfig->HDR_STATS_SITE_SEL    = pStatsControl->HDRSatsSiteSelect      |
                                              pCommonIFEStatsControl->HDRSatsSiteSelect;
        pStatsConfig->BHIST_BIN_UNIFORMITY  = pStatsControl->BhistBinUniformity     |
                                              pCommonIFEStatsControl->BhistBinUniformity;
        pStatsConfig->AWB_BG_QUAD_SYNC_EN   = pStatsControl->AWBBGQuadSyncEnable    |
                                              pCommonIFEStatsControl->AWBBGQuadSyncEnable |
                                              1;
        pStatsConfig->COLOR_CONV_EN         = pStatsControl->colorConversionEnable  |
                                              pCommonIFEStatsControl->colorConversionEnable;
        pStatsConfig->RS_SHIFT_BITS         = pStatsControl->RSShiftBits            |
                                              pCommonIFEStatsControl->RSShiftBits;
        pStatsConfig->CS_SHIFT_BITS         = pStatsControl->CSShiftBits            | pCommonIFEStatsControl->CSShiftBits;
        pStatsConfig->HDR_BHIST_FIELD_SEL   = pStatsControl->HDRBhistFieldSelect    |
                                              pCommonIFEStatsControl->HDRBhistFieldSelect;
        pStatsConfig->HDR_BHIST_CHAN_SEL    = pStatsControl->HDRBhistChannelSelect  |
                                              pCommonIFEStatsControl->HDRBhistChannelSelect;
        pStatsConfig->BHIST_CHAN_SEL        = pStatsControl->BhistChannelSelect     |
                                              pCommonIFEStatsControl->BhistChannelSelect;
        pStatsConfig->IHIST_CHAN_SEL        = pStatsControl->IhistChannelSelect     |
                                              pCommonIFEStatsControl->IhistChannelSelect;
        pStatsConfig->IHIST_SITE_SEL        = pStatsControl->IHistSiteSelect        | pCommonIFEStatsControl->IHistSiteSelect;
        pStatsConfig->IHIST_SHIFT_BITS      = pStatsControl->IHistShiftBits         | pCommonIFEStatsControl->IHistShiftBits;

        // Update the DSP select config
        pDSPConfig->DSP_TO_SEL              = pInputData->pCalculatedData->moduleEnable.DSPConfig.DSPSelect;


        if (NULL != pInputData->pCmdBuffer)
        {
            CmdBuffer* pCmdBuffer = pInputData->pCmdBuffer;

            // Update the raw output path
            if (pIFEOutputPathInfo[IFEOutputPortCAMIFRaw].path)
            {
                raw_dump = 0;
            }
            else if (pIFEOutputPathInfo[IFEOutputPortLSCRaw].path)
            {
                raw_dump = 1;
            }
            else if (pIFEOutputPathInfo[IFEOutputPortGTMRaw].path)
            {
                raw_dump = 2;
            }


            CAMX_LOG_VERBOSE(CamxLogGroupISP, "Lens processing module enable config                          [0x%x]",
                             m_moduleConfig.lensProcessingModuleConfig.u32All);
            CAMX_LOG_VERBOSE(CamxLogGroupISP, "Frame processing module enable config                         [0x%x]",
                             m_moduleConfig.frameProcessingModuleConfig.u32All);
            CAMX_LOG_VERBOSE(CamxLogGroupISP, "Color processing module enable config                         [0x%x]",
                             m_moduleConfig.colorProcessingModuleConfig.u32All);
            CAMX_LOG_VERBOSE(CamxLogGroupISP, "FD path Round, Clamp and Crop module enable config            [0x%x]",
                             m_moduleConfig.FDLumaCropRoundClampConfig.u32All);
            CAMX_LOG_VERBOSE(CamxLogGroupISP, "FD Chroma path Round, Clamp and Crop module enable config     [0x%x]",
                             m_moduleConfig.FDChromaCropRoundClampConfig.u32All);
            CAMX_LOG_VERBOSE(CamxLogGroupISP, "Full Luma path Round, Clamp and Crop module enable config     [0x%x]",
                             m_moduleConfig.videoLumaCropRoundClampConfig.u32All);
            CAMX_LOG_VERBOSE(CamxLogGroupISP, "Full Chroma path Round, Clamp and Crop module enable config   [0x%x]",
                             m_moduleConfig.videoChromaCropRoundClampConfig.u32All);
            CAMX_LOG_VERBOSE(CamxLogGroupISP, "DS4 Luma path Round, Clamp and Crop module enable config      [0x%x]",
                             m_moduleConfig.videoDS4LumaCropRoundClampConfig.u32All);
            CAMX_LOG_VERBOSE(CamxLogGroupISP, "DS4 Chroma path Round, Clamp and Crop module enable config    [0x%x]",
                             m_moduleConfig.videoDS4ChromaCropRoundClampConfig.u32All);
            CAMX_LOG_VERBOSE(CamxLogGroupISP, "DS16 Luma path Round, Clamp and Crop module enable config     [0x%x]",
                             m_moduleConfig.videoDS16LumaCropRoundClampConfig.u32All);
            CAMX_LOG_VERBOSE(CamxLogGroupISP, "DS16 Chroma path Round, Clamp and Crop module enable config   [0x%x]",
                             m_moduleConfig.videoDS16ChromaCropRoundClampConfig.u32All);
            CAMX_LOG_VERBOSE(CamxLogGroupISP, "Stats module enable config                                    [0x%x]",
                             m_moduleConfig.statsEnable.u32All);
            CAMX_LOG_VERBOSE(CamxLogGroupISP, "Stats general configuration register                          [0x%x]",
                             m_moduleConfig.statsConfig.u32All);
            CAMX_LOG_VERBOSE(CamxLogGroupISP, "raw_dump                                                      [0x%x]",
                             raw_dump);

            result = PacketBuilder::WriteRegRange(
                         pCmdBuffer,
                         regIFE_IFE_0_VFE_MODULE_LENS_EN,
                         (sizeof(m_moduleConfig.lensProcessingModuleConfig) / RegisterWidthInBytes),
                         reinterpret_cast<UINT32*>(&m_moduleConfig.lensProcessingModuleConfig));

            result = PacketBuilder::WriteRegRange(
                         pCmdBuffer,
                         regIFE_IFE_0_VFE_MODULE_COLOR_EN,
                         (sizeof(m_moduleConfig.colorProcessingModuleConfig) / RegisterWidthInBytes),
                         reinterpret_cast<UINT32*>(&m_moduleConfig.colorProcessingModuleConfig));

            result |= PacketBuilder::WriteRegRange(
                         pCmdBuffer,
                         regIFE_IFE_0_VFE_MODULE_ZOOM_EN,
                         (sizeof(m_moduleConfig.frameProcessingModuleConfig) / RegisterWidthInBytes),
                         reinterpret_cast<UINT32*>(&m_moduleConfig.frameProcessingModuleConfig));

            result |= PacketBuilder::WriteRegRange(
                         pCmdBuffer,
                         regIFE_IFE_0_VFE_FD_OUT_Y_CROP_RND_CLAMP_CFG,
                         (sizeof(m_moduleConfig.FDLumaCropRoundClampConfig) / RegisterWidthInBytes),
                         reinterpret_cast<UINT32*>(&m_moduleConfig.FDLumaCropRoundClampConfig));

            result |= PacketBuilder::WriteRegRange(
                         pCmdBuffer,
                         regIFE_IFE_0_VFE_FD_OUT_C_CROP_RND_CLAMP_CFG,
                         (sizeof(m_moduleConfig.FDChromaCropRoundClampConfig) / RegisterWidthInBytes),
                         reinterpret_cast<UINT32*>(&m_moduleConfig.FDChromaCropRoundClampConfig));

            result |= PacketBuilder::WriteRegRange(
                         pCmdBuffer,
                         regIFE_IFE_0_VFE_FULL_OUT_Y_CROP_RND_CLAMP_CFG,
                         (sizeof(m_moduleConfig.videoLumaCropRoundClampConfig) / RegisterWidthInBytes),
                         reinterpret_cast<UINT32*>(&m_moduleConfig.videoLumaCropRoundClampConfig));

            result |= PacketBuilder::WriteRegRange(
                         pCmdBuffer,
                         regIFE_IFE_0_VFE_FULL_OUT_C_CROP_RND_CLAMP_CFG,
                         (sizeof(m_moduleConfig.videoChromaCropRoundClampConfig) / RegisterWidthInBytes),
                         reinterpret_cast<UINT32*>(&m_moduleConfig.videoChromaCropRoundClampConfig));

            result |= PacketBuilder::WriteRegRange(
                         pCmdBuffer,
                         regIFE_IFE_0_VFE_DS4_Y_CROP_RND_CLAMP_CFG,
                         (sizeof(m_moduleConfig.videoDS4LumaCropRoundClampConfig) / RegisterWidthInBytes),
                         reinterpret_cast<UINT32*>(&m_moduleConfig.videoDS4LumaCropRoundClampConfig));

            result |= PacketBuilder::WriteRegRange(
                         pCmdBuffer,
                         regIFE_IFE_0_VFE_DS4_C_CROP_RND_CLAMP_CFG,
                         (sizeof(m_moduleConfig.videoDS4ChromaCropRoundClampConfig) / RegisterWidthInBytes),
                         reinterpret_cast<UINT32*>(&m_moduleConfig.videoDS4ChromaCropRoundClampConfig));

            result |= PacketBuilder::WriteRegRange(
                         pCmdBuffer,
                         regIFE_IFE_0_VFE_DS16_Y_CROP_RND_CLAMP_CFG,
                         (sizeof(m_moduleConfig.videoDS16LumaCropRoundClampConfig) / RegisterWidthInBytes),
                         reinterpret_cast<UINT32*>(&m_moduleConfig.videoDS16LumaCropRoundClampConfig));

            result |= PacketBuilder::WriteRegRange(
                         pCmdBuffer,
                         regIFE_IFE_0_VFE_DS16_C_CROP_RND_CLAMP_CFG,
                         (sizeof(m_moduleConfig.videoDS16ChromaCropRoundClampConfig) / RegisterWidthInBytes),
                         reinterpret_cast<UINT32*>(&m_moduleConfig.videoDS16ChromaCropRoundClampConfig));

            result |= PacketBuilder::WriteRegRange(
                         pCmdBuffer,
                         regIFE_IFE_0_VFE_MODULE_STATS_EN,
                         (sizeof(m_moduleConfig.statsEnable) / RegisterWidthInBytes),
                         reinterpret_cast<UINT32*>(&m_moduleConfig.statsEnable));

            result |= PacketBuilder::WriteRegRange(
                         pCmdBuffer,
                         regIFE_IFE_0_VFE_STATS_CFG,
                         (sizeof(m_moduleConfig.statsConfig) / RegisterWidthInBytes),
                         reinterpret_cast<UINT32*>(&m_moduleConfig.statsConfig));

            if ((pIFEOutputPathInfo[IFEOutputPortCAMIFRaw].path) ||
                (pIFEOutputPathInfo[IFEOutputPortLSCRaw].path)   ||
                (pIFEOutputPathInfo[IFEOutputPortGTMRaw].path))
            {
                result |= PacketBuilder::WriteRegRange(
                             pCmdBuffer,
                             regIFE_IFE_0_VFE_PIXEL_RAW_DUMP_CFG,
                             (sizeof(raw_dump) / RegisterWidthInBytes),
                             reinterpret_cast<UINT32*>(&raw_dump));
            }
            result |= PacketBuilder::WriteRegRange(
                        pCmdBuffer,
                        regIFE_IFE_0_VFE_DSP_TO_SEL,
                        (sizeof(m_moduleConfig.DSPConfig) / RegisterWidthInBytes),
                        reinterpret_cast<UINT32*>(&m_moduleConfig.DSPConfig));

            if (CamxResultSuccess != result)
            {
                CAMX_ASSERT_ALWAYS_MESSAGE("Module enable register write failed");
            }
        }
        else
        {
            CAMX_ASSERT_ALWAYS_MESSAGE("Invalid input parameter");
            result = CamxResultEInvalidArg;
        }
    }
    else
    {
        CAMX_ASSERT_ALWAYS_MESSAGE("Invalid input parameter");
        result = CamxResultEInvalidArg;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Titan170IFE::GetISPIQModulesOfType()
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID Titan170IFE::GetISPIQModulesOfType(
    ISPIQModuleType moduleType,
    VOID*           pModuleInfo)
{
    UINT            numOfIQModule = sizeof(IFEIQModuleItems2) / sizeof(IFEIQModuleInfo);
    IFEIQModuleInfo* pIFEmoduleInfo = static_cast<IFEIQModuleInfo*>(pModuleInfo);

    for (UINT i = 0; i < numOfIQModule; i++)
    {
        if (IFEIQModuleItems2[i].moduleType == moduleType)
        {
            Utils::Memcpy(pIFEmoduleInfo, &IFEIQModuleItems2[i], sizeof(IFEIQModuleInfo));
            break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Titan170IFE::FillConfigTuningMetadata()
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID Titan170IFE::FillConfigTuningMetadata(
    ISPInputData* pInputData)
{
    IFETuningModuleEnableConfig* pIFEEnableConfig = &pInputData->pIFETuningMetadata->IFEEnableConfig;

    pIFEEnableConfig->lensProcessingModuleConfig            = m_moduleConfig.lensProcessingModuleConfig.u32All;
    pIFEEnableConfig->colorProcessingModuleConfig           = m_moduleConfig.colorProcessingModuleConfig.u32All;
    pIFEEnableConfig->frameProcessingModuleConfig           = m_moduleConfig.frameProcessingModuleConfig.u32All;
    pIFEEnableConfig->FDLumaCropRoundClampConfig            = m_moduleConfig.FDLumaCropRoundClampConfig.u32All;
    pIFEEnableConfig->FDChromaCropRoundClampConfig          = m_moduleConfig.FDChromaCropRoundClampConfig.u32All;
    pIFEEnableConfig->fullLumaCropRoundClampConfig          = m_moduleConfig.videoLumaCropRoundClampConfig.u32All;
    pIFEEnableConfig->fullChromaCropRoundClampConfig        = m_moduleConfig.videoChromaCropRoundClampConfig.u32All;
    pIFEEnableConfig->DS4LumaCropRoundClampConfig           = m_moduleConfig.videoDS4LumaCropRoundClampConfig.u32All;
    pIFEEnableConfig->DS4ChromaCropRoundClampConfig         = m_moduleConfig.videoDS4ChromaCropRoundClampConfig.u32All;
    pIFEEnableConfig->DS16LumaCropRoundClampConfig          = m_moduleConfig.videoDS16LumaCropRoundClampConfig.u32All;
    pIFEEnableConfig->DS16ChromaCropRoundClampConfig        = m_moduleConfig.videoDS16ChromaCropRoundClampConfig.u32All;
    pIFEEnableConfig->statsEnable                           = m_moduleConfig.statsEnable.u32All;
    pIFEEnableConfig->statsConfig                           = m_moduleConfig.statsConfig.u32All;
    pIFEEnableConfig->dspConfig                             = m_moduleConfig.DSPConfig.u32All;
    pIFEEnableConfig->frameProcessingDisplayModuleConfig    = m_moduleConfig.frameProcessingDisplayModuleConfig.u32All;
    pIFEEnableConfig->displayFullLumaCropRoundClampConfig   = m_moduleConfig.displayFullLumaCropRoundClampConfig.u32All;
    pIFEEnableConfig->displayFullChromaCropRoundClampConfig = m_moduleConfig.displayFullChromaCropRoundClampConfig.u32All;
    pIFEEnableConfig->displayDS4LumaCropRoundClampConfig    = m_moduleConfig.displayDS4LumaCropRoundClampConfig.u32All;
    pIFEEnableConfig->displayDS4ChromaCropRoundClampConfig  = m_moduleConfig.displayDS4ChromaCropRoundClampConfig.u32All;
    pIFEEnableConfig->displayDS16LumaCropRoundClampConfig   = m_moduleConfig.displayDS16LumaCropRoundClampConfig.u32All;
    pIFEEnableConfig->displayDS16ChromaCropRoundClampConfig = m_moduleConfig.displayDS16ChromaCropRoundClampConfig.u32All;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Titan170IFE::DumpTuningMetadata()
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID Titan170IFE::DumpTuningMetadata(
    ISPInputData*       pInputData,
    DebugDataWriter*    pDebugDataWriter)
{
    CamxResult              result          = CamxResultSuccess;
    IFETuningMetadata17x*   pMetadata17x    = &pInputData->pIFETuningMetadata->metadata17x;

    FillConfigTuningMetadata(pInputData);

    // Add IFE tuning metadata tags
    result = pDebugDataWriter->AddDataTag(DebugDataTagID::TuningIFEModuleEnableConfig,
                                          DebugDataTagType::TuningIFEEnableConfig,
                                          1,
                                          &pInputData->pIFETuningMetadata->IFEEnableConfig,
                                          sizeof(pInputData->pIFETuningMetadata->IFEEnableConfig));
    if (CamxResultSuccess != result)
    {
        CAMX_LOG_WARN(CamxLogGroupISP, "AddDataTag failed with error: %d", result);
        result = CamxResultSuccess; // Non-fatal error
    }

    if (TRUE == pInputData->pCalculatedData->moduleEnable.IQModules.rolloffEnable)
    {
        result = pDebugDataWriter->AddDataTag(DebugDataTagID::TuningIFELSC34Register,
                                              DebugDataTagType::UInt32,
                                              CAMX_ARRAY_SIZE(pMetadata17x->IFELSCData.LSCConfig1),
                                              &pMetadata17x->IFELSCData.LSCConfig1,
                                              sizeof(pMetadata17x->IFELSCData.LSCConfig1));
        if (CamxResultSuccess != result)
        {
            CAMX_LOG_WARN(CamxLogGroupISP, "AddDataTag failed with error: %d", result);
            result = CamxResultSuccess; // Non-fatal error
        }

        result = pDebugDataWriter->AddDataTag(DebugDataTagID::TuningIFELSC34PackedMesh,
                                              DebugDataTagType::TuningLSC34MeshLUT,
                                              1,
                                              &pMetadata17x->IFEDMIPacked.LSCMesh,
                                              sizeof(pMetadata17x->IFEDMIPacked.LSCMesh));
        if (CamxResultSuccess != result)
        {
            CAMX_LOG_WARN(CamxLogGroupISP, "AddDataTag failed with error: %d", result);
            result = CamxResultSuccess; // Non-fatal error
        }
    }

    if (TRUE == pInputData->pCalculatedData->moduleEnable.IQModules.gammaEnable)
    {
        result = pDebugDataWriter->AddDataTag(DebugDataTagID::TuningIFEGamma16Register17x,
                                              DebugDataTagType::UInt32,
                                              CAMX_ARRAY_SIZE(pMetadata17x->IFEGammaData.gammaConfig),
                                              &pMetadata17x->IFEGammaData.gammaConfig,
                                              sizeof(pMetadata17x->IFEGammaData.gammaConfig));
        if (CamxResultSuccess != result)
        {
            CAMX_LOG_WARN(CamxLogGroupISP, "AddDataTag failed with error: %d", result);
            result = CamxResultSuccess; // Non-fatal error
        }

        result = pDebugDataWriter->AddDataTag(DebugDataTagID::TuningIFEGamma16PackedLUT,
                                              DebugDataTagType::TuningGammaCurve,
                                              CAMX_ARRAY_SIZE(pMetadata17x->IFEDMIPacked.gamma),
                                              &pMetadata17x->IFEDMIPacked.gamma,
                                              sizeof(pMetadata17x->IFEDMIPacked.gamma));
        if (CamxResultSuccess != result)
        {
            CAMX_LOG_WARN(CamxLogGroupISP, "AddDataTag failed with error: %d", result);
            result = CamxResultSuccess; // Non-fatal error
        }
    }

    if (TRUE == pInputData->pCalculatedData->moduleEnable.IQModules.GTMEnable)
    {
        result = pDebugDataWriter->AddDataTag(DebugDataTagID::TuningIFEGTM10Register17x,
                                              DebugDataTagType::UInt32,
                                              CAMX_ARRAY_SIZE(pMetadata17x->IFEGTMData.GTMConfig),
                                              &pMetadata17x->IFEGTMData.GTMConfig,
                                              sizeof(pMetadata17x->IFEGTMData.GTMConfig));
        if (CamxResultSuccess != result)
        {
            CAMX_LOG_WARN(CamxLogGroupISP, "AddDataTag failed with error: %d", result);
            result = CamxResultSuccess; // Non-fatal error
        }

        result = pDebugDataWriter->AddDataTag(DebugDataTagID::TuningIFEGTM10PackedLUT,
                                              DebugDataTagType::TuningGTMLUT,
                                              1,
                                              &pMetadata17x->IFEDMIPacked.GTM,
                                              sizeof(pMetadata17x->IFEDMIPacked.GTM));
        if (CamxResultSuccess != result)
        {
            CAMX_LOG_WARN(CamxLogGroupISP, "AddDataTag failed with error: %d", result);
            result = CamxResultSuccess; // Non-fatal error
        }
    }

    if ((TRUE == pInputData->pCalculatedData->moduleEnable.IQModules.HDRMACEnable) ||
        (TRUE == pInputData->pCalculatedData->moduleEnable.IQModules.HDRReconEnable))
    {
        result = pDebugDataWriter->AddDataTag(DebugDataTagID::TuningIFEHDR2xRegister,
                                              DebugDataTagType::TuningIFE2xHDR,
                                              1,
                                              &pMetadata17x->IFEHDRData,
                                              sizeof(pMetadata17x->IFEHDRData));
        if (CamxResultSuccess != result)
        {
            CAMX_LOG_WARN(CamxLogGroupISP, "AddDataTag failed with error: %d", result);
            result = CamxResultSuccess; // Non-fatal error
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Titan170IFE::FillCGCConfig
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID Titan170IFE::FillCGCConfig(
    CmdBuffer*  pCmdBuffer)
{
    if (NULL != pCmdBuffer)
    {
        PacketBuilder::WriteInterleavedRegs(pCmdBuffer, IFE170CGCNumRegs, IFE170CGCOnCmds);
    }
    else
    {
        CAMX_LOG_ERROR(CamxLogGroupISP, "Unable to Set CGC Commands");
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Titan170IFE::FillFlushConfig
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID Titan170IFE::FillFlushConfig(
    CmdBuffer*  pCmdBuffer)
{
    if (NULL != pCmdBuffer)
    {
        IFE_IFE_0_VFE_ISP_MODULE_FLUSH_HALT_CFG  flushHaltCfg;

        /* Need to update based on calculation */
        UINT32 value = 0b1010101010101010;
        flushHaltCfg.bitfields.FLUSH_HALT_GEN_EN = 1;
        flushHaltCfg.bitfields.FLUSH_HALT_PATTERN = value;


        PacketBuilder::WriteRegRange(pCmdBuffer,
            regIFE_IFE_0_VFE_ISP_MODULE_FLUSH_HALT_CFG,
            sizeof(flushHaltCfg) / RegisterWidthInBytes,
            reinterpret_cast<UINT32*>(&flushHaltCfg));
    }
    else
    {
        CAMX_LOG_ERROR(CamxLogGroupISP, "Unable to Set FLUSH HALT Config");
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Titan170IFE::GetIFEDefaultConfig()
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID Titan170IFE::GetIFEDefaultConfig(
    IFEDefaultModuleConfig* pDefaultData)
{
    pDefaultData->RSStatsHorizRegions   = RSStats14MaxHorizRegions;
    pDefaultData->RSStatsVertRegions    = RSStats14MaxVertRegions;
    pDefaultData->CSStatsHorizRegions   = CSStats14Titan17xMaxHorizRegions;
    pDefaultData->CSStatsVertRegions    = CSStats14Titan17xMaxVertRegions;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Titan170IFE::UpdateDMIBankSelectValue
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID Titan170IFE::UpdateDMIBankSelectValue(
    IFEDMIBankUpdate* pBankUpdate,
    BOOL              isBankValueZero)
{
    pBankUpdate->isValid = TRUE;

    // The actual bank value will be set in each IQ module after toggled first.
    const UINT32 bankValue = (TRUE == isBankValueZero) ? 0 : 1;

    pBankUpdate->pedestalBank       = bankValue;
    pBankUpdate->linearizaionBank   = bankValue;
    pBankUpdate->ABFBank            = bankValue;
    pBankUpdate->LSCBank            = bankValue;
    pBankUpdate->GTMBank            = bankValue;
    pBankUpdate->gammaBank          = bankValue;
    pBankUpdate->BFStatsDMIBank     = bankValue;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Titan170IFE::SetupHangRegDump
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID Titan170IFE::SetupHangRegDump(
    VOID*         pHangBuffer,
    IFEModuleMode mode,
    BOOL          isPerFrameDump)
{
    UINT                    numberOfRegReadInfoRegions  = 0;
    BYTE*                   pDumpPtr;
    IFERegDumpDescriptor*   pLeftRegDumpDescriptor      = NULL;
    IFERegDumpDescriptor*   pRightRegDumpDescriptor     = NULL;
    IFERegDumpInfo*         pRegDumpInfo                = NULL;
    UINT                    numRegReadRange             = 0;
    const StaticSettings*   pSettings                   = HwEnvironment::GetInstance()->GetStaticSettings();
    UINT32                  bufferSize;

    if (NULL != pHangBuffer)
    {
        pDumpPtr           = reinterpret_cast<BYTE*>(pHangBuffer);
        pRegDumpInfo       = reinterpret_cast<IFERegDumpInfo*>(pHangBuffer);

        if (FALSE == isPerFrameDump)
        {
            bufferSize = m_hangDumpOutputBufferSize;
        }
        else
        {
            bufferSize = m_perFrameDumpOutputBufferSize;
        }

        if (IFEModuleMode::DualIFENormal == mode)
        {
            pRegDumpInfo->numberOfRegDumps  = 2;
            pRegDumpInfo->regDumpOffsets[0] = sizeof(IFERegDumpInfo);
            pRegDumpInfo->regDumpOffsets[1] = sizeof(IFERegDumpInfo) + sizeof(IFERegDumpDescriptor);

            pLeftRegDumpDescriptor = reinterpret_cast<IFERegDumpDescriptor*>(pDumpPtr + pRegDumpInfo->regDumpOffsets[0]);

            pLeftRegDumpDescriptor->type         = IFERegDumpTypeIFELeft;

            pLeftRegDumpDescriptor->bufferOffset = sizeof(IFERegDumpInfo) +
                                                   (sizeof(IFERegDumpDescriptor) * pRegDumpInfo->numberOfRegDumps);
            pLeftRegDumpDescriptor->bufferSize   = bufferSize;


            pRightRegDumpDescriptor = reinterpret_cast<IFERegDumpDescriptor*>(pDumpPtr + pRegDumpInfo->regDumpOffsets[1]);

            pRightRegDumpDescriptor->type         = IFERegDumpTypeIFERight;

            pRightRegDumpDescriptor->bufferOffset = sizeof(IFERegDumpInfo) +
                                                    (sizeof(IFERegDumpDescriptor) * pRegDumpInfo->numberOfRegDumps) +
                                                    bufferSize;
            pRightRegDumpDescriptor->bufferSize   = bufferSize;

        }
        else
        {
            pRegDumpInfo->numberOfRegDumps       = 1;
            pRegDumpInfo->regDumpOffsets[0]      = sizeof(IFERegDumpInfo);
            pLeftRegDumpDescriptor               = reinterpret_cast<IFERegDumpDescriptor*>(pDumpPtr +
                                                                                           pRegDumpInfo->regDumpOffsets[0]);
            pLeftRegDumpDescriptor->bufferOffset = sizeof(IFERegDumpInfo) +
                                                   (sizeof(IFERegDumpDescriptor) * pRegDumpInfo->numberOfRegDumps);
            pLeftRegDumpDescriptor->bufferSize   = bufferSize;
            pLeftRegDumpDescriptor->type         = IFERegDumpTypeIFELeft;
        }

        BOOL includeReg = TRUE;

        for (UINT32 index = 0; index < sizeof(IFERegDump) / sizeof(RegDumpInfo); index++)
        {
            // include register if indicated by the ife reg dump mask for per frame dumps
            if (TRUE == isPerFrameDump)
            {
                includeReg = Utils::IsBitSet(pSettings->IFERegDumpMask, IFERegDump[index].regGroupType);
            }
            // TO -DO: Fill up the DMI Rgeisters Info also
            if ((TRUE == includeReg) && (IFERegDumpReadTypeReg == IFERegDump[index].regReadInfo.readType))
            {
                FillIFERegReadInfo(&pLeftRegDumpDescriptor->regReadInfo[numRegReadRange],
                                   IFERegDump[index].regReadInfo.regDescriptor.regReadCmd.offset,
                                   IFERegDump[index].regReadInfo.regDescriptor.regReadCmd.numberOfRegisters);
                if (IFEModuleMode::DualIFENormal == mode)
                {
                    FillIFERegReadInfo(&pRightRegDumpDescriptor->regReadInfo[numRegReadRange],
                                       IFERegDump[index].regReadInfo.regDescriptor.regReadCmd.offset,
                                       IFERegDump[index].regReadInfo.regDescriptor.regReadCmd.numberOfRegisters);
                }
                numRegReadRange++;
            }
        }

        pLeftRegDumpDescriptor->numberOfRegReadInfo = numRegReadRange;
        if (IFEModuleMode::DualIFENormal == mode)
        {
            pRightRegDumpDescriptor->numberOfRegReadInfo = numRegReadRange;
        }
    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Titan170IFE::ParseHangRegDump
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID Titan170IFE::ParseHangRegDump(
    VOID* pHangBuffer,
    INT   leftHangFd,
    INT   rightHangFd)
{
    UINT                  numberOfRegReadInfoRegions = 0;
    BYTE*                 pDumpPtr;
    IFERegDumpDescriptor* pLeftRegDumpDescriptor = NULL;
    IFERegDumpDescriptor* pRightRegDumpDescriptor = NULL;
    IFERegDumpInfo*       pRegDumpInfo = NULL;
    CAMX_LOG_ERROR(CamxLogGroupISP, "pHangBuffer %p", pHangBuffer);
    if (NULL != pHangBuffer)
    {
        pDumpPtr               = reinterpret_cast<BYTE*>(pHangBuffer);
        pRegDumpInfo           = reinterpret_cast<IFERegDumpInfo*>(pHangBuffer);
        pLeftRegDumpDescriptor = reinterpret_cast<IFERegDumpDescriptor*>(pDumpPtr + pRegDumpInfo->regDumpOffsets[0]);
        CAMX_LOG_ERROR(CamxLogGroupISP, "pDumpPtr %p pRegDumpInfo %p pLeftRegDumpDescriptor%p numberOfRegDumps %d", pDumpPtr,
            pRegDumpInfo, pLeftRegDumpDescriptor, pRegDumpInfo->numberOfRegDumps);
        if (pRegDumpInfo->numberOfRegDumps > 1)
        {
            pRightRegDumpDescriptor = reinterpret_cast<IFERegDumpDescriptor*>(pDumpPtr + pRegDumpInfo->regDumpOffsets[1]);
        }

        IFERegDumpOutput* pOutput = reinterpret_cast<IFERegDumpOutput*>(pDumpPtr + pLeftRegDumpDescriptor->bufferOffset);

        UINT32* pRegData = &pOutput->data[0];
        UINT32  offset   = 0;
        UINT32  value    = 0;
        CAMX_LOG_ERROR(CamxLogGroupISP, "leftHangFd %d pRegData %p pOutput->data %d pOutput->requestID %d", leftHangFd,
            pRegData,
            pOutput->data[0],
            pOutput->requestID);
        if (0 <= leftHangFd)
        {
            for (UINT32 index = 0; index < (pOutput->numberOfBytes / RegisterWidthInBytes) / 2; index ++)
            {
                offset = *pRegData;
                pRegData++;
                value  = *pRegData;
                CAMX_LOG_ERROR(CamxLogGroupISP, "leftHangFd %d 0x%X=0x%X", leftHangFd, offset, value);
                CAMX_LOG_TO_FILE(leftHangFd, 0, "0x%X=0x%X", offset, value);
                pRegData++;
            }
        }

        if (NULL != pRightRegDumpDescriptor)
        {
            pOutput  = reinterpret_cast<IFERegDumpOutput*>(pDumpPtr + pRightRegDumpDescriptor->bufferOffset);
            pRegData = &pOutput->data[0];

            if (0 <= rightHangFd)
            {
                for (UINT32 index = 0; index < (pOutput->numberOfBytes / RegisterWidthInBytes) / 2; index ++)
                {
                    offset = *pRegData;
                    pRegData++;
                    value  = *pRegData;
                    CAMX_LOG_ERROR(CamxLogGroupISP, "rightHangFd %d 0x%X=0x%X", rightHangFd, offset, value);
                    CAMX_LOG_TO_FILE(rightHangFd, 0, "0x%X=0x%X", offset, value);
                    pRegData++;
                }
            }
        }
    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Titan170IFE::Titan170IFE
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Titan170IFE::Titan170IFE()
{
    const StaticSettings* pSettings = HwEnvironment::GetInstance()->GetStaticSettings();

    for (UINT32 index = 0; index < sizeof(IFERegDump) / sizeof(RegDumpInfo); index++)
    {
        if (IFERegDumpReadTypeReg == IFERegDump[index].regReadInfo.readType)
        {
            m_hangDumpOutputBufferSize+= IFERegDump[index].regReadInfo.regDescriptor.regReadCmd.numberOfRegisters;
        }
        else if (IFERegDumpReadTypeDMI == IFERegDump[index].regReadInfo.readType)
        {
            m_hangDumpOutputBufferSize+= IFERegDump[index].regReadInfo.regDescriptor.dmiReadCmd.regRangeCmd.numberOfRegisters +
                                         IFERegDump[index].regReadInfo.regDescriptor.dmiReadCmd.numberOfRegWrites;
        }
    }

    m_hangDumpOutputBufferSize = sizeof(IFERegDumpOutput) + (m_hangDumpOutputBufferSize * sizeof(UINT32) * 2);

    SetHangDumpBufferSize(sizeof(IFERegDumpInfo)                                +
                          (sizeof(IFERegDumpDescriptor) * MaxRegDumpOffsets)    +
                          (m_hangDumpOutputBufferSize   * MaxRegDumpOffsets));

    // TO -DO: follow up on per frame dump output buffer size
    m_perFrameDumpOutputBufferSize = m_hangDumpOutputBufferSize;

    UINT regDumpBufferSize = 0;
    if (TRUE == pSettings->enableIFERegDump)
    {
        regDumpBufferSize = sizeof(IFERegDumpInfo)                                  +
                            (sizeof(IFERegDumpDescriptor) * MaxRegDumpOffsets)      +
                            (m_perFrameDumpOutputBufferSize   * MaxRegDumpOffsets);
    }
    SetRegDumpBufferSize(regDumpBufferSize);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Titan170IFE::~Titan170IFE
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Titan170IFE::~Titan170IFE()
{

}

CAMX_NAMESPACE_END
