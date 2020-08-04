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
/// file    appCCADecoderMsgProfDec.c
/// @brief  Parser and handle of CCA Decoder Destination "Prof-Dec"
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////


#define _APP_CCA_DECODERMSGPROFDEC_C

/********************************************************************************/
/*                            Header Files                                      */
/********************************************************************************/
// common
#include <ngl_types.h>
#include <ngl_log.h>
#include <string.h>
// api
#include "appCCADecoderMsg.h"
#include "appCCADecoderMsgProfDec.h"
#include "appCCADecoderMsgTextMsg.h"
// driver
NGL_MODULE(PROFDEC);

namespace cca{
	
#if defined (MSOS_TYPE_LINUX)
#include <linux/string.h>
#endif

//#define CCA_DECMSG_PROFDEC_DEBUG

/********************************************************************************/
/*                                 Macro                                        */
/********************************************************************************/


/******************************************************************************/
/*                                 External                                   */
/******************************************************************************/


/********************************************************************************/
/*                            Global Variables                                  */
/********************************************************************************/
static stIrdMsgProfDecPayload gstProfDec;
static stIrdMsgProfDecInfoData* gpstInfoData = nullptr;

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
///  global  MApp_DecoderMsg_ProfDec_Parser
///  Parser of Destination ProfDec Message in IRD message
///
///  @param [in]    pu8Data     Pointer to the Message data
///  @param [in]    u16Length  Message length
///
///  @return BOOLEAN true if success, false if failed.
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////

BOOL MApp_DecoderMsg_ProfDecMsg_Parser(const UINT8* pu8Data, UINT16 u16Length)
{
    UINT32 u32LoopN = 0;
    UINT32 i, j;
    BOOL bError = TRUE;

    // Check if the payload consists of 6-byte header
    if (u16Length < IRDMSG_PROFDEC_HEADER_LENGTH)
    {
        NGLOG_DEBUG("[ProfDec] Something wrong on Payload length = %d\n", u16Length);
        return FALSE;
    }
    else
    {
        // Zero out the structure
        memset(&gstProfDec, 0x0, sizeof(stIrdMsgProfDecPayload));

        gstProfDec.u16OriginalNetworkID  = ((UINT16)pu8Data[0] << 8) | ((UINT16)pu8Data[1]);
        gstProfDec.u16ClubNumber         = ((UINT16)pu8Data[2] << 8) | ((UINT16)pu8Data[3]);
        gstProfDec.u16MiniConDataService = ((UINT16)pu8Data[4] << 8) | ((UINT16)pu8Data[5]);

        if (gstProfDec.u16ClubNumber != IRD_NO_CLUB_NUM_ASSIGNED)
        {
            // These club numbers are the same ones defined and downloaded in the Text Message structure
            bError = MApp_DecoderMsg_TextMsg_CheckClubNum(gstProfDec.u16ClubNumber);
            if (bError == FALSE)
            {
                NGLOG_DEBUG("[ProfDec] Subscriber 0x%x is NOT one of club members\n", gstProfDec.u16ClubNumber);
                return bError;
            }
            else
            {
                NGLOG_DEBUG("[ProfDec] Subscriber 0x%x is one of club members\n", gstProfDec.u16ClubNumber);
            }
        }
        else
        {
            NGLOG_DEBUG("[ProfDec] 0x%x disables the filtering and receives the message always\n", gstProfDec.u16ClubNumber);
        }

        // Allocate memory to store the data
        if (gpstInfoData != NULL)
        {
            IRD_FREE(gpstInfoData);
            gpstInfoData = NULL;
        }

        // Calculate total number of Info data payload
        u32LoopN = (u16Length - IRDMSG_PROFDEC_HEADER_LENGTH) / IRDMSG_PROFDEC_INFODATA_LENGTH;
        NGLOG_DEBUG("[ProfDec] LoopN = %d\n", u32LoopN);

        if (gpstInfoData == NULL)
        {
            gpstInfoData = (stIrdMsgProfDecInfoData*)IRD_MALLOC((sizeof(stIrdMsgProfDecInfoData) * u32LoopN));
            if (gpstInfoData == NULL)
            {
                NGLOG_DEBUG("[ProfDec] Failed to allocate memory\n");
                return FALSE;
            }
        }

        // Process Info data
        gstProfDec.pstInfoData = &pu8Data[6];
        for (i = 0; i < u32LoopN; i++)
        {
              j = i * IRDMSG_PROFDEC_INFODATA_LENGTH;
              gpstInfoData[i].u8InformationType = gstProfDec.pstInfoData[j];

              if (gpstInfoData[i].u8InformationType == IRDMSG_PROFDEC_INFO_TYPE_S)
              {
                   gpstInfoData[i].stInfoS.u16ServiceID = ((UINT16)gstProfDec.pstInfoData[j + 1] << 8) |
                                                           ((UINT16)gstProfDec.pstInfoData[j + 2]);
              }
              else if (gpstInfoData[i].u8InformationType == IRDMSG_PROFDEC_INFO_TYPE_A)
              {
                   gpstInfoData[i].stInfoA.u32ISOLanguageCode = ((UINT32)gstProfDec.pstInfoData[j + 1] << 16) |
                                                                 ((UINT32)gstProfDec.pstInfoData[j + 2] << 8) |
                                                                 ((UINT32)gstProfDec.pstInfoData[j + 3]);
                   gpstInfoData[i].stInfoA.u8AudioType        = gstProfDec.pstInfoData[j + 4];
              }
              else
              {
                  NGLOG_DEBUG("[ProfDec] Unknown Information Type %c\n", gpstInfoData[i].u8InformationType);
                  return FALSE;
              }
        }
    }

    return TRUE;
}
}
#undef _APP_CCA_DECODERMSGPROFDEC_C

