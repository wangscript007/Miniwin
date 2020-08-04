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

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    MApp_ZUI_ACTCCACasCmd.h
/// @brief  functions to handle CCA IRD commands for UI control
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MApp_ZUI_ACTCCACASCMD_H
#define _MApp_ZUI_ACTCCACASCMD_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#ifdef _MApp_ZUI_ACTCCACASCMD_C
#define INTERFACE
#else
#define INTERFACE extern
#endif

#include "UniversalClient_API.h" // needed for base API calls"
#include "UniversalClient.h"

namespace cca{
//-------------------------------------------------------------------------------------------------
//  Defines
//-------------------------------------------------------------------------------------------------
#define STATUS_ITEM_MAX_LINES            64  // Lines
#define STATUS_ITEM_MAX_LINE_LENGTH      254  // 80 bytes

#define STATUS_CODE_LENGTH              6  //ex D029-0

#define FlexiFlash_MSG_MAX_LINE_LENGTH            64

//FIXME one service only support one handle now
#define CCA_CAS_SERVICE_MAX_HANDLE_NUM                  1
#define CCA_CAS_TOTAL_HANDLE_NUM                 (SERVICE_MAX_NUM * CCA_CAS_SERVICE_MAX_HANDLE_NUM)

typedef enum
{
    E_APP_CMD_CCA_CA_SERVICE_LIST_STATUS,
    E_APP_CMD_CCA_CA_LOADER_STATUS,
    E_APP_CMD_CCA_CA_PRODUCT_STATUS,
    E_APP_CMD_CCA_CA_CLIENT_STATUS,
    E_APP_CMD_CCA_CA_SERVICE_STATUS,//for each service detail
    E_APP_CMD_CCA_CA_END, ///< End of menu item list.
} __attribute__((packed)) EN_CCA_APPCMD_CA_STATUS;

typedef enum
{
  SERVICE_BROADCAST_EMM,
  SERVICE_PULL_EMM,
  SERVICE_DESCRAMBLE,
  SERVICE_PVR_PLAYBACK,
  SERVICE_SMARTCARD_EMM,
  SERVICE_MAX_NUM,
} SERVICE_TYPE;

typedef struct
{
	UINT16 u16TotalHandleNum;
	UINT16 u16ServiceHandle[CCA_CAS_SERVICE_MAX_HANDLE_NUM];

}stServiceTypeUnit;


// Specific service status - DVB Descramble Service
INTERFACE stServiceTypeUnit* MApp_CCAUI_CasCmd_GetServiceTypeUnit(UINT8 type);
INTERFACE UINT32 MApp_CCAUI_CasCmd_GetServiceIndexByHandle(UINT32 u16Handle);
INTERFACE BOOL MApp_CCAUI_CasCmd_GetServiceItemName(UINT32 u32SrvIndex, INT8* pu8Buf, UINT16 u16BufSize);
INTERFACE UINT8 MApp_CCAUI_CasCmd_GetDvbClientServices(INT8 *pu8Buf, UINT8 u8LineCount, UINT16 u16LineLength);
INTERFACE BOOL MApp_CCAUI_CasCmd_GetServiceItemStatus(UINT32 u32SrvIndex, INT8 *pu8Buf, UINT16 u16LineLength);
INTERFACE UINT8 MApp_CCAUI_CasCmd_UpdateServiceItemStatus(UINT32 u32handle, UINT8 *u8StatusCode);
INTERFACE UINT8 MApp_CCAUI_CasCmd_GetServiceItemCaStatus(UINT32 u32SrvIndex, INT8 *pu8Buf, UINT8 u8LineCount, UINT16 u16LineLength);
INTERFACE BOOL MApp_CCAUI_CasCmd_UpdateServiceItemCaStatus(UINT8 u8SrvType, UINT16 u16CaPid, UINT8 *u8StatusCode);
INTERFACE UINT8 MApp_CCAUI_CasCmd_UpdateServiceMonitorStatus(UINT32 u32Handle, UINT8 *pMonitorMsg);
INTERFACE UINT8 MApp_CCAUI_CasCmd_GetServiceItemMonitorStatus(UINT32 u32SrvIndex, INT8 *pu8Buf, UINT8 u8LineCount, UINT16 u16LineLength);
INTERFACE BOOL MApp_CCAUI_CasCmd_EnableServiceMonitorStatus(UINT32 u32Handle, BOOL enable);
INTERFACE void MApp_CCAUI_CasCmd_GetLoaderItemDeviceValue(INT8* pu8Buf, UINT16 u16BufSize);
INTERFACE void MApp_CCAUI_CasCmd_GetLoaderItemKey(INT8* pu8Buf, UINT16 u16BufSize);
INTERFACE void MApp_CCAUI_CasCmd_GetLoaderItemCssn(INT8* pu8Buf, UINT16 u16BufSize);
INTERFACE UINT32 MApp_CCAUI_CasCmd_GetLoaderItemDoadloadId(void);
INTERFACE void MApp_CCAUI_CasCmd_GetLoaderFirmwareVersion(INT8* pu8Buf, UINT16 u16BufSize);
INTERFACE UINT16 MApp_CCAUI_CasCmd_GetLoaderItemLoaderVersion(void);   
INTERFACE void MApp_CCAUI_CasCmd_GetLoaderItemSerialNo(INT8* pu8Buf, UINT16 u16BufSize);
INTERFACE void MApp_CCAUI_CasCmd_GetLoaderSpecificInfo(INT8 *pu8Buf, UINT16 u16BufSize);
INTERFACE UINT8 MApp_CCAUI_CasCmd_UpdateLoaderStatus(void);
INTERFACE UINT32 MApp_CCAUI_CasCmd_GetProductCount(void);
INTERFACE UINT8 MApp_CCAUI_CasCmd_GetProductStatus(INT8 *pu8Buf, UINT8 u8LineCount, UINT16 u16LineLength, UINT32 u32Index);
INTERFACE UINT8 MApp_CCAUI_CasCmd_UpdateProductStatus(void);
INTERFACE void MApp_CCAUI_CasCmd_GetClientItemCCAVersion(INT8* pu8Buf, UINT16 u16BufSize);
INTERFACE void MApp_CCAUI_CasCmd_GetClientItemBuildInfo(INT8* pu8Buf, UINT16 u16BufSize);
INTERFACE void MApp_CCAUI_CasCmd_GetClientItemCssn(INT8* pu8Buf, UINT16 u16BufSize);
INTERFACE void MApp_CCAUI_CasCmd_GetClientItemLockId(INT8* pu8Buf, UINT16 u16BufSize);
INTERFACE void MApp_CCAUI_CasCmd_GetClientItemSecureType(INT8* pu8Buf, UINT16 u16BufSize);
INTERFACE UINT8 MApp_CCAUI_CasCmd_GetClientItemClientId(INT8 *pu8Buf, UINT8 u8LineCount, UINT16 u16LineLength);
INTERFACE UINT8 MApp_CCAUI_CasCmd_GetClientItemSerialNo(INT8 *pu8Buf, UINT8 u8LineCount, UINT16 u16LineLength);
INTERFACE UINT8 MApp_CCAUI_CasCmd_GetClientItemCASystemId(INT8 *pu8Buf, UINT8 u8LineCount, UINT16 u16LineLength);
INTERFACE UINT8 MApp_CCAUI_CasCmd_GetClientItemNationality(INT8 *pu8Buf, UINT8 u8LineCount, UINT16 u16LineLength);
INTERFACE UINT8 MApp_CCAUI_CasCmd_GetClientItemTMS(INT8 *pu8Buf, UINT8 u8LineCount, UINT16 u16LineLength);
INTERFACE UINT8 MApp_CCAUI_CasCmd_GetClientItemCapabilities(INT8 *pu8Buf, UINT8 u8LineCount, UINT16 u16LineLength);
INTERFACE UINT16 MApp_CCAUI_CasCmd_GetClientStatus(INT8 *pu8Buf, UINT8 u8LineCount, UINT16 u16LineLength);
INTERFACE UINT8 MApp_CCAUI_CasCmd_GetClientItemEcmEmmCount(INT8 *pu8Buf, UINT8 u8LineCount, UINT16 u16LineLength);
INTERFACE UINT16 MApp_CCAUI_CasCmd_GetClientStatus(INT8 *pu8Buf, UINT8 u8LineCount, UINT16 u16LineLength);
INTERFACE UINT8 MApp_CCAUI_CasCmd_UpdateClientStatus(void);
INTERFACE UINT8 MApp_CCAUI_CasCmd_UpdateFlexiFlashMsg(UINT8 *pu8SecureCore, UINT8 *pu8DownloadStatus);
INTERFACE UINT8 MApp_CCAUI_CasCmd_UpdateFlexiFlashIFCPMsg(UINT8 *pu8FlexiCore, UINT8 *pu8DownloadStatus);
INTERFACE UINT16 MApp_CCAUI_CasCmd_GetMailMaxItem(void);
INTERFACE void MApp_CCAUI_CasCmd_GetMailType(UINT8 u8Index, INT8* pu8Buf, UINT16 u16BufSize);
INTERFACE void MApp_CCAUI_CasCmd_GetMailId(UINT8 u8Index, INT8* pu8Buf, UINT16 u16BufSize);
INTERFACE BOOL MApp_CCAUI_CasCmd_CheckNewMailItem(UINT8 u8Index);
INTERFACE void MApp_CCAUI_CasCmd_GetMailCreateTime(UINT8 u8Index, INT8* pu8Buf, UINT16 u16BufSize);
INTERFACE UINT8 MApp_CCAUI_CasCmd_GetComponentsMaxString(void);
INTERFACE void MApp_CCAUI_CasCmd_GetComponentsIndexString(UINT8 u8Index, INT8* pu8Buf, UINT16 u16BufSize);
INTERFACE void MApp_CCAUI_CasCmd_SetMailData(void);
INTERFACE void MApp_CCAUI_CasCmd_SaveMailData(void);
INTERFACE void MApp_CCAUI_CasCmd_GetMailContent(UINT8 u8Index, INT8* pu8Buf, UINT16 u16BufSize);
INTERFACE void MApp_CCAUI_CasCmd_DelMailData(UINT8 u8Index);
INTERFACE void MApp_CCAUI_CasCmd_DelAllMailData(void);
INTERFACE UINT8 MApp_CCAUI_CasCmd_GetNewMailItemNumber(void);
INTERFACE BOOL MApp_CCAUI_CasCmd_IfNewMailOrAnncRead(UINT8 u8Type);


#undef INTERFACE
}
#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif  // _MApp_ZUI_ACTCCACASCMD_H

