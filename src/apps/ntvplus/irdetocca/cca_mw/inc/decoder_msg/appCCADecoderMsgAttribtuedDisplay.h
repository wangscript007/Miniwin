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
/// file    appCCADecoderMsgAttribtuedDisplay.h
/// @brief  Parser and handle of CCA Decoder Destination "Attributed Display"
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef _APPCCADECODERMSGATTRIBUTEDDISPLAY_H
#define _APPCCADECODERMSGATTRIBUTEDDISPLAY_H

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
#if (CA_CCA_FINGERPRINT_ENHANCED_OVERT == 1)
#define TAG_ENHANCED_OVERT_FINGERPRINTING_OPTIONS 0x00
#define FINGERPRINT_OPTION_TLV_MAX_LENGTH  255 //  8 bits to represetnt the length
#endif
#define IRDMSG_ATTR_HEADER_LENGTH        6     // 6 bytes

#define IRDMSG_ATTR_MAX_TEXT_LENGTH      511

#define FINGERPRINT_TYPE_OVERT           0
#define FINGERPRINT_TYPE_COVERT          1

#define MAIL_STRING_MAX          512

/////////////////////////////////////////
// Structures of all messages for Destination IDs...

/// Define attributed text message class
typedef enum
{
    E_CCA_ATTR_MESSAGE_TYPE_NORMAL = 0,
    E_CCA_ATTR_MESSAGE_TYPE_FORCED_TEXT,
    E_CCA_ATTR_MESSAGE_TYPE_FRIGERPRINT,
    E_CCA_ATTR_MESSAGE_TYPE_FRIGERPRINTING_OPTIONS,
} EN_CCA_ATTR_MESSAGE_TYPE;



typedef enum
{
    E_CCA_DISPLAY_PRIORITY_FINGERPRINT        = 0,     // Highest
    E_CCA_DISPLAY_PRIORITY_FINGERPRINT_OPTION = 0,
    E_CCA_DISPLAY_PRIORITY_MENU,
    E_CCA_DISPLAY_PRIORITY_TEXT_MSG,
    E_CCA_DISPLAY_PRIORITY_GENERAL_ERROR_BANNERS,
    E_CCA_DISPLAY_PRIORITY_CA_ERROR_BANNERS,
    E_CCA_DISPLAY_PRIORITY_LEGACY,
    E_CCA_DISPLAY_PRIORITY_SPARE,
} EN_CCA_DISPLAY_PRIORITY;

typedef enum
{
    E_CCA_ERROR_TYPE_GENERAL_ERROR = 0,
    E_CCA_ERROR_TYPE_CA_ERROR,
    E_CCA_ERROR_TYPE_LEGACY,
    E_CCA_ERROR_TYPE_SPARE,
} EN_CCA_ERROR_TYPE;


#if (CA_CCA_FINGERPRINT_ENHANCED_OVERT == 1)
/// Enhanced Overt Fingerprinting Options (Tag=0x00) variable
typedef struct
{
    UINT8   u8LocationXFactor;
    UINT8   u8LocationYFactor;
    UINT8   u8BgTransparencyAlphaFactor;
    UINT32  u32BgColourRGB;                  // 24 bits
    UINT8   u8FontTransparencyAlphaFactor;
    UINT32  u32FontColourRGB;                // 24 bits
    UINT8   u8FontTypeIndex;
    UINT8   u8Reserved;
} __attribute__((packed)) stIrdEofOptionsVariable;

typedef struct
{
    UINT8   u8Tag;
    UINT8   u8Length;
    union
    {
        UINT8                    au8Variable[FINGERPRINT_OPTION_TLV_MAX_LENGTH];
        stIrdEofOptionsVariable  stEofVar;   ///< For FP option = 0
    };
} __attribute__((packed)) stIrdFPOptionsTLV;
#endif

typedef struct
{
    BOOL bFlashing;                       ///< 1, flashing; 0, not flashing
    BOOL bBanner;                         ///< 1, normal; 0, banner
    UINT8   u8CoverageCode;                  ///< value range [0:63]
} __attribute__((packed)) stIrdAttributedDisplayMethod;

typedef struct
{
    UINT8                         u8MessageType;
    UINT16                        u16Duration;
    stIrdAttributedDisplayMethod  stDisplayMethod;
    BOOL                       bFingerprintType;
    UINT8                         u8Reserved;
    UINT16                        u16TextLength;               // 12 bits

    union
    {
        UINT8              au8CSSNASCII[IRDMSG_ATTR_MAX_TEXT_LENGTH + 1]; // For Fingerprint, an array of bytes in ASCII numerical representation. Last byte for 'n'
                                                                          // string of 10 digits S/N + 1 check digit + '0'
        #if (CA_CCA_FINGERPRINT_ENHANCED_OVERT == 1)
        stIrdFPOptionsTLV  stFPTLV;                                   // For Fingerprinting option
        #endif
        const UINT8*       pu8TextByte;                               // For Normal and Forced Text
    };
} __attribute__((packed)) stIrdMsgAttributedDisplayPayload;


// APIs
INTERFACE BOOL MApp_DecoderMsg_AttributedDisplay_Parser(const UINT8 *pu8Data, UINT16 u16Length, UINT16 u16Crc);

#if (CA_CCA_FINGERPRINT_ENHANCED_OVERT == 1)
INTERFACE stIrdFPOptionsTLV* MApp_DecoderMsg_AttributedDisplay_GetFPTLV(UINT8 u8FPTag);
#endif

INTERFACE stIrdAttributedDisplayMethod* MApp_DecoderMsg_AttributedDisplay_GetDisplayMethod(void);
INTERFACE UINT8 MApp_IrdAttributedDisplay_GetCoveragPercentage(UINT8 u8CoverageCode);


#undef INTERFACE

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  // _APPCCADECODERMSGATTRIBUTEDDISPLAY_H

