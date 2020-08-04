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
/// file    appCCADecoderMsgCIPlusCam.h
/// @brief  Parser and handle of CCA Decoder Destination "CI+ CAM"
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef _APPCCADECODERMSGCIPLUSCAM_H
#define _APPCCADECODERMSGCIPLUSCAM_H

//-------------------------------------------------------------------------------------------------
// Standard include files:
//-------------------------------------------------------------------------------------------------

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

#define RSM_REGISTRATION_RESPONSE_LENGTH        21
#define RSM_REGISTRATION_RESPONSE_CSSN_LENGTH   25

// Define CI+ CAM Message types
typedef enum
{
    E_CCA_CIPLUSCAM_MESSAGE_CCK_REFRESH_REQUEST = 0,
    E_CCA_CIPLUSCAM_MESSAGE_MAXIMUM_CCK_LIFETIME,
    E_CCA_CIPLUSCAM_MESSAGE_SWITCH_RSD_DETECTION,
    E_CCA_CIPLUSCAM_MESSAGE_RSD_FILE,
    E_CCA_CIPLUSCAM_MESSAGE_RSM_REGISTRATION_RESPONSE,
    E_CCA_CIPLUSCAM_MESSAGE_UNKNOWN,
} EN_CCA_CIPLUSCAM_MESSAGE_TYPE;


// Define encryption method identity
typedef enum
{
    E_CCA_CIPLUS_ENCRYPTION_NONE= 0,
    E_CCA_CIPLUS_ENCRYPTION_RESERVED,
} EN_CCA_CIPLUS_ENCRYPTION_METHOD_IDENTITY;

// Define signature method identity

typedef enum
{
    E_CCA_CIPLUS_SIGNATURE_RSA_SSA_PSS_2048 = 0,
    E_CCA_CIPLUS_SIGNATURE_RESERVED,
} EN_CCA_CIPLUS_SIGNATURE_METHOD_IDENTITY;

/////////////////////////////////////////
// Structures of all messages for Destination IDs...

/// Type 0
typedef struct
{
    BOOL bTriggerCCKRefresh;
    UINT8   u8Spare;
} __attribute__((packed)) stIrdMsgCiPlusCckRefresh;

/// Type 1
typedef struct
{
    UINT16 u16MaxCckLifetime;
} __attribute__((packed)) stIrdMsgCiPlusMaxCckLifetime;

/// Type 2
typedef struct
{
    BOOL bRsdDetection;                                          ///< 1, on; 0, off.
    UINT8   u8Spare;
} __attribute__((packed)) stIrdMsgCiPlusRsdDetection;

/// RSD file entries
typedef struct
{
    UINT8  u8ModuleId;
    UINT16 u16ModuleVersion;
    UINT32 u32CrlTransmissionTimeout;                              // 24 bits
    UINT8  u8Reserved;
} __attribute__((packed)) stIrdMsgCiPlusRsdFileEntries;

/// RSD file
typedef struct
{
    UINT8        u8FileTag;
    UINT32       u32FileLen;                      // 24 bits
    UINT16       u16VersionNumber;
    UINT32       u32ValidUntilTimestamp;
    ULONGLONG       u64ServiceOperatorIdentity;
    UINT8        u8EncryptionMethodIdentity;
    UINT32       u32TransactionId;
    UINT8        u8Reserved;
    UINT8        u8NumberFileEntries;
    const UINT8* pstRsdFileEntries;
    UINT16       u16NumberServiceEntries;
    const UINT8* pu16ServiceId;
    const UINT8* pu8RsdFileSignature;
    UINT8        u8SignatureMethodIdentity;
} __attribute__((packed)) stIrdMsgCiPlusRsdFile;

///RSM Registration Response

typedef struct
{
    UINT32 u32RegistrationNumber;
    ULONGLONG u64CICAM_ID;
    ULONGLONG u64HOSTID;
    UINT32 u32CSSN;           // Optional
    UINT8  u8ActionCodel;
} __attribute__((packed)) stIrdMsgCiPlusRsmRegistrationResponse;

typedef struct
{
    UINT8   u8TextMsgType;    // 4 bits
    UINT8   u8Spare;

    union {
        stIrdMsgCiPlusCckRefresh               stCckRefresh;
        stIrdMsgCiPlusMaxCckLifetime           stMaxCckLifetime;
        stIrdMsgCiPlusRsdDetection             stRsdDetection;
        const UINT8*                           pu8RsdFile;                   ///< Type 3
        stIrdMsgCiPlusRsmRegistrationResponse  stRsmRegistrationResponse;    ///< Type 4
    };
} __attribute__((packed)) stIrdMsgCiPlusPayload;

// APIs
INTERFACE BOOL MApp_DecoderMsg_CIPlusCam_Parser(const UINT8 *pu8Data, UINT16 u16Length);


#undef INTERFACE

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  // _APPCCADECODERMSGCIPLUSCAM_H

