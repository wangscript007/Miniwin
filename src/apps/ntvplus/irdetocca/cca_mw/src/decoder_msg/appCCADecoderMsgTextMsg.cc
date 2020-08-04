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
/// file    appCCADecoderMsgTextMsg.c
/// @brief  Parser and handle of CCA Decoder Destination "Text Message"
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////


#define _APP_CCA_DECODERMSGTEXTMSG_C

/********************************************************************************/
/*                            Header Files                                      */
/********************************************************************************/
// common
#include <ngl_types.h>
#include <ngl_log.h>
#include <string.h>
// api
#include "appCCADecoderMsg.h"
#include "appCCADecoderMsgTextMsg.h"
#include "CCA_UCMsgProc.h"

// driver
NGL_MODULE(IRDCCATXTMSG);
namespace cca{

#if defined (MSOS_TYPE_LINUX)
#include <linux/string.h>
#endif

#define CCA_DECMSG_TXT_DEBUG
#define APP_CMD_DATA_LEN	20

/********************************************************************************/
/*                                 Macro                                        */
/********************************************************************************/

/******************************************************************************/
/*                                 External                                   */
/******************************************************************************/


/********************************************************************************/
/*                            Global Variables                                  */
/********************************************************************************/
static stIrdMsgTextMsgPayload gstTextMsg = {0};

static stIrdTextMsgClubNum* gpstClub     = 0;    ///< Club list
static UINT16 gu16TotalNumGrps           = 0;    ///< Max group number
static UINT16 gu16NumGrps                = 0;    ///< Number of group
static UINT16 gu16CurrentGrp             = 0;    // FIXME
static UINT16 gMailAnncmntIndex          = IRD_INVALID_MAIL_ANNCMNT_INDEX;  // The index of the massage in the Mail/Announcemente buffer


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
///  global MApp_DecoderMsg_TextMsg_CheckClubNum
///  Check if the subscriber is one of club members
///
///  @param [in]    u16ClubNum Club number
///
///  @return BOOLEAN true if success, false if failed.
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////

BOOL MApp_DecoderMsg_TextMsg_CheckClubNum(UINT16 u16ClubNum)
{
    int i;

    // Number in Club List
    for (i = 0; i < IRD_MAX_NUM_CLUBS; i++)
    {
        if (gpstClub[gu16CurrentGrp].au16ClubNum[i] == u16ClubNum)
        {
            NGLOG_DEBUG("[Club %d] Club number 0x%x found = %d\n", gu16CurrentGrp, u16ClubNum, i);
            return TRUE;
        }
    }

    return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
///  global MApp_DecoderMsg_TextMsg_ShowIcon
///
///
///  @param [in]    u16ClubNum Club number
///
///  @return BOOLEAN true if success, false if failed.
///
///  @author MStarSemi @date 2013/7/19
///////////////////////////////////////////////////////////////////////////////

void MApp_DecoderMsg_TextMsg_ShowIcon(UINT8 u8TextMsgType, BOOL bShow)
{
    tCCAIRD_UI_Event e;

    //NGLOG_DEBUG("[Mail/Announcement %d] Normal message class!\n", u8TextMsgType);
    memset(&e.u8CmdData[0], '\0', APP_CMD_DATA_LEN);	
	e.u8CmdData[0]=E_APP_CMD_CCAIRD_ICON_MSG;
	e.u8CmdData[1]=u8TextMsgType;
	e.u8CmdData[2]=bShow;
	CCAIRDEventSource::getInstance()->sendIrdCcaCmd(e);
}

///////////////////////////////////////////////////////////////////////////////
///  _MApp_TextMsg_ParseMailAnncmnt
///  Parser of Mail message or Announcement message
///
///  @param [in]    pu8Data     Pointer to the Message data
///  @param [in]    u16Length  Message length
///  @param [in]    u16Crc       CRC value obtained at  CA IRD message
///
///  @return BOOLEAN true if success, false if failed.
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////

static BOOL _MApp_TextMsg_ParseMailAnncmnt(const UINT8* pu8Data, UINT16 u16Length, UINT16 u16Crc)
{
    EN_CCA_CRC_STATUS eCrcSts = E_CCA_CRC_NEW;
    UINT8 u8MsgLengOffset    = 0;
    UINT16 u16Year           = 0;
    NGL_DATE* pstDate         = NULL;
    BOOL bIsClubMember    = FALSE;

    gstTextMsg.stMailAnncmnt.stMsg.u8Class = (UINT8)((pu8Data[0] >> 5) & 0x7);

    NGLOG_DEBUG("[Mail/Announcement %d] Message class = %d\n", gstTextMsg.u8TextMsgType,
                                                                      gstTextMsg.stMailAnncmnt.stMsg.u8Class);

    if (gstTextMsg.stMailAnncmnt.stMsg.u8Class >= E_CCA_TEXT_MSG_CLASS_SPARE)
    {
         NGLOG_DEBUG("[Mail/Announcement %d] Spare message class\n", gstTextMsg.u8TextMsgType);
         return TRUE;
    }

    // mail CRC check
    if (gstTextMsg.u8TextMsgType == E_CCA_TEXT_MSG_TYPE_MAILBOX)
    {
        if (gstTextMsg.stMailAnncmnt.stMsg.u8Class == E_CCA_TEXT_MSG_CLASS_NORMAL)
        {
            eCrcSts = MApp_DecoderMsg_CheckCrcList(E_CCA_TEXT_MSG_TYPE_MAILBOX, E_CCA_CRC_NORMAL_MAIL, u16Crc);
        }
        else if (gstTextMsg.stMailAnncmnt.stMsg.u8Class == E_CCA_TEXT_MSG_CLASS_FORCE)
        {
            eCrcSts = MApp_DecoderMsg_CheckCrcList(E_CCA_TEXT_MSG_TYPE_MAILBOX, E_CCA_CRC_FORCED_MAIL, u16Crc);
        }

        if (eCrcSts != E_CCA_CRC_NEW)
        {
            NGLOG_DEBUG("[Mail/Announcement] Duplicated message received\n");
            return TRUE; // Terminate the parsing of the current decoder message
        }
    }

    // announcement CRC check
    if (gstTextMsg.u8TextMsgType == E_CCA_TEXT_MSG_TYPE_ANNOUNCEMENT)
    {
        if (gstTextMsg.stMailAnncmnt.stMsg.u8Class == E_CCA_TEXT_MSG_CLASS_NORMAL)
        {
            eCrcSts = MApp_DecoderMsg_CheckCrcList(E_CCA_TEXT_MSG_TYPE_ANNOUNCEMENT, E_CCA_CRC_NORMAL_MAIL, u16Crc);
        }
        else if (gstTextMsg.stMailAnncmnt.stMsg.u8Class == E_CCA_TEXT_MSG_CLASS_FORCE)
        {
            eCrcSts = MApp_DecoderMsg_CheckCrcList(E_CCA_TEXT_MSG_TYPE_ANNOUNCEMENT, E_CCA_CRC_FORCED_MAIL, u16Crc);
        }

        if (eCrcSts != E_CCA_CRC_NEW)
        {
            NGLOG_DEBUG("[Mail/Announcement] Duplicated message received\n");
            return TRUE; // Terminate the parsing of the current decoder message
        }
    }

    gstTextMsg.stMailAnncmnt.bFlushBuffer = (BOOL)((pu8Data[0] >> 4) & 0x1);
    NGLOG_DEBUG("[Mail/Announcement %d] Flush Buffer = %d\n", gstTextMsg.u8TextMsgType,
                                                                     gstTextMsg.stMailAnncmnt.bFlushBuffer);

    gstTextMsg.stMailAnncmnt.bCompressed = (BOOL)((pu8Data[0] >> 3) & 0x1);
    NGLOG_DEBUG("[Mail/Announcement %d] Compressed = %d\n", gstTextMsg.u8TextMsgType,
                                                                   gstTextMsg.stMailAnncmnt.bCompressed);

    if (gstTextMsg.stMailAnncmnt.bCompressed == TRUE)
    {
         // Decompress
         // FIXME: which APIs to decompress
    }

    gstTextMsg.stMailAnncmnt.bClubMsg = (BOOL)((pu8Data[0] >> 2) & 0x1);
    NGLOG_DEBUG("[Mail/Announcement %d] Club message = %d\n", gstTextMsg.u8TextMsgType,
                                                                     gstTextMsg.stMailAnncmnt.bClubMsg);

    gstTextMsg.stMailAnncmnt.u8Priority = (UINT8)(pu8Data[0] & 0x3);
    NGLOG_DEBUG("[Mail/Announcement %d] Message priority = %d\n", gstTextMsg.u8TextMsgType,
                                                                         gstTextMsg.stMailAnncmnt.u8Priority);

    if (gstTextMsg.stMailAnncmnt.stMsg.u8Class == E_CCA_TEXT_MSG_CLASS_TIMED)
    {
        pstDate = &gstTextMsg.stMailAnncmnt.stMsg.stDelTime;
        // Get the time to delete
        u16Year = (UINT16)((pu8Data[1] >> 1) & 0x7f);

        if (u16Year > 99)
        {
            NGLOG_DEBUG("[Mail/Announcement %d] Wrong date : year = %d\n", u16Year);
            return FALSE;
        }
        else if (u16Year >= 95)
        {
            pstDate->u16Year =  u16Year + 1990; // Year 1995 to 1999
        }
        else
        {
            pstDate->u16Year =  u16Year + 2000; // Year 2000 to 2094
        }

        pstDate->u8Month = (UINT8)(((pu8Data[1] & 0x1) << 3) | ((pu8Data[2] >> 5) & 0x07));
        pstDate->u8Day   = (UINT8)(pu8Data[2] & 0x1f);
        pstDate->u8Hour  = (UINT8)((pu8Data[3] >> 3) & 0x1f);
        pstDate->u8Min   = (UINT8)(pu8Data[3] & 0x07) * 10;
        pstDate->u8Sec   = 0;  // For Text Message, u8Sec = 0 always.
    }

    if (gstTextMsg.stMailAnncmnt.bClubMsg == TRUE)
    {
        gstTextMsg.stMailAnncmnt.u16ClubNum = (((UINT16)pu8Data[4]) << 8) | ((UINT16)pu8Data[5]);


        NGLOG_DEBUG("[Mail/Announcement %d] Club number = 0x%x\n", gstTextMsg.u8TextMsgType,
                                                                          gstTextMsg.stMailAnncmnt.u16ClubNum);
        // Club number = 0xFFFF?
        if (gstTextMsg.stMailAnncmnt.u16ClubNum != IRD_NO_CLUB_NUM_ASSIGNED)
        {
            bIsClubMember = MApp_DecoderMsg_TextMsg_CheckClubNum(gstTextMsg.stMailAnncmnt.u16ClubNum);

            if (bIsClubMember != TRUE)
            {
                NGLOG_DEBUG("[Mail/Announcement %d] The subscriber is not a club memberr = 0x%x\n", gstTextMsg.u8TextMsgType,
                                                                                                           gstTextMsg.stMailAnncmnt.u16ClubNum);
                return FALSE;
            }
        }
        else
        {
            NGLOG_DEBUG("[Mail/Announcement %d] Always receive a message as Club number = 0xFFFF\n", gstTextMsg.u8TextMsgType);
        }

        // Set the offset from the payload beginning for the field "MessageLength"
        u8MsgLengOffset = 6;

    }
    else if (gstTextMsg.stMailAnncmnt.stMsg.u8Class == E_CCA_TEXT_MSG_CLASS_TIMED)
    {
        // Set the offset from the payload beginning for the field "MessageLength"
        u8MsgLengOffset = 4;
    }
    else
    {
        u8MsgLengOffset = 1;
    }

    gstTextMsg.stMailAnncmnt.stMsg.u16Length = (UINT8)pu8Data[u8MsgLengOffset];
    if (gstTextMsg.stMailAnncmnt.stMsg.u16Length != (u16Length - (u8MsgLengOffset + 1)))
    {
        NGLOG_DEBUG("[Mail/Announcement %d] Wrong message length = %d\n", gstTextMsg.u8TextMsgType,
                                                                                 gstTextMsg.stMailAnncmnt.stMsg.u16Length);
        NGLOG_DEBUG("[Mail/Announcement %d] Length = %d Offset = %d\n", gstTextMsg.u8TextMsgType,
                                                                              u16Length, u8MsgLengOffset);
        return FALSE;
    }
    else
    {
        memcpy(&gstTextMsg.stMailAnncmnt.stMsg.au8MsgByte[0],
               &pu8Data[u8MsgLengOffset + 1],
               gstTextMsg.stMailAnncmnt.stMsg.u16Length);
        NGLOG_DEBUG("[Mail/Announcement %d] Message Length = %d\n", gstTextMsg.u8TextMsgType,
                                                                           gstTextMsg.stMailAnncmnt.stMsg.u16Length);
    }

    // Save the CRC value
    gstTextMsg.stMailAnncmnt.stMsg.u16Crc = u16Crc;

    // Start to process and store the Message data
    if (gstTextMsg.u8TextMsgType == E_CCA_TEXT_MSG_TYPE_MAILBOX)
    {
        NGLOG_DEBUG("[Mail/Announcement %d] E_CCA_TEXT_MSG_TYPE_MAILBOX\n", gstTextMsg.u8TextMsgType);
        NGLOG_DEBUG("gstTextMsg.stMailAnncmnt.stMsg.u8Class = %d\n", gstTextMsg.u8TextMsgType);

        // Store the Mails
        gMailAnncmntIndex = MApp_DecoderMsg_ProcessLongTextMsg(E_CCA_IRD_DEST_BUFFER_MAIL,
                                                               gstTextMsg.stMailAnncmnt.bFlushBuffer,
                                                               &gstTextMsg.stMailAnncmnt.stMsg);
    }
    else if (gstTextMsg.u8TextMsgType == E_CCA_TEXT_MSG_TYPE_ANNOUNCEMENT)
    {
        NGLOG_DEBUG("[Mail/Announcement %d] E_CCA_TEXT_MSG_TYPE_ANNOUNCEMENT\n", gstTextMsg.u8TextMsgType);
        NGLOG_DEBUG("gstTextMsg.stMailAnncmnt.stMsg.u8Class = %d\n", gstTextMsg.u8TextMsgType);

        // Store the announcements
        gMailAnncmntIndex = MApp_DecoderMsg_ProcessLongTextMsg(E_CCA_IRD_DEST_BUFFER_ANNCMNT,
                                                               gstTextMsg.stMailAnncmnt.bFlushBuffer,
                                                               &gstTextMsg.stMailAnncmnt.stMsg);
    }

#if 0
    if (gstTextMsg.stMailAnncmnt.u8Priority)
    {
        // Deal with messages with various priorities
        // FIXME: which APIs to deal with messages with various priorities
    }
#endif

    // Start the UI control
    //if (gstTextMsg.stMailAnncmnt.stMsg.u8Class == E_CCA_TEXT_MSG_CLASS_NORMAL)
    {
        // Display an icon clearly visible on the screen
        //MApp_DecoderMsg_TextMsg_ShowIcon(gstTextMsg.u8TextMsgType, TRUE);
    }

    if (gstTextMsg.stMailAnncmnt.stMsg.u8Class == E_CCA_TEXT_MSG_CLASS_TIMED)
    {
        // Check the time to erase the message/icon
        // FIXME:
        tCCAIRD_UI_Event e;

        memset(&e.u8CmdData[0], '\0', APP_CMD_DATA_LEN);
        NGLOG_DEBUG("[Mail/Announcement %d] Timed message class\n", gstTextMsg.u8TextMsgType);		
		e.u8CmdData[0]=E_APP_CMD_CCAIRD_TMS;
		
        if (gstTextMsg.u8TextMsgType == E_CCA_TEXT_MSG_TYPE_MAILBOX)
        {
            e.u8CmdData[1] = E_CCA_TEXT_MSG_CLASS_NORMAL;
        }
        else if (gstTextMsg.u8TextMsgType == E_CCA_TEXT_MSG_TYPE_ANNOUNCEMENT)
        {
            e.u8CmdData[1] = E_CCA_TEXT_MSG_CLASS_FORCE;
        }
        e.u8CmdData[2] = (gMailAnncmntIndex >> 8) & 0xff;   // Index (H)
        e.u8CmdData[3] = (gMailAnncmntIndex & 0xff);        // Index (L)
        e.u8CmdData[4] = 0;                                 // Duration(H)
        e.u8CmdData[5] = 0;                                 // Duration(L)
        e.u8CmdData[6] = 0;                                 // Coverage code, banner, flash
		CCAIRDEventSource::getInstance()->sendIrdCcaCmd(e);

    }
    else if (gstTextMsg.stMailAnncmnt.stMsg.u8Class == E_CCA_TEXT_MSG_CLASS_FORCE)
    {
        // Immediately display the message on the screen
        // preferable in the centre of the screen
        // FIXME:
        tCCAIRD_UI_Event e;
        memset(&e.u8CmdData[0], '\0', APP_CMD_DATA_LEN);
        NGLOG_DEBUG("[Mail/Announcement %d] Forced message class %d\n", gstTextMsg.u8TextMsgType, gMailAnncmntIndex);
        e.u8CmdData[0]   = E_APP_CMD_CCAIRD_FORCED_MSG;
        e.u8CmdData[1]   = gstTextMsg.u8TextMsgType;
        e.u8CmdData[2]   = (gMailAnncmntIndex >> 8) & 0xff;   // Index (H)
        e.u8CmdData[3]   = gMailAnncmntIndex & 0xff;          // Index (L)
        CCAIRDEventSource::getInstance()->sendIrdCcaCmd(e);
    }
    else
    {
        NGLOG_DEBUG("[Mail/Announcement] Unknown or normal Class = %d\n", gstTextMsg.stMailAnncmnt.stMsg.u8Class);
    }

    return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
///  _MApp_TextMsg_ParseClubNum
///  Parser of Club numbers
///
///  @param [in]    pu8Data     Pointer to the Message data
///  @param [in]    u16Length  Message length
///
///  @return BOOLEAN true if success, false if failed.
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////

static BOOL _MApp_TextMsg_ParseClubNum(const UINT8* pu8Data, UINT16 u16Length)
{
    int i = 0, j = 0;
    UINT16 u16ByteIndex = 0;
    BOOL bClubNumMatched = FALSE;

    gstTextMsg.stClubNum.bDelClubNum = (BOOL)(pu8Data[0] >> 7) & 0x1;
    gstTextMsg.stClubNum.u8NumClubs  = (pu8Data[0] & 0x7f);

    if (gstTextMsg.stClubNum.u8NumClubs >= IRD_MAX_CLUBS_PER_UPDATE)
    {
        NGLOG_DEBUG("[Club Numbers] Update List too long\n", gstTextMsg.stClubNum.u8NumClubs);

        // Set to maximum
        gstTextMsg.stClubNum.u8NumClubs = IRD_MAX_CLUBS_PER_UPDATE;
    }
    else if (gstTextMsg.stClubNum.u8NumClubs == 0x00)
    {
        NGLOG_DEBUG("[Club Numbers] Empty list\n");
        return FALSE;
    }

    // Update the club list
    u16ByteIndex = 1;  // From the 2nd byte

    for (i = 0; i < gstTextMsg.stClubNum.u8NumClubs; i++)
    {
        u16ByteIndex = u16ByteIndex + 2;
        gstTextMsg.stClubNum.au16ClubNum[i] = (((UINT16)pu8Data[u16ByteIndex] << 8) | (UINT16)pu8Data[u16ByteIndex + 1]);

        bClubNumMatched = FALSE;

        // Check the existing numbers in Club List
        for (j = 0; j < gu16NumGrps; j++)
        {
            if (gpstClub[gu16CurrentGrp].au16ClubNum[j] == gstTextMsg.stClubNum.au16ClubNum[i])
            {
                NGLOG_DEBUG("[Club %d] Club number 0x%x found = %d!\n", gu16CurrentGrp, gstTextMsg.stClubNum.au16ClubNum[i], j);
                bClubNumMatched = TRUE;

                if (gstTextMsg.stClubNum.bDelClubNum == TRUE)
                {
                    // Find the number and delete it
                    NGLOG_DEBUG("[Club Numbers] Delete Club Number = 0x%x!\n", gstTextMsg.stClubNum.u8NumClubs);

                    gpstClub[gu16CurrentGrp].au16ClubNum[j] = IRD_NO_CLUB_NUM_ASSIGNED;
                }
                else
                {
                    // The number exists in the list already, no need to add it
                    NGLOG_DEBUG("[Club %d] Club number 0x%x exists!\n", gu16CurrentGrp, gstTextMsg.stClubNum.au16ClubNum[i]);
                }

                break;
            }
        }

        if (bClubNumMatched == FALSE)
        {
            if (gstTextMsg.stClubNum.bDelClubNum == TRUE)
            {
                // Fail to find the number to delete
                NGLOG_DEBUG("[Club Numbers] Club number 0x%x no match!\n", gstTextMsg.stClubNum.au16ClubNum[i]);
                return FALSE;
            }
            else
            {
                // Try to find an empty position in the club List and add the new number
                for (j = 0; j < IRD_MAX_NUM_CLUBS; j++)
                {
                    if (gpstClub[gu16CurrentGrp].au16ClubNum[j] == IRD_NO_CLUB_NUM_ASSIGNED)
                    {
                        NGLOG_DEBUG("[Club Numbers] Free club slot = %d!\n", j);
                        NGLOG_DEBUG("[Club Numbers] Add club number = 0x%x!\n", gstTextMsg.stClubNum.au16ClubNum[i]);
                        gpstClub[gu16CurrentGrp].au16ClubNum[j] = gstTextMsg.stClubNum.au16ClubNum[i];
                        break;
                    }
                }

                if (j == IRD_MAX_NUM_CLUBS)
                {
                    NGLOG_DEBUG("[Club Numbers] No empty position found for 0x%x!\n", gstTextMsg.stClubNum.au16ClubNum[i]);
                    return FALSE;
                }
            }
        }
    } // End of list of numbers

    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_DecoderMsg_Parser
///  Parser of Destination Text Message in IRD message
///
///  @param [in]    pu8Data     Pointer to the Message data
///  @param [in]    u16Length  Message length
///  @param [in]    u16Crc       CRC value obtained at  CA IRD message
///
///  @return BOOLEAN true if success, false if failed.
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////

BOOL MApp_DecoderMsg_TextMsg_Parser(const UINT8 *pu8Data, UINT16 u16Length, UINT16 u16Crc)
{
    BOOL bHandled = FALSE;

    // Zero out the structure
    memset((void *)&gstTextMsg, 0x0, sizeof(stIrdMsgTextMsgPayload));

    // Get the text message type (4 bits)
    gstTextMsg.u8TextMsgType = (UINT8)((pu8Data[0] >> 4) & 0x0f);
    NGLOG_DEBUG("Text Message type = %d\n", gstTextMsg.u8TextMsgType);

    // Check the text message
    if (gstTextMsg.u8TextMsgType >= E_CCA_TEXT_MSG_TYPE_SPARE)
    {
        NGLOG_DEBUG("Spare or invalid Text Message type!\n");
        return FALSE;
    }

    if (gstTextMsg.u8TextMsgType == E_CCA_TEXT_MSG_TYPE_MAILBOX)
    {
        NGLOG_DEBUG("Text Message type = E_CCA_TEXT_MESSAGE_TYPE_MAILBOX!\n");
        bHandled = _MApp_TextMsg_ParseMailAnncmnt(&pu8Data[1], u16Length - 1, u16Crc);
    }
    else if (gstTextMsg.u8TextMsgType == E_CCA_TEXT_MSG_TYPE_ANNOUNCEMENT)
    {
        NGLOG_DEBUG("Text Message type = E_CCA_TEXT_MESSAGE_TYPE_ANNOUNCEMENT!\n");
        bHandled = _MApp_TextMsg_ParseMailAnncmnt(&pu8Data[1], u16Length - 1, u16Crc);
    }
    else if (gstTextMsg.u8TextMsgType == E_CCA_TEXT_MSG_TYPE_CLUBNUM)
    {
        NGLOG_DEBUG("Text Message type = E_CCA_TEXT_MESSAGE_TYPE_CLUBNUM (Not supported)!\n");
        bHandled = _MApp_TextMsg_ParseClubNum(&pu8Data[1], u16Length - 1);
    }

    return bHandled;
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_DecoderMsg_TextMsg_InitParser
///  Initialize the parser of Destination Text Message in IRD message
///
///  @param [in]  u16MaxNumGroups
///
///  @return BOOLEAN true if success, false if failed.
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////

BOOL MApp_DecoderMsg_TextMsg_InitParser(UINT16 u16MaxNumGroups)
{
    int i, j;

    // FIXME: how CA task sends the number of group and the group index for checking the club number?
    // Create Club list
    gu16TotalNumGrps = u16MaxNumGroups;         ///< Max group number

    if (gpstClub != 0)
    {
        IRD_FREE(gpstClub);
    }
    gpstClub = (stIrdTextMsgClubNum *)IRD_MALLOC(sizeof(stIrdTextMsgClubNum) * gu16TotalNumGrps);

    if (gpstClub == 0)
    {
        NGLOG_DEBUG("[Club list] Memory allocation failed!\n");
        return FALSE;
    }
    else
    {
        for (i = 0; i < u16MaxNumGroups; i++)
        {
            for (j = 0; j < IRD_MAX_NUM_CLUBS; j++)
            {
                gpstClub[i].au16ClubNum[j] = (UINT16)IRD_NO_CLUB_NUM_ASSIGNED;
            }
        }
    }

    gu16NumGrps = 0;         ///< Number of clubs in the list

    return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
///  global  MApp_DecoderMsg_TextMsg_TerminateParser
///  Terminate the parser of Destination Text Message in IRD message
///
///  @param [in]  void
///
///  @return
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////

void MApp_DecoderMsg_TextMsg_TerminateParser(void)
{
    if (gpstClub != 0)
    {
        IRD_FREE(gpstClub);
        gpstClub = 0;
    }
}
}
#undef _APP_CCA_DECODERMSGTEXTMSG_C

