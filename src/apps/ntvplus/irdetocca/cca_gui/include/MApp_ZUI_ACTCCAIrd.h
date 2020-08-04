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
// (MStar Confidential Information!¡L) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _MAPP_ZUI_ACTCCAIRD_H
#define _MAPP_ZUI_ACTCCAIRD_H

#include "appCCADecoderMsgAttribtuedDisplay.h"
#include "MApp_ZUI_ACTCCAUIControl.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#ifdef _MAPP_ZUI_ACTCCAIRD_H
#define INTERFACE
#else
#define INTERFACE extern
#endif





/////////////////////////////////////////
// Cutomize Window Procedures...

/////////////////////////////////////////
/// Define structure of IRD message
typedef struct
{
    UINT8   au8StrBuf[MAIL_STRING_MAX];
    UINT32  u32Duration;
    UINT16  u16PosX;
    UINT16  u16PosY;
    UINT16  u16Width;
    UINT16  u16Height;
    UINT8   u8Percent;
    BOOL bFlashing;
    BOOL bBanner;
} __attribute__((packed)) stCCAUiDisplayAttribute;


//INTERFACE void MApp_ZUI_ACT_NotifyCCACaStatusUpdate(UINT8* pu8Data);


INTERFACE BOOL MApp_ZUI_ACT_ShowCCAIrdTMS(stCCAUiDisplayAttribute* pstAttribute, EN_CCA_ERROR_TYPE eErrType, EN_CCA_POPUP_TYPE popMsgType);
INTERFACE BOOL MApp_ZUI_ACT_ShowCCAIrdForcedMSG(stCCAUiDisplayAttribute* pstAttribute);
INTERFACE BOOL MApp_ZUI_ACT_ShowCCAIrdFingerPrint(stCCAUiDisplayAttribute* pstAttribute);
INTERFACE UINT16 MApp_ZUI_ACT_GetRandomNum(void);
INTERFACE void MApp_ZUI_ACT_PageClean_Change(void);
INTERFACE void MApp_ZUI_ACT_ShowErrorMSG(BOOL bShow, UINT8* pu8Buff);
INTERFACE BOOL MApp_ZUI_ACT_GetSystemOSDCheck(void);
INTERFACE void MApp_ZUI_ACT_ShowCCAIrdMailIcon(BOOL bShow);
INTERFACE void MApp_ZUI_ACT_ShowCCAIrdAnnouncementIcon(BOOL bShow);
INTERFACE BOOL MApp_ZUI_ACT_IfCCAPopMsgExist(void);
INTERFACE void MApp_ZUI_ACT_ResetPopMsgStatus(void);
INTERFACE UINT32 MApp_ZUI_ACT_GetCCAPopMsgDuration(void);
INTERFACE EN_CCA_POPUP_TYPE MApp_ZUI_ACT_GetPopMsgType(void);



#if (CA_CCA_FINGERPRINT_ENHANCED_OVERT == 1)
BOOL MApp_ZUI_ACT_StoreEnhancedOvertFingerprint(stIrdFPOptionsTLV* pstFpoTlv);
BOOL MApp_ZUI_ACT_StoreCCAIrdFingerprintingOption(stCCAUiDisplayAttribute* pstAttribute);
#endif


INTERFACE BOOL MApp_ZUI_ACT_DetectCCAIrdMailIcon(void);
INTERFACE BOOL MApp_ZUI_ACT_DetectCCAIrdAnncmntIcon(void);
INTERFACE BOOL MApp_CCAUI_GetSystemOsdCheck(void);
INTERFACE BOOL MApp_ZUI_ACT_MonitorCCAIrdScreenTextDisplay(void);




#undef INTERFACE

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  // _MAPP_ZUI_ACTCCACASTATUS_H

