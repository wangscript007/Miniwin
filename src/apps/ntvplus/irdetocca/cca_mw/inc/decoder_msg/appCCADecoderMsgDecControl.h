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
/// file    appCCADecoderMsgDecControl.h
/// @brief  Parser and handle of CCA Decoder Destination "Decoder Control"
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef _APPCCADECODERMSGDECCONTROL_H
#define _APPCCADECODERMSGDECCONTROL_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#ifdef _APP_CCA_DECODERMSG_C
#define INTERFACE
#else
#define INTERFACE extern
#endif


//-------------------------------------------------------------------------------------------------
//  Defines
//-------------------------------------------------------------------------------------------------

/////////////////////////////////////////
// Structures of all messages for Destination IDs...

typedef enum
{
    E_LINKAGE_NO_MATCH,
    E_LINKAGE_MATCH_MAN_ID,
    E_LINKAGE_MATCH,
} EN_LINKAGE;

/// Define control message type
typedef enum
{
    E_CCA_CONTROL_MESSAGE_TYPE_DOWNLOAD = 0,
    E_CCA_CONTROL_MESSAGE_TYPE_CALLBACK,
    E_CCA_CONTROL_MESSAGE_TYPE_MONITOR,
    E_CCA_CONTROL_MESSAGE_TYPE_READ_SMARTCARD,
    E_CCA_CONTROL_MESSAGE_TYPE_CHANGE_PINCODE,
    E_CCA_CONTROL_MESSAGE_TYPE_RECOVERY,
    E_CCA_CONTROL_MESSAGE_TYPE_USER,
    E_CCA_CONTROL_MESSAGE_TYPE_SPARE,
} EN_CCA_CONTROL_MESSAGE_TYPE;

/// Define recovery type of IRD command
typedef enum
{
    E_CCA_RECOVERY_TYPE_RESET_IRD = 0,
    E_CCA_RECOVERY_TYPE_RESET_CHANNEL_ORDER,
    E_CCA_RECOVERY_TYPE_RESCAN_RESET,                    ///< Re-Scan Service Information and Reset Channel Order
    E_CCA_RECOVERY_TYPE_ERASE_SERVICE_LISTS,             ///< Erase service lists RESET Re-Tune from Home Tune Partition Data, Re-Scan Service Information and Reset Channel Order
    E_CCA_RECOVERY_TYPE_RECONNECT,                       ///< Reconnect to current service
    E_CCA_RECOVERY_TYPE_FORCE_TUNE,                      ///< Force tune to specific service
    E_CCA_RECOVERY_TYPE_FORCE_SET_INSTALLER_PIN_CODE,    ///< Force set installer PIN code
    E_CCA_RECOVERY_TYPE_SPARE,
} EN_CCA_RECOVERY_TYPE;

/// Define control message type
typedef enum
{
    E_CCA_CONTROL_MESSAGE_PIN_CODE_PARENTAL = 0,
    E_CCA_CONTROL_MESSAGE_PIN_CODE_INSTALLER,
    E_CCA_CONTROL_MESSAGE_PIN_CODE_SPARE,
} EN_CCA_CONTROL_MESSAGE_PIN_CODE_TYPE;

#if (CA_IRD_CCA == 1)
typedef enum
{
    EN_OTA_MENU_TRIGGER_STATE_NONE = 0,
    EN_OTA_MENU_TRIGGER_STATE_START,
    EN_OTA_MENU_TRIGGER_STATE_READY,
    EN_OTA_MENU_TRIGGER_STATE_DONE,
    EN_OTA_MENU_TRIGGER_STATE_ERROR,
}EN_OTA_MENU_TRIGGER_STATE_TYPE;
#endif

typedef struct
{
    BOOL bDownloadAllowed;                           // 1 bit
    BOOL bForcedDownload;                            // 1 bit
    BOOL bProfdecForcedDownload;                     // 1 bit
    UINT8   u8Spare;
} __attribute__((packed)) stIrdDecControlDownload;

typedef struct
{
    UINT16  u16NVRAMParentalPINCode;
    UINT8   u8NVRAMParentalPINIndex;
} __attribute__((packed)) stIrdDecControlPinCode;

typedef struct
{
    UINT16 u16OriginalNetworkId;
    UINT16 u16TransportStreamId;
    UINT16 u16ServiceId;
} __attribute__((packed)) stIrdRecoverTypeForceTune;

typedef struct
{
    UINT8   u8Recovery_Type;                            // 4 bits
    UINT8   u8Spare;

    union
    {
        UINT16                     u16BouquetID;            // Recovery Type 1
        stIrdRecoverTypeForceTune  stForceTune;             // Recovery Type 5
        UINT16                     u16InstallerPINCode;     // Recovery Type 6
    };

} __attribute__((packed)) stIrdDecControlRecovery;

typedef struct
{
    UINT8   u8DecControl;                               // 4 bits
    UINT8   u8Spare;

    union
    {
        stIrdDecControlDownload      stDownload;        ///< Download control
        const UINT8*                 pu8Callback;       ///< Call back data
        const UINT8*                 pu8Monitor;        ///< Special monitor mode data
        const UINT8*                 pu8SCUser;         ///< Read Smartcard User Data
        stIrdDecControlPinCode       stPinCode;         ///< Change NVRAM parental PIN code
        stIrdDecControlRecovery      stRecoverData;     ///< Decoder recovery data
        const UINT8*                 pu8User;           ///< User Payload
    };

} __attribute__((packed)) stIrdMsgDecControlPayload;

// APIs
INTERFACE BOOL MApp_DecoderMsg_DecControl_InitParser(void);
INTERFACE BOOL MApp_DecoderMsg_DecControl_Parser(const UINT8 *pu8Data, UINT16 u16Length);
INTERFACE BOOL MApp_DecoderMsg_DecControl_TerminateParser(void);

#if (CA_IRD_CCA == 1)
INTERFACE BOOL MApp_MenuDownloadControl(void);
INTERFACE BOOL MApp_MenuPreDownloadControl(void);
INTERFACE BOOL MApp_MenuDownloadReady(void);
INTERFACE BOOL MApp_MenuDownloadStart(void);
INTERFACE BOOL MApp_DecoderMsg_MenuProcessOTAFlag(BOOL);
INTERFACE EN_OTA_MENU_TRIGGER_STATE_TYPE MApp_DecoderMsg_MenuStateGet(void);
INTERFACE void MApp_DecoderMsg_MenuStateSet(EN_OTA_MENU_TRIGGER_STATE_TYPE);
#endif

#undef INTERFACE

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  // _APPCCADECODERMSGDECCONTROL_H

