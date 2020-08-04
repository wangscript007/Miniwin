//<MStar Software>
//******************************************************************************
// MStar Software
// Copyright (c) 2010 - 2012 MStar Semiconductor, Inc. All rights reserved.
// All software, firmware and related documentation herein ("MStar Software") are
// intellectual property of MStar Semiconductor, Inc. ("MStar") and protected by
// law, including, but not limited to, copyright law and international treaties.
// Any use, modification, reproduction, retransmission, or republication of all
// or part of MStar Software is expressly prohibited, unless prior written
// permission has been granted by MStar.
//
// By accessing, browsing and/or using MStar Software, you acknowledge that you
// have read, understood, and agree, to be bound by below terms ("Terms") and to
// comply with all applicable laws and regulations:
//
// 1. MStar shall retain any and all right, ownership and interest to MStar
//    Software and any modification/derivatives thereof.
//    No right, ownership, or interest to MStar Software and any
//    modification/derivatives thereof is transferred to you under Terms.
//
// 2. You understand that MStar Software might include, incorporate or be
//    supplied together with third party`s software and the use of MStar
//    Software may require additional licenses from third parties.
//    Therefore, you hereby agree it is your sole responsibility to separately
//    obtain any and all third party right and license necessary for your use of
//    such third party`s software.
//
// 3. MStar Software and any modification/derivatives thereof shall be deemed as
//    MStar`s confidential information and you agree to keep MStar`s
//    confidential information in strictest confidence and not disclose to any
//    third party.
//
// 4. MStar Software is provided on an "AS IS" basis without warranties of any
//    kind. Any warranties are hereby expressly disclaimed by MStar, including
//    without limitation, any warranties of merchantability, non-infringement of
//    intellectual property rights, fitness for a particular purpose, error free
//    and in conformity with any international standard.  You agree to waive any
//    claim against MStar for any loss, damage, cost or expense that you may
//    incur related to your use of MStar Software.
//    In no event shall MStar be liable for any direct, indirect, incidental or
//    consequential damages, including without limitation, lost of profit or
//    revenues, lost or damage of data, and unauthorized system use.
//    You agree that this Section 4 shall still apply without being affected
//    even if MStar Software has been modified by MStar in accordance with your
//    request or instruction for your use, except otherwise agreed by both
//    parties in writing.
//
// 5. If requested, MStar may from time to time provide technical supports or
//    services in relation with MStar Software to you for your use of
//    MStar Software in conjunction with your or your customer`s product
//    ("Services").
//    You understand and agree that, except otherwise agreed by both parties in
//    writing, Services are provided on an "AS IS" basis and the warranty
//    disclaimer set forth in Section 4 above shall apply.
//
// 6. Nothing contained herein shall be construed as by implication, estoppels
//    or otherwise:
//    (a) conferring any license or right to use MStar name, trademark, service
//        mark, symbol or any other identification;
//    (b) obligating MStar or any of its affiliates to furnish any person,
//        including without limitation, you and your customers, any assistance
//        of any kind whatsoever, or any information; or
//    (c) conferring any license or right under any intellectual property right.
//
// 7. These terms shall be governed by and construed in accordance with the laws
//    of Taiwan, R.O.C., excluding its conflict of law rules.
//    Any and all dispute arising out hereof or related hereto shall be finally
//    settled by arbitration referred to the Chinese Arbitration Association,
//    Taipei in accordance with the ROC Arbitration Law and the Arbitration
//    Rules of the Association by three (3) arbitrators appointed in accordance
//    with the said Rules.
//    The place of arbitration shall be in Taipei, Taiwan and the language shall
//    be English.
//    The arbitration award shall be final and binding to both parties.
//
//******************************************************************************
//<MStar Software>
////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2007 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (!§MStar Confidential Information!?? by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    MApp_ZUI_ACCCAUICmd.c
/// @brief  functions to handle CCA IRD commands for UI control
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#define _MApp_ZUI_ACTCCAUICMD_C

/********************************************************************************/
/*                            Header Files                                      */
/********************************************************************************/
// common
#include "MsCommon.h"
#include "HbCommon.h"
#include "DataType.h"
#include "MsOS.h"

#include "MApp_SystemInfo.h"
#include "MApp_ZUI_ACTManualScan.h"
#include "MApp_ZUI_OsdId.h"
#include "MApp_ZUI_ACTtvbanner.h"
#include "MApp_ZUI_Utility.h"
#include "MApp_SaveData.h"

// api
#include "appCCADecoderMsg.h"
//#include "appS3Main.h"
#include "MApp_ZUI_ACTCCAIrd.h"
#include "MApp_ZUI_ACTCCAUIControl.h"
#include "MApp_ZUI_ACTCCAUICmd.h"

#include "appCCADecoderMsgAttribtuedDisplay.h"

// driver
//#include "mdrv_ir.h"

#if defined (MSOS_TYPE_LINUX)
#include <linux/string.h>
//#include "apiFS.h"
#endif

//#define CCA_UI_CMD_DEBUG

/********************************************************************************/
/*                                 Macro                                        */
/********************************************************************************/
#ifdef CCA_UI_CMD_DEBUG
#define CCA_UI_CMD_DBG(fmt, arg...)   {printf("\033[0;35m[IDR UI CMD]%s(%d): ", __FUNCTION__, __LINE__); printf((char *)fmt, ##arg); printf("\033[m");}
#else
#define CCA_UI_CMD_DBG(fmt, arg...)
#endif

/******************************************************************************/
/*                                 External                                   */
/******************************************************************************/

/********************************************************************************/
/*                            Global Variables                                  */
/********************************************************************************/


/********************************************************************************/
/*                            Local Variables                                   */
/********************************************************************************/

UINT8 Tbl_HB_key_conversion[][2] =
{
    {VK_SELECT,    IRD_VK_SELECT    },
    {VK_MENU,      IRD_VK_MENU      },
    {VK_EXIT,      IRD_VK_EXIT      },
    {VK_NUMERIC_0, IRD_VK_NUMERIC_0 },
    {VK_NUMERIC_1, IRD_VK_NUMERIC_1 },
    {VK_NUMERIC_2, IRD_VK_NUMERIC_2 },
    {VK_NUMERIC_3, IRD_VK_NUMERIC_3 },
    {VK_NUMERIC_4, IRD_VK_NUMERIC_4 },
    {VK_NUMERIC_5, IRD_VK_NUMERIC_5 },
    {VK_NUMERIC_6, IRD_VK_NUMERIC_6 },
    {VK_NUMERIC_7, IRD_VK_NUMERIC_7 },
    {VK_NUMERIC_8, IRD_VK_NUMERIC_8 },
    {VK_NUMERIC_9, IRD_VK_NUMERIC_9 },
    {IRD_VK_NUM,   IRD_VK_NUM       }
};

/********************************************************************************/
/*                            extern Function                                */
/********************************************************************************/

/********************************************************************************/
/*                            Local Function                                */
////////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_Cmd_DrawForcedMsg
///
///
///  @param [in]
///
///  @return EN_CCA_UI_CONDITION
///
///  @author MStarSemi @date 2013/5/21
///////////////////////////////////////////////////////////////////////////////

void MApp_CCAUI_Cmd_ClearOsd(void)
{
    CCA_UI_CMD_DBG("\nMApp_CCAUI_Cmd_ClearOsd\n");
    MApp_ZUI_ACT_PageClean_Change();
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_Cmd_DrawErrMsg
///
///
///  @param [in]
///
///  @return EN_CCA_UI_CONDITION
///
///  @author MStarSemi @date 2013/5/21
///////////////////////////////////////////////////////////////////////////////

void MApp_CCAUI_Cmd_DrawErrMsg(UINT8 *pStrBuf)
{
    // FIXME
    //MApp_ZUI_ACT_ShowCCAIrdForcedMSG(pStrBuf, 0, 0, 0, 200, 100);
    MApp_ZUI_ACT_ShowErrorMSG(TRUE, pStrBuf);
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_Cmd_DrawForcedMsg
///
///
///  @param [in]
///
///  @return EN_CCA_UI_CONDITION
///
///  @author MStarSemi @date 2013/5/21
///////////////////////////////////////////////////////////////////////////////

void MApp_CCAUI_Cmd_DrawForcedMsg(stCCAUiDisplayAttribute* pstAttribute)
{
    MApp_ZUI_ACT_ShowCCAIrdForcedMSG(pstAttribute);
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_Cmd_ShowFingerprint
///
///
///  @param [in]
///
///  @return EN_CCA_UI_CONDITION
///
///  @author MStarSemi @date 2013/5/21
///////////////////////////////////////////////////////////////////////////////

void MApp_CCAUI_Cmd_ShowFingerprint(stCCAUiDisplayAttribute* pstAttribute)
{
    MApp_ZUI_ACT_ShowCCAIrdFingerPrint(pstAttribute);
}

#if (CA_CCA_FINGERPRINT_ENHANCED_OVERT == 1)
///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_Cmd_StoreFingerprintingOption
///
///
///  @param [in]
///
///  @return EN_CCA_UI_CONDITION
///
///  @author MStarSemi @date 2013/5/21
///////////////////////////////////////////////////////////////////////////////

void MApp_CCAUI_Cmd_StoreFingerprintingOption(stCCAUiDisplayAttribute* pstAttribute)
{
    MApp_ZUI_ACT_StoreCCAIrdFingerprintingOption(pstAttribute);
}


///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_Cmd_StoreEnhancedOvertFingerprint
///
///
///  @param [in]
///
///  @return EN_CCA_UI_CONDITION
///
///  @author MStarSemi @date 2013/5/21
///////////////////////////////////////////////////////////////////////////////

void MApp_CCAUI_Cmd_StoreEnhancedOvertFingerprint(stIrdFPOptionsTLV* pstFpoTlv)
{
    MApp_ZUI_ACT_StoreEnhancedOvertFingerprint(pstFpoTlv);
}
#endif

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_Cmd_ShowTmsMsg
///
///
///  @param [in]
///
///  @return EN_CCA_UI_CONDITION
///
///  @author MStarSemi @date 2013/5/21
///////////////////////////////////////////////////////////////////////////////

void MApp_CCAUI_Cmd_ShowTmsMsg(stCCAUiDisplayAttribute* pstAttribute, EN_CCA_ERROR_TYPE eErrType, EN_CCA_POPUP_TYPE popMsgType)
{
    MApp_ZUI_ACT_ShowCCAIrdTMS(pstAttribute, eErrType, popMsgType);
}

#if 0 // not use
///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_Cmd_GetFingerprintCheck
///
///
///  @param [in]
///
///  @return EN_CCA_UI_CONDITION
///
///  @author MStarSemi @date 2013/5/21
///////////////////////////////////////////////////////////////////////////////
BOOL MApp_CCAUI_Cmd_GetFingerprintCheck(void)
{
    return MApp_ZUI_ACT_DetectCCAIrdFingerPrint();
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_Cmd_GetFingerprintCheck
///
///
///  @param [in]
///
///  @return EN_CCA_UI_CONDITION
///
///  @author MStarSemi @date 2013/5/21
///////////////////////////////////////////////////////////////////////////////
BOOL MApp_CCAUI_Cmd_GetForcedMsgCheck(void)
{
    return MApp_ZUI_ACT_DetectCCAIrdForcedMSG();
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_Cmd_GetOsdProgCheck
///
///
///  @param [in]
///
///  @return EN_CCA_UI_CONDITION
///
///  @author MStarSemi @date 2013/5/21
///////////////////////////////////////////////////////////////////////////////
UINT8 MApp_CCAUI_Cmd_GetOsdProgCheck(void)
{
    return MApp_ZUI_ACT_DetectCCAIrdProg();
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_Cmd_GetFingerprintCheck
///
///
///  @param [in]
///
///  @return EN_CCA_UI_CONDITION
///
///  @author MStarSemi @date 2013/5/21
///////////////////////////////////////////////////////////////////////////////
BOOL MApp_CCAUI_Cmd_GetIrdetoCheck(void)
{
    // FIXME
    return MApp_ZUI_ACT_DetectCCAIrd();
}


///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_Cmd_GetFlash
///
///
///  @param [in]
///
///  @return EN_CCA_UI_CONDITION
///
///  @author MStarSemi @date 2013/5/21
///////////////////////////////////////////////////////////////////////////////

BOOL MApp_CCAUI_Cmd_IsPinInputAvailable(void)
{
    return MApp_ZUI_ACT_DetectCCAIrdPIN();
}


#endif
///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_Cmd_GetSystemOsdCheck
///
///
///  @param [in]
///
///  @return EN_CCA_UI_CONDITION
///
///  @author MStarSemi @date 2013/5/21
///////////////////////////////////////////////////////////////////////////////
BOOL MApp_CCAUI_Cmd_GetSystemOsdCheck(void)
{
    BOOL bCheck = FALSE;

    if (MApp_ZUI_ACT_GetSystemOSDCheck())
        bCheck = TRUE;

    return bCheck;
}


///////////////////////////////////////////////////////////////////////////////
///  global  MApp_IrdAttributedDisplay_GetCoveragPercentage
///
///
///  @param [in]
///
///  @return EN_CCA_UI_CONDITION
///
///  @author MStarSemi @date 2013/5/21
///////////////////////////////////////////////////////////////////////////////

UINT8 MApp_CCAUI_Cmd_GetCoveragPercentage(UINT8 u8CoverageCode)
{
    return (MApp_IrdAttributedDisplay_GetCoveragPercentage(u8CoverageCode));
}

#undef _MApp_ZUI_ACTCCAUICMD_C

