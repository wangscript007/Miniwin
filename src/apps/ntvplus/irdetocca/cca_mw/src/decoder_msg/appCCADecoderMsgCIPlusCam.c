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
/// file    appCCADecoderMsgCIPlusCam.c
/// @brief  Parser and handle of CCA Decoder Destination "CI+ CAM"
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////


#define _APP_CCA_DECODERMSG_C

/********************************************************************************/
/*                            Header Files                                      */
/********************************************************************************/
// common
#include <ngl_types.h>

// api

#include "appCCADecoderMsg.h"
#include "appCCADecoderMsgCIPlusCam.h"

// driver

#if defined (MSOS_TYPE_LINUX)
#include <linux/string.h>
#endif

//#define CCA_DECMSG_CIPLUSCAM_DEBUG

/********************************************************************************/
/*                                 Macro                                        */
/********************************************************************************/
#ifdef CCA_DECMSG_CIPLUSCAM_DEBUG
#define CCA_DECMSG_CIPLUSCAM_DBG(fmt, arg...)   {printf("\033[0;35m[CI+]%s(%d): ", __FUNCTION__, __LINE__); printf((char *)fmt, ##arg); printf("\033[m");}
#else
#define CCA_DECMSG_CIPLUSCAM_DBG(fmt, arg...)

#endif

/******************************************************************************/
/*                                 External                                   */
/******************************************************************************/


/********************************************************************************/
/*                            Global Variables                                  */
/********************************************************************************/
static stIrdMsgCiPlusPayload gstCiPlus;

/********************************************************************************/
/*                            Local Variables                                   */
/********************************************************************************/


/********************************************************************************/
/*                            extern Function                                */
/********************************************************************************/


/********************************************************************************/
/*                            Local Function                                */
/********************************************************************************/

///////////////////////////////////////////////////////////////////////////////
///  _MApp_DecoderMsg_CIPlusCam_ParseRsdFile
///  Parser of RSD file in CI+ CAM message
///
///  @param [in]      U16Length
///  @param [in]      pu8Data
///
///  @return BOOLEAN true if success, false if fail.
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////

static BOOL _MApp_DecoderMsg_CIPlusCam_ParseRsdFile(const UINT8* pu8Data, UINT16 u16Length)
{
    UINT16 u16Index = 0;
    stIrdMsgCiPlusRsdFile stRsdFile = {0};

    // FIXME: how to employ this RSD file in the system?
    stRsdFile.u8FileTag   = (UINT8)pu8Data[u16Index];
    u16Index++;

    // 24 bits
    stRsdFile.u32FileLen  = ((UINT32)pu8Data[0] << 16) |
                            ((UINT32)pu8Data[1] << 8)  |
                            ((UINT32)pu8Data[2]);

    stRsdFile.u16VersionNumber        = ((UINT32)pu8Data[3] << 8)  |
                                        ((UINT32)pu8Data[4]);

    stRsdFile.u32ValidUntilTimestamp  = ((UINT32)pu8Data[5] << 24) |
                                        ((UINT32)pu8Data[6] << 16) |
                                        ((UINT32)pu8Data[7] << 8)  |
                                        ((UINT32)pu8Data[8]);

    stRsdFile.u64ServiceOperatorIdentity  = ((ULONGLONG)pu8Data[9] << 56)  |
                                            ((ULONGLONG)pu8Data[10] << 48) |
                                            ((ULONGLONG)pu8Data[11] << 40) |
                                            ((ULONGLONG)pu8Data[12] << 32) |
                                            ((ULONGLONG)pu8Data[13] << 24) |
                                            ((ULONGLONG)pu8Data[14] << 16) |
                                            ((ULONGLONG)pu8Data[15] << 8)  |
                                            ((ULONGLONG)pu8Data[16]);
    stRsdFile.u8EncryptionMethodIdentity  = (UINT8)pu8Data[17];
    stRsdFile.u32TransactionId            = ((UINT32)pu8Data[18] << 24) |
                                            ((UINT32)pu8Data[19] << 16) |
                                            ((UINT32)pu8Data[20] << 8)  |
                                            ((UINT32)pu8Data[21]);

    stRsdFile.u8NumberFileEntries        = (UINT8)pu8Data[22];
    u16Index = 23;
    stRsdFile.pstRsdFileEntries          = &pu8Data[23];
    u16Index = u16Index + (stRsdFile.u8NumberFileEntries * 2);        //Each entry has two bytes
    stRsdFile.u16NumberServiceEntries    = (UINT8)pu8Data[u16Index];
    stRsdFile.pu16ServiceId              = &pu8Data[u16Index];
    u16Index = u16Index + (stRsdFile.u16NumberServiceEntries * 2);    //Each ID has two bytes
    stRsdFile.u8SignatureMethodIdentity  = (UINT8)pu8Data[u16Index];
    stRsdFile.pu8RsdFileSignature        = &pu8Data[u16Index];
    u16Index = u16Index + 2048;                                       //RSD_file_signature has 2048 bytes

    if (u16Index != u16Length)
    {
        CCA_DECMSG_CIPLUSCAM_DBG("[CI+ CAM] Wrong length in RSD File message type\n");
        return FALSE;
    }

    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_DecoderMsg_CIPlusCam_Parser
///  Parser of Destination CI+ CAM Message in IRD message
///
///  @param [in]      U16Length
///  @param [in]      pu8Data
///
///  @return BOOLEAN true if success, false if fail.
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////

BOOL MApp_DecoderMsg_CIPlusCam_Parser(const UINT8* pu8Data, UINT16 u16Length)
{
    BOOL bHandled = FALSE;

    // Zero out the structure
    memset(&gstCiPlus, 0x0, sizeof(stIrdMsgCiPlusPayload));

    gstCiPlus.u8TextMsgType = (pu8Data[0] >> 4) & 0x0f;    // 4 bits

    CCA_DECMSG_CIPLUSCAM_DBG("[CI+ CAM] message type = %d\n", gstCiPlus.u8TextMsgType);

    if (gstCiPlus.u8TextMsgType >= E_CCA_CIPLUSCAM_MESSAGE_UNKNOWN)
    {
        CCA_DECMSG_CIPLUSCAM_DBG("[CI+ CAM] Unknown message types\n");
        return FALSE;
    }

    if (gstCiPlus.u8TextMsgType == E_CCA_CIPLUSCAM_MESSAGE_CCK_REFRESH_REQUEST)
    {
        if (u16Length != 2)
        {
            CCA_DECMSG_CIPLUSCAM_DBG("[CI+ CAM] Wrong Payload length in CCK refresh request\n");
            return FALSE;
        }

        //CCK Refresh
        gstCiPlus.stCckRefresh.bTriggerCCKRefresh = (BOOL)((pu8Data[1] >> 7) & 0x01);

        //FIXME: Which APIs to do CCK Refresh
        bHandled = TRUE;
    }
    else if (gstCiPlus.u8TextMsgType == E_CCA_CIPLUSCAM_MESSAGE_MAXIMUM_CCK_LIFETIME)
    {
        if (u16Length != 3)
        {
            CCA_DECMSG_CIPLUSCAM_DBG("[CI+ CAM] Wrong Payload length in Max CCK lifetime\n");
            return FALSE;
        }

        // Set max CCK lifetime
        gstCiPlus.stMaxCckLifetime.u16MaxCckLifetime = ((UINT16)pu8Data[1] << 8) |
                                                       ((UINT16)pu8Data[2]);

        //FIXME: Which APIs to set
        bHandled = TRUE;
    }
    else if (gstCiPlus.u8TextMsgType == E_CCA_CIPLUSCAM_MESSAGE_SWITCH_RSD_DETECTION)
    {
        if (u16Length != 2)
        {
            CCA_DECMSG_CIPLUSCAM_DBG("[CI+ CAM] Wrong Payload length in Switch RSD detection\n");
            return FALSE;
        }

        // Turn on/off RSD detection
         gstCiPlus.stRsdDetection.bRsdDetection = (BOOL)((pu8Data[1] >> 7) & 0x01);

        //FIXME: Which APIs to set
        bHandled = TRUE;
    }
    else if (gstCiPlus.u8TextMsgType == E_CCA_CIPLUSCAM_MESSAGE_RSD_FILE)
    {
        // RSD file
        gstCiPlus.pu8RsdFile = &pu8Data[1];

        //FIXME: where to store the file and how to use it
        bHandled = _MApp_DecoderMsg_CIPlusCam_ParseRsdFile(gstCiPlus.pu8RsdFile, u16Length - 1);
    }
    else if (gstCiPlus.u8TextMsgType == E_CCA_CIPLUSCAM_MESSAGE_RSM_REGISTRATION_RESPONSE)
    {

        if ((u16Length - 1) != RSM_REGISTRATION_RESPONSE_LENGTH)
        {
            if ((u16Length - 1) != RSM_REGISTRATION_RESPONSE_CSSN_LENGTH)
            {
                CCA_DECMSG_CIPLUSCAM_DBG("[CI+ CAM] Wrong Payload length in RSM register response\n");
                return FALSE;
            }
        }

        // RSD file
        gstCiPlus.stRsmRegistrationResponse.u32RegistrationNumber = ((UINT32)pu8Data[1] << 24)  |
                                                                    ((UINT32)pu8Data[2] << 16)  |
                                                                    ((UINT32)pu8Data[3] << 8)   |
                                                                    ((UINT32)pu8Data[4]);
        gstCiPlus.stRsmRegistrationResponse.u8ActionCodel         = ((UINT8)pu8Data[5]);
        gstCiPlus.stRsmRegistrationResponse.u64CICAM_ID           = ((ULONGLONG)pu8Data[6] << 56)  |
                                                                    ((ULONGLONG)pu8Data[7] << 48)  |
                                                                    ((ULONGLONG)pu8Data[8] << 40)  |
                                                                    ((ULONGLONG)pu8Data[9] << 32)  |
                                                                    ((ULONGLONG)pu8Data[10] << 24) |
                                                                    ((ULONGLONG)pu8Data[11] << 16) |
                                                                    ((ULONGLONG)pu8Data[12] << 8)  |
                                                                    ((ULONGLONG)pu8Data[13]);
        gstCiPlus.stRsmRegistrationResponse.u64HOSTID             = ((ULONGLONG)pu8Data[14] << 56) |
                                                                    ((ULONGLONG)pu8Data[15] << 48) |
                                                                    ((ULONGLONG)pu8Data[16] << 40) |
                                                                    ((ULONGLONG)pu8Data[17] << 32) |
                                                                    ((ULONGLONG)pu8Data[18] << 24) |
                                                                    ((ULONGLONG)pu8Data[19] << 16) |
                                                                    ((ULONGLONG)pu8Data[20] << 8)  |
                                                                    ((ULONGLONG)pu8Data[21]);

        if ((u16Length - 1) == RSM_REGISTRATION_RESPONSE_CSSN_LENGTH)
        {
            // Optional
            gstCiPlus.stRsmRegistrationResponse.u32CSSN           = ((UINT32)pu8Data[22] << 24) |
                                                                    ((UINT32)pu8Data[23] << 16) |
                                                                    ((UINT32)pu8Data[24] << 8)  |
                                                                    ((UINT32)pu8Data[25]);
        }

        //FIXME: where to store the file and how to use it

        bHandled = TRUE;
    }

    return bHandled;
}

#undef _APP_CCA_DECODERMSG_C

